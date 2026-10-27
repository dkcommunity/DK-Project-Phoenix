//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCashChargeURLHandler.cpp
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge URL cevabi.
//               result==0 → URL WebView2 "Cash Charge" penceresinde acilir
//               (runtime yoksa WebView2Host icinde ShellExecute fallback).
//               result!=0 → kullaniciya sistem mesaji (Ingilizce string).
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCashChargeURL.h"
#include "ClientDef.h"
#include "WebView2Host.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void GCCashChargeURLHandler::execute ( GCCashChargeURL * pPacket , Player * pPlayer )

{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	BYTE result = pPacket->getResult();

	if ( result == 0 )
	{
		// Checkout URL'ini WebView2 penceresinde ac.
		// WebView2 Runtime yoksa / olusum hatasi olursa WebView2Host
		// kendi icinde ShellExecute fallback'e duser.
		OpenCashChargeWindow( g_hWnd, pPacket->getURL() );
	}
	else
	{
		// Server-side red/hata — kullaniciya sistem mesaji goster.
		const char* message = "Cash charge request failed. Please try again later.";
		switch ( result )
		{
		case 1:
			message = "Cash charge is currently unavailable.";
			break;
		case 2:
			message = "Too many pending cash charge requests. Please wait and try again.";
			break;
		case 3:
			message = "Cash charge system error. Please try again later.";
			break;
		}

		if ( g_pSystemMessage != NULL )
			g_pSystemMessage->Add( message );
	}

#endif

	__END_CATCH
}
