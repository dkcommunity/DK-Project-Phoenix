//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBerserker.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectADV2THEROAROFAWAKENWARRIOR.h"
#include "Slayer.h"
#include "Player.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV2THEROAROFAWAKENWARRIOR::EffectADV2THEROAROFAWAKENWARRIOR(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2THEROAROFAWAKENWARRIOR::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2THEROAROFAWAKENWARRIOR::unaffect()
	    throw(Error)
{
    __BEGIN_TRY

	//cout << "EffectBerserker " << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);

	//cout << "EffectBerserker " << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2THEROAROFAWAKENWARRIOR::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	//cout << "EffectBerserker " << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer() == true);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR);

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	pSlayer->initAllStatAndSend();

	// 捞棋飘啊 荤扼脸促绊 舅妨霖促.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pSlayer->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR);
	pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcRemoveEffect);

	//cout << "EffectBerserker " << "unaffect END" << endl;

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV2THEROAROFAWAKENWARRIOR::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectADV2TRUECLOLOROFVAMPLOAD("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

