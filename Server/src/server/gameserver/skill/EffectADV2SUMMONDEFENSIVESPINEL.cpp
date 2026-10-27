
#include "EffectADV2SUMMONDEFENSIVESPINEL.h"
#include "Ousters.h"
#include "Gpackets/GCRemoveEffect.h"
EffectADV2SUMMONDEFENSIVESPINEL::EffectADV2SUMMONDEFENSIVESPINEL(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	setTarget(pCreature);

	__END_CATCH
}
void EffectADV2SUMMONDEFENSIVESPINEL::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2SUMMONDEFENSIVESPINEL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
	Assert( pTargetOusters != NULL );

	//pTargetOusters->initAllStatAndSend();

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2SUMMONDEFENSIVESPINEL);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}
void EffectADV2SUMMONDEFENSIVESPINEL::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}
string EffectADV2SUMMONDEFENSIVESPINEL::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectADV2SUMMONDEFENSIVESPINEL("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

