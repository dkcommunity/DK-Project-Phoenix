//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoAllHandler.cpp
// Description : AOS/MOBA team scoreboard handler.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCMobaInfoAll.h"
#include "ClientDef.h"

void GCMobaInfoAllHandler::execute(GCMobaInfoAll* pPacket, Player* pPlayer)
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	(void)pPlayer;
	if (g_pSystemMessage != NULL)
	{
		char message[128];
		sprintf(message, "AOS Battle scoreboard updated: %u player(s).", (uint)pPacket->getMembers().size());
		g_pSystemMessage->Add(message);
	}
#else
	(void)pPacket;
	(void)pPlayer;
#endif

	__END_CATCH
}
