//////////////////////////////////////////////////////////////////////////////
// Filename    : CGCashChargeRequest.cpp
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge — bos body istek (sablon: CGRequestPing)
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGCashChargeRequest.h"


void CGCashChargeRequest::read (SocketInputStream & iStream)

{
	__BEGIN_TRY

	// bos body — okunacak alan yok

	__END_CATCH
}

void CGCashChargeRequest::write (SocketOutputStream & oStream) const

{
	__BEGIN_TRY

	// bos body — yazilacak alan yok

	__END_CATCH
}

void CGCashChargeRequest::execute (Player* pPlayer)

{
	__BEGIN_TRY

	CGCashChargeRequestHandler::execute(this , pPlayer);

	__END_CATCH
}
