//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuctionListHandler.cpp
// Description : Auction House list packet handler.
//////////////////////////////////////////////////////////////////////////////

#include "GCAuctionList.h"
#include "../../Debug/PhoenixAuctionHouseHUD.h"

void GCAuctionListHandler::execute(GCAuctionList* pPacket, Player* pPlayer)
	throw(Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	if (pPacket != NULL)
		PhoenixImGui::UpdateAuctionHouseList(*pPacket);
	(void)pPlayer;
	__END_DEBUG_EX __END_CATCH
}
