//-----------------------------------------------------------------------------
// CWaitPacketUpdate.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)
#include "CWaitPacketUpdate.h"
#include "Client.h"
#include "login_socket_trace.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void PhoenixWaitPacketTrace(const char* fmt, ...)
{
#if !PHOENIX_CLIENT_LOGS_ENABLED
	(void)fmt;
	return;
#else
	char logPath[MAX_PATH] = {0};
	if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
		char* lastSlash = strrchr(logPath, '\\');
		if (lastSlash != NULL) {
			strcpy(lastSlash + 1, "login_handshake_trace.log");
		} else {
			strcpy(logPath, "login_handshake_trace.log");
		}
	} else {
		strcpy(logPath, "login_handshake_trace.log");
	}

	FILE* fp = fopen(logPath, "a");
	if (fp == NULL) return;

	time_t now = time(NULL);
	struct tm tmNow;
	localtime_s(&tmNow, &now);
	fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d ",
	        tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
	        tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);

	va_list args;
	va_start(args, fmt);
	vfprintf(fp, fmt, args);
	va_end(args);

	fputc('\n', fp);
	fclose(fp);
#endif
}


// Global
CWaitPacketUpdate*		g_pCWaitPacketUpdate = NULL;

//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
CWaitPacketUpdate::Init()
{
	// mouse event 처리
	g_pDXInput->SetMouseEventReceiver( NULL );

	// keyboard event 처리
	g_pDXInput->SetKeyboardEventReceiver( NULL );
}


//-----------------------------------------------------------------------------
// Update WaitPosition
//-----------------------------------------------------------------------------
// 게임 시작 전에.. Player의 좌표를 받기 위해서 기다린다.
//-----------------------------------------------------------------------------
void 
CWaitPacketUpdate::Update()
{
	static uint updateCount = 0;
	if (++updateCount % 100 == 0) CT_Done("DIAG: CWaitPacketUpdate::Update() hit (count:%u)", updateCount);
	if (updateCount <= 40 || updateCount % 60 == 0) {
		PhoenixWaitPacketTrace("CWaitPacketUpdate Update count=%u mode=%d modeNext=%d current=%lu limit=%lu",
			updateCount,
			(int)g_Mode,
			(int)g_ModeNext,
			(unsigned long)g_CurrentTime,
			(unsigned long)m_DelayLimit);
	}

	//------------------------------------------	
	// 한계 시간을 넘어갔으면..
	//---
	if ((DWORD)g_CurrentTime > m_DelayLimit)
	{
		PhoenixWaitPacketTrace("CWaitPacketUpdate timeout mode=%d current=%lu limit=%lu",
			(int)g_Mode,
			(unsigned long)g_CurrentTime,
			(unsigned long)m_DelayLimit);
		CT_Done("DIAG: CWaitPacketUpdate timeout reached (current:%d, limit:%d)", g_CurrentTime, m_DelayLimit);
		PhoenixLoginSocketTrace("CWaitPacketUpdate: timeout current=%lu limit=%lu",
			(unsigned long)g_CurrentTime,
			(unsigned long)m_DelayLimit);
		DEBUG_ADD_FORMAT("[WaitPacket] Timeout : Current=%d, Limit=%d", g_CurrentTime, m_DelayLimit);

		SetMode( MODE_MAINMENU );
		UpdateDisconnected();
		return;
	}

	//------------------------------------------
	// Socket부분 처리
	//------------------------------------------	
	if (!UpdateSocketInput())
	{
		PhoenixWaitPacketTrace("CWaitPacketUpdate UpdateSocketInput returned false mode=%d", (int)g_Mode);
		return;
	}

	//------------------------------------------	
	// Mode가 바뀐 경우
	//------------------------------------------	
	if (g_ModeNext!=MODE_NULL)
	{
		PhoenixWaitPacketTrace("CWaitPacketUpdate modeNext transition mode=%d modeNext=%d",
			(int)g_Mode,
			(int)g_ModeNext);
		CT_Done("DIAG: CWaitPacketUpdate - transitioning to g_ModeNext=%d", (int)g_ModeNext);
		DEBUG_ADD_FORMAT("[WaitPacketUpdate] g_ModeNext is Not MODE_NULL(%d)", (int)g_ModeNext);

		SetMode( g_ModeNext );
		g_ModeNext = MODE_NULL;

		return;
	}

	UpdateSocketOutput();
	if (updateCount <= 40 || updateCount % 60 == 0) {
		PhoenixWaitPacketTrace("CWaitPacketUpdate UpdateSocketOutput done mode=%d modeNext=%d",
			(int)g_Mode,
			(int)g_ModeNext);
	}
}

//-----------------------------------------------------------------------------
// Set Delay
//-----------------------------------------------------------------------------
// 현재 시간(g_CurrentTime)부터 delay만큼 더 Update를 하다가
// Server부터 메세지가 오지 않으면 .... 프로그램 종료~
//-----------------------------------------------------------------------------
void		
CWaitPacketUpdate::SetDelay(DWORD delay)
{
	g_CurrentTime = timeGetTime();

	m_DelayLimit = g_CurrentTime + delay;
	PhoenixWaitPacketTrace("CWaitPacketUpdate SetDelay delay=%lu current=%lu limit=%lu mode=%d",
		(unsigned long)delay,
		(unsigned long)g_CurrentTime,
		(unsigned long)m_DelayLimit,
		(int)g_Mode);
	PhoenixLoginSocketTrace("CWaitPacketUpdate: SetDelay current=%lu delay=%lu limit=%lu",
		(unsigned long)g_CurrentTime,
		(unsigned long)delay,
		(unsigned long)m_DelayLimit);

	DEBUG_ADD_FORMAT("[WaitPacket] Current=%d, Limit=%d", g_CurrentTime, m_DelayLimit);
}
