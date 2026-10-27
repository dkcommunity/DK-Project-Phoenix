//////////////////////////////////////////////////////////////////////////////
// Filename    : Trouser.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Trouser.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "ItemInfoManager.h"
#include "Stash.h"
#include "ItemUtil.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

// global variable declaration
TrouserInfoManager* g_pTrouserInfoManager = NULL;

ItemID_t Trouser::m_ItemIDRegistry = 0;
Mutex    Trouser::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Trouser::Trouser()
	throw()
//: m_ItemType(0), m_Durability(0)
{
	setItemType(0);
	setDurability(0);
	//m_EnchantLevel = 0;
}

Trouser::Trouser(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
	setItemType(itemType);
	setOptionType(optionType);
	//m_EnchantLevel = 0;

	setDurability(computeMaxDurability(this));

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList()))
	{
		filelog("itembug.log", "Trouser::Trouser() : Invalid item type or option type");
		throw ("Trouser::Trouser() : Invalid item type or optionType");
	}
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void Trouser::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	setOptionTypeToField( getOptionTypeList(), optionField );

	string GTwooptionField;
	setOptionTypeToField( getGTwoOptionTypeList(), GTwooptionField );

			
	string thirdoptionField;
	setOptionTypeToField( getThirdOptionTypeList(), thirdoptionField );
	
	ostringstream os;

	os << "INSERT INTO TrouserObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
		<< " X, Y, OptionType, ThirdOptionType, Durability, Grade,GTwooptionType, ItemFlag)"
		<< " VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< getItemType() << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", " 
		<< storageID << ", " 
		<<(int)x << ", " 
		<<(int)y << ", '" 
		<< optionField.c_str() << "', '" 
		<< thirdoptionField.c_str()  << "', " 
		<< getDurability() << ", " 
		<< (int)getGrade() << ", '" 
		<< GTwooptionField.c_str() << "', " 
		<< (int)m_CreateType << ")"
		
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " GTwooptionType=(" << getOptionTypeToString(getGTwoOptionTypeList()).c_str() << ")"
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		

	__END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void Trouser::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	string optionField;
	setOptionTypeToField( getOptionTypeList(), optionField );

	string GTwooptionField;
	setOptionTypeToField( getGTwoOptionTypeList(), GTwooptionField );
			
	string thirdoptionField;
	setOptionTypeToField( getThirdOptionTypeList(), thirdoptionField );
	
	ostringstream os;
	
	os << "UPDATE TrouserObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << getItemType()
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", OptionType='" << optionField.c_str()
		<< "', ThirdOptionType='" << thirdoptionField.c_str()
		<< "', Durability=" << getDurability()
		<< ", Grade=" << (int)getGrade()
		<< ", EnchantLevel=" << (int)getEnchantLevel()
		<< ", GTwooptionType='" << GTwooptionField.c_str()
		<< "' WHERE ItemID=" << m_ItemID
	 
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " GTwooptionType=(" << getOptionTypeToString(getGTwoOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
					
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Trouser::toString() const 
	throw()
{
	StringStream msg;

	msg << "Trouser("
		<< "ItemID:"        << m_ItemID
		<< ",ItemType:"     <<(int)getItemType()
		<< ",OptionType:"   <<getOptionTypeToString(getOptionTypeList()).c_str()
		<< ",Durability:"   <<(int)getDurability()
		<< ",EnchantLevel:" <<(int)getEnchantLevel()
		<< ")";

	return msg.toString();
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string TrouserInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "TrouserInfo("
		<< "ItemType:" << m_ItemType
		<< ",Name:" << m_Name
		<< ",EName:" << m_EName
		<< ",Price:" << m_Price
		<< ",VolumeType:" << Volume2String[m_VolumeType]
		<< ",Weight:" << m_Weight
		<< ",Description:" << m_Description
		<< ",Durability:" << m_Durability
		<< ",DefenseBonus:" << m_DefenseBonus
		<< ",ProtectionBonus:" << m_ProtectionBonus
		<< ")";

	return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void TrouserInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM TrouserInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, DowngradeRatio FROM TrouserInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			TrouserInfo* pTrouserInfo = new TrouserInfo();

			pTrouserInfo->setItemType(pResult->getInt(++i));
			pTrouserInfo->setName(pResult->getString(++i));
			pTrouserInfo->setEName(pResult->getString(++i));
			pTrouserInfo->setPrice(pResult->getInt(++i));
			pTrouserInfo->setVolumeType(pResult->getInt(++i));
			pTrouserInfo->setWeight(pResult->getInt(++i));
			pTrouserInfo->setRatio(pResult->getInt(++i));
			pTrouserInfo->setDurability(pResult->getInt(++i));
			pTrouserInfo->setDefenseBonus(pResult->getInt(++i));
			pTrouserInfo->setProtectionBonus(pResult->getInt(++i));
			pTrouserInfo->setReqAbility(pResult->getString(++i));
			pTrouserInfo->setItemLevel(pResult->getInt(++i));
			pTrouserInfo->setDefaultOptions(pResult->getString(++i));
			pTrouserInfo->setUpgradeRatio(pResult->getInt(++i));
			pTrouserInfo->setUpgradeCrashPercent(pResult->getInt(++i));
			pTrouserInfo->setNextOptionRatio(pResult->getInt(++i));
			pTrouserInfo->setNextItemType(pResult->getInt(++i));
			pTrouserInfo->setDowngradeRatio(pResult->getInt(++i));

			addItemInfo(pTrouserInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void TrouserLoader::load(Creature* pCreature) 
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

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< "OptionType, Durability, EnchantLevel FROM TrouserObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/
#ifdef VERSION_THIRD_ENCHANT_1
	    Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, ThirdOptionType, Durability, Grade, EnchantLevel, ItemFlag,GTwoOptionType FROM TrouserObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
								pCreature->getName().c_str() );
#else
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, Durability, Grade, EnchantLevel, ItemFlag FROM TrouserObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
			pCreature->getName().c_str() );
#endif


		while (pResult->next())
		{
			try {
				uint i = 0;

				Trouser* pTrouser = new Trouser();

				pTrouser->setItemID(pResult->getDWORD(++i));
				pTrouser->setObjectID(pResult->getDWORD(++i));
				pTrouser->setItemType(pResult->getDWORD(++i));
	
				if (g_pTrouserInfoManager->getItemInfo(pTrouser->getItemType())->isUnique())
					pTrouser->setUnique();

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pTrouser->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
				string thirdoptionField = pResult->getString(++i);
				list<OptionType_t> thirdoptionTypes;
				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
				pTrouser->setThirdOptionType(thirdoptionTypes);
#endif

				pTrouser->setDurability(pResult->getInt(++i));
				pTrouser->setGrade(pResult->getInt(++i));
				pTrouser->setEnchantLevel(pResult->getInt(++i));
				pTrouser->setCreateType((Item::CreateType)pResult->getInt(++i));

				string GTwooptionField = pResult->getString(++i);
				list<OptionType_t> GTwooptionTypes;
				setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
				pTrouser->setGTwoOptionType(GTwooptionTypes);

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
								processItemBugEx(pCreature, pTrouser);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pTrouser))
						{
							pInventory->addItemEx(x, y, pTrouser);
						}
						else
						{
							processItemBugEx(pCreature, pTrouser);
						}
						break;

					case STORAGE_GEAR:
						if (pCreature->isSlayer())
						{
							if ( y == pSlayer->getCurrentGearSlotID() )
							{
								if (!pSlayer->isWear((Slayer::WearPart)x))
								{
									pSlayer->wearItem((Slayer::WearPart)x, pTrouser);
								}
								else
								{
									processItemBugEx(pCreature, pTrouser);
								}
							}
							else
							{
								pSlayer->putItemToGear(x, y, pTrouser);
							}
						}
						else if (pCreature->isVampire())
						{
							processItemBugEx(pCreature, pTrouser);
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pTrouser);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pTrouser);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pTrouser);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pTrouser);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pTrouser);
						}
						else pStash->insert(x, y, pTrouser);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pTrouser);
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
void TrouserLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;
#ifdef VERSION_THIRD_ENCHANT_1
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< " OptionType, ThirdOptionType, Durability, EnchantLevel, ItemFlag,GTwoOptionType FROM TrouserObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
#else
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< " OptionType, Durability, EnchantLevel, ItemFlag FROM TrouserObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
#endif

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			Trouser* pTrouser = new Trouser();

			pTrouser->setItemID(pResult->getInt(++i));
			pTrouser->setObjectID(pResult->getInt(++i));
			pTrouser->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			string optionField = pResult->getString(++i);
			list<OptionType_t> optionTypes;
			setOptionTypeFromField(optionTypes, optionField);
			pTrouser->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
			string thirdoptionField = pResult->getString(++i);
			list<OptionType_t> thirdoptionTypes;
			setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
			pTrouser->setThirdOptionType(thirdoptionTypes);
#endif

			pTrouser->setDurability(pResult->getInt(++i));
			pTrouser->setEnchantLevel(pResult->getInt(++i));
			pTrouser->setCreateType((Item::CreateType)pResult->getInt(++i));

			string GTwooptionField = pResult->getString(++i);
			list<OptionType_t> GTwooptionTypes;
			setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
			pTrouser->setGTwoOptionType(GTwooptionTypes);

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pTrouser);
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
void TrouserLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

TrouserLoader* g_pTrouserLoader = NULL;
