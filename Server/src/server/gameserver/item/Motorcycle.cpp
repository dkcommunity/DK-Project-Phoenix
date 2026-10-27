//////////////////////////////////////////////////////////////////////////////
// Filename    : Motorcycle.cpp
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Motorcycle.h"
#include "ParkingCenter.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "ItemInfoManager.h"
#include "Stash.h"
#include "ItemUtil.h"
#include "DBExecuteManager.h"

#include <sstream>

// global variable declaration
MotorcycleInfoManager* g_pMotorcycleInfoManager = NULL;

ItemID_t Motorcycle::m_ItemIDRegistry = 0;
Mutex    Motorcycle::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Motorcycle::Motorcycle()
	throw()
: m_ItemType(0), m_Durability(0), m_pInventory(NULL)
{
}

Motorcycle::Motorcycle(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw(Error)
//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0), m_pInventory(NULL)
: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
	__BEGIN_TRY

	/*
	// 모터사이클은 타입에 따라 인벤토리의 크기가 다르다.
	switch(itemType)
	{
		case 0: m_pInventory = new Inventory(10, 6); break;
		case 1: m_pInventory = new Inventory(10, 6); break;
		case 2: m_pInventory = new Inventory(10, 6); break;
		case 3: m_pInventory = new Inventory(10, 6); break;
		case 4: m_pInventory = new Inventory(10, 6); break;
		case 5: m_pInventory = new Inventory(10, 6); break;
		default:
			m_pInventory = new Inventory(10, 6); break;
	}
	*/
	m_pInventory = new Inventory(0, 0);

	m_Durability = computeMaxDurability(this);

	m_BodyColor = 0;
	m_EffectColor = 0;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "Motorcycle::Motorcycle() : Invalid item type or option type");
		throw ("Motorcycle::Motorcycle() : Invalid item type or optionType");
	}

	__END_CATCH
}

Motorcycle::~Motorcycle()
	throw()
{
//	SAFE_DELETE(m_pInventory);
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Motorcycle::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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

	string optionField;
	setOptionTypeToField( m_OptionType, optionField );
	
	ostringstream os;
	
	os << "INSERT INTO MotorcycleObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
		<< " X, Y, OptionType, Durability, BodyColor, EffectColor) VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< m_ItemType << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", " 
		<< storageID << ", " 
		<<(int)x << ", " 
		<<(int)y << ", '" 
		<< optionField.c_str() << "', "
		<< m_Durability << ","
		<<(int)m_BodyColor << ", " 
		<<(int)m_EffectColor << ")"
		
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Motorcycle::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	string optionField;
	setOptionTypeToField( m_OptionType, optionField );
	
	ostringstream os;
	
	os <<  "UPDATE MotorcycleObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", OptionType='" << optionField.c_str()
		<< "', Durability=" << m_Durability
		<< ", BodyColor=" << (int)m_BodyColor
		<< ", EffectColor=" << (int)m_EffectColor
		<< " WHERE ItemID=" << m_ItemID

		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Motorcycle::toString() const 
	throw()
{
	StringStream msg;

	msg << "Motorcycle("
		<< "ItemID:"      << m_ItemID
		<< ",ItemType:"   <<(int)m_ItemType
		<< ",OptionType:" <<getOptionTypeToString(m_OptionType).c_str()
		<< ",Durability:" <<(int)m_Durability
		<< ")";

	return msg.toString();
}


//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t Motorcycle::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pMotorcycleInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t Motorcycle::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pMotorcycleInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t Motorcycle::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pMotorcycleInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string MotorcycleInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "MotorcycleInfo("
		<< "ItemType:" << m_ItemType
		<< ",Name:" << m_Name
		<< ",EName:" << m_EName
		<< ",Price:" << m_Price
		<< ",VolumeType:" << Volume2String[m_VolumeType]
		<< ",Weight:" << m_Weight
		<< ",Description:" << m_Description
		<< ",Durability:" << m_Durability
		<< ")";

	return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void MotorcycleInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM MotorcycleInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability FROM MotorcycleInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			MotorcycleInfo* pMotorcycleInfo = new MotorcycleInfo();

			pMotorcycleInfo->setItemType(pResult->getInt(++i));
			pMotorcycleInfo->setName(pResult->getString(++i));
			pMotorcycleInfo->setEName(pResult->getString(++i));
			pMotorcycleInfo->setPrice(pResult->getInt(++i));
			pMotorcycleInfo->setVolumeType(pResult->getInt(++i));
			pMotorcycleInfo->setWeight(pResult->getInt(++i));
			pMotorcycleInfo->setRatio(pResult->getInt(++i));
			pMotorcycleInfo->setDurability(pResult->getInt(++i));

			addItemInfo(pMotorcycleInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void MotorcycleLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( 
			"SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability "
				" FROM MotorcycleObject "
				" WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
			pCreature->getName().c_str() 
		);

		while (pResult->next())
		{
			try {
				uint i = 0;

				Motorcycle* pMotorcycle = new Motorcycle();

				pMotorcycle->setItemID(pResult->getDWORD(++i));
				pMotorcycle->setObjectID(pResult->getDWORD(++i));
				pMotorcycle->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pMotorcycle->setOptionType(optionTypes);

				pMotorcycle->setDurability(pResult->getInt(++i));

				/*
				switch(storage)
				{
					case STORAGE_INVENTORY:
					case STORAGE_GEAR:
					case STORAGE_BELT :
					case STORAGE_EXTRASLOT :
					case STORAGE_MOTORCYCLE:
					case STORAGE_STASH:
						// 모터 사이클 안에 모터 사이클을 보관할 수가 있나
						Assert(false);
						pMotorcycle->destroy();
						SAFE_DELETE(pMotorcycle);
						break;

					default :
						SAFE_DELETE(pStmt);	// by sigi
						throw Error("invalid storage or OwnerID must be NULL");
				}
				*/

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

void MotorcycleLoader::loadForKey(ItemID_t targetID)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( 
			"SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability "
				" FROM MotorcycleObject "
				" WHERE ItemID = '%d' AND Storage IN (0, 1, 2, 3, 4, 5, 9)",
				targetID
		);

		if (pResult->next())
		{
			try {
				uint i = 0;

				Motorcycle* pMotorcycle = new Motorcycle();

				pMotorcycle->setItemID(pResult->getDWORD(++i));
				pMotorcycle->setObjectID(pResult->getDWORD(++i));
				pMotorcycle->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pMotorcycle->setOptionType(optionTypes);

				pMotorcycle->setDurability(pResult->getInt(++i));
				
				MotorcycleBox* pBox = new MotorcycleBox(pMotorcycle, NULL, -1, -1);
				g_pParkingCenter->addMotorcycleBox(pBox);

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
void MotorcycleLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, OptionType, Storage, StorageID, X, Y, "
			<< " Durability , BodyColor, EffectColor FROM MotorcycleObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			Motorcycle* pMotorcycle = new Motorcycle();

			pMotorcycle->setItemID(pResult->getInt(++i));
			pMotorcycle->setObjectID(pResult->getInt(++i));
			pMotorcycle->setItemType(pResult->getInt(++i));
			
			string optionField = pResult->getString(++i);
			list<OptionType_t> optionTypes;
			setOptionTypeFromField(optionTypes, optionField);
			pMotorcycle->setOptionType(optionTypes);

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pMotorcycle->setDurability(pResult->getInt(++i));

			pMotorcycle->setBodyColor(pResult->getInt(++i));
			pMotorcycle->setEffectColor(pResult->getInt(++i));
			
			/*
			switch(storage)
			{
				case STORAGE_ZONE :	
					{
//						Tile & pTile = pZone->getTile(x,y);
//						Assert(!pTile.hasItem());
						pZone->addItem(pMotorcycle, x, y);
//						pTile.addItem(pMotorcycle);
					}
					break;

				case STORAGE_STASH :
				case STORAGE_CORPSE :
					throw UnsupportedError("상자 및 시체안의 아이템의 저장은 아직 지원되지 않습니다.");

				default :
					throw Error("Storage must be STORAGE_ZONE");
			}
			*/
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void MotorcycleLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

MotorcycleLoader* g_pMotorcycleLoader = NULL;
