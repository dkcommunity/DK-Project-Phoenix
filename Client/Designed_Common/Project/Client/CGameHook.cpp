//-----------------------------------------------------------------------------
// CGameUpdate.cpp
//-----------------------------------------------------------------------------
// 실제 게임을 진행하는 부분
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "CGameHook.h"
#include <Windows.h>
#include <MMSystem.h>
#include <string>
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


CGameHook::CGameHook()
{

}

void CGameHook::Init(MPlayer* pMainPlayer)
{
	// pMainPlayer->CalculateDistance()
}

void CGameHook::Update()
{

}
