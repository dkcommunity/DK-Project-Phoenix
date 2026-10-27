//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaJoinHandler.cpp
// Description : AOS/MOBA queue state handler.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCMobaJoin.h"
#include "ClientDef.h"
#include "../../Debug/PhoenixMobaHUD.h"

void GCMobaJoinHandler::execute(GCMobaJoin* pPacket, Player* pPlayer)
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	(void)pPlayer;
	PhoenixImGui::UpdateMobaQueueState(
		pPacket->getState(),
		pPacket->getJoinedCount(),
		pPacket->getMaxCount(),
		pPacket->getRemainingSeconds());

	if (g_pSystemMessage != NULL)
	{
		char message[128];
		switch (pPacket->getState())
		{
		case GCMobaJoin::QUEUE_STATE_CANCELLED:
			strcpy(message, "AOS Battle queue cancelled.");
			break;
		case GCMobaJoin::QUEUE_STATE_WAITING:
			sprintf(message, "AOS Battle queue: %u/%u player(s), %02u:%02u remaining.",
				(uint)pPacket->getJoinedCount(),
				(uint)pPacket->getMaxCount(),
				(uint)(pPacket->getRemainingSeconds() / 60),
				(uint)(pPacket->getRemainingSeconds() % 60));
			break;
		case GCMobaJoin::QUEUE_STATE_STARTED:
			strcpy(message, "AOS Battle has started.");
			break;
		case GCMobaJoin::QUEUE_STATE_TIMEOUT:
			strcpy(message, "AOS Battle queue timed out.");
			break;
		default:
			sprintf(message, "AOS Battle queue state update failed. state=%u",
				(uint)pPacket->getState());
			break;
		}
		g_pSystemMessage->Add(message);
	}
#else
	(void)pPacket;
	(void)pPlayer;
#endif

	__END_CATCH
}
