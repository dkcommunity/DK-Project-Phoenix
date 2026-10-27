//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAura.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectPartyAura.h"
#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Player.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectPartyAura::EffectPartyAura(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	m_bBroadcastingEffect = false;

	m_bHP		= false;
	m_bDefense	= false;
	m_bToHit	= false;
	m_bLuck		= false;
	m_bDamage	= false;
	m_bResist	= false;
	m_PartySize	= 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPartyAura::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectPartyAura::unaffect() 
	throw(Error)
{
	__BEGIN_TRY	

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPartyAura::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	//cout << "EffectPartyAura" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// 능력치를 정상적으로 되돌리기 위해서는 플래그를 끄고,
	// initAllStat을 불러야 한다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_PARTY_AURA);

    if (pCreature->isPC())
	{
		PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pCreature);

		pTargetPC->initAllStatAndSend();
    }
	else if (pCreature->isMonster())
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);
		pMonster->initAllStat();
	}
	else Assert(false);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;

	if ( m_bHP )
	{
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA_HP);
	}
	if ( m_bDefense )
	{
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA_DEFENSE);
	}
	if ( m_bToHit )
	{
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA_TOHIT);
	}
	if ( m_bLuck )
	{
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA_LUCK);
	}
	if ( m_bDamage )
	{
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA_DAMAGE);
	}
	if ( m_bResist )
	{
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA_RESIST);
	}
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_PARTY_AURA);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectPartyAura" << "unaffect END" << endl;

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectPartyAura::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectPartyAura("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH
}

