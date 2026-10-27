//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillBook.cpp
// Written By  : Shinobi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SkillBook.h"
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

// global variable declaration
SkillBookInfoManager* g_pSkillBookInfoManager = NULL;

ItemID_t SkillBook::m_ItemIDRegistry = 0;
Mutex    SkillBook::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
SkillBook::SkillBook()
	throw()
{
	m_ItemType = 0;
}

SkillBook::SkillBook(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
{
	m_ItemType = itemType;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "SkillBook::SkillBook() : Invalid item type or option type");
		throw ("SkillBook::SkillBook() : Invalid item type or optionType");
	}
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void SkillBook::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO SkillBookObject (ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y ) VALUES (" 
		<< m_ItemID << ", " 
		<< m_ObjectID << ", " 
		<< m_ItemType << ", '" 
		<< ownerID.c_str() << "', " 
		<< (int)storage << ", " 
		<< storageID << ", " 
		<< (int)x << ", " 
		<< (int)y << ")"; 
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void SkillBook::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE SkillBookObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string SkillBook::toString() const 
	throw()
{
	StringStream msg;
	msg << "SkillBook("
		<< "ItemID:"	<< m_ItemID
		<< ",ItemType:"	<< (int)m_ItemType
		<< ")";
	return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t SkillBook::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pSkillBookInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t SkillBook::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pSkillBookInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t SkillBook::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pSkillBookInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string SkillBookInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "SkillBookInfo("
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


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void SkillBookInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM SkillBookInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount + 1];

        for (uint i = 0 ; i < m_InfoCount + 1 ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, SkillType, SkillDomainType, Race, NeedAttribute FROM SkillBookInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			SkillBookInfo* pSkillBookInfo = new SkillBookInfo();

			pSkillBookInfo->setItemType(pResult->getInt(++i));
			pSkillBookInfo->setName(pResult->getString(++i));
			pSkillBookInfo->setEName(pResult->getString(++i));
			pSkillBookInfo->setPrice(pResult->getInt(++i));
			pSkillBookInfo->setVolumeType(pResult->getInt(++i));
			pSkillBookInfo->setWeight(pResult->getInt(++i));
			pSkillBookInfo->setSkillType(pResult->getInt(++i));
			pSkillBookInfo->setSkillDomainType(pResult->getInt(++i));
			pSkillBookInfo->setRace(pResult->getInt(++i));

			pSkillBookInfo->parseNeedAttribute(pResult->getString(++i));

			addItemInfo(pSkillBookInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void SkillBookLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		/*
		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM SkillBookObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/

		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM SkillBookObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );



		while (pResult->next())
		{
			try {
				uint i = 0;

				SkillBook* pSkillBook = new SkillBook();

				pSkillBook->setItemID(pResult->getDWORD(++i));
				pSkillBook->setObjectID(pResult->getDWORD(++i));
				pSkillBook->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*    pOusters        = NULL;
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
								processItemBugEx(pCreature, pSkillBook);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pSkillBook))
						{
							pInventory->addItemEx(x, y, pSkillBook);
						}
						else
						{
							processItemBugEx(pCreature, pSkillBook);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pSkillBook);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pSkillBook);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pSkillBook);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pSkillBook);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pSkillBook);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pSkillBook);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pSkillBook);
						}
						else pStash->insert(x, y, pSkillBook);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pSkillBook);
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
void SkillBookLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM SkillBookObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			SkillBook* pSkillBook = new SkillBook();

			pSkillBook->setItemID(pResult->getInt(++i));
			pSkillBook->setObjectID(pResult->getInt(++i));
			pSkillBook->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pSkillBook);
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
void SkillBookLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

void SkillBookInfo::parseNeedAttribute(const string& needattribute)
{
	m_INT = 0;
	m_DEX = 0;

	if( needattribute.size() < 1 ) return;

	uint a = 0, b = 0, c = 0;

	while( c < needattribute.size() - 1 )
	{
		a = needattribute.find_first_of('(', c);
		b = needattribute.find_first_of(',', a+1);
		c = needattribute.find_first_of(')', b+1);

		if( a > b || b > c ) break;

		string part = needattribute.substr(a+1, b-a-1);
		int value = atoi(needattribute.substr(b+1, c-b-1).c_str());

		if( part == "INT" )		m_INT = value;
		if( part == "DEX" )		m_DEX = value;
	}
}

SkillBookLoader* g_pSkillBookLoader = NULL;
