//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDEPENDSOUL.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectDEPENDSOUL.h"
#include "Slayer.h"
#include "Player.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

EffectDEPENDSOUL::EffectDEPENDSOUL(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectDEPENDSOUL::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectDEPENDSOUL::unaffect()
	    throw(Error)
{
    __BEGIN_TRY


    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);



    __END_CATCH
}

void EffectDEPENDSOUL::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG


	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer());

	pCreature->removeFlag(Effect::EFFECT_CLASS_DEPENDSOUL);

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pSlayer->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_DEPENDSOUL);
	pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcRemoveEffect);


	__END_DEBUG
	__END_CATCH
}

string EffectDEPENDSOUL::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectDEPENDSOUL("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
