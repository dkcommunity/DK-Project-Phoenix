//////////////////////////////////////////////////////////////////////////////
// Filename    : GCCashChargeURLHandler.cpp
// Description : PayPal Cash yükleme URL cevabı handler'ı.
//               Server tarafında GC handler çalışmaz — client kopyası
//               URL'i açar (GCAuctionResultHandler örüntüsü, no-op stub).
//////////////////////////////////////////////////////////////////////////////

#include "GCCashChargeURL.h"

void GCCashChargeURLHandler::execute (GCCashChargeURL* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

	(void)pPacket;
	(void)pPlayer;

	__END_DEBUG_EX __END_CATCH
}
