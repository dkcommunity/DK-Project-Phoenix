//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRequestResurrect.cpp
// Written by  : elca
// Description : 
// 군인기술 Sniping 또는 뱀파이어 기술 Invisibility로 인해서
// 현재 점점 희미해져가고 있는(사라지고 있는) 크리쳐에 붙는 이펙트이다.
//////////////////////////////////////////////////////////////////////////////

#include "EffectRequestResurrect.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Creature.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "Gpackets/GCRemoveEffect.h"
#include "GDRLairManager.h"
#include "Store.h"
#include "GQuestManager.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectRequestResurrect::EffectRequestResurrect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);
	m_Duration = 100;						// 기본 10초
	m_HP = 1;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRequestResurrect::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRequestResurrect::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert( pCreature != NULL );

	pCreature->removeFlag( Effect::EFFECT_CLASS_REQUEST_RESURRECT );

	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCreature->getObjectID() );
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRequestResurrect::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectRequestResurrect::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectRequestResurrect("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

