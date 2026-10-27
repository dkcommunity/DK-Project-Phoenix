//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireNecklace.cpp
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "VampireNecklace.h"
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
VampireNecklaceInfoManager* g_pVampireNecklaceInfoManager = NULL;

ItemID_t VampireNecklace::m_ItemIDRegistry = 0;
Mutex    VampireNecklace::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
VampireNecklace::VampireNecklace()
	throw()
//: m_ItemType(0), m_Durability(0)
{
	setItemType(0);
	setDurability(0);
	//m_EnchantLevel = 0;
}

VampireNecklace::VampireNecklace(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
	setItemType(itemType);
	setOptionType(optionType);
	//m_EnchantLevel = 0;

	setDurability(computeMaxDurability(this));

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList()))
	{
		filelog("itembug.log", "VampireNecklace::VampireNecklace() : Invalid item type or option type");
		throw ("VampireNecklace::VampireNecklace() : Invalid item type or optionType");
	}
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void VampireNecklace::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO VampireNecklaceObject "
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
		<< thirdoptionField.c_str() << "', " 
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
void VampireNecklace::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
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
	
	os << "UPDATE VampireNecklaceObject SET "
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
		<< ", Grade=" << getGrade()
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
string VampireNecklace::toString() const 
	throw()
{
	StringStream msg;

	msg << "VampireNecklace("
		<< "ItemID:"        << m_ItemID
		<< ",ItemType:"     <<(int)getItemType()
		<< ",OptionType:"   <<getOptionTypeToString(getOptionTypeList()).c_str()
		<< ",Durability:"   <<(int)getDurability()
		<< ",EnchantLevel:" <<(int)getEnchantLevel()
		<< ")";

	return msg.toString();
}

/*
//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t VampireNecklace::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireNecklaceInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t VampireNecklace::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireNecklaceInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t VampireNecklace::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireNecklaceInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t VampireNecklace::getDefenseBonus() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireNecklaceInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

	__END_CATCH
}
Protection_t VampireNecklace::getProtectionBonus() const
	throw(Error)
{
	__BEGIN_TRY

	return g_pVampireNecklaceInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();
	
	__END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VampireNecklaceInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "VampireNecklaceInfo("
		<< "ItemType:" << m_ItemType
		<< ",Name:" << m_Name
		<< ",EName:" << m_EName
		<< ",Price:" << m_Price
		<< ",VolumeType:" << Volume2String[m_VolumeType]
		<< ",Weight:" << m_Weight
		<< ",Description:" << m_Description
		<< ",Durability:" << m_Durability
		<< ",DefenseBonus:" << m_DefenseBonus
		<< ")";

	return msg.toString();
}


//--------------------------------------------------------------------------------
// load from DB
//--------------------------------------------------------------------------------
void VampireNecklaceInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM VampireNecklaceInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

         // 20080703 마법 방어 추가
        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, MagicProtection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, DowngradeRatio FROM VampireNecklaceInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			VampireNecklaceInfo* pVampireNecklaceInfo = new VampireNecklaceInfo();

			pVampireNecklaceInfo->setItemType(pResult->getInt(++i));
			pVampireNecklaceInfo->setName(pResult->getString(++i));
			pVampireNecklaceInfo->setEName(pResult->getString(++i));
			pVampireNecklaceInfo->setPrice(pResult->getInt(++i));
			pVampireNecklaceInfo->setVolumeType(pResult->getInt(++i));
			pVampireNecklaceInfo->setWeight(pResult->getInt(++i));
			pVampireNecklaceInfo->setRatio(pResult->getInt(++i));
			pVampireNecklaceInfo->setDurability(pResult->getInt(++i));
			pVampireNecklaceInfo->setDefenseBonus(pResult->getInt(++i));
			pVampireNecklaceInfo->setProtectionBonus(pResult->getInt(++i));
			pVampireNecklaceInfo->setMagicProtectionBonus(pResult->getInt(++i)); // 20080703 마법 방어 추가
			pVampireNecklaceInfo->setReqAbility(pResult->getString(++i));
			pVampireNecklaceInfo->setItemLevel(pResult->getInt(++i));
			pVampireNecklaceInfo->setDefaultOptions(pResult->getString(++i));
			pVampireNecklaceInfo->setUpgradeRatio(pResult->getInt(++i));
			pVampireNecklaceInfo->setUpgradeCrashPercent(pResult->getInt(++i));
			pVampireNecklaceInfo->setNextOptionRatio(pResult->getInt(++i));
			pVampireNecklaceInfo->setNextItemType(pResult->getInt(++i));
			pVampireNecklaceInfo->setDowngradeRatio(pResult->getInt(++i));

			addItemInfo(pVampireNecklaceInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void VampireNecklaceLoader::load(Creature* pCreature) 
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
			<< "OptionType, Durability, EnchantLevel FROM VampireNecklaceObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/
#ifdef VERSION_THIRD_ENCHANT_1
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, ThirdOptionType, Durability, Grade, EnchantLevel, ItemFlag,GTwoOptionType FROM VampireNecklaceObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
								pCreature->getName().c_str() );
#else
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,OptionType, Durability, Grade, EnchantLevel, ItemFlag FROM VampireNecklaceObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
			pCreature->getName().c_str() );
#endif

		while (pResult->next())
		{
			try {
				uint i = 0;

				VampireNecklace* pVampireNecklace = new VampireNecklace();

				pVampireNecklace->setItemID(pResult->getDWORD(++i));
				pVampireNecklace->setObjectID(pResult->getDWORD(++i));
				pVampireNecklace->setItemType(pResult->getDWORD(++i));
	
				if (g_pVampireNecklaceInfoManager->getItemInfo(pVampireNecklace->getItemType())->isUnique())
					pVampireNecklace->setUnique();

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pVampireNecklace->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
				string thirdoptionField = pResult->getString(++i);
				list<OptionType_t> thirdoptionTypes;
				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
				pVampireNecklace->setThirdOptionType(thirdoptionTypes);
#endif

				pVampireNecklace->setDurability(pResult->getInt(++i));
				pVampireNecklace->setGrade(pResult->getInt(++i));
				pVampireNecklace->setEnchantLevel(pResult->getInt(++i));
				pVampireNecklace->setCreateType((Item::CreateType)pResult->getInt(++i));

				string GTwooptionField = pResult->getString(++i);
				list<OptionType_t> GTwooptionTypes;
				setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
				pVampireNecklace->setGTwoOptionType(GTwooptionTypes);


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
								processItemBugEx(pCreature, pVampireNecklace);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pVampireNecklace))
						{
							pInventory->addItemEx(x, y, pVampireNecklace);
						}
						else
						{
							processItemBugEx(pCreature, pVampireNecklace);
						}
						break;

					case STORAGE_GEAR:
						if (pCreature->isSlayer())
						{
							processItemBugEx(pCreature, pVampireNecklace);
						}
						else if (pCreature->isVampire())
						{
							if ( y == pVampire->getCurrentGearSlotID() )
							{
								if (!pVampire->isWear((Vampire::WearPart)x))
								{
									pVampire->wearItem((Vampire::WearPart)x, pVampireNecklace);
								}
								else
								{
									processItemBugEx(pCreature, pVampireNecklace);
								}
							}
							else
							{
								pVampire->putItemToGear(x, y, pVampireNecklace);
							}
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pVampireNecklace);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pVampireNecklace);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pVampireNecklace);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pVampireNecklace);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pVampireNecklace);
						}
						else pStash->insert(x, y, pVampireNecklace);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pVampireNecklace);
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
void VampireNecklaceLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;
#ifdef VERSION_THIRD_ENCHANT_1
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< " OptionType, ThirdOptionType, Durability, EnchantLevel, ItemFlag,GTwoOptionType FROM VampireNecklaceObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
#else
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< " OptionType, Durability, EnchantLevel, ItemFlag FROM VampireNecklaceObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
#endif
		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			VampireNecklace* pVampireNecklace = new VampireNecklace();

			pVampireNecklace->setItemID(pResult->getInt(++i));
			pVampireNecklace->setObjectID(pResult->getInt(++i));
			pVampireNecklace->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			string optionField = pResult->getString(++i);
			list<OptionType_t> optionTypes;
			setOptionTypeFromField(optionTypes, optionField);
			pVampireNecklace->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
			string thirdoptionField = pResult->getString(++i);
			list<OptionType_t> thirdoptionTypes;
			setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
			pVampireNecklace->setThirdOptionType(thirdoptionTypes);
#endif


			pVampireNecklace->setDurability(pResult->getInt(++i));
			pVampireNecklace->setEnchantLevel(pResult->getInt(++i));
			pVampireNecklace->setCreateType((Item::CreateType)pResult->getInt(++i));

			string GTwooptionField = pResult->getString(++i);
			list<OptionType_t> GTwooptionTypes;
			setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
			pVampireNecklace->setGTwoOptionType(GTwooptionTypes);
			

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pVampireNecklace);
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
void VampireNecklaceLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

VampireNecklaceLoader* g_pVampireNecklaceLoader = NULL;
