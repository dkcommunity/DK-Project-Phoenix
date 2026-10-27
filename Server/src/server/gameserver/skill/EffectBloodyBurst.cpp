//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyBurst.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectBloodyBurst.h"
#include "Monster.h"
#include "Zone.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBloodyBurst::EffectBloodyBurst(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isMonster());

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyBurst::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectBloodyBurst" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// 플래그를 끈다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_BLOODY_BURST);

	Monster* pTargetMonster= dynamic_cast<Monster*>(pCreature);
	Assert( pTargetMonster!= NULL );

	// 이펙트를 삭제하라고 알려준다.
	/*
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(getSendEffectClass());
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);
	*/
	//cout << "EffectBloodyBurst" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyBurst::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBloodyBurst::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectBloodyBurst("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

