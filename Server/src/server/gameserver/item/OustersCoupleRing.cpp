//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersCoupleRing.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "OustersCoupleRing.h"
#include "DB.h"
#include "Slayer.h"
#include "Ousters.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "Stash.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "PlayerCreature.h"
#include "FlagSet.h"
#include "couple/CoupleManager.h"

#include <stdio.h>
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

OustersHarmonicPendentInfoManager* g_pOustersHarmonicPendentInfoManager = NULL;

ItemID_t OustersHarmonicPendent::m_ItemIDRegistry = 0;
Mutex    OustersHarmonicPendent::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class OustersHarmonicPendent member methods
//////////////////////////////////////////////////////////////////////////////

OustersHarmonicPendent::OustersHarmonicPendent()
	throw()
{
	m_ItemType = 0;
}

OustersHarmonicPendent::OustersHarmonicPendent(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
: m_ItemType(itemType), m_OptionType( optionType )
{
	__BEGIN_TRY

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "OustersHarmonicPendent::OustersHarmonicPendent() : Invalid item type or option type");
		throw ("OustersHarmonicPendent::OustersHarmonicPendent() : Invalid item type or optionType");
	}

	__END_CATCH
}

void OustersHarmonicPendent::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	string thirdoptionField;
	setOptionTypeToField( getThirdOptionTypeList(), thirdoptionField );
	
	ostringstream os;
	
	os << "INSERT INTO OustersHarmonicPendentObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, OptionType, ThirdOptionType, Name, PartnerItemID)"
		<< " VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< m_ItemType << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", "
		<< storageID << ", " 
		<<(int)x << ", " 
		<<(int)y << ", '" 
		<< optionField.c_str() << "', '" 
		<< thirdoptionField.c_str() 
		<< "', '" << getName().c_str() 
		<< "', " << getPartnerItemID() << ")"
		
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		

	__END_CATCH
}
	
void OustersHarmonicPendent::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	string optionField;
	setOptionTypeToField( getOptionTypeList(), optionField );
	
	string thirdoptionField;
	setOptionTypeToField( getThirdOptionTypeList(), thirdoptionField );
	
	ostringstream os;
	
	os << "UPDATE OustersHarmonicPendentObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", Name='" << getName().c_str()
		<< "', PartnerItemID=" << getPartnerItemID()
		<< " WHERE ItemID=" << m_ItemID
	
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());			
	
	__END_CATCH
}

string OustersHarmonicPendent::toString() const 
	throw()
{
	StringStream msg;
	msg << "OustersHarmonicPendent("
		<< "ItemID:" << m_ItemID
		<< ",ItemType:" << m_ItemType
		<< ")";
	return msg.toString();
}

VolumeWidth_t OustersHarmonicPendent::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersHarmonicPendentInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t OustersHarmonicPendent::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersHarmonicPendentInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t OustersHarmonicPendent::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pOustersHarmonicPendentInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}

bool OustersHarmonicPendent::hasPartnerItem()
	throw(Error)
{
	__BEGIN_TRY

	bool bRet = false;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery(
				"SELECT count(*) from OustersHarmonicPendentObject where ItemID=%ld and Storage IN(0, 1, 2, 3, 4, 9)", getPartnerItemID() );

		// UPDATE인 경우는 Result* 대신에.. pStmt->getAffectedRowCount()

		if (pResult->next()) 
		{
			int	count = pResult->getInt(1);

			Assert( count >= 0 );
			Assert( count <= 1 );

			if ( count == 1 ) bRet = true;
		}
		else
		{
			bRet = false;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return bRet;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class OustersHarmonicPendentInfo member methods
//////////////////////////////////////////////////////////////////////////////

string OustersHarmonicPendentInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "OustersHarmonicPendentInfo("
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

void OustersHarmonicPendentInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM OustersHarmonicPendentInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM OustersHarmonicPendentInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			OustersHarmonicPendentInfo* pOustersHarmonicPendentInfo = new OustersHarmonicPendentInfo();

			pOustersHarmonicPendentInfo->setItemType(pResult->getInt(++i));
			pOustersHarmonicPendentInfo->setName(pResult->getString(++i));
			pOustersHarmonicPendentInfo->setEName(pResult->getString(++i));
			pOustersHarmonicPendentInfo->setPrice(pResult->getInt(++i));
			pOustersHarmonicPendentInfo->setVolumeType(pResult->getInt(++i));
			pOustersHarmonicPendentInfo->setWeight(pResult->getInt(++i));
			pOustersHarmonicPendentInfo->setRatio(pResult->getInt(++i));

			addItemInfo(pOustersHarmonicPendentInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class OustersHarmonicPendentLoader member methods
//////////////////////////////////////////////////////////////////////////////

void OustersHarmonicPendentLoader::load(Creature* pCreature) 
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

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM OustersHarmonicPendentObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/
#ifdef VERSION_THIRD_ENCHANT_1
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, ThirdOptionType, Name, PartnerItemID FROM OustersHarmonicPendentObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );
#else
		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Name, PartnerItemID FROM OustersHarmonicPendentObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );
#endif

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>( pCreature );
		if ( pResult->getRowCount() == 0 && pPC->getFlagSet()->isOn( FLAGSET_IS_COUPLE ) )
		{
			pPC->getFlagSet()->turnOff( FLAGSET_IS_COUPLE );
			pPC->getFlagSet()->save( pPC->getName() );

			g_pCoupleManager->removeCoupleForce( pPC );
		}

		while (pResult->next())
		{
			try {
				uint i = 0;

				OustersHarmonicPendent* pOustersHarmonicPendent = new OustersHarmonicPendent();

				pOustersHarmonicPendent->setItemID(pResult->getDWORD(++i));
				pOustersHarmonicPendent->setObjectID(pResult->getDWORD(++i));
				pOustersHarmonicPendent->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField( optionTypes, optionField );
				pOustersHarmonicPendent->setOptionType( optionTypes );
#ifdef VERSION_THIRD_ENCHANT_1
				string thirdoptionField = pResult->getString(++i);
				list<OptionType_t> thirdoptionTypes;
				setOptionTypeFromField(thirdoptionTypes, thirdoptionField);
				pOustersHarmonicPendent->setThirdOptionType(thirdoptionTypes);
#endif
				pOustersHarmonicPendent->setName( pResult->getString(++i) );
				pOustersHarmonicPendent->setPartnerItemID( pResult->getDWORD(++i) );

				// 파트너 아이템이 없거나 더 이상 커플이 아니면 아이템을 지워준다.
				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
//				if ( !pOustersHarmonicPendent->hasPartnerItem() )
//				if ( pPC != NULL && !g_pCoupleManager->isCouple( pPC, pOustersHarmonicPendent->getName() ) )
				if ( pPC != NULL && ( !g_pCoupleManager->isCouple( pPC, pOustersHarmonicPendent->getName() ) || !pOustersHarmonicPendent->hasPartnerItem() ) ) 
				{
					g_pCoupleManager->removeCoupleForce( pPC, pOustersHarmonicPendent->getName() );
					//pOustersHarmonicPendent->destroy();
					char sql[30];
					sprintf( sql, "Storage = 10" );
					pOustersHarmonicPendent->tinysave(sql );
					SAFE_DELETE( pOustersHarmonicPendent );

					// FlagSet 도 날려준다.
					pPC->getFlagSet()->turnOff( FLAGSET_IS_COUPLE );
					pPC->getFlagSet()->save( pPC->getName() );
					continue;
				}

				Inventory*  pInventory      = NULL;
				Ousters*    pOusters        = NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
				//Item*       pItem           = NULL;
				Stash*      pStash          = NULL;
				//Belt*       pBelt           = NULL;
				//Inventory*  pBeltInventory  = NULL;

				if (pCreature->isOusters()) 
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
								processItemBugEx(pCreature, pOustersHarmonicPendent);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pOustersHarmonicPendent))
						{
							pInventory->addItemEx(x, y, pOustersHarmonicPendent);
						}
						else
						{
							processItemBugEx(pCreature, pOustersHarmonicPendent);
						}
						break;

					case STORAGE_GEAR:
						if ( pCreature->isOusters() )
						{
							if ( y == pOusters->getCurrentGearSlotID() )
							{
								if (!pOusters->isWear((Ousters::WearPart)x))
								{
									pOusters->wearItem((Ousters::WearPart)x, pOustersHarmonicPendent);
								}
								else
								{
									processItemBugEx(pCreature, pOustersHarmonicPendent);
								}
							}
							else
							{
								pOusters->putItemToGear(x, y, pOustersHarmonicPendent);
							}
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pOustersHarmonicPendent);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pOustersHarmonicPendent);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pOustersHarmonicPendent);
						break;

					case STORAGE_STASH:
						processItemBugEx(pCreature, pOustersHarmonicPendent);
				/*		if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pOustersHarmonicPendent);
						}
						else pStash->insert(x, y, pOustersHarmonicPendent); */
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pOustersHarmonicPendent);
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

void OustersHarmonicPendentLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	cout << "존에 떨어진 아이템 로드는 지원하지 않습니다." << endl;
	Assert( false );

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM OustersHarmonicPendentObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			OustersHarmonicPendent* pOustersHarmonicPendent = new OustersHarmonicPendent();

			pOustersHarmonicPendent->setItemID(pResult->getInt(++i));
			pOustersHarmonicPendent->setObjectID(pResult->getInt(++i));
			pOustersHarmonicPendent->setItemType(pResult->getInt(++i));

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
						pTile.addItem(pOustersHarmonicPendent);
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

void OustersHarmonicPendentLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

OustersHarmonicPendentLoader* g_pOustersHarmonicPendentLoader = NULL;

