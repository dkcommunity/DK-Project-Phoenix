//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectStun.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectStun.h"
#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillHandler.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

EffectStun::EffectStun(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);
	__END_CATCH
}

void EffectStun::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	__END_CATCH 

}

void EffectStun::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectStun::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectStun " << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	pCreature->removeFlag(Effect::EFFECT_CLASS_STUN);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STUN);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

/*	if (pCreature->isSlayer())
	{
		Slayer*       pTargetSlayer = dynamic_cast<Slayer*>(pCreature);
		Player*   	  pGamePlayer = pTargetSlayer->getPlayer();
//		SLAYER_RECORD prev;

//		pTargetSlayer->getSlayerRecord(prev);
//		pTargetSlayer->initAllStat();
//		pTargetSlayer->sendRealWearingInfo();
//		pTargetSlayer->sendModifyInfo(prev);

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID(pTargetSlayer->getObjectID());
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STUN);
		pGamePlayer->sendPacket(&gcRemoveEffect);
		pZone->broadcastPacket(pTargetSlayer->getX(), pTargetSlayer->getY(), &gcRemoveEffect, pTargetSlayer);
	}
	else if (pCreature->isVampire())
	{
		Vampire*       pTargetVampire = dynamic_cast<Vampire*>(pCreature);
		Player*        pGamePlayer = pTargetVampire->getPlayer();
//		VAMPIRE_RECORD prev;

//		pTargetVampire->getVampireRecord(prev);
//		pTargetVampire->initAllStat();
//		pTargetVampire->sendRealWearingInfo();
//		pTargetVampire->sendModifyInfo(prev);

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID(pTargetVampire->getObjectID());
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STUN);
		pGamePlayer->sendPacket(&gcRemoveEffect);
		pZone->broadcastPacket(pTargetVampire->getX(), pTargetVampire->getY(), &gcRemoveEffect, pTargetVampire);
	}
	else
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);
//		pMonster->initAllStat();

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID(pMonster->getObjectID());
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STUN);
		pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcRemoveEffect);
	}
	*/

	//cout << "EffectStun " << "unaffect END" << endl;

	__END_CATCH
}

void EffectStun::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectStun " << "unaffect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	//cout << "EffectStun " << "unaffect END" << endl;

	__END_CATCH
}

string EffectStun::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectStun("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

