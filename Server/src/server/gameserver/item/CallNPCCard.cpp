//////////////////////////////////////////////////////////////////////////////
// Filename    : CallNPCCard.cpp
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CallNPCCard.h"
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

CallNPCCardInfoManager* g_pCallNPCCardInfoManager = NULL;

ItemID_t CallNPCCard::m_ItemIDRegistry = 0;
Mutex    CallNPCCard::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCard member methods
//////////////////////////////////////////////////////////////////////////////

CallNPCCard::CallNPCCard()
	throw()
{
	m_ItemType = 0;
}

CallNPCCard::CallNPCCard(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)
	throw()
{
	m_ItemType = itemType;
	m_Num      = Num;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "CallNPCCard::CallNPCCard() : Invalid item type or option type");
		throw ("CallNPCCard::CallNPCCard() : Invalid item type or optionType");
	}
}

void CallNPCCard::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO CallNPCCardObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, ItemFlag) VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< m_ItemType << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", " 
		<< storageID << ", " 
		<< (int)x << ", " 
		<< (int)y << ", " 
		<< (int)m_Num << ", " 
		<< (int)m_CreateType << ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}

void CallNPCCard::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE CallNPCCardObject SET "
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

string CallNPCCard::toString() const 
	throw()
{
	StringStream msg;

	msg << "CallNPCCard("
		<< "ItemID:"    << m_ItemID
		<< ",ItemType:" <<(int)m_ItemType
		<< ",Num:"      <<(int)m_Num
		<< ")";

	return msg.toString();
}

VolumeWidth_t CallNPCCard::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCallNPCCardInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t CallNPCCard::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCallNPCCardInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t CallNPCCard::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCallNPCCardInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCardInfo member methods
//////////////////////////////////////////////////////////////////////////////

string CallNPCCardInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "CallNPCCardInfo("
		<< "ItemType:"     <<(int)m_ItemType
		<< ",Name:"        << m_Name
		<< ",EName:"       << m_EName
		<< ",Price:"       <<(int)m_Price
		<< ",VolumeType:"  << Volume2String[m_VolumeType]
		<< ",Weight:"      <<(int)m_Weight
		<< ")";
	return msg.toString();
}

void CallNPCCardInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM CallNPCCardInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, NPCID, ScriptType FROM CallNPCCardInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			CallNPCCardInfo* pCallNPCCardInfo = new CallNPCCardInfo();

			pCallNPCCardInfo->setItemType(pResult->getInt(++i));
			pCallNPCCardInfo->setName(pResult->getString(++i));
			pCallNPCCardInfo->setEName(pResult->getString(++i));
			pCallNPCCardInfo->setPrice(pResult->getInt(++i));
			pCallNPCCardInfo->setVolumeType(pResult->getInt(++i));
			pCallNPCCardInfo->setWeight(pResult->getInt(++i));
			pCallNPCCardInfo->setRatio(pResult->getInt(++i));
			pCallNPCCardInfo->setCallNPCID(pResult->getInt(++i));
			pCallNPCCardInfo->setScriptType(pResult->getInt(++i));

			addItemInfo(pCallNPCCardInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCardLoader member methods
//////////////////////////////////////////////////////////////////////////////

void CallNPCCardLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM CallNPCCardObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );

		while (pResult->next())
		{
			try {
				uint i = 0;

				CallNPCCard* pCallNPCCard = new CallNPCCard();

				pCallNPCCard->setItemID(pResult->getDWORD(++i));
				pCallNPCCard->setObjectID(pResult->getDWORD(++i));
				pCallNPCCard->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				pCallNPCCard->setNum(pResult->getBYTE(++i));
				pCallNPCCard->setCreateType((Item::CreateType)pResult->getInt(++i));

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*    pOusters        = NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
				Item*       pItem           = NULL;
				Stash*      pStash          = NULL;

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
								processItemBugEx(pCreature, pCallNPCCard);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pCallNPCCard))
						{
							pInventory->addItemEx(x, y, pCallNPCCard);
						}
						else
						{
							processItemBugEx(pCreature, pCallNPCCard);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pCallNPCCard);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pCallNPCCard);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pCallNPCCard);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pCallNPCCard);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pCallNPCCard);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pCallNPCCard);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pCallNPCCard);
						}
						else pStash->insert(x, y, pCallNPCCard);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pCallNPCCard);
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

void CallNPCCardLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM CallNPCCardObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			CallNPCCard* pCallNPCCard = new CallNPCCard();

			pCallNPCCard->setItemID(pResult->getInt(++i));
			pCallNPCCard->setObjectID(pResult->getInt(++i));
			pCallNPCCard->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pCallNPCCard->setNum(pResult->getBYTE(++i));
			pCallNPCCard->setCreateType((Item::CreateType)pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pCallNPCCard);
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

void CallNPCCardLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

CallNPCCardLoader* g_pCallNPCCardLoader = NULL;
