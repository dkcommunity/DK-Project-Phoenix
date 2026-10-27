//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEatCorpse.cpp
// Written by  : excel96
// Description : EatCorpse에 의해서 생성되는 산성 데미지 약화 이펙트이다.
//////////////////////////////////////////////////////////////////////////////

#include "EffectEatCorpse.h"
#include "Creature.h"
#include "Zone.h"
#include "Player.h"
#include "HitRoll.h"
#include "EffectAftermath.h"
#include "EffectDoom.h"
#include "EffectBlunting.h"
#include "EffectParalyze.h"
#include "EffectSeduction.h"
#include "EffectPoison.h"
#include "EffectYellowPoisonToCreature.h"
#include "EffectDarkBluePoison.h"
#include "EffectGreenStalker.h"
#include "EffectBloodDrain.h"
#include "Vampire.h"
#include "ZoneUtil.h"

#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCChangeDarkLight.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectEatCorpse::EffectEatCorpse(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectEatCorpse::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	Assert( pCreature != NULL );
	affect(pCreature);

	__END_CATCH
}

void EffectEatCorpse::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert( pCreature != NULL );

	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	Vampire* pVampire = dynamic_cast<Vampire *>(pCreature);
	Assert( pVampire != NULL );

	HP_t RemainHP = pVampire->getHP(ATTR_CURRENT)+m_RemainHP;
	bool bReamin = ( RemainHP >= pVampire->getHP(ATTR_MAX) );
	
	if( bReamin )
	{
		pVampire->setHP(pVampire->getHP(ATTR_MAX), ATTR_CURRENT);
	}
	else
	{
		pVampire->setHP( RemainHP, ATTR_CURRENT);
	}

	// 변한 HP를 브로드 캐스팅 해준다
	GCStatusCurrentHP	pkt;
	pkt.setObjectID(pVampire->getObjectID());
	pkt.setCurrentHP(RemainHP);
	pZone->broadcastPacket( pVampire->getX(), pVampire->getY(), &pkt);

	if( bReamin )
	{
		setDeadline(0);
	}

	setNextTime(m_Delay);
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectEatCorpse::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectEatCorpse::unaffect()
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	
	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	pCreature->removeFlag(Effect::EFFECT_CLASS_EAT_CORPSE);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 이펙트가 사라졌다고 알려준다.
//	GCRemoveEffect gcRemoveEffect;
//	gcRemoveEffect.setObjectID(pCreature->getObjectID());
//	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_EAT_CORPSE);
//	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectEatCorpse::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectEatCorpse("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

