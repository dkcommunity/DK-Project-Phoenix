#include "EffectSnipingMode2.h"
#include "DB.h"
#include "Slayer.h"
#include "ZoneUtil.h"
#include "Viewable.h"
#include "Gpackets/GCRemoveEffect.h"


EffectSnipingMode2::EffectSnipingMode2(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	m_RevealRatio = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Viewable prevViewable = *(pCreature->getViewable());
	pCreature->getViewable()->addViewEffect( Effect::EFFECT_CLASS_SNIPING_MODE_2, 0 );
	pCreature->getZone()->updateViewable( pCreature, prevViewable );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode2::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode2::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer()); 

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	pCreature->removeFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2);


	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_SNIPING_MODE_2);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	pSlayer->initAllStatAndSend();

	Viewable prevViewable = *(pCreature->getViewable());
	pCreature->getViewable()->removeViewEffect( Effect::EFFECT_CLASS_SNIPING_MODE_2, 0 );
	pCreature->getZone()->updateViewable( pCreature, prevViewable );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSnipingMode2::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSnipingMode2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSnipingMode2("
		<< "ObjectID:" << getObjectID()
		<< "RevealRatio:" << m_RevealRatio
		<< ")";
	return msg.toString();

	__END_CATCH
}

