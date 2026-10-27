//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBatStorm2.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectBATBREAKER.h"
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
EffectBATBREAKER::EffectBATBREAKER(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
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
void EffectBATBREAKER::affect()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pZone != NULL);

	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );
	if ( m_bPlayer )
	{
		if ( pCastCreature == NULL )
		{
			setDeadline(0);
			return;
		}
	}

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	int diff =2;

	Vampire* ThisVampire = dynamic_cast<Vampire*>(pCastCreature);
	Assert(ThisVampire != NULL);

	if (ThisVampire->hasRankGem(NewRankGem::SKILL_BAT_BREAKER_RANGE ) )
	{
		NewRankGem* pRankGemBREAKERD = ThisVampire->getRankGem( NewRankGem::SKILL_BAT_BREAKER_RANGE );
		Assert( pRankGemBREAKERD != NULL );

		diff =3;
	}

	for (int x=-diff; x<=diff; x++)
	{
		for (int y=-diff; y<=diff; y++)
		{
			int X = m_X + x;
			int Y = m_Y + y;

			if (!rect.ptInRect(X, Y)) continue;
			Tile& tile = m_pZone->getTile(X, Y);

			int Damage = m_Damage;

			// ž Ÿ¿¡ žçŽ ¿ºê§®µé» °»öŽ.
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

					// º žö ŸŽŽ
					// ¹«û»ó ©. by sigi. 2002.9.5
					if (pCreature->getObjectID()==m_UserObjectID
						|| !canAttack( pCastCreature, pCreature )
						|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						|| !checkZoneLevelToHitTarget( pCreature )
					)
					{
						continue;
					}

					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) ) continue;
					}

					int hitbonus =0;
					int cribonus =0;

					if (ThisVampire->hasRankGem(NewRankGem::SKILL_BAT_BREAKER_TOHIT ) )
					{
						NewRankGem* pRankGemBREAKERTOHIT = ThisVampire->getRankGem( NewRankGem::SKILL_BAT_BREAKER_TOHIT );
						Assert( pRankGemBREAKERTOHIT != NULL );

						hitbonus += pRankGemBREAKERTOHIT->getGrade();
					}

					if (ThisVampire->hasRankGem(NewRankGem::SKILL_BAT_BREAKER_CRI ) )
					{
						NewRankGem* pRankGemBREAKERTOHIT = ThisVampire->getRankGem( NewRankGem::SKILL_BAT_BREAKER_CRI );
						Assert( pRankGemBREAKERTOHIT != NULL );

						cribonus += pRankGemBREAKERTOHIT->getGrade();
					}

					bool bHitRoll = HitRoll::isSuccess( pCastCreature, pCreature,hitbonus);
					bool bCriticalHit = false;
					if( bHitRoll)
					{
					  Damage += computeDamage(pCastCreature, pCreature, cribonus, bCriticalHit);
	
					//GCModifyInformation gcMI;
					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;

					if (pCreature->isSlayer()) 
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

						::setDamage( pSlayer, Damage, pCastCreature, SKILL_BAT_BREAKER, &gcSkillToObjectOK2, &gcAttackerMI);
					} 
					else if (pCreature->isVampire())
					{
						// ¹ìŸî°¡ »ç¿ë» °æ¿ì ¹ìŸîŽ  ž» ¿°íŽ žö ŸŽŽ.
						if ( m_bPlayer )
							continue;

						Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

						::setDamage( pVampire, Damage, pCastCreature, SKILL_BAT_BREAKER, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

						::setDamage( pOusters, Damage, pCastCreature, SKILL_BAT_BREAKER, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, Damage, pCastCreature, SKILL_BAT_BREAKER, NULL, &gcAttackerMI);

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );
					}

					// »óŽë°¡ ŸúŽžé °æè¡ž ¿·Ž.
					if ( pCreature->isDead() )
					{
						if ( pCastCreature != NULL && pCastCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pCastCreature);
							Assert( pVampire != NULL );

							int exp = computeCreatureExp(pCreature, KILL_EXP);
							shareVampExp(pVampire, exp, gcAttackerMI);
							computeAlignmentChange( pCreature, Damage, pCastCreature, &gcSkillToObjectOK2, &gcAttackerMI );

							pVampire->getPlayer()->sendPacket( &gcAttackerMI );
						}
					}

					// userŽ žŽ žð» ºž¿©Ž.
					if (pCreature->isPC())
					{
						gcSkillToObjectOK2.setObjectID( 1 );	// ¹ ŸøŽ.
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
	}

	setNextTime(m_Tick);

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBATBREAKER::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBATBREAKER::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBATBREAKER::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectBATBREAKER" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBATBREAKER::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectBATBREAKER("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectBATBREAKERLoader* g_pEffectBATBREAKERLoader = NULL;
