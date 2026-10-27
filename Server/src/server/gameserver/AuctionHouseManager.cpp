//////////////////////////////////////////////////////////////////////////////
// AuctionHouseManager.cpp
//////////////////////////////////////////////////////////////////////////////

#include "AuctionHouseManager.h"

#include "Assert.h"
#include "DB.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "PCFinder.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "StringStream.h"

#include "Gpackets/GCAuctionList.h"
#include "Gpackets/GCAuctionResult.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCModifyInformation.h"
#include "ModifyInfo.h"

#include <sstream>

AuctionHouseManager* g_pAuctionHouseManager = new AuctionHouseManager();

AuctionHouseManager::AuctionHouseManager()
	throw()
{
}

AuctionHouseManager::~AuctionHouseManager()
	throw()
{
}

AuctionHouseManager::Config AuctionHouseManager::loadConfig() const
	throw(Error)
{
	Config config;
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = NEW_STMT;
		Result* pResult = pStmt->executeQuery(
			"SELECT FeePercent, MaxListCount, VisibleCount, ColumnsPerRow, MinPrice, MaxPrice, ListingDurationSeconds "
			"FROM AuctionConfig WHERE ConfigID=1");

		if (pResult->next())
		{
			config.FeePercent = pResult->getBYTE(1);
			config.MaxListCount = pResult->getBYTE(2);
			config.VisibleCount = pResult->getBYTE(3);
			config.ColumnsPerRow = pResult->getBYTE(4);
			config.MinPrice = pResult->getDWORD(5);
			config.MaxPrice = pResult->getDWORD(6);
			config.ListingDurationSeconds = pResult->getDWORD(7);
		}

		if (config.FeePercent > 100) config.FeePercent = 10;
		if (config.MaxListCount == 0 || config.MaxListCount > MAX_AUCTION_LIST) config.MaxListCount = MAX_AUCTION_LIST;
		if (config.VisibleCount == 0) config.VisibleCount = 6;
		if (config.ColumnsPerRow == 0) config.ColumnsPerRow = 3;
		if (config.MinPrice == 0) config.MinPrice = 1;
		if (config.MaxPrice == 0 || config.MaxPrice > MAX_MONEY) config.MaxPrice = MAX_MONEY;
		if (config.MinPrice > config.MaxPrice) config.MinPrice = 1;

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return config;
}

std::string AuctionHouseManager::escapeSQL(const std::string& value) const
	throw()
{
	std::string out;
	out.reserve(value.size() + 8);

	for (std::string::const_iterator itr = value.begin(); itr != value.end(); ++itr)
	{
		char c = *itr;
		if (c == '\\' || c == '\'')
			out += '\\';
		out += c;
	}

	return out;
}

const char* AuctionHouseManager::raceTableName(BYTE race) const
	throw()
{
	switch (race)
	{
		case RACE_SLAYER: return "Slayer";
		case RACE_VAMPIRE: return "Vampire";
		case RACE_OUSTERS: return "Ousters";
		default: return NULL;
	}
}

int AuctionHouseManager::getItemRace(Item::ItemClass itemClass) const
	throw()
{
	if (isSlayerWeapon(itemClass) || isSlayerArmor(itemClass) || isSlayerAccessory(itemClass))
		return RACE_SLAYER;
	if (isVampireWeapon(itemClass) || isVampireArmor(itemClass) || isVampireAccessory(itemClass))
		return RACE_VAMPIRE;
	if (isOustersWeapon(itemClass) || isOustersArmor(itemClass) || isOustersAccessory(itemClass))
		return RACE_OUSTERS;
	return -1;
}

int AuctionHouseManager::getItemRace(const Item* pItem) const
	throw()
{
	if (pItem == NULL)
		return -1;
	return getItemRace(pItem->getItemClass());
}

bool AuctionHouseManager::isAuctionWearable(const Item* pItem) const
	throw()
{
	return getItemRace(pItem) != -1;
}

Gold_t AuctionHouseManager::calcFee(Gold_t price, BYTE feePercent) const
	throw()
{
	if (price == 0 || feePercent == 0)
		return 0;

	unsigned long long fee = ((unsigned long long)price * feePercent + 99) / 100;
	if (fee == 0) fee = 1;
	if (fee > MAX_MONEY) fee = MAX_MONEY;
	return (Gold_t)fee;
}

void AuctionHouseManager::sendResult(PlayerCreature* pPC, BYTE resultCode, const std::string& message) const
	throw()
{
	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	GCAuctionResult packet;
	packet.setResultCode(resultCode);
	packet.setMessage(message.substr(0, 128));
	pPC->getPlayer()->sendPacket(&packet);
}

void AuctionHouseManager::sendGold(PlayerCreature* pPC) const
	throw()
{
	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	GCModifyInformation gcMI;
	gcMI.addLongData(MODIFY_GOLD, pPC->getGold());
	pPC->getPlayer()->sendPacket(&gcMI);
}

void AuctionHouseManager::fillPCItemInfo(const ListingRow& row, PCItemInfo& info) const
	throw()
{
	info.setObjectID(row.ObjectID);
	info.setItemClass((BYTE)row.ItemClass);
	info.setItemType(row.ItemType);
	info.setOptionType(row.OptionTypes);
	info.setGTwoOptionType(row.GTwoOptionTypes);
	info.setSilver(row.Silver);
	info.setDurability(row.Durability);
	info.setGrade(row.Grade);
	info.setEnchantLevel(row.EnchantLevel);
	info.setItemNum(row.ItemNum);
	info.setMainColor(row.MainColor);
#ifdef VERSION_THIRD_ENCHANT_1
	info.setThirdOptionType(row.ThirdOptionTypes);
	info.setThirdEnchantType(row.ThirdEnchantType);
#endif
}

void AuctionHouseManager::sendCreateInventoryItem(PlayerCreature* pPC, Item* pItem, CoordInven_t x, CoordInven_t y) const
	throw()
{
	if (pPC == NULL || pItem == NULL || pPC->getPlayer() == NULL)
		return;

	PCItemInfo itemInfo;
	pItem->makePCItemInfo(itemInfo);

	GCCreateItem gcCreateItem;
	gcCreateItem.setInvenX(x);
	gcCreateItem.setInvenY(y);
	gcCreateItem.setInventoryItemObjectID(0);
	gcCreateItem.SetPCItemInfo(itemInfo);
	pPC->getPlayer()->sendPacket(&gcCreateItem);
}

void AuctionHouseManager::saveItemStorage(Item* pItem, const std::string& ownerID, Storage storage, StorageID_t storageID, CoordInven_t x, CoordInven_t y) const
	throw(Error)
{
	Assert(pItem != NULL);

	std::ostringstream sql;
	sql << "UPDATE " << pItem->getObjectTableName()
		<< " SET OwnerID='" << escapeSQL(ownerID) << "'"
		<< ", Storage=" << (uint)storage
		<< ", StorageID=" << (uint)storageID
		<< ", X=" << (uint)x
		<< ", Y=" << (uint)y
		<< " WHERE ItemID=" << (uint)pItem->getItemID();

	Statement* pStmt = NULL;
	BEGIN_DB
	{
		pStmt = NEW_STMT;
		pStmt->executeQuery(sql.str());
		if (pStmt->getAffectedRowCount() == 0)
		{
			StringStream msg;
			msg << "Auction item storage update affected 0 rows: itemID=" << (uint)pItem->getItemID()
				<< " objectID=" << (uint)pItem->getObjectID();
			filelog("AuctionHouse.log", "%s", msg.toString().c_str());
			throw Error(msg.toString());
		}
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

void AuctionHouseManager::cancelActiveListing(DWORD listingID, const char* context) const
	throw()
{
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = NEW_STMT;
		pStmt->executeQuery(
			"UPDATE AuctionListing SET Status='CANCELLED', CancelledAt=NOW() "
			"WHERE ListingID=%u AND Status='ACTIVE'",
			(uint)listingID);
		if (pStmt->getAffectedRowCount() == 0)
		{
			filelog("AuctionHouse.log", "[%s] listing cancel cleanup affected 0 rows listing=%u",
				context, (uint)listingID);
		}
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

void AuctionHouseManager::restoreActiveListing(DWORD listingID, const char* context) const
	throw()
{
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = NEW_STMT;
		pStmt->executeQuery(
			"UPDATE AuctionListing "
			"SET Status='ACTIVE', BuyerPlayerID=NULL, BuyerName=NULL, SoldAt=NULL, CancelledAt=NULL "
			"WHERE ListingID=%u AND Status IN ('SOLD','CANCELLED')",
			(uint)listingID);
		if (pStmt->getAffectedRowCount() == 0)
		{
			filelog("AuctionHouse.log", "[%s] listing restore cleanup affected 0 rows listing=%u",
				context, (uint)listingID);
		}
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

bool AuctionHouseManager::loadListing(DWORD listingID, ListingRow& row) const
	throw(Error)
{
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = NEW_STMT;
		Result* pResult = pStmt->executeQuery(
			"SELECT ListingID, SellerPlayerID, SellerName, SellerRace, ItemRace, "
			"ItemClass, ItemType, ItemID, ObjectID, OptionType, GTwoOptionType, ThirdOptionType, ThirdEnchantType, "
			"Durability, Silver, Grade, EnchantLevel, ItemNum, MainColor, Price, FeePaid "
			"FROM AuctionListing WHERE ListingID=%u AND Status='ACTIVE'",
			(uint)listingID);

		if (!pResult->next())
		{
			SAFE_DELETE(pStmt);
			return false;
		}

		row.ListingID = pResult->getDWORD(1);
		row.SellerPlayerID = pResult->getString(2);
		row.SellerName = pResult->getString(3);
		row.SellerRace = pResult->getBYTE(4);
		row.ItemRace = pResult->getBYTE(5);
		row.ItemClass = (Item::ItemClass)pResult->getInt(6);
		row.ItemType = (ItemType_t)pResult->getUInt(7);
		row.ItemID = (ItemID_t)pResult->getDWORD(8);
		row.ObjectID = (ObjectID_t)pResult->getDWORD(9);
		setOptionTypeFromField(row.OptionTypes, pResult->getString(10));
		setOptionTypeFromField(row.GTwoOptionTypes, pResult->getString(11));
		setOptionTypeFromField(row.ThirdOptionTypes, pResult->getString(12));
		row.ThirdEnchantType = pResult->getBYTE(13);
		row.Durability = (Durability_t)pResult->getDWORD(14);
		row.Silver = (Silver_t)pResult->getDWORD(15);
		row.Grade = (Grade_t)pResult->getInt(16);
		row.EnchantLevel = (EnchantLevel_t)pResult->getInt(17);
		row.ItemNum = (ItemNum_t)pResult->getUInt(18);
		row.MainColor = pResult->getWORD(19);
		row.Price = pResult->getDWORD(20);
		row.FeePaid = pResult->getDWORD(21);

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return true;
}

Item* AuctionHouseManager::makeItemFromListing(const ListingRow& row) const
	throw(Error)
{
	Item* pItem = g_pItemFactoryManager->createItem(row.ItemClass, row.ItemType, row.OptionTypes);
	Assert(pItem != NULL);

	pItem->setObjectID(row.ObjectID);
	pItem->setItemID(row.ItemID);
	pItem->setGTwoOptionType(row.GTwoOptionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
	pItem->setThirdOptionType(row.ThirdOptionTypes);
	pItem->setThirdEnchantType(row.ThirdEnchantType);
#endif
	pItem->setDurability(row.Durability);
	pItem->setSilver(row.Silver);
	pItem->setGrade(row.Grade);
	pItem->setEnchantLevel(row.EnchantLevel);
	pItem->setNum(row.ItemNum);
	return pItem;
}

void AuctionHouseManager::sendList(PlayerCreature* pPC)
	throw()
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	Config config;
	try
	{
		config = loadConfig();
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[sendList] config load failed player=%s error=%s",
			pPC->getName().c_str(), t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction tables are not installed.");
		return;
	}

	GCAuctionList packet;

	Statement* pStmt = NULL;
	try
	{
		pStmt = NEW_STMT;
		Result* pResult = pStmt->executeQuery(
			"SELECT ListingID, Price, SellerRace, ItemRace, SellerName, "
			"ItemClass, ItemType, ItemID, ObjectID, OptionType, GTwoOptionType, ThirdOptionType, ThirdEnchantType, "
			"Durability, Silver, Grade, EnchantLevel, ItemNum, MainColor "
			"FROM AuctionListing "
			"WHERE Status='ACTIVE' AND ItemRace=%u AND (ExpiresAt IS NULL OR ExpiresAt > NOW()) "
			"ORDER BY CreatedAt DESC LIMIT %u",
			(uint)pPC->getRace(),
			(uint)config.MaxListCount);

		while (pResult->next())
		{
			GCAuctionList::Entry* pEntry = new GCAuctionList::Entry();
			Assert(pEntry != NULL);

			pEntry->ListingID = pResult->getDWORD(1);
			pEntry->Price = pResult->getDWORD(2);
			pEntry->SellerRace = pResult->getBYTE(3);
			pEntry->ItemRace = pResult->getBYTE(4);
			pEntry->SellerName = pResult->getString(5);

			pEntry->ItemInfo.setItemClass(pResult->getBYTE(6));
			pEntry->ItemInfo.setItemType((ItemType_t)pResult->getUInt(7));
			pEntry->ItemInfo.setObjectID((ObjectID_t)pResult->getDWORD(9));

			std::list<OptionType_t> options;
			std::list<OptionType_t> gtwoOptions;
			std::list<OptionType_t> thirdOptions;
			setOptionTypeFromField(options, pResult->getString(10));
			setOptionTypeFromField(gtwoOptions, pResult->getString(11));
			setOptionTypeFromField(thirdOptions, pResult->getString(12));

			pEntry->ItemInfo.setOptionType(options);
			pEntry->ItemInfo.setGTwoOptionType(gtwoOptions);
			pEntry->ItemInfo.setDurability((Durability_t)pResult->getDWORD(14));
			pEntry->ItemInfo.setSilver((Silver_t)pResult->getDWORD(15));
			pEntry->ItemInfo.setGrade((Grade_t)pResult->getInt(16));
			pEntry->ItemInfo.setEnchantLevel((EnchantLevel_t)pResult->getInt(17));
			pEntry->ItemInfo.setItemNum((ItemNum_t)pResult->getUInt(18));
			pEntry->ItemInfo.setMainColor(pResult->getWORD(19));
#ifdef VERSION_THIRD_ENCHANT_1
			pEntry->ItemInfo.setThirdOptionType(thirdOptions);
			pEntry->ItemInfo.setThirdEnchantType(pResult->getBYTE(13));
#endif

			packet.addEntry(pEntry);
		}

		SAFE_DELETE(pStmt);
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pStmt);
		filelog("AuctionHouse.log", "[sendList] list query failed player=%s error=%s",
			pPC->getName().c_str(), t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction list could not be loaded.");
		return;
	}

	pPC->getPlayer()->sendPacket(&packet);

	__END_CATCH
}

void AuctionHouseManager::listItem(PlayerCreature* pPC, ObjectID_t objectID, Gold_t price)
	throw()
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	Config config;
	try
	{
		config = loadConfig();
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[listItem] config load failed player=%s objectID=%u error=%s",
			pPC->getName().c_str(), (uint)objectID, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction tables are not installed.");
		return;
	}

	if (price < config.MinPrice || price > config.MaxPrice)
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "Invalid auction price.");
		return;
	}

	Inventory* pInventory = pPC->getInventory();
	CoordInven_t x = 0;
	CoordInven_t y = 0;
	Item* pItem = (pInventory != NULL) ? pInventory->findItemOID(objectID, x, y) : NULL;
	if (pItem == NULL)
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "Item is not in inventory.");
		return;
	}

	if (!isAuctionWearable(pItem) || getItemRace(pItem) != pPC->getRace())
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "Only your race wearable items can be listed.");
		return;
	}

	if (!canTrade(pItem) || !canTradeInventoryItem(pItem))
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "This item cannot be traded.");
		return;
	}

	Gold_t fee = calcFee(price, config.FeePercent);
	if (pPC->getGold() < fee)
	{
		sendGold(pPC);
		sendResult(pPC, AUCTION_RESULT_ERROR, "Not enough gold for auction fee.");
		return;
	}

	std::string optionType;
	std::string gtwoOptionType;
	std::string thirdOptionType;
	setOptionTypeToField(pItem->getOptionTypeList(), optionType);
	setOptionTypeToField(pItem->getGTwoOptionTypeList(), gtwoOptionType);
	setOptionTypeToField(pItem->getThirdOptionTypeList(), thirdOptionType);

	Statement* pStmt = NULL;
	DWORD listingID = 0;

	try
	{
		pStmt = NEW_STMT;

		Result* pDupResult = pStmt->executeQuery(
			"SELECT ListingID FROM AuctionListing WHERE ObjectID=%u AND Status='ACTIVE'",
			(uint)pItem->getObjectID());
		if (pDupResult->next())
		{
			SAFE_DELETE(pStmt);
			sendResult(pPC, AUCTION_RESULT_ERROR, "This item is already listed.");
			return;
		}

		std::ostringstream sql;
		sql << "INSERT INTO AuctionListing "
			<< "(WorldID, SellerPlayerID, SellerName, SellerRace, ItemRace, ItemClass, ItemType, ItemID, ObjectID, "
			<< "OptionType, GTwoOptionType, ThirdOptionType, ThirdEnchantType, Durability, Silver, Grade, EnchantLevel, ItemNum, MainColor, "
			<< "Price, FeePaid, Status, CreatedAt, ExpiresAt) VALUES ("
			<< "1, '"
			<< escapeSQL(pPC->getPlayer()->getID()) << "', '"
			<< escapeSQL(pPC->getName()) << "', "
			<< (uint)pPC->getRace() << ", "
			<< (uint)getItemRace(pItem) << ", "
			<< (uint)pItem->getItemClass() << ", "
			<< (uint)pItem->getItemType() << ", "
			<< (uint)pItem->getItemID() << ", "
			<< (uint)pItem->getObjectID() << ", '"
			<< escapeSQL(optionType) << "', '"
			<< escapeSQL(gtwoOptionType) << "', '"
			<< escapeSQL(thirdOptionType) << "', "
			<< (uint)pItem->getThirdEnchantType() << ", "
			<< (uint)pItem->getDurability() << ", "
			<< (uint)pItem->getSilver() << ", "
			<< (int)pItem->getGrade() << ", "
			<< (int)pItem->getEnchantLevel() << ", "
			<< (uint)pItem->getNum() << ", "
			<< 0 << ", "
			<< (uint)price << ", "
			<< (uint)fee << ", 'ACTIVE', NOW(), ";
		if (config.ListingDurationSeconds > 0)
			sql << "DATE_ADD(NOW(), INTERVAL " << (uint)config.ListingDurationSeconds << " SECOND))";
		else
			sql << "NULL)";

		pStmt->executeQuery(sql.str());
		listingID = pStmt->getInsertID();
		SAFE_DELETE(pStmt);
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pStmt);
		filelog("AuctionHouse.log", "[listItem] insert failed player=%s objectID=%u item=%u/%u error=%s",
			pPC->getName().c_str(), (uint)objectID, (uint)pItem->getItemClass(), (uint)pItem->getItemType(), t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing could not be created.");
		return;
	}

	if (listingID == 0)
	{
		filelog("AuctionHouse.log", "[listItem] insert id is zero player=%s objectID=%u", pPC->getName().c_str(), (uint)objectID);
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing could not be created.");
		return;
	}

	try
	{
		saveItemStorage(pItem, pPC->getName(), STORAGE_AUCTION, (StorageID_t)listingID, 0, 0);
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[listItem] item storage failed player=%s listing=%u objectID=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, (uint)objectID, t.toString().c_str());
		cancelActiveListing(listingID, "listItem-storage");
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction item storage could not be updated.");
		return;
	}

	pPC->decreaseGoldEx(fee);
	sendGold(pPC);

	pInventory->deleteItem(x, y);
	GCDeleteInventoryItem gcDelete(pItem->getObjectID());
	pPC->getPlayer()->sendPacket(&gcDelete);
	SAFE_DELETE(pItem);

	sendResult(pPC, AUCTION_RESULT_LISTED, "Item listed in Auction House.");
	sendList(pPC);

	__END_CATCH
}

void AuctionHouseManager::paySeller(const ListingRow& row) const
	throw(Error)
{
	Creature* pCreature = (g_pPCFinder != NULL) ? g_pPCFinder->getCreature(row.SellerName) : NULL;
	PlayerCreature* pSellerPC = dynamic_cast<PlayerCreature*>(pCreature);
	if (pSellerPC != NULL)
	{
		pSellerPC->increaseGoldEx(row.Price);
		sendGold(pSellerPC);
		return;
	}

	const char* tableName = raceTableName(row.SellerRace);
	if (tableName == NULL)
	{
		filelog("AuctionHouse.log", "[paySeller] invalid seller race listing=%u seller=%s race=%u",
			(uint)row.ListingID, row.SellerName.c_str(), (uint)row.SellerRace);
		throw Error("invalid auction seller race");
	}

	Statement* pStmt = NULL;
	BEGIN_DB
	{
		pStmt = NEW_STMT;
		std::ostringstream sql;
		sql << "UPDATE " << tableName
			<< " SET Gold=LEAST(Gold+" << (uint)row.Price << ", " << (uint)MAX_MONEY << ")"
			<< " WHERE Name='" << escapeSQL(row.SellerName) << "'";
		pStmt->executeQuery(sql.str());
		if (pStmt->getAffectedRowCount() == 0)
		{
			StringStream msg;
			msg << "Auction seller payout affected 0 rows: listing=" << (uint)row.ListingID
				<< " seller=" << row.SellerName;
			filelog("AuctionHouse.log", "%s", msg.toString().c_str());
			throw Error(msg.toString());
		}

		std::ostringstream msg;
		msg << "Your Auction House item was sold for " << (uint)row.Price << " gold.";
		pStmt->executeQuery(
			"INSERT INTO Messages (Receiver, Message) VALUES ('%s', '%s')",
			escapeSQL(row.SellerName).c_str(),
			escapeSQL(msg.str()).c_str());

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

void AuctionHouseManager::buyItem(PlayerCreature* pPC, DWORD listingID)
	throw()
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	ListingRow row;
	try
	{
		if (!loadListing(listingID, row))
		{
			sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing is no longer available.");
			return;
		}
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[buyItem] load failed buyer=%s listing=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing could not be loaded.");
		return;
	}

	if (row.ItemRace != pPC->getRace())
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "This item is not available for your race.");
		return;
	}

	if (row.SellerName == pPC->getName())
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "You cannot buy your own auction item.");
		return;
	}

	if (pPC->getGold() < row.Price)
	{
		sendGold(pPC);
		sendResult(pPC, AUCTION_RESULT_ERROR, "Not enough gold.");
		return;
	}

	Item* pItem = NULL;
	try
	{
		pItem = makeItemFromListing(row);
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[buyItem] item recreate failed buyer=%s listing=%u item=%u/%u error=%s",
			pPC->getName().c_str(), (uint)listingID, (uint)row.ItemClass, (uint)row.ItemType, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction item could not be restored.");
		return;
	}

	Inventory* pInventory = pPC->getInventory();
	_TPOINT emptyPos;
	if (pInventory == NULL || !pInventory->getEmptySlot(pItem, emptyPos))
	{
		SAFE_DELETE(pItem);
		sendResult(pPC, AUCTION_RESULT_ERROR, "Not enough inventory space.");
		return;
	}

	Statement* pStmt = NULL;
	try
	{
		pStmt = NEW_STMT;
		pStmt->executeQuery(
			"UPDATE AuctionListing SET Status='SOLD', BuyerPlayerID='%s', BuyerName='%s', SoldAt=NOW() "
			"WHERE ListingID=%u AND Status='ACTIVE'",
			escapeSQL(pPC->getPlayer()->getID()).c_str(),
			escapeSQL(pPC->getName()).c_str(),
			(uint)listingID);
		if (pStmt->getAffectedRowCount() == 0)
		{
			SAFE_DELETE(pStmt);
			SAFE_DELETE(pItem);
			sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing is no longer available.");
			return;
		}
		SAFE_DELETE(pStmt);
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pStmt);
		SAFE_DELETE(pItem);
		filelog("AuctionHouse.log", "[buyItem] sold mark failed buyer=%s listing=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction purchase could not be completed.");
		return;
	}

	try
	{
		saveItemStorage(pItem, pPC->getName(), STORAGE_INVENTORY, 0, emptyPos.x, emptyPos.y);
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pItem);
		filelog("AuctionHouse.log", "[buyItem] item storage failed buyer=%s listing=%u objectID=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, (uint)row.ObjectID, t.toString().c_str());
		restoreActiveListing(listingID, "buyItem-storage");
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction item storage could not be updated.");
		return;
	}

	pPC->decreaseGoldEx(row.Price);
	sendGold(pPC);

	Assert(pInventory->addItem((CoordInven_t)emptyPos.x, (CoordInven_t)emptyPos.y, pItem));
	sendCreateInventoryItem(pPC, pItem, (CoordInven_t)emptyPos.x, (CoordInven_t)emptyPos.y);

	try
	{
		paySeller(row);
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[buyItem] seller payout failed buyer=%s listing=%u seller=%s error=%s",
			pPC->getName().c_str(), (uint)listingID, row.SellerName.c_str(), t.toString().c_str());
	}

	sendResult(pPC, AUCTION_RESULT_BOUGHT, "Auction item purchased.");
	sendList(pPC);

	__END_CATCH
}

void AuctionHouseManager::cancelListing(PlayerCreature* pPC, DWORD listingID)
	throw()
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	ListingRow row;
	try
	{
		if (!loadListing(listingID, row))
		{
			sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing is no longer available.");
			return;
		}
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[cancelListing] load failed player=%s listing=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing could not be loaded.");
		return;
	}

	if (row.SellerName != pPC->getName())
	{
		sendResult(pPC, AUCTION_RESULT_ERROR, "Only the seller can cancel this listing.");
		return;
	}

	Item* pItem = NULL;
	try
	{
		pItem = makeItemFromListing(row);
	}
	catch (Throwable& t)
	{
		filelog("AuctionHouse.log", "[cancelListing] item recreate failed player=%s listing=%u item=%u/%u error=%s",
			pPC->getName().c_str(), (uint)listingID, (uint)row.ItemClass, (uint)row.ItemType, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction item could not be restored.");
		return;
	}

	Inventory* pInventory = pPC->getInventory();
	_TPOINT emptyPos;
	if (pInventory == NULL || !pInventory->getEmptySlot(pItem, emptyPos))
	{
		SAFE_DELETE(pItem);
		sendResult(pPC, AUCTION_RESULT_ERROR, "Not enough inventory space.");
		return;
	}

	Statement* pStmt = NULL;
	try
	{
		pStmt = NEW_STMT;
		pStmt->executeQuery(
			"UPDATE AuctionListing SET Status='CANCELLED', CancelledAt=NOW() WHERE ListingID=%u AND Status='ACTIVE'",
			(uint)listingID);
		if (pStmt->getAffectedRowCount() == 0)
		{
			SAFE_DELETE(pStmt);
			SAFE_DELETE(pItem);
			sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing is no longer available.");
			return;
		}
		SAFE_DELETE(pStmt);
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pStmt);
		SAFE_DELETE(pItem);
		filelog("AuctionHouse.log", "[cancelListing] cancel mark failed player=%s listing=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, t.toString().c_str());
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction listing could not be cancelled.");
		return;
	}

	try
	{
		saveItemStorage(pItem, pPC->getName(), STORAGE_INVENTORY, 0, emptyPos.x, emptyPos.y);
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pItem);
		filelog("AuctionHouse.log", "[cancelListing] item storage failed player=%s listing=%u objectID=%u error=%s",
			pPC->getName().c_str(), (uint)listingID, (uint)row.ObjectID, t.toString().c_str());
		restoreActiveListing(listingID, "cancelListing-storage");
		sendResult(pPC, AUCTION_RESULT_ERROR, "Auction item storage could not be updated.");
		return;
	}

	Assert(pInventory->addItem((CoordInven_t)emptyPos.x, (CoordInven_t)emptyPos.y, pItem));
	sendCreateInventoryItem(pPC, pItem, (CoordInven_t)emptyPos.x, (CoordInven_t)emptyPos.y);

	sendResult(pPC, AUCTION_RESULT_CANCELLED, "Auction listing cancelled.");
	sendList(pPC);

	__END_CATCH
}
