//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSnipingMode.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectSnipingMode.h"
#include "DB.h"
#include "Slayer.h"
#include "ZoneUtil.h"
#include "Viewable.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSnipingMode::EffectSnipingMode(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	m_RevealRatio = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Viewable prevViewable = *(pCreature->getViewable());
	pCreature->getViewable()->addViewEffect( Effect::EFFECT_CLASS_SNIPING_MODE, 0 );
	pCreature->getZone()->updateViewable( pCreature, prevViewable );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectSnipingMode " << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer()); // 슬레이어말고는 걸리지 않는다.

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// addUnSnipingModeCreature() 내부에서 플래그를 끄므로...
	pCreature->removeFlag(Effect::EFFECT_CLASS_SNIPING_MODE);
//	addUnSnipingModeCreature(pZone, pCreature, false);

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_SNIPING_MODE);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	pSlayer->initAllStatAndSend();

	Viewable prevViewable = *(pCreature->getViewable());
	pCreature->getViewable()->removeViewEffect( Effect::EFFECT_CLASS_SNIPING_MODE, 0 );
	pCreature->getZone()->updateViewable( pCreature, prevViewable );

	//cout << "EffectSnipingMode " << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSnipingMode::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSnipingMode("
		<< "ObjectID:" << getObjectID()
		<< "RevealRatio:" << m_RevealRatio
		<< ")";
	return msg.toString();

	__END_CATCH
}

