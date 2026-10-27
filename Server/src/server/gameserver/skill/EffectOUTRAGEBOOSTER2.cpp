//////////////////////////////////////////////////////////////////////////////
// Filename    : OUTRAGEBOOSTER.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectOUTRAGEBOOSTER2.h"
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
EffectOUTRAGEBOOSTER2::EffectOUTRAGEBOOSTER2(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectOUTRAGEBOOSTER2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
																															    	
	__END_CATCH
}

void EffectOUTRAGEBOOSTER2::unaffect() 
	throw(Error)
{
	__BEGIN_TRY	
    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectOUTRAGEBOOSTER2::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG


	Assert(pCreature != NULL);

	pCreature->removeFlag(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2);

	if (pCreature->isPC())
	{
		PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pCreature);

		pTargetPC->initAllStatAndSend();
	}
	else if (pCreature->isMonster())
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);
		pMonster->initAllStat();
	}
	else Assert(false);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);


	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectOUTRAGEBOOSTER2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectOUTRAGEBOOSTER2("
		<< "ObjectID:" << getObjectID()
		<< ",STR:" << m_STRPenalty
		<< ",DEX:" << m_DEXPenalty 
		<< ",INT:" << m_INTPenalty 
		<< ")";

	return msg.toString();

	__END_CATCH
}
