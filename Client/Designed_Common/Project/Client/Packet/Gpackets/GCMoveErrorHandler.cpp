//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMoveErrorHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMoveError.h"
#include "ClientDef.h"
#include "crash_trace_tool.h"
#include <stdio.h>

#ifndef PHOENIX_MOVE_PACKET_AUDIT_ENABLED
#define PHOENIX_MOVE_PACKET_AUDIT_ENABLED 0
#endif

extern char g_CWD[_MAX_PATH];

namespace
{
#if PHOENIX_MOVE_PACKET_AUDIT_ENABLED
	void Phoenix_MovePacketAuditWrite(const char* stage, const char* detail, int x, int y)
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
			"tick=%lu move_stage=%s detail=%s x=%d y=%d\r\n",
			(unsigned long)GetTickCount(),
			stage != NULL ? stage : "-",
			detail != NULL ? detail : "-",
			x,
			y);
		fclose(fp);
	}
#else
#define Phoenix_MovePacketAuditWrite(stage, detail, x, y) ((void)0)
#endif
}

#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCMoveErrorHandler::execute ( GCMoveError * pPacket , Player * pPlayer )

{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
/*
	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

	pClientPlayer->setX( pPacket->getX() );
	pClientPlayer->setY( pPacket->getY() );

	std::cout << "Move Back To (" << (int)pPacket->getX() << "," << (int)pPacket->getY() << ")" << std::endl;

#elif __WINDOWS__
*/

	// FIX 2026-05-12: g_pPlayer NULL guard (zone-change race'inde MoveError gelirse NULL deref).
	if (g_pPlayer == NULL)
	{
		Phoenix_MovePacketAuditWrite("MOVE_GCMOVEERROR_DROP", "G_PLAYER_NULL", pPacket->getX(), pPacket->getY());
		return;
	}

	Phoenix_MovePacketAuditWrite("MOVE_GCMOVEERROR", "SERVER_MOVE_ERROR", pPacket->getX(), pPacket->getY());
	CT_BAT("GCMoveErrorHandler: SERVER MOVE ERROR — server=(%d,%d)", (int)pPacket->getX(), (int)pPacket->getY());

	// 서버 좌표 설정
	// 2001.10.8 제거
	g_pPlayer->SetServerPosition( pPacket->getX(), pPacket->getY() );

	// MoveOK message가 하나 안 날아온다는걸 알 수 있다.
	g_pPlayer->PacketMoveNO();


#endif

	__END_CATCH
}
