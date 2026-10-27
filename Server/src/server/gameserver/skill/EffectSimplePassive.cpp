//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSimplePassive.cpp
// Written by  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectSimplePassive.h"
#include "Slayer.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSimplePassive::EffectSimplePassive(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSimplePassive::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectSimplePassive" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// 플래그를 끈다.
	pCreature->removeFlag(m_EffectClass);

	if ( m_EffectClass == Effect::EFFECT_CLASS_BURST_GUN && pCreature->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		pSlayer->initAllStat();
	}

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);


	// 이펙트를 삭제하라고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(m_EffectClass);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectSimplePassive" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSimplePassive::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSimplePassive::toString()
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

