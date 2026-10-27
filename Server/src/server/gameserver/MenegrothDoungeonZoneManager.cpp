#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Utility.h"
#include "Monster.h"
#include "Zone.h"
#include "MonsterManager.h"

#include "MenegrothDoungeonZoneManager.h"
#include "EffectMenegrothTrap1.h"
#include "EffectMenegrothTrap2.h"
#include "EffectSummonCabracam.h"
#include "StringPool.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "ItemFactoryManager.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "PacketUtil.h"
#include "VariableManager.h"

#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCCreateItem.h"


MenegrothDoungeonZoneManager::MenegrothDoungeonZoneManager(Zone *pZone)
{
	m_pZone = pZone;

	m_bReadyToSummon = false;
	m_bSummonCabracam = false;
	m_CabracamObjectID = 0;

	ResetCabracamHeartToAltar();

	g_MenegrothDoungeonManager.getTrap1Pos(m_pZone, m_Trap1PosList);
	g_MenegrothDoungeonManager.getTrap2Pos(m_pZone, m_Trap2PosList);
	m_Trap2InstallTimeList.reserve(m_Trap2PosList.size());

	{
		TTrapPosList::iterator iter;
		iter = m_Trap1PosList.begin();
		for( ; iter != m_Trap1PosList.end() ; ++iter)
		{
			InstallTrap1((*iter).x, (*iter).y);
		}
	}

	{
		TTrapInstallTimeList::iterator iter;
		iter = m_Trap2InstallTimeList.begin();
		for(; iter != m_Trap2InstallTimeList.end(); ++iter)
		{
			getCurrentTime((*iter));
		}
	}

	getCurrentTime(m_PrevTrapCheckTime);

	g_MenegrothDoungeonManager.getBossMonster(m_pZone->getZoneID(), m_summonMonsterType, m_summonX, m_summonY);

	calcNextCabracamRegenTime();

}

void MenegrothDoungeonZoneManager::calcNextCabracamRegenTime()
{
	// 17�ñ������� 3�ð����� ������ ����

	getCurrentTime(m_nextCabracamRegenTime);

	int iRegenPeriod;

	// per 3 hours 
	iRegenPeriod = 60 * 60 * 3;
	// 0�� �������� 3�ð� ���� ���
	m_nextCabracamRegenTime.tv_sec = ((m_nextCabracamRegenTime.tv_sec / iRegenPeriod) + 1) * iRegenPeriod; 
	// 0�� �������� ���� ����� 2�ð��� ���� -> 17��(2��) �������� 3�ð����� ��
	m_nextCabracamRegenTime.tv_sec += (60*60*2); 

	/*
	// to test, per 10 minutes
	iRegenPeriod = 60 * 10;
	m_nextCabracamRegenTime.tv_sec = ((m_nextCabracamRegenTime.tv_sec / iRegenPeriod) + 1) * iRegenPeriod; 
	*/
}

bool MenegrothDoungeonZoneManager::IsAlreadySummonCabracam()
{
	bool bSummonCabracam = false;

	if ( m_CabracamObjectID == 0 )
	{
		return false;
	}

	// �̹� ī���į ���� �ִ��� üũ
	Creature *pCreature = m_pZone->getCreature(m_CabracamObjectID);
	if ( pCreature != NULL && m_CabracamObjectID == pCreature->getObjectID() )
	{
		if ( pCreature->isDead() )
		{
			bSummonCabracam = false;
		}
		else
		{
			// �̹� ī���į ���� �ִ� 
		    bSummonCabracam = true;
		}
    }

	return bSummonCabracam;

}

void MenegrothDoungeonZoneManager::giveRewardItemToAllPCInZone()
{
	cout << "giveRewardItemToAllPCInZone()" << endl;

	PCManager* pPM = (PCManager*)m_pZone->getPCManager();

	map< ObjectID_t, Creature* > & pcs = pPM->getCreatures();
	map< ObjectID_t, Creature* >::iterator itr = pcs.begin();

	for( ; itr != pcs.end() ; itr++)
	{
		Creature* pCreature = itr->second;
		PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* pInventory = pPlayerCreature->getInventory();

		cout << "MenegrothDoungeon, giveRewardItemToAllPCInZone() " 
			<< "ZoneID = " << m_pZone->getZoneID() 
			<< "PlayerName = " << pPlayerCreature->getName() 
			<< endl;

		if ( pInventory == NULL )
		{
			continue;
		}

		// ������ ��ǥ�� �˻� 
		Item::ItemClass ItemClass;
		ItemType_t ItemType;
		ItemNum_t ItemNum = 1;

		if ( !g_MenegrothDoungeonManager.getRewardMenegrothVoucher(pCreature, ItemClass, ItemType) )
		{
			continue;
		}
		
		if ( m_pZone->getZoneLevel( pPlayerCreature->getX(), pPlayerCreature->getY() ) & SAFE_ZONE )
		{
			ItemNum = 1;
		}
		else
		{
			ItemNum = g_pVariableManager->getMenegroVoucherCount();
		}

		list<OptionType_t> nullList;
		Item* pItem = g_pItemFactoryManager->createItem(ItemClass, ItemType, nullList);
		pItem->setNum(ItemNum);

		(m_pZone->getObjectRegistry()).registerObject(pItem);


		// �κ��丮�� �� ���� ã�´�.
		_TPOINT p;
		if (pInventory->getEmptySlot(pItem, p))
		{
			// �κ��丮�� �߰��Ѵ�.
			pInventory->addItem(p.x, p.y, pItem);
			pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);

			// ItemTrace �� Log �� �����
			if ( pItem != NULL && pItem->isTraceItem() )
			{
				remainTraceLog( pItem, "MenegrothDoungeon", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
				remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, m_pZone->getZoneID() );
			}

			// �κ��丮�� ������ ���� ��Ŷ�� �����ش�.
			GCCreateItem gcCreateItem;
			makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );
			pCreature->getPlayer()->sendPacket(&gcCreateItem);
		}
	}
}

void MenegrothDoungeonZoneManager::SummonCabracam()
{
	Monster *pMonster = NULL;

	try
	{
		if ( IsAlreadySummonCabracam() )
		{
			return;
		}

		pMonster = new Monster(m_summonMonsterType);
		Assert( pMonster != NULL );

		pMonster->setDir(DOWN);
		m_pZone->addCreature( pMonster, m_summonX, m_summonY, pMonster->getDir() );

		// addCreature basariliysa Zone monster'i sahiplenir; bizde tutmamak icin local pointer'i temizle.
		Monster *pOwned = pMonster;
		pMonster = NULL;

		calcNextCabracamRegenTime();

		m_bSummonCabracam = true;
		m_bReadyToSummon = false;
		m_CabracamObjectID = pOwned->getObjectID();
	}
	catch ( Throwable& )
	{
		// addCreature fail (EmptyTile veya Error). pMonster hala bizde ise guvenle delete et;
		// addCreature basariliysa pMonster yukarida NULL'a cekildi, SAFE_DELETE no-op.
		SAFE_DELETE( pMonster );

		// Sonsuz dongu onleme: regen zamanini ileri at, m_bReadyToSummon reset.
		calcNextCabracamRegenTime();
		m_bReadyToSummon = false;
	}
}

bool MenegrothDoungeonZoneManager::BeReadyToSummonCabracam()
{
	if ( IsAlreadySummonCabracam() )
	{
		// ī���į�� �����ϰ� �ִ�.

		return false;

	}

	if ( m_bReadyToSummon )
	{
		return false;
	}

	cout << "CurrentTime = " << getCurrentTimeStringEx()
		<< " ZoneID=" << m_pZone->getZoneID() 
		<< " Cabracam MonsterType = " << m_summonMonsterType 
		<< " X = " << m_summonX 
		<< " Y = " << m_summonY << endl; 

	EffectSummonCabracam *pEffectSummonCabracam = NULL;

	try
	{
		pEffectSummonCabracam = new EffectSummonCabracam(m_pZone, m_summonX, m_summonY);
		pEffectSummonCabracam->setNextTime(5);

		Tile& tile = m_pZone->getTile(m_summonX, m_summonY);  // throw potential

		ObjectRegistry & objectregister = m_pZone->getObjectRegistry();
		objectregister.registerObject(pEffectSummonCabracam);

		m_pZone->addEffect(pEffectSummonCabracam);
		tile.addEffect(pEffectSummonCabracam);

		// Effect Zone+Tile tarafindan sahiplenildi; local pointer temizle.
		pEffectSummonCabracam = NULL;

		m_bReadyToSummon = true;

		SummonCabracam();

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString(STRID_SHOWUP_CABRACAM_MONSTER) );
		m_pZone->broadcastPacket(&gcSystemMessage);
	}
	catch ( Throwable& )
	{
		// Memory leak fix: getTile/registerObject fail olursa pEffectSummonCabracam orphan kalir.
		SAFE_DELETE( pEffectSummonCabracam );

		// Sonsuz heartbeat-loop onleme: regen zamanini ileri at.
		calcNextCabracamRegenTime();
		m_bReadyToSummon = false;
	}

	return true;

}

void MenegrothDoungeonZoneManager::ResetCabracamHeartToAltar()
{
	list<ObjectID_t>::iterator iter;

	iter = m_AltarToInsertCabracamHeartList.begin();
	for( ; iter != m_AltarToInsertCabracamHeartList.end() ; ++iter)
	{
		ObjectID_t ObjectID = (*iter);

		Creature* pCreature = m_pZone->getCreature(ObjectID);
		if ( pCreature != NULL && pCreature->isNPC() )
		{
			if ( pCreature->isFlag(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR) )
			{
				Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR);
				if ( pEffect != NULL )
				{
					pEffect->unaffect();
				}
			}
		}
	}

	m_AltarToInsertCabracamHeartList.clear();
}

void MenegrothDoungeonZoneManager::AddCabracamHeartToAltar(ObjectID_t ObjectID)
{
	m_AltarToInsertCabracamHeartList.push_back(ObjectID);
}

int MenegrothDoungeonZoneManager::GetCabracamHeartCountToAltar()
{
	return m_AltarToInsertCabracamHeartList.size();
}

void MenegrothDoungeonZoneManager::InstallTrap1(ZoneCoord_t trapX, ZoneCoord_t trapY)
{
	// �������� Ʈ��
	//cout << "Install EffectMenegrothTrap1" << endl;

	int iMinDamage, iMaxDamage;

	g_MenegrothDoungeonManager.getTrap1Damage(m_pZone, iMinDamage, iMaxDamage);

	EffectMenegrothTrap1 *pEffectMenegrothTrap1;
	pEffectMenegrothTrap1 = new EffectMenegrothTrap1(m_pZone, trapX, trapY);
	pEffectMenegrothTrap1->setNextTime(0);
	pEffectMenegrothTrap1->setDeadline(99999999);
	pEffectMenegrothTrap1->setDamage(iMinDamage, iMaxDamage);

	m_pZone->registerObject(pEffectMenegrothTrap1);
	m_pZone->addEffect(pEffectMenegrothTrap1);
}

void MenegrothDoungeonZoneManager::InstallTrap2(ZoneCoord_t trapX, ZoneCoord_t trapY)
{
	// �Ͻ����� Ʈ��
	//cout << "Install EffectMenegrothTrap2" << endl;

	int iMinDamage, iMaxDamage;

	g_MenegrothDoungeonManager.getTrap2Damage(m_pZone, iMinDamage, iMaxDamage);

	EffectMenegrothTrap2 *pEffectMenegrothTrap2;
	pEffectMenegrothTrap2 = new EffectMenegrothTrap2(m_pZone, trapX, trapY);
	pEffectMenegrothTrap2->setNextTime(0);
	pEffectMenegrothTrap2->setDeadline(20);		// 2 seconds
	pEffectMenegrothTrap2->setDamage(iMinDamage, iMaxDamage);

	m_pZone->registerObject(pEffectMenegrothTrap2);
	m_pZone->addEffect(pEffectMenegrothTrap2);

	/*
	GCAddEffectToTile gcAE;
	gcAE.setEffectID(pEffectMenegrothTrap2->getEffectClass());
	gcAE.setObjectID(pEffectMenegrothTrap2->getObjectID());
	gcAE.setXY(trapX, trapY);
	gcAE.setDuration(20);
	m_pZone->broadcastPacket(trapX, trapY, &gcAE);
	*/
}

void MenegrothDoungeonZoneManager::heartbeat()
{
	Timeval curTime;
	Timeval diffTime;

	getCurrentTime(curTime);

	diffTime = timediff(curTime, m_PrevTrapCheckTime);

	if ( diffTime.tv_sec >= 1 )
	{
		// 1�� ���� Ʈ�� üũ

		for(int iIndex = 0 ; iIndex < m_Trap2PosList.size() ; ++iIndex)
		{
			ZONE_COORD TrapPos;

			diffTime = timediff(curTime, m_Trap2InstallTimeList[iIndex]);

			if ( diffTime.tv_sec >= 4 + ( rand() % 4) )
			{
				TrapPos = m_Trap2PosList[iIndex];
				InstallTrap2(TrapPos.x, TrapPos.y);

				getCurrentTime(m_Trap2InstallTimeList[iIndex]);
			}

		}

		getCurrentTime(m_PrevTrapCheckTime);
	}

	if ( m_bSummonCabracam )
	{
		if ( !IsAlreadySummonCabracam() )
		{
			// ��ȯ�Ǿ� �ְ�, ���� �������� �ʴ´ٸ� ���� ����̴�.
			// ���� ���� ��� �÷��̾�� ��ǥ�� �����Ѵ�.

			giveRewardItemToAllPCInZone();

			m_bSummonCabracam = false;
		}
	}

	if ( curTime >= m_nextCabracamRegenTime )
	{
		BeReadyToSummonCabracam();
	}
}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
