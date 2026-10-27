//-----------------------------------------------------------------------------
// CGameUpdate.cpp
//-----------------------------------------------------------------------------
// ���� ������ �����ϴ� �κ�
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
// 2026-05-18 ImGui Hibrit Admin Menu — NewFrame entry
#include "Debug/PhoenixImGui.h"
#include "frame_perf_probe.h"
#include "crash_trace_tool.h"
#pragma warning(disable:4786)

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <MMSystem.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include "Client.h"
#include "GameObject.h"
#include "UserInformation.h"
#include "ServerInfo.h"
#include "PacketDef.h"
#include "VS_UI.h"
#include "VS_UI_Mouse_pointer.h"
#include "UIDialog.h"
#include "DebugInfo.h"
#include "CGameUpdate.h"
#include "MMoneyManager.h"
#include "MGameStringTable.h"
#include "MObjectSelector.h"
#include "ClientFunction.h"
#include "MMusic.h"
#include "MZoneSoundManager.h"
#include "TempInformation.h"
#include "MFakeCreature.h"
#include "MParty.h"
#include "Profiler.h"
#include "MTestDef.h"
#include "MEventManager.h"
#include "UIFunction.h"
#include "cmp3.h"
#include "COGGSTREAM.H"
#include "SystemAvailabilities.h"
#include "MWarManager.H"
#include "MItemFinder.h"
#include "MWorkThreadGlobal.h"
#include "OperatorOption.h"
#include "Packet/ClientPlayer.h"
#include "Packet/Cpackets/CGRequestPing.h"

#ifdef OUTPUT_DEBUG    
	#include "packet\Gpackets\GCSkillFailed2.h"
	#include "packet\Gpackets\GCSkillToObjectOK5.h"
	#include "packet\Gpackets\GCSkillToSelfOK2.h"
	#include "packet\Gpackets\GCSkillToTileOK2.h"
	#include "packet\Cpackets\CGTypeStringList.h"
	#include "packet\Gpackets\GCAddEffect.h"
	#include "packet\Cpackets\CGGQuestAccept.h"
#endif

	bool g_bSaveSlideScreenShot = false;
	bool g_bSlideRectSelect = false;
	RECT g_rectScreenShot = { 0, 0, 0, 0 };

#ifdef OUTPUT_DEBUG
	int gtx_op = 4;
	int gtx_src = 2;
	int gtx_dest = 2;
#endif

// Global
CGameUpdate*		g_pCGameUpdate = NULL;

bool g_bPreviousMove = false;
	
extern DWORD g_double_click_time;

// COGGSTREAM*					g_pOGG = NULL;
// CDirectSoundBuffer*			g_pSoundBufferForOGG = NULL;
// FILE	*					g_oggfile = NULL;

extern int g_SoundPerSecond;

extern int	g_previousSoundID;
extern bool g_bMouseInPortal;
		
extern POINT g_MouseSector;	
extern bool	g_bTestMode;

extern bool g_bZoneSafe;

// ����...
extern void	SetWatchMode( bool );
extern bool g_bWatchMode;

extern void	CheckTime();
extern void	KeepConnection();  
extern BOOL	InitInfomation();
extern void	ExecuteLogout();

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
extern void	ExecuteQuitExit();
#endif //__080405_FIREST_UI_UPDATE

extern char	g_CWD[_MAX_PATH];

#ifndef PHOENIX_VISUAL_AUDIT_ENABLED
#define PHOENIX_VISUAL_AUDIT_ENABLED 0
#endif

extern "C" int Phoenix_VisualAudit_IsEnabled()
{
	return PHOENIX_VISUAL_AUDIT_ENABLED;
}

extern "C" void Phoenix_VisualAudit_Log(const char* category, const char* format, ...)
{
#if PHOENIX_VISUAL_AUDIT_ENABLED
	char dataDir[_MAX_PATH];
	char logDir[_MAX_PATH];
	char logPath[_MAX_PATH];

	if (g_CWD[0] != '\0')
	{
		_snprintf(dataDir, sizeof(dataDir) - 1, "%s\\data", g_CWD);
		_snprintf(logDir, sizeof(logDir) - 1, "%s\\data\\log", g_CWD);
		_snprintf(logPath, sizeof(logPath) - 1, "%s\\data\\log\\visual_audit.log", g_CWD);
	}
	else
	{
		_snprintf(dataDir, sizeof(dataDir) - 1, "data");
		_snprintf(logDir, sizeof(logDir) - 1, "data\\log");
		_snprintf(logPath, sizeof(logPath) - 1, "data\\log\\visual_audit.log");
	}

	dataDir[sizeof(dataDir) - 1] = '\0';
	logDir[sizeof(logDir) - 1] = '\0';
	logPath[sizeof(logPath) - 1] = '\0';

	CreateDirectoryA(dataDir, NULL);
	CreateDirectoryA(logDir, NULL);

	FILE* fp = fopen(logPath, "a");
	if (fp == NULL)
	{
		OutputDebugStringA("[VisualAudit] cannot open data\\log\\visual_audit.log\r\n");
		return;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);
	fprintf(fp, "%04u-%02u-%02u %02u:%02u:%02u.%03u [%s] ",
		(unsigned)st.wYear,
		(unsigned)st.wMonth,
		(unsigned)st.wDay,
		(unsigned)st.wHour,
		(unsigned)st.wMinute,
		(unsigned)st.wSecond,
		(unsigned)st.wMilliseconds,
		category != NULL ? category : "UNKNOWN");

	va_list args;
	va_start(args, format);
	vfprintf(fp, format != NULL ? format : "-", args);
	va_end(args);

	fprintf(fp, "\r\n");
	fclose(fp);
#else
	(void)category;
	(void)format;
#endif
}

#define	MOUSE_POINTER_SKIP_LEFT		-16
#define	MOUSE_POINTER_SKIP_UP		    -16

//#define __FIX_GUNFRAME__

#ifdef __FIX_GUNFRAME__
	extern int g_tempX;
	extern int g_tempY;
#endif

namespace
{
	int g_FpsPingOverlayX = 10;
	int g_FpsPingOverlayY = 10;
	bool g_FpsPingOverlayDragging = false;
	int g_FpsPingOverlayDragOffsetX = 0;
	int g_FpsPingOverlayDragOffsetY = 0;
	bool g_FpsPingOverlayLastFrameVisible = false;

	const int kFpsPingOverlayWidth = 96;
	// 2026-09-03 R3 — 3. satir (Res: WxH) eklendi: 34 -> 50 (drag hitbox +
	// alt kenar clamp yeni satiri da kapsasin)
	const int kFpsPingOverlayHeight = 50;
	const int kFpsPingLineHeight = 16;

	int Phoenix_GetOverlayScreenWidth()
	{
		return (g_pUserInformation != NULL && g_pUserInformation->iResolution_x > 0)
			? g_pUserInformation->iResolution_x
			: 800;
	}

	int Phoenix_GetOverlayScreenHeight()
	{
		return (g_pUserInformation != NULL && g_pUserInformation->iResolution_y > 0)
			? g_pUserInformation->iResolution_y
			: 600;
	}

	int Phoenix_ClampOverlayCoord(int value, int maxValue)
	{
		if (value < 0)
			return 0;
		if (value > maxValue)
			return maxValue;
		return value;
	}

	void Phoenix_ClampFpsPingOverlay()
	{
		const int maxX = Phoenix_GetOverlayScreenWidth() - kFpsPingOverlayWidth;
		const int maxY = Phoenix_GetOverlayScreenHeight() - kFpsPingOverlayHeight;

		g_FpsPingOverlayX = Phoenix_ClampOverlayCoord(g_FpsPingOverlayX, maxX > 0 ? maxX : 0);
		g_FpsPingOverlayY = Phoenix_ClampOverlayCoord(g_FpsPingOverlayY, maxY > 0 ? maxY : 0);
	}

	bool Phoenix_IsFpsPingOverlayVisible()
	{
		return g_FpsPingOverlayLastFrameVisible
			|| (g_pUserOption != NULL && g_pUserOption->DrawFPS);
	}

	void Phoenix_SetFpsPingOverlayVisible(bool visible)
	{
		g_FpsPingOverlayLastFrameVisible = visible;
		if (!visible)
			g_FpsPingOverlayDragging = false;
	}

	bool Phoenix_IsInFpsPingOverlay(int x, int y)
	{
		Phoenix_ClampFpsPingOverlay();
		return x >= g_FpsPingOverlayX
			&& x < g_FpsPingOverlayX + kFpsPingOverlayWidth
			&& y >= g_FpsPingOverlayY
			&& y < g_FpsPingOverlayY + kFpsPingOverlayHeight;
	}

	void Phoenix_UpdateFpsPingOverlayDrag(int x, int y)
	{
		g_FpsPingOverlayX = x - g_FpsPingOverlayDragOffsetX;
		g_FpsPingOverlayY = y - g_FpsPingOverlayDragOffsetY;
		Phoenix_ClampFpsPingOverlay();
	}

	void Phoenix_ConsumeFpsPingOverlayMouseState()
	{
		if (g_pDXInput != NULL)
		{
			g_pDXInput->m_lb_down = FALSE;
			g_pDXInput->m_lb_up = FALSE;
		}

		g_bLButtonDown = FALSE;
	}

#ifndef PHOENIX_MOUSE_CLICK_AUDIT_ENABLED
#define PHOENIX_MOUSE_CLICK_AUDIT_ENABLED 0
#endif

	enum PhoenixMouseAuditButton
	{
		PHX_MOUSE_AUDIT_LEFT = 0,
		PHX_MOUSE_AUDIT_RIGHT = 1,
		PHX_MOUSE_AUDIT_COUNT = 2
	};

	struct PhoenixMouseAuditClick
	{
		bool active;
		DWORD id;
		DWORD tick;
		int x;
		int y;
	};

	struct PhoenixMouseAuditCounters
	{
		DWORD raw[PHX_MOUSE_AUDIT_COUNT];
		DWORD process[PHX_MOUSE_AUDIT_COUNT];
		DWORD ui[PHX_MOUSE_AUDIT_COUNT];
		DWORD game[PHX_MOUSE_AUDIT_COUNT];
		DWORD pending[PHX_MOUSE_AUDIT_COUNT];
		DWORD lost[PHX_MOUSE_AUDIT_COUNT];
	};

	PhoenixMouseAuditClick g_PhoenixMouseAuditClick[PHX_MOUSE_AUDIT_COUNT] = {};
	PhoenixMouseAuditCounters g_PhoenixMouseAuditCounters = {};
	DWORD g_PhoenixMouseAuditNextId = 1;
	bool g_PhoenixMouseAuditSessionStarted = false;
	const DWORD kPhoenixMouseAuditLostMs = 1500;

	const char* Phoenix_MouseAuditButtonName(PhoenixMouseAuditButton button)
	{
		return button == PHX_MOUSE_AUDIT_LEFT ? "L" : "R";
	}

	void Phoenix_MouseAuditWrite(PhoenixMouseAuditButton button, DWORD id, const char* stage, const char* detail, int x, int y)
	{
#if PHOENIX_MOUSE_CLICK_AUDIT_ENABLED
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
		{
			OutputDebugStringA("[MouseAudit] cannot open data\\log\\mouse_click_audit.log\r\n");
			return;
		}

		if (!g_PhoenixMouseAuditSessionStarted)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			fprintf(fp,
				"\r\n===== Mouse Click Audit session %04u-%02u-%02u %02u:%02u:%02u =====\r\n",
				(unsigned)st.wYear,
				(unsigned)st.wMonth,
				(unsigned)st.wDay,
				(unsigned)st.wHour,
				(unsigned)st.wMinute,
				(unsigned)st.wSecond);
			fprintf(fp, "Format: tick id button stage detail x y totals(raw/process/game/ui/pending/lost L/R)\r\n");
			g_PhoenixMouseAuditSessionStarted = true;
		}

		fprintf(fp,
			"tick=%lu id=%lu btn=%s stage=%s detail=%s x=%d y=%d totals raw=%lu/%lu process=%lu/%lu game=%lu/%lu ui=%lu/%lu pending=%lu/%lu lost=%lu/%lu\r\n",
			(unsigned long)GetTickCount(),
			(unsigned long)id,
			Phoenix_MouseAuditButtonName(button),
			stage != NULL ? stage : "-",
			detail != NULL ? detail : "-",
			x,
			y,
			(unsigned long)g_PhoenixMouseAuditCounters.raw[PHX_MOUSE_AUDIT_LEFT],
			(unsigned long)g_PhoenixMouseAuditCounters.raw[PHX_MOUSE_AUDIT_RIGHT],
			(unsigned long)g_PhoenixMouseAuditCounters.process[PHX_MOUSE_AUDIT_LEFT],
			(unsigned long)g_PhoenixMouseAuditCounters.process[PHX_MOUSE_AUDIT_RIGHT],
			(unsigned long)g_PhoenixMouseAuditCounters.game[PHX_MOUSE_AUDIT_LEFT],
			(unsigned long)g_PhoenixMouseAuditCounters.game[PHX_MOUSE_AUDIT_RIGHT],
			(unsigned long)g_PhoenixMouseAuditCounters.ui[PHX_MOUSE_AUDIT_LEFT],
			(unsigned long)g_PhoenixMouseAuditCounters.ui[PHX_MOUSE_AUDIT_RIGHT],
			(unsigned long)g_PhoenixMouseAuditCounters.pending[PHX_MOUSE_AUDIT_LEFT],
			(unsigned long)g_PhoenixMouseAuditCounters.pending[PHX_MOUSE_AUDIT_RIGHT],
			(unsigned long)g_PhoenixMouseAuditCounters.lost[PHX_MOUSE_AUDIT_LEFT],
			(unsigned long)g_PhoenixMouseAuditCounters.lost[PHX_MOUSE_AUDIT_RIGHT]);

		fclose(fp);
#else
		(void)button;
		(void)id;
		(void)stage;
		(void)detail;
		(void)x;
		(void)y;
#endif
	}

	bool Phoenix_MouseAuditButtonFromEvent(CDirectInput::E_MOUSE_EVENT event, PhoenixMouseAuditButton* outButton)
	{
		if (event == CDirectInput::LEFTDOWN)
		{
			*outButton = PHX_MOUSE_AUDIT_LEFT;
			return true;
		}

		if (event == CDirectInput::RIGHTDOWN)
		{
			*outButton = PHX_MOUSE_AUDIT_RIGHT;
			return true;
		}

		return false;
	}

	void Phoenix_MouseAuditResolveLost(PhoenixMouseAuditButton button, const char* detail)
	{
		PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[button];
		if (!click.active)
			return;

		g_PhoenixMouseAuditCounters.lost[button]++;
		Phoenix_MouseAuditWrite(button, click.id, "LOST", detail, click.x, click.y);
		click.active = false;
	}

	void Phoenix_MouseAuditResolveUi(PhoenixMouseAuditButton button, const char* detail)
	{
		PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[button];
		if (!click.active)
			return;

		g_PhoenixMouseAuditCounters.ui[button]++;
		Phoenix_MouseAuditWrite(button, click.id, "UI", detail, click.x, click.y);
		click.active = false;
	}

	void Phoenix_MouseAuditResolveGame(PhoenixMouseAuditButton button, const char* detail, int x, int y)
	{
		PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[button];
		if (!click.active)
			return;

		g_PhoenixMouseAuditCounters.game[button]++;
		Phoenix_MouseAuditWrite(button, click.id, "GAME", detail, x, y);
		click.active = false;
	}

	void Phoenix_MouseAuditMarkProcess(PhoenixMouseAuditButton button, const char* detail, int x, int y)
	{
		PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[button];
		if (!click.active)
			return;

		g_PhoenixMouseAuditCounters.process[button]++;
		Phoenix_MouseAuditWrite(button, click.id, "PROCESS", detail, x, y);
	}

	void Phoenix_MouseAuditMarkPending(PhoenixMouseAuditButton button, const char* detail, int x, int y)
	{
		PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[button];
		if (!click.active)
			return;

		g_PhoenixMouseAuditCounters.pending[button]++;
		Phoenix_MouseAuditWrite(button, click.id, "PENDING", detail, x, y);
	}

	void Phoenix_MouseAuditMarkWait(PhoenixMouseAuditButton button, const char* detail, int x, int y)
	{
		PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[button];
		if (!click.active)
			return;

		Phoenix_MouseAuditWrite(button, click.id, "WAIT", detail, x, y);
	}

	void Phoenix_MouseAuditRawDown(CDirectInput::E_MOUSE_EVENT event, int x, int y)
	{
		PhoenixMouseAuditButton button;
		if (!Phoenix_MouseAuditButtonFromEvent(event, &button))
			return;

		if (g_PhoenixMouseAuditClick[button].active)
			Phoenix_MouseAuditResolveLost(button, "REPLACED_BY_NEW_RAW_DOWN");

		DWORD id = g_PhoenixMouseAuditNextId++;
		if (g_PhoenixMouseAuditNextId == 0)
			g_PhoenixMouseAuditNextId = 1;

		g_PhoenixMouseAuditClick[button].active = true;
		g_PhoenixMouseAuditClick[button].id = id;
		g_PhoenixMouseAuditClick[button].tick = GetTickCount();
		g_PhoenixMouseAuditClick[button].x = x;
		g_PhoenixMouseAuditClick[button].y = y;
		g_PhoenixMouseAuditCounters.raw[button]++;
		Phoenix_MouseAuditWrite(button, id, "RAW", "DX_MOUSE_EVENT", x, y);
	}

	void Phoenix_MouseAuditTick()
	{
		const DWORD now = GetTickCount();
		for (int i = 0; i < PHX_MOUSE_AUDIT_COUNT; ++i)
		{
			PhoenixMouseAuditClick& click = g_PhoenixMouseAuditClick[i];
			if (click.active && now - click.tick > kPhoenixMouseAuditLostMs)
				Phoenix_MouseAuditResolveLost((PhoenixMouseAuditButton)i, "UNRESOLVED_TIMEOUT");
		}
	}

	void Phoenix_MouseAuditResolveActiveLost(const char* detail)
	{
		Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, detail);
		Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_RIGHT, detail);
	}

	void Phoenix_MouseAuditResolveActiveUi(const char* detail)
	{
		Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_LEFT, detail);
		Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_RIGHT, detail);
	}

	bool Phoenix_IsLogicalMouseButtonDown(bool leftButton);
	bool Phoenix_IsMiddleMouseButtonDown();

	// Short-lived game-world click retry for clicks that arrive during action delay.
	struct PhoenixPendingGroundClick
	{
		bool active;
		int x;
		int y;
		DWORD tick;
	};

	PhoenixPendingGroundClick g_PhoenixPendingGroundClick = { false, 0, 0, 0 };
	const DWORD kPhoenixPendingGroundClickMs = 750;

	void Phoenix_ClearPendingGroundClick()
	{
		g_PhoenixPendingGroundClick.active = false;
	}

	bool Phoenix_IsMovementModifierDown()
	{
		if (g_pDXInput == NULL)
			return true;

		return g_pDXInput->KeyDown(DIK_LSHIFT)
			|| g_pDXInput->KeyDown(DIK_RSHIFT)
			|| g_pDXInput->KeyDown(DIK_LCONTROL)
			|| g_pDXInput->KeyDown(DIK_RCONTROL);
	}

	bool Phoenix_CanQueueGroundClick()
	{
		if (g_pDXInput == NULL || g_pPlayer == NULL || g_pTopView == NULL)
			return false;

		if (Phoenix_IsMovementModifierDown())
			return false;

		if (g_pPlayer->IsInCasket()
			|| g_pPlayer->IsUndergroundCreature()
			|| g_pPlayer->CurPernalShop() == 1
			|| g_pPlayer->CurPernalShop() == 2
			|| g_pPlayer->IsRepeatAction()
			|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)
			|| gC_vs_ui.IsRunningPersnalShop()
			|| gC_vs_ui.IsRunningPayEvent()
			|| gC_vs_ui.IsInstallMineProgress()
			|| gC_vs_ui.IsSkillCastingProgress()
			|| gC_vs_ui.IsRunningTraceWindow()
			|| g_pUIDialog->IsRunningPCTalkDlg()
			|| gC_vs_ui.IsRunningElevator()
			|| gC_vs_ui.IsRunningXmasCardWindow()
			|| UI_IsRunning_WebBrowser()
			|| UI_IsRunningSelectWayPoint()
			|| g_pUIDialog->IsLockInput()
			|| g_bUIInput)
		{
			return false;
		}

		return true;
	}

	void Phoenix_RecordPendingGroundClick(int x, int y)
	{
		if (!Phoenix_CanQueueGroundClick())
			return;

		g_PhoenixPendingGroundClick.active = true;
		g_PhoenixPendingGroundClick.x = x;
		g_PhoenixPendingGroundClick.y = y;
		g_PhoenixPendingGroundClick.tick = GetTickCount();
	}

	bool Phoenix_TryImmediateGroundRetarget(MObject* pObject, const char* successDetail, const char* failDetail)
	{
		if (g_pDXInput == NULL || g_pPlayer == NULL || g_pTopView == NULL)
			return false;

		if (!g_pDXInput->m_lb_down || pObject != NULL)
			return false;

		if (!Phoenix_CanQueueGroundClick() || g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE))
			return false;

		POINT sector = g_pTopView->GetSelectedSector(g_x, g_y);
		g_SelectSector = sector;

		g_pPlayer->UnSetRequestMode();
		g_pTopView->SetSelectedSectorNULL();

		if (g_pPlayer->RequestMovementRetarget(sector.x, sector.y))
		{
			g_bPreviousMove = true;
			Phoenix_ClearPendingGroundClick();
			Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_LEFT, successDetail, sector.x, sector.y);
			return true;
		}

		Phoenix_ClearPendingGroundClick();
		Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, failDetail);
		return true;
	}

	bool Phoenix_TryConsumePendingGroundClick()
	{
		if (!g_PhoenixPendingGroundClick.active)
			return false;

		const DWORD elapsed = GetTickCount() - g_PhoenixPendingGroundClick.tick;
		if (elapsed > kPhoenixPendingGroundClickMs)
		{
			Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "PENDING_EXPIRED");
			Phoenix_ClearPendingGroundClick();
			return false;
		}

		if (!Phoenix_CanQueueGroundClick())
		{
			Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "PENDING_BLOCKED_BY_GAME_OR_UI_GATE");
			Phoenix_ClearPendingGroundClick();
			return false;
		}

		if (g_pDXInput->m_lb_down || g_pDXInput->m_rb_down || g_pDXInput->m_cb_down)
			return false;

		if (Phoenix_IsLogicalMouseButtonDown(true)
			|| Phoenix_IsLogicalMouseButtonDown(false)
			|| Phoenix_IsMiddleMouseButtonDown())
		{
			return false;
		}

		if (!g_pPlayer->IsNotDelay() || g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE))
			return false;

		MObject* pObject = g_pTopView->GetSelectedObject(g_PhoenixPendingGroundClick.x, g_PhoenixPendingGroundClick.y);
		if (pObject != NULL)
		{
			Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "PENDING_TARGET_BECAME_OBJECT");
			Phoenix_ClearPendingGroundClick();
			return false;
		}

		POINT sector = g_pTopView->GetSelectedSector(g_PhoenixPendingGroundClick.x, g_PhoenixPendingGroundClick.y);
		Phoenix_ClearPendingGroundClick();

		g_pPlayer->UnSetRequestMode();
		g_pTopView->SetSelectedSectorNULL();

		if (g_pPlayer->RequestMovementRetarget(sector.x, sector.y))
		{
			g_bPreviousMove = false;
			Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_LEFT, "PENDING_MOVE_RETARGET_OK", sector.x, sector.y);
			return true;
		}

		Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "PENDING_RETARGET_FAILED");
		return false;
	}

	bool Phoenix_IsLogicalMouseButtonDown(bool leftButton)
	{
		const bool swapped = (GetSystemMetrics(SM_SWAPBUTTON) != 0);
		const int vk = leftButton
			? (swapped ? VK_RBUTTON : VK_LBUTTON)
			: (swapped ? VK_LBUTTON : VK_RBUTTON);
		return (GetAsyncKeyState(vk) & 0x8000) != 0;
	}

	bool Phoenix_IsMiddleMouseButtonDown()
	{
		return (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
	}

	void Phoenix_ClearDxMouseEdges(bool leftButton, bool rightButton, bool centerButton)
	{
		if (g_pDXInput == NULL)
			return;

		if (leftButton)
		{
			g_pDXInput->m_lb_down = FALSE;
			g_pDXInput->m_lb_up = FALSE;
		}

		if (rightButton)
		{
			g_pDXInput->m_rb_down = FALSE;
			g_pDXInput->m_rb_up = FALSE;
		}

		if (centerButton)
		{
			g_pDXInput->m_cb_down = FALSE;
			g_pDXInput->m_cb_up = FALSE;
		}
	}

	void Phoenix_CancelClientMouseHoldState(bool leftButton, bool rightButton, bool centerButton)
	{
		if (leftButton)
		{
			g_bLButtonDown = FALSE;
			g_bPreviousMove = false;
		}

		if (rightButton)
			g_bRButtonDown = FALSE;

		if (centerButton)
			g_bCButtonDown = FALSE;

		if (g_pPlayer != NULL && (leftButton || rightButton || centerButton))
		{
			g_pPlayer->UnSetLockMode();
			if (leftButton || rightButton)
				g_pPlayer->UnSetRepeatAction();
			if (rightButton)
				g_pPlayer->UnBlazeWalk2SetLockMode();
		}
	}

	bool Phoenix_CancelMouseCaptureState(const char* reason, bool leftButton, bool rightButton, bool centerButton, bool clearDxEdges)
	{
		bool cancelled = false;

		if (gpC_window_manager != NULL)
			cancelled = gpC_window_manager->CancelMouseCaptureState();

		Phoenix_CancelClientMouseHoldState(leftButton, rightButton, centerButton);

		if (clearDxEdges)
			Phoenix_ClearDxMouseEdges(leftButton, rightButton, centerButton);

		if (cancelled)
		{
			CT_Done("[MOUSE-CAPTURE] cancel reason=%s left=%d right=%d center=%d", reason, (int)leftButton, (int)rightButton, (int)centerButton);
			Phoenix_VisualAudit_Log("MOUSE_CAPTURE",
				"cancel reason=%s left=%d right=%d center=%d clearDx=%d gL=%d gR=%d gC=%d dxEdges=%d/%d/%d/%d/%d/%d",
				reason != NULL ? reason : "-",
				(int)leftButton,
				(int)rightButton,
				(int)centerButton,
				(int)clearDxEdges,
				(int)g_bLButtonDown,
				(int)g_bRButtonDown,
				(int)g_bCButtonDown,
				g_pDXInput != NULL ? (int)g_pDXInput->m_lb_down : -1,
				g_pDXInput != NULL ? (int)g_pDXInput->m_lb_up : -1,
				g_pDXInput != NULL ? (int)g_pDXInput->m_rb_down : -1,
				g_pDXInput != NULL ? (int)g_pDXInput->m_rb_up : -1,
				g_pDXInput != NULL ? (int)g_pDXInput->m_cb_down : -1,
				g_pDXInput != NULL ? (int)g_pDXInput->m_cb_up : -1);
		}

		return cancelled;
	}

	void Phoenix_ReconcileLostMouseRelease(const char* reason)
	{
		if (g_pDXInput == NULL)
			return;

		const bool hasLeftEdge = (g_pDXInput->m_lb_down || g_pDXInput->m_lb_up);
		const bool hasRightEdge = (g_pDXInput->m_rb_down || g_pDXInput->m_rb_up);
		const bool hasCenterEdge = (g_pDXInput->m_cb_down || g_pDXInput->m_cb_up);

		const bool staleLeft = !hasLeftEdge && !Phoenix_IsLogicalMouseButtonDown(true) && g_bLButtonDown;
		const bool staleRight = !hasRightEdge && !Phoenix_IsLogicalMouseButtonDown(false) && g_bRButtonDown;
		const bool staleCenter = !hasCenterEdge && !Phoenix_IsMiddleMouseButtonDown() && g_bCButtonDown;

		const bool staleUiCapture = !hasLeftEdge && !hasRightEdge
			&& !Phoenix_IsLogicalMouseButtonDown(true)
			&& !Phoenix_IsLogicalMouseButtonDown(false)
			&& gpC_window_manager != NULL
			&& gpC_window_manager->HasMouseCaptureState();

		if (staleLeft || staleRight || staleCenter || staleUiCapture)
		{
			Phoenix_VisualAudit_Log("MOUSE_CAPTURE_STALE",
				"reason=%s staleLeft=%d staleRight=%d staleCenter=%d staleUiCapture=%d logicalDown=%d/%d middle=%d gL=%d gR=%d gC=%d",
				reason != NULL ? reason : "-",
				(int)staleLeft,
				(int)staleRight,
				(int)staleCenter,
				(int)staleUiCapture,
				(int)Phoenix_IsLogicalMouseButtonDown(true),
				(int)Phoenix_IsLogicalMouseButtonDown(false),
				(int)Phoenix_IsMiddleMouseButtonDown(),
				(int)g_bLButtonDown,
				(int)g_bRButtonDown,
				(int)g_bCButtonDown);

			Phoenix_CancelMouseCaptureState(reason,
				staleLeft || staleUiCapture,
				staleRight || staleUiCapture,
				staleCenter,
				false);
		}
	}

	bool Phoenix_HandleFpsPingOverlayMouseEvent(CDirectInput::E_MOUSE_EVENT event, int x, int y)
	{
		if (!Phoenix_IsFpsPingOverlayVisible())
		{
			g_FpsPingOverlayDragging = false;
			return false;
		}

		switch (event)
		{
			case CDirectInput::LEFTDOWN:
				if (Phoenix_IsInFpsPingOverlay(x, y))
				{
					g_FpsPingOverlayDragging = true;
					g_FpsPingOverlayDragOffsetX = x - g_FpsPingOverlayX;
					g_FpsPingOverlayDragOffsetY = y - g_FpsPingOverlayY;
					Phoenix_ConsumeFpsPingOverlayMouseState();
					return true;
				}
				break;

			case CDirectInput::MOVE:
				if (g_FpsPingOverlayDragging)
				{
					Phoenix_UpdateFpsPingOverlayDrag(x, y);
					return true;
				}
				break;

			case CDirectInput::LEFTUP:
				if (g_FpsPingOverlayDragging)
				{
					Phoenix_UpdateFpsPingOverlayDrag(x, y);
					g_FpsPingOverlayDragging = false;
					Phoenix_ConsumeFpsPingOverlayMouseState();
					return true;
				}
				break;
		}

		return false;
	}

	bool Phoenix_HandleFpsPingOverlayPolling(int x, int y)
	{
		if (!Phoenix_IsFpsPingOverlayVisible() || g_pDXInput == NULL)
			return false;

		if (g_pDXInput->m_lb_down && Phoenix_IsInFpsPingOverlay(x, y))
		{
			g_FpsPingOverlayDragging = true;
			g_FpsPingOverlayDragOffsetX = x - g_FpsPingOverlayX;
			g_FpsPingOverlayDragOffsetY = y - g_FpsPingOverlayY;
			Phoenix_ConsumeFpsPingOverlayMouseState();
			return true;
		}

		if (g_FpsPingOverlayDragging)
		{
			Phoenix_UpdateFpsPingOverlayDrag(x, y);

			if (g_pDXInput->m_lb_up)
				g_FpsPingOverlayDragging = false;

			Phoenix_ConsumeFpsPingOverlayMouseState();
			return true;
		}

		if (g_pDXInput->m_lb_up && Phoenix_IsInFpsPingOverlay(x, y))
		{
			Phoenix_ConsumeFpsPingOverlayMouseState();
			return true;
		}

		return false;
	}

	void Phoenix_SendPingRequestIfNeeded()
	{
		if (g_pSocket == NULL || g_pUserOption == NULL || !g_pUserOption->DrawFPS)
			return;

		if (g_pSocket->getPlayerStatus() != CPS_NORMAL)
			return;

		static DWORD s_lastPingRequestTime = 0;
		const DWORD now = timeGetTime();
		if (s_lastPingRequestTime != 0 && now - s_lastPingRequestTime < 5000)
			return;

		CGRequestPing ping;
		g_pSocket->sendPacket(&ping);
		g_PingSendTime = now;
		s_lastPingRequestTime = now;
	}

	COLORREF Phoenix_GetPingColor()
	{
		if (g_Ping < 200)
			return RGB(0, 255, 0);
		if (g_Ping > 200 && g_Ping < 700)
			return RGB(255, 150, 0);
		return RGB(255, 0, 0);
	}
}

// update loop�� ���� debug message�� ����ұ� ����?
#if defined(OUTPUT_DEBUG) //&& defined(_DEBUG)
	#ifdef __METROTECH_TEST__
//		#define	OUTPUT_DEBUG_UPDATE_LOOP	
//		#define OUTPUT_DEBUG_PROCESS_INPUT

		extern int  g_iAutoHealPotion;
		extern bool g_bAutoManaPotion;
		extern int  g_iAutoReload;
		extern int	g_iSpeed;
		extern bool g_bLight;
	#endif
#endif

#ifdef OUTPUT_DEBUG
	extern int g_HISTORY_LINE;
#endif

bool	g_bFrameChanged	 = false;
bool	g_bTestMusic = false;

extern LONG	g_lGameRunBreakTime;

#ifdef OUTPUT_DEBUG
std::string g_musicfilename[8]=
{
	"data\\music\\blood.mp3",
	"data\\music\\chaos.mp3",
	"data\\music\\dominator of darkness.mp3", 
	"data\\music\\oblivion.mp3",
	"data\\music\\rest.mp3",
	"data\\music\\ruin.mp3",
	"data\\music\\silence of battlefield.mp3",
	"data\\music\\underworld.mp3"
};

int			g_CurrentMusicNum = 0;
#endif

extern MCreature* AddClientCreature();

void	PacketTest();
bool IsExistCorpseFromPlayer(MCreature* OriginCreature, int creature_type);
bool			HasEffectStatusSummonSylph( MCreature* pCreature );
#if __CONTENTS(__FAST_TRANSFORTER)
bool			HasEffectStatusWingSylph( MCreature* pCreature );
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
bool			HasEffectStatusUnicorn( MCreature* pCreature );
bool			HasEffectStatusOusterNewWings1( MCreature* pCreature );
bool			HasEffectStatusOusterNewWings2( MCreature* pCreature );
#endif //__SECOND_TRANSFORTER
extern void	SetFlagTo( bool bTae );
#if __CONTENTS(__AUTO_ATTACT)
bool		IsTargetCanAutoAttcak();
#endif //__AUTO_ATTACT

CGameUpdate::CGameUpdate()
{
	m_bForceAttack = 0;
	m_userGraddisplay.StartTimer(3600000, true);
}

//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
CGameUpdate::Init() 
{
	g_bPreviousMove = false;

	// mouse event ó��
	g_pDXInput->SetMouseEventReceiver( DXMouseEvent );
	// keyboard event ó��
	g_pDXInput->SetKeyboardEventReceiver( DXKeyboardEvent ); 
}

//-----------------------------------------------------------------------------
// DXMouseEvent
//-----------------------------------------------------------------------------
void		
CGameUpdate::DXMouseEvent(CDirectInput::E_MOUSE_EVENT event, int x, int y, int z)
{
	Phoenix_MouseAuditRawDown(event, x, y);

	// 2026-05-18 ImGui Hibrit Admin Menu — AGRESIF MOUSE GATE.
	// CDirectInput DirectInput7 polling-based (WindowProc bypass).
	// WantCaptureMouse ImGui frame senkronizasyonuna bagli (NewFrame sonrasi
	// hesaplanir, DirectInput callback'i NewFrame'den once tetiklenebilir,
	// stale state). Bu nedenle DebugConsole **acikken** tum mouse event'leri
	// blocake — kullanici menu disindayken sayfaya tiklayarak karakter
	// hareket ettirmek istiyorsa once F12 ile menu kapatsin. Bu trade-off
	// "click-through bos alan" ozelligi yerine kesin guvenlik.
	if (PhoenixImGui::IsDebugConsoleVisible())
	{
		Phoenix_MouseAuditResolveActiveUi("DX_DEBUG_CONSOLE_VISIBLE");
		return;
	}

	if(g_pEventManager->GetEventByFlag(EVENTFLAG_DENY_INPUT_MOUSE))
	{
//		g_pEventManager->RemoveAllEvent();
		Phoenix_MouseAuditResolveActiveLost("DX_EVENTFLAG_DENY_INPUT_MOUSE");
		return;
	}

	POINT overlayPoint;
	GetCursorPos(&overlayPoint);
	ScreenToClient(g_hWnd, &overlayPoint);
	if (Phoenix_HandleFpsPingOverlayMouseEvent(event, overlayPoint.x, overlayPoint.y))
	{
		Phoenix_MouseAuditResolveActiveUi("DX_FPS_PING_OVERLAY");
		return;
	}

	static DWORD	last_click_time;
	static int		double_click_x, double_click_y;
	
	switch (event)
	{
			case CDirectInput::LEFTDOWN :
				// 2008. 10. 02 "�����̾� ���޾�����" ����Ŭ��ó�� ���Ͽ� �Ʒ��ڵ带 ���� ������. ij-ch
				// Start
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI) // �Ʒ���ƾ�� ������ �������� �ƴ϶�, �ּ�ó���� �ڵ忴��.
				if ((DWORD)abs(int(GetTickCount() - last_click_time)) <= g_double_click_time)
				{
					if (g_x>= double_click_x-1 && g_x <= double_click_x+1 &&
						g_y>= double_click_y-1 && g_y <= double_click_y+1)
					{
						#ifdef OUTPUT_DEBUG_UPDATE_LOOP
						DEBUG_ADD("MLD");
						#endif
						
						bool consumedDoubleClick = gC_vs_ui.MouseControl(M_LB_DOUBLECLICK, g_x, g_y);
						if (consumedDoubleClick)
						{
							Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_LEFT, "DX_UI_DOUBLECLICK_CONSUMED");
							g_pDXInput->m_lb_down = false;
							g_pDXInput->m_lb_up = false;

							#ifdef OUTPUT_DEBUG_UPDATE_LOOP
							DEBUG_ADD("MLD2");
							#endif

							last_click_time = 0;
							return;
						}
					}
				}
#endif //__PREMIUM_GIVE_ITEM_UI
				// End
				
				//gC_vs_ui.MouseControl(M_LEFTBUTTON_DOWN, g_x, g_y);
				last_click_time = GetTickCount(); 
				double_click_x = g_x;
				double_click_y = g_y;
				
//				#ifdef OUTPUT_DEBUG			
				if (g_pDXInput->KeyDown(DIK_LSHIFT) && g_pOperatorOption->bSlideScreenShot)
				{
					g_rectScreenShot.left = g_x;
					g_rectScreenShot.right = g_x;
					g_rectScreenShot.top = g_y;
					g_rectScreenShot.bottom = g_y;
				}

				//---------------------------------------------------------
				// Minimap�� Ŭ���ϸ� �������� �̵��Ѵ�.
				//---------------------------------------------------------
				/* // 2001.7.14 �ּ�ó��
				if (g_pDXInput->KeyDown(DIK_RCONTROL) && g_pUserOption->DrawMinimap)
				{
					int x = g_x - (800 - 256) * g_pZone->GetWidth() / 256;
					int y = g_y * g_pZone->GetWidth() / 128;

					if (x>=0 && x<g_pZone->GetWidth()
						&& y>=0 && y<g_pZone->GetWidth())
					{
						char str[80];

						int zoneID = (g_bZonePlayerInLarge? g_nZoneLarge : g_nZoneSmall);

						sprintf(str, "*warp %d %d %d", zoneID, x, y);

						#ifdef CONNECT_SERVER
							CGSay _CGSay;
							_CGSay.setMessage( str );	//pWansungString );
							g_pSocket->sendPacket( &_CGSay ); 
							#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
										DEBUG_ADD_FORMAT("[Send] %s", _CGSay.toString().c_str() );
							#endif	
						#endif
					}
				}

				//---------------------------------------------------------
				// Chattingâ�� extra input
				//---------------------------------------------------------
				gC_vs_ui.ChatMouseControlExtra( M_LEFTBUTTON_DOWN, g_x, g_y );
				*/
//				#endif
			break;

			case CDirectInput::RIGHTUP :
				// ���� ����� �� ����
				gC_vs_ui.EndInstallMineProgress();
				//gC_vs_ui.EndCreateMineProgress();
				//gC_vs_ui.EndCreateBombProgress();
			break;

			//case CDirectInput::RIGHTDOWN :
				//---------------------------------------------------------
				// Chattingâ�� extra input
				//---------------------------------------------------------
				// 2001.7.14 �ּ�ó��
				//gC_vs_ui.ChatMouseControlExtra( M_RIGHTBUTTON_DOWN, g_x, g_y );
			//break;


		case CDirectInput::WHEELDOWN:
			gC_vs_ui.MouseControl(M_WHEEL_DOWN, g_x, g_y);
			///gC_vs_ui.ChatMouseControlExtra( M_WHEEL_DOWN, g_x, g_y );
			break;

		case CDirectInput::WHEELUP:
			gC_vs_ui.MouseControl(M_WHEEL_UP, g_x, g_y);
//			gC_vs_ui.ChatMouseControlExtra( M_WHEEL_UP, g_x, g_y );
			break;
	}
}

//#include "PacketDef.h"
				
//-----------------------------------------------------------------------------
// DXKeyboardEvent
//-----------------------------------------------------------------------------
void	
CGameUpdate::DXKeyboardEvent(CDirectInput::E_KEYBOARD_EVENT event, DWORD key)
{
	// 2026-05-18 ImGui Hibrit Admin Menu — AGRESIF KEYBOARD GATE.
	// Aynii DXMouseEvent rationale — WantCaptureKeyboard stale state olabilir,
	// DebugConsole acikken tum keyboard event'leri Phoenix'ten kes. CTRL+SHIFT
	// +F12 toggle WindowProc'tan once islenir (WndProcHandler), keybind
	// engellenmez.
	if (PhoenixImGui::IsDebugConsoleVisible())
		return;

	if(g_pEventManager->GetEventByFlag(EVENTFLAG_DENY_INPUT_KEYBOARD))
	{
		return;
	}
	
	if (event==CDirectInput::KEYDOWN)
	{
		if(key == 0xcc) return;		// �̰� ����-_-?;;
				
		gC_vs_ui.DIKeyboardControl(event, key);
	
		switch (key) 
		{	
//			__BEGIN_HELP_EVENT
//				case DIK_LMENU :
//					// [����] alt������
//				
//					ExecuteHelpEvent( HE_PRESSED_ALT );					
//				break;
//			__END_HELP_EVENT				

// ���� �̰�-_-?;;; Ȥ�� ? �ƴѵ�-_-;
//			case DIK_F10 :
//				gC_vs_ui.HotKey_F10();
//			break;

			case DIK_SPACE :
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL) )
				{
				//	gC_vs_ui.HotKey_SummonPet(); 
//					gC_vs_ui.HotKey_WindowToggle();
				}
			break;
			case DIK_EQUALS :  
				{ 	
					//if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL) )
					//{
						//if(UI_IsRunning_WebBrowser())   
					
						//	UI_Close_WebBrowser();
						//else
						//	UI_Run_WebBrowser("www.naver.com") ; 

						//	UI_Run_WebBrowser("http://www.darkeden.com/priceInfo/payStep1.php") ; 
						//UI_Run_WebBrowser("http://gmk.darkeden.com/?userid=sjheon" ) ;  //  "http://www.darkeden.com/priceInfo/payStep1.php") ; 
						//g_pPlayer->AddComboEndValue() ; // 2005.07.06
					//}
				}
				break ; 

				case DIK_1 :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL) )
					{
						UI_BLOODBURST_ATTACKGAGEFULL()	;
					}
					break ; 
						
				case DIK_2 :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL) )
					{
						UI_BLOODBURST_DEFENSEGAGEFULL()	;
					}
					break ; 

				case DIK_3 :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL) )
					{
						UI_BLOODBURST_PARTYGAGEFULL()	;
					}
					break ; 
			//------------------------------------
			// L-CONTROL + S
			//------------------------------------
			//case DIK_S :				
			//	if (g_pDXInput->KeyDown(DIK_LCONTROL))
			//	{	
					/*
					switch (rand()%3) 
					{
						case 0 : (*g_pUIDialog).PopupMessageDlg(UIDialog::MESSAGE_CANNOT_BUY_NO_SPACE); break;
						case 1 : (*g_pUIDialog).PopupMessageDlg(UIDialog::MESSAGE_CANNOT_BUY_NO_MONEY); break;
						case 2 : (*g_pUIDialog).PopupMessageDlg(UIDialog::MESSAGE_CANNOT_BUY_NO_ITEM); break;
					}
					*/
					/*
					// �ӽ�...
					g_TempInformation.Value2 = SKILL_DOUBLE_IMPACT;

					if (g_TempInformation.Mode==TempInformation::MODE_SKILL_LEARN)
					{
						#ifdef CONNECT_SERVER
							CGLearnSkill _CGLearnSkill;

							// temp information�� ����� ���� �б�
							_CGLearnSkill.setSkillDomainType( g_TempInformation.Value1 );
							_CGLearnSkill.setSkillType( g_TempInformation.Value2 );
							
							
							#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
									DEBUG_ADD_FORMAT( "[ Send ] %s", _CGLearnSkill.toString().c_str() );
							#endif

							g_pSocket->sendPacket( &_CGLearnSkill );
						#endif
					}
					*/


					// test
					/*
					ExecuteActionInfoFromMainNode(
							MAGIC_LIGHT,
						
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							g_pPlayer->GetDirection(),
							
							g_pPlayer->GetID(),
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							
							64,
							
							NULL
					);
					*/

					//---------------------------------------------------
					//
					// PC Talk Box Test
					//
					//---------------------------------------------------
					//---------------------------------------------------
					// UI�� �˸�...
					//---------------------------------------------------
					//
					// [ TEST CODE ]
					//
					// UI���� ���õ� ���
					/*
					g_PCTalkBox.SetAnswerID( rand()%g_PCTalkBox.size() );

					//---------------------------------------------------
					// server�� �˸�
					//---------------------------------------------------
					#ifdef CONNECT_SERVER
						CGNPCAskAnswer _CGNPCAskAnswer;
						_CGNPCAskAnswer.setObjectID( g_PCTalkBox.GetNPCID() );
						_CGNPCAskAnswer.setScriptID( g_PCTalkBox.GetScriptID() );
						_CGNPCAskAnswer.setAnswerID( g_PCTalkBox.GetAnswerID() );

						#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
								DEBUG_ADD_FORMAT("[Send] %s", _CGNPCAskAnswer.toString().c_str());
						#endif

						g_pSocket->sendPacket( &_CGNPCAskAnswer );
					#endif
					*/
			//	}
			//break;

			//------------------------------------
			// FPS Toggle
			//------------------------------------
			case DIK_F : 
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					if(g_pEventManager->IsEvent(EVENTID_OUSTERS_FIN))
					{
						g_pEventManager->RemoveEvent(EVENTID_OUSTERS_FIN);
					}
//					else
//					{
//						MEvent event;
//						event.eventID = EVENTID_OUSTERS_FIN;
//						event.eventType = EVENTTYPE_ZONE;
//						event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
//						event.parameter1 = 0;
//						event.parameter4 = EVENTBACKGROUNDID_CLOUD;
//						g_pEventManager->AddEvent(event);
//					}
					(*g_pUserOption).DrawFPS = !(*g_pUserOption).DrawFPS;
//							g_abHolyLandBonusSkills[4] = !g_abHolyLandBonusSkills[4];
//							g_pSkillAvailable->SetAvailableSkills();
				}

				#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
					else if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						if (g_pPlayer->HasEffectStatus( EFFECTSTATUS_FADE_OUT ))
						{
							g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_FADE_OUT );
						}
						else
						{
							// 5�� ���� �¿� �ܻ�
							g_pPlayer->AddEffectStatus( EFFECTSTATUS_FADE_OUT, 5*1000 );
						}
					}
				#endif
			break;				

			//------------------------------------
			// Minimap Toggle
			//------------------------------------
			/*
			case DIK_M :   
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).DrawMinimap = !(*g_pUserOption).DrawMinimap;
				}
			break;

			//------------------------------------
			// DrawZoneName Toggle
			//------------------------------------
			case DIK_Z :
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).DrawZoneName = !(*g_pUserOption).DrawZoneName;
				}
			break;

			//------------------------------------
			// DrawGameTime Toggle
			//------------------------------------
			case DIK_T :
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).DrawGameTime = !(*g_pUserOption).DrawGameTime;
				}
			break;
			*/

			//------------------------------------
			// Blending Shadow
			//------------------------------------
			/*
			case DIK_V : 
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					(*g_pUserOption).BlendingShadow = !(*g_pUserOption).BlendingShadow;
					if (g_pTopView!=NULL)
					{
						g_pTopView->ClearShadowManager();
					}
				}
			break;
			*/

			//------------------------------------
			// ȿ���� on/off
			//------------------------------------
			/*
			case DIK_E : 
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					//------------------------------------
					// �������̸�.. �ߴ�..
					//------------------------------------
					if (g_pUserOption->PlaySound)
					{
						g_pUserOption->PlaySound = FALSE;

						StopSound( g_previousSoundID );
						g_previousSoundID = SOUNDID_NULL;						
					}
					else
					{
						g_pUserOption->PlaySound = TRUE;
					}
				}
				//---------------------------------------------------------
				// Watch Mode
				//---------------------------------------------------------
				else if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
				{
					SetWatchMode( !g_bWatchMode );
				}
			break;
			*/

			//------------------------------------
			// ��� ���� on/off
			//------------------------------------
			/*
			case DIK_B : 
				#ifdef OUTPUT_DEBUG
					//------------------------------------
					// ���� ���� test
					//------------------------------------
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						MActionResult* pResult = new MActionResult;

						if (g_pPlayer->GetCreatureType()!=185)
						{									
							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), 185 ) );
							//g_pPlayer->SetCreatureType( 185 );
							//g_pPlayer->SetFlyingCreature();
						}
						else
						{
							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), CREATURETYPE_VAMPIRE_MALE ) );
							//g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_MALE );
							//g_pPlayer->SetGroundCreature();
						}

						//--------------------------------------------------
						// ���� ���� test
						//--------------------------------------------------								
						ExecuteActionInfoFromMainNode(
							RESULT_MAGIC_TRANSFORM_TO_BAT,										// ��� ��� ��ȣ
						
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							g_pPlayer->GetDirection(),														// ��� ����
							
							OBJECTID_NULL,												// ��ǥ�� ���� ����
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 
							
							0,													// ����� (����) ���� �ð�		
							
							pResult, //NULL,
							
							false);			// ��� ÷���� �����Ѵ�.

						g_pPlayer->SetDelay( 1000 );
					}
					
				#endif
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					//if (g_DXMusic.IsPlay())
					//{
					//	g_DXMusic.Pause();
					//}
					//else
					//{						
					//	g_DXMusic.Resume();
					//}
					
					//------------------------------------
					// �������̸�.. �ߴ�..
					//------------------------------------
					if (g_pUserOption->PlayWaveMusic)
					{
						if (g_pUserOption->PlayMusic)
						{
							g_pUserOption->PlayMusic = FALSE;

							if (g_pDXSoundStream->IsPlay())
							{
								g_pDXSoundStream->Stop();
							}
						}
						//------------------------------------
						// �ƴϸ�.. �ٽ� play
						//------------------------------------
						else 
						{
							g_pUserOption->PlayMusic = TRUE;
										
							PlayMusicCurrentZone();
						}
					}
					else
					{
						if (g_pUserOption->PlayMusic)
						{
							g_pUserOption->PlayMusic = FALSE;

							if (g_Music.IsPlay())
							{
								g_Music.Pause();
							}
						}
						//------------------------------------
						// �ƴϸ�.. �ٽ� play
						//------------------------------------
						else 
						{
							g_pUserOption->PlayMusic = TRUE;

							if (g_Music.IsPlay())
							{
								g_Music.Resume();
							}
							else
							{													
								PlayMusicCurrentZone();
							}
						}
					}
				}
			break;
			*/

			//------------------------------------
			// DrawInterface Toggle
			//------------------------------------
			/*
			case DIK_H :
				if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					//(*g_pUserOption).DrawInterface = !(*g_pUserOption).DrawInterface;

					// ���� ����.
					//(*g_pUIDialog).PopupHelpDlg();
				}
			break;
			*/
			
				case DIK_SCROLL :
					if (g_pOperatorOption->bSlideScreenShot)
					{
						g_bSaveSlideScreenShot = !g_bSaveSlideScreenShot;
						g_bSlideRectSelect = false;
						
						if( g_bSaveSlideScreenShot  )
							gpC_mouse_pointer->SetCursor( C_VS_UI_MOUSE_POINTER::CURSOR_MAX );
						else
							gpC_mouse_pointer->SetCursorDefault();
					}
					break;

			
			#ifdef OUTPUT_DEBUG	

				/*
				case DIK_S :
					if (g_pDXInput->KeyDown(DIK_LMENU))
					{
						g_bSlideScreenShot = !g_bSlideScreenShot;
						g_bSlideRectSelect = false;
					}
				break;
				*/

				case DIK_P :
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU) )
					{
						bool en = true;
						if(g_abHolyLandBonusSkills[0] == true)
							en = false;
						else
							en = true;

						for(int i = 0; i < HOLYLAND_BONUS_MAX; i++)
						{
							g_abHolyLandBonusSkills[i] = en;
						}

						g_pSkillAvailable->SetAvailableSkills();

//						if (g_pProfiler!=NULL)
//						{
//							g_pProfiler->WriteToFile("profiler.txt", true);
//						}
					}
				break;

				#if defined(_DEBUG) || defined(OUTPUT_DEBUG)
					case DIK_F1 :
					{
						/*
						// Add Effect Test
						GCAddEffect _GCAddEffect;
						
						_GCAddEffect.setObjectID(g_pPlayer->GetID());
						_GCAddEffect.setEffectID(EFFECTSTATUS_STUN);
						_GCAddEffect.setDuration(50);
						
						DEBUG_ADD_FORMAT("[Execute] %s", _GCAddEffect.toString().c_str());
						_GCAddEffect.execute( g_pSocket );
						*/
						//gC_vs_ui.RunTimeCount(15, "Ÿ�̸� �׽�Ʈ��");


// 						for(int i = 0; i < 100; ++i)
// 						{
// 							CGUseBonusPoint _CGUseBonusPoint;
// 							_CGUseBonusPoint.setWhich(INC_STR);
// 
// 							g_pSocket->sendPacket(&_CGUseBonusPoint);
// 						}

//  						GCAddEffect _GCAddEffect;	
//  						_GCAddEffect.setObjectID(g_pPlayer->GetID());
//  						_GCAddEffect.setEffectID(EFFECTSTATUS_CURSE_OF_BLOOD);
//  						_GCAddEffect.setDuration(50);
//  
//  						_GCAddEffect.execute( g_pSocket );

						
					}
					break;

					case DIK_F3 :
					{
// 						CGGQuestAccept _CGGQuestAccept;
// 						_CGGQuestAccept.setQuestID(1012);
// 						g_pSocket->sendPacket(&_CGGQuestAccept);

					}
					break;

					//------------------------------------------------------------
					//
					// ��� ��� - packet �׽�Ʈ�� ���ؼ�
					//
					//------------------------------------------------------------
					case DIK_L : 
					{
						if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
						{
							MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();					

							for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
							{
								MCreature* pCreature = (*iCreature).second;

								if (pCreature!=g_pPlayer)
								{
									// ��� ���
									// ��� ����
									//int targetID;						
										
									//int nth;
									
									MZone::CREATURE_MAP::const_iterator iCreature2;

									// ��� ����
								
									/*if (rand()%2==0)
									{
										GCSkillToTileOK2 gcSkillToTileOK2;

										gcSkillToTileOK2.setObjectID( pCreature->GetID() );
										gcSkillToTileOK2.setSkillType( ((rand()%2)? 34 : 37) );
										gcSkillToTileOK2.setX( pCreature->GetX() );
										gcSkillToTileOK2.setY( pCreature->GetY() );
										gcSkillToTileOK2.setRange( rand()%8 );
										gcSkillToTileOK2.setDuration( 0 );
										gcSkillToTileOK2.addCListElement( g_pPlayer->GetID() );
										gcSkillToTileOK2.addShortData( (ModifyType)12, 97 );
										gcSkillToTileOK2.addLongData( (ModifyType)48, 68002 );										

//										DEBUG_ADD_FORMAT("[Execute] %s", gcSkillToTileOK2.toString().c_str());

										gcSkillToTileOK2.execute( g_pSocket );
										
										DEBUG_ADD("TileOK2OKOK");
									}*/
									/*
									else*/
// 									GCSkillToObjectOK5 gcSkillToObjectOK5;
// 									
// 									gcSkillToObjectOK5.setObjectID( pCreature->GetID() );
// 									gcSkillToObjectOK5.setTargetObjectID( g_pPlayer->GetID() );
// 									gcSkillToObjectOK5.setSkillType( SKILL_CLIENT_FREEZE);
// 									gcSkillToObjectOK5.setDuration( 50 );
// 									
// 									DEBUG_ADD_FORMAT("[Execute] %s", gcSkillToObjectOK5.toString().c_str());
// 									gcSkillToObjectOK5.execute( g_pSocket );
								

									/*if (rand()%2==0)
									{
										int nth = rand()%g_pZone->GetCreatureNumber();

										iCreature2 = g_pZone->GetCreatureBegin();
										int targetID ; 
										for (int i=0; i<nth; i++)
										{
											targetID = iCreature->second->GetID();
											iCreature2 ++;
										}

										GCSkillToObjectOK5 gcSkillToObjectOK5;

										gcSkillToObjectOK5.setObjectID( pCreature->GetID() );
										gcSkillToObjectOK5.setTargetObjectID( targetID );
										gcSkillToObjectOK5.setSkillType( rand()%60+10 );
										gcSkillToObjectOK5.setDuration( 50 );

										DEBUG_ADD_FORMAT("[Execute] %s", gcSkillToObjectOK5.toString().c_str());

										//#ifdef CONNECT_SERVER
											gcSkillToObjectOK5.execute( g_pSocket );
										//#else
										//	gcSkillToObjectOK5.execute( NULL );
										//#endif	
									}/*
									else if (rand()%2==0)
									{										
										int skillType = rand()%60+10;

										if (skillType != MAGIC_INVISIBILITY
											&& skillType != SKILL_SNIPPING)
										{
											GCSkillToSelfOK2 gcSkillToSelfOK2;

											gcSkillToSelfOK2.setObjectID( pCreature->GetID() );
											gcSkillToSelfOK2.setSkillType( skillType );
											gcSkillToSelfOK2.setDuration( 50 );

											DEBUG_ADD_FORMAT("[Execute] %s", gcSkillToSelfOK2.toString().c_str());

											#ifdef CONNECT_SERVER
												gcSkillToSelfOK2.execute( g_pSocket );
											#else
												gcSkillToSelfOK2.execute( NULL );
											#endif	
										}
									}
									else
									{
										// ��� ����
										nth = rand()%g_pZone->GetCreatureNumber();

										iCreature2 = g_pZone->GetCreatureBegin();

										for (int i=0; i<nth; i++)
										{
											targetID = iCreature->second->GetID();
											iCreature2 ++;
										}						

										GCSkillFailed2 gcSkillFailed2;
										gcSkillFailed2.setObjectID( pCreature->GetID() );
										gcSkillFailed2.setTargetObjectID( targetID );
										gcSkillFailed2.setSkillType( (rand()%2? 5:65) );

										DEBUG_ADD_FORMAT("[Execute] %s", gcSkillFailed2.toString().c_str());
										
										#ifdef CONNECT_SERVER
											gcSkillFailed2.execute( g_pSocket );
										#else
											gcSkillFailed2.execute( NULL );
										#endif
									}
									*/
								}

								iCreature++;
							}
						}
					}
					break;

					//case DIK_1 :
//						if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//						{
//							if( g_pSoundBufferForOGG == NULL )
//								g_pSoundBufferForOGG = new CDirectSoundBuffer(g_hWnd, SOUND_STEREO, SOUND_44K, SOUND_16BIT);
//
//							if( g_pOGG == NULL )
//								g_pOGG = new COGGSTREAM(g_hWnd, g_pSoundBufferForOGG, 44100, 11025, 8800,1);
//							
//							if( g_oggfile != NULL)
//								fclose(g_oggfile );
//							g_oggfile = fopen("C:\\ogggggg\\3\\blood.ogg","rb");
//							g_pOGG->streamLoad(g_oggfile, NULL);
//							g_pOGG->streamPlay(SOUND_PLAY_REPEAT);
//							
////							gC_vs_ui.SetOustersSkillDown();
////							g_pMP3->Stop();
////							g_pMP3->Open("data\\music\\blood.mp3");
////							g_pMP3->Play( true );
////							g_bTestMusic = false;
////							g_pSystemMessage->Add("Blood.mp3 �� �����մϴ�");							
//						}
//						// ALT + 1  --> ���� �ӵ� Slow
					//	if(g_pDXInput->KeyDown(DIK_LCONTROL))
					//	{ 
							
							//g_pPlayer->SetSpecialActionInfo(TEMP_SKILL_INFINITY_THUNDERBLOT);
//							if( g_pEventManager->GetEvent( EVENTID_WAR_EFFECT ) == NULL )
//							{
//								MEvent event;
//								event.eventID = EVENTID_WAR_EFFECT;
//								event.eventType = EVENTTYPE_NULL;
//								event.eventFlag = EVENTFLAG_SHAKE_SCREEN | EVENTFLAG_FADE_SCREEN;
//								event.parameter2 = RGB( -30, -30, 0 );
//								event.parameter3 = 1;
//								event.eventDelay = -1;
//								event.showTime = 2000;
//								event.totalTime = 30000;
//								g_pEventManager->AddEvent( event );
//							}
//							else
//							{
//								g_pEventManager->RemoveEvent( EVENTID_WAR_EFFECT );
//							}
//							ExecuteActionInfoFromMainNode(SKILL_DESTRUCTION_SPEAR_MASTERY,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0, 100, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SUMMON_GDR,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							MEvent event;
//
//							event.eventID = EVENTID_GDR_PRESENT;
//							event.eventType = EVENTTYPE_ZONE;
//							event.eventFlag = EVENTFLAG_SHAKE_SCREEN;
//							event.parameter3 = 2;
//							event.eventDelay =5000;
//							g_pEventManager->AddEvent(event);
//							//SetFadeStart(31, 0, 1, 0,0,0, 6);
							//gC_vs_ui.RunNameing();
							//g_pPlayer->SetAction(ACTION_DRAINED);
//							g_pPlayer->m_ActionCount = 0;
							//g_pPlayer->SetCauseCriticalWounds(100);
							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_GLACIER, 100);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_BLINDNESS,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 30, NULL, false);
						//	g_pPlayer->AddEffectStatus(EFFECTSTATUS_GRAND_MASTER_SLAYER_130, 0xffff);
							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_BLINDNESS, 10);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_CAN_ENTER_GDR_LAIR, 10);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_ICE_STICK_SE, 10);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_GDR_GLACIER_SLAYER, 100);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_GRAND_MASTER_OUSTERS_130, 0xffff);
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_GRAND_MASTER_OUSTERS_150, 0xffff);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SLAYER_MASTER_130,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SLAYER_MASTER_150,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
////
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_VAMPIRE_MASTER_130,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_VAMPIRE_MASTER_150,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_OUSTERS_MASTER_130,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_VIOLENT_PHANTOM,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 20, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_SUMMON_GORE_GRAND_GROUND,g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0, 50, NULL, false);
//							ExecuteActionInfoFromMainNode(RESULT_CLIENT_TEST_SWORD_OF_THOR,g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0, 100, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_BURNING_SOL_1,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0, 100, NULL, false);
//							g_pPlayer->SetAction(ACTION_SLAYER_BLADE);
//							ExecuteActionInfoFromMainNode(SKILL_PLEASURE_EXPLOSION,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 0, NULL, false);			
							
//							if(g_pEventManager->IsEvent(EVENTID_BG_CLOUD))
//							{
//								g_pEventManager->RemoveEvent(EVENTID_BG_CLOUD);
//							}
//							else
//							{
//								MEvent event;
//								event.eventID = EVENTID_BG_CLOUD;
//								event.eventType = EVENTTYPE_ZONE;
//								event.eventFlag = EVENTFLAG_CLOUD_BACKGROUND;
//								event.parameter3 = 2;
//								event.eventDelay =60000; // 5 ��
//								g_pEventManager->AddEvent(event);
//							}
							
//							ExecuteActionInfoFromMainNode(SKILL_PLEASURE_EXPLOSION,g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+3, 0, 100, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_LAND_MINE_LIGHT,g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0, 100, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_PLEASUER_STATION_LIGHT,g_pPlayer->GetX()+3, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX()+3, g_pPlayer->GetY(), 0, 100, NULL, false);

							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1, 1000);
//
//							MActionResult* pResult = new MActionResult;
////
//							if (g_pPlayer->GetCreatureType()!=742)
//							{									
//								g_pPlayer->SetCreatureType( 742 );
//
//								g_pPlayer->SetGroundCreature();
//					
//								g_pPlayer->SetDirection( 2 );
//								g_pPlayer->SetCurrentDirection( 2 );
//								g_pPlayer->SetAction( ACTION_STAND );			
//
//								//--------------------------------------------------
//								// ����� �����ϴ� ���
//								//--------------------------------------------------
//								MActionResult* pResult = new MActionResult;
//
//								pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), 742 ) );
//
//								//--------------------------------------------------
//								// ���� ���� 
//								//--------------------------------------------------								
//								ExecuteActionInfoFromMainNode(
//									RESULT_SKILL_INSTALL_TURRET,										// ��� ��� ��ȣ
//								
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
//									g_pPlayer->GetDirection(),														// ��� ����
//									
//									OBJECTID_NULL,												// ��ǥ�� ���� ����
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 
//									
//									0,													// ����� (����) ���� �ð�		
//									
//									NULL, //NULL,
//									
//									false);			// ��� ÷���� �����Ѵ�.
//
//							}
//							else
//							{
//								pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), CREATURETYPE_SLAYER_FEMALE ) );
//								//g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_MALE );
//								//g_pPlayer->SetGroundCreature();
//							}
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TEST_BURNING_SOL_4,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 20, pResult, false);	
//
//							g_pPlayer->SetDelay( 1000 );

							
//							UI_RunPetStorage();SKILL_CLIENT_TEST_SUMMON_GORE_GRAND_GROUND
//							UI_SetPetStorage(g_pStorage);
							// start �� ��ȯ ����Ű ����
//							for(int i = 0; i< 10; i++)
//								for(int j = 0; j < 6; j++)
//								{
//									const MItem * p_item = g_pInventory->GetItem(i, j);
//				
//									if (p_item) // Item�� �ִ�.
//									{
//										if(p_item->GetItemClass() == ITEM_CLASS_PET_ITEM)
//										{
//											gpC_base->SendMessage(UI_ITEM_USE, p_item->GetID(), 0, (MItem *)p_item);
//											return;
//										}
//									}
//								}
							// end �� ��ȯ ����Ű ����

							// start npc ��ȭâ ����
//							g_pPCTalkBox->Release();
//							//---------------------------------------------------
//							// normal
//							//---------------------------------------------------
//							g_pPCTalkBox->SetType( PCTalkBox::SELECT_QUEST );
//						
//							int scriptID = 101;
//							
//							//---------------------------------------------------
//							// PC Talk Box�� ���� ����
//							//---------------------------------------------------
//							// SetContent��� �̸��� �Ǿ�������.. Subject�̴�. - -;
//							g_pPCTalkBox->SetContent( "������" );
//							g_pPCTalkBox->SetNPCID( 297 );
//							g_pPCTalkBox->SetCreatureType( 297 );
//							g_pPCTalkBox->SetScriptID( scriptID );
//							
//							//---------------------------------------------------
//							// �� string �߰�
//							//---------------------------------------------------
//							int contentSize = 2;//g_pNPCScriptTable->GetContentSize( scriptID );
//							
//							//for (int i=0; i<contentSize; i++)
//							{
//								// g_PCTalkBox�� �߰�
//								g_pPCTalkBox->AddString( "������" );
//								g_pPCTalkBox->AddString( "ũ����" );
//							}
//							
//							g_pUIDialog->PopupPCTalkDlg();
							// start npc ��ȭâ ����

//							gC_vs_ui.AddHelpMail(6); 
//							gC_vs_ui.AddHelpMail(0); 
//							gC_vs_ui.AddHelpMail(1); 
//							gC_vs_ui.AddHelpMail(2); 
//							gC_vs_ui.AddHelpMail(3); 
//							gC_vs_ui.AddHelpMail(4); 
//							gC_vs_ui.AddHelpMail(5); 
						
//							gC_vs_ui.RunSMSMessage();
//							extern void Add_GDR_Effect(int nEffect, bool bAppearBossMonster);
//							Add_GDR_Effect(1, false);
//							Add_GDR_Effect(2, false);
//
//							ExecuteActionInfoFromMainNode(TEMP_SKILL_SPIT_STREAM,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 0xffff, NULL, false);			
							//g_pPlayer->SetAction(ACTION_OUSTERS_CHAKRAM_SLOW);
	// create ghost
							//{

//								extern void SetDragonTorando(int Type, DWORD ObjectID, int TileX, int TileY);
//								SetDragonTorando(EFFECTSTATUS_DRAGON_TORNADO, 11000, g_pPlayer->GetX()+4, g_pPlayer->GetY());

							//	UI_PopupMessage( UI_STRING_MESSAGE_CAMPAIGN_HELP_THANKS );
//
//								ExecuteActionInfoFromMainNode(SKILL_CLIENT_BIKE_CRASH, g_pPlayer->GetX(), g_pPlayer->GetY(), 0, g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);	
//								MFakeCreature *pFakeCreature = NewFakeCreature(CREATURETYPE_ROCKET_LUNCHER, g_pPlayer->GetX()+5, g_pPlayer->GetY(), rand()%8);
//
//								if (!g_pZone->AddFakeCreature( pFakeCreature ))
//								{
//									delete pFakeCreature;
//								}
//								else
//								{
//									pFakeCreature->SetZone(g_pZone);
//									pFakeCreature->SetFakeCreatureType(MFakeCreature::FAKE_CREATURE_ROCKET);
//									pFakeCreature->SetMoveType(MCreature::CREATURE_FAKE_FLYING);
//									pFakeCreature->AddEffectStatus( (EFFECTSTATUS)(EFFECTSTATUS_ROCKET_LUNCHER), 0xFFFF );
//									pFakeCreature->SetTraceID(g_pPlayer->GetID());
//									pFakeCreature->SetTime(timeGetTime()+3000); 
//								//	pFakeCreature->SetAction( ACTION_MOVE );
//								}
					//		}
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_DROP_SIGN,g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY(), 0, 10, NULL, false);			
//							
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER_SIGN,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10, NULL, false);			
//							
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_AUGER,g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_AUGER_SIGN,g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-5, 0, 10, NULL, false);	
//							
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SW,g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0, 0, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SW_SIGN,g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0, 0, NULL, false);			
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SE,g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SE_SIGN,g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY(), 0, 10, NULL, false);	
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NE,g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NE_SIGN,g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()-5, g_pPlayer->GetY()-5, 0, 10, NULL, false);
//
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NW,g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0, 5000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_NW_SIGN,g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0, 10, NULL, false);
//							
//								for(int i = 0; i< 100 ; i++)
//								{
//									int MapX = rand()% g_pZone->GetWidth();
//									int MapY = rand()% g_pZone->GetHeight();
//									//MFakeCreature *pFakeCreature = NewFakeCreature(716, g_pPlayer->GetX(), g_pPlayer->GetY(), 2);
//									MFakeCreature *pFakeCreature = NewFakeCreature(CREATURETYPE_GHOST, MapX, MapY, rand()%8);
//
//									if (!g_pZone->AddFakeCreature( pFakeCreature ))
//									{
//										delete pFakeCreature;
//									}
//									pFakeCreature->SetZone(g_pZone);
//									pFakeCreature->SetFakeCreatureType(MFakeCreature::FAKE_CREATURE_GHOST);
//									pFakeCreature->SetMoveType(MCreature::CREATURE_FAKE_UNDERGROUND);
//									pFakeCreature->AddEffectStatus( (EFFECTSTATUS)(EFFECTSTATUS_GHOST_1+rand()%2), 0xFFFF );
//								}

								
//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_FIRE_ELEMENTAL))
//								g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL );
//							else
//								g_pPlayer->AddEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL, 0xFFFF );
//							g_pPlayer->SetDelay(2000);
//							TYPE_OBJECTID petID = g_pPlayer->GetPetID();
//							if(petID != OBJECTID_NULL)
//							{
//								MFakeCreature *pCreature = (MFakeCreature *)g_pZone->GetFakeCreature(petID);
//								pCreature->SetAction( ACTION_ATTACK );
//								pCreature->SyncTurretDirection();
//
//
//								ExecuteActionInfoFromMainNode(
//									SKILL_CLIENT_TANK_ATTACK_3,										// ��� ��� ��ȣ
//									
//									0, 0, 0,
//									pCreature->GetDirection(), // ��� ����
//									
//									petID,												// ��ǥ�� ���� ����
//									0, 0, 0, 
//									
//									0,													// ����� (����) ���� �ð�		
//									
//									NULL,
//									
//									false);	
//								pCreature->AddEffectStatus(EFFECTSTATUS_COMA, 0xFFFF);
//								pCreature->SetDead();
//							SetFlagTo( true );
//						}
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_HAS_PET, 0);
					//	}
					//	if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					//	{
							//g_pPlayer->SetWeaponSpeed( MCreature::WEAPON_SPEED_SLOW );

							// 1 ~ 24
							/*
							if (++gtx_op > 24)
							{
								gtx_op = 1;
							}
							*/
						//	#ifdef __METROTECH_TEST__
						////		g_iAutoHealPotion = (g_iAutoHealPotion+1)%2;
					//		#endif
					//	}
				//	break;

					//case DIK_2 :
					//	if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					//	{  
//							extern void SetDragonTorando(int Type, DWORD ObjectID, int TileX, int TileY);
//							SetDragonTorando(EFFECTSTATUS_DRAGON_TORNADO, 11000, g_pPlayer->GetX()+3, g_pPlayer->GetY());
////
//							UI_RunModifyTax();  
//							if(g_pEventManager->IsEvent(EVENTID_ADVANCEMENT_QUEST_ENDING))
//							{
//								g_pEventManager->RemoveEvent(EVENTID_ADVANCEMENT_QUEST_ENDING);
//							}
//							else
//							{
//								MEvent event;
//								event.eventID = EVENTID_ADVANCEMENT_QUEST_ENDING;
//								event.eventType = EVENTTYPE_ZONE;
//								event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
//								event.parameter1 = 0;
//								event.parameter4 = 3;
//								g_pEventManager->AddEvent(event);
//
////								MEvent event;
////								event.eventID = EVENTID_WAR_EFFECT;
////								event.eventType = EVENTTYPE_NULL;
////								event.eventFlag = EVENTFLAG_SHAKE_SCREEN | EVENTFLAG_FADE_SCREEN;
////								event.parameter2 = RGB( -30, -30, 0 );
////								event.parameter3 = 1;
////								event.eventDelay = -1;
////								event.showTime = 2000;
////								event.totalTime = 30000;
////								g_pEventManager->AddEvent( event );
//							}
//
							//if(UI_IsRunning_WebBrowser())
							//	UI_Close_WebBrowser(); 
							//else
							//	UI_Run_WebBrowser("www.darkeden.com") ;
								//UI_Run_WebBrowser("http://www.darkeden.com/priceInfo/payStep1.php");
								//UI_Run_WebBrowser(/*"http://market.darkeden.com/"*/"www.naver.com");

//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE))
//								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE); 
//							else
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE, 0xffff);
							//g_pPlayer->SetCreatureType(770); 
//							if(g_pPlayer->GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR)
//								g_pPlayer->SetCreatureType(358);
//							else
//								g_pPlayer->SetCreatureType(CREATURETYPE_SLAYER_OPERATOR);
//							
						//	g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_GLACIER);
						//	g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BLESS_FIRE);

//							ExecuteActionInfoFromMainNode(TEMP_SKILL_NOOSE_OF_WRAITH,g_SelectSector.x, g_SelectSector.y, 0,g_pPlayer->GetDirection(), g_pPlayer->GetID()	,	
//								g_SelectSector.x, g_SelectSector.y, 0, 0xffff, NULL, false);
//							g_pPlayer->SetAction(ACTION_DRAINED);
//							ExecuteActionInfoFromMainNode(RESULT_MAGIC_CAUSE_CRITICAL_WOUNDS,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 0xffff, NULL, false);

//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2, 32);
//							else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3, 32);
//							else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4, 32);
//							else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4))
//							{
//								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4);
//								g_pPlayer->StopBurningSol();
//								ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_4,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX()-4, g_pPlayer->GetY(), 0, 100, NULL, false);	
//								ExecuteActionInfoFromMainNode(RESULT_CLIENT_BURNING_SOL_4,g_pPlayer->GetX()-4, g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//									g_pPlayer->GetX()-4, g_pPlayer->GetY(), 0, 100, NULL, false);	
//							}
//							else 
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1, 32);
//
//							ExecuteActionInfoFromMainNode(RESULT_SKILL_SWORD_OF_THOR,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 20, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWORD_OF_THOR,g_pPlayer->GetX()-4, g_pPlayer->GetY()-4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()-4, g_pPlayer->GetY()-4, 0, 50, NULL, false);
//
//							ExecuteActionInfoFromMainNode(RESULT_SKILL_WHITSUNTIDE,g_pPlayer->GetX()+4, g_pPlayer->GetY()+4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+4, g_pPlayer->GetY()+4, 0, 50, NULL, false);
//
//							ExecuteActionInfoFromMainNode(SKILL_SWEEP_VICE_1,g_pPlayer->GetX(), g_pPlayer->GetY()+4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()+4, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_SWEEP_VICE_5,g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0, 50, NULL, false);
//							g_pPlayer->SetAction(ACTION_MAGIC);

							
	//						EVENTFLAG_NOT_FADE_SCREEN
						//	g_pPlayer->AddEffectStatus(EFFECTSTATUS_ICICLE_LARGE_AUGER, 100);
						//	ExecuteActionInfoFromMainNode(SKILL_WIDE_ICE_FIELD,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	0,	
						//				g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 100, NULL, false);	
							
						//	ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER,g_pPlayer->GetX()-5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
						//		g_pPlayer->GetX()-5, g_pPlayer->GetY()+5, 0, 5, NULL, false);			
							//ExecuteActionInfoFromMainNode(SKILL_ICE_HAIL,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,0,	0,	
							//	g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10000, NULL, false);			
						//	ExecuteActionInfoFromMainNode(SKILL_ICE_HAIL,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,0,	0,	
						//		g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10000, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_ICE_WAVE,g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
//								g_pPlayer->GetX()+5, g_pPlayer->GetY()+5, 0, 100, NULL, false);			
							//ExecuteActionInfoFromMainNode(SKILL_CLIENT_TRAP_ICE_STICK_SW,g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0,g_pPlayer->GetDirection(),	0,	
							//	g_pPlayer->GetX(), g_pPlayer->GetY()+5, 0, 0, NULL, false);			
						//	UI_RunNotice(7, 100000000);
						//	extern void Add_GDR_Effect(int nEffect, bool bAppearBossMonster);
//						//	Add_GDR_Effect(1,true);	
						//	Add_GDR_Effect(2,true);	
							
//							SetFlagTo( false );
//							TYPE_OBJECTID petID = g_pPlayer->GetPetID();
//							if(petID != OBJECTID_NULL)
//							{
//								MCreature *pCreature = g_pZone->GetFakeCreature(petID);
//								
//								g_pPlayer->SetAction( ACTION_DAMAGED );
//								ExecuteActionInfoFromMainNode(
//									SKILL_CLIENT_TANK_ATTACKED,										// ��� ��� ��ȣ
//									
//									0, 0, 0,
//									DIRECTION_DOWN, // ��� ����
//									
//									g_pPlayer->GetID(),												// ��ǥ�� ���� ����
//									0, 0, 0, 
//									
//									0,													// ����� (����) ���� �ð�		
//									
//									NULL,
//									
//									false);	
//							}
//							g_pPlayer->AddEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL_ATTACK, 21 );
//							MEvent event;
//							event.eventID = EVENTID_OUSTERS_FIN;
//							event.eventType = EVENTTYPE_ZONE;
//							event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | 
//								EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | 
//								EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | 
//								EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
//							event.parameter1 = 0;
//							event.parameter4 = EVENTBACKGROUNDID_QUEST_2;
//							g_pEventManager->AddEvent(event);		
						//}
						// ALT + 2  --> ���� �ӵ� normal
					//	if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					//	{
							//g_pPlayer->SetWeaponSpeed( MCreature::WEAPON_SPEED_NORMAL);

							// 1 ~ 13
							/*
							if (++gtx_src > 13)
							{
								gtx_src = 1;
							}
							*/
					//		#ifdef __METROTECH_TEST__
					//			g_bAutoManaPotion = !g_bAutoManaPotion;
					//		#endif
					//	}
					//break;

					//case DIK_3 :
					//	if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					//	{
//							g_pZone->RemoveTileEffect( g_pPlayer->GetX()+3, g_pPlayer->GetY(), EFFECTSTATUS_DRAGON_TORNADO, 11000 );

					//		#ifndef __GAME_CLIENT__
					//			MItem* pItem = MItem::NewItem( ITEM_CLASS_SUB_INVENTORY );
					//			pItem->SetID( rand()%10000 );
					//			pItem->SetItemType( 0 );
					//			gC_vs_ui.RunSubInventory( pItem );
					//		#endif	
							//g_pPlayer->SetAction(ACTION_ADVANCEMENT_SLAYER_MAGIC_ATTACK);
							//gC_vs_ui.RunPopupMessage("ComeBackEventNotice.txt", C_VS_UI_POPUP_MESSAGE::POPUP_WELCOME );
//							UI_PopupMessage(UI_STRING_MESSAGE_GET_EVENT_ITEM_NOTICE);
							//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_MESSAGE_GET_EVENT_ITEM_NOTICE].GetString() );
							//g_pPlayer->SetInvisibleSoon();
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_QUEST_MAGIC_ZONE,16, 16, 0,0,	0,	
//								16, 16, 0, 0xffff, NULL, false);			
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_QUEST_MAGIC_ZONE_POTAL,16, 16, 0,0,	0,	
//								16, 16, 0, 0xffff, NULL, false);			
//							if(gC_vs_ui.IsRunningPowerjjang())
//							{
//								gC_vs_ui.PowerjjangGambleResult(rand()%7);
//							}
//							else
//							gC_vs_ui.RunPowerjjang();
//							if(!g_pPlayer->HasEffectStatus(EFFECTSTATUS_VIEW_HP))
//								g_pPlayer->AddEffectStatus(EFFECTSTATUS_VIEW_HP,0xffffffff);
//							else
//							{
////								ExecuteActionInfoFromMainNode(SKILL_TURRET_FIRE,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,2,	g_pPlayer->GetID(),	
////									g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 200, NULL, true);	
//
//								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_VIEW_HP);
//							}
//							ExecuteActionInfoFromMainNode(SKILL_VIOLENT_PHANTOM,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,DIRECTION_DOWN,	g_pPlayer->GetID(),	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 200, NULL, true);	
//							g_pPlayer->SetAction(ACTION_DAMAGED);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX(), g_pPlayer->GetY()-4, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+1, g_pPlayer->GetY()-3, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX()+2, g_pPlayer->GetY()-2, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+2, g_pPlayer->GetY()-2, 0, 1000, NULL, false);
//							ExecuteActionInfoFromMainNode(SKILL_CLIENT_TURRET_LASER_ATTACK,g_pPlayer->GetX()+3, g_pPlayer->GetY()-1, 0,g_pPlayer->GetDirection(),0	,	
//								g_pPlayer->GetX()+3, g_pPlayer->GetY()-1, 0, 1000, NULL, false);
//��      ��   �� ��
//��      ��   �� ��
//������   �� �� 
//��      ��   �� ��
//������   �� ��
//             �� ��
//������   �� ��
//    ��   ���� ��
//    ��       �� ��
//����������
//        ��      ��
//������      ��
//��              ��
//������      ��



							//g_pPlayer->AddEffectStatus( EFFECTSTATUS_SIEGE_DEFENDERL, 100 );
//							ExecuteActionInfoFromMainNode(SKILL_WIDE_ICE_HAIL,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,0,	0,	
//								g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 10000, NULL, false);	
//							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_BLINDNESS);
//							g_pZone->RemoveTileEffect(137, 80,EFFECTSTATUS_ELECTRIC_POST);
//							g_pZone->RemoveTileEffect(143, 86,EFFECTSTATUS_ELECTRIC_POST);
//							g_pZone->RemoveTileEffect(136, 93,EFFECTSTATUS_ELECTRIC_POST);
//							g_pZone->RemoveTileEffect(130, 87,EFFECTSTATUS_ELECTRIC_POST);

//							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_WATER_ELEMENTAL))
//								g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL );
//							else
//								g_pPlayer->AddEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL, 0xFFFF );

//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\dominator of darkness.mp3");
//							g_pMP3->Play( true );
//							g_pSystemMessage->Add("dominator of darkness.mp3 �� �����մϴ�");
//							g_bTestMusic = false;
						//}
						// ALT + 3  --> ���� �ӵ� Fast
						//if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
						//{
							//g_pPlayer->SetWeaponSpeed( MCreature::WEAPON_SPEED_FAST );

							// 1 ~ 13
							/*
							if (++gtx_dest > 13)
							{
								gtx_dest = 1;
							}
							*/
						//	#ifdef __METROTECH_TEST__
						//		g_iAutoReload = (g_iAutoReload+1)%5;
						//	#endif
						//}
					//break;

					case DIK_4 :
						if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
						{
							int TempCount = ShowCursor(FALSE);
//							g_pUserInformation->IsNonPK = !g_pUserInformation->IsNonPK;
//							if(g_pUserInformation->IsNonPK)
//								g_pSystemMessage->Add("PK Not Available");
//							else
//								g_pSystemMessage->Add("PK Available");
							//g_pPlayer->AddEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL_HEAL, 21 );
							//g_pPlayer->AddEffectStatus(EFFECTSTATUS_BLINDNESS, 300);
//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\oblivion.mp3");
//							g_pMP3->Play( true );
//							g_pSystemMessage->Add("oblivion.mp3 �� �����մϴ�");
//							g_bTestMusic = false;
						}
#ifdef __METROTECH_TEST__
						if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
							g_bLight = !g_bLight;
//							g_bHolyWater = !g_bHolyWater;
#endif
						break;
					case DIK_5 :
#ifdef __METROTECH_TEST__
						if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
						UI_RunHorn(g_pZone->GetID());
#endif
//						if (g_pDXInput->KeyDown(DIK_RCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//						{
//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\rest.mp3");
//							g_pMP3->Play( true );
//							g_pSystemMessage->Add("rest.mp3 �� �����մϴ�");
//							g_bTestMusic = false;
//						}
						
						//						{
						//							g_pClientConfig->QuestItemColorSet += 15;
						//							
						//							if( g_pClientConfig->QuestItemColorSet >= 495 )
						//								g_pClientConfig->QuestItemColorSet = 0;
						//						} else
						//						{							
						//							g_pClientConfig->QuestItemColorSet += 15;
						//							
						//							if( g_pClientConfig->QuestItemColorSet >= 495 )
						//								g_pClientConfig->QuestItemColorSet = 0;
						//						}
						
						break;

					case DIK_6 :
//						{
//							if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//							{
//								g_pMP3->Stop();
//								g_pMP3->Open("data\\music\\ruin.mp3");
//								g_pMP3->Play( true );
//								g_pSystemMessage->Add("ruin.mp3 �� �����մϴ�");
//								g_bTestMusic = false;
//								
//								(*g_pUserOption).BlendingShadow = !(*g_pUserOption).BlendingShadow;
//								if (g_pTopView!=NULL)
//								{
//									g_pTopView->ClearShadowManager();
//								}
//							}				
//						}
#ifdef __METROTECH_TEST__
						if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
						UI_RunMinigame(0);
#endif
						break;
						
					case DIK_7 :
//						{
//							if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//							{
//								g_pMP3->Stop();
//								g_pMP3->Open("data\\music\\silence of battlefield.mp3");
//								g_pMP3->Play( true );
//								g_pSystemMessage->Add("silence of battlefield.mp3 �� �����մϴ�");
//								g_bTestMusic = false;
//							}				
//						}
						break;

					case DIK_8 :
#ifdef __METROTECH_TEST__
						if (g_pDXInput->KeyDown(DIK_RCONTROL) && g_pDXInput->KeyDown(DIK_RSHIFT))// &&
							//							g_pDXInput->KeyDown(DIK_LCONTROL) && g_pDXInput->KeyDown(DIK_RCONTROL))
						{
							g_iSpeed *= -1;
						}
#endif
//						if (g_pDXInput->KeyDown(DIK_RCONTROL) || g_pDXInput->KeyDown(DIK_LCONTROL))// &&
//							//							g_pDXInput->KeyDown(DIK_LCONTROL) && g_pDXInput->KeyDown(DIK_RCONTROL))
//						{
////							g_iSpeed *= -1;
//							g_pMP3->Stop();
//							g_pMP3->Open("data\\music\\underworld.mp3");
//							g_pMP3->Play( true );
//							g_pSystemMessage->Add("underworld.mp3 �� �����մϴ�");
//							g_bTestMusic = false;
//						}
						break;
						
					case DIK_9 :
//						if( g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//						{
//							g_bTestMusic = !g_bTestMusic;
//
//							if(g_bTestMusic)
//							{
//								if (g_CurrentMusicNum >= 8 ) g_CurrentMusicNum = 0;
//								g_pMP3->Stop();
//								g_pMP3->Open(g_musicfilename[g_CurrentMusicNum].c_str());
//								g_pMP3->Play( false );
//							} else
//							{
//								g_pMP3->Stop();								
//							}
//						}

#ifdef __METROTECH_TEST__
						if (g_pDXInput->KeyDown(DIK_RCONTROL) && g_pDXInput->KeyDown(DIK_RSHIFT))// &&
//							g_pDXInput->KeyDown(DIK_LCONTROL) && g_pDXInput->KeyDown(DIK_RCONTROL))
						{
							int mi = 1;
							if(g_iSpeed < 0)
							{
								mi = -1;
								g_iSpeed *= -1;
							}
							g_iSpeed = (g_iSpeed%4)+1;
							g_iSpeed *= mi;
						}
#endif
						break;
					case DIK_0 :
						{
							if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
							{
								if( g_bTestMusic )
								{
									g_pMP3->Stop();									
								}
							}				
						}

					case DIK_Z :
						if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
						{
							CDirectDraw::RestoreAllSurfaces();
							CDirect3D::Restore();					
								
							if (g_pTopView!=NULL)
							{
								g_pTopView->RestoreSurface();
							}
						}
					break;
					
					case DIK_N :
						{
							if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
							{
								g_pUserInformation->IsNetmarble = !g_pUserInformation->IsNetmarble;
								if(g_pUserInformation->IsNetmarble)
									g_pSystemMessage->Add("Netmarble Set");
								else
									g_pSystemMessage->Add("Netmarble Unset");
							}
							else
								if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
								{
									g_pPCTalkBox->Release();
									
									//---------------------------------------------------
									// normal
									//---------------------------------------------------
									g_pPCTalkBox->SetType( PCTalkBox::NORMAL );
									
									
									int scriptID = 101;
									
									//---------------------------------------------------
									// PC Talk Box�� ���� ����
									//---------------------------------------------------
									// SetContent��� �̸��� �Ǿ�������.. Subject�̴�. - -;
									g_pPCTalkBox->SetContent( "������" );
									g_pPCTalkBox->SetNPCID( 297 );
									g_pPCTalkBox->SetCreatureType( 297 );
									g_pPCTalkBox->SetScriptID( scriptID );
									
									//---------------------------------------------------
									// �� string �߰�
									//---------------------------------------------------
									int contentSize = 2;//g_pNPCScriptTable->GetContentSize( scriptID );
									
									//for (int i=0; i<contentSize; i++)
									{
										// g_PCTalkBox�� �߰�
										g_pPCTalkBox->AddString( "������" );
										g_pPCTalkBox->AddString( "ũ����" );
									}
									
									g_pUIDialog->PopupPCTalkDlg();
								}
						}
						break;
						
					case DIK_M :
						if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
						{
							if(g_pEventManager->IsEvent(EVENTID_METEOR))
							{
								g_pEventManager->RemoveEvent(EVENTID_METEOR);
							}
							else
							{
								MEvent event;
								event.eventID = EVENTID_METEOR;
								event.eventType = EVENTTYPE_ZONE;
								event.eventFlag = EVENTFLAG_FADE_SCREEN;
								event.parameter2 = 30 << 16;
//								event.parameter4 = EVENTBACKGROUNDID_COSMOS;
								g_pEventManager->AddEvent(event);
							}
						}
						break;

					case DIK_G:
						if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
						{
//							MFakeCreature* pFakeCreature = NewFakeCreature( g_pPlayer, g_pPlayer->GetX(), g_pPlayer->GetY() );
//							
//							pFakeCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_FAST_MOVE_ACTION );
//							
//							pFakeCreature->SetFakeCreatureFastMoveAction(g_pPlayer->GetX()+10, g_pPlayer->GetY()+10, 0, 0);
//							
//							//------------------------------------------------------
//							// Fake Creature�� Zone�� �߰�
//							//------------------------------------------------------
//							if (!g_pZone->AddFakeCreature( pFakeCreature ))
//							{
//								delete pFakeCreature;
//							}
						}
						break;

					case DIK_H :
					{
						if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
						{
							if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION))
							{
								g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_HALLUCINATION);
							}
							else
							{
								g_pPlayer->AddEffectStatus(EFFECTSTATUS_HALLUCINATION, 16*15);
							}
						}
					}
					break;
				#endif
				
			#endif


			#if defined(OUTPUT_DEBUG) && defined(_DEBUG)	//!defined(CONNECT_SERVER)
				/*
				case DIK_L :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						if (g_pPlayer->IsVampire())
						{
							#ifdef OUTPUT_DEBUG
								#ifdef CONNECT_SERVER
									int value = g_pPlayer->GetBonusPoint();

									if (value > 0)
									{
										int strValue = value/4;
										int intValue = value - strValue;

										// NPC ó�� packet�� ������.
										CGUseBonusPoint _CGUseBonusPoint;

										// 1/4�� STR�� �ø���.
										_CGUseBonusPoint.setWhich( INC_STR );

										for (int i=0; i<strValue; i++)
										{
											g_pSocket->sendPacket( &_CGUseBonusPoint );

											#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
													DEBUG_ADD_FORMAT( "[Send] %s", _CGUseBonusPoint.toString().c_str() );							
											#endif
										}
										
										// 3/4�� INT�� �ø���.
										_CGUseBonusPoint.setWhich( INC_INT );

										for (int i=0; i<strValue; i++)
										{
											g_pSocket->sendPacket( &_CGUseBonusPoint );

											#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
													DEBUG_ADD_FORMAT( "[Send] %s", _CGUseBonusPoint.toString().c_str() );							
											#endif
										}	
									}

									
								#endif			
							#endif
						}
					}
				break;
				*/				

				//------------------------------------------------------------
				//
				// ���� ���� ����
				//
				//------------------------------------------------------------				
				case DIK_SUBTRACT :
				{
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						int volume = g_Music.GetVolume();

						DEBUG_ADD_FORMAT("[MIDI] Volume = %x", volume);

						volume -= 0x1000;

						if (volume < 0)
						{
							volume = 0;
						}

						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
						
					}
					
				}
				break;
				
				case DIK_ADD :
				{
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						int volume = g_Music.GetVolume();

						DEBUG_ADD_FORMAT("[MIDI] Volume = %x", volume);

						volume += 0x1000;

						if (volume > 0xFFFF)
						{
							volume = 0xFFFF;  
						}

						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
						
					}
					

					
				}
				break;
				
				//------------------------------------------------------------
				//
				// wide chatting
				//
				//------------------------------------------------------------
				/*
				case DIK_E : 
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						if (g_HISTORY_LINE > 4)//HISTORY_LINE) ����
						{
							g_HISTORY_LINE = 4;
						}
						else
						{
							g_HISTORY_LINE = 16;
						}
					}
					//---------------------------------------------------------
					// Wolf ���� test
					//---------------------------------------------------------
					else if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						MActionResult* pResult = new MActionResult;

						if (g_pPlayer->GetCreatureType()!=186)
						{									
							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), 186 ) );
							//g_pPlayer->SetCreatureType( 185 );
							//g_pPlayer->SetFlyingCreature();
						}
						else
						{
							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), CREATURETYPE_VAMPIRE_MALE ) );
							//g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_MALE );
							//g_pPlayer->SetGroundCreature();
						}

						//--------------------------------------------------
						// ���� ���� test
						//--------------------------------------------------								
						ExecuteActionInfoFromMainNode(
							RESULT_MAGIC_TRANSFORM_TO_WOLF,										// ��� ��� ��ȣ
						
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							g_pPlayer->GetDirection(),														// ��� ����
							
							OBJECTID_NULL,												// ��ǥ�� ���� ����
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 
							
							0,													// ����� (����) ���� �ð�		
							
							pResult, //NULL,
							
							false);			// ��� ÷���� �����Ѵ�.

						g_pPlayer->SetDelay( 1000 );
					}

				break;
				*/
				//------------------------------------------------------------
				// ���
				//------------------------------------------------------------
				/*
				case DIK_H :
				{
					// helicopter �׽�Ʈ
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						TYPE_OBJECTID creatureID = g_pPlayer->GetID();

						if (g_pZone->GetHelicopter( creatureID )==NULL)
						{
							g_pZone->AddHelicopter( creatureID, 0, 0 );
						}
						else
						{
							g_pZone->RemoveHelicopter( creatureID );
						}
					}
				}
				break;
				*/

				//------------------------------------------------------------
				// Warp
				//------------------------------------------------------------
				case DIK_W :
				{
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						int x, y;

						while (1)
						{
							x = rand()%g_pZone->GetWidth();
							y = rand()%g_pZone->GetHeight();

							if (!g_pZone->GetSector(x,y).IsBlockGround())
							{
								break;
							}
						}

						g_pPlayer->SetStop();						
						g_pPlayer->MovePosition( x, y );
					}
				}
				break;

				//------------------------------------------------------------
				//
				//			Portal
				// 
				//------------------------------------------------------------
				/*
				case DIK_P : 
				{					
					//------------------------------------------------------------
					// ���� ������ ����
					//------------------------------------------------------------
					if (g_pDXInput->KeyDown(DIK_LMENU))
					{
						MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

						for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
						{
							MCreature* pCreature = (*iCreature).second;

							//--------------------------------------------------
							// Player, NPC�� �ƴ� ���
							//--------------------------------------------------					
							if (pCreature->GetID()!=g_pPlayer->GetID()
								&& pCreature->IsVampire()
								&& !pCreature->IsNPC())
							{					
								AddVampirePortal(
										0,								// ����Ʈ�� OID
										pCreature->GetName(),								// ��Ż ����
										pCreature->GetX(), pCreature->GetY(),		// ��Ż�� ��ǥ
										16*10,							// ��Ż�� ���� �ð�
										
										// ��ǥ ����
										rand()%4+11,							// ��Ż�� ��ǥ �� ID
										rand()%255,			// ��Ż�� ��ǥ ��ǥ x
										rand()%255,			// ��Ż�� ��ǥ ��ǥ y

										(rand()%2? true : false) );

								MoveIntoPortal( pCreature );

								g_pZone->RemoveCreature( pCreature->GetID() );
								
								break;
							}

							iCreature++;
						}
					}
					//------------------------------------------------------------
					// ������ ������ ������
					//------------------------------------------------------------
					else if (g_pDXInput->KeyDown(DIK_RMENU))
					{
						MCreature* pCreature = AddClientCreature();

						if (pCreature!=NULL
							&& pCreature->IsVampire()
							&& !pCreature->IsNPC())
						{
							AddVampirePortal(
										0,								// ����Ʈ�� OID
										pCreature->GetName(),								// ��Ż ����
										pCreature->GetX(), pCreature->GetY(),		// ��Ż�� ��ǥ
										16*10,							// ��Ż�� ���� �ð�
										
										// ��ǥ ����
										rand()%4+11,							// ��Ż�� ��ǥ �� ID
										rand()%255,			// ��Ż�� ��ǥ ��ǥ x
										rand()%255,			// ��Ż�� ��ǥ ��ǥ y

										(rand()%2? true : false) );

							ComeFromPortal( pCreature );
						}
					}					
				}
				break;				
				*/

				//------------------------------------------------------------
				//
				// casket
				//
				//------------------------------------------------------------
				case DIK_V : 
				{
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

						for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
						{
							MCreature* pCreature = (*iCreature).second;

							//--------------------------------------------------
							// Player, NPC�� �ƴ� ���
							//--------------------------------------------------					
							if (pCreature->GetID()!=g_pPlayer->GetID()
								&& pCreature->IsVampire()
								&& !pCreature->IsNPC())
							{							
								if (pCreature->IsInCasket())
								{
									pCreature->RemoveCasket();
								}
								else
								{
									pCreature->AddCasket( rand()%3 );
								}							
							}

							iCreature++;
						}
					}
				}
				break;

				case DIK_C : 
				{
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						if (g_pPlayer->IsInCasket())
						{
							g_pPlayer->RemoveCasket();
						}
						else
						{
							g_pPlayer->AddCasket( rand()%3 );
						}

						/*
						#ifdef OUTPUT_DEBUG
							if (g_pDebugMessage!=NULL)
							{
								char logFile[128];
								strcpy(logFile, g_pDebugMessage->GetFilename());
								
								g_pDebugMessage->Init(MAX_DEBUGMESSAGE, 256, logFile);
							}
						#endif
						*/
						/*
						int x = g_pPlayer->GetX();
						int y = g_pPlayer->GetY();
						int dir = g_pPlayer->GetDirection();

						MFakeCreature* pCreature = new MFakeCreature;

						pCreature->SetID( 0 );
						pCreature->SetSameBody( g_pPlayer );
						pCreature->SetSameWear( g_pPlayer );
						
						pCreature->SetZone( g_pZone );

						pCreature->SetPosition( x, y );
						pCreature->SetServerPosition( x, y );
						pCreature->SetDirection( dir );
						pCreature->SetCurrentDirection( dir );
						pCreature->SetAction( ACTION_ATTACK );
						pCreature->SetWeaponSpeed( g_pPlayer->GetWeaponSpeed() );

						pCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_FADE_ACTION );						

						if (!g_pZone->AddFakeCreature( pCreature ))
						{
							delete pCreature;
						}
						*/
					}
				}
				break;

				//------------------------------------------------------------
				//
				// ��ǻ�� ����
				//
				//------------------------------------------------------------
				case DIK_D : 
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						UI_RunComputer();
					}
					else if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						
					}

				break;

				//------------------------------------------------------------
				//
				// ĳ���� �߰� - packet �׽�Ʈ�� ���ؼ�
				//
				//------------------------------------------------------------
				case DIK_K : 
				{
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						int objectID = MFakeCreature::GetFakeID();
						char name[80];
						sprintf(name, "Fake%d", objectID);
						int x = g_pPlayer->GetX() + rand()%30-15;
						int y = g_pPlayer->GetY() + rand()%30-15;
						int dir = rand()%8;

						if (x >=0 && x < g_pZone->GetWidth()
							&& y >= 0 && y < g_pZone->GetHeight())
						{
							MCreature* pCreature = g_pZone->GetCreature( objectID );

							if (pCreature!=NULL)
							{
								break;
							}


							//------------------------------------------------------------
							// Vampire �߰�
							//------------------------------------------------------------
							if (rand()%2)
							{
								pCreature = new MCreatureWear;

								pCreature->SetZone( g_pZone );
								
								//--------------------------------------------------
								// CreatureType ����
								//--------------------------------------------------
								if (rand()%3)
								{
									if( pCreature->IsVampire() && pCreature->IsAdvancementClass() )
										pCreature->SetCreatureType( CREATURETYPE_VAMPIRE_GHOST );
									else
									{
										pCreature->SetCreatureType( CREATURETYPE_BAT );
										pCreature->SetFlyingCreature();
									}
								}
								else
								{
									pCreature->SetCreatureType( (rand()%2? CREATURETYPE_VAMPIRE_MALE1 : CREATURETYPE_VAMPIRE_FEMALE1) );
								}

								pCreature->SetMale( pCreature->GetCreatureType()==CREATURETYPE_VAMPIRE_MALE1 );						
								

								pCreature->SetName( name );

								// �ӽ÷�
								pCreature->SetGuildNumber( 2 );

								pCreature->SetGroundCreature();
								pCreature->SetID( objectID );
								//pCreature->SetAction(ACTION_MOVE);
								
								pCreature->SetPosition( x, y );
								pCreature->SetServerPosition( x, y );
								pCreature->SetDirection( dir );
								pCreature->SetCurrentDirection( dir );
								pCreature->SetAction( ACTION_STAND );

								// ����
								pCreature->SetBodyColor1( rand()%MAX_COLORSET );
								pCreature->SetBodyColor2( rand()%MAX_COLORSET );
								
								pCreature->SetStatus( MODIFY_MAX_HP, 80 );
								pCreature->SetStatus( MODIFY_CURRENT_HP, 100 );
								pCreature->SetStatus( MODIFY_ALIGNMENT, 7500 );

								pCreature->SetWeaponSpeed( 70 );							

								if (!g_pZone->AddCreature( pCreature ))
								{
									delete pCreature;
								}
							}
							//------------------------------------------------------------
							// Slayer �߰�
							//------------------------------------------------------------
							else
							{
								MCreatureWear* pCreatureWear = new MCreatureWear;

								pCreatureWear->SetZone( g_pZone );

								pCreatureWear->SetCreatureType( (rand()%2)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );
								pCreatureWear->SetGroundCreature();

								
								
								pCreatureWear->SetID( objectID );
								//pCreatureWear->SetAction(ACTION_MOVE);
								pCreatureWear->SetPosition( x, y );
								pCreatureWear->SetServerPosition( x, y );
								pCreatureWear->SetDirection( dir );
								pCreatureWear->SetCurrentDirection( dir );
								pCreatureWear->SetAction( ACTION_STAND );

								// �Ǻλ�
								pCreatureWear->SetBodyColor2( rand()%MAX_COLORSET );

								pCreatureWear->SetStatus( MODIFY_MAX_HP, 80 );
								pCreatureWear->SetStatus( MODIFY_CURRENT_HP, 100 );

								// �̸�
								pCreatureWear->SetName( name );

								// �ӽ÷� NPC������ ������.
								SetAddonToSlayer( pCreatureWear, 250 );

								
								
								if (!g_pZone->AddCreature( pCreatureWear ))
								{
									delete pCreatureWear;
								}
							}
						}
					}
				}
				break;
				

				//-------------------------------------------------------------
				// ���� �ٲٱ�
				//-------------------------------------------------------------
				case DIK_LEFT :
				case DIK_RIGHT :
				{
					MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

					for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
					{
						MCreature* pCreature = (*iCreature).second;

						//--------------------------------------------------
						// Player, NPC�� �ƴ� ���
						//--------------------------------------------------					
						if (pCreature->GetID()!=g_pPlayer->GetID()
							&& !pCreature->IsNPC())
						{							
							int dir = pCreature->GetDirection();

							if (key==DIK_LEFT)
							{
								dir = (dir + 7) % 8;
							}
							else
							{
								dir = (dir + 1) % 8;
							}

							pCreature->SetDirection( dir );									
						}

						iCreature++;
					}
				}
				break;

				//------------------------------------
				// HP ȸ�� test
				//------------------------------------
				case DIK_R : 
					if (g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU))
					{
						g_pPlayer->SetStatus( MODIFY_CURRENT_HP, 0 );
						g_pPlayer->SetStatus( MODIFY_MAX_HP, 100 );
						g_pPlayer->SetRecoveryHP( 1, 100, 200 );
					}
				break;


				//------------------------------------------------------------
				//
				// zone �̵� �ڵ�..
				//
				//------------------------------------------------------------
				case DIK_LBRACKET : case DIK_RBRACKET :
					/*
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						char str[80];

						if (key==DIK_LBRACKET)
						{
							// �����̾� ���
							strcpy(str, "*warp 3 229 52");
						}
						else
						{
							// �����̾� ���
							sprintf(str, "*warp 9 55 78");
						}

						#ifdef CONNECT_SERVER
							CGSay _CGSay;
							_CGSay.setMessage( str );	//pWansungString );
							g_pSocket->sendPacket( &_CGSay );

							#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
									DEBUG_ADD_FORMAT("[Send] %s", _CGSay.toString().c_str() );
							#endif	
						#endif
					}
					*/
				break;						

				/*
				case DIK_GRAVE :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						(*g_pUIDialog).PopupDirectionSelectDlg(0, 0);
					}					
				break;
				*/

				

//				case DIK_5 :
//				{	
//					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//					{
//						g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );
//						g_pPlayer->SetBasicActionInfo( SKILL_ATTACK_MELEE );
//						g_pPlayer->SetStop();
//								
//						if (g_pPlayer->IsSlayer())
//						{
//							if (g_pPlayer->IsMale())
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_SLAYER_FEMALE );
//							}
//							else
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_SLAYER_MALE );
//							}
//						}
//						else
//						{
//							if (g_pPlayer->IsMale())
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_FEMALE1 );
//							}
//							else
//							{
//								g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_MALE1 );
//							}
//						}
//						/*
//						if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//							for (int i=0; i<8; i++)
//							{
//								const MItem* pItem = g_Inventory.GetItem(i, 0);
//
//								if (pItem!=NULL)
//								{
//									AddNewInventoryEffect( pItem->GetID(),
//														RESULT_MAGIC_CREATE_HOLY_WATER,
//														80	// 5��
//													);
//								}
//							}
//							*/
//					}
//				}
//				break;

				

//				case DIK_8 :
//				{
//					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//					{
//						if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SNIPPING_MODE))
//						{
//							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_SNIPPING_MODE);
//						}
//						else
//						{
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_SNIPPING_MODE, 16*10);
//							g_pPlayer->SetInvisible();
//						}
//					}				
//				}
//				break;

//				case DIK_9 :
//				{
//					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
//					{
//						if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS))
//						{
//							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS);
//						}
//						else
//						{
//							g_pPlayer->AddEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS, 16*10);
//						}
////						if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SHUT_DOWN))
////						{
////							g_pPlayer->RemoveEffectStatus(EFFECTSTATUS_SHUT_DOWN);
////						}
////						else
////						{
////							g_pPlayer->AddEffectStatus(EFFECTSTATUS_SHUT_DOWN, 16*10);
////						}
//					}				
//				}
//				break;

				

				/*
				case DIK_1 :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						MItem*	pBlade = new MBlade;	pBlade->SetItemType( 0 ); pBlade->SetItemOption( 0 );
						g_pPlayer->RemoveAddon( ADDON_RIGHTHAND );
						g_pPlayer->SetAddonItem( pBlade );				
					}
				break;

				case DIK_2 :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						MItem*	pSword = new MSword;	pSword->SetItemType( 0 );	pSword->SetItemOption( 0 );
						g_pPlayer->RemoveAddon( ADDON_RIGHTHAND );
						g_pPlayer->SetAddonItem( pSword );
					}				
				break;

				case DIK_7 :
				case DIK_8 :
				case DIK_9 :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						static int itemType[] = { 0, 1, 5 };
						MItem*	pShield = new MShield;	pShield->SetItemType( itemType[key-DIK_7] ); pShield->SetItemOption( 0 );
						g_pPlayer->RemoveAddon( ADDON_LEFTHAND );
						g_pPlayer->SetAddonItem( pShield );				
					}
				break;

				case DIK_3 :			
				case DIK_4 :
				case DIK_5 :			
				case DIK_6 :
				{
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						MGunItem* pGunItem;

						#ifdef __FIX_GUNFRAME__
							int ac;
						#endif
						
						switch (key)
						{
							case DIK_3 :
								pGunItem = new MGunTR;
								#ifdef __FIX_GUNFRAME__
									ac = SKILL_ATTACK_GUN_TR;						
								#endif
							break;

							case DIK_4 :
								pGunItem = new MGunSMG;
								#ifdef __FIX_GUNFRAME__
									ac = SKILL_ATTACK_GUN_SMG;						
								#endif
							break;

							case DIK_5 :
								pGunItem = new MGunAR;
								#ifdef __FIX_GUNFRAME__
									ac = SKILL_ATTACK_GUN_AR;						
								#endif
							break;

							case DIK_6 :
								pGunItem = new MGunSG;
								#ifdef __FIX_GUNFRAME__
									ac = SKILL_ATTACK_GUN_SG;						
								#endif
							break;

							
						}

						#ifdef __FIX_GUNFRAME__
							// �ӽ� �� �Ҳ�
							g_pPlayer->ClearAttachEffect();
							MAttachEffect* pEffect = g_pPlayer->CreateAttachEffect( 
																(*g_pActionInfoTable)[ac].GetActionEffectSpriteType(), 
																//(*g_pActionInfoTable)[m_nUsedActionInfo].GetDelay()
																0xFFFF
																);	// ���� �ð�

							if (pEffect != NULL)
							{
								pEffect->SetDirection( g_pPlayer->GetDirection() );

								pEffect->SetLink( ac, NULL );

								// �پ�� �ϴ� ĳ����
								pEffect->SetAttachCreatureID( 1 );			
							}
						#endif

						pGunItem->SetItemType( 0 ); pGunItem->SetItemOption( 0 );				
					
						MMagazine* pMagazine = (MMagazine*)MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );

						// �ǹ� ���� - -;
						pMagazine->SetID( 0 );

						// �̰Ŵ� �ѿ� ���缭 ����ߵȴ�.
						for (int j=0; j<g_ItemTable[ITEM_CLASS_MAGAZINE].GetSize(); j++)			
						{
							pMagazine->SetItemType(	j );

							if (pMagazine->IsInsertToItem( pGunItem ))
							{
								break;
							}
						}

						// �ǹ� ����
						pMagazine->SetItemOption( 0 );

						// źâ ����
						pMagazine->SetNumber( 0xFFFF );

						//------------------------------------
						// źâ ����
						//------------------------------------
						pGunItem->SetMagazine( pMagazine );

						g_pPlayer->RemoveAddon( ADDON_RIGHTHAND );
						g_pPlayer->RemoveAddon( ADDON_LEFTHAND );
						g_pPlayer->SetAddonItem( pGunItem );
					}
				}
				break;
				*/

				case DIK_A :
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						//------------------------------------------------------
						// �����
						//------------------------------------------------------
						MCreature* pCreature = g_pZone->GetCreature( 1000 );

						if (pCreature!=NULL)
						{
							//------------------------------------------------------
							// TileOK2�� ���� ��� 
							//------------------------------------------------------
							MActionResult* pResult = new MActionResult;
	
							pResult->Add( new MActionResultNodeActionInfo( 
														MAGIC_LIGHT,
														pCreature->GetID(),
														g_pPlayer->GetID(), 
														g_pPlayer->GetX(),
														g_pPlayer->GetY()
														) 
										);
								
							//------------------------------------------------------
							// TileOK2�� ���� ��� �߰�
							//------------------------------------------------------
							//Duration_t	m_Duration;
							pCreature->PacketSpecialActionToSector(
												MAGIC_LIGHT, 
												g_pPlayer->GetX(), g_pPlayer->GetY(),
												pResult						// ���
							);

							// ������ �ٶ󺸱�
							pCreature->SetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY());
					
						}
					}
				break;

				/*		
				case DIK_Q :								
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						if (g_pPlayer->IsDead())
						{						
							g_pPlayer->SetAlive();
							if (g_pTopView!=NULL)
							{
								g_pTopView->SetFadeEnd();
							}
						}
						else
						{
							// ���� ���..
							g_pPlayer->SetDead();						
						}
					}
				break;

				case DIK_W :								
					if (g_pDXInput->KeyDown(DIK_LCONTROL) || g_pDXInput->KeyDown(DIK_RCONTROL))
					{
						for (int i=0; i<g_MaxNPC; i++)
						{
							MCreature* pCreature = g_pZone->GetCreature(1000+i);

							if (pCreature!=NULL)
							{
								if (pCreature->IsDead())
								{
									pCreature->SetAlive();
								}
								else
								{
									pCreature->SetDead();
								}
							}
						}
					}
				break;
				*/
			#endif			
		}
	}
	else // if (event==CDirectInput::KEYUP)
	{
	}
}

//-----------------------------------------------------------------------------
// Update Game
//-----------------------------------------------------------------------------
// ���� ���� ��...
//-----------------------------------------------------------------------------
class CGLeranSkill;
void 
CGameUpdate::Update(void)
{
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		DEBUG_ADD("UB");
	#endif

	__BEGIN_PROFILE("GameUpdate")

	static DWORD lastTime = g_CurrentTime;
	//bool bChanged = false;

	if (g_Mode!=MODE_GAME || g_pClientConfig==NULL)
	{
		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			DEBUG_ADD("UE1");
		#endif
		return;
	}

	Phoenix_SendPingRequestIfNeeded();
	
	//------------------------------------------
	// Logout �� �ð���ġ üũ�Ѵ�.
	//------------------------------------------
	if (g_pUserInformation!=NULL
		&& g_pUserInformation->LogoutTime!=0
		&& g_CurrentTime > g_pUserInformation->LogoutTime
		&& g_pPlayer->GetWaitVerify() == MPlayer::WAIT_VERIFY_LOGOUT)
	{
		ExecuteLogout();
		
		g_pUserInformation->LogoutTime = 0;	// Logout�ð� ����

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			DEBUG_ADD("UE2");
		#endif

		return;
	}

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	//------------------------------------------
	// ���������� �ð���ġ üũ�Ѵ�.
	//------------------------------------------
	if (g_pUserInformation!=NULL
		&& g_pUserInformation->QuitExitTime!=0
		&& g_CurrentTime > g_pUserInformation->QuitExitTime
		&& g_pPlayer->GetWaitVerify() == MPlayer::WAIT_VERIFY_LOGOUT)
	{
		ExecuteQuitExit();
		
		g_pUserInformation->QuitExitTime = 0;	// Logout�ð� ����

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			DEBUG_ADD("UE2");
		#endif

		return;
	}
#endif //__080405_FIREST_UI_UPDATE

	//------------------------------------------
	// Sound Stream
	//------------------------------------------
//	if (g_pDXSoundStream!=NULL)
//	{
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//			DEBUG_ADD("DSU");
//		#endif
//
//		g_pDXSoundStream->Update();
//
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//			DEBUG_ADD("DSUk");
//		#endif
//	}


	//---------------------------------------------
	// Sound����
	//---------------------------------------------
	// �� loop ���� �����൵ ���� ������..
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		DEBUG_ADD("DXRTDB");
	#endif

	{
		Phoenix_FrameProbeStageScope fpStage("Update.SoundRelease", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		g_DXSound.ReleaseTerminatedDuplicateBuffer();
	}

	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		DEBUG_ADD("DXRTDBk");
	#endif
	
	static DWORD nextSoundCheckTime = g_CurrentTime;

	if (g_CurrentTime > nextSoundCheckTime)
	{
		// �ʴ� play�� sound ��..
		g_SoundPerSecond = 0;

		// 1�� ��
		nextSoundCheckTime = g_CurrentTime + 1000;
	}

	//---------------------------------------------------
	//
	// ���� �ð����� �ѹ��� ó���� ���ش�.
	//
	//---------------------------------------------------
	// [����]
	// g_UpdateDelay	: 100
	// lastTime			: 1000 
	// currentTime		: 1240 
	//
	// 1200, 1100�� �ι��� ó���ϰ� �ȴ�.
	//---------------------------------------------------

	int k = g_pClientConfig->MAX_UPDATE_ONETIME;		// 12 frame * 1 Tile.. 

//	#ifdef OUTPUT_DEBUG
		if (g_pOperatorOption->bSlideScreenShot)
		{
			k = 1;
		}
//	#endif
	bool bGameLoopStop = false;
//	if (g_CurrentTime - lastTime >= g_lGameRunBreakTime)
	if (g_lGameRunBreakTime)
	{
		k = 1;
		bGameLoopStop = true;
	}

	{
		Phoenix_FrameProbeStageScope fpStage("Update.Mouse", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		UpdateMouse();
	}

	//------------------------------------------
	// ���ѷ��� ���浵 �ǰ�...
	// k������ �ѹ����� Draw�� ���ֱ� ������
	// Frame Skipping�� �����Ų��.				
	//------------------------------------------
	if (g_CurrentTime - lastTime >= g_UpdateDelay)
	{
		// ��ȭ�Ȱ��� �ִٰ� check
		g_bFrameChanged = true;
		do
		{
			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD("CGUP");
			#endif

			// ������ frame�� ����
			g_CurrentFrame++;
		
			//if (--k==0) break;

			//------------------------------------------
			//
			// Socket �Է� ó��
			//
			//------------------------------------------
			__BEGIN_PROFILE("GameSocketInput")
			{
				Phoenix_FrameProbeStageScope fpStage("Update.SocketInput", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				if (!UpdateSocketInput())
				{
					CT_BD("[SOCK] CGameUpdate UpdateSocketInput failed -> return");
					DEBUG_ADD("[CGameUpdate] UpdateSocketInput Failed");

					__END_PROFILE("GameSocketInput")

					return;
				}
			}
			__END_PROFILE("GameSocketInput")

			if (g_Mode!=MODE_GAME)
			{
				DEBUG_ADD("[CGameUpdate] Not MODE_GAME");
				
				return;
			}

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD( "USInOK" );
			#endif

			//------------------------------------------
			//
			// Input ó��
			//
			//------------------------------------------
			if (g_bActiveGame)
			{
				// Input���� �о�´�.
				if (!(g_lGameRunBreakTime))
				{
					Phoenix_FrameProbeStageScope fpStage("Update.RawInput", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
					UpdateInput();
				}
				
				__BEGIN_PROFILE("ProcessInput")

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD( "PI" );
				#endif

				{
					Phoenix_FrameProbeStageScope fpStage("Update.ProcessInput", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
					ProcessInput();
				}

				__END_PROFILE("ProcessInput")
			}

			__BEGIN_PROFILE("UIProcess")

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD( "UI_pro" );
			#endif

			{
				Phoenix_FrameProbeStageScope fpStage("Update.UIProcess", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				gC_vs_ui.Process();
			}

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD( "UI_pro ok" );
			#endif

			__END_PROFILE("UIProcess")

			if (!g_bActiveApp || g_Mode!=MODE_GAME)
			{
				DEBUG_ADD("[CGameUpdate] Not bActiveApp");

				return;
			}

			//------------------------------------------
			// 1�и��� �ѹ��� ������ packet
			//------------------------------------------
			{
				Phoenix_FrameProbeStageScope fpStage("Update.CheckTime", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				CheckTime();
			}

			//------------------------------------------
			// ������ �����ؾ��ϴ� ��쿡�� ���� ����..
			//------------------------------------------			
			//KeepConnection();

			//------------------------------------------
			//
			// Socket ��� ó��
			//
			//------------------------------------------
			__BEGIN_PROFILE("GameSocketOutput")

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD( "USOut" );
			#endif

			{
				Phoenix_FrameProbeStageScope fpStage("Update.SocketOutput", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				if (!UpdateSocketOutput() || g_Mode!=MODE_GAME)
				{
					DEBUG_ADD("[CGameUpdate] UpdateSocketOutput Failed or Not MODE_GAME");

					__END_PROFILE("GameSocketOutput")
				
					return;
				}
			}

			__END_PROFILE("GameSocketOutput")

			//------------------------------------------
			//
			//	���� ó��
			//
			//------------------------------------------
			if (g_pWeather!=NULL)
			{
				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD( "Weather" );
				#endif

				{
					Phoenix_FrameProbeStageScope fpStage("Update.Weather", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
					g_pWeather->Action();
				}

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD( "WeaOK" );
				#endif
			}
			///*
			
			//*/
			

			//------------------------------------------
			//
			//	Effect Manager
			//
			//-------------------------------------------
			if (g_pInventoryEffectManager!=NULL)
			{
				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD( "IEM" );
				#endif

				{
					Phoenix_FrameProbeStageScope fpStage("Update.InventoryEffect", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
					g_pInventoryEffectManager->Update();
				}

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD( "IEM ok" );
				#endif
			}
			
			//------------------------------------------
			//
			//	Event Manager
			//
			//-------------------------------------------
			if (g_pEventManager != NULL)
			{
				Phoenix_FrameProbeStageScope fpStage("Update.EventManager", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				g_pEventManager->ProcessEvent();
			}

			if (g_pEventMessage) 
			{
				Phoenix_FrameProbeStageScope fpStage("Update.EventMessage", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				g_pEventMessage->Update();
			}

			//------------------------------------------
			//
			// Zone�� object���� update�Ѵ�.
			//
			//------------------------------------------
			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD("z");//[Update] Before ZoneUpdate");
			#endif
				
			__BEGIN_PROFILE("ZoneUpdate")

			{
				Phoenix_FrameProbeStageScope fpStage("Update.ZoneUpdate", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				g_pZone->Update();
			}

			__END_PROFILE("ZoneUpdate")

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD("zk");//[Update] After ZoneUpdate");
			#endif
	
			lastTime += g_UpdateDelay;
		}
		while (g_CurrentTime - lastTime >= g_UpdateDelay  
					&& --k
					&& !g_lGameRunBreakTime);
		
		// k�� ��ŭ update�ߴµ���..
		// �� update�ؾ��� ���� ������.. �����Ѵ�..
		// �����ұ�??
		// ���ó��.. ���ڱ� �޸���� ���? -_-;
		static int OnetimeUpdateCount = 0;
		if (k==0)
		{
			//lastTime = g_CurrentTime;  // �����ϴ� ���..
			OnetimeUpdateCount++;
	
			if (OnetimeUpdateCount > g_pClientConfig->MAX_UPDATE_ONETIME_COUNT)
			{
				// �� �̻��� update���ϰ� �Ѵ�.
				lastTime = g_CurrentTime;
				OnetimeUpdateCount = 0;
			}
		}
		else
		{
			OnetimeUpdateCount = 0;
		}
		if (bGameLoopStop && g_lGameRunBreakTime)
		{
			lastTime			= g_CurrentTime;
			g_lGameRunBreakTime	= 0;
		}
		
		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			DEBUG_ADD("CGUPOK");
		#endif
	}
	else
	{
		g_bFrameChanged = false;
	}

	//---------------------------------------------
	// Update ȸ�� ���
	//---------------------------------------------
	/*
	#ifdef OUTPUT_DEBUG
		if (k!=g_pClientConfig->MAX_UPDATE_ONETIME)
		{
			if (g_bActiveGame && bChanged)
			{
					DEBUG_ADD_FORMAT("[ Update (%d) times & Draw ]", 12-k);
			}
			else
			{
					DEBUG_ADD_FORMAT("[ Update (%d) times ]", 12-k);
			}
		}
	#endif
	*/
	// ��ð� ���� �̿� ���� ����
	if( g_pUserInformation != NULL && g_pUserInformation->IsNetmarble)
	{
		static int playhour;
		DWORD currentPlayHour = gGetApplcationRunTime()/3600000;//testTime 1��
		switch( currentPlayHour )
		{
		case 0:
			playhour = 0;
			break;
		case 1:
		case 2:
			if( currentPlayHour > playhour )//add once
			{
				playhour++;
				//g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_WARMING_1HOUROVER].GetString(),currentPlayHour);
				g_pNoticeMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_WARMING_1HOUROVER].GetString(),currentPlayHour);
			}
			break;
		default://3�̻�
			if( currentPlayHour > playhour )//add once
			{
				playhour++;
				g_pNoticeMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_WARMING_3HOUROVER].GetString(),currentPlayHour);
			}
			break;
		}
	}
#if __CONTENTS(__USER_GRADE)
	//����� ��޿� ���� ���� ��� ǥ��20070419 by diesirace 
	if(m_userGraddisplay.IsTimeLimit())
	{
		if(g_pUserInformation->GoreLevel)
		{
			UI_AddChatToHistory((*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADEDISPLAY_BLACK].GetString(), 
			(*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADEDISPLAY_SAYMAN].GetString(), 7, RGB(255, 255, 0));
		}
		else
		{
			UI_AddChatToHistory((*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADEDISPLAY_WHITE].GetString(), 
			(*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADEDISPLAY_SAYMAN].GetString(), 7, RGB(255, 255, 0));
		}

		DWORD currentTime = timeGetTime();
		DWORD startTime = m_userGraddisplay.GetStartTime();
		DWORD passageTime = currentTime - startTime;
		DWORD passgeHour = passageTime/m_userGraddisplay.GetTickSize();
		if(passgeHour)
		{
			char str[256];
			sprintf(str,(*g_pGameStringTable)[UI_STRING_MESSAGE_PALYTIME_DISPLAY].GetString(),passgeHour);
			if(passgeHour%3 == 0)
			{
				sprintf(str,(*g_pGameStringTable)[UI_STRING_MESSAGE_PALYTIME_DISPLAY_ADD].GetString(),passgeHour);
			}
			UI_AddChatToHistory(str,(*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADEDISPLAY_SAYMAN].GetString(),
				7,RGB(255, 255, 0));
		}
	}
	

	//static DWORD lastHearBeatTime = g_CurrentTime;

	//if ( g_CurrentTime - lastHearBeatTime > 5000 ) //5����������
	//{
	//	CGRequestPing ping;
	//	g_pSocket->sendPacket(&ping); 
	//	g_PingSendTime = timeGetTime();
	//	lastHearBeatTime = g_CurrentTime;
	//}
	
#endif	//__USER_GRADE

	if (g_pGameTime!=NULL)
	{
		Phoenix_FrameProbeStageScope fpStage("Update.GameTimeUI", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		static int lastHour = g_pGameTime->GetHour()+1;

		int currentHour = g_pGameTime->GetHour();

		if (lastHour != currentHour)
		{
			//---------------------------------------------
			// �ð��� �ٲ����� �׸��ڵ� �ٲ�� �Ѵ�.
			//---------------------------------------------
			if (g_pTopView!=NULL)
			{
				g_pTopView->ClearShadowManager();
			}

			lastHour = g_pGameTime->GetHour();

			// ���� �ð��� ���� ������ �ٲ��� �ʴ� ����� �ּ�ó��
			//---------------------------------------------
			// ���ǵ� �ٲ۴�.
			//---------------------------------------------
			// ¦�� �ð��뿡�� ���� ����..
			//if ((currentHour & 0x01)==0)
			//{
			//	PlayMusicCurrentZone();
			//}

		}

		
		//---------------------------------------------
		// �ð�
		//---------------------------------------------
		char str[80];

		static DWORD lastDisplayGameTime = g_CurrentTime;	

		if (g_pGameStringTable!=NULL)
		{
			g_pGameTime->SetCurrentTime( g_CurrentTime );
		
			sprintf(str, (*g_pGameStringTable)[STRING_DRAW_GAME_TIME].GetString(),
							g_pGameTime->GetHour(),
							g_pGameTime->GetMinute(),
							g_pGameTime->GetSecond()
					);
			
			lastDisplayGameTime = g_CurrentTime;

			gC_vs_ui.SetTime( str );
		}

		//---------------------------------------------
		// �ð�
		//---------------------------------------------
		sprintf(str, (*g_pGameStringTable)[STRING_DRAW_GAME_DATE].GetString(),
					g_pGameTime->GetYear(),
					g_pGameTime->GetMonth(),
					g_pGameTime->GetDay()
				);

		gC_vs_ui.SetDate( str );
	}
	
	
	//---------------------------------------------
	// Zone�� ȯ�� ���� ���
	//---------------------------------------------
	if (g_pZoneSoundManager!=NULL)
	{
		Phoenix_FrameProbeStageScope fpStage("Update.ZoneSound", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		g_pZoneSoundManager->UpdateSound();
	}

	//---------------------------------------------
	// UI�� player ��ǥ ����
	//---------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage("Update.UISetXY", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		gC_vs_ui.SetXY( g_pPlayer->GetX(), g_pPlayer->GetY() );
	}

	//---------------------------------------------
	//
	// Draw - ��ȭ�Ȱ� ������ �׷��ش�.
	//
	//---------------------------------------------
	if (g_bActiveGame
#ifdef OUTPUT_DEBUG
		|| g_bTestMode
#endif
		)
	{			
		static int oldX = -1;
		static int oldY = -1;
		static DWORD oldFrame = 0;

		//------------------------------------------------------
		// Update������.. 
		// ���ο� ���� ȭ���� �׷��ش�.
		//------------------------------------------------------
		{
			static DWORD s_LastUpdateTickTrace = 0;
			DWORD nowTick = timeGetTime();
			if (nowTick - s_LastUpdateTickTrace > 1000) {
				s_LastUpdateTickTrace = nowTick;
				CT_BD("[UPDATE] frameChanged=%d smoothOpt=%d smoothCur=%d dt=%u delay=%d",
				      g_bFrameChanged ? 1 : 0,
				      (*g_pUserOption).UseSmoothCursor ? 1 : 0,
				      g_bSmoothCursor ? 1 : 0,
				      (unsigned)(g_CurrentTime - lastTime),
				      g_UpdateDelay);
			}
		}
		if (g_bFrameChanged || !(*g_pUserOption).UseSmoothCursor)
		{
			g_bNewDraw = true;		
			
			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD("d");//[Update] Before Draw");
			#endif
			
			/*
			//------------------------------------------------------
			// ��ġ�� �ð� ����..
			//------------------------------------------------------
			if (g_CurrentTime > g_pUserInformation->GlobalSayTime+g_pClientConfig->DELAY_GLOBAL_SAY
				//#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
				//	|| 1
				//#endif
				)			
			{
				// �������� ���			
				gC_vs_ui.SetInputStringColor( gpC_base->m_chatting_pi.text_color );
			}
			//------------------------------------------------------
			// ��ġ�� �Ұ��� ����
			//------------------------------------------------------
			else
			{
				const char* pString = gC_vs_ui.GetInputString();

				//if (pString!=NULL && pString[0]=='!')
				if (gC_vs_ui.GetChatMode()==ZONE)
				{
					//if (gbl_vampire_interface)
					{
					//	gC_vs_ui.SetInputStringColor( RGB(50, 50, 200) );
					}
					//else
					{
						gC_vs_ui.SetInputStringColor( RGB(250, 50, 50) );
					}
				}
				else
				{				
					gC_vs_ui.SetInputStringColor( gpC_base->m_chatting_pi.text_color );
				}
			}
			*/

			//------------------------------------------------------
			// ȭ�� ���
			//------------------------------------------------------
			// ���콺 ��ǥ�� ... frame�� �ٲ� ��쿡 ���..
			if (g_CurrentFrame != oldFrame
				|| g_x != oldX
				|| g_y != oldY || 1)
			{
				__BEGIN_PROFILE("GameUpdateDraw")

				{
					Phoenix_FrameProbeStageScope fpStage("Update.GameUpdateDraw", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
					UpdateDraw();
				}

				__END_PROFILE("GameUpdateDraw")

				//
				g_FrameCount++;


				oldX = g_x;
				oldY = g_y;
				oldFrame = g_CurrentFrame;


//				#ifdef OUTPUT_DEBUG
					if (g_pOperatorOption->bSlideScreenShot && g_bSaveSlideScreenShot)
					{
						MakeScreenShot();
					}
//				#endif
			}

			

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD("dk");//[Update] After Draw");
			#endif
			
			//g_FrameCount++;
		}		
		//------------------------------------------------------
		// �ε巴�� �����̴� Ŀ���� ����ұ�??
		//------------------------------------------------------
		else if (g_bSmoothCursor)
		{
			///*
			static int surface = 0;
			POINT point;

			POINT cursorPoint;
			GetCursorPos(&cursorPoint);
			ScreenToClient(g_hWnd, &cursorPoint);


			// UI�� ���콺 ��ǥ ����
			gC_vs_ui.MouseControl(M_MOVING, cursorPoint.x, cursorPoint.y);


			//------------------------------------------------------
			// ��� ���� ȭ���� �׷��� ���¶��..
			//------------------------------------------------------
			// �������ʹ� Flip�ؼ� ���콺�� �׸��� ���ؼ�..
			// Primary�� Back�� �Ȱ��� �����.
			//------------------------------------------------------
			if (g_bNewDraw)
			{
				// �ٽ� false��..
				g_bNewDraw = false;
				
				//------------------------------------------------------
				// PrimarySurface --> BackSurface�� copy
				//------------------------------------------------------
				// �ϵ���� ������ �Ǵ� ���..
				// Primary --> Back����..
				// �׷���(!) ������ ��!! �ִ�.. 
				// �׷��� �ι� Blt�����. - -;;
				//------------------------------------------------------
				// (��� �׸� ȭ��� ������ ȭ���� �ٸ��� ������.. �Ȱ��� ���ش�)
//				if (CDirect3D::IsHAL())
//				{
//					point.x = 0;
//					point.y = 0;
//					RECT rect = { 0, 0, 800, 600 };
//
//					g_pBack->BltPrimarySurface(&point, &rect);
//
//					// ����ߴ� Ŀ�� �κ��� �����ش�. --> (0)
//					//g_pCursorSurface->Restore(0, g_pBack);
//					//g_FrameCount++;
//					CDirectDraw::Flip();
//
//					g_pBack->BltPrimarySurface(&point, &rect);
//
//					// ����ߴ� Ŀ�� �κ��� �����ش�. --> (0)
//					g_pCursorSurface->Restore(0, g_pBack);
//					
//					// ���� ȭ���� Interface�κ��� Update������� �Ѵ�.
//					//
//					// [ TEST CODE ]					
//					//gC_vs_ui.VS_UI_Loop();					
//
//					//---------------------------------------
//					// ���� ���
//					//---------------------------------------
//					//UpdateDrawText();
//				}
//				//------------------------------------------------------
//				// �ϵ���� ������ �ȵǴ� ���..
//				// Last --> Back���� copy
//				//------------------------------------------------------
//				else
				{
					// Last --> Back���� copy
					RECT rect;
					point.x = 0;
					point.y = 0;
					rect.left = 0;
					rect.top = 0;
					rect.right = CLIPSURFACE_WIDTH;
					rect.bottom = CLIPSURFACE_HEIGHT;
					{
						Phoenix_FrameProbeStageScope fpStage("Update.SmoothCursorLastToBack", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
						g_pBack->BltNoColorkey( &point, g_pLast, &rect );
					}

					// ���� ȭ���� Interface�κ��� Update������� �Ѵ�.
					//
					// [ TEST CODE ]					
					{
						Phoenix_FrameProbeStageScope fpStage("Update.SmoothCursorUIShow", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
						gC_vs_ui.Show();
					}

					//---------------------------------------
					// ���� ���
					//---------------------------------------
					//UpdateDrawText();

				}					

				
				MOUSEPOINTER_INFO mp_info;
				gC_vs_ui.GetCurrentMousePointerInfo(mp_info);

				// Ŀ�� ��� ��ġ�� ����Ų��. --> (1)
				point.x = mp_info.x;
				point.y = mp_info.y;
				g_pCursorSurface->Store(1, g_pBack, &point);

				// ������ ó���� surface��ȣ
				surface = 0;
			}
			//------------------------------------------------------
			// Flip�ؼ� ���콺�� Update�ϴ� ����..
			//------------------------------------------------------
			else
			{	
				// ����ߴ� �κ��� ����ؼ� Ŀ���� �����ش�. --> (surface)
				g_pCursorSurface->Restore(surface, g_pBack);

				// ���콺 ��� ���� ����
				MOUSEPOINTER_INFO mp_info;
				gC_vs_ui.GetCurrentMousePointerInfo(mp_info);

				// Ŀ�� ��� ��ġ�� ����Ų��. --> (surface)
				point.x = mp_info.x;
				point.y = mp_info.y;
				g_pCursorSurface->Store(surface, g_pBack, &point);
				
				//g_pCursorSurface->Restore(surface, g_pBack);

				// ������ ó���� surface��ȣ
				surface = ((surface==0)? 1 : 0);				
			}
			
			//static POINT previousPoint;
			//if (previousPoint.x != cursorPoint.x
			//	|| previousPoint.y != cursorPoint.y)
			//{
			//	gC_vs_ui.VS_UI_Loop();
			//}
			//previousPoint = cursorPoint;
			
			if (!g_pTopView->IsDrawRequest())
			{
				gC_vs_ui.DrawMousePointer();
			}			
			

			// Flip		
			//g_FrameCount++;
			{
				Phoenix_FrameProbeStageScope fpStage("Update.SmoothCursorFlip", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
				CDirectDraw::Flip();
			}
		}
	}
//#ifdef OUTPUT_DEBUG
//	static DWORD playTime = 0;
//	if( g_bTestMusic )			// ������� �������̸�
//	{		
//		if( playTime == 0 )
//			playTime = timeGetTime();
//
//		if( g_pMP3->IsStop() )
//		{
//			playTime = timeGetTime();
//			g_CurrentMusicNum ++;
//			if(g_CurrentMusicNum >= 8 )
//				g_CurrentMusicNum = 0;
//
//			g_pMP3->Stop();
//			g_pMP3->Open( g_musicfilename[g_CurrentMusicNum].c_str() );
//			g_pMP3->Play( false );
//
//			g_pSystemMessage->Add( g_musicfilename[g_CurrentMusicNum].c_str() );
//		} else
//		{
//			g_pSystemMessage->Clear();
//			char szbuffer[512];
//			wsprintf(szbuffer,"CTRL+1~8�� �����ø� �ش� � �ݺ��ؼ� ���� �� �ֽ��ϴ�.CTRL+0 �� ���������� �Ѿ�ϴ�.");
//			g_pSystemMessage->Add(szbuffer);
//			wsprintf(szbuffer,"%s(%d/8) %2d:%2d", g_musicfilename[g_CurrentMusicNum].c_str(), g_CurrentMusicNum+1,(timeGetTime() - playTime)/1000/60, ((timeGetTime() - playTime )/ 1000)%60 );
//			g_pSystemMessage->Add(szbuffer);
//		}
//	}
//#endif
//

	__END_PROFILE("GameUpdate")

	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
		DEBUG_ADD("UE0");
	#endif
}

//-----------------------------------------------------------------------------
// ProcessInput RButton Down
//-----------------------------------------------------------------------------
// pObject�� ����..�� ���� ���� �ְ�..
//-----------------------------------------------------------------------------
void
ProcessInputRButtonDown(MObject* pObject, bool bForceAttack = false)
{
	g_bRButtonDown = TRUE;

	// ���õ� sector�� ���� �Ѵ�.
	if (g_pTopView!=NULL)
	{
		g_pTopView->SetSelectedSectorNULL();
	}
//	UI_RunRaceWarNotice(0xFFFFFFFF);
	
	//bool bFreePKZone = g_pZoneTable->Get( g_pZone->GetID() )->FreePK;
	if(NULL != pObject)
		bForceAttack |= (g_pObjectSelector->IsWarEnemy( (MCreature*) pObject ) == TRUE);
	//bForceAttack |= ((g_pZone->GetID() == 1303)||(g_pZone->GetID() == 1020));
//	bForceAttack |= g_pZone->IsFreePKZone();

	if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_DRAGON_EYES))
		return;
	if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		if(!g_pPlayer->IsStop())
			return;

		if(g_pPlayer->GetSpecialActionInfo() == MAGIC_UN_TRANSFORM)
		{
		//	if(!g_pPlayer->IsStop()) return;
			CGSkillToSelf _CGSkillToSelf;
			_CGSkillToSelf.setSkillType( MAGIC_UN_TRANSFORM );
			g_pSocket->sendPacket( &_CGSkillToSelf );
		//	g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF );
			return ;
		}
		else if(g_pPlayer->GetSpecialActionInfo() == SKILL_TURRET_FIRE)
			PlaySound( SOUND_SLAYER_TURRET_FIRE_START );
	}

	//Add By Hays 2013  3  10

	if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
	{
		if(!g_pPlayer->IsStop())
			return;

		if(g_pPlayer->GetSpecialActionInfo() == MAGIC_UN_TRANSFORM)
		{
		//	if(!g_pPlayer->IsStop()) return;
			CGSkillToSelf _CGSkillToSelf;
			_CGSkillToSelf.setSkillType( MAGIC_UN_TRANSFORM );
			g_pSocket->sendPacket( &_CGSkillToSelf );
		//	g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF );
			return ;
		}
	//	else if(g_pPlayer->GetSpecialActionInfo() == SKILL_CLINT_ADV2_HALCYON_MK1)
	//		PlaySound( SOUND_SLAYER_TURRET_FIRE_START );
	} // by rallser

	if( HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(g_pPlayer))
#if __CONTENTS(__FAST_TRANSFORTER)
		|| HasEffectStatusWingSylph( dynamic_cast<MCreature*>(g_pPlayer))
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| HasEffectStatusUnicorn( dynamic_cast<MCreature*>(g_pPlayer))
		|| HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(g_pPlayer))
		|| HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(g_pPlayer))
#endif //__SECOND_TRANSFORTER
		)
//		&& //g_pPlayer->HasEffectStatus( EFFECTSTATUS_SUMMON_SYLPH ) && 
//		!g_bZoneSafe && !g_pPlayer->IsInSafeSector() 
	{ 
		if(!g_pPlayer->IsStop()) return;
		
		if(!g_bZoneSafe && !g_pPlayer->IsInSafeSector())
		{
			CGSkillToSelf _CGSkillToSelf;
			_CGSkillToSelf.setSkillType( MAGIC_UN_TRANSFORM );
	//		_CGSkillToSelf.setCEffectID( m_pEffectTarget->GetEffectID() );
			g_pSocket->sendPacket( &_CGSkillToSelf );
			g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF );
			g_pSkillInfoTable->SetAvailableTime(SKILL_SUMMON_SYLPH, 4000);
	//		gC_vs_ui.UnselectSkill();
		}
		else 
			return;
	} else
	//---------------------------------------------------------------
	// 
	// ������̸� Ÿ�� �ִ� ��� 
	//
	//---------------------------------------------------------------
	// ������̿��� ������.
	//---------------------------------------------------------------
	if (g_pPlayer->GetMoveDevice()==MCreature::MOVE_DEVICE_RIDE
		&& g_pPlayer->IsStop()
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_EXPLOSION_WATER)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_STUN)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_CURSE_OF_BLOOD)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_FREEZE)
		&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_unknow660)
		//&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_unknow231_21)
		//&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_994)
		)
	{
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("MotorOff");
#endif
	
		if( ! IsExistCorpseFromPlayer(dynamic_cast<MCreature*>(g_pPlayer), 670) )
		{		
			CGGetOffMotorCycle _CGGetOffMotorCycle;
			
			_CGGetOffMotorCycle.setObjectID( 0 );
			
			g_pSocket->sendPacket(&_CGGetOffMotorCycle);
			
#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[Send] %s", _CGGetOffMotorCycle.toString().c_str() );
#endif
			
			g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_MOTORCYCLE_GETOFF );
		} else
		{
			g_pSystemMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_ACTION_MOTORCYCLE_FLAG].GetString() );
		}
	}
	//---------------------------------------------------------------
	//
	// Burrow �� ��� ������ �ھƳ��´ٴ� packet�� ������.
	//
	//---------------------------------------------------------------
	else if (g_pPlayer->IsUndergroundCreature())
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("Unburrow");
		#endif

		CGUnburrow _CGUnburrow;

		_CGUnburrow.setX( g_pPlayer->GetX() );
		_CGUnburrow.setY( g_pPlayer->GetY() );
		_CGUnburrow.setDir( g_pPlayer->GetDirection() );

		g_pSocket->sendPacket(&_CGUnburrow);

		#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[Send] %s", _CGUnburrow.toString().c_str() );
		#endif

		g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
	}
	//---------------------------------------------------------------
	//
	// object/tile�� ����� ����ϴ� ��� 
	//
	//---------------------------------------------------------------
	else
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("TileSkill");
		#endif

		// sector �����ϱ�
		//g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
		//g_pTopView->SetSelectedSector(g_SelectSector.x, g_SelectSector.y);

		//
		//MObject*	pObject = g_pTopView->GetSelectedObject(g_x, g_y);
	
		//--------------------------------------------------
		// ������ ���� Object�� ������ ..
		//--------------------------------------------------
		if (pObject == NULL)
		{		
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();

			//--------------------------------------------------
			// �ڽ����� Ư�� ��� ���
			//--------------------------------------------------
			if (g_pPlayer->SelfSpecialAction())
			{
				g_pPlayer->SetRepeatAction();
			}
			//--------------------------------------------------
			// �ڽ����� ����ϴ� Ư�� ����� �ƴ� ���
			//--------------------------------------------------				
			else if (!g_pPlayer->IsSpecialActionInfoTargetSelf())
			{
				g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

				//--------------------------------------------------				
				// Zone�� Ư�� ����� ����Ѵ�.
				//--------------------------------------------------				
				if (g_pPlayer->TraceSectorToSpecialAction( g_SelectSector.x, g_SelectSector.y ))
				{
					// ���õ� Sector�� ǥ���Ѵ�.
					g_pTopView->SetSelectedSector( g_SelectSector );
				
					g_pPlayer->SetRepeatAction();

					g_bPreviousMove = false;
				}
			}

			//----------------------------------
			// Tile�̳� �ڽſ��� �ϴ� �ൿ �ݺ� ����
			//----------------------------------						
			g_pTopView->SetSelectedSectorNULL();
			
		}		
		//--------------------------------------------------
		// ���õ� Object�� ���ؼ� 
		//--------------------------------------------------
		else 
		{				
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("Obj");
			#endif


			switch (pObject->GetObjectType())
			{	
				//--------------------------------------------------
				// Creature
				//--------------------------------------------------
				case MObject::TYPE_CREATURE :		
				{
					// ��ǥ�� �Ǵ� Creature
					MCreature* pCreature = ((MCreature*)pObject);
					
					//--------------------------------------------------
					// �ٸ� �ִ����� ����ϴ°ǰ�?
					//--------------------------------------------------
					if (g_pPlayer->TraceCreatureToSpecialAction( pCreature->GetID(), bForceAttack ))
					{
						g_pPlayer->SetRepeatAction(); 								
						g_bPreviousMove = false;
					}
					else if (!g_pPlayer->IsSpecialActionInfoTargetOther())
					{
						//--------------------------------------------------
						// (!) �ٸ� Creature�� ����ϴ°� �ƴ� ���
						//--------------------------------------------------
						// �ڽ����� Ư�� ��� ���
						if (g_pPlayer->SelfSpecialAction())
						{
							g_pPlayer->SetRepeatAction();
						}
						else
						{	
							//--------------------------------------------------
							// �ڽſ��� ����ϴ°� �ƴ� ���
							//--------------------------------------------------
							// ���õƴ� Creature�� ��ǥ�� ..
							// Zone�� Ư�� ����� ����ұ�?
							if (!g_pPlayer->IsSpecialActionInfoTargetSelf()
								&& g_pPlayer->TraceSectorToSpecialAction( pCreature->GetX(), pCreature->GetY()))
							{
								g_pPlayer->SetRepeatAction();

								g_bPreviousMove = false;
							}
						}									
					}
					
					//----------------------------------
					// ���� sectorǥ�� ���ֱ�
					//----------------------------------
					g_pTopView->SetSelectedSectorNULL();
				}
				break;

				//--------------------------------------------------
				// Item
				//--------------------------------------------------
				// Item�� ����.. �ڽ����� ����ϰų� Zone�� ����Ѵ�.
				case MObject::TYPE_ITEM :		
				{
					//--------------------------------------------------
					// �ڽ����� Ư�� ��� ���
					//--------------------------------------------------
					if (g_pPlayer->SelfSpecialAction())
					{
						g_pPlayer->SetRepeatAction();
					}
					else
					{	
						//--------------------------------------------------
						// �ڽ����� ����ϴ°� �ƴ� ���..
						//--------------------------------------------------
						// ��ǥ�� �Ǵ� Creature
						if (!g_pPlayer->IsSpecialActionInfoTargetSelf())
						{
							MItem* pItem = ((MItem*)pObject);

							if (pItem!=NULL)
							{
								// ���ϵ� ������ ��ü���� ������ ���� ó��
								if(g_pPlayer->GetSpecialActionInfo() == SKILL_WILD_WOLF)
								{
									g_pPlayer->UseWildWolf_Corpse(pItem);
								}
								// ���õƴ� Creature�� ��ǥ�� ..
								// Zone�� Ư�� ����� ����Ѵ�.
								else if (g_pPlayer->TraceSectorToSpecialAction( pItem->GetX(), pItem->GetY()))
								{
									g_pPlayer->SetRepeatAction();
									
									g_bPreviousMove = false;
								}
							}
						}
					}
				}
				break;

			}
		}		
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("RbOK");
	#endif
}

//---------------------------------------------------------------------------
// Process Game Input
//---------------------------------------------------------------------------
void
CGameUpdate::ProcessInput()
{
	Phoenix_MouseAuditTick();

	if(g_pEventManager->GetEventByFlag(EVENTFLAG_DENY_INPUT))
	{
		Phoenix_MouseAuditResolveActiveLost("PROCESS_EVENTFLAG_DENY_INPUT");
		Phoenix_CancelMouseCaptureState("deny-input", true, true, true, true);
		return;
	}

	Phoenix_ReconcileLostMouseRelease("process-input");

	// FAZ 4 (2026-05-11) Bug 9 fix — Mouse focus loss.
	// Karakter mouse-look ile yuruyor; mouse window disina cikinca veya pencere
	// focus kaybedince yuruyusu durdurmuyordu. Cunku DirectInput m_rb_up/m_lb_up
	// eventi yalnizca pencerede gerceklesirse uretiliyor; focus disinda kalmis
	// buton state TRUE kaliyor -> RepeatAction continue.
	// Fix: GetForegroundWindow() != g_hWnd ise input state'i guvenli reset et
	// (RButton/LButton/CButton stop, RepeatAction off).
	if (GetForegroundWindow() != g_hWnd)
	{
		Phoenix_MouseAuditResolveActiveLost("PROCESS_FOCUS_LOST");
		Phoenix_CancelMouseCaptureState("focus-lost", true, true, true, true);
		if (g_bRButtonDown || g_bLButtonDown)
		{
			g_bRButtonDown = FALSE;
			g_bLButtonDown = FALSE;
			if (g_pPlayer != NULL)
			{
				g_pPlayer->UnSetLockMode();
				g_pPlayer->UnSetRepeatAction();
			}
		}
		// Input event'lerini ProcessInput'tan skiple — diger update'ler
		// (UI animation, audio, network) ust caller'da (CGameUpdate::Update)
		// ayri olarak isleniyor zaten, sadece ProcessInput'tan early-return.
		return;
	}

	// ImGui DebugConsole/overlay mouse input'u DirectInput polling path'inden de
	// korunmali. Keyboard capture mouse gate'e dahil edilmez; aktif bir ImGui
	// text/key focus'u oyun alanindaki mouse hareketini sessizce yutmamali.
	const bool imguiDebugVisible = PhoenixImGui::IsDebugConsoleVisible();
	const bool imguiMouseCapture = PhoenixImGui::WantCaptureMouse();
	if (imguiDebugVisible || imguiMouseCapture)
	{
		Phoenix_MouseAuditResolveActiveUi(imguiDebugVisible ? "PROCESS_IMGUI_DEBUG_VISIBLE" : "PROCESS_IMGUI_MOUSE_CAPTURE");
		Phoenix_CancelMouseCaptureState("imgui-capture", true, true, true, true);
		g_bRButtonDown = FALSE;
		g_bLButtonDown = FALSE;
		g_bCButtonDown = FALSE;

		if (g_pPlayer != NULL)
		{
			g_pPlayer->UnSetLockMode();
			g_pPlayer->UnSetRepeatAction();
		}

		if (g_pTopView != NULL)
		{
			g_pTopView->SetSelectedNULL();
			g_pTopView->SetSelectedSectorNULL();
		}

		return;
	}

	// FPS/PING overlay DirectInput callback yerine gercek cursor koordinatiyla
	// polling path'inde de yakalanmali; aksi halde click-through olur.
	if (Phoenix_HandleFpsPingOverlayPolling(g_x, g_y))
		return;

	//-----------------------------------------------
	// Quit
	//-----------------------------------------------
	///*
	#ifdef OUTPUT_DEBUG
	if (g_pDXInput->KeyDown(DIK_LMENU) && g_pDXInput->KeyDown(DIK_X))
	{	
		//TerminateThread(g_hFileThread, 0);
		SetMode( MODE_QUIT );
		
		return;
	}	
	#endif
	//*/	


	//-----------------------------------------------
	// packet �̵� �׽�Ʈ
	//-----------------------------------------------
	#ifdef OUTPUT_DEBUG
	if (g_pDXInput->KeyDown(DIK_M) 
		&& g_pDXInput->KeyDown(DIK_LMENU))
	{			
		PacketTest();
	}
	#endif
	
	/*
	// [ TEST CODE ] DEBUG MESSAGE
	if (g_pDXInput->m_lb_down || g_pDXInput->m_lb_up)
			DEBUG_ADD("----------");

	if (g_pDXInput->m_lb_down && g_pDXInput->m_lb_up)
	{
			DEBUG_ADD("LButtonDown & Up");
	}
	else if (g_pDXInput->m_lb_down)
	{
			DEBUG_ADD("LButtonDown");
	}
	else if (g_pDXInput->m_lb_up)
	{
			DEBUG_ADD("LButtonUp");
	}
	*/

	
	//g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//�ڵ������� �Ҽ� �ִ� ����� ���� �ݺ� �׼��� Ǯ�� �ʴ´�.

//by svi
/*
#if __CONTENTS(__JAPAN_UI)
	if (!g_pUserOption->DoNotOneClickAttackOff &&
		(g_pDXInput->m_lb_down || g_pDXInput->m_rb_down))
	{
		g_bLButtonDown	= TRUE;	
		g_pPlayer->UnSetRepeatAction();
	}
#endif //__JAPAN_UI
#if __CONTENTS(__AUTO_ATTACT)
	if ((g_pDXInput->m_lb_down || g_pDXInput->m_rb_down)
		&& IsTargetCanAutoAttcak())
	{
		g_bLButtonDown	= TRUE;	
		g_pPlayer->UnSetRepeatAction();
	}
#endif //__AUTO_ATTACT
*/
#if __CONTENTS(__AUTO_ATTACT)
	if (!g_pUserOption->DoNotOneClickAttackOff &&
		(g_pDXInput->m_lb_down || g_pDXInput->m_rb_down))
	{
		g_bLButtonDown	= TRUE;	
		g_pPlayer->UnSetRepeatAction();
	}

	if ((g_pDXInput->m_lb_down || g_pDXInput->m_rb_down)
		&& IsTargetCanAutoAttcak())
	{
		g_bLButtonDown	= TRUE;	
		g_pPlayer->UnSetRepeatAction();
	}
#endif //__AUTO_ATTACT
//end svi
	//---------------------------------------------------	
	// LButton Up
	//---------------------------------------------------	
	if (g_bLButtonDown
		// L�� Up�̰ų�.. R�� Down�� ���
		&& (g_pDXInput->m_lb_up	|| g_pDXInput->m_rb_down)
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("LBUP");
		#endif

		g_bLButtonDown = FALSE;

		// ������ �������� ��� �����ϴ� mode�� �����Ѵ�.
		g_pPlayer->UnSetLockMode();
	
		if (g_pPlayer->IsRepeatAction()
// !!!!Japan�� Auto Attack�� ���� ��� �ؼ� �ȵȴ�.!!!!
// by svi
/*
#if __CONTENTS(__JAPAN_UI)
			&& g_pUserOption->DoNotOneClickAttackOff
#endif //__JAPAN_UI
#if __CONTENTS(__AUTO_ATTACT)
			&& !IsTargetCanAutoAttcak()
#endif //__AUTO_ATTACT
		*/
#if __CONTENTS(__AUTO_ATTACT)
			&& g_pUserOption->DoNotOneClickAttackOff
#endif //__AUTO_ATTACT
// end
			)
		{
			// ��ư�� �������Ƿ� �ൿ �ݺ��� ����Ѵ�.
			//�ڵ������� �Ҽ� �ִ� ����� ���� �ݺ� �׼��� Ǯ�� �ʴ´�.
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();

			#ifdef OUTPUT_DEBUG_UPDATE_LOOP
					DEBUG_ADD("plu");//[ProcessInput] Lbutton UP");
			#endif
		}
		else 
		{		
			//---------------------------------------------------	
			// item�� ��� ���� ���� ���
			//---------------------------------------------------	
			// ���õ� sector�� ���Ѵ�.
			POINT point;
			g_pPlayer->GetNextDestination( point );		
			if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
			{
				// ���� ���� �ִ� ���� ������
				g_pPlayer->GetDestination( point );
				if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
				{
					//-_-;;
				}
				else
				{
					g_pTopView->SetSelectedSector( point );
				}
			}
			else
			{
				g_pTopView->SetSelectedSector( point );
			}
		}
	}

	//---------------------------------------------------	
	// RButton Up
	//---------------------------------------------------	
	if (g_bRButtonDown
		// R�� Up�̰ų�. L�� Down�� ���
		&& (g_pDXInput->m_rb_up || g_pDXInput->m_lb_down)
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("RBUP");
		#endif

		g_bRButtonDown = FALSE;

		g_pPlayer->UnSetLockMode();
//		g_pPlayer->UnBlazeWalk2SetLockMode();

		// 2004, 8, 30, sobeit add start
		g_pPlayer->CheckRbuttonUpSkill();
		// 2004, 8, 30, sobeit add end
		{
			if (g_pPlayer->IsRepeatAction()
// !!!!Japan�� Auto Attack�� ���� ��� �ؼ� �ȵȴ�.!!!!
// by svi
/*
#if __CONTENTS(__JAPAN_UI)
				&& g_pUserOption->DoNotOneClickAttackOff
#endif //__JAPAN_UI
#if __CONTENTS(__AUTO_ATTACT)
				&& !IsTargetCanAutoAttcak()
#endif //__AUTO_ATTACT
			*/
#if __CONTENTS(__AUTO_ATTACT)
				&& g_pUserOption->DoNotOneClickAttackOff
				|| !IsTargetCanAutoAttcak()
#endif //__AUTO_ATTACT
//end
				)
			{
				// ��ư�� �������Ƿ� �ൿ �ݺ��� ����Ѵ�.
				//�ڵ������� �Ҽ� �ִ� ����� ���� �ݺ� �׼��� Ǯ�� �ʴ´�.
				g_pPlayer->UnSetRepeatAction();
				//g_pPlayer->TraceNextNULL();

				#ifdef OUTPUT_DEBUG_UPDATE_LOOP
						DEBUG_ADD("pru");//[ProcessInput] Rbutton UP");
				#endif
			}
		}
	}

	if(g_bCButtonDown && g_pDXInput->m_cb_up)
	{
		g_bCButtonDown = FALSE;
	}
	
	//---------------------------------------------------	
	// UI Input
	//---------------------------------------------------	
	POINT cursorPoint;
	GetCursorPos(&cursorPoint);
	ScreenToClient(g_hWnd, &cursorPoint);

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("UIMM");
	#endif
	gC_vs_ui.MouseControl(M_MOVING, cursorPoint.x, cursorPoint.y);
	//gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
	//if (gC_vs_ui.MouseControl(M_MOVING, g_x, g_y))
	//	return;

	// ����ȸ�� Client�� interface�� �� �׷��ٶ��� �ִ�.
	//#ifdef __EXPO_CLIENT__	
	//	if ((*g_pUserOption).DrawInterface)
	//	{
	//#endif

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("IRP");
	#endif

	
	bool bRunningParty = UI_IsRunningParty();

	if (g_pDXInput->m_lb_down)
	{
		Phoenix_MouseAuditMarkProcess(PHX_MOUSE_AUDIT_LEFT, "PROCESS_LEFTDOWN_EDGE", g_x, g_y);

#if __CONTENTS(__FRIEND_ADDITION)
		UI_ReleasePlayerPopupMenu(g_x,g_y);
#endif //__FRIEND_ADDITION		

//		#ifdef OUTPUT_DEBUG
			if (g_pDXInput->KeyDown(DIK_LSHIFT) && g_pOperatorOption->bSlideScreenShot)
			{
				g_bSlideRectSelect = true;
			}
//		#endif

		if (PhoenixImGui::WantCaptureMouse())
		{
			Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_LEFT, "LEFTDOWN_IMGUI_CAPTURE");
			Phoenix_CancelMouseCaptureState("imgui-left-down", true, false, false, true);
			return;
		}

		bool leftUiConsumed = gC_vs_ui.MouseControl(M_LEFTBUTTON_DOWN, g_x, g_y);
		bool leftPayEvent = gC_vs_ui.IsRunningPayEvent();
		if (leftUiConsumed || leftPayEvent)
		{
			Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_LEFT, leftUiConsumed ? "LEFTDOWN_VS_UI_CONSUMED" : "LEFTDOWN_PAY_EVENT");

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI LButton Down return");
			#endif

			return;				
		}

		Phoenix_RecordPendingGroundClick(g_x, g_y);
		Phoenix_MouseAuditMarkPending(PHX_MOUSE_AUDIT_LEFT, "LEFTDOWN_NOT_UI_PENDING_FOR_GAME_PATH", g_x, g_y);
	}

	if (g_pDXInput->m_lb_up)
	{
		#ifdef OUTPUT_DEBUG
			g_bSlideRectSelect = false;			
		#endif

		if (PhoenixImGui::WantCaptureMouse())
		{
			Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_LEFT, "LEFTUP_IMGUI_CAPTURE");
			Phoenix_CancelMouseCaptureState("imgui-left-up", true, false, false, true);
			return;
		}

		if (gC_vs_ui.MouseControl(M_LEFTBUTTON_UP, g_x, g_y))
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI LButton Up return");
			#endif

			return;
		}
	}

	if (g_pDXInput->m_rb_down)				//���콺 ������ ��ư üũ
	{
		Phoenix_MouseAuditMarkProcess(PHX_MOUSE_AUDIT_RIGHT, "PROCESS_RIGHTDOWN_EDGE", g_x, g_y);

#if __CONTENTS(__FRIEND_ADDITION)
		UI_ReleasePlayerPopupMenu(g_x,g_y);
#endif //__FRIEND_ADDITION		

#if __CONTENTS(__GEAR_SWAP_CHANGE)			// �������� ��ü ���϶����� �ൿ�� �ߴ��Ѵ�.
		if(g_pPlayer)
		{
			bool	bGearChange	= true;
			switch(g_pPlayer->GetRace())
			{
			case RACE_SLAYER:
				bGearChange		= g_pSlayerGear->GetProcessingGearChange();
				break;	
					
			case RACE_VAMPIRE:
				bGearChange		= g_pVampireGear->GetProcessingGearChange();
				break;
						
			case RACE_OUSTERS:
				bGearChange		= g_pOustersGear->GetProcessingGearChange();
				break;
			}
			if(!bGearChange)
			{
				if (PhoenixImGui::WantCaptureMouse())
				{
					Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_RIGHT, "RIGHTDOWN_IMGUI_CAPTURE");
					Phoenix_CancelMouseCaptureState("imgui-right-down", false, true, false, true);
					return;
				}

				bool rightUiConsumed = gC_vs_ui.MouseControl(M_RIGHTBUTTON_DOWN, g_x, g_y);
				if (rightUiConsumed
					&& !bRunningParty)
				{
					Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_RIGHT, "RIGHTDOWN_VS_UI_CONSUMED");

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
							DEBUG_ADD("[ProcessInput] UI RButton Down return");
					#endif

					return;
				}
			}
			else
			{
				Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_RIGHT, "RIGHTDOWN_GEAR_CHANGE_BUSY");
				return;
			}
		}
#else
		if (PhoenixImGui::WantCaptureMouse())
		{
			Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_RIGHT, "RIGHTDOWN_IMGUI_CAPTURE");
			Phoenix_CancelMouseCaptureState("imgui-right-down", false, true, false, true);
			return;
		}

		bool rightUiConsumed = gC_vs_ui.MouseControl(M_RIGHTBUTTON_DOWN, g_x, g_y);
		if (rightUiConsumed
			&& !bRunningParty)
		{
			Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_RIGHT, "RIGHTDOWN_VS_UI_CONSUMED");

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI RButton Down return");
			#endif

			return;
		}

#endif // __GEAR_SWAP_CHANGE
	}
	
//�׽�Ʈ �ڵ�
#if __CONTENTS(__AUTO_ITEM_ROOTING)
	else
	{
		if(g_pZone != NULL)
		{
			if(strstr(g_pPlayer->arrchRootingTest, "*command autoItemRooting"))
			{
				int iRandom_X	= (rand() % 150)+ 1;
				int iRandom_Y	= (rand() % 150)+ 1;

				if(g_CurrentFrame%24 == 0)
				{
					g_pDXInput->m_rb_down	= TRUE;
					g_x	+= iRandom_X;
					g_y	+= iRandom_Y;
					gC_vs_ui.MouseControl(M_RIGHTBUTTON_DOWN, g_x, g_y);
				}
			}
		}
	}
#endif // __AUTO_ITEM_ROOTING

	if (g_pDXInput->m_rb_up)
	{
		if (PhoenixImGui::WantCaptureMouse())
		{
			Phoenix_MouseAuditResolveUi(PHX_MOUSE_AUDIT_RIGHT, "RIGHTUP_IMGUI_CAPTURE");
			Phoenix_CancelMouseCaptureState("imgui-right-up", false, true, false, true);
			return;
		}

		if (gC_vs_ui.MouseControl(M_RIGHTBUTTON_UP, g_x, g_y)
			&& !bRunningParty)
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("[ProcessInput] UI RButton Up return");
			#endif

			return;
		}
	}	
//�׽�Ʈ �ڵ�
#if __CONTENTS(__AUTO_ITEM_ROOTING)
	else
	{
		if(g_pZone != NULL)
		{
//			int iRandom_X	= (rand() % 200) + 1;
//			int iRandom_Y	= (rand() % 200) + 1;

			if(strstr(g_pPlayer->arrchRootingTest, "*command autoItemRooting"))
			{
//				g_pDXInput->m_rb_up	= TRUE;
//				gC_vs_ui.MouseControl(M_RIGHTBUTTON_UP, g_x, g_y);
			}
		}
	}
#endif // __AUTO_ITEM_ROOTING

	if( g_pDXInput->m_cb_up)
	{
		if( gC_vs_ui.MouseControl(M_CENTERBUTTON_UP, g_x,g_y)
			&& !bRunningParty)
		{
			return;
		}
	}

	if( g_pDXInput->m_cb_down)
	{
		if( gC_vs_ui.MouseControl( M_CENTERBUTTON_DOWN, g_x,g_y)
			&& !bRunningParty)
		{
			return;
		}
	}
	
	//#ifdef OUTPUT_DEBUG
	// ä��
//		if (gC_vs_ui.ChatMouseControlExtra( M_MOVING, g_x, g_y ))
//			return;
	//#endif

	// 2000.7.8. KJTINC
	/*
	static bool bl_ctrl;
	if (g_pDXInput->KeyDown(DIK_LCONTROL) ||
		g_pDXInput->KeyDown(DIK_RCONTROL))
	{
		bl_ctrl = true;
	}
	//if (g_pDXInput->KeyUp(DIK_LCONTROL) &&
	//	 g_pDXInput->KeyUp(DIK_RCONTROL))
	//{
	//	bl_ctrl = false;
	//}
	if (g_pDXInput->KeyDown(DIK_SPACE) && bl_ctrl == true)
	{
		gC_vs_ui.PressCtrlSpace();
	}*/


	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("CINL");
	#endif

	//---------------------------------------------------	
	// ItemName List ���?
	//---------------------------------------------------	
	if ((g_pDXInput->KeyDown(DIK_LMENU) || g_pDXInput->KeyDown(DIK_RMENU) || g_pDXInput->KeyDown(DIK_RCONTROL))
		&& !g_pDXInput->KeyDown(DIK_LSHIFT)
		&& g_pTradeManager==NULL && g_pPlayer->GetWaitVerify()!=MPlayer::WAIT_VERIFY_TRADE)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("DINL");
		#endif

		g_pTopView->SetDrawItemNameList();

		if (!g_bWatchMode 
			// ���� �ݺ��ൿ�� �ϰ� ���� ���� ���
			&& !g_pPlayer->IsRepeatAction()
			// ���㳪 ���� , ����Ʈ �� �ƴ� ���
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_VAMPIRE_GHOST
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF
#if __CONTENTS(__FAST_TRANSFORTER)
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_FLITTERMOUSE
	      	&& g_pPlayer->GetCreatureType()!=CREATURETYPE_NEDE
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_KLTL
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
			&& g_pPlayer->GetCreatureType()!=CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
			)
		{
			g_pTopView->UnSetRequestMode();
			g_pTopView->SetSelectedSectorNULL();					

			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
			{					
				if (g_pDXInput->KeyDown(DIK_LMENU))
				{
					g_pTopView->SetRequestMode( MTopView::REQUEST_TRADE );
				}
				else if (g_pDXInput->KeyDown(DIK_RMENU)|| g_pDXInput->KeyDown(DIK_PGUP))
				{
					if (!gC_vs_ui.IsRunningRequestParty()
						&& g_pSystemAvailableManager->IsAvailablePartySystem() )
					{
						g_pTopView->SetRequestMode( MTopView::REQUEST_PARTY );
					}
				}
				else if (g_pDXInput->KeyDown(DIK_RCONTROL))
				{
					g_pTopView->SetRequestMode( MTopView::REQUEST_INFO );
				}

			}
		}
	}
	else
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("NDINL");
		#endif

		g_pTopView->UnSetDrawItemNameList();
		g_pTopView->UnSetRequestMode();		
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("PMC");
	#endif

		
	//---------------------------------------------------	
	// ��Ƽ ���µ� ��Ƽâ�� �� �ִ� ���..
	// ��Ƽ ��û ���.
	//---------------------------------------------------	
//	if (g_pParty!=NULL 
//		&& g_pParty->GetSize()==0
//		&& UI_IsRunningParty()
//		&& !gC_vs_ui.IsRunningRequestParty()
//		&& g_pTempInformation->Mode==TempInformation::MODE_NULL
//		&& !g_bWatchMode 
//		// ���� �ݺ��ൿ�� �ϰ� ���� ���� ���
//		&& !g_pPlayer->IsRepeatAction()
//		// ���㳪 ���밡 �ƴ� ���
//		&& g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
//		&& g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF)
//	{
//		g_pTopView->SetSelectedSectorNULL();
//		g_pTopView->SetRequestMode( MTopView::REQUEST_PARTY );
//	}

	//---------------------------------------------------	
	//
	//		�Է��� ���ѵǴ� ���..
	//
	//---------------------------------------------------	
	// - ���� ���ϰ� �ϴ� dialog�� �� �ִ� ���
	//
	// - Player�� server�κ��� ������ ��ٸ��� ����
	//
	// - �ݺ� action�߿��� �� �Է��� �ʿ����.(����)
	//
	// - UI���� �Է��� �ް� �ִ� ���
	//
	//---------------------------------------------------	
	if (g_pPlayer->IsWaitVerify() 
		|| gC_vs_ui.IsInstallMineProgress()		// ���� ����� ���̸�..
		//|| gC_vs_ui.IsCreateMineProgress()	// ���� ����� ���̸�..
		//|| gC_vs_ui.IsCreateBombProgress()	// ���� ����� ���̸�..
		|| gC_vs_ui.IsSkillCastingProgress()	// ��ų ĳ���� ���̸�..
		|| g_pPlayer->IsFastMove()	// ���� �����̴� ���
		// 2004, 12, 3, ���ξ� �߰�
//		|| g_pPlayer->CurPernalShop() == 1  || gC_vs_ui.inventory_mode == 2
//		|| gC_vs_ui.IsRunningPersnalShop()
		// 2004, 12, 3, ���ξ� �߰�
		|| UI_IsRunning_WebBrowser()
		)	
	{
		Phoenix_MouseAuditResolveActiveLost("PRE_GAME_WAITVERIFY_FASTMOVE_SKILLCAST_OR_BROWSER");

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD_FORMAT("[ProcessInput] Is WaitVerify Return. %d %d", g_pPlayer->IsWaitVerify(), g_pPlayer->IsFastMove());
		#endif

		return;
	}

	// ���� ����Ʈ���� ĳ���Ϳ� ���� ������ ������ ����
	if (g_pPlayer->IsRepeatAction())
	{
		Phoenix_MouseAuditResolveActiveLost("PRE_GAME_REPEAT_ACTION");
		g_pTopView->SetSelectedSectorNULL();

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("[ProcessInput] Is RepeatAction return");
		#endif

		return;
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("CUILock");
	#endif

	//---------------------------------------------------	
	// UI���� mouseĿ�� �Է��� ��� �ִ� ���
	// elevator �۵���..
	//---------------------------------------------------	
	if ((g_pUIDialog->IsLockInput() || g_bUIInput)
		
		// ��Ƽ ���������� ��Ƽâ�� ĳ���� ������ ���� �ִ�.
		&& !bRunningParty

		// �ٸ� ���� ���ϵ��� ...
		|| gC_vs_ui.IsRunningTraceWindow()
		|| g_pUIDialog->IsRunningPCTalkDlg()
		|| gC_vs_ui.IsRunningElevator()
		|| gC_vs_ui.IsRunningXmasCardWindow()
		|| UI_IsRunningSelectWayPoint())
	{
		Phoenix_MouseAuditResolveActiveUi("PRE_GAME_MODAL_UI_LOCK");
		g_pTopView->SetSelectedNULL();
			
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD_FORMAT("[ProcessInput] UI return. %d %d %d", g_pUIDialog->IsLockInput(), g_bUIInput, gC_vs_ui.IsRunningElevator());
		#endif

		return;
	}

	
	
	//---------------------------------------------------	
	//
	// Player�� ���� ���... 
	//
	//---------------------------------------------------		
	if (g_pPlayer->IsDead())
	{
		Phoenix_MouseAuditResolveActiveLost("PRE_GAME_PLAYER_DEAD");
		g_pTopView->SetSelectedNULL();
		g_pTopView->SetSelectedSectorNULL();

		//---------------------------------------------------	
		// delay�ð��� ������ ����
		// ������ ������ �ٽ� ��Ƴ��� �Ѵ�.
		//---------------------------------------------------	
		//if (g_pPlayer->IsNotDeadDelay() && g_pDXInput->KeyDown( DIK_SPACE ))
		//{
		//	g_pPlayer->SetAlive();
		//	g_pTopView->SetFadeEnd();
		//}
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("[ProcessInput] Player Dead return.");
		#endif

		return;
	}
 


	// Mouse�� ���ؼ� �ٶ󺸴� player...
	//POINT temp = g_pTopView->ScreenToPixel(g_x, g_y);
	//temp = MTopView::PixelToMap( temp.x, temp.y );
	//g_pPlayer->SetDirectionToPosition(temp.x, temp.y);

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("FA");
	#endif

	//---------------------------------------------------	
	//
	// ���� mouse�� ��ġ�� �ִ� object�� ���ؼ� check
	//
	//---------------------------------------------------	
	// ���������̰ų� SmallZone�� �ְų�
	// Hallucination�� �ɷ����� ��~ ����
	//---------------------------------------------------	
	

	// ���� ������ �����Ǵ� ���
	bool bForceAttack = (m_bForceAttack
						|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION)
						||g_pDXInput->KeyDown(DIK_LSHIFT));
	
		if (bForceAttack
			//|| !g_bZonePlayerInLarge
			|| g_bZoneSafe
			|| g_pPlayer->IsInSafeSector()
			|| g_pPlayer->IsSlayer() && g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL && (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetUser() == FLAG_ACTIONINFO_USER_SLAYER
			|| g_pPlayer->IsVampire() && g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL && (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetUser() == FLAG_ACTIONINFO_USER_VAMPIRE
			|| g_pPlayer->IsOusters() && g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL && (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetUser() == FLAG_ACTIONINFO_USER_OUSTERS
			// 2004, 11, 26, sobeit add start - ���� 140 ��æ ��ų - �������� �ູ, ������ ����..���� 
			|| g_pPlayer->GetSpecialActionInfo() == SKILL_INTIMATE_GRAIL
						|| g_pPlayer->GetSpecialActionInfo() == SKILL_INTIMATE_GRAIL_2
												|| g_pPlayer->GetSpecialActionInfo() == SKILL_FAVOR_OF_LUNA
			// 2004, 11, 26, sobeit add end
			)
		{
			g_pObjectSelector->SelectAll();				
		}
	
	//---------------------------------------------------	
	// L-Control������ �츮���� ����
	//---------------------------------------------------	
	else if (g_pDXInput->KeyDown(DIK_LCONTROL)
		|| g_pTopView->IsRequestMode())
	{
		g_pObjectSelector->SelectFriend();		
	}
	//---------------------------------------------------	
	// �ƴϸ�.. ���� ����
	//---------------------------------------------------	
	else
	{
		// Notice Event üũ�ؼ� ...
		// ���� �����ϱ� ���� �������̶��!!
		// 1. ������ �����ϴ� ����� ��� �ڱ� ��� �ƴϸ� �� ��!
		// 2. ���� ��û�� �� ����ϰ�� �츮 ��� �ƴϸ� �� ��!
		// 3. ��尣 ���� ��û �ܿ� �ٸ�����ϰ�� �� �� ���� ���� �� ���� ��!
		
		g_pObjectSelector->SelectEnemy();		
	}

	//---------------------------------------------------	
	// �̰� �ܿ���..
	//
	// g_pObjectSelector->SelectByRace()����
	// g_pObjectSelector->SelectByGuild()������ �����ؾ� �Ѵ�.
	//
	// ȭ�鿡 �� Icon�� ������°� ���� ��.. Race/Guild
	//
	// ��, ���ݸ�忡 ���� Peace/Attack/Normal Icon��
	// �־�� �Ѵ�.
	//---------------------------------------------------	
	// Slayer�� ���� ������ ���� ����
	//---------------------------------------------------	
	if (g_pPlayer->IsSlayer() || g_pPlayer->IsOusters() )
	{
		g_pObjectSelector->SelectByRace();
	}
	//---------------------------------------------------	
	// Vampire�� ���� Guild�� ���� ����
	//---------------------------------------------------	
	else
	{
		g_pObjectSelector->SelectByGuild();
	}


	//MObject*	pObject = g_pTopView->GetSelectedObjectSprite(g_x, g_y);
	MObject*	pObject = NULL;
	
	
	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("PMF");
	#endif

	int partyMember = (bRunningParty? gC_vs_ui.GetPartyManagerFocused() : -1) - 1;

	//---------------------------------------------------	
	// ��ƼUI�� �����ϴ� ���..
	//---------------------------------------------------	
	if (partyMember >= 0
		&& partyMember < g_pParty->GetSize())
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("SelP");
		#endif

		PARTY_INFO* pInfo = g_pParty->GetMemberInfo( partyMember );

		if (pInfo!=NULL)
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("SPM");
			#endif

			// ���.. terrible.. - -;
			// MZone�� Name���� id�˻��� �� �ְ� map�� �߰��ؾ��Ѵ�.
			// �׸���.. �ٷ� Name --> MCreature* �� �� �� �ְ� �ؾ��Ѵ�.. ����? - -;
			pObject = g_pZone->GetCreature( g_pZone->GetCreatureID( pInfo->Name.GetString(), 1 ) );

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("SPMk");
			#endif
		}
	}
	//---------------------------------------------------	
	// ������ ����..
	//---------------------------------------------------	
	else
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("selZ");
		#endif

		//---------------------------------------------------
		// �ƿ� �� UI�� focus �� ������ �� return�ؾ� �Ѵ�.
		// inventory���� ������ ����ߴµ� ��� ���� ���� �־.
		//---------------------------------------------------
		if (partyMember==-2		// partyâ�� focus �Ȱ� �ƴϰ�
			&& g_bUIInput)		// UI�� �Է��� �ִٸ�..
		{
			Phoenix_MouseAuditResolveActiveUi("PRE_GAME_PARTY_OR_UI_FOCUS");

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selZk1");
			#endif

			return;
		}

		//---------------------------------------------------	
		// ItemName�� �����ϴ� ����..
		//---------------------------------------------------	
		if (g_pTopView->IsDrawItemNameList())
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("drawItemNL");
			#endif

			MItem* pItem = g_pTopView->SelectItemName(g_x, g_y);

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selIN");
			#endif

			if (pItem==NULL)
			{
				pObject = NULL;
			}
			else
			{
				pObject = (MObject*)pItem;
			}

			//---------------------------------------------------	
			// ItemName�� ���� �ȵ����� Object����
			//---------------------------------------------------	
			if (pObject==NULL)
			{
				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("selObj");
				#endif

				pObject = g_pTopView->GetSelectedObject(g_x, g_y);

				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("selObjK");
				#endif
			}
		}
		//---------------------------------------------------	
		// �׳� Object ����
		//---------------------------------------------------	
		else
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selObj2");
			#endif

			pObject = g_pTopView->GetSelectedObject(g_x, g_y);

			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("selObj2K");
			#endif
		}

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("selZk");
		#endif
	}

//	if (g_pPlayer->IsRepeatAction())
//	{
//		g_pTopView->SetSelectedSectorNULL();
//		
//#ifdef OUTPUT_DEBUG_PROCESS_INPUT
//		DEBUG_ADD("[ProcessInput] Is RepeatAction return");
//#endif
//		
//		return;
//	}
	
	//---------------------------------------------------		
	// Watch mode
	//---------------------------------------------------		
	if (g_bWatchMode)
	{
		Phoenix_MouseAuditResolveActiveLost("PRE_GAME_WATCH_MODE");
		int cx = 0, cy = 0;

		if (g_x < 10)		cx = -1;
		else if (g_x > 790)	cx = 1;
		if (g_y < 10)		cy = -1;
		else if (g_y > 590)	cy = 1;

		int x = g_pPlayer->GetX();
		int y = g_pPlayer->GetY();

		x += cx;
		y += cy;

		if (x < 0) x = 0;
		else if (x >= g_pZone->GetWidth()) x = g_pZone->GetWidth()-1;
		if (y < 0) y = 0;
		else if (y >= g_pZone->GetHeight()) y = g_pZone->GetHeight()-1;

		g_pPlayer->SetPosition( x, y );

		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("[ProcessInput] Watch Mode");
		#endif

		return;
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("ckL");
	#endif
		

	static bool bDownPress = false;

	//---------------------------------------------------	
	// Lock Mode üũ
	//---------------------------------------------------	

	if( g_pDXInput->KeyDown(DIK_CAPITAL)) {
		
		if( !g_pPlayer->IsInCasket()			&&
			!g_pPlayer->IsUndergroundCreature())
		{
			if (g_pDXInput->m_lb_down)
			{		
				g_bLButtonDown = TRUE;			
			}
			else if (g_pDXInput->m_rb_down)
			{
				g_bRButtonDown = TRUE;
			}
			else if (g_pDXInput->m_cb_down)
			{
				g_bCButtonDown = TRUE;
			}

			if (bDownPress == false) {

				g_pPlayer->SetLockMode();
				// ������ ���� Ŀ���� �ٲ��.
				gpC_mouse_pointer->SetCursorAttack();
			
				bDownPress = true;
				// Lock Mode������ ���õ� sector�� ���ش�.
				g_pTopView->SetSelectedSectorNULL();
			}
			
		}
		
	}
	else {

		bDownPress = false;

		// �ƹ��͵� ������ ���� ���¿��� 
		// LockMode�̸� LockMode �����ؾ� �Ѵ�.
		if (!g_bLButtonDown && !g_bRButtonDown
#ifdef __METROTECH_TEST__
			&& !g_bCButtonDown
#endif
			)
			g_pPlayer->UnSetLockMode();
	}

///////////////blaze walk2 lock////////////////
	if (g_pDXInput->m_rb_down)
	{
		g_bRButtonDown = TRUE;
		if (bDownPress == false) 
		{
			g_pPlayer->SetBlazeWalk2LockMode();
			bDownPress = true;
		}
	}
	else
	{
		bDownPress = false;
		if(!g_bRButtonDown)
			g_pPlayer->UnBlazeWalk2SetLockMode();
	}
	//---------------------------------------------------	
	// Lock Mode�� �ƴ� ��츸 
	// �Ϲ����� �Է��� �޾Ƶ鿩 �ൿ�� ���Ѵ�.
	//---------------------------------------------------	
	if (!g_pPlayer->IsLockMode())
	{	
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("notLock");
#endif
		
		//---------------------------------------------------	
		// Mouse ��ġ�� Object�� ������..
		//---------------------------------------------------	
		if (pObject==NULL)
		{
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("noObj");
#endif
			
			// ���õ� �� ���� �Ѵ�.
			g_pTopView->SetSelectedNULL();
			
			//---------------------------------------------------	
			// mouse pointer ����
			//---------------------------------------------------	
			//if (!g_bMouseInPortal)
			{
				if (g_pPlayer->OnAttacking() 
					|| g_pCGameUpdate->GetForceAttack()
					||g_pDXInput->KeyDown(DIK_LSHIFT))
				{
					gpC_mouse_pointer->SetCursorAttack();
				}
				else
				{
					if (g_MouseSector.x >= 0 
						&& g_MouseSector.y >= 0
						&& g_MouseSector.x < g_pZone->GetWidth() 
						&& g_MouseSector.y < g_pZone->GetHeight())
					{
						        // --- BURAYA EKLENECEK DEBUG LOG ---
							CT_BAT("MouseClick_Debug: CharID=%d, IsFlying=%d, MoveType=%d, Sector=(%d, %d)",
								   g_pPlayer->GetID(),
								   g_pPlayer->IsFlyingCreature(),
								   g_pPlayer->GetMoveType(),
								   g_MouseSector.x,
								   g_MouseSector.y);
							// ----------------------------------
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD_FORMAT("getSec(%d, %d)", g_MouseSector.x, g_MouseSector.y);
#endif
						
						const MSector& sector = g_pZone->GetSector(g_MouseSector.x, g_MouseSector.y);
						
						//---------------------------------------------------
						// �� �� ���� ���̸�
						//---------------------------------------------------
						if (g_pPlayer->IsGroundCreature() && sector.IsBlockGround()
							|| g_pPlayer->IsUndergroundCreature() && sector.IsBlockUnderground()
							|| g_pPlayer->IsFlyingCreature() && sector.IsBlockFlying())
							//g_pZone->CanMove(g_pPlayer->GetMoveType(), g_MouseSector.x, g_MouseSector.y))
						{	
							// ��Ż�� �ƴϰ�
							if (!g_bMouseInPortal
								// UI�� �Է��� ���� ��� --> ���ӿ� Ŀ���� �ִ� ���
								&& !g_bUIInput
								// ���õ� ĳ���Ͱ� ���� ���
								&& g_pTopView->GetSelectedCreature()==OBJECTID_NULL)
							{
								// �� �� ���� Ŀ���� ǥ��
								gpC_mouse_pointer->SetCursorNotMove();					
							}
						}
						//---------------------------------------------------
						// �� �� �ִ� ���̸�..
						//---------------------------------------------------
						else
						{
							if (!g_bMouseInPortal)
							{
								gpC_mouse_pointer->SetCursorDefault();
							}
						}
					}
					else
					{
						if (!g_bMouseInPortal)
						{
							gpC_mouse_pointer->SetCursorDefault();
						}
					}
				}
			}
			
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("noObjk");
#endif
		}
		//---------------------------------------------------	
		//
		// Mouse ��ġ�� Object�� �ִ� ���
		//
		//---------------------------------------------------	
		else
		{
			BYTE objectType = pObject->GetObjectType();
			
#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD_FORMAT("obj(%d)", objectType);
#endif
			
			//------------------------------------------------
			// Creature ���� mouse�� �ִ� ���
			//------------------------------------------------
			if (objectType==MObject::TYPE_CREATURE)
			{
				MCreature* pCreature = (MCreature*)pObject;

				g_pTopView->SetSelectedCreature( pCreature->GetID() );
					
				if (!g_bMouseInPortal 
						
					// ������ ���� ������ �� �ְų�
					// ���� �����̰ų�..
					//&& (g_pPlayer->CanAttackTribe( pCreature ) || bForceAttack)
					&& g_pObjectSelector->CanAttack( pCreature ) )
					// �����̾��� ��쿡�� Guild�� ���� ������ �� �ְų�..	
						
				{
					if (pCreature->IsNPC())
					{
						gpC_mouse_pointer->SetCursorDefault();
					}
					else
					{
						gpC_mouse_pointer->SetCursorAttack();
					}
				}
			
					
			}
			
			//------------------------------------------------
			// Item ���� mouse�� �ִ� ���
			//------------------------------------------------
			else if (pObject->GetObjectType()==MObject::TYPE_ITEM)
			{
				MItem* pItem = (MItem*)pObject;
				MCorpse* pCorpse = (MCorpse*) pItem;
				MCreature* pCreature = pCorpse->GetCreature();
				
				if(g_pZone != NULL && pItem != NULL )
				{
					// �����̾�� �ȵǰ�
					// �����̾�� creature ã�Ƽ� �����ΰ�츸

					if(!g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() || 
						g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() && g_pPlayer->IsVampire() && g_pZone->GetID() != 3001||
						g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() && !g_pPlayer->IsVampire() && 
						g_pPlayer->HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) && g_pZone->GetID() != 3001						
						|| g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
						|| g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)

#ifdef __METROTECH_TEST__
						|| g_bLight
#endif
						)// &&
						//pCreature != NULL && pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549)
					{
						g_pTopView->SetSelectedItem( pItem->GetID() );			
						
						COLORREF color;
						//------------------------------------------------
						// option�� ���� ����
						//------------------------------------------------
#if __CONTENTS(__TUNING_ITEM)
						if (pItem->IsTuningItem())
						{
							WORD temp_color = CIndexSprite::ColorSet[MItem::GetTuningItemColorSet()][8];
							color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
						}
						else
#endif //__TUNING_ITEM
						if (pItem->IsSpecialColorItem() )
						{
							WORD temp_color = CIndexSprite::ColorSet[pItem->GetSpecialColorItemColorset()][16];
							color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
						}
						else if (pItem->IsNameColorCycleItem())
						{
							color = pItem->GetNameColorCycle();
						}
						else if (pItem->IsItemNameColorChange())
						{
							WORD temp_color = CIndexSprite::ColorSet[pItem->GetItemNameColorSet()][15];
							color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
						}
						else if((*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()].DropItemNameTag)
						{
							color = (*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()].DropItemNameTag;
						}
						else if (pItem->IsEmptyItemOptionList())
						{
							color = g_pClientConfig->COLOR_NAME_ITEM;
						}
						else if(pItem->GetItemOptionListCount() > 1)
						{
							color = g_pClientConfig->COLOR_NAME_ITEM_RARE_OPTION;
						}
						else
						{
							color = g_pClientConfig->COLOR_NAME_ITEM_OPTION;
						}
						
						//------------------------------------------------
						// item���� Ŀ��
						//------------------------------------------------
						if (!g_bMouseInPortal&&!g_pPlayer->IsInDarkness())
						{						
							// ��ü�� ���� '��ü'��� ǥ�� ���Ѵ�.
							if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
							{
								gpC_mouse_pointer->SetCursorPickUp( "", color );
							}
							// ���� �������� �̸� ǥ���Ѵ�.
							else
							{
								char str[80];
								strcpy(str, pItem->GetName());
								
								gpC_mouse_pointer->SetCursorPickUp( str, color );
							}
						}
					} 
				}
			}

			//------------------------------------------------
			// Effect ���� mouse�� �ִ� ���
			//------------------------------------------------
			else if (pObject->GetObjectType()==MObject::TYPE_EFFECT)
			{
				g_pTopView->SetSelectedEffect( pObject->GetID() );
			}

			//------------------------------------------------
			// InteractionObject ���� mouse�� �ִ� ���
			//------------------------------------------------
//			else if (pObject->GetObjectType()==MObject::TYPE_INTERACTIONOBJECT)
//			{
//				g_pTopView->SetSelectedInteractionObject( pObject->GetID() );
//			}
		}

		//---------------------------------------------------	
		//
		// player�� ��� ��� delay �ð��� ������ ���� �����ϴ�.
		//
		//---------------------------------------------------	
		if (!g_pPlayer->IsNotDelay() && !g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE))
		{
			if (Phoenix_TryImmediateGroundRetarget(pObject,
				"LEFT_GAME_MOVE_RETARGET_ACTION_DELAY",
				"LEFT_MOVE_RETARGET_ACTION_DELAY_FAILED"))
			{
				return;
			}
		}

		if (g_pPlayer->IsNotDelay() && !g_pPlayer->HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE ) )
		{
			#ifdef OUTPUT_DEBUG_PROCESS_INPUT
				DEBUG_ADD("noDel");
			#endif

			//-----------------------------------------------
			// Shift + L/R ButtonDown : ���� Tile ����
			//-----------------------------------------------
			if (g_pDXInput->KeyDown(DIK_LSHIFT))
			{
				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("shiAt");
				#endif
				//-----------------------------------------------
				// Shift + LButtonDown : ���� ����
				//-----------------------------------------------
				if (g_pDXInput->m_lb_down
					// ���� �ִ� ��찡 �ƴҶ�
					&& !g_pPlayer->IsInCasket()
					&& !g_pPlayer->IsUndergroundCreature()
					&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)
				)
				{
					g_pPlayer->UnSetRequestMode();

					//-----------------------------------------------
					// ĳ���͸� ���� ����..
					//-----------------------------------------------
					if (pObject!=NULL)
					{
						if (pObject->GetObjectType()==MObject::TYPE_CREATURE)
						{
							// Object�� ���� ���õ��� ��쿡�� 
							// LButton�� ������ �ִ� �͸����� �̵��� �����ʰ� �Ѵ�.
							MCreature *pCreature = dynamic_cast<MCreature*>(pObject);
							g_bLButtonDown = TRUE;

							if(pCreature && g_pObjectSelector->CanForceAttack(pCreature))
							{							
								BOOL	ForceAttack = TRUE; 

								if (g_pPlayer->TraceCreatureToBasicAction( 
											pCreature->GetID(), 
											true))		// ���� ����
								{
									//----------------------------------
									// ������ �ϴ� �⺻ �ൿ �ݺ� ����
									//----------------------------------
									g_pPlayer->SetRepeatAction(); 
									g_bPreviousMove = false;

									// ���� �޺� ���� �ʱ�ȭ Sjheon	2005.07.04 Add 
									//g_pPlayer->SetCombo(1)	;
									// ���� �޺� ���� �ʱ�ȭ Sjheon	2005.07.04 End

								}
								// ������ ǥ�ø� ���ش�.
								g_pTopView->SetSelectedSectorNULL();
							}
						}
					}
					//-----------------------------------------------
					// ���� Tile ����
					//-----------------------------------------------
					//g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
					
					// ���õ� Sector�� ǥ���Ѵ�.
					//g_pTopView->SetSelectedSector( g_SelectSector );

					//g_pPlayer->TraceSectorToBasicAction( g_SelectSector.x, g_SelectSector.y );			

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("shiKK");
					#endif
				}		
				//-----------------------------------------------
				// Shift + RButtonDown : ���� Tile ��� ����
				//-----------------------------------------------
				else if (g_pDXInput->m_rb_down)
				{
					g_pPlayer->UnSetRequestMode();

// 					BYTE_ATTRIBUTE actionTarget(FLAG_ACTIONINFO_SELECT_ALL);
// 					TYPE_ACTIONINFO actionInfoIdx = g_pPlayer->GetSpecialActionInfo();
// 					if(actionInfoIdx != ACTIONINFO_NULL)
// 					{
// 						MActionInfo& actionInfo = (*g_pActionInfoTable)[actionInfoIdx];
// 						actionTarget = actionInfo.GetActionTarget();
// 					}
// 					MObject* pTargetObject = g_pTopView->GetSelectedObject(g_x, g_y, actionTarget);
// 
// 					ProcessInputRButtonDown(pTargetObject, true);
// 
// 					if(pTargetObject)
// 						g_pTopView->SetSelectedCreature(pTargetObject->GetID());

					ProcessInputRButtonDown(pObject, true);

					g_bPreviousMove = false;
					/*
					g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

					// ���õ� Sector�� ǥ���Ѵ�.
					g_pTopView->SetSelectedSector( g_SelectSector );

					if (g_pPlayer->TraceSectorToSpecialAction( g_SelectSector.x, g_SelectSector.y ))
					{
						//g_pPlayer->SetRepeatAction();
					}


					// ������ ǥ�ø� ���ش�.
					g_pTopView->SetSelectedSectorNULL();
					*/

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("shiRK");
					#endif
				}
				
			}

			//---------------------------------------------------------------
			// L/R ButtonDown only
			//---------------------------------------------------------------
			else
			{
				
				#ifdef OUTPUT_DEBUG_PROCESS_INPUT
					DEBUG_ADD("LRonly");
				#endif
				//---------------------------------------------------------------
				//
				//				LButtonDown
				//
				//---------------------------------------------------------------
				bool consumedPendingGroundClick = false;
				if (!g_pDXInput->m_lb_down && !g_pDXInput->m_rb_down)
				{
					consumedPendingGroundClick = Phoenix_TryConsumePendingGroundClick();
				}

				if (!consumedPendingGroundClick && g_pDXInput->m_lb_down)
				{
					DEBUG_ADD_FORMAT("[NPC-CLICK] LBdown raw: InCasket=%d Underground=%d PShop=%d RunPShop=%d",
						(int)g_pPlayer->IsInCasket(), (int)g_pPlayer->IsUndergroundCreature(),
						(int)g_pPlayer->CurPernalShop(), (int)gC_vs_ui.IsRunningPersnalShop());
				}
				if (!consumedPendingGroundClick && g_pDXInput->m_lb_down
					// burrow ���°� �ƴϾ�� �Ѵ�.
					&& !g_pPlayer->IsInCasket()
					&& !g_pPlayer->IsUndergroundCreature()
					&& g_pPlayer->CurPernalShop() != 2
					&& g_pPlayer->CurPernalShop() != 1
					&& gC_vs_ui.IsRunningPersnalShop() == false
					)
				{
					CT_Done("[NPC-CLICK] LBdown ACCEPTED -> processing");
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("Lon");
					#endif

					g_pPlayer->UnSetRequestMode();

					g_bLButtonDown = TRUE;
					
					// ���õ� sector�� ���� �Ѵ�.
					g_pTopView->SetSelectedSectorNULL();

					// sector �����ϱ�
					g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
					//g_pTopView->SetSelectedSector(g_SelectSector.x, g_SelectSector.y);

					//
					//MObject*	pObject = g_pTopView->GetSelectedObject(g_x, g_y);

					// �ν��� �ͷ��϶�.. ����� lbuttton ó�� �ʴ´�.. lbutton = ������ �ͷ� ����
					if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
					{		
						Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_LEFT, "LEFT_GAME_INSTALL_TURRET_BRANCH", g_SelectSector.x, g_SelectSector.y);
						Phoenix_ClearPendingGroundClick();

						int TempDir = MTopView::GetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY(),g_SelectSector.x, g_SelectSector.y);
						
//							if(g_pPlayer->GetDirection() != TempDir)
//							{
//								CGMove _CGMove;
//								_CGMove.setX( g_pPlayer->GetX() );
//								_CGMove.setY( g_pPlayer->GetY() );
//								_CGMove.setDir( TempDir );
//								g_pSocket->sendPacket( &_CGMove );
//							}
						if(g_pPlayer->GetDirection() == TempDir)
						{
//							g_pPlayer->UnSetRequestMode();
//							g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//							g_pPlayer->SetSpecialActionInfo(SKILL_TURRET_FIRE);
//							if (g_pPlayer->TraceSectorToSpecialAction( g_SelectSector.x, g_SelectSector.y ))
//							{
//								// ���õ� Sector�� ǥ���Ѵ�.
//								g_pTopView->SetSelectedSector( g_SelectSector );
//							
//								g_pPlayer->SetRepeatAction();
//							}
//
//							g_bPreviousMove = false;
//								char szTemp[128];
//								sprintf(szTemp, "������ ����: %d,���� ", TempDir);
//								g_pSystemMessage->Add(szTemp);
						}
						else
						{
							//CGMove _CGMove;
							//_CGMove.setX( g_pPlayer->GetX() );
							//_CGMove.setY( g_pPlayer->GetY() );
							//_CGMove.setDir( TempDir );
							//g_pSocket->sendPacket( &_CGMove );
							if(g_pPlayer->IsStop())
								g_pPlayer->SetCurrentDirection(TempDir);
						}
					}
					//--------------------------------------------------
					// ������ ���� Object�� ������ MOVE
					//--------------------------------------------------
					else if (pObject == NULL)
					{
						DEBUG_ADD_FORMAT("[NPC-CLICK] LButton pObject=NULL (sector=%d,%d) -> MOVE PATH",
							(int)g_SelectSector.x, (int)g_SelectSector.y);
						// l-shift�� l-control�� �������� ���� ���¿����� �̵�.
						if (//!g_pDXInput->KeyDown(DIK_LSHIFT) &&
							!g_pDXInput->KeyDown(DIK_LCONTROL))
						{
							if (g_pPlayer->IsNotDelay() && !g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE )
								// 2004, 9, 14, sobeit add start - ѽ 130 skill 
								//&&  !g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET ) 
								// 2004, 9, 14, sobeit add end - ѽ 130 skill 
								)
							{
								CT_BAT("MoveCheck: NotDelay=%d, PauseStatus=%d", g_pPlayer->IsNotDelay(), g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE));
								if (g_pPlayer->RequestMovementRetarget(g_SelectSector.x, g_SelectSector.y))
								{
									Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_LEFT, "LEFT_GAME_MOVE_RETARGET_OK", g_SelectSector.x, g_SelectSector.y);
									g_bPreviousMove = true;
								}
								else
								{
									Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "LEFT_MOVE_RETARGET_FAILED");
								}
								Phoenix_ClearPendingGroundClick();
							}
							else if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE))
							{
								Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "LEFT_MOVE_BLOCKED_ETERNITY_PAUSE");
								Phoenix_ClearPendingGroundClick();
							}
							else
							{
								if (g_pPlayer->RequestMovementRetarget(g_SelectSector.x, g_SelectSector.y))
								{
									Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_LEFT, "LEFT_GAME_MOVE_RETARGET_QUEUED_DURING_DELAY", g_SelectSector.x, g_SelectSector.y);
									Phoenix_ClearPendingGroundClick();
									g_bPreviousMove = true;
								}
								else
								{
									Phoenix_MouseAuditMarkWait(PHX_MOUSE_AUDIT_LEFT, "LEFT_MOVE_WAIT_IS_NOT_DELAY_FALSE", g_SelectSector.x, g_SelectSector.y);
								}
							}
						}
						else
						{
							Phoenix_MouseAuditResolveLost(PHX_MOUSE_AUDIT_LEFT, "LEFT_MOVE_BLOCKED_LCONTROL");
							Phoenix_ClearPendingGroundClick();
						}

						/*
						if (g_pPlayer->IsStop())
						{
							// ���� ��ǥ��ġ�� �����Ѵ�
							g_pPlayer->TraceNULL();
							
							if (g_pPlayer->SetMovePosition(g_SelectSector.x, g_SelectSector.y))
							{
								g_pPlayer->SetNextAction( ACTION_MOVE );
							}
						}
						*/
					}
					//--------------------------------------------------
					// ���õ� Object�� ���ؼ� 
					//--------------------------------------------------
					else 
					{	
						Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_LEFT, "LEFT_GAME_OBJECT_BRANCH", g_x, g_y);
						Phoenix_ClearPendingGroundClick();

						// Object�� ���� ���õ��� ��쿡�� 
						// LButton�� ������ �ִ� �͸����� �̵��� �����ʰ� �Ѵ�.
						//g_bLButtonDown = FALSE;

						switch (pObject->GetObjectType())
						{			
							case MObject::TYPE_CREATURE :
								{
									DEBUG_ADD_FORMAT("[NPC-CLICK] TYPE_CREATURE id=%d -> TraceCreatureToBasicAction",
										(int)pObject->GetID());
									//20090226���ڴԵ��� ���� ctrl Ű�� ������ Ŭ���ϴ� ĳ���ʹ� ��� ����ȴ�. ���� ���� ���� ���� ��� ����.
									if(g_pDXInput->KeyDown(DIK_RSHIFT) && g_pOperatorOption->bRecordCharName)
									{
										g_pPlayer->RecordCharName(pObject->GetID());
									}
									// 2004, 12, 3, ���ξ� �߰�
									MCreature* TempCreature = (MCreature*)pObject;
									if(TempCreature->CurPernalShop() == 1 )
//										|| g_pPlayer->IsFlyingCreature() || g_pPlayer->IsUndergroundCreature() 
//										|| g_pPlayer->IsGroundCreature() || g_pPlayer->IsFakeCreature() 
//										|| g_pPlayer->GetMoveDevice()== 1|| g_pPlayer->GetMoveDevice()== 2)
									{
										gpC_base->SendMessage(UI_REQUEST_STORE_INFO, pObject->GetID(),0);
										gC_vs_ui.SetOtherObjectID(pObject->GetID());
									}
									// 2004, 12, 3, ���ξ� �߰�
									if (g_pPlayer->TraceCreatureToBasicAction( pObject->GetID(), 
																				bForceAttack, true ))
									{
										//----------------------------------
										// ������ �ϴ� �⺻ �ൿ �ݺ� ����
										//----------------------------------
										g_pPlayer->SetRepeatAction();
										g_bPreviousMove = false;

										// ���� �޺� ���� �ʱ�ȭ Sjheon	2005.07.04 Add 
									//	g_pPlayer->SetCombo(1) ;
										// ���� �޺� ���� �ʱ�ȭ Sjheon	2005.07.04 End
									}

									g_pTopView->SetSelectedSectorNULL();
								}
							break;

							case MObject::TYPE_ITEM :
								if(!g_pPlayer->HasEffectStatus(EFFECTSTATUS_GHOST) || g_pOperatorOption->bCanRootingGhost)
								{
									g_pPlayer->TraceItem( pObject->GetID() );
									g_bLButtonDown = FALSE;
									g_bPreviousMove = false;
								}

							break;

							case MObject::TYPE_EFFECT :
								g_pPlayer->TraceEffect( pObject->GetID() );

								g_bLButtonDown = FALSE;

								g_bPreviousMove = false;								
							break;
							
							case MObject::TYPE_INTERACTIONOBJECT :
								g_pPlayer->TraceInteractionObject( pObject->GetID(), ((MInteractionObject*)pObject)->GetAction() );

								g_bLButtonDown = FALSE;

								g_bPreviousMove = false;
								//-------------------------
								// TEST CODE
								//-------------------------
								//((MInteractionObject*)pObject)->SetAction( !((MInteractionObject*)pObject)->GetAction() );
							break;
						}
					}
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("LK");
					#endif
				}

				//---------------------------------------------------------------
				//
				//		 RButtonDown
				//
				//---------------------------------------------------------------
				else if (!consumedPendingGroundClick && g_pDXInput->m_rb_down)
				{
					Phoenix_MouseAuditResolveGame(PHX_MOUSE_AUDIT_RIGHT, "RIGHT_GAME_BRANCH", g_x, g_y);
					Phoenix_ClearPendingGroundClick();

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("Ro");
					#endif

#if __CONTENTS(__FRIEND_ADDITION)
					bool b_user_popup_check = false;
					MCreature* pCreature = NULL;
					if (pObject != NULL) pCreature = g_pZone->GetCreature(pObject->GetID());
					if (pObject != NULL && pCreature != NULL && pObject->GetObjectType() == MObject::TYPE_CREATURE && !pCreature->IsNPC() && pCreature->IsPlayerOnly())
					{
						if ((((MCreature*)pObject)->IsSlayer() && g_pPlayer->IsSlayer())
						|| (((MCreature*)pObject)->IsVampire() && g_pPlayer->IsVampire())
						|| (((MCreature*)pObject)->IsOusters() && g_pPlayer->IsOusters()))
						{
							if (g_pDXInput->KeyDown(DIK_LCONTROL))
							{
								UI_GeneratePlayerPopupMenu(g_x,g_y,pObject->GetID());
								b_user_popup_check = true;
							}
						}
					}
#endif //__FRIEND_ADDITION

					g_pPlayer->UnSetRequestMode();
/**/
// 					BYTE_ATTRIBUTE actionTarget(FLAG_ACTIONINFO_SELECT_ALL);
// 					TYPE_ACTIONINFO actionInfoIdx = g_pPlayer->GetSpecialActionInfo();
// 					if(actionInfoIdx != ACTIONINFO_NULL)
// 					{
// 						MActionInfo& actionInfo = (*g_pActionInfoTable)[actionInfoIdx];
// 						actionTarget = actionInfo.GetActionTarget();
// 					}
// 					MObject* pTargetObject = g_pTopView->GetSelectedObject(g_x, g_y, actionTarget);
// 
// 					ProcessInputRButtonDown(pTargetObject);
// 
// 					if(pTargetObject)
// 						g_pTopView->SetSelectedCreature(pTargetObject->GetID());
/**/

#if __CONTENTS(__FRIEND_ADDITION)
					if (!b_user_popup_check)
#endif //__FRIEND_ADDITION
					{
						ProcessInputRButtonDown(pObject);
					}

					g_bPreviousMove = false;

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("RK");
					#endif
				}

				//---------------------------------------------------------------
				// L/R Button�� �������� �ʾ�����,
				// ������ ���� LButton�� ��� ������ ���¶��..
				// But(!), L-Shift�� �������� �ʾƾ� �Ѵ�.
				//---------------------------------------------------------------
				else if (!consumedPendingGroundClick && g_bLButtonDown
						&& !g_pDXInput->KeyDown(DIK_LSHIFT)
#ifdef __METROTECH_TEST__
						&& !g_bCButtonDown
#endif
						)
				{
					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("ConM");
					#endif

					//--------------------------------------------------
					// ������ ���� Object�� ����
					// ������� �̵��ϰ� �ִ� ���̸� MOVE
					//--------------------------------------------------
					if (pObject == NULL && g_bPreviousMove)
					{
						DEBUG_ADD_FORMAT("[NPC-CLICK] DRAG-MOVE pObject=NULL sector=%d,%d",
							(int)g_SelectSector.x, (int)g_SelectSector.y);
						if (g_pPlayer->IsNotDelay() && !g_pPlayer->IsRepeatAction())
						{
							g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

							// ���� ��ǥ��ġ�� �����Ѵ�
							g_pPlayer->RequestMovementRetarget(g_SelectSector.x, g_SelectSector.y);
						}
					}
					else if (pObject != NULL)
					{
						DEBUG_ADD_FORMAT("[NPC-CLICK] DRAG-HOVER pObject=NOT-NULL type=%d id=%d",
							(int)pObject->GetObjectType(), (int)pObject->GetID());
					}

					#ifdef OUTPUT_DEBUG_PROCESS_INPUT
						DEBUG_ADD("ConMK");
					#endif
				}
			}
		}
	}	// lock mode

	//---------------------------------------------------	
	// LButton Up
	//---------------------------------------------------	
	if (g_pDXInput->m_lb_up && g_bLButtonDown
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("LUP2");
		#endif

		g_bLButtonDown = FALSE;
	
		if (g_pPlayer->IsRepeatAction())
		{
			// ��ư�� �������Ƿ� �ൿ �ݺ��� ����Ѵ�.
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();

			g_bPreviousMove = false;
		}
		else
		{	
			// ���õ� sector�� ���Ѵ�.
			POINT point;
			
			// ���� �� ���� ������
			g_pPlayer->GetNextDestination( point );		
			if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
			{
				// ���� ���� �ִ� ���� ������
				g_pPlayer->GetDestination( point );
				if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
				{
					//-_-;;
				}
				else
				{
					g_pTopView->SetSelectedSector( point );
				}
			}
			else
			{
				g_pTopView->SetSelectedSector( point );
			}		
		}
	}

	//---------------------------------------------------	
	// RButton Up
	//---------------------------------------------------	
	if (g_pDXInput->m_rb_up && g_bRButtonDown
#ifdef __METROTECH_TEST__
		&& !g_bCButtonDown
#endif
		)
	{
		#ifdef OUTPUT_DEBUG_PROCESS_INPUT
			DEBUG_ADD("RUP2");
		#endif

		g_bRButtonDown = FALSE;
	
		if (g_pPlayer->IsRepeatAction())
		{
			// ��ư�� �������Ƿ� �ൿ �ݺ��� ����Ѵ�.
			g_pPlayer->UnSetRepeatAction();
			//g_pPlayer->TraceNextNULL();
		}
	}

	if(g_pDXInput->m_cb_up && g_bCButtonDown)
	{
		g_bCButtonDown = FALSE;
	}

	// ������ �� �ִ� �����̸� ��ǥ ��ġ�� ���ش�.
	if (g_pPlayer->GetAction()==ACTION_STAND)
	{
		g_pTopView->SetSelectedSectorNULL();
	}

	#ifdef OUTPUT_DEBUG_PROCESS_INPUT
		DEBUG_ADD("IAEnd");
	#endif


	//---------------------------------------------------
	//
	// keyboard
	//
	//---------------------------------------------------
	/*
	// Missile ���� �ٲٱ�
	if (g_pDXInput->KeyDown(DIK_1))
		g_pPlayer->SetActionInfo( ACTIONINFO_BOMB_TO_CREATURE );

	else if (g_pDXInput->KeyDown(DIK_2))
			g_pPlayer->SetActionInfo( ACTIONINFO_FIRE_TO_CREATURE );

	else if (g_pDXInput->KeyDown(DIK_3))
			g_pPlayer->SetActionInfo( ACTIONINFO_LIGHTNING_TO_CREATURE );

	else if (g_pDXInput->KeyDown(DIK_4))
			g_pPlayer->SetActionInfo( ACTIONINFO_ATTACH_FIRE_TO_CREATURE );

	else if (g_pDXInput->KeyDown(DIK_5))
			g_pPlayer->SetActionInfo( ACTIONINFO_ATTACH_AURA_TO_SELF );
	*/

	#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
		//---------------------------------------------------
		// ��~~
		//---------------------------------------------------
		if (g_pDXInput->KeyDown(DIK_8))
		{
			SetWeather( WEATHER_RAINY, rand()%10+10);
		}

		//---------------------------------------------------
		// ��~~
		//---------------------------------------------------
		if (g_pDXInput->KeyDown(DIK_9))
		{
			SetWeather( WEATHER_SNOWY, rand()%10+10);
		}

		//---------------------------------------------------
		// ���� ����
		//---------------------------------------------------
		if (g_pDXInput->KeyDown(DIK_0))
		{
			SetWeather( WEATHER_CLEAR, 0 );
		}



		//---------------------------------------------------
		// �þ�  + / -
		//---------------------------------------------------

		if (g_pDXInput->KeyDown(DIK_SUBTRACT) 
			)//&& g_pPlayer->GetLightSight()!=0)
		{
//			g_pPlayer->SetLightSight( g_pPlayer->GetLightSight() - 1 );		
						g_pClientConfig->QuestItemColorSet -= 15;

					if( g_pClientConfig->QuestItemColorSet <= 0 )
						g_pClientConfig->QuestItemColorSet = OPTIONEXT_ITEM_COLOR-15;
		}
		
		if (g_pDXInput->KeyDown(DIK_ADD) )// && g_pPlayer->GetLightSight()!=13)
		{
//			g_pPlayer->SetLightSight( g_pPlayer->GetLightSight() + 1 );		
			
			
		}

		//-----------------------------------------------
		// play Sound
		//-----------------------------------------------
		if (g_pDXInput->KeyDown(DIK_F4))
		{
			//g_sb2 = g_Sound.LoadWav((*g_pSoundTable)[rand()%g_pSoundTable->GetSize()].Filename);
			//g_sb2 = g_Sound.LoadWav("Data\\Sound\\Sound1.wav");
			// [ TEST CODE ]
			/*
			static int pan = -10000;
			static int p = 200;
			pan += p;

			if (pan > 10000) 
			{
				pan = 10000;
				p = -p;
			}

			if (pan < -10000) 
			{
				pan = -10000;
				p = -p;
			}
			*/

			//g_Sound.ChangePan((*g_pSoundTable)[rand()%g_pSoundTable->GetSize()].pDSBuffer, pan);
			//g_Sound.Play((*g_pSoundTable)[rand()%g_pSoundTable->GetSize()].pDSBuffer);

			//PlaySound( rand()%g_pSoundTable->GetSize(), 
			//			false,
			//			g_pPlayer->GetX(), g_pPlayer->GetY() );
			PlaySound( SOUND_S_DIE, 
						false,
						g_pPlayer->GetX() + rand()%20-10, g_pPlayer->GetY() + rand()%20-10 );			

		}

		//---------------------------------------------------
		// ��� ����
		//---------------------------------------------------
		if (g_pDXInput->KeyDown(DIK_F9))
		{
			BYTE DarkBits = g_pTopView->GetDarkBits()-1;
			g_pTopView->SetDarkBits(DarkBits);
		}
		
		//---------------------------------------------------
		// ��� ����
		//---------------------------------------------------
		if (g_pDXInput->KeyDown(DIK_F10))
		{
			BYTE DarkBits = g_pTopView->GetDarkBits()+1;
			g_pTopView->SetDarkBits(DarkBits);
		}	
		
		// Gamma Test
		/*
		static BYTE gammaStep = 255;
		if (CDirectDraw::IsSupportGammaControl())
		{
			if (g_pDXInput->KeyDown(DIK_LBRACKET))
			{
				gammaStep -= 10;
				CDirectDraw::SetGammaRamp( gammaStep );
			}
			if (g_pDXInput->KeyDown(DIK_RBRACKET))
			{		
				gammaStep += 10;		
				CDirectDraw::SetGammaRamp( gammaStep );
			}
		}
		*/
	#endif

	//-----------------------------------------------
	// File Loading Test with Thread
	//-----------------------------------------------		
	/*
	if (g_pDXInput->KeyDown(DIK_Z))
	{	
		// test for state of event nonsignaled?
		if (WaitForSingleObject(g_hFileEvent, 0) != WAIT_OBJECT_0)
		{
			g_ThreadJob = THREADJOB_LOAD_CREATURE;
			SetEvent(g_hFileEvent);			
		}		
	} 

	if (g_pDXInput->KeyDown(DIK_X) && WaitForSingleObject(g_hFileEvent, 0) != WAIT_OBJECT_0)
	{	
		g_pTopView->ReleaseCreatureSPK( 0 );
		g_pTopView->ReleaseCreatureSPK( 1 );
	}
	*/

	//-----------------------------------------------
	// Thread Priority Test
	//-----------------------------------------------	
	/*
	if (g_pDXInput->KeyDown(DIK_9))
	{
		SetThreadPriority(g_hFileThread, 
							THREAD_PRIORITY_BELOW_NORMAL);
	}

	if (g_pDXInput->KeyDown(DIK_0))
	{
		SetThreadPriority(g_hFileThread, 
							THREAD_PRIORITY_HIGHEST);
	}
	*/
		
	#ifdef OUTPUT_DEBUG
		// Zone�̵� test
		/*
		if (g_pDXInput->KeyDown(DIK_1))
		{	
			MoveZone( 1 );				
		}
		else if (g_pDXInput->KeyDown(DIK_2))
		{				
			MoveZone( 2 );				
		}	
		else if (g_pDXInput->KeyDown(DIK_3))
		{							
			MoveZone( 3 );			
		}
		else if (g_pDXInput->KeyDown(DIK_4))
		{			
			MoveZone( 4 );				
		}
		*/
		

		//-----------------------------------------------
		// Test for Effect
		//-----------------------------------------------
		#ifdef _DEBUG
			static DWORD start=0, current = 0;

			

			current++;

			if (current-start > 10)
			{
				if (g_pDXInput->KeyDown(DIK_LCONTROL) && g_pDXInput->KeyDown(DIK_RCONTROL))
				{					
					
					POINT playerPoint = MTopView::MapToPixel(g_pPlayer->GetX(), g_pPlayer->GetY());

					for (int i=-1; i<=1; i++)
					{
						for (int j=-1; j<=1; j++)
						{
							if (i==0 && j==0)
								continue;
							/*
							if (i==0 && j==0)
							{
								MEffect*	pEffect;
								pEffect = new MEffect;

								pEffect->SetFrameID(0, 8);		// 0�� Effect, Max 8 Frame
								pEffect->SetPosition(g_pPlayer->GetX()+j, g_pPlayer->GetY()+i);	// Sector ��ǥ						
								pEffect->SetCount(125+rand()%8);			// ���ӵǴ� Frame

								g_pZone->AddEffect( pEffect );
							}
							*/

							BYTE effectSpriteType = rand()%(*g_pEffectSpriteTypeTable).GetSize();
			
							//if (g_pDXInput->KeyDown(DIK_1))	effectSpriteType = 0;
							//else if (g_pDXInput->KeyDown(DIK_2))	effectSpriteType = 1;
							//else if (g_pDXInput->KeyDown(DIK_3))	effectSpriteType = 2;
							//else if (g_pDXInput->KeyDown(DIK_4))	effectSpriteType = 3;

							
							TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[effectSpriteType].FrameID;
							BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[effectSpriteType].BltType;
							
							// BLT_NORMAL
							int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
							
							// Sound ... Test *_*;;
							//g_Sound.Play(g_sb1);



							MLinearEffect*	pEffect;
							//pEffect = new MParabolaEffect(BLT_EFFECT);
							pEffect = new MLinearEffect(BLT_EFFECT);

							pEffect->SetFrameID(frameID, maxFrame);		// 0�� Effect, Max 8 Frame

							// �߻� ��ġ Pixel��ǥ
							pEffect->SetPixelPosition(playerPoint.x, playerPoint.y, 0);	
							
							// ��ǥ ��ġ Pixel��ǥ
							pEffect->SetTarget(playerPoint.x + -i*300, 
												playerPoint.y + -j*350,
												0,
												20);	// step

							// ���ӵǴ� Frame (��ǥ�� �ִٸ� ���� ���� ���� - -;)
							pEffect->SetCount(1000);

							//pEffect->SetLight(1);

							// 2026-05-20 DISABLED: 2026-05-12 commit 045040e3 yanlis "leak fix" eklenmis.
							// MZone::AddEffect false donerken zaten kendisi delete pNewEffect yapiyor
							// (MZone.cpp:4098 CanAddEffect fail + MZone.cpp:4130 outer zone fail).
							// Ek delete = double-free -> MEffect::~MEffect garbage member dereference -> AV
							// (BLOOD_DROP runtime crash 2026-05-20 empirical).
							// if (!g_pZone->AddEffect( pEffect ))
							//     delete pEffect;
							g_pZone->AddEffect( pEffect );



							//*/	
							/*
							MLinearEffect*	pEffect;
							pEffect = new MLinearEffect;

							pEffect->SetFrameID(0, 8);		// 0�� Effect, Max 8 Frame

							// �߻� ��ġ Pixel��ǥ
							int xx=rand()%800-300;
							int yy=rand()%400-400;
							int last=rand()%500;
							pEffect->SetPixelPosition(playerPoint.x+xx, playerPoint.y+yy, 0);
							
							// ��ǥ ��ġ Pixel��ǥ
							pEffect->SetTarget(playerPoint.x+xx-rand()%50 , 
												playerPoint.y+yy+last,
												0,
												20);

							// ���ӵǴ� Frame (��ǥ�� �ִٸ� ���� ���� ���� - -;)
							pEffect->SetCount(25);						

							g_pZone->AddEffect( pEffect );
							*/
						}					
					}
				}

				start = current;
			}
		#endif
	#endif
}

//---------------------------------------------------------------------------
// UpdateGame Draw
//---------------------------------------------------------------------------
void
CGameUpdate::UpdateDraw()
{
	{
		static DWORD s_LastDrawEnterTrace = 0;
		DWORD nowDraw = timeGetTime();
		if (nowDraw - s_LastDrawEnterTrace > 1000) {
			s_LastDrawEnterTrace = nowDraw;
			CT_BD("[DRAW] enter items=%d ge=%d cre=%d fake=%d",
			      g_pZone ? g_pZone->GetItemNumber() : -1,
			      g_pZone ? g_pZone->GetGroundEffectCount() : -1,
			      g_pZone ? g_pZone->GetCreatureNumber() : -1,
			      g_pZone ? g_pZone->GetFakeCreatureNumber() : -1);
		}
	}
	// 2026-05-18 ImGui Hibrit Admin Menu: per-frame NewFrame — ImGui input
	// state'i ve draw command list'i hazirlar. Asil Render() D3D9Backend
	// Present() oncesi cagriliyor (D3D9Backend.cpp:1267). attrOperator gate
	// Render icinde — bu noktada her zaman cagrilabilir (cheap if disabled).
	{
		Phoenix_FrameProbeStageScope fpStage("Draw.ImGuiNewFrame", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		PhoenixImGui::NewFrame();
	}

	//--------------------------------------------------------
	/*
	RECT rectClip =
	{ 
		50,
		50, 
		750, 
		550 
	};
	g_pBack->SetClip(&rectClip);
	*/
	static bool outputInfo = false;		
	

	POINT	point;
	RECT	rect;
	
	//#ifdef	OUTPUT_DEBUG
		char	str[128];
	//#endif

	// buffer : InitSurface���� SYSTEMMEMORY�� ���ְ� ��ߵȴ�.
	//
	//g_pLast->FillSurface(CDirectDraw::Color(20,20,20));
	
	// ȭ�� �ؿ� Interface�κ� �����ֱ�...
	// [ TEST CODE ]
	/*
	rect.left = 0;
	rect.right = CLIPSURFACE_WIDTH;
	rect.top = CLIPSURFACE_HEIGHT;
	rect.bottom = SURFACE_HEIGHT;
	g_pBack->FillRect(&rect, CDirectDraw::Color(5,7,7));
	*/

	//-----------------------------------------------------------------		
	// ���콺 ��ǥ �ٽ� ����
	//-----------------------------------------------------------------
	GetCursorPos(&point);
	ScreenToClient(g_hWnd, &point);
		
	// ui�� mouse��ǥ ����
	{
		Phoenix_FrameProbeStageScope fpStage("Draw.MouseControl", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		gC_vs_ui.MouseControl(M_MOVING, point.x, point.y);
	}

	//-----------------------------------------------------------------
	// ž�� Zone ���
	//-----------------------------------------------------------------
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			DEBUG_ADD("dd");//[Update-Draw] Before Draw");
	#endif

//	if (CDirect3D::IsHAL())
//	{
//		//-----------------------------------------------------------------
//		// Game ȭ�� ���
//		//-----------------------------------------------------------------
//		__BEGIN_PROFILE("GameDraw3D")
//			
//		const MEvent *event = g_pEventManager->GetEventByFlag(EVENTFLAG_SHAKE_SCREEN);
//		if(event != NULL)
//		{		
//			static int count = 0;
//			const int maxStep = 13;
//			const int step[maxStep] = { 0, 2, 3, 6, 5, 3, 0, -4, -6, -5, -3, -1, 0 };
//			
//			g_pTopView->Draw( step[count]*2, ((g_pEventManager->IsEvent(EVENTID_METEOR_SHAKE) == true)?((count%2)*20-10):(count%3-1)*3) );
//			
//			if(g_bFrameChanged)
//			{
//				count ++;
//				count %= maxStep;			
//			}
//		}
//		else
//			g_pTopView->Draw(0,0);
//
//		__END_PROFILE("GameDraw3D")
//
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//				DEBUG_ADD("us");//[Update-Draw] Before UI Show");
//		#endif
//
//		//-----------------------------------------------------------------
//		// UI ���
//		//-----------------------------------------------------------------		
//		__BEGIN_PROFILE("UIDraw3D")
//
//		gC_vs_ui.Show();		
//
//		__END_PROFILE("UIDraw3D")
//
//
//		__BEGIN_PROFILE("DrawFPS")
//			if (outputInfo || g_pUserOption->DrawFPS)
//			{
//				//-----------------------------------------------------------------
//				// FPS ���	
//				//-----------------------------------------------------------------
//				sprintf(str, "%d FPS(HAL)", g_FrameRate);	
//			
//				g_pBack->GDI_Text(11,11, str, RGB(20,20,20));
//				g_pBack->GDI_Text(10,10, str, 0xFFFFFF);
//			}
//		__END_PROFILE("DrawFPS")
//		
//
//		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
//				DEBUG_ADD("usk");//[Update-Draw] After UI Show");
//		#endif
//
//		__BEGIN_HELP_EVENT
//			UpdateDrawHelp();
//		__END_HELP_EVENT
//	}
//	else
	{
		//-----------------------------------------------------------------
		// Font ����ϴ� Surface�� �ٲ���� �Ѵ�.
		//-----------------------------------------------------------------
		//g_SetFL2Surface( g_pLast->GetSurface() );

		//-----------------------------------------------------------------
		// Game ȭ�� ���
		//-----------------------------------------------------------------
		__BEGIN_PROFILE("GameDraw2D")
		{
			Phoenix_FrameProbeStageScope fpStage("Draw.TopViewDraw", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);

//		#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
//			if (g_pDXInput->KeyDown(DIK_SPACE))
			const MEvent *event = g_pEventManager->GetEventByFlag(EVENTFLAG_SHAKE_SCREEN);
			if(event != NULL && event->IsShowTime() == true)
			{		
				static int count = 0;
				const int maxStep = 13;

				// 2004, 5, 21 sobeit modify start
				//const int step[maxStep] = { 0, 2, 3, 6, 5, 3, 0, -4, -6, -5, -3, -1, 0 };
				//g_pTopView->Draw( step[count]*event->parameter3, ((g_pEventManager->IsEvent(EVENTID_METEOR_SHAKE) == true)?((count%2)*3-3):(count%3-1)*3)*event->parameter3 );
				const int step[maxStep] = { 0, 1, 2, 2, 2, 1, 0, -2, -2, -2, -1, -1, 0 };
				g_pTopView->Draw( step[count]*event->parameter3, ((g_pEventManager->IsEvent(EVENTID_METEOR_SHAKE) == true)?((count%2)*3-2):(count%3-1)*3)*event->parameter3 );
				// 2004, 5, 21 sobeit modify end

				if(g_bFrameChanged)
				{
					count ++;
					count %= maxStep;			
				}
			}
			else
//		#endif
			{
			CT_Done("CGU: pre g_pTopView->Draw(0,0)");
			g_pTopView->Draw(0,0);			
			CT_Done("CGU: post g_pTopView->Draw(0,0)");
			{
				static DWORD s_LastAfterTopView = 0;
				DWORD nowAfterTV = timeGetTime();
				if (nowAfterTV - s_LastAfterTopView > 1000) {
					s_LastAfterTopView = nowAfterTV;
					CT_BD("[DRAW] after TopView");
				}
			}
			}
		}

		__END_PROFILE("GameDraw2D")

		//g_SetFL2Surface( g_pBack->GetSurface() );

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD("us");//[Update-Draw] Before UI Show");
		#endif

		//-----------------------------------------------------------------
		// UI ���
		//-----------------------------------------------------------------		
		__BEGIN_PROFILE("UIDraw2D")

		CT_Done("CGU: pre gC_vs_ui.Show()");
		{
			Phoenix_FrameProbeStageScope fpStage("Draw.UIShow", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			gC_vs_ui.Show();
		}
		CT_Done("CGU: post gC_vs_ui.Show()");
		{
			static DWORD s_LastAfterUI = 0;
			DWORD nowAfterUI = timeGetTime();
			if (nowAfterUI - s_LastAfterUI > 1000) {
				s_LastAfterUI = nowAfterUI;
				CT_BD("[DRAW] after UI");
			}
		}

		__END_PROFILE("UIDraw2D")

		#ifdef OUTPUT_DEBUG_UPDATE_LOOP
				DEBUG_ADD("usk");//[Update-Draw] After UI Show");
		#endif

//		__BEGIN_HELP_EVENT
//			UpdateDrawHelp();
//		__END_HELP_EVENT

		if (!g_pTopView->IsDrawRequest())
		{
			Phoenix_FrameProbeStageScope fpStage("Draw.MousePointer", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			gC_vs_ui.DrawMousePointer();
		}

		char str[256];
		
		__BEGIN_PROFILE("DrawFPS")
		{
			Phoenix_FrameProbeStageScope fpStage("Draw.FPS", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			const bool showFpsPingOverlay = outputInfo || (g_pUserOption != NULL && g_pUserOption->DrawFPS);
			Phoenix_SetFpsPingOverlayVisible(showFpsPingOverlay);
			if (showFpsPingOverlay)
			{
				//-----------------------------------------------------------------
				// FPS / PING
				//-----------------------------------------------------------------				
				Phoenix_ClampFpsPingOverlay();

				sprintf(str, "%d FPS", g_FrameRate);
				g_pLast->GDI_Text(g_FpsPingOverlayX + 1, g_FpsPingOverlayY + 1, str, RGB(20,20,20));
				g_pLast->GDI_Text(g_FpsPingOverlayX, g_FpsPingOverlayY, str, 0xFFFFFF);

				sprintf(str, "%d Ping", g_Ping);
				g_pLast->GDI_Text(g_FpsPingOverlayX + 1, g_FpsPingOverlayY + kFpsPingLineHeight + 1, str, RGB(20,20,20));
				g_pLast->GDI_Text(g_FpsPingOverlayX, g_FpsPingOverlayY + kFpsPingLineHeight, str, Phoenix_GetPingColor());

				// 2026-09-03 R3 — kullanici istegi: FPS/Ping altinda aktif
				// cozunurluk (canli resolution degisiminin dogrulanmasi icin).
				sprintf(str, "Res: %dx%d",
				        g_pUserInformation->iResolution_x,
				        g_pUserInformation->iResolution_y);
				g_pLast->GDI_Text(g_FpsPingOverlayX + 1, g_FpsPingOverlayY + kFpsPingLineHeight * 2 + 1, str, RGB(20,20,20));
				g_pLast->GDI_Text(g_FpsPingOverlayX, g_FpsPingOverlayY + kFpsPingLineHeight * 2, str, 0xFFFFFF);
			}
		}
		__END_PROFILE("DrawFPS")

		//-----------------------------------------------------------------
		// Last�� Back���� copy - 3D HAL�� �ƴ� ��츸..
		//-----------------------------------------------------------------
		__BEGIN_PROFILE("LastToBack")
		{
			Phoenix_FrameProbeStageScope fpStage("Draw.LastToBack", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);

//		if(CDirect3D::IsHAL())
//		{
//			point.x = 0;
//			point.y = 0;
//			rect.left = 0;
//			rect.top = 0;
//			rect.right = SURFACE_WIDTH;
//			rect.bottom = SURFACE_HEIGHT;
//			g_pLast->SetTransparency(0x001F);
//			g_pBack->Blt( &point, g_pLast, &rect );		
//		}
//		else
//		{
			point.x = 0;
			point.y = 0;
			rect.left = 0;
			rect.top = 0;
			rect.right = SURFACE_WIDTH;
			rect.bottom = SURFACE_HEIGHT;
			g_pBack->BltNoColorkey( &point, g_pLast, &rect );		
//		}
		}

		__END_PROFILE("LastToBack")
	}

	Phoenix_FrameProbe_RecordRuntimeCounters(
		g_FrameRate,
		g_pZone ? g_pZone->GetCreatureNumber() : -1,
		g_pZone ? g_pZone->GetFakeCreatureNumber() : -1,
		g_pZone ? g_pZone->GetItemNumber() : -1,
		g_pZone ? g_pZone->GetEffectCount() : -1,
		g_pZone ? g_pZone->GetGroundEffectCount() : -1);
	
	#ifdef OUTPUT_DEBUG_UPDATE_LOOP
			DEBUG_ADD("ddk");//[Update-Draw] After Draw");
	#endif


	//-------------------------------------------------------------------
	// Mouse�� ������ ��ǥ�� ���� debug�� code
	//-------------------------------------------------------------------
	/*
	WORD*	lpSurface;
	long	lPitch;
	g_pLast->LockW(NULL, lpSurface, lPitch);

	for (int y=g_y-30; y<g_y+30; y++)
		for (int x=g_x-60; x<g_x+60; x++)
		{
			point = g_pTopView->GetSelectedSector(x,y);

			if (point.x==g_SelectSector.x && point.y==g_SelectSector.y)
			{
				*(lpSurface + y*lPitch + x) = 0xCCCC;
			}
		}

	g_pLast->Unlock();

	sprintf(str, "Selected Sector = (%d, %d)", g_SelectSector.x, g_SelectSector.y);
	g_pLast->GDI_Text(10,200, str, RGB(220,220,220));
	*/


	//-----------------------------------------------------------------
	// tab������ MiniMap�� ��������.
	//-----------------------------------------------------------------
	//if (g_bDrawMinimap)
	//{	
	//	g_pTopView->DrawMinimap(
	//			800-g_pZone->GetWidth()*2,	//10
	//			0, //-g_pZone->GetHeight(),	// 465
	//			1);
	//}

	//-----------------------------------------------------------------
	// ��Ʈ�� ���°� ������?
	//-----------------------------------------------------------------
	/*
	if (!g_bNetStatusGood)
	{
		DEBUG_ADD("Network Condition is Bad.");

		g_pBack->GDI_Text(351,11, (*g_pGameStringTable)[STRING_NETWORK_CONDITION_BAD].GetString(), RGB(10,10,10));
		g_pBack->GDI_Text(350,10, (*g_pGameStringTable)[STRING_NETWORK_CONDITION_BAD].GetString(), RGB(230,50,50));		
	}
	*/

	//-----------------------------------------------------------------
	// ���� ����..
	//-----------------------------------------------------------------
	// FAZ 4 (2026-05-04): NULL guard. Disconnect sonrası g_pPlayer yok edilir
	// ama main loop bir-iki frame daha çalışıyor → NULL->IsDead() crash. Dump
	// (00:42) PDB ile CGameUpdate.cpp:6377 olarak tespit edildi.
	if (g_pPlayer == NULL) {
		return;
	}
	if (g_pPlayer->IsDead())
	{
		Phoenix_FrameProbeStageScope fpStage("Draw.DeadStateUI", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
		//ij-ch 2008.09.29 �����̾� ����â ���� Add
		if(gC_vs_ui.IsRunningRequest_PremiumGiveItem())
			gC_vs_ui.FinishRequest_PremiumGiveItem();
		//ij-ch 2008.09.29 �����̾� ����â ���� End
#endif //__PREMIUM_GIVE_ITEM_UI

		//m_pC_game->CloseTraceWindow();() ; 
		//Sjheon 2005.11.11 Ʈ���̽� â ���� Add
		if(gC_vs_ui.IsRunningTraceWindow())
			gC_vs_ui.CloseTraceWindow(); 
		//Sjheon 2005.11.11 Ʈ���̽� â ���� End

		// �����ε� ����Ʈ ����
		g_pTopView->SetBlindEffect(false);

		if (g_pPlayer->GetActionCount()==g_pPlayer->GetActionCountMax())		
		{	
			static BYTE a = 0;
			static BYTE count = 0;

			int second = g_pPlayer->GetDeadDelayLast();
			//char str[128];

			// 30~20
			int blackValue = 20;
			if (second >= 3)
			{
				//blackValue = max(0, (g_pClientConfig->DELAY_PLAYER_DEAD/1000 - (second-5)*2)) * 2;
				blackValue = min(31, second+20);
			}

			// �״� ������ ���� ��쿡 ȭ���� �˰�...
			g_pTopView->SetFadeStart(blackValue, 0, 0, 5,5,5);

			/*
			static bool b = true;

			if(b) {
				g_pTopView->SetFadeStart(1, 0, 0, 255,255,255);
				b = false;
			}
			*/

			// 6 frame�� �����ش�.
			if ((a & 0x01)==0)
			{
				//if (second==0)
				{
					//sprintf(str, "[SPACE]�� ������ �ǻ�Ƴ� �� �ֽ��ϴ�.", second);
					//g_pBack->GDI_Text(270,400, str, RGB(230,230,230));
				}
				//else
				{
				//	sprintf(str, (*g_pGameStringTable)[STRING_RESURRECTION_AFTER_SECONDS].GetString(), second);
					//strcpy(str, "... Please Wait for Your Resurrection ...");
				//	g_pBack->GDI_Text(301,401, str, RGB(0,0,0)); 
				//	g_pBack->GDI_Text(300,400, str, RGB(230,230,230));
				}
				
				// 6�ʰ� ���������� ��Ȱ��ư�� ����.
				if (second < 4
					&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
				{
					bool bEnforceResurrect = g_pPlayer->GetEnforceResurrect();

					// FixMe Please oh shit by ssuk
					if( (g_pWarManager->IsExist( g_pZone->GetID() ) || g_pZone->GetID() == 71 || g_pZone->GetID() == 72 || g_pZone->GetID() == 73) && gC_vs_ui.IsRunningBloodBibleStatus() )
						gC_vs_ui.RequestShrineMinimap( g_pClientConfig->DELAY_PLAYER_RESURRECT_TIMEOUT );
					
					
					DWORD timer = g_pClientConfig->DELAY_PLAYER_RESURRECT_TIMEOUT;

					if(g_pZone->IsSurvivalZone())
						timer = g_pClientConfig->DELAY_PLAYER_RESURRECT_TIMEOUT_SURVIVALZONE;
					else if(g_pPlayer->GetIsInSiegeWar())
						timer = g_pClientConfig->DELAY_PLAYER_RESURRECT_TIMEOUT_SIEGEWAR;

					gC_vs_ui.RequestDie(timer, bEnforceResurrect);

					g_pTempInformation->SetMode(TempInformation::MODE_WAIT_RESURRECT);
					bool bResurrect = false, bElixir= false, bEternity= false, IsSiegeAttacker= false, IsSkillFromOther = false;

					// 2005, 1, 18, sobeit add start - ����Ʈ action���� ���� ��쿣 �Ʒ��� üũ ���Ѵ�.
					

					if(0 == g_pPlayer->GetResurrectZoneID() && !g_pZone->IsSurvivalZone())
					// 2005, 1, 18, sobeit add end
					{

						if( g_pInventory->FindItem(ITEM_CLASS_RESURRECT_ITEM, 0) != NULL )
							bResurrect = true;
					
						if(g_pInventory->FindItem(ITEM_CLASS_RESURRECT_ITEM, 1) != NULL )
							bElixir = true;

						//bEternity = (*g_pSkillInfoTable)[SKILL_ETERNITY].IsEnable();						
						bEternity = g_pSkillAvailable->IsEnableSkill( SKILL_ETERNITY ) &&
									(*g_pSkillInfoTable)[SKILL_ETERNITY].IsAvailableTime() 
									// 2004, 11, 11, sobeit add start - ���� �˻絵 �Ѵ�.. ���..g_char_slot_ingame.DOMAIN_HEAL�� ����? -_-; ���� ����..�Ѥ�;
									&& (*g_pSkillManager)[SKILLDOMAIN_HEAL].GetDomainLevel() >= (*g_pSkillInfoTable)[SKILL_ETERNITY].GetLearnLevel()
									&& !bEnforceResurrect;
									// 2004, 11, 11, sobeit add end
						IsSiegeAttacker =	g_pZone->GetPKType() == PK_TYPE_SIEGE &&
											g_pPlayer->GetIsAttackerInSiegeWar()
										 /*( g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) ||
										 g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )*/;
					}

					IsSkillFromOther = g_pPlayer->HasEffectStatus(EFFECTSTATUS_REQUEST_RESURRECT) || g_pPlayer->HasEffectStatus(EFFECTSTATUS_unknow211_89) ;
					
					gC_vs_ui.RequestResurrect(bResurrect, bElixir, bEternity, IsSiegeAttacker, IsSkillFromOther);
				}
			}

			static DWORD lastTime = g_CurrentTime;
			if (g_CurrentTime - lastTime >= g_UpdateDelay)
			{
				if (++count==6) { count = 0; a++; }
				lastTime = g_CurrentTime;
			}			
		}
	}

	//-----------------------------------------------------------------
	// Mouse Cursor ��ġ�� image�� �����ѵα�
	//-----------------------------------------------------------------
	//
	// FullScreen�̰�...
	// �ٷ� ���� FPS�� ���FPS �Ѱ踦 ���� ���....
	// Ŀ�� ��� ��ġ�� ����Ѵ�.
	// 	
	{
		Phoenix_FrameProbeStageScope fpStage("Draw.SmoothCursorSetup", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		if (g_pUserOption->UseSmoothCursor)
		{
			if (CDirectDraw::IsFullscreen() && g_FrameRate > LIMIT_FPS_FOR_CURSOR)
			{
				GetCursorPos(&point);
				ScreenToClient(g_hWnd, &point);
			
				// ui�� mouse��ǥ ����
				gC_vs_ui.MouseControl(M_MOVING, point.x, point.y);

				// ������ ���� ����
				MOUSEPOINTER_INFO mp_info;
				gC_vs_ui.GetCurrentMousePointerInfo(mp_info);

				g_pCursorSurface->Init(2, mp_info.width, mp_info.height);
				//DEBUG_ADD_FORMAT("[MouseCursor] %d,%d, %d,%d", mp_info.width, mp_info.height, mp_info.rx, mp_info.ry);
			
				POINT tempPoint;
				tempPoint.x = mp_info.x;
				tempPoint.y = mp_info.y;
				g_pCursorSurface->Store(0, g_pBack, &tempPoint);
			
				g_bSmoothCursor = true;
			}
			else
			{
				g_bSmoothCursor = false;
			}
		}
		else
		{
			//GetCursorPos(&point);
			
			// ui�� mouse��ǥ ����
			//gC_vs_ui.MouseControl(M_MOVING, point.x, point.y);
		}
	}

	if (CDirect3D::IsHAL())
	{
		//-----------------------------------------------------------------
		// Mouse �׸���
		//-----------------------------------------------------------------		
		if (!g_pTopView->IsDrawRequest())
		{
			Phoenix_FrameProbeStageScope fpStage("Draw.HALMousePointer", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			gC_vs_ui.DrawMousePointer();
		}		
	}
	else
	{
		// â��忡�� 3D���� ���� ��쿡..
		// �� �̰� �ϴϱ� ��������? - -;
		//HDC hdc;
		//g_pBack->GetSurface()->GetDC(&hdc);
		//g_pBack->GetSurface()->ReleaseDC(hdc);
	}

	//-----------------------------------------------------------------
	// Debug Information���
	//-----------------------------------------------------------------
	__BEGIN_PROFILE("DrawDebugInfo")
	{
		Phoenix_FrameProbeStageScope fpStage("Draw.DebugInfo", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);

	PrintInfo* pPrintInfo = &gpC_base->m_chatting_pi;

		if (g_pOperatorOption->bSlideScreenShot)
		{
			if (g_bSlideRectSelect)
			{
				g_rectScreenShot.right = g_x;
				g_rectScreenShot.bottom = g_y;
			}

			RECT rect = g_rectScreenShot;

			g_pBack->DrawRect( &rect, CDirectDraw::Color(3, 30, 10) );
		}

#ifdef	OUTPUT_DEBUG

		//----------------------------------------------------------------
		// UDP packet test
		//----------------------------------------------------------------
		/* �ڵ����� ����Դ� ��� ���� --;
		#ifdef __METROTECH_TEST__

			if (g_pPlayer!=NULL)
				
			{
				if (g_iAutoHealPotion && !g_pPlayer->IsVampire())
				{
					RECT rect = 
					{ 
						2,
						448,
						10,
						455
					};

					if(g_iAutoHealPotion == 1)
						g_pBack->FillRect(&rect, CDirectDraw::Color(30, 10, 10));
					else if(g_iAutoHealPotion == 2)
						g_pBack->FillRect(&rect, CDirectDraw::Color(30, 20, 10));
					else if(g_iAutoHealPotion == 3)
						g_pBack->FillRect(&rect, CDirectDraw::Color(30, 10, 20));
				}

				if (g_bAutoManaPotion && !g_pPlayer->IsVampire())
				{
					RECT rect = 
					{ 
						12,
						448,
						20,
						455
					};

					g_pBack->FillRect(&rect, CDirectDraw::Color(10, 10, 30));
				}

				if (g_iAutoReload && g_pPlayer->IsSlayer())
				{
//					char sz_temp[3];
//					wsprintf(sz_temp, "%d", g_iAutoReload-1);
//					g_PrintColorStr(22, 448, sz_temp, gpC_base->m_chatting_pi, RGB_GREEN);

					RECT rect = 
					{ 
						22,
						448,
						30,
						455
					};

					g_pBack->FillRect(&rect, CDirectDraw::Color(10, 30, 10));

					rect.left++;
					rect.right--;
					rect.bottom++;

					for(int i = 0; i < g_iAutoReload-1; i++)
					{
						rect.bottom -= 2;
						rect.top = rect.bottom -1;
						g_pBack->FillRect(&rect, CDirectDraw::Color(0, 0, 0));
					}
				}

				if (g_bLight)
				{
					RECT rect = 
					{ 
						32,
						448,
						40,
						455
					};
					
					g_pBack->FillRect(&rect, CDirectDraw::Color(10, 30, 30));
				}

				if (g_iSpeed)
				{
					RECT rect = 
					{ 
						42,
						448,
						50,
						455
					};

					if(g_iSpeed < 0)
						g_pBack->FillRect(&rect, CDirectDraw::Color(30, 0, 0));
					else
						g_pBack->FillRect(&rect, CDirectDraw::Color(30, 30, 30));
					
					rect.left++;
					rect.right--;
					rect.bottom++;
					
					for(int i = 0; i < abs(g_iSpeed)-1; i++)
					{
						rect.bottom -= 2;
						rect.top = rect.bottom -1;
						g_pBack->FillRect(&rect, CDirectDraw::Color(0, 0, 0));
					}
				}
			}

			if (outputInfo)
			{
				const int numValue = 5;
				int value[numValue] = 
				{
					g_UDPTest.UDPPacketAvailable,
					g_UDPTest.UDPPacketReceive,
					g_UDPTest.UDPPacketRead,
					g_UDPTest.UDPPacketExecute,
					g_UDPTest.UDPPacketSend
				};

				WORD color[numValue] =
				{
					CDirectDraw::Color(10, 20, 20),
					CDirectDraw::Color(20, 20, 20),
					CDirectDraw::Color(30, 20, 20),
					CDirectDraw::Color(20, 30, 20),
					CDirectDraw::Color(20, 20, 30)
				};

				for (int v=0; v<numValue; v++)
				{
					RECT rect = 
					{ 
						210,
						455 + v*8,
						210 + (value[v]%100)*4,
						455 + v*8 + 8
					};

					g_pBack->DrawRect(&rect, color[v]);
				}

				if (outputInfo)
				{
					if (!CDirect3D::IsHAL())
					{
						g_SetFL2Surface( g_pBack->GetSurface() );
					}

					const COLORREF color = RGB(28<<3, 28<<3, 28<<3);				
					pPrintInfo->text_color	= color;
		
					char str[10];
					for (int i=0; i<numValue; i++)
					{
						sprintf(str, "%d", value[i]);
						g_Print(200 + i*40, 455 + i*8, str, pPrintInfo);
					}				

					if (!CDirect3D::IsHAL())
					{
						g_SetFL2Surface( g_pLast->GetSurface() );
					}
				}
			}
		#endif
		*/


		if (outputInfo)//!g_pDXInput->KeyDown(DIK_RSHIFT))
		{
			g_pTopView->DrawDebugInfo( g_pBack );

			if (!CDirect3D::IsHAL())
			{
				g_SetFL2Surface( g_pBack->GetSurface() );
			}

			///*
			//-----------------------------------------------------------------
			//							Lock
			//-----------------------------------------------------------------
			//if (!g_pBack->Lock()) return;

			// Surface�� ������ �����صд�.
			//S_SURFACEINFO		SurfaceInfo;
			//SetSurfaceInfo(&SurfaceInfo, g_pBack->GetDDSD());
		
			if (g_bPutMessage && g_pDebugMessage!=NULL)
			{
				int strY = 565;
				
				for (int c=g_pDebugMessage->GetSize()-1; c>=0; c--)
				{
					if ((*g_pDebugMessage)[c][0] != NULL)
					{
						//g_pBack->GDI_Text(10,strY, g_DebugMessage[c], RGB(230,230,230));
						//gC_font.PrintStringNoConvert(&SurfaceInfo, (*g_pDebugMessage)[c], 10,strY, CDirectDraw::Color(28,28,28));

						const COLORREF debugColor = RGB(28<<3, 28<<3, 28<<3);				
						pPrintInfo->text_color	= debugColor;
			
						g_Print(10, strY, (*g_pDebugMessage)[c], pPrintInfo);
						strY-=15;
					}
				}

				// Debug Log Filename ����ϱ�
				if (g_pDebugMessage->GetFilename()!=NULL)
				{
					sprintf(str, "LogFile : %s", g_pDebugMessage->GetFilename());
					//gC_font.PrintStringNoConvert(&SurfaceInfo, str, 10,580, CDirectDraw::Color(31,11,11));

					const COLORREF txColor = RGB(31<<3, 11<<3, 11<<3);										
					pPrintInfo->text_color	= txColor;
			
					g_Print(10, 580, str, pPrintInfo);
				}				
			}	
			//*/
			
			

			//if(g_pLoadingThread!=NULL && g_pLoadingThread->IsWorking())
			
			/*
			if(MWorkThreadGlobal::Instance()->IsWorking())
			{		
				sprintf(str, "<<< Loading >>>");
				//g_pBack->GDI_Text(700,580, str, 0xFFFFFF);
				//gC_font.PrintStringNoConvert(&SurfaceInfo, str, 300,5, CDirectDraw::Color(31,31,31));
				const COLORREF txColor = RGB(31<<3, 31<<3, 31<<3);				

				pPrintInfo->text_color	= txColor;
				
				g_Print(300, 5, str, pPrintInfo);
			}
			*/
			
			// Missile����
			///*	 
			if (g_pPlayer->GetSpecialActionInfo() != ACTIONINFO_NULL)
			{
				sprintf(str, "[%d] %s", g_pPlayer->GetSpecialActionInfo(), (*g_pActionInfoTable)[g_pPlayer->GetSpecialActionInfo()].GetName());
				
				//g_pBack->GDI_Text(550,10, str, RGB(20,20,20));
				//g_pBack->GDI_Text(551,11, str, RGB(120,255,120));
				//gC_font.PrintStringNoConvert(&SurfaceInfo, str, 500,10, CDirectDraw::Color(2,2,2));
				//gC_font.PrintStringNoConvert(&SurfaceInfo, str, 501,11, CDirectDraw::Color(16,31,16));


				const COLORREF bkColor = RGB(0, 0, 0);
				const COLORREF txColor = RGB(16<<3, 31<<3, 16<<3);				

				pPrintInfo->text_color	= bkColor;
				g_Print(500, 10, str, pPrintInfo);

				pPrintInfo->text_color	= txColor;
				g_Print(501, 11, str, pPrintInfo);				
			}	
			//*/

			// ���� ��
			sprintf(str, "Money : %d", (*g_pMoneyManager).GetMoney());
			
			//g_pBack->GDI_Text(550,10, str, RGB(20,20,20));
			//g_pBack->GDI_Text(551,11, str, RGB(120,255,120));
			//gC_font.PrintStringNoConvert(&SurfaceInfo, str, 500,10, CDirectDraw::Color(2,2,2));
			//gC_font.PrintStringNoConvert(&SurfaceInfo, str, 501,11, CDirectDraw::Color(16,31,16));
			
			const COLORREF bkMoneyColor = RGB(0, 0, 0);
			const COLORREF txMoneyColor = RGB(16<<3, 31<<3, 16<<3);				

			pPrintInfo->text_color	= bkMoneyColor;
			g_Print(500, 30, str, pPrintInfo);

			pPrintInfo->text_color	= txMoneyColor;
			g_Print(501, 31, str, pPrintInfo);

			sprintf(str, "QuestColorset = %d", g_pClientConfig->QuestItemColorSet);
			pPrintInfo->text_color	= txMoneyColor;
			g_Print(501, 51, str, pPrintInfo);

			//-----------------------------------------------------------------
			//							UnLock
			//-----------------------------------------------------------------
			//g_pBack->Unlock();
		

			/*
			// �ӽ÷� item ���� �����ֱ�
			if (gpC_mouse_pointer->GetPickUpItem() != NULL)
			{
				sprintf(str, "%d", gpC_mouse_pointer->GetPickUpItem()->GetNumber());
				g_pBack->GDI_Text(g_x+1,g_y+1, str, RGB(20,20,20));
				g_pBack->GDI_Text(g_x,g_y, str, 0xFFFFFF);
			}
			*/

			if (!CDirect3D::IsHAL())
			{
				g_SetFL2Surface( g_pLast->GetSurface() );
			}
		}
	#endif

	/*
	if (outputInfo || (*g_pUserOption).DrawFPS)
	{
		//-----------------------------------------------------------------
		// FPS ���	
		//-----------------------------------------------------------------
		if (CDirect3D::IsHAL())
		{
			sprintf(str, "%d FPS(HAL)", g_FrameRate);	
		}
		else
		{
			sprintf(str, "%d FPS", g_FrameRate);	
		}
		g_pBack->GDI_Text(11,11, str, RGB(20,20,20));
		g_pBack->GDI_Text(10,10, str, 0xFFFFFF);
	}
	*/

	}
	__END_PROFILE("DrawDebugInfo")

	//---------------------------------------------------------------------
	//
	// [ TEST CODE ] - Sword Domain�� Skill���� ������ �۾��Ѵ�.
	//
	//---------------------------------------------------------------------
	/*
	MSkillDomain& swordDomain = g_SkillManager[SKILLDOMAIN_SWORD];

	//---------------------------------------------------------------------
	// ��� skill�� ����ٰ� ǥ���Ѵ�.
	//---------------------------------------------------------------------
	//swordDomain.LearnSkill( SKILL_DOUBLE_IMPACT );
	//swordDomain.LearnSkill( SKILL_TRIPLE_SLASHER );
	//swordDomain.LearnSkill( SKILL_RAINBOW_SLASHER );
	//swordDomain.LearnSkill( SKILL_HURRICANE_COMBO );

	//swordDomain.UnLearnSkill( SKILL_HURRICANE_COMBO );

	//---------------------------------------------------------------------
	// Sword Domain�� ��� ������� ����Ѵ�.
	//---------------------------------------------------------------------
	swordDomain.SetBegin();
	
	while (swordDomain.IsNotEnd())
	{
		// skill�� id�� status
		ACTIONINFO					id		= swordDomain.GetSkillID();
		MSkillDomain::SKILLSTATUS	status	= swordDomain.GetSkillStatus();

		//---------------------------------------
		// status�� ������ ����. 
		//---------------------------------------
		//	MSkillDomain::SKILLSTATUS_LEARNED		// �����.
		//	MSkillDomain::SKILLSTATUS_NEXT			// ������ ��� �� �ִ�.
		//	MSkillDomain::SKILLSTATUS_OTHER			// ������ ��� �� ����.	
		//---------------------------------------
		
		//---------------------------------------
		// id�� �˸� g_SkillInfoTable���� 
		// �� id�� skill�� ���� ������ ���� �� �ִ�.
		//---------------------------------------
		COLORREF color;
		switch (status)
		{
			case MSkillDomain::SKILLSTATUS_LEARNED :	
				color = RGB(250, 250, 250);
			break;

			case MSkillDomain::SKILLSTATUS_NEXT :	
				color = RGB(10, 10, 250);
			break;

			case MSkillDomain::SKILLSTATUS_OTHER :	
				color = RGB(120, 120, 120);
			break;
		}

		SKILLINFO_NODE& skillInfo = g_SkillInfoTable[id];
		
		int x = skillInfo.GetX()*2/3 - strlen(skillInfo.GetName())*8 + 100;
		int y = skillInfo.GetY();
		g_pBack->GDI_Text(x+1, y+1, skillInfo.GetName(), 0);
		g_pBack->GDI_Text(x, y, skillInfo.GetName(), color);

		// ����
		swordDomain.Next();
	}
	//*/

	// Ŀ�� ��� 
	//WORD color = 0xFFFF;//(rand()%2)?CDirectDraw::Color(20,20,20):CDirectDraw::Color(230,230,230);
	//g_pBack->HLine(point.x-7, point.y, 7, color);
	//g_pBack->HLine(point.x+1, point.y, 7, color);
	//g_pBack->VLine(point.x, point.y-7, 7, color);
	//g_pBack->VLine(point.x, point.y+1, 7, color);	
	//*/
	

	//-----------------------------------------------------------------
	// flip
	//-----------------------------------------------------------------
	{
		static DWORD s_LastDrawFlipTrace = 0;
		DWORD nowFlip = timeGetTime();
		if (nowFlip - s_LastDrawFlipTrace > 1000) {
			s_LastDrawFlipTrace = nowFlip;
			CT_BD("[DRAW] before Flip");
		}
		Phoenix_FrameProbeStageScope fpStage("Draw.Flip", g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
		CDirectDraw::Flip();
	}
}

//-----------------------------------------------------------------------------
// Update Draw Help
//-----------------------------------------------------------------------------
void
CGameUpdate::UpdateDrawHelp()
{
	int strX = 350;
	int strY = (g_pPlayer->IsSlayer()? 430 : 460);

	PrintInfo* pPrintInfo = &gpC_base->m_info_pi;

	for (int c=g_pHelpMessage->GetSize()-1; c>=0; c--)
	{
		if ((*g_pHelpMessage)[c][0] != NULL)
		{
			const COLORREF color = RGB(28<<3, 28<<3, 8<<3);	//CDirectDraw::Color(29,8,12);

			pPrintInfo->text_color = 0;
			g_Print(strX+1, strY+1, (*g_pHelpMessage)[c], pPrintInfo);

			pPrintInfo->text_color = color;
			g_Print(strX, strY, (*g_pHelpMessage)[c], pPrintInfo);

			strY-=20;
		}		
	}

	// 5�ʸ��� �ѹ���.. scroll
	static DWORD HelplastTime = g_CurrentTime;
	if (g_CurrentTime - HelplastTime >= g_pClientConfig->DELAY_GAMEMESSAGE)
	{
		g_pHelpMessage->Add("\0");
		HelplastTime = g_CurrentTime;
	}
}

//-----------------------------------------------------------------------------
// PacketTest
//-----------------------------------------------------------------------------
void
PacketTest()
{
	MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();

	for (int i=0; i<g_pZone->GetCreatureNumber(); i++)
	{
		MCreature* pCreature = (*iCreature).second;

		if (pCreature->GetID()!=g_pPlayer->GetID()
			&& !pCreature->IsNPC()
			&& (rand()%8==0))
		{	
			int x = pCreature->GetX();
			int y = pCreature->GetY();
			int dir = rand()%8;
			pCreature->PacketMove(x, y, dir); 
		}

		iCreature++;
	}
}
#if __CONTENTS(__AUTO_ATTACT)
// Trace Obj�� ũ�����̰� �÷��̾ �ƴҶ��� �ڵ����� ����
bool
IsTargetCanAutoAttcak()
{
	int targetObjectID = g_pPlayer->GetTraceID();
	MCreature* targetCreature = NULL;
	if(targetObjectID && g_pPlayer->IsTraceCreature())
	{
		targetCreature = g_pZone->GetCreature(targetObjectID);
		if (targetCreature && !targetCreature->IsPlayerOnly())
		{
			return true;
		}
	}
	return false;
}
#endif //__AUTO_ATTACT
//-----------------------------------------------------------------------------
// AttackMelee
//-----------------------------------------------------------------------------
/*
#include "packet\Gpackets\GCAttackMeleeOK2.h"
#include "packet\Gpackets\GCAttackMeleeOK3.h"
void
PacketAttackMelee(int user, int target)
{
	Packet* pPacket;
	
	if (user==g_pPlayer->GetID())
	{
		pPacket = new GCAttackMeleeOK1;
	}
	GCAttackMeleeOK3 packet;
	packet.setObjectID( user );
	packet.setTargetObjectID( target );

	packet.execute( g_pSocket );
}
*/
