//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuctionResultHandler.cpp
// Description : Auction House result packet handler.
//////////////////////////////////////////////////////////////////////////////

#include "GCAuctionResult.h"
#include "../../Debug/PhoenixAuctionHouseHUD.h"

void GCAuctionResultHandler::execute(GCAuctionResult* pPacket, Player* pPlayer)
	throw(Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	if (pPacket != NULL)
		PhoenixImGui::UpdateAuctionHouseResult(pPacket->getResultCode(), pPacket->getMessage());
	(void)pPlayer;
	__END_DEBUG_EX __END_CATCH
}
