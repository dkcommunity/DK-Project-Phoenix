//--------------------------------------------------------------------------------
//
// Filename    : GCMoveOKHandler.cpp
// Written By  : elca, Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCMoveOK.h"
#include "ClientDef.h"
#include "VS_UI.h"
#include "crash_trace_tool.h"
#include <stdio.h>

#ifndef PHOENIX_MOVE_PACKET_AUDIT_ENABLED
#define PHOENIX_MOVE_PACKET_AUDIT_ENABLED 0
#endif

extern char g_CWD[_MAX_PATH];

namespace
{
#if PHOENIX_MOVE_PACKET_AUDIT_ENABLED
	void Phoenix_MovePacketAuditWrite(const char* stage, const char* detail, int x, int y, int dir)
	{
		char dataDir[_MAX_PATH];
		char logDir[_MAX_PATH];
		char logPath[_MAX_PATH];

		if (g_CWD[0] != '\0')
		{
			_snprintf(dataDir, sizeof(dataDir) - 1, "%s\\data", g_CWD);
			_snprintf(logDir, sizeof(logDir) - 1, "%s\\data\\log", g_CWD);
			_snprintf(logPath, sizeof(logPath) - 1, "%s\\data\\log\\mouse_click_audit.log", g_CWD);
		}
		else
		{
			_snprintf(dataDir, sizeof(dataDir) - 1, "data");
			_snprintf(logDir, sizeof(logDir) - 1, "data\\log");
			_snprintf(logPath, sizeof(logPath) - 1, "data\\log\\mouse_click_audit.log");
		}

		dataDir[sizeof(dataDir) - 1] = '\0';
		logDir[sizeof(logDir) - 1] = '\0';
		logPath[sizeof(logPath) - 1] = '\0';

		CreateDirectoryA(dataDir, NULL);
		CreateDirectoryA(logDir, NULL);

		FILE* fp = fopen(logPath, "a");
		if (fp == NULL)
			return;

		fprintf(fp,
			"tick=%lu move_stage=%s detail=%s x=%d y=%d dir=%d\r\n",
			(unsigned long)GetTickCount(),
			stage != NULL ? stage : "-",
			detail != NULL ? detail : "-",
			x,
			y,
			dir);
		fclose(fp);
	}
#else
#define Phoenix_MovePacketAuditWrite(stage, detail, x, y, dir) ((void)0)
#endif
}


#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCMoveOKHandler::execute ( GCMoveOK * pPacket , Player * pPlayer )

{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	// FIX 2026-05-12: dynamic_cast NULL guard + g_pPlayer NULL guard.
	// Move sık paket; cast fail veya pre-zone state'inde NULL deref crash kaynağı.
	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);
	if (pClientPlayer == NULL)
	{
		Phoenix_MovePacketAuditWrite("MOVE_GCMOVEOK_DROP", "CLIENTPLAYER_CAST_NULL", pPacket->getX(), pPacket->getY(), pPacket->getDir());
		return;
	}

	Phoenix_MovePacketAuditWrite("MOVE_GCMOVEOK", "SERVER_MOVE_OK", pPacket->getX(), pPacket->getY(), pPacket->getDir());
	CT_BAT("GCMoveOKHandler: SERVER MOVE OK — server=(%d,%d) dir=%d", (int)pPacket->getX(), (int)pPacket->getY(), (int)pPacket->getDir());

	pClientPlayer->setX( pPacket->getX() );
	pClientPlayer->setY( pPacket->getY() );
	pClientPlayer->setDir( pPacket->getDir() );

	//std::cout << "Move to (" << (int)pPacket->getX() << "," << (int)pPacket->getY() << ")" << std::endl;


	//--------------------------------------------------
	// 검증된 Tile에 대한 이동
	//--------------------------------------------------
	if (g_pPlayer != NULL)
		g_pPlayer->PacketMoveOK(pPacket->getX(), pPacket->getY(), pPacket->getDir());
	else
		Phoenix_MovePacketAuditWrite("MOVE_GCMOVEOK_DROP", "G_PLAYER_NULL", pPacket->getX(), pPacket->getY(), pPacket->getDir());

#endif

	__END_CATCH
}
