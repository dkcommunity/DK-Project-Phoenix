//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectWOLFROAR.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectWOLFROAR.h"
#include "Vampire.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectWOLFROAR::EffectWOLFROAR(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isVampire());

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectWOLFROAR::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectGroundBless" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isVampire());

	// ÇÃ·¡±×žŠ ²öŽÙ.
	pCreature->removeFlag(Effect::EFFECT_CLASS_WOLFROAR);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
	Assert( pVampire != NULL );

			//VAMPIRE_RECORD prev;
			//pVampire->getVampireRecord(prev);
			pVampire->initAllStatAndSend();
			//pVampire->sendRealWearingInfo();
			//pVampire->sendModifyInfo(prev);

	// ÀÌÆåÆ®žŠ »èÁŠÇÏ¶ó°í ŸË·ÁÁØŽÙ.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_WOLFROAR);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectGroundBless" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectWOLFROAR::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectWOLFROAR::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectWOLFROAR("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

