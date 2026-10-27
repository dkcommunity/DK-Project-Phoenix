//----------------------------------------------------------------------
//
// Filename    : GCSetPositionHandler.cc
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCSetPosition.h"
#include "ClientDef.h"

#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
#endif

//----------------------------------------------------------------------
// 
// GCSetPositionHander::execute
//
//----------------------------------------------------------------------
void GCSetPositionHandler::execute ( GCSetPosition * pPacket , Player * pPlayer )
{
	CT_Done("DIAG: GCSetPositionHandler::execute() hit - Transitioning to Game World");
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	CT_Done("GCSP: pre dynamic_cast ClientPlayer pPlayer=%p", (void*)pPlayer);
	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);
	CT_Done("GCSP: post dynamic_cast pClientPlayer=%p", (void*)pClientPlayer);

	// FIX 2026-05-12: dynamic_cast NULL guard. Cast fail → setXY/setPlayerStatus NULL deref.
	if (pClientPlayer == NULL)
		return;

	DEBUG_ADD_FORMAT("Set Position To ( %d, %d ) to Dir(%d)", (int)pPacket->getX(), (int)pPacket->getY(), (int)pPacket->getDir());

	CT_Done("GCSP: pre setXY x=%d y=%d", (int)pPacket->getX(), (int)pPacket->getY());
	pClientPlayer->setXY( pPacket->getX() , pPacket->getY() );
	CT_Done("GCSP: post setXY, pre setPlayerStatus");

	pClientPlayer->setPlayerStatus( CPS_NORMAL );
	CT_Done("GCSP: post setPlayerStatus, pre InitPlayer");


	//--------------------------------------------------------
	// Player의 위치 지정
	//--------------------------------------------------------
	InitPlayer(	pPacket->getX(),
				pPacket->getY(),
				pPacket->getDir());
	CT_Done("GCSP: post InitPlayer, pre InfomationBord check");

	if ( g_pInfomationBord ) {
		CT_Done("GCSP: pre InfomationBord Parse");
		g_pInfomationBord->Parse("@HideBord");
		CT_Done("GCSP: post InfomationBord Parse");
	}

	//--------------------------------------------------------
	// 게임 시작..
	//--------------------------------------------------------
	CT_Done("DIAG: GCSetPositionHandler - transitioning to MODE_GAME");
	SetMode( MODE_GAME );
	CT_Done("GCSP: post SetMode(MODE_GAME), EXIT handler");


#endif
	
	__END_CATCH
}
