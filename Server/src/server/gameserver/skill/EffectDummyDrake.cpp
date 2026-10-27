//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDummyDrake.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectDummyDrake.h"
#include "EffectDummyDrakeBomb.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectDummyDrake::EffectDummyDrake(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrake::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectDummyDrake " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectDummyDrake " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrake::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	EffectDummyDrakeBomb* pEffect = new EffectDummyDrakeBomb(pCreature);
	pEffect->setDamage(m_Damage);
	pEffect->setUserObjectID( m_UserObjectID );
	pEffect->affect();

	pCreature->setFlag( Effect::EFFECT_CLASS_DUMMY_DRAKE_BOMB );
	pCreature->addEffect( pEffect );

	GCAddEffect gcAddEffect;
	gcAddEffect.setObjectID( pCreature->getObjectID() );
	gcAddEffect.setEffectID( Effect::EFFECT_CLASS_DUMMY_DRAKE_BOMB );

	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);

	setDeadline(0);

	//cout << "EffectDummyDrake " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrake::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_DUMMY_DRAKE);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_DUMMY_DRAKE);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrake::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectDummyDrake::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectDummyDrake("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

