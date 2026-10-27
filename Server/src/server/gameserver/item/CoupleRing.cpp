//////////////////////////////////////////////////////////////////////////////
// Filename    : CoupleRing.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CoupleRing.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
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

CoupleRingInfoManager* g_pCoupleRingInfoManager = NULL;

ItemID_t CoupleRing::m_ItemIDRegistry = 0;
Mutex    CoupleRing::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class CoupleRing member methods
//////////////////////////////////////////////////////////////////////////////

CoupleRing::CoupleRing()
	throw()
{
	m_ItemType = 0;
}

CoupleRing::CoupleRing(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
: m_ItemType(itemType), m_OptionType(optionType)
{
	__BEGIN_TRY

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "CoupleRing::CoupleRing() : Invalid item type or option type");
		throw ("CoupleRing::CoupleRing() : Invalid item type or optionType");
	}

	__END_CATCH
}

void CoupleRing::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO CoupleRingObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, OptionType, Name, PartnerItemID)"
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
		<< getName().c_str() << "', " 
		<< getPartnerItemID() << ")"
		
		<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")"; 
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		

	__END_CATCH
}

void CoupleRing::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE CoupleRingObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << m_ItemType
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", Name = '" << getName().c_str()
		<< "', PartnerItemID=" << getPartnerItemID()
		<< " WHERE ItemID=" << m_ItemID;
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

string CoupleRing::toString() const 
	throw()
{
	StringStream msg;
	msg << "CoupleRing("
		<< "ItemID:" << m_ItemID
		<< ",ItemType:" << m_ItemType
		<< ")";
	return msg.toString();
}

VolumeWidth_t CoupleRing::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCoupleRingInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t CoupleRing::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCoupleRingInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t CoupleRing::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pCoupleRingInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}

bool CoupleRing::hasPartnerItem()
	throw(Error)
{
	__BEGIN_TRY

	bool bRet = false;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery(
				"SELECT count(*) from CoupleRingObject where ItemID=%ld and Storage IN(0, 1, 2, 3, 4, 9)", getPartnerItemID() );

		// UPDATE인 경우는 Result* 대신에.. pStmt->getAffectedRowCount()

		if (pResult->next()) 
		{
			int	count = pResult->getInt(1);

			// 위험!
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
// class CoupleRingInfo member methods
//////////////////////////////////////////////////////////////////////////////

string CoupleRingInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "CoupleRingInfo("
		<< "ItemType:" << m_ItemType
		<< ",Name:" << getName().c_str()
		<< ",EName:" << m_EName
		<< ",Price:" << m_Price
		<< ",VolumeType:" << Volume2String[m_VolumeType]
		<< ",Weight:" << m_Weight
		<< ",Description:" << m_Description
		<< ")";
	return msg.toString();
}

void CoupleRingInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM CoupleRingInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM CoupleRingInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			CoupleRingInfo* pCoupleRingInfo = new CoupleRingInfo();

			pCoupleRingInfo->setItemType(pResult->getInt(++i));
			pCoupleRingInfo->setName(pResult->getString(++i));
			pCoupleRingInfo->setEName(pResult->getString(++i));
			pCoupleRingInfo->setPrice(pResult->getInt(++i));
			pCoupleRingInfo->setVolumeType(pResult->getInt(++i));
			pCoupleRingInfo->setWeight(pResult->getInt(++i));
			pCoupleRingInfo->setRatio(pResult->getInt(++i));

			addItemInfo(pCoupleRingInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class CoupleRingLoader member methods
//////////////////////////////////////////////////////////////////////////////

void CoupleRingLoader::load(Creature* pCreature) 
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

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM CoupleRingObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/

		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, OptionType, Name, PartnerItemID FROM CoupleRingObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );

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

				CoupleRing* pCoupleRing = new CoupleRing();

				pCoupleRing->setItemID(pResult->getDWORD(++i));
				pCoupleRing->setObjectID(pResult->getDWORD(++i));
				pCoupleRing->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				string optionField = pResult->getString(++i);
				list<OptionType_t> optionTypes;
				setOptionTypeFromField( optionTypes, optionField );
				pCoupleRing->setOptionType( optionTypes );

				pCoupleRing->setName( pResult->getString(++i) );
				pCoupleRing->setPartnerItemID( pResult->getDWORD(++i) );

				// 파트너 아이템이 없거나 더 이상 커플이 아니면 아이템을 지워준다.
				if ( pPC != NULL && ( !g_pCoupleManager->isCouple( pPC, pCoupleRing->getName() ) || !pCoupleRing->hasPartnerItem() ) ) 
				{
					g_pCoupleManager->removeCoupleForce( pPC, pCoupleRing->getName() );
					//pCoupleRing->destroy();
					char sql[30];
					sprintf( sql, "Storage = 10" );
					pCoupleRing->tinysave( sql );
					SAFE_DELETE( pCoupleRing );

					// FlagSet 도 날려준다.
					pPC->getFlagSet()->turnOff( FLAGSET_IS_COUPLE );
					pPC->getFlagSet()->save( pPC->getName() );
					continue;
				}

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
								processItemBugEx(pCreature, pCoupleRing);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pCoupleRing))
						{
							pInventory->addItemEx(x, y, pCoupleRing);
						}
						else
						{
							processItemBugEx(pCreature, pCoupleRing);
						}
						break;

					case STORAGE_GEAR:
						if (pCreature->isSlayer())
						{
							if ( y == pSlayer->getCurrentGearSlotID() )
							{
								if (!pSlayer->isWear((Slayer::WearPart)x))
								{
									pSlayer->wearItem((Slayer::WearPart)x, pCoupleRing);
								}
								else
								{
									processItemBugEx(pCreature, pCoupleRing);
								}
							}
							else
							{
								pSlayer->putItemToGear(x, y, pCoupleRing);
							}
						}
						else if ( pCreature->isVampire() )
						{
							if ( y == pVampire->getCurrentGearSlotID() )
							{
								if (!pVampire->isWear((Vampire::WearPart)x))
								{
										pVampire->wearItem((Vampire::WearPart)x, pCoupleRing);
								}
								else
								{
									processItemBugEx(pCreature, pCoupleRing);
								}
							}
							else
							{
								pVampire->putItemToGear(x, y, pCoupleRing);
							}
						}
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pCoupleRing);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pCoupleRing);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pCoupleRing);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pCoupleRing);
						break;

					case STORAGE_STASH:
						processItemBugEx(pCreature, pCoupleRing);
				/*		if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pCoupleRing);
						}
						else pStash->insert(x, y, pCoupleRing); */
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pCoupleRing);
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

void CoupleRingLoader::load(Zone* pZone) 
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

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y FROM CoupleRingObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			CoupleRing* pCoupleRing = new CoupleRing();

			pCoupleRing->setItemID(pResult->getInt(++i));
			pCoupleRing->setObjectID(pResult->getInt(++i));
			pCoupleRing->setItemType(pResult->getInt(++i));

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
						pTile.addItem(pCoupleRing);
					}
					break;

				case STORAGE_STASH :
				case STORAGE_CORPSE :
					SAFE_DELETE(pStmt);
					throw UnsupportedError("상자 및 시체안의 아이템의 저장은 아직 지원되지 않습니다.");

				default :
					SAFE_DELETE(pStmt);
					throw Error("Storage must be STORAGE_ZONE");
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

void CoupleRingLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

CoupleRingLoader* g_pCoupleRingLoader = NULL;
