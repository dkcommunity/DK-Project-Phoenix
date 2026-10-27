//////////////////////////////////////////////////////////////////////////////
// Filename    : TraceCreature.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TraceCreature.h"
#include "ZoneUtil.h"
#include "Creature.h"
#include "EffectWarpMonster.h"

//////////////////////////////////////////////////////////////////////////////
// 생성자
// 마스크를 초기화한다.
//////////////////////////////////////////////////////////////////////////////
TraceCreature::TraceCreature()
	    throw()
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 몬스터 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
void TraceCreature::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	//cout << "TraceCreature" << endl;

	Assert(pMonster != NULL);
	Zone* pZone = pMonster->getZone();
	Assert(pZone != NULL);

	try 
	{
		if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			return;
		}
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
			if ( pInvi ) pInvi->setDeadline(0);
		}

//		list<ObjectID_t>& EnemiesList = pMonster->getEnemies();
//		list<ObjectID_t>::iterator iter = EnemiesList.begin();

		if (true)
		{
			int targetEnemies = 0;
			int count = 0;

//			targetEnemies = rand()%EnemiesList.size();
			// 해당 존에 있는 플레이어를 모두 가져온다.
			const PCManager* pPCManager = pZone->getPCManager();
			const map< ObjectID_t, Creature* > & pZoneCreature = pPCManager->getCreatures();
			map< ObjectID_t, Creature* >::const_iterator iter = pZoneCreature.begin();

			// 존에 있는 플레이어의 숫자를 랜덤으로 추출하여 타겟을 지정한다.
			targetEnemies = rand()%pZoneCreature.size();

//			if(targetEnemies == 0)
//				return;

			for(; iter != pZoneCreature.end(); ++iter)
			{
				if( !(targetEnemies == count) )
				{
					count++;
					continue;
				}
				
				//const ObjectID_t& pObjectID = *iter;
				//Creature* pCreature = pZone->getCreature(pObjectID);
				Creature* pCreature = iter->second;

				Assert( pCreature != NULL );

				if( pCreature->isDead() )
					return;

				TPOINT pt;
				pt = findSuitablePosition(pZone, pCreature->getX(), pCreature->getY(), Creature::MOVE_MODE_WALKING);

				if( pt.x == -1 || ( pZone->getZoneLevel(pt.x, pt.y) & SAFE_ZONE) )
				{
					return;
				}

				Dir_t	targetDir = DIR_NONE;

				if( pCreature->getX() < pt.x )
				{
					if( pCreature->getY() < pt.y )		targetDir = RIGHTDOWN;
					else if( pCreature->getY() > pt.y )	targetDir = RIGHTUP;
					else								targetDir = RIGHT;
				}
				else if( pCreature->getX() > pt.x )
				{
					if( pCreature->getY() < pt.y )		targetDir = LEFTDOWN;
					else if( pCreature->getY() > pt.y )	targetDir = LEFTUP;
					else								targetDir = LEFT;
				}
				else
				{
					if( pCreature->getY() < pt.y )		targetDir = DOWN;
					else if( pCreature->getY() > pt.y )	targetDir = UP;
					else								targetDir = DIR_NONE;
				}

				pMonster->setScanEnemy();

				EffectWarpMonster* pEffect = new EffectWarpMonster( pZone, pMonster->getObjectID(), pt.x, pt.y, targetDir );
				pEffect->setNextTime( 999999 );
				pEffect->setDeadline( 0 );
				pZone->registerObject( pEffect );
				pZone->addEffect( pEffect );
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

TraceCreature g_TraceCreature;
