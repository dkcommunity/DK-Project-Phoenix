//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectItem.h"
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

EffectItemInfoManager* g_pEffectItemInfoManager = NULL;

ItemID_t EffectItem::m_ItemIDRegistry = 0;
Mutex    EffectItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class EffectItem member methods
//////////////////////////////////////////////////////////////////////////////

EffectItem::EffectItem()
	throw()
{
	setItemType(0);
}

EffectItem::EffectItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num)
	throw()
{
	setItemType(itemType);
	setNum(Num);

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), getItemType(), getOptionTypeList()))
	{
		filelog("itembug.log", "EffectItem::EffectItem() : Invalid item type or option type");
		throw ("EffectItem::EffectItem() : Invalid item type or optionType");
	}
}

void EffectItem::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
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
	
	os << "INSERT INTO EffectItemObject "
		<< "(ItemID,  ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, Num, ItemFlag) VALUES(" 
		<< m_ItemID << ", "
		<< m_ObjectID << ", " 
		<< getItemType() << ", '" 
		<< ownerID << "', " 
		<<(int)storage << ", " 
		<< storageID << ", " 
		<< (int)x << ", " 
		<< (int)y << ", " 
		<< (int)getNum() << ", " 
		<< (int)m_CreateType << ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}

void EffectItem::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE EffectItemObject SET "
		<< "ObjectID=" << m_ObjectID
		<< ", ItemType=" << getItemType()
		<< ", OwnerID='" << ownerID.c_str()
		<< "', Storage=" << (int)storage
		<< ", StorageID=" << storageID
		<< ", X=" << (int)x
		<< ", Y=" << (int)y
		<< ", Num=" << (int)getNum()
		<< " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

string EffectItem::toString() const 
	throw()
{
	StringStream msg;

	msg << "EffectItem("
		<< "ItemID:"    << m_ItemID
		<< ",ItemType:" <<(int)getItemType()
		<< ",Num:"      <<(int)getNum()
		<< ")";

	return msg.toString();
}

/*VolumeWidth_t EffectItem::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEffectItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t EffectItem::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEffectItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t EffectItem::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pEffectItemInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}
*/
//////////////////////////////////////////////////////////////////////////////
// class EffectItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string EffectItemInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "EffectItemInfo("
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

// 20070724
bool EffectItemInfoManager::isForceScroll(ItemType_t itemType)
{
	switch(itemType)
	{
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 32:
		case 33:
		// 20070911 추석 이벤트 
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
//20081127 wlzzi - 프리미엄 카드 추가
		case 39: case 40: case 41: case 55: case 56: case 57: case 58:
/*		
  		//20080926 PREMIUM SERVICE 20080204 Kim Sung Yong - 30DAY CARD
		case 39:
        //20080926 PREMIUM SERVICE 20080204 Kim Sung Yong - 1DAY CARD
        case 40:
*/
			
		// 20070214 Moon Clock(Pay Item) 81Lv ~ 120 Lv
		case 42:		//1 day
		case 43:		//4 day
		case 44:		//7 day
		// 20070214 Moon Clock
		// 20070310 계급경험치모래시계 by Kim sung yong
		case 45:		//1 day
		case 46:		//1 day
		// 20070310 계급경험치모래시계
		case 47:		// 20080602 경험의 돌
//		case 48:		// 20081029 wlzzi - Extreme Force Scroll
		case 54:		// 1215 wlzzi - Extreme Force Scroll의 ItemType 변경 // 일본서비스와의 통합을 위함. ( 48 --> 54 )	
			return true;
			break;
		case 59 : // 하급 경험의 모래시계 라이트
		case 60 : // 중급 경험의 모래시계 라이트
		case 61 : // 상급 경험의 모래시계 라이트
		case 62 : // 하급 경험의 모래시계 미디움
		case 63 : // 중급 경험의 모래시계 미디움
		case 64 : // 상급 경험의 모래시계 미디움
			return true;
			break;
		case 65 : // 
		case 66 : //
		case 67 : // 
		case 68 : // 
		case 69 : // 
		case 70 : // 
		case 71 : // 
		case 72 : // 
		case 73 : // 		
			return true;
			break;
		case 85 : // 
		case 86 : //	
		case 89 : 
		case 90 : 
			return true;
			break;
		default:
			return false;
			break;
	}
	return false;
}

// 2007 07 25 중복 가능하지 않은 이펙트를 찾는다.
bool EffectItemInfoManager::isNotDuplicate(EffectID_t effectID)
{
	switch(effectID)
	{
		case Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL:
		case Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL:
		case Effect::EFFECT_CLASS_CARNELIAN_FORCE_SCROLL:
//		case Effect::EFFECT_CLASS_ODIN_FORCE_SCROLL:		//1110 - wlzzi 오딘 은 중첩체크 대상제외
//		case Effect::EFFECT_CLASS_TITUOS_FORCE_SCROLL:		//1110 - wlzzi 티투오스도 중첩체크 대상제외
			return true;
		default:
			return false;
	}
	return false;
}

void EffectItemInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM EffectItemInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, EffectClass, TimeSec FROM EffectItemInfo"
		);

		while (pResult->next()) 
		{
			uint i = 0;

			EffectItemInfo* pEffectItemInfo = new EffectItemInfo();

			pEffectItemInfo->setItemType(pResult->getInt(++i));
			pEffectItemInfo->setName(pResult->getString(++i));
			pEffectItemInfo->setEName(pResult->getString(++i));
			pEffectItemInfo->setPrice(pResult->getInt(++i));
			pEffectItemInfo->setVolumeType(pResult->getInt(++i));
			pEffectItemInfo->setWeight(pResult->getInt(++i));
			pEffectItemInfo->setRatio(pResult->getInt(++i));
			pEffectItemInfo->setEffectClass((Effect::EffectClass)pResult->getInt(++i));
			pEffectItemInfo->setDuration(pResult->getInt(++i));

			addItemInfo(pEffectItemInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class EffectItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void EffectItemLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		/*
		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num FROM EffectItemObject"
			<< " WHERE OwnerID = '" << pCreature->getName() << "' AND Storage IN("
			<<(int)STORAGE_INVENTORY << ", " <<(int)STORAGE_GEAR << ", " <<(int)STORAGE_BELT << ", " 
			<<(int)STORAGE_EXTRASLOT << ", " <<(int)STORAGE_MOTORCYCLE << ", " <<(int)STORAGE_STASH << ", " 
			<<(int)STORAGE_GARBAGE << ")";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/

		Result* pResult = pStmt->executeQuery( "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM EffectItemObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9)",
												pCreature->getName().c_str() );



		while (pResult->next())
		{
			try {
				uint i = 0;

				EffectItem* pEffectItem = new EffectItem();

				pEffectItem->setItemID(pResult->getDWORD(++i));
				pEffectItem->setObjectID(pResult->getDWORD(++i));
				pEffectItem->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				pEffectItem->setNum(pResult->getBYTE(++i));
				pEffectItem->setCreateType((Item::CreateType)pResult->getInt(++i));

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*    pOusters        = NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
				Item*       pItem           = NULL;
				Stash*      pStash          = NULL;
				Belt*       pBelt           = NULL;
				Inventory*  pBeltInventory  = NULL;

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
								processItemBugEx(pCreature, pEffectItem);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pEffectItem))
						{
							pInventory->addItemEx(x, y, pEffectItem);
						}
						else
						{
							processItemBugEx(pCreature, pEffectItem);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pEffectItem);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pEffectItem);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pEffectItem);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pEffectItem);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pEffectItem);
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pEffectItem);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pEffectItem);
						}
						else pStash->insert(x, y, pEffectItem);
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pEffectItem);
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

void EffectItemLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, Num, ItemFlag FROM EffectItemObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			EffectItem* pEffectItem = new EffectItem();

			pEffectItem->setItemID(pResult->getInt(++i));
			pEffectItem->setObjectID(pResult->getInt(++i));
			pEffectItem->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pEffectItem->setNum(pResult->getBYTE(++i));
			pEffectItem->setCreateType((Item::CreateType)pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pEffectItem);
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

void EffectItemLoader::load(StorageID_t storageID, Inventory* pInventory) 
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

EffectItemLoader* g_pEffectItemLoader = NULL;
