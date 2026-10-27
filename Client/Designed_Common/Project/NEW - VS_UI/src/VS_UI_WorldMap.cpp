#include "client_PCH.h"
#include "crash_trace_tool.h"

#include "VS_UI_GameCommon.h"
#include "VS_UI.h"
#include "VS_UI_filepath.h"
#include "VS_UI_mouse_pointer.h"
#include "VS_UI_WorldMap.h"

#include "MZoneTable.h"
#include "MParty.h"
#include "MGameStringTable.h"
#include "../../Client/DXLib/D3D11SpriteCommands.h"  // Faz G5 ? ham pixel yazilari dirty bildirimi

#if __CONTENTS(__GPS_ADD)
#include "VS_UI_GPS.H"
#endif	//__GPS_ADD
C_VS_UI_WORLD_MAP::WORLDMAP_UI_INFO_MAP C_VS_UI_WORLD_MAP::s_WorldMapUIInfoMap;
int C_VS_UI_WORLD_MAP::s_NowZoneID;

#if __CONTENTS(__GPS_ADD)
extern C_VS_UI_GPS						*gpC_Gps;
#endif //__GPS_ADD

extern	DWORD		g_CurrentFrame;
//-----------------------------------------------------------------------------
// C_VS_UI_WORLD_MAP()
// : ??????
//-----------------------------------------------------------------------------
C_VS_UI_WORLD_MAP::C_VS_UI_WORLD_MAP()
{
	g_RegisterWindow(this);	

	m_pC_world_map_spk		= new C_SPRITE_PACK(SPK_WORLD_MAP);
	m_pC_world_map_icon_spk	= new C_SPRITE_PACK(SPK_WORLD_MAP_ICON);
	
	m_pC_button_group	= new ButtonGroup(this);

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(664, 507,
		m_pC_world_map_spk->GetWidth(WORLDMAP_CLOSE_BUTTON),
		m_pC_world_map_spk->GetHeight(WORLDMAP_CLOSE_BUTTON),
		WORLDMAP_CLOSE, this, WORLDMAP_CLOSE_BUTTON));

	m_DrawColor			= 21;
	m_DrawColorAdd		= 1;

	// TIMER
	m_dw_prev_tickcount = GetTickCount();
	m_dw_millisec		= 100;

	m_ptStartOffset.x	= 74;
	m_ptStartOffset.y	= 100;

#if __CONTENTS(__GPS_ADD)
	m_iSelectMapIndex				= 0;
	m_iSelectMapIndex_Click			= 0;
	m_iZoneID						= 0;
	m_iBackZoneID					= 0;
	m_iBackSelectMapIndex_Click		= 0;
	memset((S_SELECT_INFO*)m_S_SelectInfo, NULL, sizeof(m_S_SelectInfo));
#endif	//__GPS_ADD

#if __CONTENTS(__QUEST_RENEWAL)
	m_QuestIcon_Info.clear();
	m_QuestIcon_Info.resize(0);
#endif//__QUEST_RENEWAL

}

//-----------------------------------------------------------------------------
// ~C_VS_UI_WORLD_MAP()
// : ?©¥???
//-----------------------------------------------------------------------------
C_VS_UI_WORLD_MAP::~C_VS_UI_WORLD_MAP()
{
	g_UnregisterWindow(this);
	
	DeleteNew(m_pC_button_group);

	DeleteNew(m_pC_world_map_icon_spk);
	DeleteNew(m_pC_world_map_spk);
#if __CONTENTS(__GPS_ADD)
	m_iSelectMapIndex				= 0;
	m_iSelectMapIndex_Click			= 0;
	m_iZoneID						= 0;
	m_iBackZoneID					= 0;
	m_iBackSelectMapIndex_Click		= 0;
	memset((S_SELECT_INFO*)m_S_SelectInfo, NULL, sizeof(m_S_SelectInfo));

#endif //__GPS_ADD

#if __CONTENTS(__QUEST_RENEWAL)
	m_QuestIcon_Info.clear();
	m_QuestIcon_Info.resize(0);
#endif//__QUEST_RENEWAL

}

//-----------------------------------------------------------------------------
// Start()
// : ????? ????? ???
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::Start()
{
	PI_Processor::Start();
	//m_pC_button_group->Init();
	gpC_window_manager->AppearWindow(this);
	
	AttrTopmost(true);
	AttrPin(true);


	SIZE worldMapSize = {
		m_pC_world_map_spk->GetWidth(),
		m_pC_world_map_spk->GetHeight()
	};

	Set((RESOLUTION_X - worldMapSize.cx) / 2,
		(RESOLUTION_Y - worldMapSize.cy) / 2,
		worldMapSize.cx, worldMapSize.cy);
}

//-----------------------------------------------------------------------------
// Finish()
// : ???? ????? ???
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::Finish()
{
	PI_Processor::Finish();
	
	gpC_window_manager->DisappearWindow(this);	
}

//-----------------------------------------------------------------------------
// ShowButtonWidget()
// : ????? ????? ??????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::ShowButtonWidget(C_VS_UI_EVENT_BUTTON* p_button)
{
	if(p_button->GetPressState())
		m_pC_world_map_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index + 2);
	else if(p_button->GetFocusState())
		m_pC_world_map_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index + 1);

#if __CONTENTS(__GPS_ADD)
	if(p_button->GetPressState())
		m_pC_world_map_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index + 2);
	else if(p_button->GetFocusState())
		m_pC_world_map_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index + 1);
#endif
}

//-----------------------------------------------------------------------------
// ShowButtonDescription()
// : ????? ?????? ??????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::ShowButtonDescription(C_VS_UI_EVENT_BUTTON* p_button)
{
}

//-----------------------------------------------------------------------------
// IsPixel()
// : ???????? ????? ?????.
//-----------------------------------------------------------------------------
bool C_VS_UI_WORLD_MAP::IsPixel(int _x, int _y)
{
	if(Moving()) return true;
	//return IsInRect(_x, _y);
	return m_pC_world_map_spk->IsPixel(_x - x, _y - y, WORLDMAP_HELEA);
}

//-----------------------------------------------------------------------------
// Run()
// : ????? ????????? ???? ??????? ??????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::Run(id_t id)
{
	switch (id)
	{	
	case WORLDMAP_CLOSE:
		gC_vs_ui.CloseWorldMap();
#if __CONTENTS(__GPS_ADD)
		if(gpC_Gps != NULL)
		{
			if(!gpC_Gps->GetGpsShow())
				gpC_Gps->NonActive();
		}
#endif //__GPS_ADD
		break;
	}
}

//-----------------------------------------------------------------------------
// Process()
// : ?? ????????? ?????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::Process()
{
	m_pC_button_group->Process();
	
	if(ColorTimer())
	{
		m_DrawColor += m_DrawColorAdd;
		if(m_DrawColor > 30 || m_DrawColor < 21)
			m_DrawColorAdd *= -1;
	}
}


//-----------------------------------------------------------------------------
// Timer
//
//-----------------------------------------------------------------------------
bool C_VS_UI_WORLD_MAP::ColorTimer()
{
	if(m_dw_prev_tickcount + m_dw_millisec <= GetTickCount())
	{
		m_dw_prev_tickcount = GetTickCount();
		return true;
	}
	
	return false;
}


//-----------------------------------------------------------------------------
// Show()
// : ?????®œ ??????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::Show()
{
	CT_Done("WnSh: C_VS_UI_WORLD_MAP::Show ENTER pthis=%p", (void*)this);
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		//gpC_global_resource->m_pC_assemble_box_spk->BltLocked(x, y, 0);

		m_pC_world_map_spk->BltLocked(x, y, WORLDMAP_HELEA);
#if __CONTENTS(__GPS_ADD)
												//ZoneInfo????????? ???? ??????? ???? ?????(73????)
			m_pC_world_map_spk->BltLockedAlpha(m_S_SelectInfo[m_iSelectMapIndex].rt.left + x + 73,
												m_S_SelectInfo[m_iSelectMapIndex].rt.top + y + 98, 
												m_iSelectMapIndex + MAP_INDEX, 30);	// 73?? ???????? ???? ?????? X??? 98?? Y???, 35?? ???? ?????.

#endif //__GPS_ADD
		
		WORLDMAP_UI_INFO_MAP::iterator	pos		= s_WorldMapUIInfoMap.begin();
		WORLDMAP_UI_INFO_MAP::iterator	endpos	= s_WorldMapUIInfoMap.end();

		for(; pos != endpos; ++pos)
		{
			int nZoneID = pos->first;
			WORLDMAP_UI_INFO*	pWorldMapUIInfo	= &pos->second;
			ZONETABLE_INFO*		pZoneTableInfo	= g_pZoneTable->Get(nZoneID);
			assert(pZoneTableInfo->IsShownInWorldMap());
			
			const CRect& rcWorldMap = pZoneTableInfo->WorldMapInfo.WorldMapPosition;
			const CSize& sizeMap	= pWorldMapUIInfo->mapSize;

			WORLDMAP_UI_INFO::MINIMAP_PORTAL_VEC::iterator portalPos	= pWorldMapUIInfo->portalVec.begin();
			WORLDMAP_UI_INFO::MINIMAP_PORTAL_VEC::iterator portalEndpos	= pWorldMapUIInfo->portalVec.end();

			for(; portalPos < portalEndpos; ++portalPos)
			{
				const CRect& rcPortalPos = portalPos->position;

#if __CONTENTS(__RACE_OUSTERS)
				DrawPortal(gpC_base->m_p_DDSurface_back, nZoneID, sizeMap,
					rcWorldMap, rcPortalPos.CenterPoint());
//#else
//				if(nZoneID == WORLDMAP_ID_CASTALO_NE)
//					break;
//				DrawPortal(gpC_base->m_p_DDSurface_back, nZoneID, sizeMap,
//					rcWorldMap, rcPortalPos.CenterPoint());				 //???????
#endif // __RACE_OUSTERS
			}
			
			// ????? ??? ???
			for(int i = 0; i < g_pParty->GetSize(); ++i)
			{
				const PARTY_INFO* pPartyInfo = g_pParty->GetMemberInfo(i);
		
				CPoint ptPos(pPartyInfo->zoneX, pPartyInfo->zoneY);

				if(pPartyInfo != NULL && pPartyInfo->zoneID == nZoneID)
					DrawCharacter(gpC_base->m_p_DDSurface_back, nZoneID, sizeMap, rcWorldMap, ptPos, WORLDMAPICON_OTHERS);
			}
			
			// ??? ??? ???
			if(nZoneID == s_NowZoneID)
			{
				DrawCharacter(gpC_base->m_p_DDSurface_back, nZoneID, sizeMap,
						rcWorldMap, m_ptPlayerPosition, WORLDMAPICON_SLAYER + g_eRaceInterface);
			}
		}

//		m_pC_button_group->Show();
#if __CONTENTS(__QUEST_RENEWAL)	// C_VS_UI_WORLD_MAP		QuestIconShow()????;
		QuestIconShow();
#endif	// C_VS_UI_WORLD_MAP
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}


void C_VS_UI_WORLD_MAP::DrawCharacter(CSpriteSurface *pDescSurface,
									  TYPE_ZONEID zoneID, const CSize& sizeMap, const CRect& rcWorldMap,
									  const CPoint& ptCharacterPos, int nIconIndex)
{
	CSize sizeWorldMap = rcWorldMap.Size();

	int _x = x + m_ptStartOffset.x + rcWorldMap.left + ptCharacterPos.x * sizeWorldMap.cx / sizeMap.cx;
	int _y = y + m_ptStartOffset.y + rcWorldMap.top + ptCharacterPos.y * sizeWorldMap.cy / sizeMap.cy;
	
	CSize sizeIcon(m_pC_world_map_icon_spk->GetWidth(nIconIndex),
				   m_pC_world_map_icon_spk->GetHeight(nIconIndex));

	if(_x > 0 && _x < RESOLUTION_X-1 && _y > 0 && _y < RESOLUTION_Y-1)
		m_pC_world_map_icon_spk->BltLocked(_x - sizeIcon.cx / 2, _y - sizeIcon.cy / 2, nIconIndex);
}


void C_VS_UI_WORLD_MAP::DrawPortal(CSpriteSurface *pDescSurface,
								   TYPE_ZONEID zoneID, const CSize& sizeMap,
								   const CRect& rcWorldMap, const CPoint& ptPotalPos)
{
	BYTE r = 31, g = 63, b = 31;
	WORD portal_color = CDirectDraw::Color(r, g, b);
	
	WORD _color, _color2;
	
	// ??? ??? ??? ??
	r = m_DrawColor;
	g = m_DrawColor - 10;
	b = m_DrawColor;
	_color = CDirectDraw::Color(r, g, b);

	r = m_DrawColor * 2 / 3;
	g = (m_DrawColor - 10) * 2 / 3;
	b = m_DrawColor * 2 / 3;
	_color2 = CDirectDraw::Color(r, g, b);

	CSize	sizeWorldMap = rcWorldMap.Size();

	int _x = x + m_ptStartOffset.x + rcWorldMap.left + ptPotalPos.x * sizeWorldMap.cx / sizeMap.cx;
	int _y = y + m_ptStartOffset.y + rcWorldMap.top + ptPotalPos.y * sizeWorldMap.cy / sizeMap.cy;
	
	WORD*	pDescBuffer = (WORD *)pDescSurface->GetSurfacePointer();
	long	pitch		= pDescSurface->GetSurfacePitch();
	long	pitch_div_2 = pitch >> 1;

	if(_x > 1 && _x < RESOLUTION_X-2 && _y > 2 && _y < RESOLUTION_Y-1)
	{
		pDescBuffer[(_y)*pitch_div_2	+ _x]	= _color;
		pDescBuffer[(_y-1)*pitch_div_2	+ _x-1] = _color2;
		pDescBuffer[(_y-1)*pitch_div_2	+ _x]	= _color;
		pDescBuffer[(_y-1)*pitch_div_2	+ _x+1] = _color2;
		pDescBuffer[(_y-2)*pitch_div_2	+ _x-1] = _color;
		pDescBuffer[(_y-2)*pitch_div_2	+ _x]	= _color;
		pDescBuffer[(_y-2)*pitch_div_2	+ _x+1] = _color;
		
		pDescBuffer[(_y-3)*pitch_div_2	+ _x-1] = 0;
		pDescBuffer[(_y-3)*pitch_div_2	+ _x]	= 0;
		pDescBuffer[(_y-3)*pitch_div_2	+ _x+1] = 0;
		pDescBuffer[(_y-2)*pitch_div_2	+ _x-2] = 0;
		pDescBuffer[(_y-2)*pitch_div_2	+ _x+2] = 0;
		pDescBuffer[(_y-1)*pitch_div_2	+ _x-2] = 0;
		pDescBuffer[(_y-1)*pitch_div_2	+ _x+2] = 0;
		pDescBuffer[(_y)*pitch_div_2	+ _x-1] = 0;
		pDescBuffer[(_y)*pitch_div_2	+ _x+1] = 0;
		pDescBuffer[(_y+1)*pitch_div_2	+ _x]	= 0;

		// Faz G5 (2026-09-01) ? dx11gpu: ham portal isareti dirty bildirimi
		{
			phoenix::render::D3D11SpriteCommands& cmds =
				phoenix::render::D3D11SpriteCommands::Instance();
			if(cmds.IsActive() && cmds.IsCompositionTarget(pDescBuffer))
			{
				RECT dirty;
				dirty.left   = _x - 2;
				dirty.top    = _y - 3;
				dirty.right  = _x + 3;
				dirty.bottom = _y + 2;
				cmds.MarkDirty(&dirty);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// SetPlayerXY()
// : ???? ?¡À???? ????? ???????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::SetPlayerXY(int px, int py)
{
	m_ptPlayerPosition.x = px;
	m_ptPlayerPosition.y = py;
}

//-----------------------------------------------------------------------------
// SetNowZoneID()
// : ???? ?? ????? ???????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::SetNowZoneID(int zoneID)
{
	s_NowZoneID = zoneID;
}

//-----------------------------------------------------------------------------
// MouseControl()
// : ???²J ??????? ???????.
//-----------------------------------------------------------------------------
bool C_VS_UI_WORLD_MAP::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);

	CPoint ptWindowPos(_x - x, _y - y);

	m_pC_button_group->MouseControl(message, ptWindowPos.x, ptWindowPos.y);

	if(message == M_MOVING)
	{
#if __CONTENTS(__QUEST_RENEWAL)	// C_VS_UI_WORLD_MAP		QuestIconShow()????;
		QuestDescription(_x, _y);
#endif	// __QUEST_RENEWAL
		ShowMouseDescription(ptWindowPos.x, ptWindowPos.y);
	}

#if __CONTENTS(__GPS_ADD)
	else if(message == M_LEFTBUTTON_DOWN)
	{
		m_iSelectMapIndex_Click	= m_iSelectMapIndex;	// SPK_INDEX ????? ???? ?????. WORLDMAP_CLOSE_BUTTON_PUSHED = 3??? 	WORLDMAP_SELECT_LIMBO_NW = 4
//		WORLDMAP_SELECT_LIMBO_NW?? ???? ??? ???? 0?? ????? - WORLDMAP_SELECT_LIMBO_NW???? ????? ???.

		S_GPS_MONSTER_INFO	MonsteInfo;

		if(m_iZoneID < 1500)
		{
			gpC_Gps->Start();
			m_iBackZoneID				= m_iZoneID;
			m_iBackSelectMapIndex_Click	= m_iSelectMapIndex_Click;
			gpC_Gps->Active(m_iSelectMapIndex_Click, m_iZoneID);
			MonsteInfo	= gpC_Gps->m_C_GpsManager.GetMonsterInfo();
			gpC_Gps->m_C_GpsManager.SetMonsterLv(MonsteInfo.wMonsterLv1, MonsteInfo.wMonsterLv2);
		}
		else
		{
			if(m_iBackSelectMapIndex_Click != 0 || m_iBackZoneID != 0)			//???? ????? ???? ???? ??? ?????? ????
			{
				if(gpC_Gps->GetGpsShow())
				{
					gpC_Gps->Active(m_iBackSelectMapIndex_Click, m_iBackZoneID);
					MonsteInfo	= gpC_Gps->m_C_GpsManager.GetMonsterInfo();
					gpC_Gps->m_C_GpsManager.SetMonsterLv(MonsteInfo.wMonsterLv1, MonsteInfo.wMonsterLv2);
				}
			}
		}
	}
#endif	//__GPS_ADD

	return true; 
}


//-----------------------------------------------------------------------------
// KeyboardControl()
// : ????? ??????? ???????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::KeyboardControl(UINT message, UINT key, long extra)	
{	
	if(message == WM_KEYDOWN && key == VK_ESCAPE)
		Run(WORLDMAP_CLOSE);
}


//-----------------------------------------------------------------------------
// ShowMouseDescription()
// : ???²J ?????? ???????.
//-----------------------------------------------------------------------------
void C_VS_UI_WORLD_MAP::ShowMouseDescription(int _x, int _y)
{
	const CPoint ptPointer(gpC_mouse_pointer->GetPointerX(), gpC_mouse_pointer->GetPointerY());
	const CPoint ptMouse(_x, _y);

	WORLDMAP_UI_INFO_MAP::iterator	pos		= s_WorldMapUIInfoMap.begin();
	WORLDMAP_UI_INFO_MAP::iterator	endpos	= s_WorldMapUIInfoMap.end();

	for(; pos != endpos; ++pos)
	{
		int nZoneID = pos->first;
		
		ZONETABLE_INFO*		pZoneTableInfo	= g_pZoneTable->Get(nZoneID);
		assert(pZoneTableInfo->IsShownInWorldMap());
		
		const CRect&	rcWorldMap		= pZoneTableInfo->WorldMapInfo.WorldMapPosition;

#if __CONTENTS(__GPS_ADD)	
		if(gpC_Gps != NULL)
		{
			gpC_Gps->m_C_GpsManager.SetSelectMapIndex(nZoneID);
			m_iSelectMapIndex	= gpC_Gps->m_C_GpsManager.GetSelectMapIndex();
			m_S_SelectInfo[m_iSelectMapIndex].rt			= rcWorldMap;
			m_S_SelectInfo[m_iSelectMapIndex].iZoneID		= nZoneID;
			m_iZoneID										= nZoneID;	
		}
#endif //__GPS_ADD
		
		CRect rcUIWorldMap = rcWorldMap;
		rcUIWorldMap.OffsetRect(m_ptStartOffset);

		/*
		const int edge = 5;
		rcUIWorldMap.left	-= edge;
		rcUIWorldMap.top	-= edge;
		rcUIWorldMap.right	+= edge;
		rcUIWorldMap.bottom	+= edge;
		*/
		
		//////////////////////////////////////////////////////////////////////////
		// ??? ?? ??? ???²J?? ??????????? ???
		if(rcUIWorldMap.PtInRect(ptMouse))
		{
			WORLDMAP_UI_INFO*	pWorldMapUIInfo	= &pos->second;

			const CSize		sizeWorldMap	= rcWorldMap.Size();
			const CSize&	sizeMap			= pWorldMapUIInfo->mapSize;
			
			WORLDMAP_UI_INFO::MINIMAP_PORTAL_VEC::iterator portalPos	= pWorldMapUIInfo->portalVec.begin();
			WORLDMAP_UI_INFO::MINIMAP_PORTAL_VEC::iterator portalEndpos	= pWorldMapUIInfo->portalVec.end();
			
			int portalCnt = 0;
			
			const float fSizeRateX = sizeWorldMap.cx / (float)sizeMap.cx;
			const float fSizeRateY = sizeWorldMap.cy / (float)sizeMap.cy;
			
			//////////////////////////////////////////////////////////////////////////
			// ????? ???? ???
			const int partySize = g_pParty->GetSize();
			
#if __CONTENTS(__GPS_ADD)
			if(gpC_Gps != NULL)
			{
				gpC_Gps->m_C_GpsManager.SetSelectMapIndex(nZoneID);
				m_iSelectMapIndex	= gpC_Gps->m_C_GpsManager.GetSelectMapIndex();
			}

#endif //__GPS_ADD

			for(int i = 0; i < partySize; i++)
			{
				PARTY_INFO* partyInfo = g_pParty->GetMemberInfo(i);
				if(partyInfo != NULL && partyInfo->zoneID == nZoneID)
				{
					CPoint	pt(m_ptStartOffset.x + rcWorldMap.left + static_cast<int>(partyInfo->zoneX * fSizeRateX),
						m_ptStartOffset.y + rcWorldMap.top + static_cast<int>(partyInfo->zoneY * fSizeRateY));
					
					const int edge = 4;
					CRect	rcMouseOver(pt.x - edge, pt.y - edge, pt.x + edge, pt.y + edge);
					
					if(rcMouseOver.PtInRect(ptMouse))
					{
						if(partyInfo->Name.GetString())
							strcpy(m_PartyNameBuf, partyInfo->Name.GetString());
						else 
							strcpy(m_PartyNameBuf, "");
						
						wsprintf(m_PartyDescBuf, "(%d,%d)", partyInfo->zoneX, partyInfo->zoneY);

						ZONETABLE_INFO*	pPartyZoneTableInfo = g_pZoneTable->Get(nZoneID);
						
						m_HelpString.sz_main_str	= m_PartyNameBuf;
						m_HelpString.sz_main_str2	= pPartyZoneTableInfo->Name.GetString();
						m_HelpString.sz_sub_str		= m_PartyDescBuf;
						
						g_descriptor_manager.Set(DID_HELP, ptPointer.x, ptPointer.y, &m_HelpString);

						return;
					}
				}
			}
			
			//////////////////////////////////////////////////////////////////////////
			// ??? ???? ???
			for(; portalPos < portalEndpos; ++portalPos, ++portalCnt)
			{
				const CRect& rcPortalPos = portalPos->position;
				
				CPoint	ptPotalPos		= rcPortalPos.CenterPoint();
				
				CPoint	pt(m_ptStartOffset.x + rcWorldMap.left + static_cast<int>(ptPotalPos.x * fSizeRateX),
						   m_ptStartOffset.y + rcWorldMap.top + static_cast<int>(ptPotalPos.y * fSizeRateY));
				
				const int edge = 4;
				CRect	rcMouseOver(pt.x - edge, pt.y - edge, pt.x + edge, pt.y + edge);
				
				if(rcMouseOver.PtInRect(ptMouse))
				{
					TYPE_ZONEID		targetZoneID			= portalPos->zoneID;
					ZONETABLE_INFO*	pTargetZoneTableInfo	= g_pZoneTable->Get(targetZoneID);
					
					if(targetZoneID != 1500)		// ??? ???? -_-;; ?? ??? ????
					{
						m_HelpString.sz_main_str = pTargetZoneTableInfo->Name.GetString();
					}
					else
					{
						int messageIdx = 0;
						switch(portalCnt)
						{
						case 0:	messageIdx = STRING_MESSAGE_SIEGE_POTAL_201;	break;
						case 1:	messageIdx = STRING_MESSAGE_SIEGE_POTAL_200;	break;
						case 2:	messageIdx = STRING_MESSAGE_SIEGE_POTAL_202;	break;
						case 3:	messageIdx = STRING_MESSAGE_SIEGE_POTAL_203;	break;
						}
						
						if(messageIdx != 0)
						{
							m_HelpString.sz_main_str = (*g_pGameStringTable)[STRING_MESSAGE_SIEGE_POTAL_201].GetString();
						}
					}

					WORLDMAP_INFO::PAYMENT_TYPE paymentType = pTargetZoneTableInfo->WorldMapInfo.PaymentType;
					
					//m_HelpString.sz_main_str2	= (*g_pGameStringTable)[UI_STRING_MESSAGE_PAYMENT_FREE + paymentType].GetString();
					m_HelpString.sz_main_str2	= NULL;
					m_HelpString.sz_sub_str		= NULL; //"??????";
					
					long color;
					if(paymentType == WORLDMAP_INFO::PAYMENT_TYPE::PAYMENT_PREMIUM)
						color = RGB(255, 128, 0);	// ???
					else
						color = RGB(0, 255, 255);	// ????
					
					g_descriptor_manager.Set(DID_HELP, ptPointer.x, ptPointer.y, &m_HelpString, color);					
					return;
				}
			}
			
			//////////////////////////////////////////////////////////////////////////
			// ????? ??? 
			m_HelpString.sz_main_str = pZoneTableInfo->Name.GetString();
			
			WORLDMAP_INFO::PAYMENT_TYPE paymentType = pZoneTableInfo->WorldMapInfo.PaymentType;
			
			//m_HelpString.sz_main_str2	= (*g_pGameStringTable)[UI_STRING_MESSAGE_PAYMENT_FREE + paymentType].GetString();
			m_HelpString.sz_main_str2	= NULL;
			m_HelpString.sz_sub_str		= NULL; //"??????";
			
#if __CONTENTS(__QUEST_RENEWAL)	
			// 2009.11.10 ???????? ????? ???? ???? ??????? ??? ????
			m_strPayZone	= "";//m_HelpString.sz_main_str2;
#endif // __QUEST_RENEWAL

			long color;
			if(paymentType == WORLDMAP_INFO::PAYMENT_TYPE::PAYMENT_PREMIUM)
				color = RGB(255, 128, 0);	// ???
			else
				color = RGB(0, 255, 255);	// ????
			
			g_descriptor_manager.Set(DID_HELP, ptPointer.x, ptPointer.y, &m_HelpString, color);
			return;
		}
	}
}


#if __CONTENTS(__QUEST_RENEWAL)	// C_VS_UI_WORLD_MAP		QuestIconShow() ????;
void	C_VS_UI_WORLD_MAP::QuestIconShow()
{
	int		iSecFrameTemp	= (g_CurrentFrame%16) + 1;
	WORD	wIndex	= 0;
	int		iPos_X	= 0;
	int		iPos_Y	= 0;

	if(gpC_Gps == NULL)
		return;

	for(int i = 0; i < m_QuestIcon_Info.size(); i++)
	{
		gpC_Gps->m_C_GpsManager.SetSelectMapIndex(m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wID);
		wIndex	= gpC_Gps->m_C_GpsManager.GetSelectMapIndex();

		if(m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Width == 0 || m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Height == 0)
			return;

		iPos_X	= (m_QuestIcon_Info[i].wPos_X * m_S_SelectInfo[wIndex].rt.Width()) / m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Width;
		iPos_Y	= (m_QuestIcon_Info[i].wPos_Y * m_S_SelectInfo[wIndex].rt.Height()) / m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Height;

		if(m_QuestIcon_Info[i].bQuestShow)
		{
			m_pC_world_map_icon_spk->BltLockedAlpha(m_S_SelectInfo[wIndex].rt.left + x + 73 + iPos_X,
													m_S_SelectInfo[wIndex].rt.top + y + 98	+ iPos_Y, 
													m_QuestIcon_Info[i].wIndex + WORLDMAPICON_QUEST_WAIT, iSecFrameTemp * 2);
		}
	}

}



void	C_VS_UI_WORLD_MAP::GameMapToSelectMap(int &iPos_X, int &iPos_Y, S_QUESTICON_INFO Quest_Info)
{

}


void	C_VS_UI_WORLD_MAP::QuestDescription(int iMouse_X, int iMouse_Y)
{
	m_Quest_String.sz_main_str;
	m_Quest_String.sz_main_str2;
	m_Quest_String.sz_sub_str;

	int		iPos_X	= 0;
	int		iPos_Y	= 0;

	int		iPos_RangeMin_X	= 0;
	int		iPos_RangeMax_X	= 0;
	int		iPos_RangeMin_Y	= 0;
	int		iPos_RangeMAX_Y	= 0;

	WORD	wIndex	= 0;

	if(gpC_Gps == NULL)
		return;

	for(int i = 0; i < m_QuestIcon_Info.size(); i++)
	{
		char	strDebugString[1000]	= {'\0',};
		char	strNum[10]			= {'\0',};

		if(m_QuestIcon_Info[i].bQuestShow == false)
			continue;

		gpC_Gps->m_C_GpsManager.SetSelectMapIndex(m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wID);
		wIndex	= gpC_Gps->m_C_GpsManager.GetSelectMapIndex();

		if(m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Width == 0 || m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Height == 0)
			return;
		
		iPos_X	= (m_QuestIcon_Info[i].wPos_X * m_S_SelectInfo[wIndex].rt.Width()) / m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Width;
		iPos_Y	= (m_QuestIcon_Info[i].wPos_Y * m_S_SelectInfo[wIndex].rt.Height()) / m_QuestIcon_Info[i].S_Gps_Game_Map_Info.wMapSize_Height;

		iPos_RangeMin_X	= m_S_SelectInfo[wIndex].rt.left	+	x	+	73	+	iPos_X;
		iPos_RangeMax_X	= m_S_SelectInfo[wIndex].rt.left	+	x	+	73	+	iPos_X		
															+	m_pC_world_map_icon_spk->GetWidth(m_QuestIcon_Info[i].wIndex + WORLDMAPICON_QUEST_WAIT);

		iPos_RangeMin_Y	= m_S_SelectInfo[wIndex].rt.top		+	y	+	98	+	iPos_Y;
		iPos_RangeMAX_Y	= m_S_SelectInfo[wIndex].rt.top		+	y	+	98	+	iPos_Y	
															+	m_pC_world_map_icon_spk->GetHeight(m_QuestIcon_Info[i].wIndex + WORLDMAPICON_QUEST_WAIT);

		if( iPos_RangeMin_X	<= iMouse_X && iPos_RangeMax_X >= iMouse_X
			&& iPos_RangeMin_Y <= iMouse_Y && iPos_RangeMAX_Y >= iMouse_Y)
		{
			m_Quest_String.sz_main_str	= const_cast<char *>(m_QuestIcon_Info[i].strQuestName_WorldView.c_str());
			m_Quest_String.sz_main_str2	= NULL;
			// 2009.11.10 ???????? ????? ???? ???? ??????? ??? ????
			m_Quest_String.sz_sub_str	= NULL;//const_cast<char *>(m_strPayZone[0].c_str());

			g_descriptor_manager.Set(DID_HELP, iMouse_X, iMouse_Y, &m_Quest_String, RGB(255, 128, 0), RGB(0, 255, 255));
		}
	}
}

void	C_VS_UI_WORLD_MAP::GpsToBringQuestInfo()
{
	if(gpC_Gps != NULL)
		m_QuestIcon_Info	= gpC_Gps->m_C_GpsManager.GetQuestInfo();
}

#endif // __QUEST_RENEWAL
