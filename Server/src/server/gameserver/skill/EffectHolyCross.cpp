//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHolyCross.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectHolyCross.h"
#include "Slayer.h"
#include "Player.h"
#include "ZoneUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

EffectHolyCross::EffectHolyCross(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectHolyCross::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	__END_CATCH 
}

void EffectHolyCross::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	int X = pCreature->getX();
	int Y = pCreature->getY();

	for(int oY = -2; oY <= 2; oY++)
	for(int oX = -2; oX <= 2; oX++)
	{
		int tileX = X+oX;
		int tileY = Y+oY;

		if (isValidZoneCoord(pZone, tileX, tileY))
		{
			Tile& tile = pZone->getTile(tileX, tileY);

			if ( tile.hasCreature( Creature::MOVE_MODE_WALKING ) ) 
			{
				const list<Object*>& oList = tile.getObjectList();
				for(list<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++) 
				{
					Object* pTarget = *itr;
					Creature* pTargetCreature = NULL;
					if (pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE 
						&& (pTargetCreature = dynamic_cast<Creature*>(pTarget))->isSlayer()) 
					{
						Assert(pTargetCreature != NULL);
						HP_t RemainHP = 0;

						if (pTargetCreature->isSlayer() && !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA) ) 
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
							HP_t CurrentHP = pSlayer->getHP(ATTR_CURRENT);
							HP_t MaxHP = pSlayer->getHP(ATTR_MAX);

							if( CurrentHP < MaxHP )
							{
								RemainHP = min((HP_t)(CurrentHP + (MaxHP * 0.1)), (HP_t)MaxHP);
								pSlayer->setHP(RemainHP, ATTR_CURRENT);

								GCModifyInformation gcMI;
								gcMI.addShortData(MODIFY_CURRENT_HP, RemainHP);
								pSlayer->getPlayer()->sendPacket(&gcMI);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HOLY_CROSS_HEAL );
								gcAddEffect.setDuration(0);

								pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );

								Zone* pZone = pTargetCreature->getZone();
								GCStatusCurrentHP gcStatusCurrentHP;
								gcStatusCurrentHP.setObjectID(pTargetCreature->getObjectID());
								gcStatusCurrentHP.setCurrentHP(RemainHP);
								pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcStatusCurrentHP );
							}
						} 
					}
				}	
			}
		}	
	}

	setNextTime(m_Delay);

	__END_CATCH
}

void EffectHolyCross::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	pCreature->removeFlag(Effect::EFFECT_CLASS_HOLY_CROSS);

	Zone* pZone = pCreature->getZone();
	Assert(pZone!=NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_HOLY_CROSS);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

void EffectHolyCross::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

string EffectHolyCross::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectHolyCross("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

