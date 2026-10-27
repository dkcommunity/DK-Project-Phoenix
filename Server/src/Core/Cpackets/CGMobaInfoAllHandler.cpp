//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMobaInfoAllHandler.cpp
// Description : AOS/MOBA scoreboard request handler.
//////////////////////////////////////////////////////////////////////////////

#include "CGMobaInfoAll.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "DynamicZoneMoba.h"
#endif

void CGMobaInfoAllHandler::execute(CGMobaInfoAll* pPacket, Player* pPlayer)
	throw(Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	if (pPC == NULL)
	{
		filelog("Moba.log", "[CGMobaInfoAllHandler] invalid player creature player=%p", pPlayer);
		return;
	}

	DynamicZoneMoba::SendScoreboard(pPC, pPacket->getTargetName());
#else
	(void)pPacket;
	(void)pPlayer;
#endif

	__END_DEBUG_EX __END_CATCH
}
