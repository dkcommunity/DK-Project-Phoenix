//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoSelfHandler.cpp
// Description : AOS/MOBA self score handler.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCMobaInfoSelf.h"
#include "ClientDef.h"

void GCMobaInfoSelfHandler::execute(GCMobaInfoSelf* pPacket, Player* pPlayer)
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	(void)pPlayer;
	if (g_pSystemMessage != NULL)
	{
		char message[160];
		sprintf(message, "AOS Lv:%u  Kill:%u  Death:%u  Monster:%u  Time:%u",
			pPacket->getLevel(),
			pPacket->getKillPlayer(),
			pPacket->getDead(),
			pPacket->getKillMonster(),
			pPacket->getTimeLeft());
		g_pSystemMessage->Add(message);
	}
#else
	(void)pPacket;
	(void)pPlayer;
#endif

	__END_CATCH
}
