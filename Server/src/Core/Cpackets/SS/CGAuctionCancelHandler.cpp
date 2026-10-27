//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionCancelHandler.cpp
// Description : Auction House cancel handler.
//////////////////////////////////////////////////////////////////////////////

#include "CGAuctionCancel.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "AuctionHouseManager.h"
#endif

void CGAuctionCancelHandler::execute(CGAuctionCancel* pPacket, Player* pPlayer)
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
		filelog("AuctionHouse.log", "[CGAuctionCancelHandler] invalid player creature player=%p", pPlayer);
		return;
	}

	g_pAuctionHouseManager->cancelListing(pPC, pPacket->getListingID());
#else
	(void)pPacket;
	(void)pPlayer;
#endif

	__END_DEBUG_EX __END_CATCH
}
