//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGnomesWhisper.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectGnomesWhisper.h"
#include "Ousters.h"

#include "ViewAbility.h"
#include "Zone.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGnomesWhisper::EffectGnomesWhisper(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
//	Assert(pCreature->isOusters());

	setTarget(pCreature);

	__END_CATCH
}


void EffectGnomesWhisper::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	ViewAbility prevView = *(pCreature->getViewAbility());

	if ( canSeeHide() ) pCreature->getViewAbility()->addCanSeeEffect( Effect::EFFECT_CLASS_HIDE );
	if ( canSeeInvisibility() || pCreature->isGOD() || pCreature->isDM() ) pCreature->getViewAbility()->addCanSeeEffect( Effect::EFFECT_CLASS_INVISIBILITY );
	if ( canSeeSniping() || pCreature->isGOD() || pCreature->isDM() ) pCreature->getViewAbility()->addCanSeeEffect( Effect::EFFECT_CLASS_SNIPING_MODE );

	pCreature->getZone()->updateViewAbility( pCreature, prevView );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGnomesWhisper::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	// 플래그를 끈다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 이펙트를 삭제하라고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GNOMES_WHISPER);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	ViewAbility prevView = *(pCreature->getViewAbility());//

	if ( canSeeHide() ) pCreature->getViewAbility()->removeCanSeeEffect( Effect::EFFECT_CLASS_HIDE );
	if ( canSeeInvisibility() ) pCreature->getViewAbility()->removeCanSeeEffect( Effect::EFFECT_CLASS_INVISIBILITY );
	if ( canSeeSniping() ) pCreature->getViewAbility()->removeCanSeeEffect( Effect::EFFECT_CLASS_SNIPING_MODE );

	pZone->updateViewAbility( pCreature, prevView );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGnomesWhisper::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGnomesWhisper::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectGnomesWhisper("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

