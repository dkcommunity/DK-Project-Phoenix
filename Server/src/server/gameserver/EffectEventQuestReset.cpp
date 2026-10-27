//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEventQuestReset.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectEventQuestReset.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "GQuestManager.h"
#include "Gpackets/GCSystemMessage.h"
#include <cstdio>
#include "StringPool.h"

EffectEventQuestReset::EffectEventQuestReset(Creature* pCreature, int type) throw(Error)
	: m_Type(type)
{
	__BEGIN_TRY

	setTarget(pCreature);

	// 서버 전용 Effect이다. by sigi. 2002.11.14
	m_bBroadcastingEffect = false;

	__END_CATCH
}

EffectEventQuestReset::~EffectEventQuestReset()
	throw()
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectEventQuestReset::affect ()
	throw(Error)
{
	__BEGIN_TRY

	setNextTime(BROADCASTING_DELAY*10);
    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	__END_CATCH
}

void EffectEventQuestReset::affect (Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	if ( !pCreature->isPC() ) return;
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

//	int lastSec = getRemainDuration() * 10;
	int lastSec = timediff( m_Deadline, gCurrentTime ).tv_sec;
	cout << "lastSec : " << lastSec << endl;
	int lastHours = lastSec/3600;
	int lastMins = (lastSec%3600)/60;

	char buffer[256];
	sprintf(buffer, g_pStringPool->c_str(STRID_REMAIN_LIMIT_TIME_EVENT_QUEST), lastHours, lastMins); // 20070814 
	GCSystemMessage gcSM;
	gcSM.setMessage(buffer);
	pPC->getPlayer()->sendPacket( &gcSM );

	cout << gcSM.getMessage() << endl;

	__END_CATCH
}

void EffectEventQuestReset::unaffect()
	    throw(Error)
{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

void EffectEventQuestReset::unaffect (Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	if ( !pCreature->isPC() ) return;
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

	pPC->getGQuestManager()->eraseQuest(1001);
	pPC->getGQuestManager()->eraseQuest(1002);
	pPC->getGQuestManager()->eraseQuest(1003);
	pPC->getGQuestManager()->eraseQuest(1004);
	pPC->getGQuestManager()->eraseQuest(2001);
	pPC->getGQuestManager()->eraseQuest(2002);
	pPC->getGQuestManager()->eraseQuest(2003);
	pPC->getGQuestManager()->eraseQuest(2004);
	pPC->getGQuestManager()->eraseQuest(3001);
	pPC->getGQuestManager()->eraseQuest(3002);
	pPC->getGQuestManager()->eraseQuest(3003);
	pPC->getGQuestManager()->eraseQuest(3004);
	pPC->getGQuestManager()->refreshQuest();

	GCSystemMessage gcSM;
	gcSM.setMessage(g_pStringPool->c_str(STRID_YOU_CAN_A_LEVEL_QUEST) ); // 20070814
	pPC->getPlayer()->sendPacket( &gcSM );

	__END_CATCH
}

string EffectEventQuestReset::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectEventQuestReset("
		<< ")";
	return msg.toString();

	__END_CATCH
}

