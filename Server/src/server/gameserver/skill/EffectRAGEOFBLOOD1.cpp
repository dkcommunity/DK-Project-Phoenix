//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMephisto.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectRAGEOFBLOOD.h"
#include "Creature.h"
#include "Vampire.h"
#include "Vampire.h"
#include "Monster.h"
#include "Player.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

EffectRAGEOFBLOOD::EffectRAGEOFBLOOD(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectRAGEOFBLOOD::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	if ( pCreature == NULL ) return;

	if ( !pCreature->isVampire() ) return;

	Vampire* pVampire = dynamic_cast<Vampire*>( pCreature );
	Assert( pVampire != NULL );

	HP_t	curHP = pVampire->getHP( ATTR_CURRENT );
	HP_t	maxHP = pVampire->getHP( ATTR_MAX ) - pVampire->getSilverDamage();

	HP_t	nextHP = curHP + getBonus();

	if ( maxHP - getBonus() <= curHP ) nextHP = maxHP;

	pVampire->setHP( nextHP );

	setNextTime(10);

	__END_CATCH
}

void EffectRAGEOFBLOOD::unaffect()
	    throw(Error)
{
    __BEGIN_TRY

	//cout << "EffectMephisto" << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);

	//cout << "EffectMephisto" << "unaffect END" << endl;

    __END_CATCH
}

void EffectRAGEOFBLOOD::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	//cout << "EffectMephisto" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isVampire());

	pCreature->removeFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

	Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

	pVampire->initAllStatAndSend();

	Zone* pZone = pVampire->getZone();
	Assert(pZone != NULL);

	// ÀÌÆåÆ®°¡ »ç¶óÁ³ŽÙ°í ŸË·ÁÁØŽÙ.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pVampire->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);
	pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcRemoveEffect);

	//cout << "EffectMephisto" << "unaffect END" << endl;

	__END_DEBUG
	__END_CATCH
}

string EffectRAGEOFBLOOD::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectMephisto("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
