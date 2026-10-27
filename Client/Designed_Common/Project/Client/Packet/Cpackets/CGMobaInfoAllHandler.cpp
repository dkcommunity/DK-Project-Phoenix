//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMobaInfoAllHandler.cpp
// Description : Client-side no-op handler for packet class symmetry.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGMobaInfoAll.h"

void CGMobaInfoAllHandler::execute(CGMobaInfoAll* pPacket, Player* pPlayer)
{
	__BEGIN_TRY

	(void)pPacket;
	(void)pPlayer;

	__END_CATCH
}
