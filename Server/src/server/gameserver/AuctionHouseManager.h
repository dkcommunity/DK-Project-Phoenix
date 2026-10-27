//////////////////////////////////////////////////////////////////////////////
// AuctionHouseManager.h
//////////////////////////////////////////////////////////////////////////////

#ifndef __AUCTION_HOUSE_MANAGER_H__
#define __AUCTION_HOUSE_MANAGER_H__

#include "Types.h"
#include "Exception.h"
#include "Item.h"
#include <list>
#include <string>

class PlayerCreature;
class Item;
class PCItemInfo;

class AuctionHouseManager
{
public:
	AuctionHouseManager() throw();
	~AuctionHouseManager() throw();

	void sendList(PlayerCreature* pPC) throw();
	void listItem(PlayerCreature* pPC, ObjectID_t objectID, Gold_t price) throw();
	void buyItem(PlayerCreature* pPC, DWORD listingID) throw();
	void cancelListing(PlayerCreature* pPC, DWORD listingID) throw();

private:
	struct Config
	{
		BYTE FeePercent;
		BYTE MaxListCount;
		BYTE VisibleCount;
		BYTE ColumnsPerRow;
		Gold_t MinPrice;
		Gold_t MaxPrice;
		DWORD ListingDurationSeconds;

		Config()
			: FeePercent(10)
			, MaxListCount(20)
			, VisibleCount(6)
			, ColumnsPerRow(3)
			, MinPrice(1)
			, MaxPrice(2000000000)
			, ListingDurationSeconds(604800)
		{
		}
	};

	struct ListingRow
	{
		DWORD ListingID;
		std::string SellerPlayerID;
		std::string SellerName;
		BYTE SellerRace;
		BYTE ItemRace;
		Item::ItemClass ItemClass;
		ItemType_t ItemType;
		ItemID_t ItemID;
		ObjectID_t ObjectID;
		std::list<OptionType_t> OptionTypes;
		std::list<OptionType_t> GTwoOptionTypes;
		std::list<OptionType_t> ThirdOptionTypes;
		BYTE ThirdEnchantType;
		Durability_t Durability;
		Silver_t Silver;
		Grade_t Grade;
		EnchantLevel_t EnchantLevel;
		ItemNum_t ItemNum;
		WORD MainColor;
		Gold_t Price;
		Gold_t FeePaid;

		ListingRow()
			: ListingID(0)
			, SellerRace(0)
			, ItemRace(0)
			, ItemClass(Item::ITEM_CLASS_MAX)
			, ItemType(0)
			, ItemID(0)
			, ObjectID(0)
			, ThirdEnchantType(99)
			, Durability(0)
			, Silver(0)
			, Grade(0)
			, EnchantLevel(0)
			, ItemNum(0)
			, MainColor(0)
			, Price(0)
			, FeePaid(0)
		{
		}
	};

	Config loadConfig() const throw(Error);
	bool loadListing(DWORD listingID, ListingRow& row) const throw(Error);
	int getItemRace(const Item* pItem) const throw();
	int getItemRace(Item::ItemClass itemClass) const throw();
	bool isAuctionWearable(const Item* pItem) const throw();
	Gold_t calcFee(Gold_t price, BYTE feePercent) const throw();
	Item* makeItemFromListing(const ListingRow& row) const throw(Error);
	void fillPCItemInfo(const ListingRow& row, PCItemInfo& info) const throw();
	void sendResult(PlayerCreature* pPC, BYTE resultCode, const std::string& message) const throw();
	void sendGold(PlayerCreature* pPC) const throw();
	void sendCreateInventoryItem(PlayerCreature* pPC, Item* pItem, CoordInven_t x, CoordInven_t y) const throw();
	void saveItemStorage(Item* pItem, const std::string& ownerID, Storage storage, StorageID_t storageID, CoordInven_t x, CoordInven_t y) const throw(Error);
	void cancelActiveListing(DWORD listingID, const char* context) const throw();
	void restoreActiveListing(DWORD listingID, const char* context) const throw();
	void paySeller(const ListingRow& row) const throw(Error);
	const char* raceTableName(BYTE race) const throw();
	std::string escapeSQL(const std::string& value) const throw();
};

extern AuctionHouseManager* g_pAuctionHouseManager;

#endif
