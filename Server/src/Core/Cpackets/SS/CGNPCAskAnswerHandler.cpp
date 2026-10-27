//////////////////////////////////////////////////////////////////////////////
// Filename    : CGNPCAskAnswer.cpp
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGNPCAskAnswer.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "NPC.h"
	#include <fstream>
using namespace std;
	#include "GlobalNPCManager.h"

	#include "quest/TriggerManager.h"
	#include "quest/Trigger.h"
	#include "quest/Condition.h"
	#include "quest/Action.h"

	#include "Gpackets/GCNPCResponse.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// 플레이어가 NPC를 클릭할 경우, 클라이언트가 CGNPCAskAnswer 패킷을
// 서버로 전송한다.  서버는 이 패킷을 핸들링할 때,
// NPC에 CONDITION_TALKED_BY 컨디션 플래그가 켜져 있으면,
// 트리거를 탐색하면서 적절한 트리거를 찾아서 연관된 액션을 실행한다.
//////////////////////////////////////////////////////////////////////////////
void CGNPCAskAnswerHandler::execute (CGNPCAskAnswer* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Creature*   pCreature   = NULL;

	if (pPC == NULL) 
	{
		return;
	}

	Zone* pZone = pPC->getZone();

	if (pZone == NULL) 
	{
		return; 
	}

	NPC* pNPC = NULL;

	pCreature = pPC->GetTalkingCallNPC();
	
	if ( pPC->GetTalkingCallNPC() == NULL )
	{
		// 존에 있는 NPC와 대화를 나누는 경우
		pCreature = pZone->getCreature(pPacket->getObjectID());
	}
	
	if (pCreature == NULL || !pCreature->isNPC())
	{
		GCNPCResponse okpkt;
		pPlayer->sendPacket(&okpkt);

		return;
	}
	
	pNPC = dynamic_cast<NPC*>(pCreature);
	

	if ( pPC->GetTalkingCallNPC() == NULL )
	{
		// 존에 종속된 NPC와 대화를 나누는 경우
		
		if(pPC->getDistance(pNPC->getX(), pNPC->getY()) > 10)
		{
			GCNPCResponse okpkt;
			pPlayer->sendPacket(&okpkt);
	
			return;
		}
	}
	
	COND_ANSWERED_BY cond;
	cond.ScriptID = pPacket->getScriptID();
	cond.AnswerID = pPacket->getAnswerID();
	
	if ( !pNPC->activateTrigger(pPC, Condition::CONDITION_ANSWERED_BY, Trigger::PASSIVE_TRIGGER, (void*)&cond ) )
	{
		// 해당 Condition이 없는 경우
	}

#endif

	__END_DEBUG_EX __END_CATCH
}
