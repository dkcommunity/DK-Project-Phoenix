//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHydroConvergenceBomb.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectHydroConvergenceBomb.h"
#include "GamePlayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Tile.h"
#include "SkillUtil.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Zone.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectHydroConvergenceBomb::EffectHydroConvergenceBomb( Creature* pCreature )
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_pZone = pCreature->getZone();
	m_Damage = 0;

	__END_CATCH
}

void EffectHydroConvergenceBomb::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	affect(pCreature);

	__END_CATCH
}

void EffectHydroConvergenceBomb::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	Assert(m_pZone != NULL);

	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );

	ZoneCoord_t X = pCreature->getX();
	ZoneCoord_t Y = pCreature->getY();

	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );

	for( int x = -1; x <= 1; ++x )
	{
		for( int y = -1; y <= 1; ++y )
		{
			ZoneCoord_t targetX = X + x;
			ZoneCoord_t targetY = Y + y;

			if( rect.ptInRect( targetX, targetY ) )
			{
				Tile& tile = m_pZone->getTile( targetX, targetY );
				Creature* pTargetCreature = NULL;
				GCStatusCurrentHP gcHP;

				if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
				{
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				}

				if( pTargetCreature == NULL 
#ifdef VERSION_SKILL_1
				|| HitRoll::isSuccessMagic( pCastCreature, pTargetCreature )
#endif
				)
				{
					continue;
				}

				GCModifyInformation gcMI;
				Damage_t hpDamage = 0;
				Damage_t mpDamage = 0;

				if( x == 0 && y == 0 )
				{
					hpDamage = min(265, int(getDamage()*1.3));
					mpDamage = min(225,int(getDamage()*1.1));
				}
				else
				{
					hpDamage = min(225, int(getDamage()*1.1));
					mpDamage = min(180, int(getDamage()*0.9));
				}

				if( pTargetCreature != NULL
					&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
					&& !pTargetCreature->isDead()
					&& !pTargetCreature->isNPC()
					&& !pTargetCreature->isOusters()
				)
				{
					if( pTargetCreature->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

						::setDamage( pSlayer, hpDamage, pCastCreature, SKILL_HYDRO_CONVERGENCE, &gcMI, NULL, true, false );

						MP_t CurrentMP	= pSlayer->getMP(ATTR_CURRENT);
						MP_t RemainMP	= max(0, (int)CurrentMP - mpDamage);

						pSlayer->setMP( RemainMP, ATTR_CURRENT );
						gcMI.addShortData( MODIFY_CURRENT_MP, RemainMP );
						
						pTargetCreature->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isVampire() )
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);

						Damage_t damage = Damage_t(hpDamage + mpDamage*0.4);

						::setDamage( pVampire, damage, pCastCreature, SKILL_HYDRO_CONVERGENCE, &gcMI, NULL, true, false );

						pTargetCreature->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
						
						::setDamage( pMonster, hpDamage, pCastCreature, SKILL_HYDRO_CONVERGENCE, NULL, NULL, true, false);
					}

					if( pTargetCreature->isPC() )
					{
						GCSkillToObjectOK2 gcSkillToObjectOK2;
						gcSkillToObjectOK2.setObjectID(1);
						gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
						gcSkillToObjectOK2.setDuration(0);
						pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
					}

					if( pCastCreature != NULL )
					{
						if( pTargetCreature->isDead() && pCastCreature->isOusters() )
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pCastCreature);
							Assert( pOusters!= NULL );

							int exp = computeCreatureExp(pTargetCreature, 70, pOusters);
							shareOustersExp(pOusters, exp, gcMI);
						}
					}
				}
			}
		}
	}

	setDeadline(0);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHydroConvergenceBomb::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	Assert( pCreature != NULL );

	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );
	pCreature->removeFlag(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE_BOMB);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE_BOMB);
	m_pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectHydroConvergenceBomb::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectHydroConvergenceBomb("
		<< ")";
	return msg.toString();

	__END_CATCH
}

