#include <iterator>
//////////////////////////////////////////////////////////////////////////////
// Filename    : PlayerCreature.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "PlayerCreature.h"
#include "Stash.h"
#include "DB.h"
#include "Item.h"
#include "ItemUtil.h"
#include "Monster.h"
#include "Belt.h"
#include "Inventory.h"
#include "GoodsInventory.h"
#include "Zone.h"
#include "FlagSet.h"
#include "ParkingCenter.h"
#include "Key.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Gpackets/GCRankBonusInfo.h"
#include "Gpackets/GCTimeLimitItemInfo.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCMonsterKillQuestInfo.h"
#include "Gpackets/GCPetStashList.h"
#include "Gpackets/GCModifyNickname.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCCreateItem.h"
#include "PacketUtil.h"
#include "Player.h"
#include "RankBonusInfo.h"
#include "GamePlayer.h"
#include "TimeLimitItemManager.h"
#include "mission/QuestManager.h"
#include "Properties.h"
#include "PKZoneInfoManager.h"
#include "RankExpTable.h"
#include "EventKick.h"
#include "GameServerInfoManager.h"
#include <list>
#include <string.h>
#include "DefaultOptionSetInfo.h"
#include "Pet.h"
#include "SMSAddressBook.h"
#include "NicknameBook.h"
#include "GQuestManager.h"
#include "VariableManager.h"
#include "CreatureUtil.h"
#include "BloodBibleSignInfo.h"
#include "Store.h"
#include "AdvancementClassExpTable.h"
#include "SubInventory.h"
#include "PlayerInventory.h"
#include "SkillUtil.h"
#include "SkillHandler.h"
#include "SkillCastingTimeManager.h"

#include "TimeChecker.h"
#include "Born.h"
#ifdef __CONTRIBUTION_SYSTEM__
#include "ContributeInfo.h"
#include "LevelWarZoneInfoManager.h"
#endif

#include "WeekItemListManager.h"
#include "GlobalNPCManager.h"
#include "DBExecuteManager.h"
#include "Gpackets/GCChangeShape.h"
#include "EventSystemMessage.h"
#include "StringPool.h"
#include "RelicUtil.h"
#include "PCFinder.h"
#include "Utility.h"
#include "Gpackets/GSGuildMemberLogOn.h"
#include "SharedServerManager.h"
#include "ResurrectLocationManager.h"
#include "Gpackets/GSLogout.h"
#include "Gpackets/GCFriendUpdateConnect.h"
#include "Gpackets/GSFriendUpdateZone.h"
#include "EffectBonusExp.h"
#include "NewRankGem.h"
#include "RankGem.h"
#include "EffectChaoticExp.h"

const int MAX_GOODS_INVENTORY_SIZE = 10;

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
PlayerCreature::PlayerCreature(ObjectID_t OID, Player* pPlayer)
	throw()
: Creature(OID, pPlayer), m_pAdvancementClass( NULL )
{
	__BEGIN_TRY

	m_pInventory          = new PlayerInventory(this, 10, 6);
	m_pExtraInventorySlot = new InventorySlot();

	m_pGoodsInventory     = new GoodsInventory();

	m_pStash              = new Stash;
	m_StashNum            = 0;
	m_StashGold           = 0;
	m_bStashStatus        = false;

	m_pFlagSet            = new FlagSet;
	m_isPK				  = false;
	m_GuildID			  = 0;

	m_pQuestManager		  = new QuestManager(this);
	m_pTimeLimitItemManager = new TimeLimitItemManager( this );

	m_bLotto			  = false;
	m_pQuestItem		= NULL;
	m_pPetInfo			= NULL;

	m_RankExpSaveCount   = 0;
	m_pRank = NULL;
	m_PhoenixMonsterCounterEnabled = false;
	m_PhoenixMonsterCounterRewarded = false;
	m_PhoenixMonsterCounter = 0;
	m_PhoenixMonsterCounterGoal = 2000;
	m_PhoenixMonsterCounterRewardGoodsID = 0;
	m_PhoenixMonsterCounterRewardGoodsCount = 1;
	m_PhoenixMonsterCounterMaxLevelGap = 30;
	m_pPet = NULL;
//	m_pSMSAddressBook = NULL;
	m_pSMSAddressBook = new SMSAddressBook( this );
	m_SMSCharge = 0;
//	m_pNicknameBook = NULL;
	m_pNicknameBook = new NicknameBook( this );
	m_pNickname = NULL;
	
	m_PetStash.reserve( MAX_PET_STASH );
	for ( int i=0; i<MAX_PET_STASH; ++i ) m_PetStash.push_back(NULL);

	m_pGQuestManager = new GQuestManager( this );
	m_pBloodBibleSign = new BloodBibleSignInfo;
	m_BaseLuck = 0;
	m_pStore = new Store;
	m_bAdvanced = false;
	m_AdvancementClassExpSaveCount = 0;
	
	m_HP[0] = m_HP[1] = m_HP[2] = 0; 
	m_MP[0] = m_MP[1] = m_MP[2] = 0;

	m_AdvancedAttrBonus = 0;
	//20070907
	m_AdvancedSTR[0] = m_AdvancedSTR[1] = m_AdvancedSTR[2] = 0;
	m_AdvancedDEX[0] = m_AdvancedDEX[1] = m_AdvancedDEX[2] = 0;
	m_AdvancedINT[0] = m_AdvancedINT[1] = m_AdvancedINT[2] = 0;
	m_MasterEffectColor = 0;

	m_CurrentGearSlotID = 0;
	
	m_pWeekItemListManager = new WeekItemListManager(this);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// Extra slot (mouse) item'ı envantere geri yaz.
// Destructor inventory'yi ~180'de sildiği için logout flush save()/disconnect
// ve destructor'ın inventory silinmesinden ÖNCE çağrılmalıdır.
//////////////////////////////////////////////////////////////////////////////
bool PlayerCreature::restoreExtraInventorySlotToInventory(bool bSendCreatePacket) throw()
{
	try
	{
		if (m_pExtraInventorySlot == NULL)
		{
			return false;
		}

		Item* pItem = m_pExtraInventorySlot->getItem();
		if (pItem == NULL)
		{
			return false;
		}

		Inventory* pInventory = getInventory();
		if (pInventory == NULL)
		{
			filelog("EventStarExtraSlot.log",
				"restore extra skipped: inventory NULL name=%s itemClass=%d itemID=%u",
				getName().c_str(), (int)pItem->getItemClass(), pItem->getItemID());
			return false;
		}

		_TPOINT pt;
		if (!pInventory->getEmptySlot(pItem, pt))
		{
			filelog("EventStarExtraSlot.log",
				"restore extra skipped: inventory full name=%s itemClass=%d itemID=%u oid=%u num=%d",
				getName().c_str(), (int)pItem->getItemClass(), pItem->getItemID(),
				pItem->getObjectID(), (int)pItem->getNum());
			return false;
		}

		// Extra'dan unlink; Item objesi silinmez.
		deleteItemFromExtraInventorySlot();

		if (!pInventory->addItem(pt.x, pt.y, pItem))
		{
			addItemToExtraInventorySlot(pItem);
			filelog("EventStarExtraSlot.log",
				"restore extra addItem failed, returned to extra name=%s x=%d y=%d",
				getName().c_str(), pt.x, pt.y);
			return false;
		}

		char pField[80];
		sprintf(pField, "Storage=%d, StorageID=%u, X=%d, Y=%d",
			STORAGE_INVENTORY, 0, pt.x, pt.y);
		pItem->tinysave(pField);

		if (bSendCreatePacket)
		{
			Player* pPlayer = getPlayer();
			if (pPlayer != NULL)
			{
				GCCreateItem gcCreateItem;
				makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);
				pPlayer->sendPacket(&gcCreateItem);
			}
		}

		filelog("EventStarExtraSlot.log",
			"restore extra to inventory name=%s itemClass=%d itemID=%u oid=%u num=%d x=%d y=%d send=%d",
			getName().c_str(), (int)pItem->getItemClass(), pItem->getItemID(),
			pItem->getObjectID(), (int)pItem->getNum(), pt.x, pt.y,
			bSendCreatePacket ? 1 : 0);

		return true;
	}
	catch (Throwable& t)
	{
		filelog("EventStarExtraSlot.log",
			"restore extra exception: %s name=%s",
			t.toString().c_str(), getName().c_str());
		return false;
	}
	catch (...)
	{
		filelog("EventStarExtraSlot.log",
			"restore extra unknown exception name=%s",
			getName().c_str());
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
PlayerCreature::~PlayerCreature()
	throw()
{
	__BEGIN_TRY

	// Extra slot, inventory silinmeden önce çantaya yazılır.
	// Aksi halde destructor extra'yı SAFE_DELETE eder; DB STORAGE_EXTRASLOT
	// kalır ve login'de çantada görünmez.
	try
	{
		restoreExtraInventorySlotToInventory(false);
	}
	catch (Throwable& t)
	{
		filelog("EventStarExtraSlot.log",
			"destructor restore extra failed: %s CharName=%s",
			t.toString().c_str(), getName().c_str());
	}
	catch (...)
	{
		filelog("EventStarExtraSlot.log",
			"destructor restore extra unknown exception CharName=%s",
			getName().c_str());
	}

	// 상점에 올라가 있는 아이템을 삭제한다.
	m_pStore->clearAll();
	
    try
    {
        // 인벤토리 삭제
        SAFE_DELETE(m_pInventory);
    }
    catch(Throwable& t)
    {
        filelog("PlayerCreatureDestructorException.log", "Delete Inventory - %s, CharName=%s", t.toString().c_str(), getName().c_str());
    }
    catch(exception& e)
    {
        filelog("PlayerCreatureDestructorException.log", "Delete Inventory - %s, CharName=%s", e.what(), getName().c_str());
    }
	catch(...)
    {
        filelog("PlayerCreatureDestructorException.log", "Delete Inventory - Unknow exception, CharName=%s", getName().c_str());
	}

	SAFE_DELETE(m_pGoodsInventory);

	// 마우스 포인터와 달려있는 아이템을 삭제한다.
	if (m_pExtraInventorySlot != NULL)
	{
		Item* pItem = m_pExtraInventorySlot->getItem();
		if (pItem != NULL)
		{
			if (pItem->getItemClass() == Item::ITEM_CLASS_KEY)
			{
				Key* pKey = dynamic_cast<Key*>(pItem);
				// 2007 03 08 146번줄에서 팅했다~ 의심나서 걸어봐따;
				if( pKey != NULL )
				{
					// 걍 간단하게 이안에서 알아서 존에서 지워 주도록 하자.
					if (g_pParkingCenter->hasMotorcycleBox(pKey->getTarget()))
					{
						g_pParkingCenter->deleteMotorcycleBox(pKey->getTarget());
					}
				}
			}

			m_pExtraInventorySlot->deleteItem();
			SAFE_DELETE(pItem);
		}

		SAFE_DELETE(m_pExtraInventorySlot);
	}

	// 보관함 삭제
	SAFE_DELETE(m_pStash);
    try
    {
        // 보관함 삭제
		SAFE_DELETE(m_pStash);
    }
    catch(Throwable& t)
    {
        filelog("PlayerCreatureDestructorException.log", "Delete Stash - %s, CharName=%s", t.toString().c_str(), getName().c_str());
    }
    catch(exception& e)
    {
        filelog("PlayerCreatureDestructorException.log", "Delete Stash - %s, CharName=%s", e.what(), getName().c_str());
    }
    catch(...)
    {
        filelog("PlayerCreatureDestructorException.log", "Delete Stash - Unknow exception, CharName=%s", getName().c_str());
    }

	// 플래그 셋 삭제
	SAFE_DELETE(m_pFlagSet);

	// RankBonus hash_map 삭제
	for ( HashMapRankBonusItor itr = m_RankBonuses.begin(); itr != m_RankBonuses.end(); itr++ )
	{
		SAFE_DELETE( itr->second );
	}
	m_RankBonuses.clear();

	if ( m_pTimeLimitItemManager != NULL )
		SAFE_DELETE( m_pTimeLimitItemManager );

	if ( m_pQuestManager != NULL )
		SAFE_DELETE( m_pQuestManager );

/*	for ( list<ItemNameInfo*>::iterator itr = m_ItemNameInfoList.begin(); itr != m_ItemNameInfoList.end(); itr++ )
	{
		ItemNameInfo* pInfo = *itr;
		SAFE_DELETE( pInfo );
	}
	m_ItemNameInfoList.clear();*/

	if ( m_pQuestItem != NULL )
		SAFE_DELETE( m_pQuestItem );

	SAFE_DELETE( m_pRank );

	for ( int i=0; i<MAX_PET_STASH; ++i )
	{
		SAFE_DELETE( m_PetStash[i] );
	}

	SAFE_DELETE( m_pPet );
	SAFE_DELETE( m_pSMSAddressBook );
	SAFE_DELETE( m_pNicknameBook );
	SAFE_DELETE( m_pGQuestManager );
	SAFE_DELETE( m_pBloodBibleSign );
	SAFE_DELETE( m_pStore );
	SAFE_DELETE( m_pAdvancementClass );

	{
		TParameterList::iterator iter;
		void *pValue;
		
		iter = m_ParameterList.begin();
		for( ; iter != m_ParameterList.end() ; ++iter )
		{
			pValue = iter->second;
			SAFE_DELETE(pValue);
		}
	}
	

	__END_CATCH
}

void PlayerCreature::initWearItemSlot(int iMaxWearItemSlot, int iMaxGearSlot)
{
	m_pWearItem.resize(iMaxWearItemSlot);
	m_pRealWearingCheck.resize(iMaxWearItemSlot);

	m_pWearItemSwapPool.resize(iMaxGearSlot);

	for(int i = 0; i < m_pWearItemSwapPool.size() ; ++i)
	{
		m_pWearItemSwapPool[i].resize(m_pWearItem.size());
	}

	GearSlotID_t GearSlotID;

	for (int i = 0; i < m_pWearItem.size() ; i++)
	{
		m_pWearItem[i] = NULL;

		for(GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size() ; ++GearSlotID)
		{
			m_pWearItemSwapPool[GearSlotID][i] = NULL;
		}
	}
}

bool PlayerCreature::load()

	throw(InvalidProtocolException, Error)
{
	__BEGIN_TRY

	m_pSMSAddressBook->load();

#ifdef __MOFUS__
	m_PowerPoint = loadPowerPoint( getName() );
#endif

#ifdef __CONTRIBUTION_SYSTEM__
	m_ContributePoint = 0;
	m_ContributeRank = g_pContributeInfo->getRank(m_ContributePoint);
	loadContribute(); // 기여도 관련 정보 로드
#endif
	return true;

	__END_CATCH
}

#ifdef __CONTRIBUTION_SYSTEM__
void PlayerCreature::loadContribute()
	throw()
{
	Statement* pStmt = NULL;
	Result* pResult  = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT ContributePoint, HolyWarCheckPoint, HolyWarCheckDate, LevelWarCheckPoint, LevelWarCheckDate FROM ContributeDegree where UserName = '%s'"
			, getName().c_str());

		if (pResult->getRowCount() != 0) 
		{
			pResult->next();
			m_ContributePoint = pResult->getInt(1);
			m_ContributeRank = g_pContributeInfo->getRank(m_ContributePoint);
			m_HolyWarCheckPoint = pResult->getInt(2);
			m_HolyWarCheckDate = VSDateTime(pResult->getString(3));

			m_LevelWarCheckPoint = pResult->getInt(4);
			m_LevelWarCheckDate = VSDateTime(pResult->getString(5));
		}
		else
		{
			m_HolyWarCheckDate = VSDateTime("2000:01:01:00:00:00");
			m_LevelWarCheckDate = VSDateTime("2000:01:01:00:00:00");
			// 2026-05-10 FIX: MySQL 8 strict mode 'YYYY.MM.DD-HH:MM:SS:mmm' formatini reddediyor.
			// toString() yerine toDateTime() ('YYYY-MM-DD HH:MM:SS' MySQL-uyumlu) kullaniyoruz.
			pStmt->executeQuery("INSERT INTO ContributeDegree (UserName, ContributePoint, HolyWarCheckDate, LevelWarCheckDate) VALUES ('%s', 0, '%s', '%s')"
				, getName().c_str(), m_HolyWarCheckDate.toDateTime().c_str(), m_LevelWarCheckDate.toDateTime().c_str());
		}
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)	

	VSDateTime dt = VSDateTime::currentDateTime();

	int lastDay = dt.daysTo(m_HolyWarCheckDate);
	if( lastDay < -4) // 성서전 출석 체크시 저번에 불참했을 시 체크하는 부분.... 대략 체크 흠...달리 방법이 ㅠㅠ
		m_HolyWarCheckPoint = 0;

	lastDay = dt.daysTo(m_LevelWarCheckDate);
	if( lastDay < -1) // 이건 레벨전
		m_LevelWarCheckPoint = 0;

}

void PlayerCreature::addContributePoint(int addContributePoint)
throw()
{
	m_ContributePoint += addContributePoint;
	m_ContributeRank = g_pContributeInfo->getRank(m_ContributePoint);

	saveContributePoint();
}

void PlayerCreature::saveContributePoint() const
throw()
{
	ostringstream os;
	
	os << "UPDATE ContributeDegree SET ContributePoint = " 
		<< m_ContributePoint 
		<< " where UserName = '" << getName().c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());
}

void PlayerCreature::setContributePoint(int point)
throw()
{
	m_ContributePoint = point;
	m_ContributeRank = g_pContributeInfo->getRank(m_ContributePoint);

	saveContributePoint();
}

void PlayerCreature::addHolyWarCheckPoint()
throw()
{
	VSDateTime dt = VSDateTime::currentDateTime();
	
	int lastDay = dt.daysTo(m_HolyWarCheckDate);
	if( lastDay == 0)
		return;

	m_HolyWarCheckPoint++;
	if(m_HolyWarCheckPoint > 3)
		m_HolyWarCheckPoint = 3;
	
	m_HolyWarCheckDate = dt;

	ostringstream os;
	
	// 2026-05-10 FIX: MySQL 8 toDateTime() formatı zorunlu
	os << "UPDATE ContributeDegree SET "
		<< "HolyWarCheckPoint = " << m_HolyWarCheckPoint
		<< ", HolyWarCheckDate = '" << m_HolyWarCheckDate.toDateTime()
		<< "' where UserName = '" << getName() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());
}

void PlayerCreature::addLevelWarCheckPoint()
throw()
{
	VSDateTime dt = VSDateTime::currentDateTime();

	int lastDay = dt.daysTo(m_LevelWarCheckDate);
	if( lastDay == 0)
		return;

	m_LevelWarCheckPoint++;
	if(m_LevelWarCheckPoint > 3)
		m_LevelWarCheckPoint = 3;

	m_LevelWarCheckDate = dt;

	ostringstream os;
	
	// 2026-05-10 FIX: MySQL 8 toDateTime() formatı zorunlu
	os << "UPDATE ContributeDegree SET "
		<< "LevelWarCheckPoint = " << m_LevelWarCheckPoint
		<< ", LevelWarCheckDate = '" << m_LevelWarCheckDate.toDateTime()
		<< "' where UserName = '" << getName() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());
}
#endif

//////////////////////////////////////////////////////////////////////////////
// 아이템 하나 등록하기
// *** 주의 ***
// 이 함수를 부르기 전에 반드시 OR에다 락을 걸어야 한다.
// 내부적으로는 락을 걸지 않기 때문이다.
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::registerItem(Item* pItem, ObjectRegistry& OR)
    throw()
{
	__BEGIN_TRY

	Assert(pItem != NULL);

	// Item 자체에 ObjectID 할당
	OR.registerObject_NOLOCKED(pItem);
	// 시간제한 아이템 매니저에 OID 가 바뀌었다고 알려준다.
	m_pTimeLimitItemManager->registerItem( pItem );

	// 벨트라면 안에 있는 아이템들도 OID를 받아놓아야 한다.
	if (pItem->getItemClass() == Item::ITEM_CLASS_BELT)
	{
		Belt*       pBelt       = dynamic_cast<Belt*>(pItem);
		PocketNum_t PocketCount = pBelt->getPocketCount();
		Inventory*  pInventory  = pBelt->getInventory();

		for (int k=0; k<PocketCount; k++)
		{
			Item* pBeltItem = pInventory->getItem(k, 0);
			if (pBeltItem != NULL)
			{
				OR.registerObject_NOLOCKED(pBeltItem);
				// 시간제한 아이템 매니저에 OID 가 바뀌었다고 알려준다.
				m_pTimeLimitItemManager->registerItem( pBeltItem );
			}
		}
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
	{
		SubInventory* pSubInventory = dynamic_cast<SubInventory*>(pItem);
		Inventory* pInventory = pSubInventory->getInventory();

		list<Item*> ItemList;
		int height = pInventory->getHeight();
		int width  = pInventory->getWidth();

		for (int j=0; j<height; j++)
		{
			for (int i=0; i<width; i++)
			{
				Item* pItem = pInventory->getItem(i, j);
				if (pItem != NULL)
				{
					list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

					if (itr == ItemList.end())
					{
						// 같은 아이템을 두번 등록하지 않기 위해서
						// 리스트에다가 아이템을 집어넣는다.
						ItemList.push_back(pItem);

						// 아이템의 OID를 할당받는다.
						registerItem(pItem, OR);

						i += pItem->getVolumeWidth() - 1;
					}
				}
			}
		}
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 인벤토리 안에 있는 아이템 등록하기
// *** 주의 ***
// 이 함수를 부르기 전에 반드시 OR에다 락을 걸어야 한다.
// 내부적으로는 락을 걸지 않기 때문이다.
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::registerInventory(ObjectRegistry& OR)
    throw()
{
	__BEGIN_TRY

	list<Item*> ItemList;
	int height = m_pInventory->getHeight();
	int width  = m_pInventory->getWidth();

	for (int j=0; j<height; j++)
	{
		for (int i=0; i<width; i++)
		{
			Item* pItem = m_pInventory->getItem(i, j);
			if (pItem != NULL)
			{
				// 등록된 아이템의 리스트에서 현재 아이템을 찾는다.
				list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

				if (itr == ItemList.end())
				{
					// 같은 아이템을 두번 등록하지 않기 위해서
					// 리스트에다가 아이템을 집어넣는다.
					ItemList.push_back(pItem);

					// 아이템의 OID를 할당받는다.
					registerItem(pItem, OR);

					i += pItem->getVolumeWidth() - 1;
				}
			}
		}
	}

	__END_CATCH
}

void PlayerCreature::registerInitInventory(ObjectRegistry& OR)
    throw()
{
	__BEGIN_TRY

	list<Item*> ItemList;
	int height = m_pInventory->getHeight();
	int width  = m_pInventory->getWidth();

	for (int j=0; j<height; j++)
	{
		for (int i=0; i<width; i++)
		{
			Item* pItem = m_pInventory->getItem(i, j);
			if (pItem != NULL)
			{
				// 등록된 아이템의 리스트에서 현재 아이템을 찾는다.
				list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

				if (itr == ItemList.end())
				{
					// 같은 아이템을 두번 등록하지 않기 위해서
					// 리스트에다가 아이템을 집어넣는다.
					ItemList.push_back(pItem);

					// ItemTrace 를 남길 것인지 결정
					pItem->setTraceItem( bTraceLog( pItem ) );

					// 아이템의 OID를 할당받는다.
					registerItem(pItem, OR);

					i += pItem->getVolumeWidth() - 1;
				}
			}
		}
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 보관함 안에 들어있는 아이템 등록하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::registerStash(void)
	throw()
{
	__BEGIN_TRY

	Zone* pZone = getZone();
	ObjectRegistry& OR = pZone->getObjectRegistry();

	__ENTER_CRITICAL_SECTION(OR)

	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			Item* pStashItem = m_pStash->get(r, i);
			if (pStashItem != NULL) 
			{
				pStashItem->setTraceItem( bTraceLog( pStashItem ) );

				registerItem(pStashItem, OR);
			}
		}
	}

	__LEAVE_CRITICAL_SECTION(OR)

	m_bStashStatus = true;

	__END_CATCH
}

void PlayerCreature::registerGoodsInventory(ObjectRegistry& OR)
    throw()
{
	__BEGIN_TRY

	GoodsInventory::ListItem& goods = m_pGoodsInventory->getGoods();
	GoodsInventory::ListItemItr itr = goods.begin();

	for ( ; itr != goods.end(); itr++ )
	{
		registerItem( (*itr).m_pItem, OR);
	}

	__END_CATCH
}


void PlayerCreature::loadTimeLimitItem() throw(Error)
{
	__BEGIN_TRY

	Assert( m_pTimeLimitItemManager != NULL );

	m_pTimeLimitItemManager->load();

	__END_CATCH
}

void PlayerCreature::loadItem()
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	loadTimeLimitItem();

	m_pNicknameBook->load();
	m_pGQuestManager->init();


	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT ItemType FROM BloodBibleSignObject WHERE OwnerID='%s' ORDER BY ItemType", getName().c_str());

		while ( pResult->next() )
		{
			m_pBloodBibleSign->getList().push_back( pResult->getInt(1) );
		}

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt );

	__END_CATCH
}

int PlayerCreature::getItemClassTypeNum( Item::ItemClass itemClass, ItemType_t itemType )
	throw()
{
	__BEGIN_TRY

	int total = 0;

	total += m_pInventory->getItemClassTypeNum( itemClass, itemType );
	
	for ( BYTE rack = 0; rack < STASH_RACK_MAX; ++rack )
	{
		for ( BYTE index = 0; index < STASH_INDEX_MAX; ++index )
		{
			Item* pItem = m_pStash->get( rack, index );

			if ( pItem != NULL
				&& pItem->getItemClass() == itemClass
				&& pItem->getItemType() == itemType
			   )
			{
				total += pItem->getNum();
			}
		}
	}

	return total;

	__END_CATCH
}

int PlayerCreature::decreaseItemClassTypeNum( Item::ItemClass itemClass, ItemType_t itemType, int num )
	throw()
{
	__BEGIN_TRY

	int decreaseNum = num;

	decreaseNum -= m_pInventory->decreaseItemClassTypeNum( itemClass, itemType, decreaseNum, getPlayer() );

	if ( decreaseNum > 0 )
	{
		for ( BYTE rack = 0; rack < STASH_RACK_MAX; ++rack )
		{
			for ( BYTE index = 0; index < STASH_INDEX_MAX; ++index )
			{
				Item* pItem = m_pStash->get( rack, index );

				if ( pItem != NULL
					&& pItem->getItemClass() == itemClass
					&& pItem->getItemType() == itemType
				   )
				{
					if ( decreaseNum > 0 )
					{
						int itemNum = pItem->getNum();

						if ( itemNum <= decreaseNum )
						{
							m_pStash->remove( rack, index );

							pItem->destroy();
							SAFE_DELETE( pItem );

							decreaseNum -= itemNum;
						}
						else
						{
							pItem->setNum( itemNum - decreaseNum );

							// DB에 저장
							char pField[80];
							sprintf( pField, "Num=%d", pItem->getNum() );
							pItem->tinysave( pField );

							return num;
						}
					}
					else
					{
						return num;
					}
				}
			}
		}
	}

	// 지운 갯수를 리턴한다.
	return num - decreaseNum;

	__END_CATCH
}


bool PlayerCreature::wasteIfTimeLimitExpired(Item* pItem)
	throw (Error)
{
	__BEGIN_TRY

	if ( pItem == NULL ) return false;

	if ( m_pTimeLimitItemManager->wasteIfTimeOver( pItem ) )
	{
		pItem->whenPCLost(this);
		pItem->waste( STORAGE_TIMEOVER );
		return true;
	}

	return false;

	__END_CATCH
}

void PlayerCreature::sendTimeLimitItemInfo()
	throw(Error)
{
	__BEGIN_TRY

	GCTimeLimitItemInfo gcTimeLimitItemInfo;

	if ( m_pTimeLimitItemManager->makeTimeLimitItemInfo( gcTimeLimitItemInfo ) )
	{
		getPlayer()->sendPacket( &gcTimeLimitItemInfo );
	}

	__END_CATCH
}

void PlayerCreature::addTimeLimitItem(Item* pItem, DWORD time) throw(Error)
{
	m_pTimeLimitItemManager->addTimeLimitItem( pItem, time );
} 
// 20080913
DWORD PlayerCreature::getLimitTime(Item* pItem) throw(Error)
{
	return m_pTimeLimitItemManager->getLimitTime(pItem);
}

void PlayerCreature::sellItem( Item* pItem ) throw(Error)
{
	__BEGIN_TRY

	if ( pItem->isTimeLimitItem() ) m_pTimeLimitItemManager->itemSold( pItem );

	__END_CATCH
}

void PlayerCreature::deleteItemByMorph( Item* pItem ) throw(Error)
{
	__BEGIN_TRY

	if ( pItem->isTimeLimitItem() ) m_pTimeLimitItemManager->deleteItemByMorph( pItem );

	__END_CATCH
}

void PlayerCreature::updateItemTimeLimit( Item* pItem, DWORD time ) throw(Error)
{
	__BEGIN_TRY

	if ( pItem->isTimeLimitItem() ) m_pTimeLimitItemManager->updateItemTimeLimit( pItem, time );

	__END_CATCH
}

void PlayerCreature::sendCurrentQuestInfo() const throw(Error)
{
	m_pQuestManager->sendQuestInfo();
}

void PlayerCreature::whenQuestLevelUpgrade()
{
	static bool bNonPK = g_pGameServerInfoManager->getGameServerInfo( 1, g_pConfig->getPropertyInt( "ServerID" ), g_pConfig->getPropertyInt( "WorldID" ) )->isNonPKServer();
// 2007 04 23 논피케이 렙제한 본서버 제외
#ifdef __NETMARBLE_SERVER__
	if ( bNonPK && getLevel() > 80 )
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
		Assert(pGamePlayer!=NULL);

		GCSystemMessage gcSM;
		gcSM.setMessage( "PK가 금지된 서버를 이용할 수 있는 레벨이 초과했습니다. 10초 뒤에 접속이 종료됩니다." );
		pGamePlayer->sendPacket( &gcSM );

		bool newEvent = false;
		EventKick* pEvent = dynamic_cast<EventKick*>(pGamePlayer->getEvent(Event::EVENT_CLASS_KICK));
		if ( pEvent == NULL )
		{
			pEvent = new EventKick( pGamePlayer );
			newEvent = true;
		}

		pEvent->setDeadline(100);
		if ( newEvent ) pGamePlayer->addEvent( pEvent );
	}
#endif

	if ( g_pVariableManager->getVariable( CHOBO_EVENT ) )
	{
		Level_t level = getLevel();

		//by viva
		int TypeMax = 0;
		bool SaveNextType = false;
		Statement* pStmt = NULL;
		BEGIN_DB
		{
			StringStream sql;
			sql << "SELECT TypeMax FROM GQuestGiveMax WHERE Name='" << getName() << "'";
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			Result* pResult = pStmt->executeQuery("SELECT TypeMax FROM GQuestGiveMax WHERE Name='%s'", getName().c_str() );
			if ( pResult->next() )
				TypeMax = pResult->getInt(1);

			if ( level < 80 && (level%10) == 0 )
			{
				if(TypeMax == 0)
				{
					SaveNextType = true;
					pStmt->executeQuery("INSERT INTO GQuestGiveMax(Name, TypeMax) VALUES('%s', %d)", getName().c_str(), 13+(level/10));
				}
				else
				{

					if(TypeMax < 13+(level/10))
					{
						SaveNextType = true;
						pStmt->executeQuery("UPDATE GQuestGiveMax SET TypeMax=%d where Name='%s'", 13+(level/10), getName().c_str());
					}
				}
			}
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
		//end
		if ( level < 80 && (level%10) == 0 && SaveNextType/*by viva*/) // 20080718
		{
			getGQuestManager()->getGQuestInventory().saveOne( getName(), 13+(level/10) );
			getPlayer()->sendPacket( getGQuestManager()->getGQuestInventory().getInventoryPacket() );

			GCNoticeEvent gcNE;
			gcNE.setCode( NOTICE_EVENT_GIVE_PRESENT_1 + (level/10) );
			getPlayer()->sendPacket( &gcNE );
		}
		if ( level == 80 )
		{
			GCNoticeEvent gcNE;
			gcNE.setCode( NOTICE_EVENT_CAN_OPEN_PRESENT_8 );
			getPlayer()->sendPacket( &gcNE );
		}
	}

	m_pGQuestManager->levelUp();

	if ( getQuestLevel() == 40 )
	{
		GCNoticeEvent gcNE;
		gcNE.setCode( NOTICE_EVENT_CAN_PET_QUEST );
		getPlayer()->sendPacket(&gcNE);
	}

	if ( getLevel() >= 10 )
	{
		NicknameInfo* pLevelNickInfo = m_pNicknameBook->getNicknameInfo(1);
		if ( pLevelNickInfo == NULL )
		{
			pLevelNickInfo = new NicknameInfo;
			pLevelNickInfo->setNicknameID( 1 );
			pLevelNickInfo->setNicknameType( NicknameInfo::NICK_BUILT_IN );
			pLevelNickInfo->setNicknameIndex( 0 );
			m_pNicknameBook->setNicknameInfo( 1, pLevelNickInfo );
		}

		if ( pLevelNickInfo->getNicknameType() == NicknameInfo::NICK_BUILT_IN
				&& pLevelNickInfo->getNicknameIndex() != getLevel()/10 )
		{
			pLevelNickInfo->setNicknameIndex( getLevel()/10 );

			Packet* pNicknamePacket = m_pNicknameBook->getNicknameBookListPacket();
			getPlayer()->sendPacket( pNicknamePacket );
			SAFE_DELETE( pNicknamePacket );

			if ( pLevelNickInfo == m_pNickname )
			{
				GCModifyNickname gcMN;
				gcMN.setObjectID( getObjectID() );
				gcMN.setNicknameInfo( m_pNickname );
				getZone()->broadcastPacket( getX(), getY(), &gcMN );
			}
		}
	}

#ifdef __TEST_SERVER__
	if ( getLevel() >= 150 )
	{
		increaseAdvancementClassExp(1);
		GCModifyInformation gcMI;
		gcMI.addShortData(MODIFY_ADVANCEMENT_CLASS_LEVEL, getAdvancementClassLevel());
		gcMI.addLongData(MODIFY_ADVANCEMENT_CLASS_GOAL_EXP, getAdvancementClassGoalExp());
		getPlayer()->sendPacket( &gcMI );

		GCOtherModifyInfo gcOMI;
		gcOMI.addShortData(MODIFY_ADVANCEMENT_CLASS_LEVEL, getAdvancementClassLevel());
		getZone()->broadcastPacket(getX(), getY(), &gcOMI, this);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////
// 보관함 갯수 세팅하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::setStashNumEx(BYTE num)
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	setStashNum(num);

	BEGIN_DB
	{
		StringStream sqlSlayer;
		StringStream sqlVampire;
		StringStream sqlOusters;

		sqlSlayer << "UPDATE Slayer set StashNum = " << (int)num
			<< " WHERE Name = '" << getName() << "'";
		sqlVampire << "UPDATE Vampire set StashNum = " << (int)num
			<< " WHERE Name = '" << getName() << "'";
		sqlOusters << "UPDATE Ousters set StashNum = " << (int)num
			<< " WHERE Name = '" << getName() << "'";

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery(sqlSlayer.toString());
		if ( !isOusters() )
			pStmt->executeQuery(sqlVampire.toString());
		else
			pStmt->executeQuery(sqlOusters.toString());

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 보관함에 들어있는 돈 세팅하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::setStashGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY

	//cout << "setStashGoldEx Called" << "Name:" << getName() << " Gold: " << (int)gold << endl;
	Statement* pStmt = NULL;

	setStashGold(gold);

	BEGIN_DB
	{
		StringStream sqlSlayer;
		StringStream sqlVampire;
		StringStream sqlOusters;

		sqlSlayer << "UPDATE Slayer set StashGold = " << (int)gold
			<< " WHERE Name = '" << getName() << "'";
		sqlVampire << "UPDATE Vampire set StashGold = " << (int)gold
			<< " WHERE Name = '" << getName() << "'";
		sqlOusters << "UPDATE Ousters set StashGold = " << (int)gold
			<< " WHERE Name = '" << getName() << "'";

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery(sqlSlayer.toString());
		if ( !isOusters() )
			pStmt->executeQuery(sqlVampire.toString());
		else
			pStmt->executeQuery(sqlOusters.toString());

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 보관함에 들어있는 돈 세팅하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::increaseStashGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	setStashGold(m_StashGold + gold);

	BEGIN_DB
	{
		StringStream sqlSlayer;
		StringStream sqlVampire;
		StringStream sqlOusters;

		sqlSlayer << "UPDATE Slayer set StashGold = " << (int)m_StashGold
			<< " WHERE Name = '" << getName() << "'";
		sqlVampire << "UPDATE Vampire set StashGold = " << (int)m_StashGold
			<< " WHERE Name = '" << getName() << "'";
		sqlOusters << "UPDATE Ousters set StashGold = " << (int)m_StashGold
			<< " WHERE Name = '" << getName() << "'";

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery(sqlSlayer.toString());
		if ( !isOusters() )
			pStmt->executeQuery(sqlVampire.toString());
		else
			pStmt->executeQuery(sqlOusters.toString());

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 보관함에 들어있는 돈 세팅하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::decreaseStashGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	setStashGold(m_StashGold - gold);

	BEGIN_DB
	{
		StringStream sqlSlayer;
		StringStream sqlVampire;
		StringStream sqlOusters;

		sqlSlayer << "UPDATE Slayer set StashGold = " << (int)m_StashGold
			<< " WHERE Name = '" << getName() << "'";
		sqlVampire << "UPDATE Vampire set StashGold = " << (int)m_StashGold
			<< " WHERE Name = '" << getName() << "'";
		sqlOusters << "UPDATE Ousters set StashGold = " << (int)m_StashGold
			<< " WHERE Name = '" << getName() << "'";

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery(sqlSlayer.toString());
		if ( !isOusters() )
			pStmt->executeQuery(sqlVampire.toString());
		else
			pStmt->executeQuery(sqlOusters.toString());

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 보관함 메모리에서 삭제하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::deleteStash(void)
	throw()
{
	__BEGIN_TRY

	SAFE_DELETE(m_pStash);
	m_StashNum = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 플래그셋 메모리에서 삭제하기
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::deleteFlagSet(void)
	throw()
{
	__BEGIN_TRY

	SAFE_DELETE(m_pFlagSet);

	__END_CATCH
}

//----------------------------------------------------------------------
// 선공자의 리스트에 선공자를 추가하는 함수
//----------------------------------------------------------------------
void PlayerCreature::addEnemy(const string& Name)
	throw(Error)
{
	__BEGIN_DEBUG

	list<string>::iterator itr = find(m_Enemies.begin() , m_Enemies.end() , Name);

	// 없다면 추가하자
	if (itr == m_Enemies.end()) 
	{
		m_Enemies.push_back(Name);
		//cout << "선공자를 추가한다 : " << Name << endl;
	}

	__END_DEBUG
}

//----------------------------------------------------------------------
// 선공자의 리스트에 선공자를 삭제하는 함수
//----------------------------------------------------------------------
void PlayerCreature::deleteEnemy(const string& Name)
	throw(NoSuchElementException, Error)
{
	__BEGIN_DEBUG

    list<string>::iterator itr = find(m_Enemies.begin(), m_Enemies.end() , Name);
	if (itr != m_Enemies.end()) 
	{
		m_Enemies.erase(itr);
		//cout << "선공자를 지운다 : " << Name << endl;
	}

	__END_DEBUG
}

//----------------------------------------------------------------------
// 특정 이름을 가진 선공자가 이미 있는지 없는지 확인하는 함수.
//----------------------------------------------------------------------
bool PlayerCreature::hasEnemy(const string& Name)
	const throw()
{
	__BEGIN_DEBUG

    list<string>::const_iterator itr = find(m_Enemies.begin(), m_Enemies.end() , Name);
	if (itr != m_Enemies.end()) 
	{
		//cout << "선공자로 이미 설정이 되어있다 : " << Name << endl;
		return true;
	} 
	else 
	{
		return false;
	}

	__END_DEBUG
}

//----------------------------------------------------------------------
// 길드 이름을 가져오는 함수
//----------------------------------------------------------------------
string PlayerCreature::getGuildName() const
	throw()
{
	Guild* pGuild = g_pGuildManager->getGuild( m_GuildID );
	
	if ( pGuild != NULL )
		return pGuild->getName();

	return "";
}

//----------------------------------------------------------------------
// 길드 멤버 랭크를 가져오는 함수
//----------------------------------------------------------------------
GuildMemberRank_t PlayerCreature::getGuildMemberRank() const
	throw()
{
	Guild* pGuild = g_pGuildManager->getGuild( m_GuildID );

	if ( pGuild != NULL )
	{
		GuildMember* pGuildMember = pGuild->getMember( getName() );
		if ( pGuildMember != NULL )
		{
			return pGuildMember->getRank();
		}
	}

	return GuildMember::GUILDMEMBER_RANK_DENY;
}

Rank_t PlayerCreature::getRank() const throw() { return m_pRank->getLevel(); }
RankExp_t PlayerCreature::getRankExp() const throw() { return m_pRank->getTotalExp(); }
RankExp_t PlayerCreature::getRankGoalExp() const throw() { return m_pRank->getGoalExp(); }

RankBonus* PlayerCreature::getRankBonus( RankBonus::RankBonusType type ) const
	throw()
{
	__BEGIN_TRY
	
	HashMapRankBonusConstItor itr = m_RankBonuses.find( type );

	if ( itr == m_RankBonuses.end() )
	{
		return NULL;
	}
	
	return itr->second;

	__END_CATCH
}

void PlayerCreature::addRankBonus( RankBonus* rankBonus )
	throw()
{
	__BEGIN_TRY

	HashMapRankBonusItor itr = m_RankBonuses.find( rankBonus->getType() );

	if ( itr == m_RankBonuses.end() )
	{
		m_RankBonuses[rankBonus->getType()] = rankBonus;
		m_RankBonusFlag.set( rankBonus->getType() );
	}
	else
	{
		SAFE_DELETE( rankBonus );
	}

	__END_CATCH
}

void PlayerCreature::clearRankBonus()
	throw()
{
	__BEGIN_TRY

	HashMapRankBonusItor itr = m_RankBonuses.begin();
	for ( ; itr != m_RankBonuses.end(); itr++ )
	{
		SAFE_DELETE( itr->second );
	}

	m_RankBonuses.clear();
	m_RankBonusFlag.reset();

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "DELETE FROM RankBonusData WHERE OwnerID = '%s'", getName().c_str() );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	__END_CATCH
}

RankBonus* PlayerCreature::getRankBonusByRank( Rank_t rank ) const
	throw()
{
	__BEGIN_TRY

	HashMapRankBonusConstItor itr = m_RankBonuses.begin();

	for ( ; itr != m_RankBonuses.end(); itr++ )
	{
		RankBonus* pLearnedRankBonus = itr->second;

		if ( rank == pLearnedRankBonus->getRank() )
		{
			return pLearnedRankBonus;
		}
	}

	return NULL;

	__END_CATCH
}

void PlayerCreature::clearRankBonus( Rank_t rank )
	throw()
{
	__BEGIN_TRY

	RankBonus* pRankBonus = getRankBonusByRank( rank );
	if ( pRankBonus == NULL )
		return;

	DWORD rankBonusType = pRankBonus->getType();

	HashMapRankBonusItor itr = m_RankBonuses.find( rankBonusType );
	if ( itr != m_RankBonuses.end() )
	{
		m_RankBonusFlag.reset( rankBonusType );
		SAFE_DELETE( itr->second );
		m_RankBonuses.erase( itr );
	}

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "DELETE FROM RankBonusData WHERE OwnerID = '%s' AND Type = %d", getName().c_str(), (int)rankBonusType );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	__END_CATCH
}

bool PlayerCreature::learnRankBonus( DWORD type )
	throw()
{
	__BEGIN_TRY

	// 같은 Rank 의 Bonus 가 있다면 배울 수 없다.
	HashMapRankBonusConstItor itr = m_RankBonuses.begin();

	RankBonusInfo* pRankBonusInfo = NULL;

	try
	{
		pRankBonusInfo = g_pRankBonusInfoManager->getRankBonusInfo( type );
	}
	catch ( NoSuchElementException& nsee )
	{
		filelog( "rankBonusError.txt", "No Such Element. Name : %s, Type = %u", m_Owner.c_str(), type );
		return false;
	}
	catch ( OutOfBoundException& oobe )
	{
		filelog( "rankBonusError.txt", "Out of Bound. Name : %s, Type = %u", m_Owner.c_str(), type );
		return false;
	}

	// 종족 검사. 0이 슬레이어. 1이 뱀파이어
	bool bValidRace = isSlayer() && pRankBonusInfo->getRace() == 0
						|| isVampire() && pRankBonusInfo->getRace() == 1
						|| isOusters() && pRankBonusInfo->getRace() == 2;

	// 계급 검사
	if ( getRank() < pRankBonusInfo->getRank() )
		return false;

	if (!bValidRace)
		return false;

	for ( ; itr != m_RankBonuses.end(); itr++ )
	{
		RankBonus* pLearnedRankBonus = itr->second;

		if ( pRankBonusInfo->getRank() == pLearnedRankBonus->getRank() )
		{
			DWORD type = pLearnedRankBonus->getType();

			RankBonusInfo* pLearnedRankBonusInfo = g_pRankBonusInfoManager->getRankBonusInfo( type );

			// 같은 종족의 같은 수준의 기술은 못 배운다.
			if ( pRankBonusInfo->getRace()==pLearnedRankBonusInfo->getRace())
			{
				return false;
			}
		}
	}
	
	RankBonus* rankBonus = new RankBonus( pRankBonusInfo->getType(), pRankBonusInfo->getPoint(), pRankBonusInfo->getRank() );

	addRankBonus( rankBonus );

	// DB 에 추가
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "INSERT INTO RankBonusData ( OwnerID, Type )  VALUES ( '%s', %d )", getName().c_str(), type );

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return true;

	__END_CATCH
}

void PlayerCreature::sendRankBonusInfo()
	throw()
{
	__BEGIN_TRY

	HashMapRankBonusConstItor itr = m_RankBonuses.begin();

	GCRankBonusInfo gcRankBonusInfo;

	for ( ; itr != m_RankBonuses.end(); itr++ )
	{
		gcRankBonusInfo.addListElement( itr->second->getType() );
	}

	m_pPlayer->sendPacket( &gcRankBonusInfo );

	__END_CATCH
}

void PlayerCreature::loadRankBonus()
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result* pResult = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery( "SELECT Type FROM RankBonusData WHERE OwnerID ='%s'", getName().c_str() );

		while ( pResult->next() )
		{
			DWORD rankBonusType = pResult->getInt(1);

			RankBonusInfo* pRankBonusInfo = g_pRankBonusInfoManager->getRankBonusInfo( rankBonusType );

			if ( getRace() == pRankBonusInfo->getRace() )
			{
				RankBonus* pRankBonus = new RankBonus();

				pRankBonus->setType( rankBonusType );
				pRankBonus->setPoint( pRankBonusInfo->getPoint() );
				pRankBonus->setRank( pRankBonusInfo->getRank() );

				addRankBonus( pRankBonus );
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
		
	__END_CATCH
}
void PlayerCreature::loadDianJuan()
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result* pResult = NULL;
	bool found = false;
	string safePlayerID;

	setDianJuan(0);
	if (!ensureMarketCurrencyRow("PlayerCreature::loadDianJuan"))
	{
		filelog("MarketCurrencyError.log", "[PlayerCreature::loadDianJuan] failed to ensure userinfo row player=%s", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>");
		return;
	}
	safePlayerID = ReplaceDBString(getPlayer()->getID());
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery( "SELECT havecount FROM userinfo WHERE username ='%s'", safePlayerID.c_str());

		while ( pResult->next() )
		{
			setDianJuan(pResult->getInt(1));
			found = true;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	if (!found)
	{
		filelog("MarketCurrencyError.log", "[PlayerCreature::loadDianJuan] userinfo row missing after repair player=%s", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>");
	}
		
	__END_CATCH
}
void PlayerCreature::loadJiFen()
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result* pResult = NULL;
	bool found = false;
	string safePlayerID;

	setJiFen(0);
	if (!ensureMarketCurrencyRow("PlayerCreature::loadJiFen"))
	{
		filelog("MarketCurrencyError.log", "[PlayerCreature::loadJiFen] failed to ensure userinfo row player=%s", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>");
		return;
	}
	safePlayerID = ReplaceDBString(getPlayer()->getID());
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery( "SELECT havejf FROM userinfo WHERE username ='%s'", safePlayerID.c_str());

		while ( pResult->next() )
		{
			setJiFen(pResult->getInt(1));
			found = true;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	if (!found)
	{
		filelog("MarketCurrencyError.log", "[PlayerCreature::loadJiFen] userinfo row missing after repair player=%s", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>");
	}
		
	__END_CATCH
}

bool PlayerCreature::ensureMarketCurrencyRow(const char* context)
	throw()
{
	__BEGIN_TRY

	Player* pPlayer = getPlayer();
	if (pPlayer == NULL)
	{
		filelog("MarketCurrencyError.log", "[%s] player pointer is NULL", context != NULL ? context : "ensureMarketCurrencyRow");
		return false;
	}

	string safePlayerID = ReplaceDBString(pPlayer->getID());
	Statement* pStmt = NULL;
	bool ok = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery(
			"INSERT IGNORE INTO userinfo (username, havecount, havejf, paymoney, zbmoney, regip, password) "
			"VALUES ('%s', 0, 0, 0, 0, '', '')",
			safePlayerID.c_str()
		);
		ok = true;
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return ok;

	__END_CATCH
}

bool PlayerCreature::updateMarketCurrency(const char* fieldName, const char* currencyName, Gold_t amount, bool increase, Gold_t& newValue, const char* context)
	throw()
{
	__BEGIN_TRY

	newValue = 0;

	Player* pPlayer = getPlayer();
	if (pPlayer == NULL)
	{
		filelog("MarketCurrencyError.log", "[%s] player pointer is NULL currency=%s", context != NULL ? context : "updateMarketCurrency", currencyName != NULL ? currencyName : "<null>");
		return false;
	}

	if (amount == 0)
	{
		newValue = (currencyName != NULL && strcmp(currencyName, "MarketPoint") == 0) ? getJiFen() : getDianJuan();
		return true;
	}

	if (!ensureMarketCurrencyRow(context))
	{
		filelog("MarketCurrencyError.log", "[%s] cannot repair userinfo row player=%s currency=%s amount=%u",
			context != NULL ? context : "updateMarketCurrency",
			pPlayer->getID().c_str(),
			currencyName != NULL ? currencyName : "<null>",
			(uint)amount
		);
		return false;
	}

	string safePlayerID = ReplaceDBString(pPlayer->getID());
	Statement* pStmt = NULL;
	Result* pResult = NULL;
	bool ok = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT %s FROM userinfo WHERE username='%s'", fieldName, safePlayerID.c_str());

		if (!pResult->next())
		{
			filelog("MarketCurrencyError.log", "[%s] userinfo row missing after repair player=%s currency=%s",
				context != NULL ? context : "updateMarketCurrency",
				pPlayer->getID().c_str(),
				currencyName != NULL ? currencyName : "<null>"
			);
			SAFE_DELETE(pStmt);
			return false;
		}

		Gold_t dbValue = pResult->getInt(1);
		if (increase)
		{
			if (dbValue > MAX_MONEY - amount)
				newValue = MAX_MONEY;
			else
				newValue = dbValue + amount;
		}
		else
		{
			if (dbValue < amount)
			{
				filelog("MarketCurrencyError.log", "[%s] insufficient DB balance player=%s currency=%s current=%u amount=%u",
					context != NULL ? context : "updateMarketCurrency",
					pPlayer->getID().c_str(),
					currencyName != NULL ? currencyName : "<null>",
					(uint)dbValue,
					(uint)amount
				);
				if (currencyName != NULL && strcmp(currencyName, "MarketPoint") == 0)
					setJiFen(dbValue);
				else
					setDianJuan(dbValue);
				SAFE_DELETE(pStmt);
				return false;
			}

			newValue = dbValue - amount;
		}

		if (newValue != dbValue)
		{
			pStmt->executeQuery("UPDATE userinfo SET %s=%u WHERE username='%s'", fieldName, (uint)newValue, safePlayerID.c_str());
			if (pStmt->getAffectedRowCount() != 1)
			{
				filelog("MarketCurrencyError.log", "[%s] update affected=%u player=%s currency=%s old=%u new=%u",
					context != NULL ? context : "updateMarketCurrency",
					pStmt->getAffectedRowCount(),
					pPlayer->getID().c_str(),
					currencyName != NULL ? currencyName : "<null>",
					(uint)dbValue,
					(uint)newValue
				);
				if (currencyName != NULL && strcmp(currencyName, "MarketPoint") == 0)
					setJiFen(dbValue);
				else
					setDianJuan(dbValue);
				SAFE_DELETE(pStmt);
				return false;
			}
		}

		ok = true;
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return ok;

	__END_CATCH
}

bool PlayerCreature::increaseMarketCashEx(Gold_t amount)
	throw()
{
	__BEGIN_TRY

	Gold_t newValue = getDianJuan();
	if (!updateMarketCurrency("havecount", "MarketCash", amount, true, newValue, "PlayerCreature::increaseMarketCashEx"))
		return false;

	setDianJuan(newValue);
	return true;

	__END_CATCH
}

bool PlayerCreature::decreaseMarketCashEx(Gold_t amount)
	throw()
{
	__BEGIN_TRY

	Gold_t newValue = getDianJuan();
	if (!updateMarketCurrency("havecount", "MarketCash", amount, false, newValue, "PlayerCreature::decreaseMarketCashEx"))
		return false;

	setDianJuan(newValue);
	return true;

	__END_CATCH
}

bool PlayerCreature::increaseMarketPointEx(Gold_t amount)
	throw()
{
	__BEGIN_TRY

	Gold_t newValue = getJiFen();
	if (!updateMarketCurrency("havejf", "MarketPoint", amount, true, newValue, "PlayerCreature::increaseMarketPointEx"))
		return false;

	setJiFen(newValue);
	return true;

	__END_CATCH
}

bool PlayerCreature::decreaseMarketPointEx(Gold_t amount)
	throw()
{
	__BEGIN_TRY

	Gold_t newValue = getJiFen();
	if (!updateMarketCurrency("havejf", "MarketPoint", amount, false, newValue, "PlayerCreature::decreaseMarketPointEx"))
		return false;

	setJiFen(newValue);
	return true;

	__END_CATCH
}
void PlayerCreature::increaseRankExp(RankExp_t Point, bool isItemBouns)
{
	if (Point <= 0) return;

	// PK 존 안에서는 경험치를 주지 않는다.
	if ( g_pPKZoneInfoManager->isPKZone( getZoneID() ) )
		return;

	if ( m_pZone != NULL && !m_pZone->IsGiveExperience() )
		return;

	if(isItemBouns == false)
	{
		if ( isFlag( Effect::EFFECT_CLASS_RANK_BONUS_EXP_RED )) Point*=1.25f;
		if ( isFlag(Effect::EFFECT_CLASS_RANK_BONUS_EXP_BLUE )) Point*=5;
		if ( isFlag( Effect::EFFECT_CLASS_BONUS_EXP ) )
		{
			EffectBonusExp *pEffect = dynamic_cast<EffectBonusExp *>(findEffect(Effect::EFFECT_CLASS_BONUS_EXP));
											
			if ( pEffect != NULL )
			{
				Point = (Exp_t)((float)Point * pEffect->GetBonusRate());	
			}
		}
		if ( g_pTimeChecker->isInPeriod( TIME_PERIOD_RANKEXP_UP ) ) Point*=2;
	}

	if ( m_pRank->increaseExp(Point) )
	{
		char pField[80];
		sprintf(pField, "`Rank`=%u, RankExp=%lu, RankGoalExp=%lu",
				getRank(), getRankExp(), getRankGoalExp());
		tinysave(pField);
		setRankExpSaveCount(0);

		GCModifyInformation gcModifyInformation;
		gcModifyInformation.addLongData(MODIFY_RANK, getRank());
		m_pPlayer->sendPacket(&gcModifyInformation);

		if (m_pZone != NULL)
		{
			GCOtherModifyInfo gcOtherModifyInfo;
			gcOtherModifyInfo.setObjectID(getObjectID());
			gcOtherModifyInfo.addShortData(MODIFY_RANK, getRank());

			m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
		}
		int PlayerRank = getRank();
		bool UpdateRankGemData = false;
		int Slot =0;
		switch(PlayerRank)
		{
	          case 5:
			  Slot =0;
			  UpdateRankGemData = false;
		  break; 
	          case 6:
			  Slot =1;
			  UpdateRankGemData = true;
		  break; 
	          case 10:
			  Slot =2;
			  UpdateRankGemData = false;
		  break; 
	          case 15:
			  Slot =3;
			  UpdateRankGemData = false;
		  break; 
	          case 16:
			  Slot =4;
			  UpdateRankGemData = true;
		  break; 
	          case 20:
			  Slot =5;
			  UpdateRankGemData = false;
		  break; 
	          case 25:
			  Slot =6;
			  UpdateRankGemData = false;
		  break; 
	          case 26:
			  Slot =7;
			  UpdateRankGemData = true;
		  break; 
	          case 27:
			  Slot =8;
			  UpdateRankGemData = true;
		  break; 
	          case 30:
			  Slot =9;
			  UpdateRankGemData = false;
		  break;
	          case 35:
			  Slot =10;
			  UpdateRankGemData = false;
		  break;  
	          case 36:
			  Slot =11;
			  UpdateRankGemData = true;
		  break; 
	          case 37:
			  Slot =12;
			  UpdateRankGemData = true;
		  break; 
	          case 40:
			  Slot =13;
			  UpdateRankGemData = false;
		  break;
	          case 45:
			  Slot =14;
			  UpdateRankGemData = false;
		  break;  
	          case 46:
			  Slot =15;
			  UpdateRankGemData = true;
		  break; 
	          case 47:
			  Slot =16;
			  UpdateRankGemData = true;
		  break; 
	          case 50:
			  Slot =17;
			  UpdateRankGemData = false;
		  break;
	          case 51:
			  Slot =18;
			  UpdateRankGemData = true;
		  break; 
	          case 52:
			  Slot =19;
			  UpdateRankGemData = true;
		  break; 
	          case 55:
			  Slot =20;
			  UpdateRankGemData = false;
		  break;
	          case 56:
			  Slot =21;
			  UpdateRankGemData = true;
		  break; 
	          case 57:
			  Slot =22;
			  UpdateRankGemData = true;
		  break; 
	          case 60:
			  Slot =23;
			  UpdateRankGemData = false;
		  break;
	          case 61:
			  Slot =24;
			  UpdateRankGemData = true;
		  break; 
	          case 62:
			  Slot =25;
			  UpdateRankGemData = true;
		  break; 
	          case 65:
			  Slot =26;
			  UpdateRankGemData = false;
		  break;
	          case 66:
			  Slot =27;
			  UpdateRankGemData = true;
		  break; 
	          case 67:
			  Slot =28;
			  UpdateRankGemData = true;
		  break; 
	          case 70:
			  Slot =29;
			  UpdateRankGemData = false;
		  break;
	          case 71:
			  Slot =30;
			  UpdateRankGemData = true;
		  break;
	          case 72:
			  Slot =31;
			  UpdateRankGemData = true;
		  break; 
	          case 73:
			  Slot =32;
			  UpdateRankGemData = true;
		  break; 
	          case 75:
			  Slot =33;
			  UpdateRankGemData = false;
		  break;
	          case 76:
			  Slot =34;
			  UpdateRankGemData = true;
		  break;
	          case 77:
			  Slot =35;
			  UpdateRankGemData = true;
		  break; 
	          case 78:
			  Slot =36;
			  UpdateRankGemData = true;
		  break; 
	          case 80:
			  Slot =37;
			  UpdateRankGemData = false;
		  break;
	          case 81:
			  Slot =38;
			  UpdateRankGemData = true;
		  break;
	          case 82:
			  Slot =39;
			  UpdateRankGemData = true;
		  break; 
	          case 83:
			  Slot =40;
			  UpdateRankGemData = true;
		  break; 
	          case 85:
			  Slot =41;
			  UpdateRankGemData = false;
		  break;
	          case 86:
			  Slot =42;
			  UpdateRankGemData = true;
		  break;
	          case 87:
			  Slot =43;
			  UpdateRankGemData = true;
		  break; 
	          case 88:
			  Slot =44;
			  UpdateRankGemData = true;
		  break; 
	          case 90:
			  Slot =45;
			  UpdateRankGemData = false;
		  break;
	          case 91:
			  Slot =46;
			  UpdateRankGemData = true;
		  break;
	          case 92:
			  Slot =47;
			  UpdateRankGemData = true;
		  break;
	          case 93:
			  Slot =48;
			  UpdateRankGemData = true;
		  break; 
	          case 94:
			  Slot =49;
			  UpdateRankGemData = true;
		  break; 
	          case 95:
			  Slot =50;
			  UpdateRankGemData = false;
		  break;
	          case 96:
			  Slot =51;
			  UpdateRankGemData = true;
		  break;
	          case 97:
			  Slot =52;
			  UpdateRankGemData = true;
		  break; 
	          case 98:
			  Slot =53;
			  UpdateRankGemData = true;
		  break; 
	          case 99:
			  Slot =54;
			  UpdateRankGemData = true;
		  break;
	          case 100:
			  Slot =55;
			  UpdateRankGemData = false;
		  break;
		}
		if(UpdateRankGemData == true)
			{
				Statement* pStmt = NULL;

				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
					pStmt->executeQuery( "INSERT INTO RankGemData (Name, Race, Slot,Type,Grade,OptionClass) values('%s','%d','%d',999,999,999)", getName().c_str(),  getRace(), Slot);
					SAFE_DELETE( pStmt );
				}
				END_DB(pStmt)
			}
	}
	else
	{
		WORD rankExpSaveCount = getRankExpSaveCount();
		if (rankExpSaveCount > RANK_EXP_SAVE_PERIOD)
		{
			char pField[80];
			sprintf(pField, "RankExp=%lu, RankGoalExp=%lu", getRankExp(), getRankGoalExp());
			tinysave(pField);

			rankExpSaveCount = 0;
		}
		else rankExpSaveCount++;
		setRankExpSaveCount(rankExpSaveCount);

		// 계급 경험치를 보내준다. by sigi. 2002.9.13
		GCModifyInformation gcModifyInformation;
		gcModifyInformation.addLongData(MODIFY_RANK_EXP, getRankGoalExp());
		m_pPlayer->sendPacket(&gcModifyInformation);
	}
}

bool PlayerCreature::isBillingPlayAvaiable() 
	throw(Error)
{
	__BEGIN_TRY

	if (m_pPlayer==NULL)
		return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
	Assert(pGamePlayer!=NULL);

	return pGamePlayer->isBillingPlayAvaiable();

	__END_CATCH
}


bool PlayerCreature::isPayPlayAvaiable() 
	throw(Error)
{
	__BEGIN_TRY

	if (m_pPlayer==NULL)
		return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
	return pGamePlayer->isPayPlaying();

	__END_CATCH
}


bool PlayerCreature::canPlayFree()
	throw(Error)
{
	return false;
}

void PlayerCreature::loadGoods()
	throw(Error)
{
	__BEGIN_TRY
	
	Statement* pStmt = NULL;
	Result* pResult = NULL;

	if ( m_pGoodsInventory->getNum() != 0 )
	{
		//filelog("GoodsReload.log", "마켓아템 리로드 했냐? : %s", getName().c_str() );
		// 이제 리로드 한다. by bezz. 2005.09.16
		m_pGoodsInventory->clear();
	}

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();

		pResult = pStmt->executeQuery( "SELECT ID, GoodsID, Num FROM GoodsListObject WHERE World = %d AND PlayerID = '%s' AND Name = '%s' AND Status = 'NOT'",
										g_pConfig->getPropertyInt("WorldID"),
										getPlayer()->getID().c_str(),
										getName().c_str() );

		while ( pResult->next() )
		{
			string ID = pResult->getString(1);
			DWORD goodsID = pResult->getInt(2);
			int num = pResult->getInt(3);

			for ( int i = 0; i < max(1,min(50,num)) ; i++ )
			{
				Item* pItem = createItemByGoodsID( goodsID );
				if ( pItem != NULL )
				{
					m_pGoodsInventory->addItem( ID, pItem );
				}
			}
		}

		pResult = pStmt->executeQuery( "SELECT ID, GoodsID, Num FROM GoodsListObject WHERE World = %d AND PlayerID = '%s' AND Name = '' AND Status = 'NOT'",
										g_pConfig->getPropertyInt("WorldID"),
										getPlayer()->getID().c_str() );

		while ( pResult->next() )
		{
			string ID = pResult->getString(1);
			DWORD goodsID = pResult->getInt(2);
			int num = pResult->getInt(3);

			for ( int i = 0; i < max(1,min(50,num)) ; i++ )
			{
				Item* pItem = createItemByGoodsID( goodsID );
				if ( pItem != NULL )
				{
					m_pGoodsInventory->addItem( ID, pItem );
				}
			}
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt)

	loadPhoenixMonsterCounter();

	__END_CATCH
}


namespace
{
    const char* const PHOENIX_COUNTER_MONSTER_TYPE = "MONSTER_KILL";
    const char* const PHOENIX_COUNTER_REWARD_BUYID = "PHOENIX_MONSTER_COUNTER";

    bool CanFitPhoenixCounterReward(Inventory* pInventory, Item* pSampleItem, int count)
    {
        if ( pInventory == NULL || pSampleItem == NULL || count <= 0 ) return false;

        const int inventoryWidth = pInventory->getWidth();
        const int inventoryHeight = pInventory->getHeight();
        const int itemWidth = pSampleItem->getVolumeWidth();
        const int itemHeight = pSampleItem->getVolumeHeight();

        if ( inventoryWidth <= 0 || inventoryHeight <= 0 ) return false;
        if ( inventoryWidth > MAX_INVEN_WIDTH || inventoryHeight > MAX_INVEN_HEIGHT ) return false;
        if ( itemWidth <= 0 || itemHeight <= 0 ) return false;
        if ( itemWidth > inventoryWidth || itemHeight > inventoryHeight ) return false;

        bool occupied[MAX_INVEN_WIDTH][MAX_INVEN_HEIGHT] = { false };

        for ( int x = 0; x < inventoryWidth; ++x )
        {
            for ( int y = 0; y < inventoryHeight; ++y )
            {
                occupied[x][y] = ( pInventory->getInventorySlot(x, y).getItem() != NULL );
            }
        }

        for ( int itemIndex = 0; itemIndex < count; ++itemIndex )
        {
            bool placed = false;

            for ( int x = 0; x <= inventoryWidth - itemWidth && !placed; ++x )
            {
                for ( int y = 0; y <= inventoryHeight - itemHeight && !placed; ++y )
                {
                    bool canPlace = true;

                    for ( int dx = 0; dx < itemWidth && canPlace; ++dx )
                    {
                        for ( int dy = 0; dy < itemHeight; ++dy )
                        {
                            if ( occupied[x + dx][y + dy] )
                            {
                                canPlace = false;
                                break;
                            }
                        }
                    }

                    if ( canPlace )
                    {
                        for ( int dx = 0; dx < itemWidth; ++dx )
                        {
                            for ( int dy = 0; dy < itemHeight; ++dy )
                            {
                                occupied[x + dx][y + dy] = true;
                            }
                        }

                        placed = true;
                    }
                }
            }

            if ( !placed ) return false;
        }

        return true;
    }
}

void PlayerCreature::loadPhoenixMonsterCounter()
    throw(Error)
{
    __BEGIN_TRY

    m_PhoenixMonsterCounterEnabled = false;
    m_PhoenixMonsterCounterRewarded = false;
    m_PhoenixMonsterCounter = 0;
    m_PhoenixMonsterCounterGoal = 2000;
    m_PhoenixMonsterCounterRewardGoodsID = 0;
    m_PhoenixMonsterCounterRewardGoodsCount = 1;
    m_PhoenixMonsterCounterMaxLevelGap = 30;

    if ( getPlayer() == NULL )
    {
        return;
    }

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();

        pResult = pStmt->executeQuery( "SHOW TABLES LIKE 'PhoenixCounterConfig'" );
        if ( !pResult->next() )
        {
            SAFE_DELETE( pStmt );
            return;
        }

        pResult = pStmt->executeQuery( "SHOW TABLES LIKE 'PhoenixCounterProgress'" );
        if ( !pResult->next() )
        {
            SAFE_DELETE( pStmt );
            return;
        }

        pResult = pStmt->executeQuery(
            "SELECT Enabled, GoalCount, MaxLevelGap, RewardGoodsID, RewardGoodsCount "
            "FROM PhoenixCounterConfig WHERE CounterType = '%s'",
            PHOENIX_COUNTER_MONSTER_TYPE
        );

        if ( pResult->next() )
        {
            string enabled = pResult->getString(1);
            m_PhoenixMonsterCounterEnabled = ( enabled == "YES" || enabled == "yes" || enabled == "1" );
            m_PhoenixMonsterCounterGoal = pResult->getInt(2);
            m_PhoenixMonsterCounterMaxLevelGap = pResult->getInt(3);
            m_PhoenixMonsterCounterRewardGoodsID = pResult->getInt(4);
            m_PhoenixMonsterCounterRewardGoodsCount = pResult->getInt(5);
        }
        else
        {
            pStmt->executeQuery(
                "INSERT INTO PhoenixCounterConfig "
                "(CounterType, Enabled, GoalCount, MaxLevelGap, RewardGoodsID, RewardGoodsCount, UpdatedAt) "
                "VALUES ('%s', 'YES', %u, %d, %u, %d, now())",
                PHOENIX_COUNTER_MONSTER_TYPE,
                m_PhoenixMonsterCounterGoal,
                m_PhoenixMonsterCounterMaxLevelGap,
                m_PhoenixMonsterCounterRewardGoodsID,
                m_PhoenixMonsterCounterRewardGoodsCount
            );
            m_PhoenixMonsterCounterEnabled = true;
        }

        if ( m_PhoenixMonsterCounterGoal == 0 ) m_PhoenixMonsterCounterGoal = 2000;
        if ( m_PhoenixMonsterCounterRewardGoodsCount <= 0 ) m_PhoenixMonsterCounterRewardGoodsCount = 1;
        if ( m_PhoenixMonsterCounterRewardGoodsCount > 50 ) m_PhoenixMonsterCounterRewardGoodsCount = 50;

        pResult = pStmt->executeQuery(
            "SELECT CurrentCount, Rewarded FROM PhoenixCounterProgress "
            "WHERE World = %d AND PlayerID = '%s' AND Name = '%s' AND CounterType = '%s'",
            g_pConfig->getPropertyInt("WorldID"),
            getPlayer()->getID().c_str(),
            getName().c_str(),
            PHOENIX_COUNTER_MONSTER_TYPE
        );

        if ( pResult->next() )
        {
            m_PhoenixMonsterCounter = pResult->getInt(1);
            string rewarded = pResult->getString(2);
            m_PhoenixMonsterCounterRewarded = ( rewarded == "YES" || rewarded == "yes" || rewarded == "1" );
        }
        else
        {
            pStmt->executeQuery(
                "INSERT INTO PhoenixCounterProgress "
                "(World, PlayerID, Name, CounterType, CurrentCount, Rewarded, RewardDate, UpdatedAt) "
                "VALUES (%d, '%s', '%s', '%s', 0, 'NO', NULL, now())",
                g_pConfig->getPropertyInt("WorldID"),
                getPlayer()->getID().c_str(),
                getName().c_str(),
                PHOENIX_COUNTER_MONSTER_TYPE
            );
        }

        SAFE_DELETE( pStmt );
    }
    END_DB(pStmt)

    if ( m_PhoenixMonsterCounter > m_PhoenixMonsterCounterGoal )
        m_PhoenixMonsterCounter = m_PhoenixMonsterCounterGoal;

    __END_CATCH
}

void PlayerCreature::sendPhoenixMonsterCounter()
    throw(Error)
{
    __BEGIN_TRY

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
    if ( pGamePlayer == NULL ) return;

    GCModifyInformation gcMI;
    gcMI.addLongData( MODIFY_MONSTER_COUNTER, m_PhoenixMonsterCounter );
    gcMI.addLongData( MODIFY_MONSTER_COUNTER_GOAL, m_PhoenixMonsterCounterGoal );
    pGamePlayer->sendPacket( &gcMI );

    __END_CATCH
}

bool PlayerCreature::resetPhoenixMonsterCounterCycle()
    throw(Error)
{
    __BEGIN_TRY

    if ( getPlayer() == NULL )
    {
        filelog("PhoenixCounter.log", "Cannot reset monster counter for %s: missing Player object", getName().c_str());
        return false;
    }

    Statement* pStmt = NULL;
    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();
        pStmt->executeQuery(
            "INSERT INTO PhoenixCounterProgress "
            "(World, PlayerID, Name, CounterType, CurrentCount, Rewarded, RewardDate, UpdatedAt) "
            "VALUES (%d, '%s', '%s', '%s', 0, 'NO', now(), now()) "
            "ON DUPLICATE KEY UPDATE CurrentCount = 0, Rewarded = 'NO', RewardDate = now(), UpdatedAt = now()",
            g_pConfig->getPropertyInt("WorldID"),
            getPlayer()->getID().c_str(),
            getName().c_str(),
            PHOENIX_COUNTER_MONSTER_TYPE
        );
        SAFE_DELETE( pStmt );
    }
    END_DB(pStmt)

    m_PhoenixMonsterCounter = 0;
    m_PhoenixMonsterCounterRewarded = false;
    return true;

    __END_CATCH

    return false;
}
void PlayerCreature::increasePhoenixMonsterCounter(Monster* pMonster, int playerLevel, int monsterLevel)
    throw(Error)
{
    __BEGIN_TRY

    if ( !m_PhoenixMonsterCounterEnabled || pMonster == NULL ) return;
    if ( m_PhoenixMonsterCounterGoal == 0 ) return;
    if ( playerLevel <= 0 || monsterLevel <= 0 ) return;

    if ( m_PhoenixMonsterCounterMaxLevelGap >= 0
        && playerLevel > monsterLevel + m_PhoenixMonsterCounterMaxLevelGap )
    {
        return;
    }

    if ( m_PhoenixMonsterCounterRewarded )
    {
        if ( !resetPhoenixMonsterCounterCycle() )
        {
            sendPhoenixMonsterCounter();
            return;
        }
    }

    if ( m_PhoenixMonsterCounter >= m_PhoenixMonsterCounterGoal )
    {
        if ( !m_PhoenixMonsterCounterRewarded && m_PhoenixMonsterCounterRewardGoodsID != 0 )
        {
            if ( givePhoenixMonsterCounterReward() )
            {
                if ( !resetPhoenixMonsterCounterCycle() )
                {
                    filelog("PhoenixCounter.log", "Reward delivered but counter reset failed for %s(%s)",
                        getPlayer()->getID().c_str(),
                        getName().c_str());
                }
            }
        }
        sendPhoenixMonsterCounter();
        return;
    }

    ++m_PhoenixMonsterCounter;
    if ( m_PhoenixMonsterCounter > m_PhoenixMonsterCounterGoal )
        m_PhoenixMonsterCounter = m_PhoenixMonsterCounterGoal;

    Statement* pStmt = NULL;
    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();
        pStmt->executeQuery(
            "INSERT INTO PhoenixCounterProgress "
            "(World, PlayerID, Name, CounterType, CurrentCount, Rewarded, RewardDate, UpdatedAt) "
            "VALUES (%d, '%s', '%s', '%s', %u, '%s', NULL, now()) "
            "ON DUPLICATE KEY UPDATE CurrentCount = %u, UpdatedAt = now()",
            g_pConfig->getPropertyInt("WorldID"),
            getPlayer()->getID().c_str(),
            getName().c_str(),
            PHOENIX_COUNTER_MONSTER_TYPE,
            m_PhoenixMonsterCounter,
            m_PhoenixMonsterCounterRewarded ? "YES" : "NO",
            m_PhoenixMonsterCounter
        );
        SAFE_DELETE( pStmt );
    }
    END_DB(pStmt)

    if ( !m_PhoenixMonsterCounterRewarded
        && m_PhoenixMonsterCounter >= m_PhoenixMonsterCounterGoal
        && m_PhoenixMonsterCounterRewardGoodsID != 0 )
    {
        if ( givePhoenixMonsterCounterReward() )
        {
            if ( !resetPhoenixMonsterCounterCycle() )
            {
                filelog("PhoenixCounter.log", "Reward delivered but counter reset failed for %s(%s)",
                    getPlayer()->getID().c_str(),
                    getName().c_str());
            }
        }
    }

    sendPhoenixMonsterCounter();

    __END_CATCH
}

bool PlayerCreature::givePhoenixMonsterCounterReward()
    throw(Error)
{
    __BEGIN_TRY

    if ( m_PhoenixMonsterCounterRewarded ) return true;
    if ( m_PhoenixMonsterCounterRewardGoodsID == 0 ) return false;
    if ( m_PhoenixMonsterCounterRewardGoodsCount <= 0 ) m_PhoenixMonsterCounterRewardGoodsCount = 1;
    if ( m_PhoenixMonsterCounterRewardGoodsCount > 50 ) m_PhoenixMonsterCounterRewardGoodsCount = 50;
    if ( getPlayer() == NULL )
    {
        filelog("PhoenixCounter.log", "Cannot reward %s: missing Player object", getName().c_str());
        return false;
    }

    Statement* pStmt = NULL;
    Result* pResult = NULL;
    bool validGoods = false;
    bool rewardSentToCart = false;
    bool rewardSentToInventory = false;

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();
        pResult = pStmt->executeQuery(
            "SELECT GoodsID FROM GoodsListInfo WHERE GoodsID = %u",
            m_PhoenixMonsterCounterRewardGoodsID
        );
        validGoods = pResult->next();

        SAFE_DELETE( pStmt );
    }
    END_DB(pStmt)

    if ( !validGoods )
    {
        filelog("PhoenixCounter.log", "Invalid RewardGoodsID=%u for %s(%s)",
            m_PhoenixMonsterCounterRewardGoodsID,
            getPlayer()->getID().c_str(),
            getName().c_str());
        return false;
    }

    Zone* pZone = getZone();
    Inventory* pInventory = getInventory();
    Item* pSampleItem = createItemByGoodsID( m_PhoenixMonsterCounterRewardGoodsID );
    if ( pSampleItem == NULL )
    {
        filelog("PhoenixCounter.log", "Cannot create sample RewardGoodsID=%u for %s(%s)",
            m_PhoenixMonsterCounterRewardGoodsID,
            getPlayer()->getID().c_str(),
            getName().c_str());
        return false;
    }

    bool canGiveToInventory = ( pZone != NULL
        && pInventory != NULL
        && CanFitPhoenixCounterReward(pInventory, pSampleItem, m_PhoenixMonsterCounterRewardGoodsCount) );
    SAFE_DELETE( pSampleItem );

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

    if ( canGiveToInventory )
    {
        for ( int i = 0; i < m_PhoenixMonsterCounterRewardGoodsCount; ++i )
        {
            Item* pItem = createItemByGoodsID( m_PhoenixMonsterCounterRewardGoodsID );
            if ( pItem == NULL )
            {
                filelog("PhoenixCounter.log", "Cannot create RewardGoodsID=%u itemIndex=%d for %s(%s)",
                    m_PhoenixMonsterCounterRewardGoodsID,
                    i,
                    getPlayer()->getID().c_str(),
                    getName().c_str());
                return false;
            }

            TPOINT pt;
            if ( !pInventory->getEmptySlot(pItem, pt) )
            {
                SAFE_DELETE( pItem );
                filelog("PhoenixCounter.log", "Inventory preflight changed during reward for %s(%s), goodsID=%u",
                    getPlayer()->getID().c_str(),
                    getName().c_str(),
                    m_PhoenixMonsterCounterRewardGoodsID);
                return false;
            }

            pItem->setTraceItem( bTraceLog( pItem ) );
            pZone->registerObject( pItem );

            if ( !pInventory->addItem(pt.x, pt.y, pItem) )
            {
                SAFE_DELETE( pItem );
                filelog("PhoenixCounter.log", "Inventory add failed after preflight for %s(%s), goodsID=%u, x=%d, y=%d",
                    getPlayer()->getID().c_str(),
                    getName().c_str(),
                    m_PhoenixMonsterCounterRewardGoodsID,
                    pt.x,
                    pt.y);
                return false;
            }

            pItem->create( getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
            pItem->whenPCTake( this );

            GCCreateItem gcCreateItem;
            makeGCCreateItem( &gcCreateItem, pItem, pt.x, pt.y );
            getPlayer()->sendPacket( &gcCreateItem );

            if ( pItem->isTimeLimitItem() )
            {
                unsigned long timeLimit = pItem->getHour();
                timeLimit *= 3600;
                addTimeLimitItem( pItem, timeLimit );
                sendTimeLimitItemInfo();
            }

            remainTraceLog( pItem, "MonsterCounter", getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );
        }

        rewardSentToInventory = true;
    }
    else
    {
        BEGIN_DB
        {
            pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();
            pStmt->executeQuery(
                "INSERT INTO GoodsListObject "
                "(BuyID, World, PlayerID, Name, GoodsID, Num, Status, `Count`) "
                "VALUES ('%s', %d, '%s', '%s', %u, %d, 'NOT', 0)",
                PHOENIX_COUNTER_REWARD_BUYID,
                g_pConfig->getPropertyInt("WorldID"),
                getPlayer()->getID().c_str(),
                getName().c_str(),
                m_PhoenixMonsterCounterRewardGoodsID,
                m_PhoenixMonsterCounterRewardGoodsCount
            );
            SAFE_DELETE( pStmt );
        }
        END_DB(pStmt)

        rewardSentToCart = true;
    }

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection( "PLAYER_DB" )->createStatement();
        pStmt->executeQuery(
            "INSERT INTO PhoenixCounterProgress "
            "(World, PlayerID, Name, CounterType, CurrentCount, Rewarded, RewardDate, UpdatedAt) "
            "VALUES (%d, '%s', '%s', '%s', %u, 'YES', now(), now()) "
            "ON DUPLICATE KEY UPDATE Rewarded = 'YES', RewardDate = now(), UpdatedAt = now()",
            g_pConfig->getPropertyInt("WorldID"),
            getPlayer()->getID().c_str(),
            getName().c_str(),
            PHOENIX_COUNTER_MONSTER_TYPE,
            m_PhoenixMonsterCounter
        );
        SAFE_DELETE( pStmt );
    }
    END_DB(pStmt)

    m_PhoenixMonsterCounterRewarded = true;

    if ( rewardSentToCart )
    {
        loadGoods();
        if ( getZone() != NULL )
        {
            registerGoodsInventory( getZone()->getObjectRegistry() );
        }
    }

    if ( pGamePlayer != NULL )
    {
        GCSystemMessage gcSystemMessage;
        if ( rewardSentToInventory )
        {
            gcSystemMessage.setMessage("Monster Counter reward has been added to your inventory.");
        }
        else
        {
            gcSystemMessage.setMessage("Monster Counter reward has been sent to Shop Basket because your inventory is full.");
        }
        pGamePlayer->sendPacket( &gcSystemMessage );
    }

    return true;

    __END_CATCH
}

/*void	PlayerCreature::loadQuest() 
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__

	SimpleQuestLoader::getInstance()->load( this );
	
#endif

	__END_CATCH
}

bool    PlayerCreature::addQuest(Quest* pQuest) 
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__
	if (m_pQuestManager==NULL)
	{
		m_pQuestManager = new QuestManager;
	}

	if (m_pQuestManager->addQuest( pQuest ))
	{
		return true;
	}

#endif
	__END_CATCH

	SAFE_DELETE(pQuest);

	return false;
}

bool    PlayerCreature::checkEvent(QuestEvent* pQuestEvent) 
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__

	if (m_pQuestManager!=NULL)
	{
		Quest* pCompleteQuest = m_pQuestManager->checkEvent( pQuestEvent );

		if (pCompleteQuest!=NULL)
		{
			//cout << "[Complete] " << pCompleteQuest->toString().c_str() << endl;
			return true;
		}
	}

#endif

	return false;

	__END_CATCH
}

Quest*  PlayerCreature::removeCompleteQuest() 
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__

	if (m_pQuestManager!=NULL)
	{
		Quest* pQuest = m_pQuestManager->removeCompleteQuest();

		return pQuest;
	}

#endif

	__END_CATCH

	return NULL;
}*/
/*
bool PlayerCreature::deleteItemNameInfoList( ObjectID_t objectID ) 
	throw(Error)
{
	__BEGIN_TRY

	list<ItemNameInfo*>::iterator itr = m_ItemNameInfoList.begin();
	
	for( ; itr != m_ItemNameInfoList.end() ; itr++ )
	{
		ItemNameInfo* pInfo = *itr;
		if( pInfo->getObjectID() == objectID )
		{
			SAFE_DELETE( pInfo );
			m_ItemNameInfoList.erase( itr );

			return true;
		}
	}

	return false;

	__END_CATCH
}

string PlayerCreature::getItemName( ObjectID_t objectID ) 
	throw(Error)
{
	__BEGIN_TRY

	list<ItemNameInfo*>::iterator itr = m_ItemNameInfoList.begin();
	
	for( ; itr != m_ItemNameInfoList.end() ; itr++ )
	{
		ItemNameInfo* pInfo = *itr;
		if( pInfo->getObjectID() == objectID )
		{
			return pInfo->getName();
		}
	}

	return NULL;

	__END_CATCH
}
*/

void PlayerCreature::addDefaultOptionSet( DefaultOptionSetType_t type )
	throw()
{
	// 이미 있는 것인지 확인한다.
	list<DefaultOptionSetType_t>::iterator itr = m_DefaultOptionSet.begin();
	for ( ; itr != m_DefaultOptionSet.end(); itr++ )
	{
		if ( (*itr) == type )
			return;
	}

	m_DefaultOptionSet.push_front( type );
}

void PlayerCreature::removeDefaultOptionSet( DefaultOptionSetType_t type )
	throw()
{
	list<DefaultOptionSetType_t>::iterator before = m_DefaultOptionSet.end();
	list<DefaultOptionSetType_t>::iterator current = m_DefaultOptionSet.begin();

	for ( ; current != m_DefaultOptionSet.end(); before = current++ )
	{
		if ( (*current) == type )
		{
			// 발견했다.
			if ( before == m_DefaultOptionSet.end() )
			{
				// delete first node
				m_DefaultOptionSet.pop_front();
			}
			else
			{
				m_DefaultOptionSet.erase(std::next(before));
			}

			return;
		}
	}

	// 발견못했다.
}

void PlayerCreature::addDefaultOptionType( OptionType_t type )
{
	m_DefaultOptions.push_front( type );
}

void PlayerCreature::removeDefaultOptionType( OptionType_t type )
{
	list<OptionType_t>::iterator before = m_DefaultOptions.end();
	list<OptionType_t>::iterator current = m_DefaultOptions.begin();
	list<OptionType_t>::iterator end = before;

	for ( ; current != end; before = current++ )
	{
		if ( (*current) == type )
		{
			// 발견했다.
			if ( before == end )
			{
				// 첫번째 노드 지우기
				m_DefaultOptions.pop_front();
			}
			else
			{
				m_DefaultOptions.erase(std::next(before));
			}

			return;
		}
	}
}

void PlayerCreature::clearDefaultOptionTypes()
{
	m_DefaultOptions.clear();
}

PetInfo* PlayerCreature::getPetInfo() const
{
	return m_pPetInfo;
}

void PlayerCreature::setPetInfo(PetInfo* pPetInfo)
{
	m_pPetInfo = pPetInfo;
	SAFE_DELETE( m_pPet );
	m_pPet = Pet::makePet(this, m_pPetInfo);
}

void PlayerCreature::heartbeat(const Timeval& currentTime) throw()
{
	if ( m_pPet != NULL ) m_pPet->heartbeat( currentTime );
	m_pGQuestManager->heartbeat();
}


GCMonsterKillQuestInfo::QuestInfo*	PlayerCreature::getPetQuestInfo() const
{
	GCMonsterKillQuestInfo::QuestInfo* pQI = new GCMonsterKillQuestInfo::QuestInfo;

	pQI->questID = PET_QUEST_ID;
	pQI->sType = m_TargetMonster;
	pQI->goal = m_TargetNum;
	pQI->timeLimit = m_TimeLimit;

	return pQI;
}

void	PlayerCreature::addPetStashItem(int idx, Item* pPetItem)
{
	Assert( pPetItem == NULL || pPetItem->getItemClass() == Item::ITEM_CLASS_PET_ITEM );
	Assert( idx >= 0 && idx <= MAX_PET_STASH );
	m_PetStash[idx] = pPetItem;
}

Item*	PlayerCreature::getPetStashItem(int idx)
{
	Assert( idx >= 0 && idx <= MAX_PET_STASH );
	return m_PetStash[idx];
}

bool PlayerCreature::canSee( Object* pObject ) const
{
	if ( pObject->getObjectClass() == OBJECT_CLASS_CREATURE )
	{
		return ::canSee( this, dynamic_cast<Creature*>(pObject) );
	}
	else
	{
		return true;
	}
}

Level_t PlayerCreature::getAdvancementClassLevel() const { return m_pAdvancementClass->getLevel(); }
Exp_t PlayerCreature::getAdvancementClassGoalExp() const { return m_pAdvancementClass->getGoalExp(); }
bool PlayerCreature::increaseAdvancementClassExp(Exp_t exp, bool bApplyExpBonus)
{
	// 다이나믹 존에서는 경험치를 먹지 않는다.
	if ( getZone() != NULL && !getZone()->IsGiveExperience() )
		return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
	if ( pGamePlayer != NULL )
	{

		writeExpLog(this, "AdvBaseExp", exp);

#ifdef __METRO_SERVER__
	/*
	if ( pGamePlayer->isPCRoomPlay() )//pZone->isPayPlay() || pZone->isPremiumZone() )
	{
		exp = getPercentValue( exp, g_pVariableManager->getPCRoomExpBonusPercent() );
	}
	*/

	if ( bApplyExpBonus )
	{
		exp = getPercentValueEx( exp, pGamePlayer->getPCRoomExpBonus() );
	}
	
	writeExpLog(this, "AdvPCRoomExp", exp);
#else
	// do nothing
#endif

		if ( bApplyExpBonus
			&& (pGamePlayer->isPremiumPlay() || pGamePlayer->isFamilyFreePass())
			)
		{
			exp = getPercentValue( exp, g_pVariableManager->getPremiumExpBonusPercent() );
		}

		writeExpLog(this, "AdvPremiumExp", exp);

		if( bApplyExpBonus
			&& g_pVariableManager->getExpRatio() > 100
			&& g_pVariableManager->getEventActivate() == 1)
		{
			exp = getPercentValue(exp, g_pVariableManager->getExpRatio());
		}

		writeExpLog(this, "AdvEventBonusExp", exp);

		if ( bApplyExpBonus && isAffectExp2X() )
		{
			// 경험치 두배
			exp <<= 1;
		}

		writeExpLog(this, "AdvAffect2XExp", exp);

		// 모래시계 이펙트가 있을 경우
		if ( bApplyExpBonus && isFlag( Effect::EFFECT_CLASS_BONUS_EXP ) )
		{
			EffectBonusExp *pEffect = dynamic_cast<EffectBonusExp *>(findEffect(Effect::EFFECT_CLASS_BONUS_EXP));
														
			if ( pEffect != NULL )
			{
				exp = (Exp_t)((float)exp * pEffect->GetBonusRate());	
			}
		}
		
		writeExpLog(this, "AdvSandWatchExp", exp);

		if ( bApplyExpBonus && isFlag( Effect::EFFECT_CLASS_CHAOTIC_STONE_EXP ) ) 
		{
			EffectChaoticExp *pEffect = dynamic_cast<EffectChaoticExp *>(findEffect(Effect::EFFECT_CLASS_CHAOTIC_STONE_EXP));
				
			if ( pEffect != NULL )
			{
				exp = (Exp_t)((float)exp * pEffect->GetBonusRate());
			}
		}

		if ( bApplyExpBonus && ( isFlag( Effect::EFFECT_CLASS_taozhuang_set_1 ) ||  isFlag( Effect::EFFECT_CLASS_taozhuang_set_2 ) ||  isFlag( Effect::EFFECT_CLASS_taozhuang_set_3 ) ||  isFlag( Effect::EFFECT_CLASS_taozhuang_set_4 ) ||  isFlag( Effect::EFFECT_CLASS_taozhuang_set_5 ) ||  isFlag( Effect::EFFECT_CLASS_taozhuang_set_6 )) )
		{
														
				exp = (Exp_t)((float)exp * 1.3);	
		}
		writeExpLog(this, "AdvTaoZhuangExp", exp);
		if ( bApplyExpBonus && isFlag( Effect::EFFECT_CLASS_NeckChain))
		{
														
				exp = (Exp_t)((float)exp * 1.2);	
		}
		
		writeExpLog(this, "AdvXiangLianTaoExp", exp);
#ifdef __CONTRIBUTION_SYSTEM__
		if( bApplyExpBonus && g_pLevelWarZoneInfoManager->isCreatureBonusZone(dynamic_cast<Creature*>(this), getZoneID() ) )
		{
			exp = exp + (exp * 0.5 * getZone()->expLevelWarBonusTimeCheck(getRace()));
		}

		writeExpLog(this, "AdvContributeExp", exp);
#endif

	}

	writeExpLog(this, "AdvFinalExp", exp);

	//cout << getName() << "에게 승직 경험치 " << (int)exp << "만큼 줍니다." << endl;
	Level_t prevLevel = getAdvancementClassLevel();
	bool ret = m_pAdvancementClass->increaseExp( exp, true, true );
	if ( getAdvancementClassLevel() > 0 ) m_bAdvanced = true;
	Level_t nextLevel = getAdvancementClassLevel();
	//cout << getName() << "이 " << (int)prevLevel << " 에서 " << (int)nextLevel << " 이 되었습니다." << endl;

	if ( prevLevel != nextLevel )
	{
		Bonus_t bonus = getBonus();
		Level_t _4pointstart = min((int)prevLevel, 50);
		Level_t _4pointend = min((int)nextLevel, 50);
		Level_t _5pointstart = max((int)prevLevel, 50);
		Level_t _5pointend = max((int)nextLevel, 50);

		Bonus_t bonusdiff = (( _4pointend - _4pointstart ) * 4) + (( _5pointend - _5pointstart ) * 5);
//		cout << getName() << "에게 " << (int)bonusdiff << "만큼 능력치 줍니다." << endl;
		bonus += bonusdiff;
		setBonus( bonus );

		StringStream sav;
		sav << "AdvancementClass = " << (int)nextLevel 
			<< ",AdvancementGoalExp = " << (int)getAdvancementClassGoalExp() 
			<< ",Bonus = " << (int)getBonus();

		tinysave( sav.toString() );
		initAllStatAndSend();
	}
	else
	{
		m_AdvancementClassExpSaveCount++;
		if ( m_AdvancementClassExpSaveCount > 100 )
		{
			StringStream sav;
			sav << "AdvancementClass = " << (int)getAdvancementClassLevel() 
				<< ",AdvancementGoalExp = " << (int)getAdvancementClassGoalExp();
			tinysave( sav.toString() );
			m_AdvancementClassExpSaveCount = 0;
		}
	}

	GCModifyInformation gcMI;
	gcMI.addLongData( MODIFY_ADVANCEMENT_CLASS_GOAL_EXP, getAdvancementClassGoalExp() );
	if ( ret )
	{
		gcMI.addShortData( MODIFY_ADVANCEMENT_CLASS_LEVEL, getAdvancementClassLevel() );
		gcMI.addShortData( MODIFY_BONUS_POINT, getBonus() );
		m_pGQuestManager->advancementClassLevelUp();

		// 2007 03 08 뱀파이어는 승직하게 되면 박쥐색을 초기화 해주는대... 퀘스트 완료 부분이 어딘지 모르겠어서... 우선 때려 박았다 ㅠ.,ㅠ Sorry ORZ..
/*		if( isVampire() && getAdvancementClassLevel() <= 1 )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(this);
			Assert( pVampire != NULL );

			pVampire->setBatColor( 0 );

			pVampire->tinysave( "BatColor=0" );
		} */
	}

	getPlayer()->sendPacket( &gcMI );

	return ret;
}

bool PlayerCreature::putAdvancedBonusToSTR()
{
	if ( m_AdvancedAttrBonus <= 0 ) return false;
	m_AdvancedAttrBonus--;
	m_AdvancedSTR[ATTR_BASIC]++;
	return true;
}

bool PlayerCreature::putAdvancedBonusToDEX()
{
	if ( m_AdvancedAttrBonus <= 0 ) return false;
	m_AdvancedAttrBonus--;
	m_AdvancedDEX[ATTR_BASIC]++;
	return true;
}

bool PlayerCreature::putAdvancedBonusToINT()
{
	if ( m_AdvancedAttrBonus <= 0 ) return false;
	m_AdvancedAttrBonus--;
	m_AdvancedINT[ATTR_BASIC]++;
	return true;
}

bool PlayerCreature::popAdvancedBonusFromSTR()
{
	if ( m_AdvancedSTR[ATTR_BASIC] <= 0 ) return false;
	m_AdvancedSTR[ATTR_BASIC]--;
	m_AdvancedAttrBonus++;
	return true;
}

bool PlayerCreature::popAdvancedBonusFromDEX()
{
	if ( m_AdvancedDEX[ATTR_BASIC] <= 0 ) return false;
	m_AdvancedDEX[ATTR_BASIC]--;
	m_AdvancedAttrBonus++;
	return true;
}

bool PlayerCreature::popAdvancedBonusFromINT()
{
	if ( m_AdvancedINT[ATTR_BASIC] <= 0 ) return false;
	m_AdvancedINT[ATTR_BASIC]--;
	m_AdvancedAttrBonus++;
	return true;
}

void PlayerCreature::addEffectOption( ObjectID_t oid, OptionType_t type )
{
	HashMapObjectOptionItor itr = m_EffectOptions.find( oid );

	if ( itr == m_EffectOptions.end() )
	{
		m_EffectOptions[ oid ] = type;
	}
}

void PlayerCreature::removeEffectOption( ObjectID_t oid )
{
	HashMapObjectOptionItor itr = m_EffectOptions.find( oid );

	if ( itr != m_EffectOptions.end() )
	{
		m_EffectOptions.erase( itr );
	}
}

void PlayerCreature::clearEffectOption()
{
	m_EffectOptions.clear();
}

bool PlayerCreature::canChangeMasterEffectColor()
{
	// MasterEffectColor 5 는 길드전 우승자들한테 주는 마스터 이펙트다
	// 상점의 마스터 이펙트 변환 아이템으로 이 이펙트에서 다른 이펙트로 변경할 수 없다.
	// 2005.05.17 by bezz
	return m_MasterEffectColor != 5;
}

void PlayerCreature::setBorn( const string& str )
{
	m_Born = Born::instnace().getWorldID( str );
}

void PlayerCreature::ChangeGearSlotID(GearSlotID_t GearSlotID)
{
	if ( GearSlotID < 0 || GearSlotID > (m_pWearItemSwapPool.size()-1) )
	{
		return;
	}

	int i;

	for(i = 0 ; i < m_pWearItem.size() ; ++i )
	{
		m_pWearItemSwapPool[m_CurrentGearSlotID][i] = m_pWearItem[i];
		m_pWearItem[i] = m_pWearItemSwapPool[GearSlotID][i];
		
		OnChangeItemInGearSlot(m_pWearItem[i], m_pWearItemSwapPool[m_CurrentGearSlotID][i]);
	}

	m_CurrentGearSlotID = GearSlotID;
	hasTaoZhuang();//qie huan zhuang bei lan  ,tao zhuang 211
	initAllStatAndSend();
}

void PlayerCreature::putItemToGear(int iWearPart, GearSlotID_t GearSlotID, Item* pItem)
{
	if ( GearSlotID < 0 || (m_pWearItemSwapPool.size()-1) < GearSlotID )
	{
		return;
	}

	if ( iWearPart < 0 || (m_pWearItemSwapPool[GearSlotID].size()-1) < iWearPart )
	{
		return;
	}

	if ( isTwohandWeapon(pItem) )
	{
		// 양손무기는 저장시 오른손 Part 번호를 기준으로 저장하므로 
		// 양손착용을 설정하기 위해서임.
		m_pWearItemSwapPool[GearSlotID][iWearPart-1] = pItem;
		m_pWearItemSwapPool[GearSlotID][iWearPart] = pItem;
	}
	else
	{
		m_pWearItemSwapPool[GearSlotID][iWearPart] = pItem;
	}
}

GearInfo* PlayerCreature::getGearInfo() const
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	int ItemCount = 0;

	GearInfo* pGearInfo = new GearInfo();

    //for (int i = 0; i < m_pWearItem.size(); ++i)
    for (int i = 0; i < m_pWearItemSwapPool[getCurrentGearSlotID()].size(); ++i)
    {
        //Item* pItem = m_pWearItem[i];
        Item* pItem = m_pWearItemSwapPool[getCurrentGearSlotID()][i];

        if (pItem != NULL)
        {
            GearSlotInfo* pGearSlotInfo = new GearSlotInfo();
            pItem->makePCItemInfo( *pGearSlotInfo );

            pGearSlotInfo->setSlotID(i);
            pGearSlotInfo->setActiveSlot(m_pRealWearingCheck[i]);

            pGearInfo->addListElement(pGearSlotInfo);

			++ItemCount;
        }
    }

    pGearInfo->setListNum(ItemCount);
    pGearInfo->setGearSlotID(getCurrentGearSlotID());

    if ( pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID()))
    {
	        pGearInfo->setActive(true);
    }
    else
    {
        pGearInfo->setActive(false);
    }

    return pGearInfo;

    __END_DEBUG
    __END_CATCH
}

bool  PlayerCreature::IsValidWearPart(int Part)
{
	if ( Part >= 0 && Part < m_pWearItemSwapPool[getCurrentGearSlotID()].size() ) 
	{
		return true;
	}

	return false;
}

void  PlayerCreature::addWearItem(int Part, Item* pItem) throw()
{
	//Assert(m_pWearItemSwapPool[getCurrentGearSlotID()][Part] == NULL); 

	if ( !IsValidWearPart(Part) ) 
	{
		return;
	}

	m_pWearItem[Part] = pItem;
	m_pWearItemSwapPool[getCurrentGearSlotID()][Part] = pItem;
}

void  PlayerCreature::deleteWearItem(int Part)
{
	//Assert(m_pWearItemSwapPool[getCurrentGearSlotID()][Part] != NULL); 

	deleteWearItem(getCurrentGearSlotID(), Part);
}

void  PlayerCreature::deleteWearItem(GearSlotID_t GearSlotID, int Part)
{
	if ( !IsValidWearPart(Part) ) 
	{
		return;
	}

	if ( getCurrentGearSlotID() == GearSlotID )
	{
		m_pWearItem[Part] = NULL;
	}
	m_pWearItemSwapPool[GearSlotID][Part] = NULL;
}

Item* PlayerCreature::getWearItem(GearSlotID_t GearSlotID, int Part)
{
	if ( !IsValidWearPart(Part) ) 
	{
		return NULL;
	}

	return m_pWearItemSwapPool[GearSlotID][Part];
}

Item* PlayerCreature::getWearItem(int Part)
{
	return getWearItem(getCurrentGearSlotID(), Part);
}

bool PlayerCreature::isWear(int iPart) throw() 
{
	return getWearItem(iPart) != NULL ? true : false; 
}

void PlayerCreature::setXY(ZoneCoord_t x, ZoneCoord_t y) throw()
{
	Creature::setXY(x, y);

	getGQuestManager()->touchWayPoint(this);
}

bool PlayerCreature::StartToTalkWithCallNPC(NPC *pNPC, int ScriptType)
{
	if ( g_pGlobalNPCManager->GetNPCByObjectID(pNPC->getObjectID()) == NULL )
	{
		return false;
	}
	
	if ( pNPC == NULL )
	{
		return false;
	}
	
	m_pCallNPC = pNPC;
	
	if ( !m_pCallNPC->activateTrigger(this, Condition::CONDITION_TALKED_BY, Trigger::PASSIVE_TRIGGER, (void *)ScriptType ) )
	{
		// 해당 Condition이 없는 경우
		
		return false;
	}
	
	return true;
}

void PlayerCreature::FinishToTalkWithCallNPC()
{
	m_pCallNPC = NULL;
}

NPC *PlayerCreature::GetTalkingCallNPC()
{
	return m_pCallNPC;
}

bool PlayerCreature::loadWeekItemList()
{
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
	
	if ( pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GIVE_WEEKITEM) )
	{
		m_pWeekItemListManager->load();
		
		return true;
	}
	
	return false;
}

void PlayerCreature::SendChangeShapes(bool bSendPacket)
{
	Item* pItem = NULL;
	int i = 0;
	
	for (i = 0; i < m_pWearItemSwapPool[getCurrentGearSlotID()].size(); ++i)
	{
		pItem = m_pWearItem[i];
		
		if ( pItem == NULL )
		{
			continue;
		}
		
		if (m_pRealWearingCheck[i])
		{
			Item::ItemClass IClass = pItem->getItemClass();
			ItemType_t		IType  = pItem->getItemType();

			Color_t color = getItemShapeColor( pItem );
			addShape(IClass, IType, color);

			if (bSendPacket)
			{
				GCChangeShape pkt;
				pkt.setObjectID(getObjectID());
				pkt.setItemClass(IClass);
				pkt.setItemType(IType);
				pkt.setOptionType(pItem->getFirstOptionType());
				pkt.setAttackSpeed(getAttackSpeed());

				m_pZone->broadcastPacket(m_X, m_Y , &pkt, this);
			}
		}
		else
		{
			removeShape(pItem->getItemClass(), bSendPacket);
		}
	}
}

void PlayerCreature::CheckValidWearingItem()
{
	//////////////////////////////////////////////////////////////////////////////
	// 입고 있는 아이템을 체크한다.
	// for 가 두번인 이유는 아이템으로 올라간 능력치에 의해서
	// 입을 수 있게 되는 아이템을 체크하기 위해서이다.
	//////////////////////////////////////////////////////////////////////////////
	
	for (int j = 0; j < m_pWearItemSwapPool[getCurrentGearSlotID()].size(); ++j)
	{
		int wearCount = 0;

		for (int i = 0; i < m_pWearItemSwapPool[getCurrentGearSlotID()].size(); ++i)
		{
			Item* pItem = m_pWearItem[i];

			// 현재 포인트에 아이템이 있고
			// 그것에 대한 체크를 아직 하지 않았다면...
			if (pItem != NULL && m_pRealWearingCheck[i] == false)
			{
				// 만일 진짜루 입을 수 있는 아이템이라면 능력치를 올려준다.
				if (isRealWearing(pItem))
				{
					// 양손 무기라면, 체크를 두번 하지 않도록
					// 왼쪽, 오른쪽 모두 체크 변수를 세팅
					if (isTwohandWeapon(pItem))
					{
						m_pRealWearingCheck[getWearPartOfLeftHand()] = true;
						m_pRealWearingCheck[getWearPartOfRightHand()] = true;
					}
					else 
					{
						m_pRealWearingCheck[i] = true;
					}
					computeItemStat(pItem);

					++wearCount;
				}
			}
		}

		if (wearCount == 0)
		{
			return;
		}
	}

}

void PlayerCreature::loadEventPresentItemList()
{
	// 이벤트 선물을 장바구니로 넣어주기
	Statement* pStmt = NULL;
	Result *pResult = NULL;
	
	vector<PresentItem> PresentItemList; 
		
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		
		pResult = pStmt->executeQuery(
			"SELECT EventTitle, GoodsID, GoodsCount, Period FROM EventPresentItemSchedule"
			" WHERE now() between StartDate and EndDate"
		);
		
		PresentItem presentItem;
		
		while ( pResult->next() )
		{
			presentItem.m_strEventTitle = pResult->getString(1);
			presentItem.m_iGoodsID = pResult->getInt(2);
			presentItem.m_iGoodsCount = pResult->getInt(3);
			presentItem.m_iPeriod = pResult->getInt(4);
			
			PresentItemList.push_back(presentItem);
		}

		if ( PresentItemList.size() > 0 )
		{
		
			bool bGiveGift = false;
			vector<PresentItem>::iterator iter;
			
			iter = PresentItemList.begin();
	
			for( ; iter != PresentItemList.end() ; ++iter )
			{
				pResult = pStmt->executeQuery(
						"SELECT count(*), to_days(now()) - to_days(ifnull(max(GiftRecvDate), date_add(now(), interval -1 day))) "
						" FROM EventPresentItemLog USE INDEX(IDX_EventTitleAndPlayerID)"
						" WHERE PlayerID='%s' AND EventTitle='%s'",
						getPlayer()->getID().c_str(),
						iter->m_strEventTitle.c_str()
				);
				
				bGiveGift = false;
			
				if ( pResult->next() )
				{
					int iRecvCount;
					int iPastDay;
			
					iRecvCount = pResult->getInt(1);
					iPastDay = pResult->getInt(2);
			
					if ( iter->m_iPeriod > 0 && iPastDay >= iter->m_iPeriod )
					{
						// 이벤트 기간동안, 이전 지급받은 이후로 m_iPeriod 기간만큼 지난 경우
						bGiveGift = true;
					}
					else if ( iter->m_iPeriod == 0 && iRecvCount == 0 )
					{
						// 이벤트 기간동안, 단 한번만 지급
						bGiveGift = true;
					}
				}
				else
				{
					bGiveGift = true;
				}
		
				if ( bGiveGift )
				{
					cout << "이벤트 선물 지급 : " << getPlayer()->getID() << "(" << getName().c_str() << ")" << endl;
			
					pStmt->executeQuery(
						"INSERT IGNORE INTO EventPresentItemLog(EventTitle, PlayerID, CharName, GiftRecvDate, GoodsID, GoodsCount, IP) "
						"VALUES ('%s', '%s', '%s', now(), %d, %d, '%s')",
						iter->m_strEventTitle.c_str(),
						getPlayer()->getID().c_str(),
						getName().c_str(),
						iter->m_iGoodsID,
						iter->m_iGoodsCount,
						getPlayer()->getSocket()->getHost().c_str()
					);
					
					pStmt->executeQuery("INSERT IGNORE INTO GoodsListObject VALUES ('%s', '', %d, '%s', '%s', %d, %d, 'NOT', now())",
						iter->m_strEventTitle.c_str(),
						g_pConfig->getPropertyInt("WorldID"),
						getPlayer()->getID().c_str(),
						"",
						iter->m_iGoodsID,
						iter->m_iGoodsCount
					);		
				
					GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
					Assert(pGamePlayer!=NULL);
							
					EventSystemMessage* pESM = new EventSystemMessage( pGamePlayer );
					pESM->setDeadline( 0 );
					pESM->addMessage(g_pStringPool->getString(STRID_GET_EVENT_PRESENT_IN_SHOPPINGCART).c_str());
					pGamePlayer->addEvent( pESM );
				}
			}
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)	
}

void PlayerCreature::loadEventNewStartSupplyItemList()
{
	Statement* pLoginStmt = NULL;
	Statement* pGameStmt = NULL;
	
	Result *pResult = NULL;
	
	vector<string> EventIDList;
	vector<string> EventDescList;
	
	string strPlayerCreationDate;
	string strCreatureCreationDate;
	
	BEGIN_DB
	{
		pLoginStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		
		pResult = pLoginStmt->executeQuery(
			"SELECT creation_date FROM Player WHERE PlayerID='%s'",
			getPlayer()->getID().c_str()
		);
		
		if ( pResult->next() )
		{
			strPlayerCreationDate = pResult->getString(1);
		}
		else
		{
			filelog("EventNewStartBug.log",
					"Missing player creation_date: PlayerID=%s Name=%s Race=%d",
					getPlayer()->getID().c_str(),
					getName().c_str(),
					(int)getRace());
			SAFE_DELETE(pLoginStmt);
			return;
		}
		
		SAFE_DELETE(pLoginStmt);
	}
	END_DB(pLoginStmt)
	
	BEGIN_DB
	{
		pGameStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		// Slayer tablosu legacy ortak character base tablosudur.
		// Vampire/Ousters tablolarinda creation_date kolonu yoktur.
		pResult = pGameStmt->executeQuery(
			"SELECT creation_date FROM Slayer WHERE PlayerID='%s' AND Name='%s'",
			getPlayer()->getID().c_str(),
			getName().c_str()
		);
		
		if ( pResult->next() )
		{
			strCreatureCreationDate = pResult->getString(1);
		}
		else
		{
			filelog("EventNewStartBug.log",
					"Missing character creation_date in Slayer base row: PlayerID=%s Name=%s Race=%d",
					getPlayer()->getID().c_str(),
					getName().c_str(),
					(int)getRace());
			SAFE_DELETE(pGameStmt);
			return;
		}
		
		SAFE_DELETE(pGameStmt);
	}
	END_DB(pGameStmt)
		
	BEGIN_DB
	{
		pLoginStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			
		// if EventType=1, 계정 생성을 기준으로 이벤트 검색
		// if EventType=2, 캐릭터 생성을 기준으로 이벤트 검색
		
		pResult = pLoginStmt->executeQuery(
			"SELECT E1.EventID, ifnull(E1.EventDesc, '') "
			" FROM EventNewStartSchedule E1 LEFT OUTER JOIN EventNewStart E2" 
			"	ON E1.EventID = E2.EventID AND E2.PlayerID='%s' AND E2.Name='%s'"
			" WHERE "
			"	now() BETWEEN EventSDate AND EventEDate AND" 
			"	(('%s' BETWEEN EventSDate AND EventEDate AND EventType=1) OR "
			"	 ('%s' BETWEEN EventSDate AND EventEDate AND EventType=2) ) AND"
			"	E2.RecvDate is null",
			getPlayer()->getID().c_str(),
			getName().c_str(),
			strPlayerCreationDate.c_str(),
			strCreatureCreationDate.c_str()
		);

		string strEventID;
		string strEventDesc;
		
		while( pResult->next() )
		{
			strEventID = pResult->getString(1);
			strEventDesc = pResult->getString(2);
			
			EventIDList.push_back(strEventID);
			EventDescList.push_back(strEventDesc);
		}
		
		if ( EventIDList.size() > 0 )
		{
			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
			Assert(pGamePlayer!=NULL);
			
			EventSystemMessage* pESM = new EventSystemMessage( pGamePlayer );
			pESM->setDeadline( 0 );
			
			Race_t raceType = getRace();
			
			for(int i = 0; i < EventIDList.size(); ++i)
			{
				pLoginStmt->executeQuery(
					"INSERT IGNORE INTO EventNewStart VALUES('%s', '%s', '%s', now(), '%s')",
					EventIDList[i].c_str(),
					getPlayer()->getID().c_str(),
					getName().c_str(),
					m_pPlayer->getSocket()->getHost().c_str()
				);
						
				pLoginStmt->executeQuery(
					"INSERT IGNORE INTO GoodsListObject "
					"	SELECT '%s', '', %d, '%s', '%s', GoodsID, GoodsCount, 'NOT', now() FROM EventNewStartSupplyItem"
					" WHERE EventID='%s' AND (Race=7 OR Race=%d)",
					EventIDList[i].c_str(),
					g_pConfig->getPropertyInt("WorldID"),
					getPlayer()->getID().c_str(),
					getName().c_str(),
					
					EventIDList[i].c_str(),
					(0x01 << raceType) 
				);
				
				pESM->addMessage(EventDescList[i].c_str());
			}
			
			pGamePlayer->addEvent( pESM );
		}
		
		SAFE_DELETE(pLoginStmt);
	}
	END_DB(pLoginStmt)
	
}

void PlayerCreature::loadEventComebackSupplyItemList()
{
	Statement* pLoginStmt = NULL;
	
	Result *pResult = NULL;
	
	vector<string> EventIDList;
	vector<string> EventDescList;
	
	string strLastLogoutDate;

	BEGIN_DB
	{
		pLoginStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		
		// 마지막 로그아웃 시각을 구한다. 단, 이 시각이 생성일보다 빠른경우 생성일을 로그아웃 시각으로 간주한다. 
		pResult = pLoginStmt->executeQuery(
			"SELECT if(creation_date > LastLogoutDate, creation_date, LastLogoutDate) "
			" FROM Player WHERE PlayerID='%s'",
			getPlayer()->getID().c_str()
		);
		
		if ( pResult->next() )
		{
			strLastLogoutDate = pResult->getString(1);
		}
		else
		{
			return;
		}
		
		SAFE_DELETE(pLoginStmt);
	}
	END_DB(pLoginStmt)
	
	BEGIN_DB
	{
		pLoginStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			
		pResult = pLoginStmt->executeQuery(
			"SELECT E1.EventID, ifnull(E1.EventDesc, '')  "
			 "FROM EventComebackSchedule E1 LEFT OUTER JOIN EventComeback E2"
			"	ON E1.EventID = E2.EventID AND E2.PlayerID='%s'"
			" WHERE "
			"	(now() BETWEEN EventSDate AND EventEDate) AND"       
			"	('%s' <= EventBaseDate ) AND"
			"	E2.RecvDate is null",
			getPlayer()->getID().c_str(),
			strLastLogoutDate.c_str()
		);

		string strEventID;
		string strEventDesc;
		
		while( pResult->next() )
		{
			strEventID = pResult->getString(1);
			strEventDesc = pResult->getString(2);
			
			EventIDList.push_back(strEventID);
			EventDescList.push_back(strEventDesc);
		}
		
		if ( EventIDList.size() > 0 )
		{
			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);
			Assert(pGamePlayer!=NULL);
			
			EventSystemMessage* pESM = new EventSystemMessage( pGamePlayer );
			pESM->setDeadline( 0 );
			
			Race_t raceType = getRace();
			
			for(int i = 0; i < EventIDList.size(); ++i)
			{
				pLoginStmt->executeQuery(
					"INSERT IGNORE INTO EventComeback VALUES('%s', '%s', now(), '%s')",
					EventIDList[i].c_str(),
					getPlayer()->getID().c_str(),
					m_pPlayer->getSocket()->getHost().c_str()
				);
						
				pLoginStmt->executeQuery(
					"INSERT IGNORE INTO GoodsListObject "
					"	SELECT '%s', '', %d, '%s', '', GoodsID, GoodsCount, 'NOT', now() FROM EventComebackSupplyItem"
					" WHERE EventID='%s' AND (Race=7 OR Race=%d)",
					EventIDList[i].c_str(),
					g_pConfig->getPropertyInt("WorldID"),
					getPlayer()->getID().c_str(),
					
					EventIDList[i].c_str(),
					(0x01 << raceType) 
				);
				
				pESM->addMessage(EventDescList[i].c_str());
			}
			
			pGamePlayer->addEvent( pESM );
		}
		
		SAFE_DELETE(pLoginStmt);
	}
	END_DB(pLoginStmt)
}

void PlayerCreature::OnLogOut()
{
	// 성서 떨어뜨리기
	if ( hasRelicItem() )
	{
		dropRelicToZone( (Creature *)this, false );
	}

	dropFlagToZone( this, false );
	dropSweeperToZone( this );
	
	if ( g_pPKZoneInfoManager->isPKZone( getZoneID() ) )
	{
		g_pPKZoneInfoManager->leavePKZone( getZoneID() );
	}
	
	bool bHardCore = false;
	
	if ( g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore") != 1 )
	{
		bHardCore = true;
	}
		
	if ( !bHardCore )
	{
		save();
		tinysave("LastPlayDate=now()");
		
		if ( isFlag(Effect::EFFECT_CLASS_COMA) || isFlag(Effect::EFFECT_CLASS_ETERNITY))
		{
			ZoneID_t  ZoneID = 0;
			ZoneCoord_t ZoneX = 0;
			ZoneCoord_t ZoneY = 0;
			ZONE_COORD ResurrectCoord;
	
			g_pResurrectLocationManager->getPosition( this, ResurrectCoord );

			ZoneID = ResurrectCoord.id;
			ZoneX  = ResurrectCoord.x;
			ZoneY  = ResurrectCoord.y;

			char pField[80];
			sprintf(pField, "ZoneID=%d, XCoord=%d, YCoord=%d, CurrentHP=HP", ZoneID, ZoneX, ZoneY);

			tinysave(pField);
		}
		
		// 포스 스크롤이 켜져 있으면 로그아웃하면저 저장한다.
		if ( isFlag(Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL) )
		{
			Effect* pEffect = findEffect(Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL);
			pEffect->save(getName());
		}
		if ( isFlag(Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL) )
		{
			Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL);
			pEffect->save(getName());
		}
		if ( isFlag(Effect::EFFECT_CLASS_CARNELIAN_FORCE_SCROLL) )
		{
			Effect* pEffect = findEffect(Effect::EFFECT_CLASS_CARNELIAN_FORCE_SCROLL);
			pEffect->save(getName());
		}
	}

	Zone *pZone = getZone();
	
	if ( pZone != NULL )
	{
		pZone->deleteCreature(this, getX() , getY());
	}
	
	if ( getGuildID() != getCommonGuildID() )
	{
		Guild* pGuild = g_pGuildManager->getGuild( getGuildID() );
		
		if ( pGuild != NULL )
		{
			pGuild->deleteCurrentMember( getName() );

			GSGuildMemberLogOn gsGuildMemberLogOn;
			gsGuildMemberLogOn.setGuildID( pGuild->getID() );
			gsGuildMemberLogOn.setName( getName() );
			gsGuildMemberLogOn.setLogOn( false );

			g_pSharedServerManager->sendPacket( &gsGuildMemberLogOn );
		
			ostringstream os;
			
			os << "UPDATE GuildMember SET LogOn=0 WHERE Name = '" << getName().c_str() << "'";
			
			g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());
		}
		else
		{
			filelog( "GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", 
				(int)getGuildID(), getName().c_str() 
			);
		}
	}
	
	GSLogout gsLogout;
	gsLogout.SetSender(getName());
	
	g_pSharedServerManager->sendPacket( &gsLogout );
}

void PlayerCreature::OnDead()
{
	if ( isFlag( Effect::EFFECT_CLASS_EXPLOSION_WATER ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_EXPLOSION_WATER);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_EXPLOSION_WATER );
		deleteEffect( Effect::EFFECT_CLASS_EXPLOSION_WATER );
	}

	if ( isFlag( Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1 ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1 );
		deleteEffect( Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1 );
	}

	if ( isFlag( Effect::EFFECT_CLASS_INSTALL_TURRET ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_INSTALL_TURRET);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_INSTALL_TURRET );
		deleteEffect( Effect::EFFECT_CLASS_INSTALL_TURRET );
	}

	if ( isFlag( Effect::EFFECT_CLASS_ADV2HALCYONMK1 ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_ADV2HALCYONMK1);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_ADV2HALCYONMK1 );
		deleteEffect( Effect::EFFECT_CLASS_ADV2HALCYONMK1 );
	}

	if ( isFlag( Effect::EFFECT_CLASS_DIVINE_GUIDANCE ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_DIVINE_GUIDANCE);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_DIVINE_GUIDANCE );
		deleteEffect( Effect::EFFECT_CLASS_DIVINE_GUIDANCE );
	}

	if ( isFlag( Effect::EFFECT_CLASS_GLACIER ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_GLACIER);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_GLACIER );
		deleteEffect( Effect::EFFECT_CLASS_GLACIER );
	}

	if ( isFlag( Effect::EFFECT_CLASS_ACID_ERUPTION ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_ACID_ERUPTION);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_ACID_ERUPTION );
		deleteEffect( Effect::EFFECT_CLASS_ACID_ERUPTION );
	}

	if ( isFlag( Effect::EFFECT_CLASS_FADE_OUT ) )
	{
		removeFlag( Effect::EFFECT_CLASS_FADE_OUT );
		deleteEffect( Effect::EFFECT_CLASS_FADE_OUT );
		// unaffect 를 해주면 스나이핑이나 인비지가 걸리기 때문에 해주면 안 된다.
	}

	if ( isFlag( Effect::EFFECT_CLASS_REFINIUM_TICKET ) )
	{
		removeFlag( Effect::EFFECT_CLASS_REFINIUM_TICKET );
		deleteEffect( Effect::EFFECT_CLASS_REFINIUM_TICKET );
	}

	if (isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);

		if (pEffect!=NULL)
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);
		removeFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH);
	}

	if (isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_INVISIBILITY);

		if (pEffect!=NULL)
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_INVISIBILITY);
		removeFlag(Effect::EFFECT_CLASS_INVISIBILITY);
	}

	if (isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SNIPING_MODE);

		if (pEffect!=NULL)
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_SNIPING_MODE);
		removeFlag(Effect::EFFECT_CLASS_SNIPING_MODE);
	}
	if (isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SNIPING_MODE_2);

		if (pEffect!=NULL)
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_SNIPING_MODE_2);
		removeFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2);
	}
	if (isFlag(Effect::EFFECT_CLASS_PARALYZE))
	{
		Effect* pEffectPal = findEffect(Effect::EFFECT_CLASS_PARALYZE);

		if (pEffectPal!=NULL)
		{
			pEffectPal->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_PARALYZE);
		removeFlag(Effect::EFFECT_CLASS_PARALYZE);
	}
	if (isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON))
	{
		Effect* pEffectPal = findEffect(Effect::EFFECT_CLASS_CHAINOFDEMON);

		if (pEffectPal!=NULL)
		{
			pEffectPal->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_CHAINOFDEMON);
		removeFlag(Effect::EFFECT_CLASS_CHAINOFDEMON);
	}
	if (isFlag(Effect::EFFECT_CLASS_TEMENOS))
	{
		Effect* pEffectPal = findEffect(Effect::EFFECT_CLASS_TEMENOS);

		if (pEffectPal!=NULL)
		{
			pEffectPal->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_TEMENOS);
		removeFlag(Effect::EFFECT_CLASS_TEMENOS);
	}
	if (isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS))
	{
		Effect* pEffectPal = findEffect(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS);

		if (pEffectPal!=NULL)
		{
			pEffectPal->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS);
		removeFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS);
	}
	if(isFlag(Effect::EFFECT_CLASS_STUN))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_STUN );

		if( pEffect != NULL )
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_STUN);
		removeFlag(Effect::EFFECT_CLASS_STUN);
	}
	if(isFlag(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH2))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH2 );

		if( pEffect != NULL )
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH2);
		removeFlag(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH2);
	}
	if(isFlag(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH1))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH1 );

		if( pEffect != NULL )
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH1);
		removeFlag(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH1);
	}

	if (isFlag(Effect::EFFECT_CLASS_HALLUCINATION))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_HALLUCINATION);

		if (pEffect != NULL)
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_HALLUCINATION);
		removeFlag(Effect::EFFECT_CLASS_HALLUCINATION);
	}

	if ( isFlag( Effect::EFFECT_CLASS_MAGNUM_SPEAR ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_MAGNUM_SPEAR);
		if ( pEffect != NULL )
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_MAGNUM_SPEAR);
		removeFlag(Effect::EFFECT_CLASS_MAGNUM_SPEAR);
	}

	if ( isFlag( Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
		if ( pEffect != NULL )
		{
			pEffect->unaffect();
		}
		deleteEffect(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
		removeFlag(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
	}

	// 죽었을 때 Soul Chain 이펙트를 끈다.
	// unaffect가 호출될 때 flag이 켜져있지 않다면 transport 하지 않도록한다.
	if ( isFlag( Effect::EFFECT_CLASS_SOUL_CHAIN ) )
	{
		//pCreature->deleteEffect( Effect::EFFECT_CLASS_SOUL_CHAIN );
		removeFlag( Effect::EFFECT_CLASS_SOUL_CHAIN );
	}

	// 죽었을 때 Love Chain 이펙트를 끈다.
	// unaffect가 호출될 때 flag이 켜져있지 않다면 transport 하지 않도록한다.
	if ( isFlag( Effect::EFFECT_CLASS_LOVE_CHAIN ) )
	{
		removeFlag( Effect::EFFECT_CLASS_LOVE_CHAIN );
	}

	// 죽으면 GunShotGuidance Aim 이펙트를 끈다.
	if ( isFlag( Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM ) )
	{
		deleteEffect( Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM );
		removeFlag( Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM );
	}

	if (isFlag(Effect::EFFECT_CLASS_SLEEP))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SLEEP);

		if (pEffect != NULL)
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_SLEEP);
	}

	if (isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL))
	{
		Effect* pEffect = findEffect( Effect::EFFECT_CLASS_MIST_OF_SOUL );

		if ( pEffect != NULL )
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_MIST_OF_SOUL);
	}

	if (isFlag(Effect::EFFECT_CLASS_ARMAGEDDON))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_ARMAGEDDON);

		if (pEffect != NULL)
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_ARMAGEDDON);
	}

	if (isFlag(Effect::EFFECT_CLASS_TRAPPED))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_TRAPPED);

		if (pEffect!=NULL)
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_TRAPPED);
	}

	if ( isFlag(Effect::EFFECT_CLASS_FIERCE_FLAME) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_FIERCE_FLAME);

		if ( pEffect != NULL )
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_FIERCE_FLAME);
	}

	if ( isFlag(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE);
		if( pEffect != NULL )
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE);
	}

	if( isFlag(Effect::EFFECT_CLASS_ILLUSION_INVERSION) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_ILLUSION_INVERSION);
		if( pEffect != NULL )
		{
			pEffect->unaffect();
		}
		
		deleteEffect(Effect::EFFECT_CLASS_ILLUSION_INVERSION);
	}
						
	if( isFlag(Effect::EFFECT_CLASS_BLIND) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_BLIND);
		if( pEffect != NULL )
		{
			pEffect->unaffect();
		}
		deleteEffect(Effect::EFFECT_CLASS_BLIND);
	}
	
	if( isFlag(Effect::EFFECT_CLASS_DOOM) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_DOOM);
		if(pEffect!= NULL)
		{
			pEffect->unaffect();
		}
		deleteEffect(Effect::EFFECT_CLASS_DOOM);
	}
	
	if( isFlag(Effect::EFFECT_CLASS_LOSE_SIGHT) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_LOSE_SIGHT);
		if(pEffect!= NULL)
		{
			pEffect->unaffect();
		}
		deleteEffect(Effect::EFFECT_CLASS_LOSE_SIGHT);
	}
	
	if( isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_BLOCK_HEAD);
		if(pEffect!= NULL)
		{
			pEffect->unaffect();
		}
		deleteEffect(Effect::EFFECT_CLASS_BLOCK_HEAD);
	}
	if ( isFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SQUALLY_BARRIER1);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 );
		deleteEffect( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 );
	}
	
	if ( isFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 ) )
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_SQUALLY_BARRIER2);
		if ( pEffect != NULL ) 
		{
			pEffect->unaffect();
		}
		
		removeFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 );
		deleteEffect( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 );
	}
}

void PlayerCreature::NotifyZoneToMove()
{
	GSFriendUpdateZone gsFriendUpdateZone;
	gsFriendUpdateZone.SetSender(this->getName());
	gsFriendUpdateZone.SetZoneID(this->getZone()->getZoneID());
	
	g_pSharedServerManager->sendPacket( &gsFriendUpdateZone );
}

int PlayerCreature::GetSkillCastingTime(SkillType_t SkillType)
{
	// 무기를 무엇을 들고 있는지 체크한다.
	Item* pItem = getWearItem(getWearPartOfRightHand());
	Item::ItemClass IClass = Item::ITEM_CLASS_MAX;
	if (pItem != NULL) 
	{
		IClass = pItem->getItemClass();
	}
	
	SkillCastingTime castingTime;
	int iSkillCastingTime = -1;
	
	if ( g_SkillCastingTimeManager.GetSkillTime(SkillType, IClass, castingTime) )
	{
		if ( isAdvanced() ) 
		{
			iSkillCastingTime = castingTime.iCastingTimeForAdv;
		}
		else
		{
			iSkillCastingTime = castingTime.iCastingTimeForNonAdv;
		}
	}
	else
	{
		int iSkillInterval;
		
		if ( GetSkillInterval(SkillType, iSkillInterval) )
		{
			if ( iSkillInterval == 0 )
			{
				try
				{
					SkillInfo* pSkillInfo;
					
					pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
					iSkillCastingTime = pSkillInfo->getMaxDelay();
				}
				catch(NoSuchElementException& e)
				{
					iSkillCastingTime = -1;
				}
				catch(Error& e)
				{
					iSkillCastingTime = -1;
				}
			}
			else
			{
				iSkillCastingTime = iSkillInterval;
			}
			
			iSkillCastingTime = iSkillCastingTime * 100;
		}
	}
	
	// 리턴되는 시간단위는 ms 임
	return iSkillCastingTime;
}

IP_t PlayerCreature::getIP(void) const
{
	Assert(m_pPlayer != NULL);
	Socket* pSocket = m_pPlayer->getSocket();
	Assert(pSocket != NULL);
	
	return pSocket->getHostIP();
}
NewRankGem* PlayerCreature::getRankGem( NewRankGem::RankGemType type ) const
	throw()
{
	__BEGIN_TRY
	
	HashMapRankGemConstItor itr = m_RankGem.find( type );

	if ( itr == m_RankGem.end() )
	{
		return NULL;
	}
	
	return itr->second;
	__END_CATCH
}
void PlayerCreature::addRankGem( NewRankGem* RankGem )
	throw()
{
	__BEGIN_TRY

	HashMapRankGemItor itr = m_RankGem.find( RankGem->getType() );

	if ( itr == m_RankGem.end() )
	{
		m_RankGem[RankGem->getType()] = RankGem;
		m_RankGemFlag.set( RankGem->getType() );
	}
	else
	{
		SAFE_DELETE( RankGem );
	}

	__END_CATCH
}
void PlayerCreature::loadRankGem()
	throw()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result* pResult = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery( "SELECT Type,OptionClass,Grade FROM RankGemData WHERE Name='%s'", getName().c_str() );

		while ( pResult->next() )
		{
			DWORD rankGemType = pResult->getInt(1);
			OptionClass optionclass = (OptionClass)pResult->getInt(2);
			DWORD grade = pResult->getInt(3);
			if(rankGemType!=999 && grade!=999)
			{
				RankGemInfo* pGemInfo = dynamic_cast<RankGemInfo*>(g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_RANK_GEM,rankGemType));
				Assert(pGemInfo != NULL);

				if ( (getRace() == pGemInfo->getRace()) || pGemInfo->getRace()==7  )
				{
					NewRankGem* pNewRankGem = new NewRankGem();

					pNewRankGem->setType( rankGemType );
					pNewRankGem->setOptionClass( optionclass );
					pNewRankGem->setGrade( grade );

					addRankGem( pNewRankGem );
				}
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
		
	__END_CATCH
}
void PlayerCreature::clearRankGem( NewRankGem::RankGemType type )
	throw()
{
	__BEGIN_TRY

	HashMapRankGemItor itr = m_RankGem.find( type );
	if ( itr != m_RankGem.end() )
	{
		m_RankGemFlag.reset( type );
		SAFE_DELETE( itr->second );
		m_RankGem.erase( itr );
	}

	__END_CATCH
}
