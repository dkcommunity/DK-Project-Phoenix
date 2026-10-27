//////////////////////////////////////////////////////////////////////////////
// Filename    : CGCashChargeRequest.cpp
// Description : PayPal Cash yükleme isteği (client -> server) — boş body.
//////////////////////////////////////////////////////////////////////////////

#include "CGCashChargeRequest.h"


void CGCashChargeRequest::read (SocketInputStream & iStream)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	// Boş body — okunacak alan yok (CGRequestPing örüntüsü)

	__END_CATCH
}

void CGCashChargeRequest::write (SocketOutputStream & oStream) const
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	// Boş body — yazılacak alan yok (CGRequestPing örüntüsü)

	__END_CATCH
}

void CGCashChargeRequest::execute (Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGCashChargeRequestHandler::execute(this , pPlayer);

	__END_CATCH
}
