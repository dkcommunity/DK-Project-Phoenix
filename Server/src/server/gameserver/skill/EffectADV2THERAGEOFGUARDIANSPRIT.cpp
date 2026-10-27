//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectADV2THERAGEOFGUARDIANSPRIT.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectADV2THERAGEOFGUARDIANSPRIT.h"
#include "Ousters.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV2THERAGEOFGUARDIANSPRIT::EffectADV2THERAGEOFGUARDIANSPRIT(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2THERAGEOFGUARDIANSPRIT::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	// ÇÃ·¡±×žŠ ²öŽÙ.
	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
	Assert( pTargetOusters != NULL );

	pTargetOusters->initAllStatAndSend();

	// ÀÌÆåÆ®žŠ »èÁŠÇÏ¶ó°í ŸË·ÁÁØŽÙ.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2THERAGEOFGUARDIANSPRIT::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV2THERAGEOFGUARDIANSPRIT::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectADV2THERAGEOFGUARDIANSPRIT("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

