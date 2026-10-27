//////////////////////////////////////////////////////////////////////////////
// Filename    : SlayerTunningItem.cpp
// Written By  : bigheart 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SlayerTunningItem.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "Stash.h"
#include "Utility.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

SlayerTunningItemInfoManager* g_pSlayerTunningItemInfoManager = NULL;

ItemID_t SlayerTunningItem::m_ItemIDRegistry = 0;
Mutex    SlayerTunningItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class SlayerTunningItem member methods
//////////////////////////////////////////////////////////////////////////////

SlayerTunningItem::SlayerTunningItem()
	throw()
{
	m_ItemType = 0;
}

SlayerTunningItem::SlayerTunningItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)
	throw()
{
	m_ItemType = itemType;
	m_Num      = Num;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "SlayerTunningItem::SlayerTunningItem() : Invalid item type or option type");
		throw ("SlayerTunningItem::SlayerTunningItem() : Invalid item type or optionType");
	}
}

void SlayerTunningItem::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
	throw(Error)
{
	__BEGIN_TRY

	if (itemID==0)
	{
		__ENTER_CRITICAL_SECTION(m_Mutex)

		m_ItemIDRegistry += g_pItemInfoManager->getItemIDSuccessor();
		m_ItemID = m_ItemIDRegistry;

		__LEAVE_CRITICAL_SECTION(m_Mutex)
	}
	else
	{
		m_ItemID = itemID;
	}
	
	ostringstream os;
	
	os << "INSERT INTO SlayerTunningItemObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, ItemFlag) VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< m_ItemType << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", " 
		<< storageID << ", " 
		<<(int)x << ", " 
		<<(int)y << ", " 
		<< (int)m_Num << ", " 
		<< (int)m_CreateType << ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}

void SlayerTunningItem::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE SlayerTunningItemObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", Num=" << (int)m_Num
		<< " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

string SlayerTunningItem::toString() const 
	throw()
{
	StringStream msg;

	msg << "SlayerTunningItem("
		<< "ItemID:"    << m_ItemID
		<< ",ItemType:" <<(int)m_ItemType
		<< ",Num:"      <<(int)m_Num
		<< ")";

	return msg.toString();
}

VolumeWidth_t SlayerTunningItem::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pSlayerTunningItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t SlayerTunningItem::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pSlayerTunningItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t SlayerTunningItem::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pSlayerTunningItemInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class SlayerTunningItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string SlayerTunningItemInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "SlayerTunningItemInfo("
		<< "ItemType:"     <<(int)m_ItemType
		<< ",Name:"        << m_Name
		<< ",EName:"       << m_EName
		<< ",Price:"       <<(int)m_Price
		<< ",VolumeType:"  << Volume2String[m_VolumeType]
		<< ",Weight:"      <<(int)m_Weight
		<< ",Description:" << m_Description
		<< ")";
	return msg.toString();
}

void SlayerTunningItemInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM SlayerTunningItemInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, TunningItemClass, TunningItemType, Ratio, ReqAbility FROM SlayerTunningItemInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			SlayerTunningItemInfo* pSlayerTunningItemInfo = new SlayerTunningItemInfo();

			pSlayerTunningItemInfo->setItemType(pResult->getInt(++i));
			pSlayerTunningItemInfo->setName(pResult->getString(++i));
			pSlayerTunningItemInfo->setEName(pResult->getString(++i));
			pSlayerTunningItemInfo->setPrice(pResult->getInt(++i));
			pSlayerTunningItemInfo->setVolumeType(pResult->getInt(++i));
			pSlayerTunningItemInfo->setTunningItemClass(pResult->getInt(++i));
			pSlayerTunningItemInfo->setTunningItemType(pResult->getInt(++i));
			pSlayerTunningItemInfo->setRatio(pResult->getInt(++i));
			pSlayerTunningItemInfo->setReqAbility(pResult->getString(++i));
			

			addItemInfo(pSlayerTunningItemInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class SlayerTunningItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void SlayerTunningItemLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		/*
		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM SlayerTunningItemObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/

		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM SlayerTunningItemObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );



		while (pResult->next())
		{
			try {
				uint i = 0;

				SlayerTunningItem* pSlayerTunningItem = new SlayerTunningItem();

				pSlayerTunningItem->setItemID(pResult->getDWORD(++i));
				pSlayerTunningItem->setObjectID(pResult->getDWORD(++i));
				pSlayerTunningItem->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				pSlayerTunningItem->setNum(pResult->getBYTE(++i));
				pSlayerTunningItem->setCreateType((Item::CreateType)pResult->getInt(++i));

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*    pOusters        = NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
				Item*       pItem           = NULL;
				Stash*      pStash          = NULL;
				Belt*       pBelt           = NULL;
				Inventory*  pBeltInventory  = NULL;

				if (pCreature->isSlayer())
				{
					pSlayer     = dynamic_cast<Slayer*>(pCreature);
					pInventory  = pSlayer->getInventory();
					pStash      = pSlayer->getStash();
					pMotorcycle = pSlayer->getMotorcycle();

					if (pMotorcycle) pMotorInventory = pMotorcycle->getInventory();
				}
				else if (pCreature->isVampire()) 
				{
					pVampire   = dynamic_cast<Vampire*>(pCreature);
					pInventory = pVampire->getInventory();
					pStash     = pVampire->getStash();
				}
				else if (pCreature->isOusters()) 
				{
					pOusters   = dynamic_cast<Ousters*>(pCreature);
					pInventory = pOusters->getInventory();
					pStash     = pOusters->getStash();
				}
				else throw UnsupportedError("Monster,NPC 인벤토리의 저장은 아직 지원되지 않습니다.");

				switch(storage)
				{
					case STORAGE_INVENTORY:
						if ( storageID != 0 )
						{
							SubInventory* pInventoryItem = dynamic_cast<SubInventory*>(findItemIID( pCreature, storageID ));
							if ( pInventoryItem == NULL )
							{
								processItemBugEx(pCreature, pSlayerTunningItem);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pSlayerTunningItem))
						{
							pInventory->addItemEx(x, y, pSlayerTunningItem);
						}
						else
						{
							processItemBugEx(pCreature, pSlayerTunningItem);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pSlayerTunningItem);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pSlayerTunningItem);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pSlayerTunningItem);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pSlayerTunningItem);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pSlayerTunningItem);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pSlayerTunningItem);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pSlayerTunningItem);
						}
						else pStash->insert(x, y, pSlayerTunningItem);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pSlayerTunningItem);
						break;

					default :
						SAFE_DELETE(pStmt);	// by sigi
						throw Error("invalid storage or OwnerID must be NULL");
				}

			} catch (Error& error) {
				filelog("itemLoadError.txt", "[%s] %s", getItemClassName().c_str(), error.toString().c_str());
				throw;
			} catch (Throwable& t) {
				filelog("itemLoadError.txt", "[%s] %s", getItemClassName().c_str(), t.toString().c_str());
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

void SlayerTunningItemLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM SlayerTunningItemObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			SlayerTunningItem* pSlayerTunningItem = new SlayerTunningItem();

			pSlayerTunningItem->setItemID(pResult->getInt(++i));
			pSlayerTunningItem->setObjectID(pResult->getInt(++i));
			pSlayerTunningItem->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pSlayerTunningItem->setNum(pResult->getBYTE(++i));
			pSlayerTunningItem->setCreateType((Item::CreateType)pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pSlayerTunningItem);
					}
					break;

				case STORAGE_STASH :
				case STORAGE_CORPSE :
					SAFE_DELETE(pStmt);
					throw UnsupportedError("상자 및 시체안의 아이템의 저장은 아직 지원되지 않습니다.");

				default :
					SAFE_DELETE(pStmt);
					throw Error("Storage must be STORAGE_ZONE");
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

void SlayerTunningItemLoader::load(StorageID_t storageID, Inventory* pInventory) 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
	}
	END_DB(pStmt)
	
	__END_CATCH
}

SlayerTunningItemLoader* g_pSlayerTunningItemLoader = NULL;
