//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEdgeArch.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectEdgeArch.h"
#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Player.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectEdgeArch::EffectEdgeArch(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectEdgeArch::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectEdgeArch::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	ZoneCoord_t playerX = abs(pCreature->getX() - 75);
	ZoneCoord_t playerY = abs(pCreature->getY() - 75);

	unsigned int absoluteValue = 0;
	Damage_t    damage = 0;

	if( playerY < playerX )     absoluteValue = playerX;
	else    absoluteValue = playerY;

	if( pCreature->isDM() || pCreature->isGOD() )
	{
		return;
	}

	if( pZone->getZoneID() != 8001 )
	{
		this->unaffect();
		return;
	}

	if( absoluteValue <= 8 )
	{
		return;
	}
	else if( absoluteValue <= 12 )
	{
		damage = 50;
	}
	else if( absoluteValue <= 16 )
	{
		damage = 100;
	}
	else if( absoluteValue > 16 )
	{
		damage = 200;
	}

	HP_t currentHP, finalHP = 0;
	GCStatusCurrentHP gcHP;
	gcHP.setObjectID( pCreature->getObjectID() );

	if (pCreature->isVampire())
	{
		Vampire* pTargetVampire = dynamic_cast<Vampire*>(pCreature);
		Assert( pTargetVampire != NULL );

		currentHP = pTargetVampire->getHP();
		if( currentHP < damage )
		{
			finalHP = 0;
		}
		else
		{
			finalHP = currentHP - damage;
		}
		pTargetVampire->setHP(finalHP);
	}
	else if (pCreature->isSlayer())
	{
		Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pCreature);
		Assert( pTargetSlayer != NULL );
		currentHP = pTargetSlayer->getHP();

		if( currentHP < damage )
		{
			finalHP = 0;
		}
		else
		{
			finalHP = currentHP - damage;
		}
		pTargetSlayer->setHP(finalHP);
	}
	else if (pCreature->isOusters())
	{
		Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
		Assert( pTargetOusters != NULL );

		currentHP = pTargetOusters->getHP();
		if( currentHP < damage )
		{
			finalHP = 0;
		}
		else
		{
			finalHP = currentHP - damage;
		}
		pTargetOusters->setHP(finalHP);
	}
	else Assert(false);

	gcHP.setCurrentHP( finalHP );
	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcHP );

	setNextTime(m_Tick);

	__END_CATCH
}

void EffectEdgeArch::unaffect() 
	throw(Error)
{
	__BEGIN_TRY	

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectEdgeArch::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Zone* pZone = pCreature->getZone();

	pCreature->removeFlag( Effect::EFFECT_CLASS_EDGE_ARCH );

	GCRemoveEffect	gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_EDGE_ARCH);
	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectEdgeArch::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectEdgeArch("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
