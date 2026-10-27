//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectADV2TRUECLOLOROFVAMPLOAD.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectADV2TRUECLOLOROFVAMPLOAD.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV2TRUECLOLOROFVAMPLOAD::EffectADV2TRUECLOLOROFVAMPLOAD(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	m_HPBonus = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2TRUECLOLOROFVAMPLOAD::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectBloodyShout" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// ÇÃ·¡±×žŠ ²öŽÙ.
	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
	Assert( pVampire != NULL );

			//VAMPIRE_RECORD prev;
			//pVampire->getVampireRecord(prev);
			pVampire->initAllStatAndSend();
			//pVampire->sendRealWearingInfo();
			//pVampire->sendModifyInfo(prev);

	// ÀÌÆåÆ®žŠ »èÁŠÇÏ¶ó°í ŸË·ÁÁØŽÙ.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectBloodyShout" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2TRUECLOLOROFVAMPLOAD::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV2TRUECLOLOROFVAMPLOAD::toString()
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

