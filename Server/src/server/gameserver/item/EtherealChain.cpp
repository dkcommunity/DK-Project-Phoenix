//////////////////////////////////////////////////////////////////////////////
// Filename    : EtherealChain.cpp
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EtherealChain.h"
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
EtherealChainInfoManager* g_pEtherealChainInfoManager = NULL;

ItemID_t EtherealChain::m_ItemIDRegistry = 0;
Mutex    EtherealChain::m_Mutex;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
EtherealChain::EtherealChain()
	throw()
//: m_ItemType(0), m_Durability(0)
{
	setItemType(0);
}

EtherealChain::EtherealChain(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
{
	setItemType(itemType);
	setOptionType(optionType);

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList()))
	{
		filelog("itembug.log", "EtherealChain::EtherealChain() : Invalid item type or option type");
		throw ("EtherealChain::EtherealChain() : Invalid item type or optionType");
	}
}


//--------------------------------------------------------------------------------
// create item
//--------------------------------------------------------------------------------
void EtherealChain::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	ostringstream os;
	
	os << "INSERT INTO EtherealChainObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID ,"
		<< " X, Y, OptionType, Grade, ItemFlag)"
		<< " VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< getItemType() << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", " 
		<< storageID << ", " 
		<<(int)x << ", " 
		<<(int)y << ", '" 
		<< optionField.c_str() << "', " 
		<< getGrade() << ", " 
		<< (int)m_CreateType << ")"
	
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// save item
//--------------------------------------------------------------------------------
void EtherealChain::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY


	string optionField;
	setOptionTypeToField( getOptionTypeList(), optionField );
	
	ostringstream os;
	
	os << "UPDATE EtherealChainObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << getItemType()
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", OptionType='" << optionField.c_str()
		<< "', Grade=" << getGrade()
		<< " WHERE ItemID=" << m_ItemID
	
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());	
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string EtherealChain::toString() const 
	throw()
{
	StringStream msg;

	msg << "EtherealChain("
		<< "ItemID:"        << m_ItemID
		<< ",ItemType:"     <<(int)getItemType()
		<< ",OptionType:"   <<getOptionTypeToString(getOptionTypeList()).c_str()
		<< ")";

	return msg.toString();
}


/*//--------------------------------------------------------------------------------
// get width
//--------------------------------------------------------------------------------
VolumeWidth_t EtherealChain::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEtherealChainInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get height
//--------------------------------------------------------------------------------
VolumeHeight_t EtherealChain::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEtherealChainInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}

	
//--------------------------------------------------------------------------------
// get weight
//--------------------------------------------------------------------------------
Weight_t EtherealChain::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEtherealChainInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get/set armor's Defense Bonus
//--------------------------------------------------------------------------------
Defense_t EtherealChain::getDefenseBonus() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEtherealChainInfoManager->getItemInfo(m_ItemType)->getDefenseBonus();

	__END_CATCH
}
Protection_t EtherealChain::getProtectionBonus() const
	throw(Error)
{
	__BEGIN_TRY

	return g_pEtherealChainInfoManager->getItemInfo(m_ItemType)->getProtectionBonus();
	
	__END_CATCH
}
*/

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string EtherealChainInfo::toString() const 
	throw()
{
	StringStream msg;

	msg << "EtherealChainInfo("
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
void EtherealChainInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM EtherealChainInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio,DefaultOption FROM EtherealChainInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			EtherealChainInfo* pEtherealChainInfo = new EtherealChainInfo();

			pEtherealChainInfo->setItemType(pResult->getInt(++i));
			pEtherealChainInfo->setName(pResult->getString(++i));
			pEtherealChainInfo->setEName(pResult->getString(++i));
			pEtherealChainInfo->setPrice(pResult->getInt(++i));
			pEtherealChainInfo->setVolumeType(pResult->getInt(++i));
			pEtherealChainInfo->setWeight(pResult->getInt(++i));
			pEtherealChainInfo->setRatio(pResult->getInt(++i));
			pEtherealChainInfo->setDefaultOptions(pResult->getString(++i));

			addItemInfo(pEtherealChainInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// load to creature
//--------------------------------------------------------------------------------
void EtherealChainLoader::load(Creature* pCreature) 
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
			<< " OptionType, Durability, EnchantLevel FROM EtherealChainObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/
//#ifdef VERSION_THIRD_ENCHANT_1
//		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, ThirdOptionType, Grade, ItemFlag FROM EtherealChainObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
//								pCreature->getName().c_str() );
//#else
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Grade, ItemFlag FROM EtherealChainObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
								pCreature->getName().c_str() );
//#endif

		while (pResult->next())
		{
			try {
				uint i = 0;

				EtherealChain* pEtherealChain = new EtherealChain();

				pEtherealChain->setItemID(pResult->getDWORD(++i));
				pEtherealChain->setObjectID(pResult->getDWORD(++i));
				pEtherealChain->setItemType(pResult->getDWORD(++i));
	
				if (g_pEtherealChainInfoManager->getItemInfo(pEtherealChain->getItemType())->isUnique())
					pEtherealChain->setUnique();

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pEtherealChain->setOptionType(optionTypes);
//#ifdef VERSION_THIRD_ENCHANT_1
//				string thirdoptionField = pResult->getString(++i);
//				list<OptionType_t> thirdoptionTypes;
//				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
//				pEtherealChain->setThirdOptionType(thirdoptionTypes);
//#endif

				pEtherealChain->setGrade(pResult->getInt(++i));
				pEtherealChain->setCreateType((Item::CreateType)pResult->getInt(++i));

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
								processItemBugEx(pCreature, pEtherealChain);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pEtherealChain))
						{
							pInventory->addItemEx(x, y, pEtherealChain);
						}
						else
						{
							processItemBugEx(pCreature, pEtherealChain);
						}
						break;

					case STORAGE_GEAR:
						{
							PlayerCreature *pPlayerCreature = dynamic_cast<PlayerCreature *>(pCreature);

							if ( y == pPlayerCreature->getCurrentGearSlotID() )
							{
								if (!pPlayerCreature->isWear(x))
								{
									pPlayerCreature->wearItem(x, pEtherealChain);
								}
								else
								{
									processItemBugEx(pCreature, pEtherealChain);
								}
							}
							else
							{
								pPlayerCreature->putItemToGear(x, y, pEtherealChain);
							}
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pEtherealChain);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pEtherealChain);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pEtherealChain);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pEtherealChain);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pEtherealChain);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pEtherealChain);
						}
						else pStash->insert(x, y, pEtherealChain);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pEtherealChain);
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
void EtherealChainLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;
//#ifdef VERSION_THIRD_ENCHANT_1
//		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
//			<< " OptionType, ThirdOptionType, ItemFlag FROM EtherealChainObject"
//			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
//#else
		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y,"
			<< " OptionType, ItemFlag FROM EtherealChainObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();
//#endif

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			EtherealChain* pEtherealChain = new EtherealChain();

			pEtherealChain->setItemID(pResult->getInt(++i));
			pEtherealChain->setObjectID(pResult->getInt(++i));
			pEtherealChain->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			string optionField = pResult->getString(++i);
			list<OptionType_t> optionTypes;
			setOptionTypeFromField(optionTypes, optionField);
			pEtherealChain->setOptionType(optionTypes);
//#ifdef VERSION_THIRD_ENCHANT_1
//				string thirdoptionField = pResult->getString(++i);
//				list<OptionType_t> thirdoptionTypes;
//				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
//				pEtherealChain->setThirdOptionType(thirdoptionTypes);
//#endif

			pEtherealChain->setCreateType((Item::CreateType)pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pEtherealChain);
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
void EtherealChainLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

EtherealChainLoader* g_pEtherealChainLoader = NULL;
