//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSquallyBarrier1.cpp
// Written by  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectSquallyBarrier1.h"
#include "Ousters.h"
#include "GamePlayer.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSquallyBarrier1::EffectSquallyBarrier1(Creature* pCreature)
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
void EffectSquallyBarrier1::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	// 플래그를 끈다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_SQUALLY_BARRIER1);

	Player* pPlayer = dynamic_cast<Player*>(pCreature->getPlayer());
	Assert( pPlayer != NULL );
	
	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
	Assert( pTargetOusters != NULL );

	// 이펙트를 삭제하라고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_SQUALLY_BARRIER1);
	
	pPlayer->sendPacket( &gcRemoveEffect );
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect, pCreature);
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSquallyBarrier1::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);

	if ( pCreature != NULL )
	{
		unaffect(pCreature);
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSquallyBarrier1::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSquallyBarrier1("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

