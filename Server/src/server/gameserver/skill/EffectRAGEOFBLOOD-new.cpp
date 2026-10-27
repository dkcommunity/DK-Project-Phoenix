//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSummonCasket.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectRAGEOFBLOOD.h"
#include "Creature.h"
#include "Vampire.h"
#include "Monster.h"
#include "Player.h"
#include "Gpackets/GCRemoveEffect.h"
#include "ZoneUtil.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectRAGEOFBLOOD::EffectRAGEOFBLOOD(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);


	__END_CATCH
}

void EffectRAGEOFBLOOD::affect()
	throw(Error)
{
	__BEGIN_TRY

	if ( m_pTarget != NULL && m_pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
	{
		affect( dynamic_cast<Creature*>(m_pTarget) );
	}

	__END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRAGEOFBLOOD::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
																															    	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void EffectRAGEOFBLOOD::unaffect()
	    throw(Error)
{
    __BEGIN_TRY


    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);


    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRAGEOFBLOOD::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);
	Assert(pCreature->isVampire());

	pCreature->removeFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

	Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);


	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pVampire->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);
	pVampire->getZone()->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcRemoveEffect);


	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

string EffectRAGEOFBLOOD::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectRAGEOFBLOOD("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
