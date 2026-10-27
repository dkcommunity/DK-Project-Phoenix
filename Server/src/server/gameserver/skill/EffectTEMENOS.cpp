//////////////////////////////////////////////////////////////////////////////
// Filename		: EffectTEMENOS.cpp
// Written By	:
// Description	:
//////////////////////////////////////////////////////////////////////////////

#include "EffectTEMENOS.h"
#include "DB.h"
#include "SkillHandler.h"

#include "Gpackets/GCRemoveEffect.h"

EffectTEMENOS::EffectTEMENOS( Creature* pCreature )
	throw( Error )
{
	__BEGIN_TRY

	setTarget( pCreature );
	m_DamageReduceRatio = 0;

	__END_CATCH
}

void EffectTEMENOS::affect()
	throw( Error )
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	affect( pCreature );

	__END_CATCH
}

void EffectTEMENOS::affect( Creature* pCreature )
	throw( Error )
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectTEMENOS::unaffect( Creature* pCreature )
	throw( Error )
{
	__BEGIN_TRY

	//cout << "EffectCauseCriticalWounds " << "unaffect BEGIN" << endl;

	Assert( pCreature != NULL );
	Assert( pCreature->isVampire() || pCreature->isOusters() || pCreature->isMonster() );

	pCreature->removeFlag( Effect::EFFECT_CLASS_TEMENOS );

	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCreature->getObjectID() );
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_TEMENOS );
	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect, pCreature );

	if ( pCreature->isPC() )
	{
		Player* pPlayer = pCreature->getPlayer();
		Assert( pPlayer != NULL );

		pPlayer->sendPacket( &gcRemoveEffect );
	}

	__END_CATCH
}

void EffectTEMENOS::unaffect()
	throw( Error )
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect( pCreature );

	__END_CATCH
}

string EffectTEMENOS::toString() const
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectTEMENOS("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

