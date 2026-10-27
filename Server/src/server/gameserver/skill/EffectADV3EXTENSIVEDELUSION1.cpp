#include "EffectADV3EXTENSIVEDELUSION1.h"
#include "Slayer.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV3EXTENSIVEDELUSION1::EffectADV3EXTENSIVEDELUSION1(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV3EXTENSIVEDELUSION1::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pCreature != NULL);

	pCreature->removeFlag(m_EffectClass);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);


	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(m_EffectClass);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);


	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV3EXTENSIVEDELUSION1::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV3EXTENSIVEDELUSION1::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSimplePassive("
		<< "EffectClass:" << (int)m_EffectClass
		<< ")";
	return msg.toString();

	__END_CATCH

}

