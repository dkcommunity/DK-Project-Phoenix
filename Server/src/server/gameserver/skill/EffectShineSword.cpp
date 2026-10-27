//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectShineSword.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectShineSword.h"
#include "GamePlayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Tile.h"
#include "SkillUtil.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectShineSword::EffectShineSword( Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y )
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = X;
	m_Y = Y;

	__END_CATCH
}

void EffectShineSword::affect()
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = m_pZone;
	Assert( pZone != NULL );

	Creature* pCreature = m_pZone->getCreature( m_UserObjectID );

	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );

	for( int x = -2; x <= 2; ++x )
	{
		for( int y = -2; y <= 2; ++y )
		{
			ZoneCoord_t X = m_X + x;
			ZoneCoord_t Y = m_Y + y;

			if( rect.ptInRect( X, Y ) )
			{
				Tile& tile = pZone->getTile(X, Y);
				Creature* pTargetCreature = NULL;
				GCStatusCurrentHP gcHP;

				if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
				{
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				}

				int currentHP, finalHP = 0;
				GCModifyInformation	gcMI;

				if( pTargetCreature != NULL
//					&& !(pTargetCreature->isGOD() || pTargetCreature->isDM())
					&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)
					&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
					&& !pTargetCreature->isDead()
					&& !pTargetCreature->isNPC()
					&& !pTargetCreature->isSlayer()
					&& checkZoneLevelToHitTarget(pTargetCreature)
				  )
				{
					gcHP.setObjectID( pTargetCreature->getObjectID() );

					if( pTargetCreature->isVampire() )
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
						Assert( pVampire != NULL );

						currentHP = pVampire->getHP();
						HP_t maxHP = pVampire->getHP(ATTR_MAX);
						finalHP = currentHP - m_Damage;
						Silver_t silverDamage = pVampire->getSilverDamage();

						if( finalHP <= 50 || maxHP - maxHP/10 <= silverDamage )
						{
							continue;
						}
						else
						{
							Silver_t newSilverDamage = pVampire->getSilverDamage() + m_Damage;
							pVampire->saveSilverDamage(newSilverDamage);

							gcMI.addShortData(MODIFY_SILVER_DAMAGE, newSilverDamage);
							pVampire->setHP(finalHP);
							gcHP.setCurrentHP(finalHP);
							pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcHP);
						}
						pVampire->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isOusters() )
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
						Assert( pOusters != NULL );

						currentHP = pOusters->getHP();
						HP_t maxHP = pOusters->getHP(ATTR_MAX);
						finalHP = currentHP - m_Damage;
						Silver_t silverDamage = pOusters->getSilverDamage();

						if( finalHP <= 50 || maxHP - maxHP/10 <= silverDamage )
						{
							continue;
						}
						else
						{
							Silver_t newSilverDamage = pOusters->getSilverDamage() + m_Damage;
							pOusters->saveSilverDamage(newSilverDamage);

							gcMI.addShortData(MODIFY_SILVER_DAMAGE, newSilverDamage);
							pOusters->setHP(finalHP);
							gcHP.setCurrentHP(finalHP);
							pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcHP);
						}
						pOusters->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
						Assert( pMonster != NULL );

						currentHP = pMonster->getHP();
						finalHP = currentHP - m_Damage;

						if( finalHP <= 0 )
						{
							continue;
						}
						else
						{
							pMonster->setHP(finalHP);
							gcHP.setCurrentHP(finalHP);
							pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcHP);
						}
					}

					if( pTargetCreature->isPC() )
					{
						GCSkillToObjectOK2 gcSkillToObjectOK2;
						gcSkillToObjectOK2.setObjectID(1);
						gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
						gcSkillToObjectOK2.setDuration(0);
						pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
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
void EffectShineSword::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile( m_X, m_Y );
	tile.deleteEffect( m_ObjectID );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectShineSword::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectShineSword("
		<< ")";
	return msg.toString();

	__END_CATCH
}

