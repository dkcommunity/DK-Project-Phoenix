//////////////////////////////////////////////////////////////////////////////
// Filename    : CommonQuestItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CommonQuestItem.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Creature.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "Stash.h"
#include "Utility.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

CommonQuestItemInfoManager* g_pCommonQuestItemInfoManager = NULL;

ItemID_t CommonQuestItem::m_ItemIDRegistry = 0;
Mutex    CommonQuestItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItem member methods
//////////////////////////////////////////////////////////////////////////////

CommonQuestItem::CommonQuestItem()
	throw()
{
	m_ItemType = 0;
}

CommonQuestItem::CommonQuestItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)
	throw()
{
	m_ItemType = itemType;
	setNum(Num);

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "CommonQuestItem::CommonQuestItem() : Invalid item type or option type");
		throw ("CommonQuestItem::CommonQuestItem() : Invalid item type or optionType");
	}
}

void CommonQuestItem::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO CommonQuestItemObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, ItemFlag) VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< m_ItemType << ", '" 
		<< ownerID << "', " 
		<< (int)storage << ", "
		<< storageID << ", " 
		<< (int)x << ", " 
		<< (int)y << ", " 
		<< (int)getNum() << ", " 
		<< (int)m_CreateType << ")";

	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}

void CommonQuestItem::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE CommonQuestItemObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", Num=" << (int)getNum()
		<< " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str()); 
	
	__END_CATCH
}

string CommonQuestItem::toString() const 
	throw()
{
	StringStream msg;

	msg << "CommonQuestItem("
		<< "ItemID:"    << m_ItemID
		<< ",ItemType:" <<(int)m_ItemType
		<< ")";

	return msg.toString();
}

VolumeWidth_t CommonQuestItem::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCommonQuestItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t CommonQuestItem::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCommonQuestItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
		
Weight_t CommonQuestItem::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCommonQuestItemInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string CommonQuestItemInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "CommonQuestItemInfo("
		<< "ItemType:"     <<(int)m_ItemType
		<< ",Name:"        << m_Name
		<< ",EName:"       << m_EName
		<< ",Price:"       <<(int)m_Price
		<< ",VolumeType:"  << Volume2String[m_VolumeType]
		<< ",Weight:"      <<(int)m_Weight
		<< ",BonusRatio:"      <<(int)m_BonusRatio
		<< ",Description:" << m_Description
		<< ")";
	return msg.toString();
}

void CommonQuestItemInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM CommonQuestItemInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, BonusRatio FROM CommonQuestItemInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			CommonQuestItemInfo* pCommonQuestItemInfo = new CommonQuestItemInfo();

			pCommonQuestItemInfo->setItemType(pResult->getInt(++i));
			pCommonQuestItemInfo->setName(pResult->getString(++i));
			pCommonQuestItemInfo->setEName(pResult->getString(++i));
			pCommonQuestItemInfo->setPrice(pResult->getInt(++i));
			pCommonQuestItemInfo->setVolumeType(pResult->getInt(++i));
			pCommonQuestItemInfo->setWeight(pResult->getInt(++i));
			pCommonQuestItemInfo->setRatio(pResult->getInt(++i));
			pCommonQuestItemInfo->setBonusRatio(pResult->getInt(++i));

			addItemInfo(pCommonQuestItemInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void CommonQuestItemLoader::load(Creature* pCreature) 
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

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM CommonQuestItemObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/

		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM CommonQuestItemObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );



		while (pResult->next())
		{
			try {
				uint i = 0;

				CommonQuestItem* pCommonQuestItem = new CommonQuestItem();

				pCommonQuestItem->setItemID(pResult->getDWORD(++i));
				pCommonQuestItem->setObjectID(pResult->getDWORD(++i));
				pCommonQuestItem->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				pCommonQuestItem->setNum(pResult->getBYTE(++i));

				pCommonQuestItem->setCreateType((Item::CreateType)pResult->getInt(++i));

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
								processItemBugEx(pCreature, pCommonQuestItem);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pCommonQuestItem))
						{
							pInventory->addItemEx(x, y, pCommonQuestItem);
						}
						else
						{
							processItemBugEx(pCreature, pCommonQuestItem);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pCommonQuestItem);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pCommonQuestItem);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pCommonQuestItem);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pCommonQuestItem);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pCommonQuestItem);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pCommonQuestItem);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pCommonQuestItem);
						}
						else pStash->insert(x, y, pCommonQuestItem);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pCommonQuestItem);
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

void CommonQuestItemLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM CommonQuestItemObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			CommonQuestItem* pCommonQuestItem = new CommonQuestItem();

			pCommonQuestItem->setItemID(pResult->getInt(++i));
			pCommonQuestItem->setObjectID(pResult->getInt(++i));
			pCommonQuestItem->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pCommonQuestItem->setNum(pResult->getBYTE(++i));
			pCommonQuestItem->setCreateType((Item::CreateType)pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pCommonQuestItem);
					}
					break;

				case STORAGE_STASH :
				case STORAGE_CORPSE :
					throw UnsupportedError("상자 및 시체안의 아이템의 저장은 아직 지원되지 않습니다.");

				default :
					throw Error("Storage must be STORAGE_ZONE");
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

void CommonQuestItemLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

CommonQuestItemLoader* g_pCommonQuestItemLoader = NULL;
