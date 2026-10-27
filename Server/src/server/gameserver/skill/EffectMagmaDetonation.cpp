//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMagmaDetonation.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectMagmaDetonation.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"

#include "HitRoll.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectMagmaDetonation::EffectMagmaDetonation(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_Damage = 0;
	m_UserObjectID = 0;
	m_bPlayer = bPlayer;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMagmaDetonation::affect()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pZone != NULL);

	// 이펙트를 건 크리쳐를 가져온다.
	// !! 존을 떠났을 수도 있으므로 NULL 이 될 수 있다.
	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );
	if ( m_bPlayer )
	{
//		pCastCreature = m_pZone->getCreature( m_UserObjectID );
		if ( pCastCreature == NULL )
		{
//			Tile& tile = m_pZone->getTile(m_X, m_Y);
//			tile.deleteEffect(m_ObjectID);
			setDeadline(0);
			return;
		}
	}

	Exp_t ExpUP = 0;
	
	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	// 현재 이펙트가 붙어있는 타일을 받아온다.
	// 중심타일 + 스플래쉬 타일
	for (int x=-1; x<=1; x++)
	{
		for (int y=-1; y<=1; y++)
		{
			int X = m_X + x;
			int Y = m_Y + y;

			if (!rect.ptInRect(X, Y)) continue;
			Tile& tile = m_pZone->getTile(X, Y);

			int Damage = m_Damage;

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

					// 자신은 맞지 않는다
					// 무적상태 체크. by sigi. 2002.9.5
					if (pCreature->getObjectID()==m_UserObjectID
						|| !canAttack( pCastCreature, pCreature )
						|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						|| !checkZoneLevelToHitTarget( pCreature )
					)
					{
						continue;
					}
					
					ExpUP++;

					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) ) continue;
					}

/*					if( !HitRoll::isSuccess( pCastCreature, pCreature ) )
					{
						continue;
					}
*/
					bool bCriticalHit = false;
					if(	HitRoll::isSuccess( pCastCreature, pCreature) )
					{
						Damage += computeDamage(pCastCreature, pCreature, 0, bCriticalHit)/2;
					}
					else 
					{
						continue;
					}

					//GCModifyInformation gcMI;
					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;

					if (pCreature->isVampire()) 
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

						::setDamage( pVampire, Damage, pCastCreature, SKILL_MAGMA_DETONATION_1, &gcSkillToObjectOK2, &gcAttackerMI);
					} 
					else if (pCreature->isSlayer())
					{
						// 뱀파이어가 사용했을 경우 뱀파이어는 중심 타일을 제외하고는 맞지 않는다.
						if ( m_bPlayer )
							continue;

						Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

						::setDamage( pSlayer, Damage, pCastCreature, SKILL_MAGMA_DETONATION_1, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

						::setDamage( pOusters, Damage, pCastCreature, SKILL_MAGMA_DETONATION_1, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, Damage, pCastCreature, SKILL_MAGMA_DETONATION_1, NULL, &gcAttackerMI);

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );
					}

					// 상대가 죽었다면 경험치를 올려준다.
					if ( pCreature->isDead() )
					{
						if ( pCastCreature != NULL && pCastCreature->isSlayer() )
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
							Assert( pSlayer != NULL );

							int exp = computeCreatureExp(pCreature, KILL_EXP);
							shareAttrExp( pSlayer, exp, 8, 1, 1, gcAttackerMI );
							computeAlignmentChange( pCreature, Damage, pCastCreature, &gcSkillToObjectOK2, &gcAttackerMI );

							pSlayer->getPlayer()->sendPacket( &gcAttackerMI );
						}
					}
					else
					{
						if ( pCastCreature != NULL && pCastCreature->isSlayer() )
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
							Assert( pSlayer != NULL );
						
							shareAttrExp( pSlayer, ExpUP, 8, 1, 1, gcAttackerMI );
							increaseDomainExp(pSlayer, 0, ExpUP, gcAttackerMI);

							pSlayer->getPlayer()->sendPacket( &gcAttackerMI );
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

				}
			}
		}
	}

	setNextTime(m_Tick);

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMagmaDetonation::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMagmaDetonation::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMagmaDetonation::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectMagmaDetonation" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectMagmaDetonation::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectMagmaDetonation("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectMagmaDetonationLoader* g_pEffectMagmaDetonationLoader = NULL;
