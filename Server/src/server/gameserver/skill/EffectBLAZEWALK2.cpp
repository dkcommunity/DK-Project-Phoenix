#include "EffectBLAZEWALK2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"
#include "Zone.h"
#include "ZoneUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBLAZEWALK2::EffectBLAZEWALK2(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLAZEWALK2::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	affect(pCreature);

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLAZEWALK2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	if(m_UserObjectID == NULL) return;
	
	Creature* pAttacker = pZone->getCreature( m_UserObjectID );

	Slayer* ThisSlayer = dynamic_cast<Slayer*>(pAttacker);

	int CriBonus = 0;

	if (ThisSlayer->hasRankGem(NewRankGem::SKILL_BLADE_WALK2_CRI ) )
	{
		NewRankGem* pRankGem = ThisSlayer->getRankGem( NewRankGem::SKILL_BLADE_WALK2_CRI );
		Assert( pRankGem != NULL );
		CriBonus += pRankGem->getGrade();
	}

	VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

	int cX = pCreature->getX();
	int cY = pCreature->getY();
	
	for( int x = -2; x <= 2; x++)
	{
		for(int y= -2; y <= 2; y++)
		{
			int X = cX + x;
			int Y = cY + y;

			if(!rect.ptInRect(X, Y)) continue;
			Tile& tile = pZone->getTile(X, Y);
			
			const list<Object*>& oList = tile.getObjectList();
			list<Object*>::const_iterator itr = oList.begin();
			for(; itr != oList.end(); itr++)
			{
				Assert(*itr != NULL);
				Object* pObject = *itr;
				Assert(pObject != NULL);

				if(pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pCreature2 = dynamic_cast<Creature*>(pObject);
					Assert(pCreature2 != NULL);

					if ( pCreature2 != pCreature && pCreature2->isSlayer() || isInSafeZone(pCreature2) ) continue;
	
					if (!(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE)
						&& !pCreature2->isDead()
						&& !pCreature2->isFlag(Effect::EFFECT_CLASS_COMA)
						&& canAttack( pAttacker, pCreature2 )
					   )
					{
						GCModifyInformation gcMI, gcAttackerMI;
						Damage_t damage = m_Point;
						if ( pAttacker != NULL )
						{
							bool bCriticalHit = false;
							damage += computeDamage(pAttacker, pCreature2, CriBonus, bCriticalHit);
						}
						setDamage( pCreature2, damage, pAttacker, SKILL_BLAZE_WALK_2, &gcMI, &gcAttackerMI );
						if ( pCreature2->isPC() ) pCreature2->getPlayer()->sendPacket( &gcMI );

						if (pAttacker!=NULL) 
						{
							computeAlignmentChange( pCreature2, m_Point, pAttacker, &gcMI, &gcAttackerMI );
							if ( pAttacker->isPC() )
							{ 
								if ( pAttacker->isSlayer() && !pCreature2->isSlayer() )
								{
									Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);

									if ( pSlayer != NULL )
									{
										GCModifyInformation gcMI;
										shareAttrExp( pSlayer, m_Point, 8, 1, 1, gcAttackerMI );
									}
								}

								if ( pAttacker->isPC() ) pAttacker->getPlayer()->sendPacket( &gcAttackerMI );
							}
						}
					}

				}
				
			}
		}
	}

	setDeadline(0);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLAZEWALK2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);

	if ( pCreature == NULL ) return;

	pCreature->removeFlag(Effect::EFFECT_CLASS_BLAZEWALK2);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCreature->getObjectID() );
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_BLAZEWALK2 );
	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect );				

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBLAZEWALK2::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectBLAZEWALK2("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}


