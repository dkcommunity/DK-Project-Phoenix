//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectStriking2.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectStriking2.h"
#include "Slayer.h"
#include "Player.h"

#include "Gpackets/GCRemoveEffect.h"

EffectStriking2::EffectStriking2(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer());

	setTarget(pCreature);

	__END_CATCH
}

void EffectStriking2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
																															    	
	__END_CATCH
}

void EffectStriking2::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectStriking2::unaffect()
	throw(Error)
{
	//cout << "EffectStriking " << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	//cout << "EffectStriking " << "unaffect END" << endl;
}

void EffectStriking2::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectStriking2 " << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer());

	// 먼저 플래그를 날린다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_STRIKING_2);

	// 능력치를 새로 계산해서 보내준다.
	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	pSlayer->initAllStatAndSend();

	// 이펙트가 사라졌다고 주위에 알린다.
	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STRIKING_2);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectStriking " << "unaffect END" << endl;

	__END_CATCH
}

void EffectStriking2::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

string EffectStriking2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectStriking2("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

