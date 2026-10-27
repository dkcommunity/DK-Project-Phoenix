//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersRing.cpp
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "OustersRing.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "ItemInfoManager.h"
#include "Stash.h"
#include "ItemUtil.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

// global variable declaration
OustersRingInfoManager* g_pOustersRingInfoManager = NULL;

ItemID_t OustersRing::m_ItemIDRegistry = 0;
Mutex    OustersRing::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
OustersRing::OustersRing()
	throw()
//: m_ItemType(0), m_Durability(0)
{
	setItemType(0);
	setDurability(0);
	//m_EnchantLevel = 0;
}

OustersRing::OustersRing(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
//: m_ItemType(itemType), m_OptionType(optionType), m_Durability(0)
{
	setItemType(itemType);
	setOptionType(optionType);
//	m_EnchantLevel = 0;

	setDurability(computeMaxDurability(this));

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList()))
	{
		filelog("itembug.log", "OustersRing::OustersRing() : Invalid item type or option type");
		throw ("OustersRing::OustersRing() : Invalid item type or optionType");
	}
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void OustersRing::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO OustersRingObject "
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
void OustersRing::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
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
	
	os << "UPDATE OustersRingObject SET "
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
string OustersRing::toString() const 
	throw()
{
	StringStream msg;

	msg << "OustersRing("
		<< "ItemID:"        << m_ItemID
		<< ",ItemType:"     <<(int)getItemType()
		<< ",OptionType:"   <<getOptionTypeToString(getOptionTypeList()).c_str()
		<< ",Durability:"   <<(int)getDurability()
		<< ",EnchantLevel:" <<(int)getEnchantLevel()
		<< ")";

	return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t OustersRing::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t OustersRing::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t OustersRing::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t OustersRing::getDefenseBonus() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

	__END_CATCH
}
Protection_t OustersRing::getProtectionBonus() const
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersRingInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();
	
	__END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string OustersRingInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "OustersRingInfo("
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
void OustersRingInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM OustersRingInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

         // 20080703 마법 방어 추가
        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, Durability, Defense, Protection, MagicProtection, ReqAbility, ItemLevel, DefaultOption, UpgradeRatio, UpgradeCrashPercent, NextOptionRatio, NextItemType, DowngradeRatio FROM OustersRingInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			OustersRingInfo* pOustersRingInfo = new OustersRingInfo();

			pOustersRingInfo->setItemType(pResult->getInt(++i));
			pOustersRingInfo->setName(pResult->getString(++i));
			pOustersRingInfo->setEName(pResult->getString(++i));
			pOustersRingInfo->setPrice(pResult->getInt(++i));
			pOustersRingInfo->setVolumeType(pResult->getInt(++i));
			pOustersRingInfo->setWeight(pResult->getInt(++i));
			pOustersRingInfo->setRatio(pResult->getInt(++i));
			pOustersRingInfo->setDurability(pResult->getInt(++i));
			pOustersRingInfo->setDefenseBonus(pResult->getInt(++i));
			pOustersRingInfo->setProtectionBonus(pResult->getInt(++i));
		 	pOustersRingInfo->setMagicProtectionBonus(pResult->getInt(++i)); // 20080703 마법 방어 추가
			pOustersRingInfo->setReqAbility(pResult->getString(++i));
			pOustersRingInfo->setItemLevel(pResult->getInt(++i));
			pOustersRingInfo->setDefaultOptions(pResult->getString(++i));
			pOustersRingInfo->setUpgradeRatio(pResult->getInt(++i));
			pOustersRingInfo->setUpgradeCrashPercent(pResult->getInt(++i));
			pOustersRingInfo->setNextOptionRatio(pResult->getInt(++i));
			pOustersRingInfo->setNextItemType(pResult->getInt(++i));
			pOustersRingInfo->setDowngradeRatio(pResult->getInt(++i));

			addItemInfo(pOustersRingInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void OustersRingLoader::load(Creature* pCreature) 
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

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, "
			<< "OptionType, Durability, EnchantLevel FROM OustersRingObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/
#ifdef VERSION_THIRD_ENCHANT_1
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, ThirdOptionType, Durability, Grade, EnchantLevel, ItemFlag,GTwoOptionType FROM OustersRingObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
								pCreature->getName().c_str() );
#else
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Durability, Grade, EnchantLevel, ItemFlag FROM OustersRingObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
			pCreature->getName().c_str() );
#endif

		while (pResult->next())
		{
			try {
				uint i = 0;

				OustersRing* pOustersRing = new OustersRing();

				pOustersRing->setItemID(pResult->getDWORD(++i));
				pOustersRing->setObjectID(pResult->getDWORD(++i));
				pOustersRing->setItemType(pResult->getDWORD(++i));
	
				if (g_pOustersRingInfoManager->getItemInfo(pOustersRing->getItemType())->isUnique())
					pOustersRing->setUnique();

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pOustersRing->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
				string thirdoptionField = pResult->getString(++i);
				list<OptionType_t> thirdoptionTypes;
				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
				pOustersRing->setThirdOptionType(thirdoptionTypes);
#endif

				pOustersRing->setDurability(pResult->getInt(++i));
				pOustersRing->setGrade(pResult->getInt(++i));
				pOustersRing->setEnchantLevel(pResult->getInt(++i));
				pOustersRing->setCreateType((Item::CreateType)pResult->getInt(++i));

				string GTwooptionField = pResult->getString(++i);
				list<OptionType_t> GTwooptionTypes;
				setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
				pOustersRing->setGTwoOptionType(GTwooptionTypes);

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*	pOusters		= NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
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
							CoordInven_t X, Y;
							SubInventory* pInventoryItem = dynamic_cast<SubInventory*>(findItemIID( pCreature, storageID ));
							if ( pInventoryItem == NULL )
							{
								processItemBugEx(pCreature, pOustersRing);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pOustersRing))
						{
							pInventory->addItemEx(x, y, pOustersRing);
						}
						else
						{
							processItemBugEx(pCreature, pOustersRing);
						}
						break;

					case STORAGE_GEAR:
						if (pCreature->isSlayer() || pCreature->isVampire())
						{
							processItemBugEx(pCreature, pOustersRing);
						}
						else if (pCreature->isOusters())
						{
							if ( y == pOusters->getCurrentGearSlotID() )
							{
								if (!pOusters->isWear((Ousters::WearPart)x))
								{
									pOusters->wearItem((Ousters::WearPart)x, pOustersRing);
								}
								else
								{
									processItemBugEx(pCreature, pOustersRing);
								}
							}
							else
							{
								pOusters->putItemToGear(x, y, pOustersRing);
							}
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pOustersRing);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pOustersRing);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pOustersRing);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pOustersRing);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pOustersRing);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pOustersRing);
						}
						else pStash->insert(x, y, pOustersRing);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pOustersRing);
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
void OustersRingLoader::load(Zone* pZone) 
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
			<< " OptionType, ThirdOptionType, Durability, EnchantLevel, ItemFlag,GTwoOptionType FROM OustersRingObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
#else
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< " OptionType, Durability, EnchantLevel, ItemFlag FROM OustersRingObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
#endif
		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			OustersRing* pOustersRing = new OustersRing();

			pOustersRing->setItemID(pResult->getInt(++i));
			pOustersRing->setObjectID(pResult->getInt(++i));
			pOustersRing->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			string optionField = pResult->getString(++i);
			list<OptionType_t> optionTypes;
			setOptionTypeFromField(optionTypes, optionField);
			pOustersRing->setOptionType(optionTypes);
#ifdef VERSION_THIRD_ENCHANT_1
			string thirdoptionField = pResult->getString(++i);
			list<OptionType_t> thirdoptionTypes;
			setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
			pOustersRing->setThirdOptionType(thirdoptionTypes);
#endif

			pOustersRing->setDurability(pResult->getInt(++i));
			pOustersRing->setEnchantLevel(pResult->getInt(++i));
			pOustersRing->setCreateType((Item::CreateType)pResult->getInt(++i));

			string GTwooptionField = pResult->getString(++i);
			list<OptionType_t> GTwooptionTypes;
			setOptionTypeFromField(GTwooptionTypes, GTwooptionField);
			pOustersRing->setGTwoOptionType(GTwooptionTypes);

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pOustersRing);
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
void OustersRingLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

OustersRingLoader* g_pOustersRingLoader = NULL;
