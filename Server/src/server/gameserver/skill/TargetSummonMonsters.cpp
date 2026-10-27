//////////////////////////////////////////////////////////////////////////////
// Filename    : TargetSummonMonsters.cpp
// Written by  : Shinobi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TargetSummonMonsters.h"
#include "MonsterSummonInfo.h"
#include "ZoneUtil.h"
#include "Creature.h"
#include "MasterLairManager.h"
#include "MasterLairInfoManager.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSay.h"


//////////////////////////////////////////////////////////////////////////////
// 생성자
// 마스크를 초기화한다.
//////////////////////////////////////////////////////////////////////////////
TargetSummonMonsters::TargetSummonMonsters()
	    throw()
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 몬스터 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
void TargetSummonMonsters::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pMonster != NULL);
	Zone* pZone = pMonster->getZone();
	Assert(pZone != NULL);

	try 
	{
		// 몬스터의 상태가 하이드 일때 사용할 수 없다.
		if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			return;
		}
		// 몬스터의 상태가 인비지 일때에는 인비지 상태를 해제하여준다.
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
			if ( pInvi ) pInvi->setDeadline(0);
			//addVisibleCreature(pZone, pMonster, true);
		}

		// 몬스터가 적으로 인식하고 있는 오브젝트ID레스트를 받아온다.
		list<ObjectID_t>& EnemiesList = pMonster->getEnemies();

		list<ObjectID_t>::iterator iter = EnemiesList.begin();

		// 조건을 체크 하는 곳이지만... 지금은 사용하지 않아... true처리 하였다.
		if (true)
		{
			// 소환 될때 사용되는 정보를 만든다.
			SUMMON_INFO2 summonInfo;

			// 서먼 리스트에 존재하는 서먼 인포가 존재 하는지 체크한다.
			bool hasInfo = pMonster->getMonsterSummonInfo( summonInfo );

			if (!hasInfo || summonInfo.pMonsters==NULL)
			{
				// 소환할 몹이 없는 경우다. -_-;
				executeSkillFailNormal(pMonster, getSkillType(), NULL);

				// 마스터 레어에서 마스터가 몹을 소환할려고 한 경우
				if (pZone->isMasterLair() && pMonster->isMaster())
				{
					MasterLairManager* pMasterLairManager = pZone->getMasterLairManager();
					Assert(pMasterLairManager!=NULL);

					// 더 이상 소환할게 없다면..
					// 마스터가 직접 나서서 싸운다.
					pMasterLairManager->setMasterReady();
				}
			}

			if (pMonster->isMaster() && pZone->isMasterLair())
			{
				MasterLairManager* pMasterLairManager = pZone->getMasterLairManager();
				Assert(pMasterLairManager!=NULL);
				// minion combat에서는 지정된 좌표에 소환한다.

				MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo( pZone->getZoneID() );
				Assert(pInfo!=NULL);

				if (!pMasterLairManager->isMasterReady())
				{
					ZoneCoord_t x = pInfo->getSummonX();
					ZoneCoord_t y = pInfo->getSummonY();

					GCSay gcSay;
					gcSay.setObjectID( pMonster->getObjectID() );
					gcSay.setColor( MASTER_SAY_COLOR );
					gcSay.setMessage( pInfo->getRandomMasterSummonSay() );
					if (!gcSay.getMessage().empty())
						pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcSay);
				}

				// 마스터 레어에서는 소환된 몬스터들이 아템 안 준다.
				summonInfo.hasItem = false;
			}

			for(; iter != EnemiesList.end(); ++iter)
			{

				if( *iter == 0 )
					return;
				
				const ObjectID_t& pObjectID = *iter;
				Creature* pCreature = pZone->getCreature(pObjectID);
				
				summonInfo.scanEnemy = true;
				summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_GROUP;
				summonInfo.clanID = pMonster->getClanType(); // 주인의 clan을 따른다.
				summonInfo.X = pCreature->getX();
				summonInfo.Y = pCreature->getY();
				summonInfo.regenType = REGENTYPE_PORTAL;

				// 몬스터를 존에 추가한다.
				addMonstersToZone( pZone, summonInfo );

				//cout << "TargetSummonMonsters OK" << endl;
				GCSkillToTileOK5 _GCSkillToTileOK5;

				_GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
				_GCSkillToTileOK5.setSkillType(getSkillType());
				_GCSkillToTileOK5.setX(pCreature->getX());
				_GCSkillToTileOK5.setY(pCreature->getY());
				_GCSkillToTileOK5.setDuration(0);

				pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),  &_GCSkillToTileOK5);
			}
		}
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	__END_CATCH

}

TargetSummonMonsters g_TargetSummonMonsters;
