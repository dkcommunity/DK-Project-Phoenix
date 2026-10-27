#include "EffectADV2INFINITYBLAZE.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "PacketUtil.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV2INFINITYBLAZE::EffectADV2INFINITYBLAZE(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2INFINITYBLAZE::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG


	Assert(pCreature != NULL);

	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2INFINITYBLAZE);

	if (pCreature->isPC())
	{
		PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pCreature);

		pTargetPC->initAllStatAndSend();
	}

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2INFINITYBLAZE);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);


	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2INFINITYBLAZE::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV2INFINITYBLAZE::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectADV2INFINITYBLAZE("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

