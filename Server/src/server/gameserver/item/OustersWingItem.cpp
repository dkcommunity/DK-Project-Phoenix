//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersWingItem.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "OustersWingItem.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "Stash.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

ItemID_t OustersWingItem::m_ItemIDRegistry = 0;
Mutex    OustersWingItem::m_Mutex;

OustersWingItemInfoManager* g_pOustersWingItemInfoManager = NULL;
OustersWingItemLoader*      g_pOustersWingItemLoader = NULL;

//////////////////////////////////////////////////////////////////////////////
// class OustersWingItem member methods
//////////////////////////////////////////////////////////////////////////////

OustersWingItem::OustersWingItem()
	throw()
{
	m_ItemType = 0;
}

OustersWingItem::OustersWingItem(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
{
	m_ItemType = itemType;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "OustersWingItem::OustersWingItem() : Invalid item type or option type");
		throw ("OustersWingItem::OustersWingItem() : Invalid item type or optionType");
	}

	OustersWingItemInfo *pOustersWingItemInfo = dynamic_cast<OustersWingItemInfo *>(g_pOustersWingItemInfoManager->getItemInfo(m_ItemType));

	m_BodyColor = pOustersWingItemInfo->getBodyColor();
	m_EffectColor = pOustersWingItemInfo->getEffectColor();
	
}

void OustersWingItem::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO OustersWingItemObject "
		<< "(ItemID,ObjectID,ItemType,OwnerID, Storage,StorageID,X,Y,BodyColor, EffectColor) VALUES (" 
		<< m_ItemID << "," 
		<< m_ObjectID << "," 
		<< m_ItemType << ",'" 
		<< ownerID << "'," 
		<< (int)storage << "," 
		<< storageID << "," 
		<< (int)x << "," 
		<< (int)y << ","
		<< (int)m_BodyColor << "," 
		<< (int)m_EffectColor << ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}
	
void OustersWingItem::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE OustersWingItemObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", BodyColor=" << (int)m_BodyColor
		<< ", EffectColor=" << (int)m_EffectColor
		<< " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

VolumeWidth_t OustersWingItem::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersWingItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t OustersWingItem::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersWingItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t OustersWingItem::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersWingItemInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}

string OustersWingItem::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "OustersWingItem("
		<< "ItemID:"    << m_ItemID
		<< ",ItemType:" <<(int)m_ItemType
		<< ")";

	return msg.toString();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class OustersWingItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

void OustersWingItemInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM OustersWingItemInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, DefaultBodyColor, DefaultEffectColor FROM OustersWingItemInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			OustersWingItemInfo* pOustersWingItemInfo = new OustersWingItemInfo();
			pOustersWingItemInfo->setItemType(pResult->getInt(++i));
			pOustersWingItemInfo->setName(pResult->getString(++i));
			pOustersWingItemInfo->setEName(pResult->getString(++i));
			pOustersWingItemInfo->setPrice(pResult->getInt(++i));
			pOustersWingItemInfo->setVolumeType(pResult->getInt(++i));
			pOustersWingItemInfo->setWeight(pResult->getInt(++i));
			pOustersWingItemInfo->setBodyColor(pResult->getInt(++i));
			pOustersWingItemInfo->setEffectColor(pResult->getInt(++i));

			addItemInfo(pOustersWingItemInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

string OustersWingItemInfo::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "OustersWingItemInfo("
		<< "ItemType:"     << m_ItemType
		<< ",Name:"        << m_Name
		<< ",EName:"       << m_EName
		<< ",Price:"       << m_Price
		<< ",VolumeType:"  << Volume2String[m_VolumeType]
		<< ",Weight:"      << m_Weight
		<< ")";
	return msg.toString();

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// class OustersWingItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void OustersWingItemLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;
	Result* pResult = NULL;

	BEGIN_DB 
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pResult = pStmt->executeQuery(
			"SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, BodyColor, EffectColor FROM OustersWingItemObject "
			"WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
			pCreature->getName().c_str() 
		);

		while (pResult->next())
		{
			try {
				uint i = 0;

				ItemID_t     itemID    = pResult->getDWORD(++i);
				ObjectID_t   objectID  = pResult->getDWORD(++i);
				ItemType_t   itemType  = pResult->getDWORD(++i);
				Storage      storage   = (Storage)pResult->getInt(++i);
				StorageID_t  storageID = pResult->getDWORD(++i);
				BYTE         x         = pResult->getBYTE(++i);
				BYTE         y         = pResult->getBYTE(++i);
				Color_t BodyColor = pResult->getInt(++i);
				Color_t EffectColor = pResult->getInt(++i);

				OustersWingItem* pOustersWingItem = new OustersWingItem();
				pOustersWingItem->setItemID(itemID);
				pOustersWingItem->setObjectID(objectID);
				pOustersWingItem->setItemType(itemType);
				pOustersWingItem->setBodyColor(BodyColor);
				pOustersWingItem->setEffectColor(EffectColor);

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*	pOusters		= NULL;
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

					if (pMotorcycle != NULL) pMotorInventory = pMotorcycle->getInventory();
				}
				else if (pCreature->isVampire()) 
				{
					pVampire   = dynamic_cast<Vampire*>(pCreature);
					pInventory = pVampire->getInventory();
					pStash     = pVampire->getStash();
				}
				else if (pCreature->isOusters())
				{
					pOusters	= dynamic_cast<Ousters*>(pCreature);
					pInventory	= pOusters->getInventory();
					pStash		= pOusters->getStash();
				}
				else throw UnsupportedError("Monster,NPC 인벤토리의 저장은 아직 지원되지 않습니다.");

				switch(storage)
				{
					case STORAGE_INVENTORY :
					{
						if ( storageID != 0 )
						{
							SubInventory* pInventoryItem = dynamic_cast<SubInventory*>(findItemIID( pCreature, storageID ));
							if ( pInventoryItem == NULL )
							{
								processItemBugEx(pCreature, pOustersWingItem);
								break;
							}
	
							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pOustersWingItem))
						{
							pInventory->addItemEx(x, y, pOustersWingItem);
						}
						else
						{
							processItemBugEx(pCreature, pOustersWingItem);
						}
					}
					break;

					case STORAGE_GEAR :
						processItemBugEx(pCreature, pOustersWingItem);
						break;
				
					case STORAGE_BELT :
						processItemBugEx(pCreature, pOustersWingItem);
						break;

					case STORAGE_EXTRASLOT :
					{
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pOustersWingItem);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pOustersWingItem);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pOustersWingItem);
					}
					break;

					case STORAGE_MOTORCYCLE :
						processItemBugEx(pCreature, pOustersWingItem);
						break;

					case STORAGE_STASH :
					{
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pOustersWingItem);
						}
						else pStash->insert(x, y, pOustersWingItem);
					}
					break;

					case STORAGE_GARBAGE :
						processItemBug(pCreature, pOustersWingItem);
						break;
					default :
					{
						SAFE_DELETE(pStmt);	// by sigi
						throw Error("invalid storage or OwnerID must be NULL");
					}
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

void OustersWingItemLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB 
	{
		StringStream sql;
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, BodyColor, EffectColor FROM OustersWingItemObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			ItemID_t     itemID    = pResult->getDWORD(++i);
			ObjectID_t   objectID  = pResult->getDWORD(++i);
			ItemType_t   itemType  = pResult->getDWORD(++i);
			Storage      storage   = (Storage)pResult->getInt(++i);
			StorageID_t  storageID = pResult->getDWORD(++i);
			BYTE         x         = pResult->getBYTE(++i);
			BYTE         y         = pResult->getBYTE(++i);
			Color_t BodyColor = pResult->getInt(++i);
			Color_t EffectColor = pResult->getInt(++i);

			OustersWingItem* pOustersWingItem = new OustersWingItem();
			pOustersWingItem->setItemID(itemID);
			pOustersWingItem->setObjectID(objectID);
			pOustersWingItem->setItemType(itemType);
			pOustersWingItem->setBodyColor(BodyColor);
			pOustersWingItem->setEffectColor(EffectColor);

			if (storage == STORAGE_ZONE)
			{
				Tile & pTile = pZone->getTile(x,y);
				Assert(!pTile.hasItem());
				pTile.addItem(pOustersWingItem);
			}
			else
			{
				throw Error("Storage must be STORAGE_ZONE");
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

void OustersWingItemLoader::load(StorageID_t storageID, Inventory* pInventory) 
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

