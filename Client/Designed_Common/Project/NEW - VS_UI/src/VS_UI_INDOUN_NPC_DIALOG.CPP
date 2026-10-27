#include "Client_PCH.h"

#include "VS_UI_ExtraDialog.h"
#include "VS_UI_Filepath.h"
#include "VS_UI_Mouse_pointer.h"

#include "MMoneyManager.h"
#include "MItemOptionTable.h"
#include "VS_UI.h"
#include "MPriceManager.h"
#include "MCreatureTable.h"
#include "UtilityFunction.h"
#include "MGameStringTable.H"
#include "ClientConfig.H"
#include "MNpcTable.h"
#include "UserInformation.h"

#include <algorithm>

#include "VS_UI_INDOUN_NPC_DIALOG.h"

#if __CONTENTS(__BLITZ_COUPON_EVENT)
#include "UserOption.h"
#endif //__BLITZ_COUPON_EVENT

C_VS_UI_INDOUN_DIALOG::C_VS_UI_INDOUN_DIALOG()
{
}

C_VS_UI_INDOUN_DIALOG::	C_VS_UI_INDOUN_DIALOG(S_INDOUN_DLG_INFO	Indoun_Dlg_Info, ENUMTYPE_INDUONSELECT etIndounSelect)
{
	g_RegisterWindow(this);	
	m_Indoun_Dlg_Info	= Indoun_Dlg_Info;
	m_etIndounSelect	= etIndounSelect;

	m_pC_button_group	= new ButtonGroup(this);
	
#if __CONTENTS(__DYNAMIC_UI)
	m_tx = -100.0f;
	m_Picture_Alpha = 10;
#endif //__DYNAMIC_UI
	
	if(m_etIndounSelect	== COMPENSATION_DLG)
	{
		InitFinishDlg();
	}
	else
	{
		InitStarDlg();
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_INDOUN_DIALOG::~C_VS_UI_NPC_DIALOGC_VS_UI_INDOUN_NPC_DIALOG
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_INDOUN_DIALOG::~C_VS_UI_INDOUN_DIALOG()
{
	g_UnregisterWindow(this);
	DeleteNew(m_pC_button_group);	
}


void	C_VS_UI_INDOUN_DIALOG::SetIndounNpcDlgInfo(S_INDOUN_DLG_INFO Indoun_Npc_Dlg_Info, ENUMTYPE_INDUONSELECT etIndounSelect)
{
	DeleteNew(m_pC_button_group);	

	m_Indoun_Dlg_Info	= Indoun_Npc_Dlg_Info;
	m_etIndounSelect	= etIndounSelect;

	m_pC_button_group	= new ButtonGroup(this);
	
	if(m_etIndounSelect	== COMPENSATION_DLG)
	{
		InitFinishDlg();
	}
	else
	{
		InitStarDlg();
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_INDOUN_DIALOG::Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_INDOUN_DIALOG::Start()
{
#if __CONTENTS(__DYNAMIC_UI)
	gC_vs_ui.UnProgressFlg();
#endif //__DYNAMIC_UI

	PI_Processor::Start();
	//m_pC_button_group->Init();

	gpC_window_manager->AppearWindow(this);

	AttrTopmost(true);
	AttrPin(true);
}

//-----------------------------------------------------------------------------
// C_VS_UI_INDOUN_DIALOG::MouseControl
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_INDOUN_DIALOG::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	_x-=x; _y-=y;
	bool re = m_pC_button_group->MouseControl(message, _x, _y);

	return true;
}

//-----------------------------------------------------------------------------
// C_VS_UI_INDOUN_DIALOG::KeyboardControl
//
// �ٷ� ���� �Է��� �� �ְ� �Ѵ�.
//-----------------------------------------------------------------------------
void	C_VS_UI_INDOUN_DIALOG::KeyboardControl(UINT message, UINT key, long extra)
{
	if (message == WM_KEYDOWN)
	{
		switch (key)
		{
			case VK_ESCAPE:
				{
					Run(INDOUN_NPC_DIALOG_CLOSE);
				}
				break;
		}
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_INDOUN_DIALOG::KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_INDOUN_DIALOG::Show()
{
#if __CONTENTS(__DYNAMIC_UI)
	if(x>=(g_pUserOption->Resolution1024?1024:800)||gC_vs_ui.DidProgressFlg())
	{
		gC_vs_ui.UnProgressFlg();
		Finish();
		return;
	}
	else
	{
		int min_pix = x-(int)m_tx;
		if(min_pix<0) { min_pix*=-1; }
		
		if(min_pix<=10)
		{
			x = (int)m_tx;
		}
		else
		{
			x = x + 0.1f * (m_tx - x);
		}
	}

	gpC_global_resource->DrawDialog(x, y, m_wDialog_Width, m_wDialog_Heigth, 1);
#else
	gpC_global_resource->DrawDialog(x, y, m_wDialog_Width, m_wDialog_Heigth, GetAttributes()->alpha);
#endif //__DYNAMIC_UI
	if(gpC_base->m_p_DDSurface_back->Lock())
	{

		if(m_etIndounSelect	== COMPENSATION_DLG)
		{
		}
		else
		{
			Picture_Show();
		}
		
//	gpC_base->m_p_DDSurface_back->FillRect(&box_rect, 0);			//���������� ĥ�Ѵ�.

//	gpC_global_resource->DrawOutBoxLocked(x+18, y+122, 298, 180);	//�ڽ� ������ �׸���.
	
		Rect rect;

		RECT mssageBox;
		
		mssageBox.left = x;
		mssageBox.top = y;
		mssageBox.right = x + m_wDialog_Width;
		mssageBox.bottom = y + m_wDialog_Heigth;

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	m_pC_button_group->Show();


	if(g_FL2_GetDC())
	{
		if(m_etIndounSelect	== COMPENSATION_DLG)
		{
		}
		else
		{
//			TitleName_Show();

#if __CONTENTS(!__DYNAMIC_UI)
			ButtonName_Show();
#endif //__DYNAMIC_UI

			NPCName_Show();
			Desc_Show();
		}

		g_FL2_ReleaseDC();
	}
}


void C_VS_UI_INDOUN_DIALOG::Finish()
{
	PI_Processor::Finish();
	
	gpC_window_manager->DisappearWindow(this);	
}


//-----------------------------------------------------------------------------
// ShowButtonWidget()
// : ��ư�� ����� ����Ѵ�.
//-----------------------------------------------------------------------------
#if __CONTENTS(__DYNAMIC_UI)
void C_VS_UI_INDOUN_DIALOG::ShowButtonWidget(C_VS_UI_EVENT_BUTTON* p_button)
{
	COLORREF	colRef;
	int PressPos = 0;

	if(p_button->GetPressState())
	{
		PressPos = 2;
		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x+p_button->x, y+p_button->y+PressPos, C_GLOBAL_RESOURCE::AB_DIALOG_TAG_SELECTED); //��ư Ŭ��
		colRef	= RGB(180, 180, 255);
	}
	else if(p_button->GetFocusState())
	{
		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x+p_button->x, y+p_button->y, C_GLOBAL_RESOURCE::AB_DIALOG_TAG_SELECTED); //���콺 �ѿ���
		colRef	= RGB(180, 180, 255);
	}
	else
	{
		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x+p_button->x, y+p_button->y, C_GLOBAL_RESOURCE::AB_DIALOG_TAG); //�Ϲ�
		colRef	= RGB(255, 255, 255);
	}

	if(m_etIndounSelect	== ENTRANCE_DLG)
	{
		std::vector<string>::iterator it = m_Indoun_Dlg_Info.mapButtonNameList.begin();

		for(int wCount	= 0;	it != m_Indoun_Dlg_Info.mapButtonNameList.end();	it++)
		{
			if(wCount == p_button->GetID())
			{
				g_PrintColorStr(x+p_button->x+25, y+p_button->y+4+PressPos, it->c_str(), gpC_base->m_info_pi, colRef);
				break;
			}
			wCount++;
		}
	}
}
#else
void C_VS_UI_INDOUN_DIALOG::ShowButtonWidget(C_VS_UI_EVENT_BUTTON* p_button)
{
	int id = 0;

	if(p_button->GetPressState())
	{
		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x + p_button->x, y + p_button->y, C_GLOBAL_RESOURCE::AB_DIALOG_TAG_SELECTED); //��ư Ŭ��
		id	= p_button->GetID();
	}
	else if(p_button->GetFocusState())
	{
		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x + p_button->x, y + p_button->y, C_GLOBAL_RESOURCE::AB_DIALOG_TAG_SELECTED); //���콺 �ѿ���
		m_SelectButtonIdex	= p_button->GetID();
	}
	else
	{
		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x + p_button->x, y + p_button->y, C_GLOBAL_RESOURCE::AB_DIALOG_TAG); //�Ϲ�
	}
}
#endif //__DYNAMIC_UI

//-----------------------------------------------------------------------------
// ShowButtonDescription()
// : ��ư�� ������ ����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_INDOUN_DIALOG::ShowButtonDescription(C_VS_UI_EVENT_BUTTON* p_button)
{
}



//-----------------------------------------------------------------------------
// Run()
// : ��ư�� Ŭ�������� ���� �޽����� ó���Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_INDOUN_DIALOG::Run(id_t id)
{
#if __CONTENTS(__TIPOJYU_CASTLE)
	switch (id)
	{	
	case INDOUN_NPC_DIALOG_CLOSE:
#if __CONTENTS(__DYNAMIC_UI)
		m_tx = (float)(g_pUserOption->Resolution1024?1024:800)+30;
#else
		gC_vs_ui.CloseIndounNpcDlg();
#endif //__DYNAMIC_UI
		break;

	default:
		if(m_etIndounSelect == ENTRANCE_DLG)
		{
			gpC_base->SendMessage(UI_TIPOYU_MOVE, m_Indoun_Dlg_Info.wObjectID, id, NULL);
		}
		Run(INDOUN_NPC_DIALOG_CLOSE);
		break;

	}
#endif // __TIPOJYU_CASTLE
}

//-----------------------------------------------------------------------------
// Process()
// : �� �����Ӹ��� ȣ��ȴ�.
//-----------------------------------------------------------------------------
void C_VS_UI_INDOUN_DIALOG::Process()
{
//	m_pC_button_group->Process();
}


//-----------------------------------------------------------------------------
// IsPixel()
// : �������� ����� üũ�Ѵ�.
//-----------------------------------------------------------------------------
bool C_VS_UI_INDOUN_DIALOG::IsPixel(int _x, int _y)
{
	if(Moving()) 
		return true;
	
	return	true;
}



void	C_VS_UI_INDOUN_DIALOG::InitPicture()
{
	NPC_INFO* ni=(*g_pNPCTable).GetData((unsigned int)m_Indoun_Dlg_Info.wNpcID);
	
	if(ni == NULL)
		m_iSprite = -1;
	else
		m_iSprite = ni->SpriteID;
		

//	m_iSprite	= 104;				// ServerNPC.inf�� ������������ �̰ɷ� ����.

	if(m_iSprite != -1)
	{
		m_p_NPCFace.Init( 2 );		// �ӽ÷� 1���� loading.. - -;;

		bool re;
		re = m_p_NPCFace.LoadFromFileData( 0, 0, SPK_NPCFACE, SPKI_NPCFACE );
		assert(re);
		re = m_p_NPCFace.LoadFromFileData( 1, m_iSprite, SPK_NPCFACE, SPKI_NPCFACE );
		assert(re);
	}
}



void	C_VS_UI_INDOUN_DIALOG::InitButton()
{
	// 2026-05-12 CRASH FIX: stack trace gosterdi — bos mapButtonNameList icin
	// it->c_str() dereferencing UB -> access violation. Server hostile/buggy
	// pakete (button size 0) karsi defansif. Empty ise erken return.
	// (DEBUG_ADD bu library'de yok — VS_UI standalone, Client/DebugInfo.h
	// dependency'i karistirmak istemiyoruz; silent skip yeterli.)
	if (m_Indoun_Dlg_Info.mapButtonNameList.empty()) {
		return;
	}

	// 2026-05-12: gpC_global_resource ve sub-pointer NULL guard
	if (gpC_global_resource == NULL || gpC_global_resource->m_pC_assemble_box_button_spk == NULL) {
		return;
	}

	std::vector<string>::iterator it;
	it	= m_Indoun_Dlg_Info.mapButtonNameList.begin();

	WORD	wCount	= 0;

	WORD	wWidth	= gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_DIALOG_TAG);
	WORD	wHeight	= gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_DIALOG_TAG);

	// Empty guard yukarida — burasi safe (en az 1 element var)
	string str	= it->c_str();

	for(;	it != m_Indoun_Dlg_Info.mapButtonNameList.end();	it++)
	{
		m_wSelectButton_Width	= wWidth + g_GetStringWidth(it->c_str(), gpC_base->m_dialog_menu_pi.hfont);
		m_wSelectButton_Height	= wHeight;
		
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(m_wSelectButton_Position_X, 
#if __CONTENTS(__DYNAMIC_UI)
			m_wSelectButton_Position_Y + ((m_wSelectButton_Height+3) * (wCount + 1)),
#else
			m_wSelectButton_Position_Y + (m_wSelectButton_Height * (wCount + 1)), 
#endif //__DYNAMIC_UI
			m_wSelectButton_Width,
			m_wSelectButton_Height,
			wCount, this, SELECT_BUTTON1));
	
		wCount++;
	}
}



void	C_VS_UI_INDOUN_DIALOG::InitDesc()
{
	char	charrStr[255]		= {'\0',};
	char	*pchStr				= NULL;
	char	*pchStrFind			= NULL;
	char	charrLinepid[10]	= {NULL,};

	WORD	wStrptr			= 0;

	string	str;

	m_vtDectList.clear();
	m_vtDectList.resize(0);

#if __CONTENTS(__DYNAMIC_UI)
	m_vtDectShowList.clear();
	m_vtDectShowList.resize(0);
#endif //__DYNAMIC_UI

	pchStr		=	const_cast<char*>(m_Indoun_Dlg_Info.strDesc.c_str());
	strcpy(charrLinepid, "\r\n");

	if(strstr(pchStr, "\r\n"))
	{
		strcpy(charrLinepid, "\r\n");
	}
	else if(strstr(pchStr, "\n"))
	{
		strcpy(charrLinepid, "\n");
	}

	pchStrFind	=	strstr(pchStr, charrLinepid);

 	if(pchStrFind == NULL)
	{
		m_vtDectList.push_back(m_Indoun_Dlg_Info.strDesc.c_str());
#if __CONTENTS(__DYNAMIC_UI)
		m_vtDectShowList.push_back("");
#endif //__DYNAMIC_UI
		return;
	}

	do
	{
		pchStrFind	=	strstr(pchStrFind, charrLinepid);

		memset(charrStr, NULL, sizeof(charrStr));
		strcat(charrStr, pchStr);

		if(pchStrFind)
		{
			wStrptr	= (WORD)pchStrFind	- (WORD)pchStr;
			charrStr[wStrptr]	= NULL;

			pchStrFind	+=	2;
			pchStr		=	pchStrFind;
		}
	
		str	= charrStr;
		m_vtDectList.push_back(str);
#if __CONTENTS(__DYNAMIC_UI)
		m_vtDectShowList.push_back("");
#endif //__DYNAMIC_UI
	} while(pchStrFind);
}





void	C_VS_UI_INDOUN_DIALOG::Picture_Show()
{
//	int _x = m_client_rect.x - m_p_NPCFace[0].GetWidth()-10, _y = m_client_rect.y;

	if(m_iSprite == -1)
		return;

	POINT point = {x + m_wPicture_Position_X,	y + m_wPicture_Position_Y};
	gpC_base->m_p_DDSurface_back->BltSprite(&point, &m_p_NPCFace[0]);
	
	if(m_p_NPCFace.GetSize()>1)
	{
		point.x+=17; point.y+=18;

#if __CONTENTS(__DYNAMIC_UI)
		if(m_Picture_Alpha<32)
		{
			if((int)m_tx-x<20)
			{
				m_Picture_Alpha+=2;
			}
		}
		gpC_base->m_p_DDSurface_back->BltSpriteAlpha(&point, &m_p_NPCFace[1],m_Picture_Alpha);
#else
		gpC_base->m_p_DDSurface_back->BltSprite(&point, &m_p_NPCFace[1]);
#endif //__DYNAMIC_UI
	}
}



void	C_VS_UI_INDOUN_DIALOG::TitleName_Show()
{
	int iFontLengthPosSize	= 0; //���� ������ ��ǥ ũ��
	iFontLengthPosSize	= g_GetStringWidth(m_Indoun_Dlg_Info.strDlgTitleName.c_str(), gpC_base->m_dialog_menu_pi.hfont);

	for(int i = 0; i < 2; i++)	//���� ���̰� �ϱ� ���� 2�� �׸���.
	{
		g_PrintColorStr(x + (m_wDialog_Width/2) + i + m_wDialog_Title_Postion_X - (iFontLengthPosSize/2), 
						y + m_wDialog_Title_Postion_Y + i, 
						m_Indoun_Dlg_Info.strDlgTitleName.c_str(), 
						gpC_base->m_info_pi, 
						RGB(0, 255, 255));
	}
}

#if __CONTENTS(!__DYNAMIC_UI)
void	C_VS_UI_INDOUN_DIALOG::ButtonName_Show()
{
	if(m_etIndounSelect	== ENTRANCE_DLG)
	{
		// 2026-05-12 CRASH FIX: InitButton ile ayni pattern — bos mapButtonNameList'te
		// it->c_str() dereferencing UB -> AV. Stack trace: ButtonName_Show -> Show ->
		// WindowManager::Show -> render loop her frame'de cagriliyor.
		if (m_Indoun_Dlg_Info.mapButtonNameList.empty()) {
			return;
		}
		if (gpC_global_resource == NULL || gpC_global_resource->m_pC_assemble_box_button_spk == NULL) {
			return;
		}

		std::vector<string>::iterator it;
		it	= m_Indoun_Dlg_Info.mapButtonNameList.begin();

		WORD wCount	= 0;

		WORD	wWidth	= gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_DIALOG_TAG);
		WORD	wHeight	= gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_DIALOG_TAG);

		COLORREF	colRef			= RGB(255, 255, 255);

		string	str;
		// Empty guard yukarida — burasi safe
		str	= it->c_str();
		for(;	it != m_Indoun_Dlg_Info.mapButtonNameList.end();	it++)
		{
			if(wCount == m_SelectButtonIdex)
				colRef	= RGB(0, 255, 255);
			else
				colRef	= RGB(255, 255, 255);


			g_PrintColorStr(x + m_wSelectButton_Position_X + wWidth + 5, 
							y + m_wSelectButton_Position_Y + wHeight * (wCount + 1) + 3, 
							it->c_str(), 
							gpC_base->m_info_pi, 
							colRef);
			
			wCount++;
		}
	}
}
#endif //__DYNAMIC_UI

void	C_VS_UI_INDOUN_DIALOG::NPCName_Show()
{
	if(m_iSprite == -1)
		return;

	WORD	wStrLineWidth	= 0;
	wStrLineWidth	= g_GetStringWidth(m_Indoun_Dlg_Info.strNpcName.c_str(), gpC_base->m_dialog_menu_pi.hfont);

	g_PrintColorStr(x + m_wNpcName_Position_X + m_wPicture_Position_X + (m_p_NPCFace[0].GetWidth()/2) - (wStrLineWidth/2), 
					y + m_wNpcName_Position_Y + m_wPicture_Position_Y + m_p_NPCFace[0].GetHeight(), 
					m_Indoun_Dlg_Info.strNpcName.c_str(), 
					gpC_base->m_info_pi, 
					RGB(255, 255, 255));
}


#if __CONTENTS(__DYNAMIC_UI)
void	C_VS_UI_INDOUN_DIALOG::Desc_Show()
{
	WORD		wStrLineWidth	= 0;
	WORD		wStrLineHeight	= 0;
	WORD		wPos_X			= 0;
	WORD		wPos_Y			= 0;

	COLORREF	colRef			= RGB(255, 255, 255);
	bool		ShowLineCopy	= true;

	if((int)m_tx-x>20)
	{
		ShowLineCopy = false;
	}

	for(int i = 0; i < m_vtDectList.size(); i++)
	{
		wStrLineWidth	= g_GetStringWidth(m_vtDectList[i].c_str(), gpC_base->m_dialog_menu_pi.hfont);
		wStrLineHeight	= g_GetStringHeight(m_vtDectList[i].c_str(), gpC_base->m_dialog_menu_pi.hfont);	

		wPos_X			= x + m_wDesc_Postion_X;
		wPos_Y			= y + m_wDesc_Postion_Y + ((wStrLineHeight + 6) * i);

		if(ShowLineCopy&&m_vtDectList[i].size()!=0&&m_vtDectShowList[i].size()<=m_vtDectList[i].size())
		{
			ShowLineCopy = false;
			m_vtDectShowList[i] += m_vtDectList[i][m_vtDectShowList[i].size()];
		}

		g_PrintColorStr(wPos_X, 
						wPos_Y, 
						m_vtDectShowList[i].c_str(), 
						gpC_base->m_dialog_msg_pi, 
						colRef);
	}
}
#else
void	C_VS_UI_INDOUN_DIALOG::Desc_Show()
{
	WORD		wStrLineWidth	= 0;
	WORD		wStrLineHeight	= 0;
	WORD		wPos_X			= 0;
	WORD		wPos_Y			= 0;

	COLORREF	colRef			= RGB(255, 255, 255);


	for(int i = 0; i < m_vtDectList.size(); i++)
	{
		wStrLineWidth	= g_GetStringWidth(m_vtDectList[i].c_str(), gpC_base->m_dialog_menu_pi.hfont);
		wStrLineHeight	= g_GetStringHeight(m_vtDectList[i].c_str(), gpC_base->m_dialog_menu_pi.hfont);	

		wPos_X			= x + m_wDesc_Postion_X;
		wPos_Y			= y + m_wDesc_Postion_Y + ((wStrLineHeight + 15) * i);

		g_PrintColorStr(wPos_X, 
						wPos_Y, 
						m_vtDectList[i].c_str(), 
						gpC_base->m_info_pi, 
						colRef);	
	}
}
#endif //__DYNAMIC_UI


void	C_VS_UI_INDOUN_DIALOG::Exp_Show()
{
	g_PrintColorStr(x + m_wExp_position_X, 
						y + m_wExp_position_Y , 
						m_Indoun_Dlg_Info.strExp.c_str(), 
						gpC_base->m_info_pi, 
						RGB(255, 255, 255));	
}


void	C_VS_UI_INDOUN_DIALOG::RankExp_Show()
{
	g_PrintColorStr(x + m_wExp_position_X, 
						y + m_wExp_position_Y , 
						m_Indoun_Dlg_Info.strRankExp.c_str(), 
						gpC_base->m_info_pi, 
						RGB(255, 255, 255));	
}



void	C_VS_UI_INDOUN_DIALOG::Duration_Show()
{
	g_PrintColorStr(x + m_wExp_position_X, 
						y + m_wExp_position_Y , 
						m_Indoun_Dlg_Info.strDuration.c_str(), 
						gpC_base->m_info_pi, 
						RGB(255, 255, 255));	
}




void	C_VS_UI_INDOUN_DIALOG::InitStarDlg()			//���� �Ҷ� ���� ����
{
	m_wDialog_Title_Postion_X	= 60;	//X ��ǥ�� ������ ���ͷ� ������ �ִ�.(���͸� �������� ���� �δ�.)
	m_wDialog_Title_Postion_Y	= 30;

	m_wDialog_Width				= 620;
	m_wDialog_Heigth			= 382;

	m_wSelectButton_Position_X	= 210;
	m_wSelectButton_Position_Y	= 220;

	m_wPicture_Position_X		= 40;
	m_wPicture_Position_Y		= 40;

	m_wNpcName_Position_X		= 0;	//X ��ǥ�� ���� �̹����� ���͸� �������� �Ѵ�.
	m_wNpcName_Position_Y		= 10;	//Y ��ǥ�� ���� �̹����� �ϴ��� �������� �Ѵ�.

	m_wDesc_Postion_X			= 210;	//������ �������� �Ѵ�.
	m_wDesc_Postion_Y			= 70;	//���̾� �α��� top�� �������� �Ѵ�.
	m_wDesc_Line_Widht			= 210;	//���� ������ ���� ��� ��			
	m_wDesc_Line_Height			= 100;

	Set((g_pUserInformation->iResolution_x/2) - (m_wDialog_Width/2), 
		(g_pUserInformation->iResolution_y/2) - (m_wDialog_Heigth/2), 
		m_wDialog_Width, 
		m_wDialog_Heigth);
	
#if __CONTENTS(__DYNAMIC_UI)
	m_tx = (float)x;
	x = (int)-100.0f;
	m_Picture_Alpha = 10;
#endif //__DYNAMIC_UI

	InitButton();
	InitPicture();
	InitDesc();
}



void	C_VS_UI_INDOUN_DIALOG::InitFinishDlg()			//���� ���� �� ���� ����
{
	m_wDialog_Title_Postion_X	= 0;	//X ��ǥ�� ������ ���ͷ� ������ �ִ�.(���͸� �������� ���� �δ�.)
	m_wDialog_Title_Postion_Y	= 50;

	m_wDialog_Width				= 620;
	m_wDialog_Heigth			= 382;

	m_wSelectButton_Position_X	= 310;
	m_wSelectButton_Position_Y	= 260;

	m_wPicture_Position_X		= 35;
	m_wPicture_Position_Y		= 40;

	m_wNpcName_Position_X		= 0;	//X ��ǥ�� ���� �̹����� ���͸� �������� �Ѵ�.
	m_wNpcName_Position_Y		= 10;	//Y ��ǥ�� ���� �̹����� �ϴ��� �������� �Ѵ�.

	m_wDesc_Postion_X			= 30;	//������ �������� �Ѵ�.
	m_wDesc_Postion_Y			= 85;	//���̾� �α��� top�� �������� �Ѵ�.
	m_wDesc_Line_Widht			= 210;	//���� ������ ���� ��� ��			
	m_wDesc_Line_Height			= 100;

	m_wExp_position_X			= m_wPicture_Position_X + 30;
	m_wExp_position_Y			= 100;

	m_wRankExp_position_X		= m_wPicture_Position_X + 30;
	m_wRankExp_position_Y		= 130;

	m_wDuration_Position_X		= m_wPicture_Position_X + 30;
	m_wDuration_Position_Y		= 160;

	Set((g_pUserInformation->iResolution_x/2) - (m_wDialog_Width/2), 
		(g_pUserInformation->iResolution_y/2) - (m_wDialog_Heigth/2), 
		m_wDialog_Width, 
		m_wDialog_Heigth);
	
#if __CONTENTS(__DYNAMIC_UI)
	m_tx = (float)x;
	x = (int)-100.0f;	
	m_Picture_Alpha = 10;
#endif //__DYNAMIC_UI

	InitButton();
	InitPicture();
	InitDesc();
}