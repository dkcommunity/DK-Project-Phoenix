#include "EffectADV3AURAEXPLOSION1.h"
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
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectAURAEXPLOSION1::EffectAURAEXPLOSION1(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
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
void EffectAURAEXPLOSION1::affect()
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
	Slayer* ThisSlayer = dynamic_cast<Slayer*>(pCastCreature);
	Assert(ThisSlayer != NULL);
	Exp_t ExpUP = 0;
	
	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	int HitBonus = 0;

	int diff = 2;

	for (int x=-diff; x<=diff; x++)
	{
		for (int y=-diff; y<=diff; y++)
		{
			int X = m_X + x;
			int Y = m_Y + y;

			if (!rect.ptInRect(X, Y)) continue;
			Tile& tile = m_pZone->getTile(X, Y);

			int Damage = m_Damage;

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

					bool bHitRoll = true;

					if(bHitRoll)
					{
					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;

					if (pCreature->isVampire()) 
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

						::setDamage( pVampire, Damage, pCastCreature, SKILL_AURA_EXPLOSION_1, &gcSkillToObjectOK2, &gcAttackerMI);
					} 
					else if (pCreature->isSlayer())
					{
						if ( m_bPlayer )
							continue;

						Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

						::setDamage( pSlayer, Damage, pCastCreature, SKILL_AURA_EXPLOSION_1, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

						::setDamage( pOusters, Damage, pCastCreature, SKILL_AURA_EXPLOSION_1, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, Damage, pCastCreature, SKILL_AURA_EXPLOSION_1, NULL, &gcAttackerMI);

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );
					}

					if ( pCreature->isDead() )
					{
						if ( pCastCreature != NULL && pCastCreature->isSlayer() )
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
							Assert( pSlayer != NULL );

							int exp = computeCreatureExp(pCreature, KILL_EXP);
							shareAttrExp( pSlayer, exp, 1, 1, 8, gcAttackerMI );
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
						
							shareAttrExp( pSlayer, ExpUP, 1, 1, 8, gcAttackerMI );
							increaseDomainExp(pSlayer, 0, ExpUP, gcAttackerMI);

							pSlayer->getPlayer()->sendPacket( &gcAttackerMI );
						}
					}

					if (pCreature->isPC())
					{
						gcSkillToObjectOK2.setObjectID( 1 );
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
void EffectAURAEXPLOSION1::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAURAEXPLOSION1::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAURAEXPLOSION1::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectAURAEXPLOSION1::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectAURAEXPLOSION1("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectAURAEXPLOSION1Loader* g_pEffectAURAEXPLOSION1Loader = NULL;
