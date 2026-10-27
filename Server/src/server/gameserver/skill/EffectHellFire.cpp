//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHellFire.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "EffectHellFire.h"
#include "Slayer.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "ZoneUtil.h"

#include "EffectHellFireToEnemy.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectHellFire::EffectHellFire(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_CasterName ="";
	m_CasterID = 0;
	m_bForce = false;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHellFire::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectHellFire" << "affect BEGIN" << endl;
	
	Assert(m_pZone != NULL);

	// 이펙트를 사용한 크리쳐를 가져온다.
	// !! 이미 존을 나갔을 수도 있으므로 NULL이 될 수 있다.
	// by bezz. 2003.1.4
	Creature* pCastCreature = m_pZone->getCreature( m_CasterID );

	// 현재 이펙트가 붙어있는 타일을 받아온다.
    Tile& tile = m_pZone->getTile(m_X, m_Y);

	// 타일 안에 존재하는 오브젝트들을 검색한다.
    const list<Object*>& oList = tile.getObjectList();
	list<Object*>::const_iterator itr = oList.begin();
    for (; itr != oList.end(); itr++) 
	{
		Assert(*itr != NULL);

		Object* pObject = *itr;
		Assert(pObject != NULL);

    	if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
		{
			Creature* pCreature = dynamic_cast<Creature*>(pObject);
			Assert(pCreature != NULL);

			// 무적상태 체크. by sigi. 2002.9.5
			// 산 면역. by sigi. 2002.9.13
			// 자기 자신이면 안 맞는다.
			// 안전지대 체크
			// 2003.1.10 by bezz, Sequoia
			if ( !canAttack( pCastCreature, pCreature )
				|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
				|| pCreature->getObjectID()==m_CasterID
				|| !checkZoneLevelToHitTarget( pCreature )
#ifdef VERSION_SKILL_1
				|| !HitRoll::isSuccessMagic( pCastCreature, pCreature )
#endif
			)
			{
				continue;
			}
			
			// 같은 조직(--;)이면 안 맞는다.
			if ( pCreature->isOusters() && !isForce() )
			{
				continue;
			}

	//20071227 - 헬파이어 수정
	//		EffectHellFire의 affect에서도 Damage를 주도록 수정한다.
	//			대신, m_Damae의 0.4배. 주기는 2초. 총 횟수는 5회. HellFire에서 한번 Damage를 주므로, 총 5회가 된다.
			if (pCastCreature)
			{
				GCModifyInformation gcMI, gcAttackerMI;
				::setDamage( pCreature, m_Damage*0.4, pCastCreature, SKILL_HELLFIRE, &gcMI, &gcAttackerMI, true, false );

	// ::setDamage 이후의 처리는 (우선적으로) HellFire::execute 함수에서와 동일하게 처리해 본다.
				if ( pCreature->isDead())
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pCastCreature);
					Exp_t exp = computeCreatureExp( pCreature, 70, pOusters );
	//				shareOustersExp( pOusters, exp, gcAttackerMI );
	//				increaseAlignment( pCastCreature, pCreature, gcAttackerMI );
				}

	//			if ( pCastCreature->isPC() )
	//			{
	//				computeAlignmentChange(pCreature, damage, pCastCreature, &gcMI, &gcAttackerMI);
	//				pCastCreature->getPlayer()->sendPacket( &gcAttackerMI );
	//			}

				if ( pCreature->isPC() )
				{
					pCreature->getPlayer()->sendPacket( &gcMI );
				}
			}
	// -
			if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING && !pCreature->isFlag(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY))
			{
				// 이팩트 클래스를 만들어 붙인다.
				EffectHellFireToEnemy* pEffect = new EffectHellFireToEnemy(pCreature);
				pEffect->setCasterOID(m_CasterID);
				pEffect->setDeadline(m_Duration);
	//20071227 - 헬파이어 수정
	//		Damage값은 EffectHellFire::m_Damage의 1/7로 세팅한다.
				pEffect->setDamage((Damage_t)m_Damage/14);
	//			pEffect->setDamage(m_Damage);
	// -
				pEffect->setNextTime(10);
				pCreature->addEffect(pEffect);
				pCreature->setFlag(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(pCreature->getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_HELLFIRE_TO_ENEMY);
				gcAddEffect.setDuration(m_Duration);

				m_pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffect );
			}
		}
	}
	
	setNextTime(m_Tick);

	//cout << "EffectHellFire" << "affect END" << endl;

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHellFire::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHellFire::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHellFire::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectHellFire" << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectHellFire" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectHellFire::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectHellFire("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}


EffectHellFireLoader* g_pEffectHellFireLoader = NULL;
