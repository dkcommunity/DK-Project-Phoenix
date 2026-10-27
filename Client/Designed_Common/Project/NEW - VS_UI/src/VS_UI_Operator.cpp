#include "client_PCH.h"
#include "crash_trace_tool.h"

//#pragma warning(disable:4786)
#include "VS_UI_GameCommon.h"
#include "VS_UI.h"
#include "VS_UI_filepath.h"
#include "VS_UI_mouse_pointer.h"
#include "VS_UI_Operator.h"

#include "OperatorCategory.h"

#include "OperatorOption.h"

#if __CONTENTS(__ITEM_SHOW)
#include "UIDialog.h"
#endif //__ITEM_SHOW

C_VS_UI_OPERATOR::CATEGORY_ID C_VS_UI_OPERATOR::s_selected_category_id = static_cast<CATEGORY_ID>(0);

//-----------------------------------------------------------------------------
// C_VS_UI_OPERATOR()
// : 생성자
//-----------------------------------------------------------------------------
C_VS_UI_OPERATOR::C_VS_UI_OPERATOR()
{
	AttrTopmost(false);
	AttrPin(true);
	g_RegisterWindow(this);

	m_size_window.cx	= 700;
	m_size_window.cy	= 570;
	
	m_rect_category.SetRect(30, 30, 150, m_size_window.cy - 90
#if __CONTENTS(__MOVE_ZONE)
		- 28
#endif //__MOVE_ZONE
		);
	m_rect_detail.SetRect(158, 30, m_size_window.cx - 30, m_size_window.cy - 30);

	//////////////////////////////////////////////////////////////////////////
	// 버튼 추가
	m_pC_button_group	= new ButtonGroup(this);

	CRect buttonRect(30, m_size_window.cy - 83
#if __CONTENTS(__MOVE_ZONE)
		- 28
#endif //__MOVE_ZONE
		, 150, m_size_window.cy - 58
#if __CONTENTS(__MOVE_ZONE)
		- 28
#endif //__MOVE_ZONE
		);

#if __CONTENTS(__MOVE_ZONE)
	if(gC_vs_ui.IsMoveZoneUi())
	{
		m_pC_button_group->Add(new C_VS_UI_EVENT_PUSH_BUTTON(
			buttonRect,	EID_MOVE_ZONE, this, ""));
	
		buttonRect += CPoint(0, 28);
	}
#endif //__MOVE_ZONE

	m_pC_button_group->Add(new C_VS_UI_EVENT_PUSH_BUTTON(
		buttonRect,	EID_SET_DEFAULT, this, "Reset"));

	buttonRect += CPoint(0, 28);

	m_pC_button_group->Add(new C_VS_UI_EVENT_PUSH_BUTTON(
		buttonRect,	EID_CLOSE, this, "Close"));

	//////////////////////////////////////////////////////////////////////////
	// 카테고리 추가
	m_pC_category[CATEGORY_UTILITY]		= new COperatorCategoryUtility("Utility", this, m_rect_detail);
	m_pC_category[CATEGORY_SHOWINFO]	= new COperatorCategoryShowInfo("Info Display", this, m_rect_detail);
	m_pC_category[CATEGORY_GAMESETTING]	= new COperatorCategoryGameSetting("Game Settings", this, m_rect_detail);
	m_pC_category[CATEGORY_SIMPLECOMMAND] = new COperatorCategorySimpleCommand("Commands", this, m_rect_detail);
#if __CONTENTS(__ITEM_SHOW)
	m_pC_category[CATEGORY_ITEM_SHOW]	= new COperatorCategoryShowItemSetting("Items", this, m_rect_detail);
#endif //__ITEM_SHOW
/*	m_pC_category[CATEGORY_AUTO_QA]	= new COperatorCategoryAutoQA("Auto QA", this, m_rect_detail);

	m_MousePoint.x = 0;
	m_MousePoint.y = 0;  

	m_bDelete_Operator = false;*/
} 


//-----------------------------------------------------------------------------
// ~C_VS_UI_OPERATOR()
// : 파괴자
//-----------------------------------------------------------------------------
C_VS_UI_OPERATOR::~C_VS_UI_OPERATOR()
{
	g_UnregisterWindow(this);

	for(int i = 0; i < CATEGORY_MAX; ++i)
		DeleteNew(m_pC_category[i]);
	
	DeleteNew(m_pC_button_group);
}


//-----------------------------------------------------------------------------
// Start()
// : 시작에 관련된 처리
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::Start()
{
	PI_Processor::Start();

	//////////////////////////////////////////////////////////////////////////
	// 윈도우 설정
	gpC_window_manager->AppearWindow(this);
	
	m_pC_button_group->Init();

	m_bDelete_Operator = false;

	AttrTopmost(true);
	AttrPin(true);

	Set((RESOLUTION_X - m_size_window.cx) / 2,
		(RESOLUTION_Y - m_size_window.cy) / 2,
		m_size_window.cx, m_size_window.cy);

	for(int i = 0; i < CATEGORY_MAX; ++i)
		m_pC_category[i]->Start();

#if __CONTENTS(__ITEM_SHOW)
		if(s_selected_category_id==CATEGORY_ITEM_SHOW)
			gC_vs_ui.SetInputString("Item List.");
		else
			gC_vs_ui.SetInputString("");
#endif //__ITEM_SHOW

	//s_selected_category_id = static_cast<CATEGORY_ID>(0);
}


//-----------------------------------------------------------------------------
// Finish()
// : 끝에 관련된 처리
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::Finish()
{
	PI_Processor::Finish();

	for(int i = 0; i < CATEGORY_MAX; ++i)
		m_pC_category[i]->Finish();
	
	gpC_window_manager->DisappearWindow(this);
}


//-----------------------------------------------------------------------------
// ShowButtonWidget()
// : 버튼의 모양을 출력한다.
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	C_VS_UI_EVENT_STRING_BUTTON* pStringButton = dynamic_cast<C_VS_UI_EVENT_STRING_BUTTON*>(p_button);
	
	if(pStringButton)
		pStringButton->Show(CPoint(x, y));

#if __CONTENTS(__MOVE_ZONE)
	if(gC_vs_ui.IsMoveZoneUi())
	{
		if(p_button->GetFocusState())
		{
			// ToolTip
			switch(p_button->GetID())
			{
				case EID_MOVE_ZONE:
					if(gpC_mouse_pointer->IsMoveZoneEffect())
					{
						g_descriptor_manager.Set(DID_INFO, m_MousePoint.x, m_MousePoint.y, "Right-click to close.",0,0);
					}
					break;

				default:
					break;
			}
		}

		if(pStringButton->GetID()==EID_MOVE_ZONE)
		{
			POINT	txt_offset_pt;
			txt_offset_pt.x = x+7;
			txt_offset_pt.y = y+14;

			if(!gpC_mouse_pointer->IsMoveZoneEffect())
			{
				gpC_base->m_p_DDSurface_back->Lock();

				POINT	eff_pt;
				eff_pt.x = txt_offset_pt.x+pStringButton->x-26;
				eff_pt.y = txt_offset_pt.y+pStringButton->y+75;
	
				static bool flg;
				static int offset_count = 0;

				if(offset_count>=100)
					flg = true;
			
				if(offset_count<=0)
					flg = false;
		
				if(flg)
					offset_count-=3;
				else
					offset_count+=3;

				eff_pt.x += offset_count;

				DrawUIEffect(&eff_pt,(int)339,0,3,true);
				gpC_base->m_p_DDSurface_back->Unlock();

				g_FL2_GetDC();
				g_PrintColorStr(txt_offset_pt.x+pStringButton->x+23, txt_offset_pt.y+pStringButton->y-8, "Move Point", gpC_base->m_item_pi, RGB(255,255,255));
				g_FL2_ReleaseDC();
			}
			else
			{
				g_FL2_GetDC();
				g_PrintColorStr(txt_offset_pt.x+pStringButton->x+23, txt_offset_pt.y+pStringButton->y-8, "Move Stop", gpC_base->m_item_pi, RGB(255,255,255));
				g_FL2_ReleaseDC();
			}
		}
	}
#endif //__MOVE_ZONE
}

//-----------------------------------------------------------------------------
// ShowButtonDescription()
// : 버튼의 설명을 출력한다.
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{
}

//-----------------------------------------------------------------------------
// IsPixel()
// : 윈도우의 모양을 체크한다.
//-----------------------------------------------------------------------------
bool C_VS_UI_OPERATOR::IsPixel(int _x, int _y)
{
	if(Moving()) return true;
	return IsInRect(_x, _y);
}

//-----------------------------------------------------------------------------
// Run()
// : 버튼을 클릭했을때 오는 메시지를 처리한다.
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::Run(id_t id)
{
	switch (id)
	{	
	case EID_SET_DEFAULT:
		g_pOperatorOption->SetDefault();
		CloseOperator();
		break;

	case EID_CLOSE:
		CloseOperator();
		break;

#if __CONTENTS(__MOVE_ZONE)
	case EID_MOVE_ZONE:
		if(gC_vs_ui.IsMoveZoneUi())
		{
			if(!gpC_mouse_pointer->IsMoveZoneEffect())
			{
				gpC_mouse_pointer->SetMoveZoneEffect();
				CloseOperator();
				gC_vs_ui.RunWorldMap();
			}
			else
			{
				gpC_mouse_pointer->UnSetMoveZoneEffect();
			}
		}
		break;
#endif //__MOVE_ZONE
	}
}

//-----------------------------------------------------------------------------
// Process()
// : 각 프레임마다 호출된다.
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::Process()
{
	if(m_bDelete_Operator)
	{
		// 아래함수에서 C_VS_UI_OPERATOR객체를 Delete 하기때문에, 이 코드후에
		// 또 C_VS_UI_OPERATOR내의 루틴을 수행할 수가 없다.
		// 릴리즈 모드에서 초기화 버튼, 닫기버튼을 클릭하면 클라가 뻗었던 원인은 그것이다.

		gC_vs_ui.CloseOperator();
		return;
	}

	m_pC_button_group->Process();

	if(IsValidCategoryID(s_selected_category_id))
		m_pC_category[s_selected_category_id]->Process();
}

//-----------------------------------------------------------------------------
// Show()
// : 윈도우를 출력한다.
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::Show()
{
	CT_Done("WnSh: C_VS_UI_OPERATOR::Show ENTER pthis=%p", (void*)this);
	//////////////////////////////////////////////////////////////////////////
	// 카테고리와 디테일의 구분 상자 출력

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		gpC_global_resource->DrawDialogLocked(x, y, w, h, GetAttributes()->alpha);

		// 카테고리
		CRect rcCategory(m_rect_category);
		rcCategory.OffsetRect(x, y);

		gpC_global_resource->DrawInBoxLocked(
			rcCategory.left, rcCategory.top,
			rcCategory.Width(), rcCategory.Height());

		// 자세히
		CRect rcDetail(m_rect_detail);
		rcDetail.OffsetRect(x, y);

		gpC_global_resource->DrawInBoxLocked(
			rcDetail.left, rcDetail.top,
			rcDetail.Width(), rcDetail.Height());

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	//////////////////////////////////////////////////////////////////////////
	// 카테고리 출력

	CPoint	ptNamePosition(m_rect_category.TopLeft());
	CSize	sizeDrawNameGap(8, 5);
	CSize	sizeDrawSelectBoxGap(-3, 2);

	ptNamePosition.y += sizeDrawNameGap.cy;

	for(int i = 0; i < CATEGORY_MAX; ++i)
	{
		LPCTSTR pCategoryName	= m_pC_category[i]->GetCategoryName();
		bool	bSelected		= s_selected_category_id == i;

		g_FL2_GetDC();

		g_PrintColorStrShadow(
			x + ptNamePosition.x + sizeDrawNameGap.cx,
			y + ptNamePosition.y + sizeDrawNameGap.cy,
			pCategoryName, gpC_base->m_desc_menu_pi,
			bSelected ? RGB_GREEN : RGB_WHITE, RGB_BLACK);

		g_FL2_ReleaseDC();

		const int nFontHeight	= g_GetStringHeight(pCategoryName, gpC_base->m_desc_menu_pi.hfont);
		const int nBoxHeight	= nFontHeight + (sizeDrawNameGap.cy << 1);

		if(bSelected)
		{
			gpC_global_resource->DrawOutBox(
				x + ptNamePosition.x - sizeDrawSelectBoxGap.cx,
				y + ptNamePosition.y - sizeDrawSelectBoxGap.cy,
				m_rect_category.Width() + (sizeDrawSelectBoxGap.cx << 1),
				nBoxHeight + (sizeDrawSelectBoxGap.cy << 1));
		}

		ptNamePosition.y += nBoxHeight;
	}

	//////////////////////////////////////////////////////////////////////////
	// 디테일 출력
	if(IsValidCategoryID(s_selected_category_id))
		m_pC_category[s_selected_category_id]->Show();


	//////////////////////////////////////////////////////////////////////////
	// 버튼 출력
	m_pC_button_group->Show();
}


//-----------------------------------------------------------------------------
// MouseControl()
// : 마우스 메시지를 처리힌다.
//-----------------------------------------------------------------------------
bool C_VS_UI_OPERATOR::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	
	CPoint ptLocalMouse(_x - x, _y - y);

	m_MousePoint.x = x+ptLocalMouse.x;
	m_MousePoint.y = y+ptLocalMouse.y;

	m_pC_button_group->MouseControl(message, ptLocalMouse.x, ptLocalMouse.y);

	bool bClickMessage	= message == M_LEFTBUTTON_DOWN || message == M_LB_DOUBLECLICK;
#if __CONTENTS(__ITEM_SHOW)
	bool bMoveWindows	= true;
#endif //__ITEM_SHOW

	//////////////////////////////////////////////////////////////////////////
	// 카테고리 마우스 처리
	if(bClickMessage && m_rect_category.PtInRect(ptLocalMouse))
	{
		int	nGapHeight	= 10;
		int	nFontHeight	= 14;
		int nCategoryY	= ptLocalMouse.y - nGapHeight;
		int	nSelected	= nCategoryY / (nFontHeight + nGapHeight) - 1;
		CATEGORY_ID categorySelected = static_cast<CATEGORY_ID>(nSelected);

		if(IsValidCategoryID(categorySelected))
			s_selected_category_id = categorySelected;

#if __CONTENTS(__ITEM_SHOW)
		if(categorySelected==CATEGORY_ITEM_SHOW)
			gC_vs_ui.SetInputString("Item List.");
		else
			gC_vs_ui.SetInputString("");

		bMoveWindows = false;
#endif //__ITEM_SHOW
	}

#if __CONTENTS(__ITEM_SHOW)
	//////////////////////////////////////////////////////////////////////////
	// "ItemName.txt"저장 (ISpriteEditor에서 Item리소스 뽑을때 참조하기 위한 데이터)
	if(m_rect_category.PtInRect(ptLocalMouse))
	{
		int	nGapHeight	= 10;
		int	nFontHeight	= 14;
		int nCategoryY	= ptLocalMouse.y - nGapHeight;
		int	nSelected	= nCategoryY / (nFontHeight + nGapHeight) - 1;
		CATEGORY_ID categorySelected = static_cast<CATEGORY_ID>(nSelected);
		
		if(categorySelected==CATEGORY_ITEM_SHOW)
		{
			static char st_str_Description[1024] = {"Right-click to save item name list."};

			if(lstrlen(st_str_Description)!=0)
			g_descriptor_manager.Set(DID_MULTILINE_INFO, x, y, (void *)st_str_Description,RGB(200,0,0),60);

			if(message == M_RIGHTBUTTON_DOWN)
			{
				// Output Item FileName
				char Wdr[256];
				GetCurrentDirectory(256,Wdr);
				
				char strItemName[256];
				sprintf(strItemName,"> %s\\%s saved.",Wdr,"ItemName.txt");
				g_pUIDialog->PopupFreeMessageDlg(strItemName);
				
				FILE *fp = fopen("ItemName.txt","w+t");
				int nFull = 0;
			
				for(int i=0; i<(*g_pItemTable).GetSize(); i++)
				for(int j=0; j<(*g_pItemTable)[i].GetSize(); j++)
				{
					if((*g_pItemTable)[i][j].InventoryFrameID==FRAMEID_NULL)
						continue;

					memset(strItemName,0,256);
					sprintf(strItemName,"%%%d %d %d %s(%s)",(*g_pItemTable)[i][j].InventoryFrameID,i,j,(*g_pItemTable)[i][j].HName.GetString(),(*g_pItemTable)[i][j].EName.GetString());
					fprintf(fp,"%s\n",strItemName);
					nFull++;
				}

				fprintf(fp,"Total: %d\n",nFull);
				fclose(fp);
			}
		}
	}
#endif //__ITEM_SHOW

	//////////////////////////////////////////////////////////////////////////
	// 디테일 마우스 처리
	if(m_rect_detail.PtInRect(ptLocalMouse))
	{
		if(IsValidCategoryID(s_selected_category_id))
		{
			CPoint ptDetailMouse(ptLocalMouse.x - m_rect_detail.left,
								 ptLocalMouse.y - m_rect_detail.top);

			if(s_selected_category_id<MAX_ITEM_CLASS)
				m_pC_category[s_selected_category_id]->MouseControl(
					message, ptDetailMouse.x, ptDetailMouse.y);
		}
#if __CONTENTS(__ITEM_SHOW)
		bMoveWindows = false;
#endif //__ITEM_SHOW
	}

#if __CONTENTS(__ITEM_SHOW)
	if(bClickMessage&&bMoveWindows&&gC_vs_ui.inventory_mode==NULL&&gpC_mouse_pointer->GetPickUpItem()==NULL)
	{
		_x-=x; _y-=y;
		MoveReady();
		SetOrigin(_x, _y);
	}
#endif //__ITEM_SHOW

	return true;
}


//-----------------------------------------------------------------------------
// KeyboardControl()
// : 키보드 메시지를 처리힌다.
//-----------------------------------------------------------------------------
void C_VS_UI_OPERATOR::KeyboardControl(UINT message, UINT key, long extra)
{
	if(message == WM_KEYDOWN && key == VK_ESCAPE)
		CloseOperator();
}
