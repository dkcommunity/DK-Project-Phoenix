#include "EffectADV2DIVINETRACES.h"
#include "Slayer.h"
#include "Player.h"

#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

EffectADV2DIVINETRACES::EffectADV2DIVINETRACES(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectADV2DIVINETRACES::affect() throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	__END_CATCH
}

void EffectADV2DIVINETRACES::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert( pCreature != NULL );

	if ( !pCreature->isSlayer() )
	{
		setDeadline(0);
		return;
	}

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
	Assert( pSlayer != NULL );

	if ( pSlayer->getHP(ATTR_CURRENT) >= pSlayer->getHP(ATTR_MAX) )
	{
		setNextTime( m_Tick );
		return;
	}

	HP_t CurrentHP = pSlayer->getHP(ATTR_CURRENT);
	HP_t NewHP = min( (HP_t)pSlayer->getHP(ATTR_MAX), (HP_t)(CurrentHP + m_HealPoint ));

	pSlayer->setHP( NewHP );
	GCStatusCurrentHP gcHP;
	gcHP.setObjectID( pSlayer->getObjectID() );
	gcHP.setCurrentHP( pSlayer->getHP() );

	pSlayer->getZone()->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcHP );

	setNextTime(m_Tick);

	__END_CATCH
}

void EffectADV2DIVINETRACES::unaffect()
	    throw(Error)
{
    __BEGIN_TRY


    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);


    __END_CATCH
}

void EffectADV2DIVINETRACES::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG


	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer());

	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2DIVINETRACES);

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	pSlayer->initAllStatAndSend();

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pSlayer->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2DIVINETRACES);
	pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcRemoveEffect);


	__END_DEBUG
	__END_CATCH
}

string EffectADV2DIVINETRACES::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectADV2DIVINETRACES("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
