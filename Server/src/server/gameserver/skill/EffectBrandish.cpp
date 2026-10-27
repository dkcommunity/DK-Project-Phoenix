//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBrandish.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectBrandish.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "EffectBleeding.h"
#include "ZoneUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBrandish::EffectBrandish(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_UserObjectID = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBrandish::affect()
	throw(Error)
{
	__BEGIN_TRY

	// 이펙트를 건 크리쳐를 가져온다.
	// !! 존을 떠났을 수도 있으므로 NULL 이 될 수 있다.
	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );

	Zone* pZone = m_pZone;
	Assert(pZone != NULL);

	// 캐스터 널체크를 않했다... 2006.09.01
	if( pCastCreature == NULL )
	{
		setDeadline(0);
		return;
	}

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	// 현재 이펙트가 붙어있는 타일을 받아온다.
	// 중심타일 + 스플래쉬 타일
	for (int x=-7; x<=7; x++)
	{
		for (int y=-7; y<=7; y++)
		{
			ZoneCoord_t	X = m_X + x;
			ZoneCoord_t Y = m_Y + y;

			if ( rect.ptInRect( X, Y) )
			{
				Creature* pTargetCreature = NULL;
				Tile& tile = pZone->getTile(X, Y);

				if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
				{
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				}

				if( pTargetCreature != NULL 
					&& !(m_pZone->getZoneLevel() & COMPLETE_SAFE_ZONE)
					&& pTargetCreature->isPC()
					&& canAttack( pCastCreature, pTargetCreature )
					&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
					&& !pTargetCreature->isDead()
				  )
				{
					if( pTargetCreature->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

						GCModifyInformation gcMI;
						::setDamage( pSlayer, 200, pCastCreature, SKILL_BRANDISH, &gcMI );
						pSlayer->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isVampire() )
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);

						GCModifyInformation gcMI;
						::setDamage( pVampire, 200, pCastCreature, SKILL_BRANDISH, &gcMI );
						pVampire->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isOusters() )
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

						GCModifyInformation gcMI;
						::setDamage( pOusters, 200, pCastCreature, SKILL_BRANDISH, &gcMI );
						pOusters->getPlayer()->sendPacket(&gcMI);
					}

					if( rand()%10 < 3 )
					{
						knockbackCreature( m_pZone, pTargetCreature, m_X, m_Y );
					}

					GCSkillToObjectOK2	gcSkillToObjectOK2;
					gcSkillToObjectOK2.setObjectID( 1 );
					gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
					gcSkillToObjectOK2.setDuration( 0 );
					pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);

					if( !pTargetCreature->isFlag(EFFECT_CLASS_BLEEDING) && rand()%10 < 1)
					{
						EffectBleeding* pEffect = new EffectBleeding(pTargetCreature);
						Assert( pEffect != NULL );

						pEffect->setDeadline(10);
						pTargetCreature->setFlag(Effect::EFFECT_CLASS_SILENCE);
						pTargetCreature->addEffect(pEffect);

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
						gcAddEffect.setEffectID( Effect::EFFECT_CLASS_BLEEDING );
						gcAddEffect.setDuration(200);
						m_pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
					}
				}
			}
/*
			if (!rect.ptInRect(X, Y)) continue;
			Tile& tile = m_pZone->getTile(X, Y);

			// 타일 안에 존재하는 오브젝트들을 검색한다.
			const list<Object*>& oList = tile.getObjectList();
			list<Object*>::const_iterator itr = oList.begin();
			for (; itr != oList.end(); ++itr) 
			{
				Assert(*itr != NULL);

				Object* pObject = *itr;
				Assert(pObject != NULL);

				if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pCreature = dynamic_cast<Creature*>(pObject);
					Assert(pCreature != NULL);

					// 자신은 맞지 않는다
					// 무적상태 체크. by sigi. 2002.9.5
					if (pCreature->getObjectID()==m_UserObjectID
						|| !canAttack( pCastCreature, pCreature )
						|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						|| !checkZoneLevelToHitTarget( pCreature )
						|| pCreature->isNPC()
						|| pCreature->isDead()
						|| pCastCreature->isDead()
					)
					{
						continue;
					}

					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) ) continue;
					}

					//GCModifyInformation gcMI;
					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;

					if (pCreature->isSlayer()) 
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

						::setDamage( pSlayer, 200, pCastCreature, SKILL_BRANDISH, &gcSkillToObjectOK2, NULL);

						Player* pPlayer = pSlayer->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);

					} 
					else if (pCreature->isVampire())
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

						::setDamage( pVampire, 200, pCastCreature, SKILL_BRANDISH, &gcSkillToObjectOK2, NULL);

						Player* pPlayer = pVampire->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);
					}
					else if (pCreature->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

						::setDamage( pOusters, 200, pCastCreature, SKILL_BRANDISH, &gcSkillToObjectOK2, NULL);

						Player* pPlayer = pOusters->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);
					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, 200, pCastCreature, SKILL_BRANDISH, NULL, NULL);

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );
					}

					// 상대가 죽었다면 경험치를 올려준다.
					if ( pCreature->isDead() )
					{
						if ( pCastCreature != NULL && pCastCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pCastCreature);
							Assert( pVampire != NULL );

							int exp = computeCreatureExp(pCreature, KILL_EXP);
							shareVampExp(pVampire, exp, gcAttackerMI);
							computeAlignmentChange( pCreature, 200, pCastCreature, &gcSkillToObjectOK2, &gcAttackerMI );

							pVampire->getPlayer()->sendPacket( &gcAttackerMI );
						}
					}
					// user한테는 맞는 모습을 보여준다.
					if (pCreature->isPC())
					{
						gcSkillToObjectOK2.setObjectID( 1 );	// 의미 없다.
						gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
						gcSkillToObjectOK2.setDuration(0);
						pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
					}

					GCSkillToObjectOK4 gcSkillToObjectOK4;
					gcSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
					gcSkillToObjectOK4.setSkillType( SKILL_ATTACK_MELEE );
					gcSkillToObjectOK4.setDuration(0);

					m_pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSkillToObjectOK4, pCreature);

					if( rand()%100 < 10  && !pCreature->isFlag( Effect::EFFECT_CLASS_BLEEDING ))
					{
						EffectBleeding* pEffect = new EffectBleeding(pCreature);
						pEffect->setDeadline(400);
						pEffect->setTick(10);
						pEffect->setDamage(10);
						pEffect->setNextTime(10);

						pCreature->setFlag(Effect::EFFECT_CLASS_BLEEDING);
						pCreature->addEffect(pEffect);

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID(pCreature->getObjectID());
						gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLEEDING);
						gcAddEffect.setDuration(200);
						m_pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);
					}

					if( rand()%100 < 30 )
					{
						knockbackCreature(m_pZone, pCreature, m_X, m_Y);
					}
				}
			}
		*/
		}
	}

	setDeadline(0);

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBrandish::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBrandish::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBrandish::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectBrandish" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBrandish::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectBrandish("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectBrandishLoader* g_pEffectBrandishLoader = NULL;
