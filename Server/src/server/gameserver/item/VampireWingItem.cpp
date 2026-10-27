//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireWingItem.cpp
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireWingItem.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "Stash.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

// global variable declaration
VampireWingItemInfoManager* g_pVampireWingItemInfoManager = NULL;

ItemID_t VampireWingItem::m_ItemIDRegistry = 0;
Mutex    VampireWingItem::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireWingItem::VampireWingItem()
	throw()
{
	m_ItemType = 0;
}

VampireWingItem::VampireWingItem(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
{
	m_ItemType = itemType;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "VampireWingItem::VampireWingItem() : Invalid item type or option type");
		throw ("VampireWingItem::VampireWingItem() : Invalid item type or optionType");
	}

	VampireWingItemInfo *pVampireWingItemInfo = dynamic_cast<VampireWingItemInfo *>(g_pVampireWingItemInfoManager->getItemInfo(m_ItemType));
	
	m_BodyColor = pVampireWingItemInfo->getBodyColor();
	m_EffectColor = pVampireWingItemInfo->getEffectColor();
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireWingItem::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO " << getObjectTableName()
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, BodyColor, EffectColor) VALUES (" 
		<< m_ItemID << "," 
		<< m_ObjectID << "," 
		<< m_ItemType << ",'" 
		<< ownerID << "'," 
		<< (int)storage << ", " 
		<< storageID << ", " 
		<< (int)x << "," 
		<< (int)y << "," 
		<< (int)m_BodyColor << "," 
		<< (int)m_EffectColor << ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void VampireWingItem::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE " << getObjectTableName().c_str() << " SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", BodyColor=" << m_BodyColor
		<< ", EffectColor=" << m_EffectColor
		<< " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireWingItem::toString() const 
	throw()
{
	StringStream msg;
	msg << "VampireWingItem("
		<< "ItemID:"	<< m_ItemID
		<< ",ItemType:"	<< m_ItemType
		<< ")";
	return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireWingItem::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireWingItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireWingItem::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireWingItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireWingItem::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireWingItemInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireWingItemInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "VampireWingItemInfo("
		<< "ItemType:" << m_ItemType
		<< ",Name:" << m_Name
		<< ",EName:" << m_EName
		<< ",Price:" << m_Price
		<< ",VolumeType:" << Volume2String[m_VolumeType]
		<< ",Weight:" << m_Weight
		<< ",Description:" << m_Description
		<< ")";

	return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireWingItemInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM VampireWingItemInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, DefaultBodyColor, DefaultEffectColor, ReqAbility" 
			" FROM VampireWingItemInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			VampireWingItemInfo* pVampireWingItemInfo = new VampireWingItemInfo();

			pVampireWingItemInfo->setItemType(pResult->getInt(++i));
			pVampireWingItemInfo->setName(pResult->getString(++i));
			pVampireWingItemInfo->setEName(pResult->getString(++i));
			pVampireWingItemInfo->setPrice(pResult->getInt(++i));
			pVampireWingItemInfo->setVolumeType(pResult->getInt(++i));
			pVampireWingItemInfo->setWeight(pResult->getInt(++i));
			pVampireWingItemInfo->setRatio(pResult->getInt(++i));
			pVampireWingItemInfo->setBodyColor(pResult->getInt(++i));
			pVampireWingItemInfo->setEffectColor(pResult->getInt(++i));
			pVampireWingItemInfo->setReqAbility(pResult->getString(++i));

			addItemInfo(pVampireWingItemInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireWingItemLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();


		Result* pResult = pStmt->executeQuery( 
				"SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, BodyColor, EffectColor"
				" FROM VampireWingItemObject "
				" WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
				pCreature->getName().c_str() );

		while (pResult->next())
		{
			try {
				uint i = 0;

				VampireWingItem* pVampireWingItem = new VampireWingItem();

				pVampireWingItem->setItemID(pResult->getDWORD(++i));
				pVampireWingItem->setObjectID(pResult->getDWORD(++i));
				pVampireWingItem->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				pVampireWingItem->setBodyColor(pResult->getInt(++i));
				pVampireWingItem->setEffectColor(pResult->getInt(++i));

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
				//Item*       pItem           = NULL;
				Stash*      pStash          = NULL;
				//Belt*       pBelt           = NULL;
				//Inventory*  pBeltInventory  = NULL;

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
				else throw UnsupportedError("Monster,NPC 인벤토리의 저장은 아직 지원되지 않습니다.");

				switch(storage)
				{
					case STORAGE_INVENTORY:
						if ( storageID != 0 )
						{
							SubInventory* pInventoryItem = dynamic_cast<SubInventory*>(findItemIID( pCreature, storageID ));
							if ( pInventoryItem == NULL )
							{
								processItemBugEx(pCreature, pVampireWingItem);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pVampireWingItem))
						{
							pInventory->addItemEx(x, y, pVampireWingItem);
						}
						else
						{
							processItemBugEx(pCreature, pVampireWingItem);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pVampireWingItem);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pVampireWingItem);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pVampireWingItem);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pVampireWingItem);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pVampireWingItem);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pVampireWingItem);
						}
						else pStash->insert(x, y, pVampireWingItem);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pVampireWingItem);
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


//--------------------------------------------------------------------------------
// load to zone
//--------------------------------------------------------------------------------
void VampireWingItemLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, BodyColor, EffectColor "
			<< "FROM VampireWingItemObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			VampireWingItem* pVampireWingItem = new VampireWingItem();

			pVampireWingItem->setItemID(pResult->getInt(++i));
			pVampireWingItem->setObjectID(pResult->getInt(++i));
			pVampireWingItem->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pVampireWingItem->setBodyColor(pResult->getInt(++i));
			pVampireWingItem->setEffectColor(pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pVampireWingItem);
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


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void VampireWingItemLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

VampireWingItemLoader* g_pVampireWingItemLoader = NULL;
