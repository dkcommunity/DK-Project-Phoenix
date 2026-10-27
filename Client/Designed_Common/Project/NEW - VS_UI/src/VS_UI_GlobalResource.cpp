// VS_UI_GlobalResource.cpp

#include "client_PCH.h"
#include "VS_UI_GlobalResource.h"
#include "VS_UI_filepath.h"
#include "ClientFunction.h"
#include "UserOption.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
C_GLOBAL_RESOURCE	*	gpC_global_resource;
bool	gbl_wood_skin;

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// C_GLOBAL_RESOURCE
//
// Load all resources.
//-----------------------------------------------------------------------------
C_GLOBAL_RESOURCE::C_GLOBAL_RESOURCE()
{
	//LoadAniButton();
//	LoadAssembleBox();
	//LoadCommonImage();
//	LoadCommonButton();
	m_pC_assemble_box_spk = NULL;
	m_pC_assemble_box_button_spk = NULL;
//	m_pC_assemble_box_etc_spk = NULL;
	m_pC_scroll_bar_spk = NULL;
	m_pC_box_spk = NULL;
	m_pC_info_spk = NULL;
	m_pC_common_button_spk = NULL;
	m_New_NpcDialog_spk = NULL;
	m_pC_NpcIcon_spk = NULL;
	LoadAssemble();
}

//-----------------------------------------------------------------------------
// ~C_GLOBAL_RESOURCE
//
// Free all resources.
//-----------------------------------------------------------------------------
C_GLOBAL_RESOURCE::~C_GLOBAL_RESOURCE()
{
	//FreeAniButton();
//	FreeAssembleBox();
	//FreeCommonImage();
//	FreeCommonButton();
	//FreeCommonInTitle();
	FreeAssemble();
}

//-----------------------------------------------------------------------------
// LoadAssemble
//
// 
//-----------------------------------------------------------------------------
void C_GLOBAL_RESOURCE::LoadAssemble()
{
	FreeAssemble();

	m_pC_scroll_bar_spk		= new C_SPRITE_PACK(SPK_SCROLL_BAR);
	m_pC_box_spk			= new C_SPRITE_PACK(SPK_BOX);
	m_pC_common_button_spk	= new C_SPRITE_PACK(SPK_COMMON_BUTTON);
	//m_pC_hays_spk			= new C_SPRITE_PACK("data\\ui\\spk\\new\\renewalfriend\\00000.spk");
	//m_Tmall_spk				= new C_SPRITE_PACK(_ROOT"spk\\shopmall.spk");
	//m_PayEvent_spk			= new C_SPRITE_PACK(_ROOT"spk\\pay.spk");//?????SPK????
	m_New_NpcDialog_spk = new C_SPRITE_PACK(SPK_NEW_NPC_DIALOG);
	m_pC_NpcIcon_spk = new C_SPRITE_PACK(SPK_NPC_ICON);
	//m_New_NpcDialog_spk		= new C_SPRITE_PACK(_ROOT"spk\\npcdialog.spk");//??NPC???

	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		m_pC_info_spk = new C_SPRITE_PACK(SPK_INFO_SLAYER);
		m_pC_assemble_box_spk = new C_SPRITE_PACK(SPK_ASSEMBLE_BOX_SLAYER);
		m_pC_assemble_box_button_spk = new C_SPRITE_PACK(SPK_ASSEMBLE_BOX_BUTTON_SLAYER);
		break;

	case RACE_VAMPIRE:
		m_pC_assemble_box_spk = new C_SPRITE_PACK(SPK_ASSEMBLE_BOX_VAMPIRE);
		m_pC_assemble_box_button_spk = new C_SPRITE_PACK(SPK_ASSEMBLE_BOX_BUTTON_VAMPIRE);
		m_pC_info_spk = new C_SPRITE_PACK(SPK_INFO_VAMPIRE);
		break;

	case RACE_OUSTERS:
		m_pC_assemble_box_spk = new C_SPRITE_PACK(SPK_ASSEMBLE_BOX_OUSTERS);
		m_pC_assemble_box_button_spk = new C_SPRITE_PACK(SPK_ASSEMBLE_BOX_BUTTON_OUSTERS);
		m_pC_info_spk = new C_SPRITE_PACK(SPK_INFO_OUSTERS);
		break;
	}	
}

//-----------------------------------------------------------------------------
// FreeAssemble
//
// 
//-----------------------------------------------------------------------------
void C_GLOBAL_RESOURCE::FreeAssemble()
{
	DeleteNew(m_pC_box_spk);
	DeleteNew(m_pC_scroll_bar_spk);

	//std::ofstream file("Error.log", std::ios::out | std::ios::app);
	//file << "*** m_pC_scroll_bar_spk oky" << std::endl;
	//file.close();

	DeleteNew(m_pC_assemble_box_spk);

	//std::ofstream file2("Error.log", std::ios::out | std::ios::app);
	//file2 << "*** m_pC_assemble_box_spk oky" << std::endl;
	//file2.close();

	DeleteNew(m_pC_assemble_box_button_spk);
	DeleteNew(m_pC_info_spk);
	DeleteNew(m_pC_common_button_spk);
	DeleteNew(m_New_NpcDialog_spk);
	DeleteNew(m_pC_NpcIcon_spk);
	
//	if(m_pC_assemble_box_etc_spk)
//	{
//		DeleteNew(m_pC_assemble_box_etc_spk);
//		m_pC_assemble_box_etc_spk = NULL;
//	}
}

//-----------------------------------------------------------------------------
// Load/Free CommonButton
//
// 
//-----------------------------------------------------------------------------
//void C_GLOBAL_RESOURCE::LoadCommonButton()
//{
//	m_pC_common_button = new C_SPRITE_PACK(SPK_COMMON_BUTTON);
//}
//
//void C_GLOBAL_RESOURCE::FreeCommonButton()
//{
//	DeleteNew(m_pC_common_button);
//}

//-----------------------------------------------------------------------------
// Load/Free CommonInTitle
//
// 
//-----------------------------------------------------------------------------
/*
void C_GLOBAL_RESOURCE::LoadCommonInTitle()
{
	m_pC_common_in_title = new C_SPRITE_PACK(SPK_COMMON_IN_TITLE);
}

void C_GLOBAL_RESOURCE::FreeCommonInTitle()
{
	DeleteNew(m_pC_common_in_title);
}*/
/*
//-----------------------------------------------------------------------------
// LoadAniButton
//
// 
//-----------------------------------------------------------------------------
void C_GLOBAL_RESOURCE::LoadAniButton()
{
	m_pC_ao_ani_button = new C_ANI_OBJECT(SPK_BUTTON1, FRR_BUTTON1);
}

//-----------------------------------------------------------------------------
// FreeAniButton
//
// 
//-----------------------------------------------------------------------------
void C_GLOBAL_RESOURCE::FreeAniButton()
{
	DeleteNew(m_pC_ao_ani_button);
}
*/
//-----------------------------------------------------------------------------
// LoadAssembleBox
//
// ??????? load.
//-----------------------------------------------------------------------------
/*void C_GLOBAL_RESOURCE::LoadAssembleBox()
{
	m_pic_box_back = new Tga;
	if (!m_pic_box_back->Load(IMAGE_BOXBACK))
		_Error(FAILED_JOB);

	m_pC_box_line_spk = new C_SPRITE_PACK(SPK_BOX_LINE);

	m_pC_ok_spk = new C_SPRITE_PACK(SPK_OK);
}*/

//-----------------------------------------------------------------------------
// FreeAssembleBox
//
// 
//-----------------------------------------------------------------------------
/*void C_GLOBAL_RESOURCE::FreeAssembleBox()
{
	DeleteNew(m_pC_ok_spk);
	DeleteNew(m_pC_box_line_spk);
	DeleteNew(m_pic_box_back);
}
*/
/*
//-----------------------------------------------------------------------------
// LoadCommonImage
//
// 
//-----------------------------------------------------------------------------
void C_GLOBAL_RESOURCE::LoadCommonImage()
{
	m_pC_common_spk = new C_SPRITE_PACK(SPK_COMMON);
}

//-----------------------------------------------------------------------------
// FreeCommonImage
//
// 
//-----------------------------------------------------------------------------
void C_GLOBAL_RESOURCE::FreeCommonImage()
{
	DeleteNew(m_pC_common_spk);
}*/

void C_GLOBAL_RESOURCE::DrawOutBox(Rect &rect)
{
	DrawOutBox(rect.x, rect.y, rect.w, rect.h);
}

void C_GLOBAL_RESOURCE::DrawOutBox(int x, int y, int w, int h)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawOutBoxLocked(x, y, w, h);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawOutBoxLocked(Rect &rect)
{
	DrawOutBoxLocked(rect.x, rect.y, rect.w, rect.h);
}


void C_GLOBAL_RESOURCE::DrawOutBoxLocked(int x, int y, int w, int h)
{
	Rect rect;

	rect.Set(0, 0, w-m_pC_box_spk->GetWidth(OB_RIGHT), h-m_pC_box_spk->GetHeight(OB_DOWN));
	m_pC_box_spk->BltLockedClip(x, y, rect, OB_MAIN);

	rect.Set(0, 0, m_pC_box_spk->GetWidth(OB_RIGHT), h);
	m_pC_box_spk->BltLockedClip(x+w-m_pC_box_spk->GetWidth(OB_RIGHT), y, rect, OB_RIGHT);

	rect.Set(0, 0, w, m_pC_box_spk->GetHeight(OB_DOWN));
	m_pC_box_spk->BltLockedClip(x, y+h-m_pC_box_spk->GetHeight(OB_DOWN), rect, OB_DOWN);

	m_pC_box_spk->BltLocked(x+w-m_pC_box_spk->GetWidth(OB_RIGHTDOWN), y+h-m_pC_box_spk->GetHeight(OB_RIGHTDOWN), OB_RIGHTDOWN);

}

void C_GLOBAL_RESOURCE::DrawInBox(Rect &rect)
{
	DrawInBox(rect.x, rect.y, rect.w, rect.h);
}

void C_GLOBAL_RESOURCE::DrawInBox(int x, int y, int w, int h)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawInBoxLocked(x, y, w, h);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawInBoxLocked(Rect &rect)
{
	DrawInBoxLocked(rect.x, rect.y, rect.w, rect.h);
}


void C_GLOBAL_RESOURCE::DrawInBoxLocked(int x, int y, int w, int h)
{
	Rect rect;
	
	rect.Set(0, 0, w-m_pC_box_spk->GetWidth(IB_RIGHT), h-m_pC_box_spk->GetHeight(IB_DOWN));
	m_pC_box_spk->BltLockedClip(x, y, rect, IB_MAIN);
	
	rect.Set(0, 0, m_pC_box_spk->GetWidth(IB_RIGHT), h);
	m_pC_box_spk->BltLockedClip(x+w-m_pC_box_spk->GetWidth(IB_RIGHT), y, rect, IB_RIGHT);
	
	rect.Set(0, 0, w, m_pC_box_spk->GetHeight(IB_DOWN));
	m_pC_box_spk->BltLockedClip(x, y+h-m_pC_box_spk->GetHeight(IB_DOWN), rect, IB_DOWN);
	
	m_pC_box_spk->BltLocked(x+w-m_pC_box_spk->GetWidth(IB_RIGHTDOWN), y+h-m_pC_box_spk->GetHeight(IB_RIGHTDOWN), IB_RIGHTDOWN);
	
}

void C_GLOBAL_RESOURCE::DrawDialog(Rect &rect, bool alpha)
{
	DrawDialog(rect.x, rect.y, rect.w, rect.h, alpha);
}

void C_GLOBAL_RESOURCE::DrawDialog(int x, int y, int w, int h, bool alpha)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawDialogLocked(x, y, w, h, alpha);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawDialogLocked(Rect &rect, bool alpha)
{
	DrawDialogLocked(rect.x, rect.y, rect.w, rect.h, alpha);
}


void C_GLOBAL_RESOURCE::DrawDialogLocked(int x, int y, int w, int h, bool alpha)
{
	Rect rect;

	rect.Set(0, 0, w, h);
	//if(g_eRaceInterface == RACE_OUSTERS)
	//{
	//	rect.w -= 12;
	//	rect.h -= 10;
	//}
	// 2026-09-03 plan F4 ?? alpha dali restore edildi (Reborn parity):
	// VS6'da yorumlanmis "silent-disable" blok; DefaultAlpha/ALPHA_DEPTH
	// tuketicisi olmadigi icin opacity slider'i gorsel etkisizdi. Ayni
	// mekanizma DrawTempChatBoxLocked'da AKTIF ve calisiyor (AB_BACK_ALPHA2
	// + DrawAlphaBox) ?? sprite altyapisi kanitli. Race parity: uc irkin
	// rect/renkleri orijinal bloktan aynen.
	if(alpha)
	{
		RECT alpha_rect;
		switch(g_eRaceInterface)
		{
		case RACE_SLAYER:
			SetRect(&alpha_rect, x+5, y+12, x+rect.w-4, y+rect.h-10);
			DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_VAMPIRE:
			SetRect(&alpha_rect, x+10, y+9, x+rect.w-7, y+rect.h-8);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_OUSTERS:
			SetRect(&alpha_rect, x+14, y+13, x+rect.w-3, y+rect.h-3);
			DrawAlphaBox(&alpha_rect, 0, 4, 0, g_pUserOption->ALPHA_DEPTH);
			break;			
		}
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA);
	}
	else
	{
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, AB_BACK);
	}
	//OutputDebugString("party 3.004\n");
	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT), h);
	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT), y, rect, AB_RIGHT);
	//OutputDebugString("party 3.005\n");
	rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN));
	m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN), rect, AB_DOWN);
	//OutputDebugString("party 3.006\n");
	m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN), AB_RIGHTDOWN);

}


void C_GLOBAL_RESOURCE::DrawNPC(Rect &rect, bool alpha)
{
	DrawNPC(rect.x, rect.y, rect.w, rect.h, alpha);
}

void C_GLOBAL_RESOURCE::DrawNPC(int x, int y, int w, int h, bool alpha)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawNPCLocked(x, y, w, h, alpha);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawNPCLocked(Rect &rect, bool alpha)
{
	DrawNPCLocked(rect.x, rect.y, rect.w, rect.h, alpha);
}


void C_GLOBAL_RESOURCE::DrawNPCLocked(int x, int y, int w, int h, bool alpha)
{
	Rect rect;
	m_New_NpcDialog_spk->BltLocked(x, y, AB_BACK);
}

void C_GLOBAL_RESOURCE::DrawDialogOnly(Rect &rect)
{
	DrawDialogOnly(rect.x, rect.y, rect.w, rect.h);
}

void C_GLOBAL_RESOURCE::DrawDialogOnly(int x, int y, int w, int h)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawDialogOnlyLocked(x, y, w, h);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawDialogOnlyLocked(Rect &rect)
{
	DrawDialogOnlyLocked(rect.x, rect.y, rect.w, rect.h);
}


void C_GLOBAL_RESOURCE::DrawDialogOnlyLocked(int x, int y, int w, int h)
{
	Rect rect;

	rect.Set(0, 0, w, h);

	m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA);

	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT), h);
	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT), y, rect, AB_RIGHT);

	rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN));
	m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN), rect, AB_DOWN);

	m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN), AB_RIGHTDOWN);

}


// dialog2

void C_GLOBAL_RESOURCE::DrawDialog2(Rect &rect, bool alpha)
{
	DrawDialog2(rect.x, rect.y, rect.w, rect.h, alpha);
}

void C_GLOBAL_RESOURCE::DrawDialog2(int x, int y, int w, int h, bool alpha)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawDialogLocked2(x, y, w, h, alpha);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawDialogLocked2(Rect &rect, bool alpha)
{
	DrawDialogLocked2(rect.x, rect.y, rect.w, rect.h, alpha);
}


void C_GLOBAL_RESOURCE::DrawDialogLocked2(int x, int y, int w, int h, bool alpha)
{
	Rect rect;

	rect.Set(0, 0, w, h);
	// 2026-09-03 plan F4 ?? alpha dali restore (Reborn parity; detay ana
	// DrawDialogLocked'daki nota bak).
	if(alpha)
	{
		RECT alpha_rect;
		switch(g_eRaceInterface)
		{
		case RACE_SLAYER:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
			DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_VAMPIRE:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_OUSTERS:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;
		}		
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA2);
	}
	else
	{
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, AB_BACK2);
	}

	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), h);
	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), y, rect, AB_RIGHT2);

	// Ousters DescDialog ???????? ???? ??? ????
	if( g_eRaceInterface == RACE_OUSTERS )
	{
		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
		m_pC_assemble_box_spk->BltLockedClip(x-12, y+11+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);

		m_pC_assemble_box_spk->BltLocked(x+13+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+11+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
	}
	else
	{
		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
		m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);

		m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
	}
}

//
//
//void C_GLOBAL_RESOURCE::DrawDialog3(Rect &rect, bool alpha)
//{
//	DrawDialog3(rect.x, rect.y, rect.w, rect.h, alpha);
//}
//
//void C_GLOBAL_RESOURCE::DrawDialog3(int x, int y, int w, int h, bool alpha)
//{
//	if(gpC_base->m_p_DDSurface_back->Lock())
//	{
//		DrawDialogLocked3(x, y, w, h, alpha);
//		gpC_base->m_p_DDSurface_back->Unlock();
//	}
//}
//
//void C_GLOBAL_RESOURCE::DrawDialogLocked3(Rect &rect, bool alpha)
//{
//	DrawDialogLocked3(rect.x, rect.y, rect.w, rect.h, alpha);
//}
//void C_GLOBAL_RESOURCE::DrawDialogLocked3(int x, int y, int w, int h, bool alpha)
//{
//	Rect rect;
//
//	rect.Set(0, 0, w-2, h-2);
//	if(alpha)
//	{
//		RECT alpha_rect;
//		switch(g_eRaceInterface)
//		{
//		case RACE_SLAYER:
//			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
//			DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
//			break;
//
//		case RACE_VAMPIRE:
//			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
//			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
//			break;
//
//		case RACE_OUSTERS:
//			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
//			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
//			break;
//		}		
//		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA2);
//	}
//	else
//	{
//		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, AB_BACK2);
//	}
//
//	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), h);
//	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), y, rect, AB_RIGHT2);
//
//	// Ousters DescDialog ???????? ???? ??? ????
//	if( g_eRaceInterface == RACE_OUSTERS )
//	{
//		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
//		m_pC_assemble_box_spk->BltLockedClip(x-12, y+11+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);
//
//		m_pC_assemble_box_spk->BltLocked(x+13+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+11+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
//	}
//	else
//	{
//		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
//		m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);
//
//		m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
//	}
//}

void C_GLOBAL_RESOURCE::DrawDialogOnly2(Rect &rect)
{
	DrawDialogOnly2(rect.x, rect.y, rect.w, rect.h);
}

void C_GLOBAL_RESOURCE::DrawDialogOnly2(int x, int y, int w, int h)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawDialogOnlyLocked2(x, y, w, h);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawDialogOnlyLocked2(Rect &rect)
{
	DrawDialogOnlyLocked2(rect.x, rect.y, rect.w, rect.h);
}


void C_GLOBAL_RESOURCE::DrawDialogOnlyLocked2(int x, int y, int w, int h)
{
	Rect rect;

	rect.Set(0, 0, w, h);

	m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA2);

	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), h);
	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), y, rect, AB_RIGHT2);

	rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
	m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);

	m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);

}




void C_GLOBAL_RESOURCE::DrawDialog4(Rect &rect, bool alpha)
{
	DrawDialog2(rect.x, rect.y, rect.w, rect.h, alpha);
}

void C_GLOBAL_RESOURCE::DrawDialog4(int x, int y, int w, int h, bool alpha)
{
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawDialogLocked2(x, y, w, h, alpha);
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_GLOBAL_RESOURCE::DrawDialogLocked4(Rect &rect, bool alpha)
{
	DrawDialogLocked2(rect.x, rect.y, rect.w, rect.h, alpha);
}


void C_GLOBAL_RESOURCE::DrawDialogLocked4(int x, int y, int w, int h, bool alpha)
{
	Rect rect;

	rect.Set(0, 0, w, h);
	// 2026-09-03 plan F4 ¡ª alpha dali restore (Reborn parity; detay ana
	// DrawDialogLocked'daki nota bak).
	if(alpha)
	{
		RECT alpha_rect;
		switch(g_eRaceInterface)
		{
		case RACE_SLAYER:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
			DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_VAMPIRE:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-7);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_OUSTERS:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h-1);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;
		}		
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA2);
	}
	else
	{
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, AB_BACK2);
	}

	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), h);
	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), y, rect, AB_RIGHT2);

	// Ousters DescDialog ???????? ???? ??? ????
	if( g_eRaceInterface == RACE_OUSTERS )
	{
		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN4));
		m_pC_assemble_box_spk->BltLockedClip(x-3, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN4), rect, AB_DOWN4);

		m_pC_assemble_box_spk->BltLocked(x+3+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN4), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN4), AB_RIGHTDOWN4);
	}
	else
	{
		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
		m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);

		m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
	}
}


void C_GLOBAL_RESOURCE::DrawTempChatBoxLocked(int x, int y, int w, int h, bool alpha)
{
	Rect rect;

	rect.Set(0, 0, w-2, h-2);
	if(alpha)
	{
		RECT alpha_rect;
		switch(g_eRaceInterface)
		{
		case RACE_SLAYER:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h);
			DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_VAMPIRE:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;

		case RACE_OUSTERS:
			SetRect(&alpha_rect, x+2, y+2, x+rect.w-2, y+rect.h);
			DrawAlphaBox(&alpha_rect, 2, 0, 0, g_pUserOption->ALPHA_DEPTH);
			break;
		}		
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, C_GLOBAL_RESOURCE::AB_BACK_ALPHA2);
	}
	else
	{
		m_pC_assemble_box_spk->BltLockedClip(x, y, rect, AB_BACK2);
	}

	rect.Set(0, 0, m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), h);
	m_pC_assemble_box_spk->BltLockedClip(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHT2), y, rect, AB_RIGHT2);
//
//	// Ousters DescDialog ???????? ???? ??? ????
//	if( g_eRaceInterface == RACE_OUSTERS )
//	{
//		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
//		m_pC_assemble_box_spk->BltLockedClip(x-12, y+11+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);
//
//		m_pC_assemble_box_spk->BltLocked(x+13+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+11+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
//	}
//	else
//	{
//		rect.Set(0, 0, w, m_pC_assemble_box_spk->GetHeight(AB_DOWN2));
//		m_pC_assemble_box_spk->BltLockedClip(x, y+h-m_pC_assemble_box_spk->GetHeight(AB_DOWN2), rect, AB_DOWN2);
//
//		m_pC_assemble_box_spk->BltLocked(x+w-m_pC_assemble_box_spk->GetWidth(AB_RIGHTDOWN2), y+h-m_pC_assemble_box_spk->GetHeight(AB_RIGHTDOWN2), AB_RIGHTDOWN2);
//	}
}