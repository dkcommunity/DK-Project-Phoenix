//////////////////////////////////////////////////////////////////////////////
// Filename    : ShoulderArmor.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ShoulderArmor.h"
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
ShoulderArmorInfoManager* g_pShoulderArmorInfoManager = NULL;

ItemID_t ShoulderArmor::m_ItemIDRegistry = 0;
Mutex    ShoulderArmor::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ShoulderArmor::ShoulderArmor()
	throw()
{
	setItemType(0);
	setDurability(0);
}

ShoulderArmor::ShoulderArmor(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
{
	setItemType(itemType);
	setOptionType(optionType);
	setDurability(computeMaxDurability(this));

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList()))
	{
		filelog("itembug.log", "ShoulderArmor::ShoulderArmor() : Invalid item type or option type");
		throw ("ShoulderArmor::ShoulderArmor() : Invalid item type or optionType");
	}
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void ShoulderArmor::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO ShoulderArmorObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
		<< " X, Y, OptionType, ThirdOptionType, Durability, Grade,GTwooptionType, ItemFlag) VALUES(" 
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
void ShoulderArmor::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
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
	
	os << "UPDATE ShoulderArmorObject SET "
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
string ShoulderArmor::toString() const 
	throw()
{
	StringStream msg;

	msg << "ShoulderArmor("
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
string ShoulderArmorInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "ShoulderArmorInfo("
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
void ShoulderArmorInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM ShoulderArmorInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, DowngradeRatio FROM ShoulderArmorInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			ShoulderArmorInfo* pShoulderArmorInfo = new ShoulderArmorInfo();

			pShoulderArmorInfo->setItemType(pResult->getInt(++i));
			pShoulderArmorInfo->setName(pResult->getString(++i));
			pShoulderArmorInfo->setEName(pResult->getString(++i));
			pShoulderArmorInfo->setPrice(pResult->getInt(++i));
			pShoulderArmorInfo->setVolumeType(pResult->getInt(++i));
			pShoulderArmorInfo->setWeight(pResult->getInt(++i));
			pShoulderArmorInfo->setRatio(pResult->getInt(++i));
			pShoulderArmorInfo->setDurability(pResult->getInt(++i));
			pShoulderArmorInfo->setDefenseBonus(pResult->getInt(++i));
			pShoulderArmorInfo->setProtectionBonus(pResult->getInt(++i));
			pShoulderArmorInfo->setReqAbility(pResult->getString(++i));
			pShoulderArmorInfo->setItemLevel(pResult->getInt(++i));
			pShoulderArmorInfo->setDefaultOptions(pResult->getString(++i));
			pShoulderArmorInfo->setUpgradeRatio(pResult->getInt(++i));
			pShoulderArmorInfo->setUpgradeCrashPercent(pResult->getInt(++i));
			pShoulderArmorInfo->setNextOptionRatio(pResult->getInt(++i));
			pShoulderArmorInfo->setNextItemType(pResult->getInt(++i));
			pShoulderArmorInfo->setDowngradeRatio(pResult->getInt(++i));

			addItemInfo(pShoulderArmorInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void ShoulderArmorLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
#ifdef VERSION_THIRD_ENCHANT_1
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, ThirdOptionType, Durability, Grade, EnchantLevel, ItemFlag,GTwoOptionType FROM ShoulderArmorObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
											pCreature->getName().c_str() );
#else
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, EnchantLevel, ItemFlag FROM ShoulderArmorObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
											pCreature->getName().c_str() );
#endif

		while (pResult->next())
		{
			try {
				uint i = 0;

				ShoulderArmor* pShoulderArmor = new ShoulderArmor();

				pShoulderArmor->setItemID(pResult->getDWORD(++i));
				pShoulderArmor->setObjectID(pResult->getDWORD(++i));
				pShoulderArmor->setItemType(pResult->getDWORD(++i));
	
				if (g_pShoulderArmorInfoManager->getItemInfo(pShoulderArmor->getItemType())->isUnique())
					pShoulderArmor->setUnique();

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pShoulderArmor->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
				string thirdoptionField = pResult->getString(++i);
				list<OptionType_t> thirdoptionTypes;
				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
				pShoulderArmor->setThirdOptionType(thirdoptionTypes);
#endif
				pShoulderArmor->setDurability(pResult->getInt(++i));
				pShoulderArmor->setGrade(pResult->getInt(++i));
				pShoulderArmor->setEnchantLevel(pResult->getInt(++i));
				pShoulderArmor->setCreateType((Item::CreateType)pResult->getInt(++i));

				string GTwooptionField = pResult->getString(++i);
				list<OptionType_t> GTwooptionTypes;
				setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
				pShoulderArmor->setGTwoOptionType(GTwooptionTypes);

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
								processItemBugEx(pCreature, pShoulderArmor);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pShoulderArmor))
						{
							pInventory->addItemEx(x, y, pShoulderArmor);
						}
						else
						{
							processItemBugEx(pCreature, pShoulderArmor);
						}
						break;

					case STORAGE_GEAR:
						if (pCreature->isSlayer())
						{
							if ( y == pSlayer->getCurrentGearSlotID() )
							{
								if (!pSlayer->isWear((Slayer::WearPart)x))
								{
									pSlayer->wearItem((Slayer::WearPart)x, pShoulderArmor);
								}
								else
								{
									processItemBugEx(pCreature, pShoulderArmor);
								}
							}
							else
							{
								pSlayer->putItemToGear(x, y, pShoulderArmor);
							}
						}
						else if (pCreature->isVampire())
						{
							processItemBugEx(pCreature, pShoulderArmor);
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pShoulderArmor);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pShoulderArmor);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pShoulderArmor);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pShoulderArmor);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pShoulderArmor);
						}
						else pStash->insert(x, y, pShoulderArmor);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pShoulderArmor);
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
void ShoulderArmorLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to inventory
//--------------------------------------------------------------------------------
void ShoulderArmorLoader::load(StorageID_t storageID, Inventory* pInventory) 
	throw(Error)
{
	__BEGIN_TRY

	__END_CATCH
}

ShoulderArmorLoader* g_pShoulderArmorLoader = NULL;
