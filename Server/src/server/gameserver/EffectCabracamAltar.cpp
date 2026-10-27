//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectCabracamAltar.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "EffectCabracamAltar.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "GamePlayer.h"
#include "EventTransport.h"
#include "MenegrothDoungeonManager.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectCabracamAltar::EffectCabracamAltar(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	__END_CATCH
}

void EffectCabracamAltar::affect()
	throw(Error)
{
	affect(dynamic_cast<Creature*>(m_pTarget));
}

void EffectCabracamAltar::affect(Creature* pCreature)
	throw(Error)
{
	Assert(pCreature != NULL);

//	cout << "EffectCabracamAltar" << "affect BEGIN" << endl;

	pCreature->setFlag(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR);

	GCAddEffect gcAddEffect;
	gcAddEffect.setObjectID( pCreature->getObjectID() );
	gcAddEffect.setEffectID( getSendEffectClass() );
	gcAddEffect.setDuration( getRemainDuration() );

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffect);

}

void EffectCabracamAltar::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "EffectCabracamAltar" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// 플래그를 끈다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR);
	pCreature->deleteEffect(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 이펙트를 삭제하라고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectCabracamAltar" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectCabracamAltar::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectCabracamAltar::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectCabracamAltar("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
