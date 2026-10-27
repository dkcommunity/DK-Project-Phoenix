//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMenegrothDoungeon.cpp
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "EffectMenegrothDoungeon.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "GamePlayer.h"
#include "EventTransport.h"
#include "MenegrothDoungeonManager.h"
#include "StringPool.h"

#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSystemMessage.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectMenegrothDoungeon::EffectMenegrothDoungeon(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothDoungeon::affect(Creature* pCreature)
	throw(Error)
{
	Assert(pCreature != NULL);

	cout << "EffectMenegrothDoungeon" << "affect BEGIN" << endl;

	pCreature->setFlag(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON);

	Timeval currentTime;
	getCurrentTime(currentTime);
	Timeval remainTime = timediff( m_Deadline, currentTime );
	Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

	char message[512];
	sprintf(message, g_pStringPool->c_str(STRID_MENEGROTH_DOUNGEON_LIMITED_TIMER), remainTurn);

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_TIMER );
	gcSystemMessage.setMessage(message);

	pCreature->getPlayer()->sendPacket(&gcSystemMessage);
}

void EffectMenegrothDoungeon::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	cout << "EffectMenegrothDoungeon" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// 플래그를 끈다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON);
	pCreature->deleteEffect(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 이펙트를 삭제하라고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	GamePlayer *pGamePlayer = dynamic_cast<GamePlayer *>(pCreature->getPlayer());
	EventTransport *pEventTransport = dynamic_cast<EventTransport*>(pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT));
	if ( pEventTransport == NULL )
	{
		pEventTransport = new EventTransport(pGamePlayer);
	}

	ZoneID_t targetZoneID = 0;
	ZoneCoord_t targetX;
	ZoneCoord_t targetY;

	g_MenegrothDoungeonManager.getAltarPosToMove(pCreature, targetZoneID, targetX, targetY);
		
	ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());

	if ( targetZoneID != 0 && pZoneInfo != NULL )
	{
		pEventTransport->setDeadline(0);
		pEventTransport->setZoneName(pZoneInfo->getFullName());
		pEventTransport->setTargetZone(targetZoneID, targetX, targetY);

		pGamePlayer->addEvent(pEventTransport);
	}

	//cout << "EffectMenegrothDoungeon" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothDoungeon::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectMenegrothDoungeon::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectMenegrothDoungeon("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

#endif /*  __MENEGROTH_DOUNGEON_SYSTEM__ */
