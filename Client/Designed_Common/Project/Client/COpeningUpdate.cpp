//---------------------------------------------------------------------------
// COpeningUpdate.cpp
//---------------------------------------------------------------------------
// Opening 동영상을 보여줄때의 loop
//---------------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)
#include "Client.h"
#include "ServerInfo.h"
#include "COpeningUpdate.h"
#include "PhoenixFramePacer.h"	// plan F6 — Phoenix_ShouldSkipIntroMedia

#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

// Global
COpeningUpdate*		g_pCOpeningUpdate;


//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
COpeningUpdate::Init()
{
	// mouse event 처리
	g_pDXInput->SetMouseEventReceiver( NULL );

	// keyboard event 처리
	g_pDXInput->SetKeyboardEventReceiver( NULL );
}

//---------------------------------------------------------------------------
// Opening AVI
//---------------------------------------------------------------------------
void
COpeningUpdate::PlayMPG(char* filename)
{
	CT_Done("DIAG: COpeningUpdate::PlayMPG(filename=%s) - ENTRY", filename);

	// 2026-09-03 plan F6 — "Skip intro media" (ADV bit6, Reborn
	// Application::StartIntroMedia paritesi): secenek acikken intro
	// oynatilmaz, dogrudan ana menuye gecilir. Ayni kisa devre mekanizmasi
	// asagidaki "dosya yok" auto-skip yolunda zaten kanitli.
	if (Phoenix_ShouldSkipIntroMedia())
	{
		CT_Done("DIAG: COpeningUpdate::PlayMPG - intro skipped by OptionRenewal ADV bit6");
		SetMode(MODE_MAINMENU);
		return;
	}

	// AVI
	if (g_pAvi != NULL)
	{
		//#ifdef DEBUG_NEW
		//	operator delete ( g_pAvi, __FILE__, __LINE__ );
		//#else
			delete g_pAvi;
		//#endif
	}

	g_pAvi = new CAVI;
	if(!g_pAvi->OpenMPG(g_hWnd, filename, 640, 480)) 
	{
		char str[256];
		sprintf(str, "Not Found %s", filename);
		CT_Done("DIAG: COpeningUpdate::PlayMPG - %s (Auto-skipping to MAINMENU)", str);
		// MessageBox(g_hWnd, str, "Error!", MB_OK); // Removed blocking messagebox
		
		SetMode(MODE_MAINMENU);
		return;
	}
	g_pAvi->Play();
}


//-----------------------------------------------------------------------------
// UpdateOpening
//-----------------------------------------------------------------------------
// 접속 전..
//-----------------------------------------------------------------------------
void		
COpeningUpdate::Update()
{
	static uint updateCount = 0;
	if (++updateCount % 100 == 0) CT_Done("DIAG: COpeningUpdate::Update() hit (count:%u)", updateCount);

	//------------------------------------------
	// Input
	//------------------------------------------	
	extern bool	g_bTestMode;
	if (g_bActiveGame
#ifdef OUTPUT_DEBUG
		|| g_bTestMode
#endif
		)
	{
		UpdateInput();
		UpdateMouse();

		ProcessInput();
	}

	if (!g_bActiveApp)
		return;

	//------------------------------------------
	// Draw
	//------------------------------------------
	//if (g_bActiveGame)
	//	UpdateLoginDraw();	
}


//---------------------------------------------------------------------------
// Update Opening Input
//---------------------------------------------------------------------------
void
COpeningUpdate::ProcessInput()
{
	static uint inputCount = 0;
	if (++inputCount % 100 == 0) CT_Done("DIAG: COpeningUpdate::ProcessInput() hit (count:%u)", inputCount);
}
