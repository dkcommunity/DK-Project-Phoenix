// VS_UI_PremiumGiveItem.cpp
#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "MTopView.h"
#include "VS_UI.h"

#include "VS_UI_filepath.h"
#include "UserOption.h"
#include "MItemOptionTable.H"
#include "mgamestringtable.H"

#include "MPremiumGiveItemInfo.h"
#include "VS_UI_PremiumGiveItem.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Dynamic Ui Process Start
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define PI 3.141592f

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
//-----------------------------------------------------------------------------
// DYNAMIC_DESCITEM_SEL_INFO::Process
//-----------------------------------------------------------------------------
void DYNAMIC_DESCITEM_SEL_INFO::Process(int id)
{
	float _Limit = 1.0f;

	switch(id)
	{
	case 0:
	case 1:
	case 2:
		{
			m_x = m_x+m_speed*(m_tx-m_x);
			m_y = m_y+m_speed*(m_ty-m_y);
			m_color = m_color+(m_speed/2)*(m_tcolor-m_color);

			if(m_x-m_tx<=_Limit && m_x-m_tx>=-_Limit)
			{
				m_x = m_tx;
			}
			if(m_y-m_ty<=_Limit && m_y-m_ty>=-_Limit)
			{
				m_y = m_ty;
			}
			if(m_color-m_tcolor<=(_Limit) && m_color-m_tcolor>=-(_Limit))
			{
				m_color = m_tcolor;
			}
		}
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// DYNAMIC_DESCITEM_MAP_INFO::Process
//-----------------------------------------------------------------------------
void DYNAMIC_DESCITEM_MAP_INFO::Process(int id)
{
	float _Limit = 1.0f;

	switch(id)
	{
	case -1:
		break;

	case 3:
		{
			bool x_end = true;

			for(int i=0; i<m_h; i++)
			for(int j=0; j<m_w; j++)
			{
				float amp = m_amp_add * sinf(2*PI*((g_CurrentFrame*5)-j)/get_w());

				ImageMap[i][j].m_x = ImageMap[i][j].m_x+ImageMap[i][j].m_speed*(ImageMap[i][j].m_tx-ImageMap[i][j].m_x);
				ImageMap[i][j].m_y = ImageMap[0][0].m_ty + i + 1 * amp;
				ImageMap[i][j].m_color = ImageMap[i][j].m_color+(ImageMap[i][j].m_speed/2)*(ImageMap[i][j].m_tcolor-ImageMap[i][j].m_color);

				if(ImageMap[i][j].m_x-ImageMap[i][j].m_tx<=_Limit && ImageMap[i][j].m_x-ImageMap[i][j].m_tx>=-_Limit)
				{
					ImageMap[i][j].m_x = ImageMap[i][j].m_tx;
				}
				else
				{
					x_end = false;
				}
				if(m_amp_add<_Limit)
				{
					ImageMap[i][j].m_y = ImageMap[i][j].m_ty;
				}
				if(ImageMap[i][j].m_color-ImageMap[i][j].m_tcolor<=(_Limit) && ImageMap[i][j].m_color-ImageMap[i][j].m_tcolor>=-(_Limit))
				{
					ImageMap[i][j].m_color = ImageMap[i][j].m_tcolor;
				}
			}

			if(x_end)
			{
				m_amp_add -= 0.3f;
			}
		}
		break;

	default:
		{
			for(int i=0; i<m_h; i++)
			for(int j=0; j<m_w; j++)
			{
				ImageMap[i][j].Process(id);
			}
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// DYNAMIC_DESCITEM_MAP_INFO::SetStartingPoint
//-----------------------------------------------------------------------------
void DYNAMIC_DESCITEM_MAP_INFO::SetStartingPoint(float _x, float _y, int id)
{
	float _color = 0.0f;

	switch(id)
	{
	case 0:
		{
			for(int i=0; i<get_h(); i++)
			{
				int pos  = 0;
	
				for(int j=0; j<get_w(); j++)
				{
					ImageMap[i][j].set_x(_x+get_w()+j+(pos));
					ImageMap[i][j].set_y(_y+i);
					ImageMap[i][j].set_color(_color);
				}
			}
		}
		break;

	case 1:
		{
			for(int i=0; i<get_h(); i++)
			{
				int pos  = rand()%32;
	
				for(int j=0; j<get_w(); j++)
				{
					if(i%2==0)
					{
						ImageMap[i][j].set_x(_x+get_w()+j+(pos));
						ImageMap[i][j].set_y(_y+i);
						ImageMap[i][j].set_color(_color);
					}
					else
					{
						ImageMap[i][j].set_x(_x-get_w()+j-(pos));
						ImageMap[i][j].set_y(_y+i);
						ImageMap[i][j].set_color(_color);
					}
				}
			}
		}
		break;

	case 2:
		{
			set_speed(0.2f);

			for(int i=0; i<get_h(); i++)
			for(int j=0; j<get_w(); j++)
			{
				ImageMap[i][j].set_x(_x+get_w());
				ImageMap[i][j].set_y(_y+0);
				ImageMap[i][j].set_color(32.0f);
			}
		}
		break;

	case 3:
		{
			for(int i=0; i<get_h(); i++)
			for(int j=0; j<get_w(); j++)
			{
				ImageMap[i][j].set_x(_x+get_w()+j);
				ImageMap[i][j].set_ty(_y+i);
				ImageMap[i][j].set_color(_color);
			}
		}
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// DYNAMIC_DESCITEM_MAP_INFO::SetEndingPoint
//-----------------------------------------------------------------------------
void DYNAMIC_DESCITEM_MAP_INFO::SetEndingPoint(float _x, float _y, int id)
{
	float _color = 0.0f;
	m_IsSet_EndingPoint = true;

	switch(id)
	{
	case 0:
		{
			for(int i=0; i<get_h(); i++)
			{
				int pos  = 0;
	
				for(int j=0; j<get_w(); j++)
				{
					ImageMap[i][j].set_tx(_x+j+(pos)-get_w());
					ImageMap[i][j].set_ty(_y+i);
					ImageMap[i][j].set_tcolor(_color);
				}
			}
		}
		break;

	case 1:
		{
			for(int i=0; i<get_h(); i++)
			{
				int pos  = rand()%32;
	
				for(int j=0; j<get_w(); j++)
				{
					if(i%2==0)
					{
						ImageMap[i][j].set_tx(_x+j+(pos)-get_w());
						ImageMap[i][j].set_ty(_y+i);
						ImageMap[i][j].set_tcolor(_color);
					}
					else
					{
						ImageMap[i][j].set_x(_x-get_w()+j-(pos));
						ImageMap[i][j].set_y(_y+i);
						ImageMap[i][j].set_tcolor(_color);
					}
				}
			}
		}
		break;

	case 2:
		{
			set_speed(0.2f);

			for(int i=0; i<get_h(); i++)
			for(int j=0; j<get_w(); j++)
			{
				ImageMap[i][j].set_tx(_x+0);
				ImageMap[i][j].set_ty(_y+get_h());
				ImageMap[i][j].set_color(32.0f);
				ImageMap[i][j].set_tcolor(32.0f);
			}
		}
		break;

	case 3:
		{
			for(int i=0; i<get_h(); i++)
			for(int j=0; j<get_w(); j++)
			{
				ImageMap[i][j].set_tx(_x+j-get_w());
				ImageMap[i][j].set_ty(_y+i);
				ImageMap[i][j].set_tcolor(_color);
			}
		}
		break;

	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Dynamic Ui Process End
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int C_VS_UI_PREMIUM_GIVE_ITEM::m_saveSelectIndexItem = 0;

//-----------------------------------------------------------------------------
// C_VS_UI_PREMIUM_GIVE_ITEM
//-----------------------------------------------------------------------------
C_VS_UI_PREMIUM_GIVE_ITEM::C_VS_UI_PREMIUM_GIVE_ITEM()
{
	m_id_DynamicDesc_Process = 2;

	m_pC_main_spk = NULL;
	m_Give_Item_Spk = NULL;
	IsDescSpkList = NULL;

	m_pItemListBox = NULL;

	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		m_ui_plus_x = 0; m_ui_plus_y = 0;
		m_pC_main_spk = new C_SPRITE_PACK(SPK_PREMIUM_GIVE_ITEM_SLAYER);
		break;

	case RACE_VAMPIRE:
		m_ui_plus_x = 0; m_ui_plus_y = 0;
		m_pC_main_spk = new C_SPRITE_PACK(SPK_PREMIUM_GIVE_ITEM_VAMPIRE);
		break;

	case RACE_OUSTERS:
		m_ui_plus_x = 8; m_ui_plus_y = 10;
		m_pC_main_spk = new C_SPRITE_PACK(SPK_PREMIUM_GIVE_ITEM_OUSTERS);
		break;
	}
	//CRect position(132, 41, 174, 112); // ListBox X, Y, Width, Height
	CRect position(184, 46, 184, 131);
	m_rcItemListBOX.SetRect(position.left, position.top, position.left+position.right, position.top+position.bottom);

	m_rcItemListBOX.left += m_ui_plus_x;
	m_rcItemListBOX.right += m_ui_plus_x;
	m_rcItemListBOX.top += m_ui_plus_y;
	m_rcItemListBOX.bottom += m_ui_plus_y;

	m_pItemListBox = new C_VS_UI_LISTBOX(m_ItemStrList, m_rcItemListBOX, EID_LISTBOX, this, &gpC_base->m_chatting_pi);
	m_pItemListBox->UnSetDescription();

	Set(g_pUserInformation->iResolution_x /2 - m_pC_main_spk->GetWidth()/2, g_pUserInformation->iResolution_y /2 - m_pC_main_spk->GetHeight()/2, m_pC_main_spk->GetWidth(), m_pC_main_spk->GetHeight());

	m_bActiveDlg = false;
	AttrTopmost(false);
	AttrPin(true);

	m_pC_button_group = new ButtonGroup(this);

	POINT Button_Pt[EID_MAX] =
/*	{
		275, 168,
		250, 168,
		29, 164,
		99, 164
	};*/
	{
		410, 10, // EID_CLOSE (Kapat - X butonu)
		370, 12, // EID_ALPHA (Seffaflık butonu)
		220, 190, // EID_GET (Get butonu)
		340, 190  // EID_GET_ALL (Get All butonu)
	};

	for(int i=0, j=1; i<EID_MAX; i++, j+=3)
	{
		Button_Pt[i].x += m_ui_plus_x;
		Button_Pt[i].y += m_ui_plus_y;

		m_pC_button_group->Add( new C_VS_UI_EVENT_BUTTON(Button_Pt[i].x, Button_Pt[i].y, m_pC_main_spk->GetWidth(j), m_pC_main_spk->GetHeight(j), i, this, j) );
	}

	m_pC_button_group->Add(m_pItemListBox);
	
	m_MousePoint.x = 0;
	m_MousePoint.y = 0;
	
	// SetRect(Sol, Ust, Sag, Alt)
	//m_rcShowDesc.SetRect(20, 41, 20+110, 41+110);
	m_rcShowDesc.SetRect(67, 50, 67+120, 50+130);
	
	m_rcShowDesc.left += m_ui_plus_x;
	m_rcShowDesc.right += m_ui_plus_x;
	m_rcShowDesc.top += m_ui_plus_y;
	m_rcShowDesc.bottom += m_ui_plus_y;

	m_bDescOver = false;

	PlaySound(SOUND_EVENT_FANFARE);

	Give_Item_List_ClearAll();
	g_RegisterWindow(this);
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_PREMIUM_GIVE_ITEM
//-----------------------------------------------------------------------------
C_VS_UI_PREMIUM_GIVE_ITEM::~C_VS_UI_PREMIUM_GIVE_ITEM()
{
	if(m_pItemListBox)
		m_saveSelectIndexItem = m_pItemListBox->GetSelectListIndex();
	
	gC_vs_ui.UnSetAccelMode();
	
	g_UnregisterWindow(this);
	Give_Item_List_ClearAll();
	
	DeleteNew(m_pC_button_group);
	
	DeleteNew(m_pC_main_spk);
	DeleteNew(m_Give_Item_Spk);
	DeleteNewArray(IsDescSpkList);
}

//-----------------------------------------------------------------------------
// ShowButtonWidget
//-----------------------------------------------------------------------------
void	C_VS_UI_PREMIUM_GIVE_ITEM::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	if(p_button->GetID() < EID_MAX && p_button->GetID()>=0)
	{
		if(p_button->GetFocusState())
		{
			// ToolTip
			gpC_base->m_p_DDSurface_back->Unlock();
			switch(p_button->GetID())
			{
				case EID_CLOSE:
					g_descriptor_manager.Set(DID_INFO, m_MousePoint.x, m_MousePoint.y,
						(*g_pGameStringTable)[UI_STRING_MESSAGE_CLOSE_WINDOW].GetString(),0,0);
					break;

				case EID_ALPHA:
					if(GetAttributes()->alpha)
					{
						g_descriptor_manager.Set(DID_INFO, m_MousePoint.x, m_MousePoint.y,
							(*g_pGameStringTable)[UI_STRING_MESSAGE_SHOW_NO_ALPHA_WINDOW].GetString(),0,0);
					}
					else
					{
						g_descriptor_manager.Set(DID_INFO, m_MousePoint.x, m_MousePoint.y,
							(*g_pGameStringTable)[UI_STRING_MESSAGE_SHOW_ALPHA_WINDOW].GetString(),0,0);
					}
					break;
					
				case EID_GET:
					g_descriptor_manager.Set(DID_INFO, m_MousePoint.x, m_MousePoint.y,
						(*g_pGameStringTable)[UI_STRING_MESSAGE_PREMIUM_GIVE_ITEM_TOOLTIP_GET].GetString(),0,0);
					break;

				case EID_GET_ALL:
					g_descriptor_manager.Set(DID_INFO, m_MousePoint.x, m_MousePoint.y,
						(*g_pGameStringTable)[UI_STRING_MESSAGE_PREMIUM_GIVE_ITEM_TOOLTIP_GET_ALL].GetString(),0,0);
					break;
			}
			gpC_base->m_p_DDSurface_back->Lock();
			// End ToolTip

			if(p_button->GetPressState())
				m_pC_main_spk->BltLocked(x+p_button->x, y+p_button->y, p_button->GetID()*3+3);
			else
				m_pC_main_spk->BltLocked(x+p_button->x, y+p_button->y, p_button->GetID()*3+2);
		}
		else
			m_pC_main_spk->BltLocked(x+p_button->x, y+p_button->y, p_button->GetID()*3+1);
	}

	if(p_button->GetID() == EID_LISTBOX)
	{
		C_VS_UI_LISTBOX* pListString = dynamic_cast<C_VS_UI_LISTBOX*>(p_button);

		if(pListString)
		{
			CPoint showOffset(x,y);
			gpC_base->m_p_DDSurface_back->Unlock();
			pListString->Show(showOffset);
			gpC_base->m_p_DDSurface_back->Lock();
		}
	}
}

//-----------------------------------------------------------------------------
// Process
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Process()
{
	m_pC_button_group->Process();

	Dynamic_DescItem_Process();
}

//-----------------------------------------------------------------------------
// Start
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Start()
{
	PI_Processor::Start();
	m_bActiveDlg = true;

	m_pC_button_group->Init();

	Update_Item_List();
	Set_UIEffect_Frame_Zero();
	
	gpC_window_manager->AppearWindow(this);

	AttrTopmost(true);
	AttrPin(true);
}

//-----------------------------------------------------------------------------
// Finish
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Finish()
{
	if(m_pC_main_spk)
	{
		DeleteNew(m_pC_main_spk);
		m_pC_main_spk = NULL;
	}

	PI_Processor::Finish();
	m_bActiveDlg = false;

	gpC_window_manager->DisappearWindow(this);
}

//-----------------------------------------------------------------------------
// Update_Item_List
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Update_Item_List()
{
	Give_Item_List_ClearAll();
	g_pPremiumGiveItemInfoList->SetBegin();
	m_pItemListBox->DelListNodeAll();

	while(g_pPremiumGiveItemInfoList->IsNotEnd())
	{
		MPremiumGiveItemInfo* pGiveItem = g_pPremiumGiveItemInfoList->Get();

		int _Class = pGiveItem->GetItem()->GetItemClass();
		int _Type = pGiveItem->GetItem()->GetItemType();
		if((*g_pItemTable).GetSize()<=_Class)	{ _Class = 0; }
		if((*g_pItemTable)[_Class].GetSize()<=_Type)	{ _Type = 0; }
		
		//////////////////////////////////////////////////////////////////////////
		// Prefix to be added to ItemName
		char sz_name[256];
		char sz_ename[256];
		sz_name[0] = '\0';
		sz_ename[0] = '\0';
		
		strcat(sz_ename, "(");
	
		// Time-limited items
		if ( pGiveItem->GetItem()->IsTimeLimitStatic() )
		{
			strcat( sz_name, (*g_pGameStringTable)[UI_STRING_MESSAGE_QUEST_HAN].GetString() );
			strcat( sz_name, " " );
			strcat( sz_ename,(*g_pGameStringTable)[UI_STRING_MESSAGE_QUEST_ENG].GetString() );
			strcat( sz_ename, " " );
		}
		
		// Items not available here
		strcat( sz_name, pGiveItem->GetItem()->GetName()); //(*g_pItemTable)[_Class][_Type].HName.GetString();
		strcat( sz_ename, pGiveItem->GetItem()->GetEName()); //(*g_pItemTable)[_Class][_Type].EName.GetString();
	/*
		// Item Tier
		if( pGiveItem->GetItem()->GetGrade() > 0 &&  pGiveItem->GetItem()->GetGrade() <= 10 &&
			pGiveItem->GetItem()->IsGearItem() && !pGiveItem->GetItem()->IsUniqueItem() &&
			pGiveItem->GetItem()->GetItemClass() != ITEM_CLASS_CORE_ZAP &&
			pGiveItem->GetItem()->GetItemClass() != ITEM_CLASS_CUE_OF_ADAM)
		{
			static char *szGrade = (*g_pGameStringTable)[UI_STRING_MESSAGE_ITEM_GRADE].GetString();
			strncat( sz_name, szGrade+pGiveItem->GetItem()->GetGrade()*2, 2 );
			strncat( sz_ename, szGrade+pGiveItem->GetItem()->GetGrade()*2, 2 );
		}
	*/
		strcat(sz_ename, ")");

		if(!pGiveItem->GetItem()->IsIdentified())
		{
			for(int i = 0; i < strlen(sz_name); i++)
				sz_name[i] = '?';
			
			for(int i = 0; i < strlen(sz_ename); i++)
				sz_ename[i] = '?';
		}

	//	strcat(sz_name, sz_ename);
		//////////////////////////////////////////////////////////////////////////

		m_pItemListBox->AddString(sz_name);

		Give_Item_List_Info_Insert(pGiveItem);

		g_pPremiumGiveItemInfoList->Next();
	}

	//ItemImage_Loding();
	
	if(m_saveSelectIndexItem<0 || m_saveSelectIndexItem >= m_pItemListBox->ListNodeMax())
	{
		m_saveSelectIndexItem = 0;
	}
	
	m_pItemListBox->SetScrollPos(m_saveSelectIndexItem);
	m_pItemListBox->SetSelectListIndex(m_saveSelectIndexItem);

	Dynamic_DescItem_Insert(m_saveSelectIndexItem);
}

//-----------------------------------------------------------------------------
// Run
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Run(id_t id)
{
	switch (id)
	{
	case EID_CLOSE:
		Finish();
		break;

	case EID_ALPHA:
		AttrAlpha(!GetAttributes()->alpha);
		break;

	case EID_GET:
		Set_UIEffect_Frame_Zero();
		gpC_base->SendMessage(UI_SELECT_WEEKITEM, m_Give_Item_List[m_saveSelectIndexItem]->GetId(), m_saveSelectIndexItem);
		
		if(m_saveSelectIndexItem>0)
		{ m_saveSelectIndexItem--; }

		gC_vs_ui.CloseInventory();
		gC_vs_ui.HotKey_Inventory(true);
		
		Dynamic_DescItem_Insert(m_saveSelectIndexItem);
		break;

	case EID_GET_ALL:
		Set_UIEffect_Frame_Zero();
		gpC_base->SendMessage(UI_SELECT_ALL_WEEKITEM);
		
		if(m_saveSelectIndexItem>0)
		{ m_saveSelectIndexItem--; }

		gC_vs_ui.CloseInventory();
		gC_vs_ui.HotKey_Inventory(true);
		
		Dynamic_DescItem_Insert(m_saveSelectIndexItem);
		break;
	}
}

//-----------------------------------------------------------------------------
// MouseControl
//-----------------------------------------------------------------------------
bool C_VS_UI_PREMIUM_GIVE_ITEM::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	CPoint ptLocalMouse(_x - x, _y - y);

	_x-=x; _y-=y;

	m_MousePoint.x = x+ptLocalMouse.x;
	m_MousePoint.y = y+ptLocalMouse.y;

	bool bMoveWindows	= true;
	bool re = m_pC_button_group->MouseControl(message, _x, _y);

	CPoint ptDetailMouse(_x, _y);
	bool ItemList_bt = m_pItemListBox->MouseControl(message, ptDetailMouse.x, ptDetailMouse.y);

	switch (message)
	{
		case M_MOVING :
			// Display information about the currently selected item in the list box
			if(m_pItemListBox->GetOverSelectListIndex()!=-1 && m_pItemListBox->GetOverSelectListIndex()<m_pItemListBox->ListNodeMax())
			{
				if(m_rcItemListBOX.PtInRect(ptLocalMouse))
				{
					const MItem * p_selected_item;
					p_selected_item = m_Give_Item_List[m_pItemListBox->GetOverSelectListIndex()]->GetItem();
					if (p_selected_item != NULL)
					{
						g_descriptor_manager.Set(DID_ITEM, m_MousePoint.x, m_MousePoint.y, (void *)p_selected_item);
					}
				}
			}

			// Display item information using DESCSHOW
			m_bDescOver = false;
			if(m_saveSelectIndexItem!=-1 && m_saveSelectIndexItem<m_pItemListBox->ListNodeMax())
			{
				if(m_rcShowDesc.PtInRect(ptLocalMouse))
				{
					const MItem * p_selected_item;
					p_selected_item = m_Give_Item_List[m_saveSelectIndexItem]->GetItem();
					if (p_selected_item != NULL)
					{
						g_descriptor_manager.Set(DID_ITEM, m_MousePoint.x, m_MousePoint.y, (void *)p_selected_item);
						m_bDescOver = true;
					}
				}
			}
			break;

		case M_LB_DOUBLECLICK:
			{
				if(m_saveSelectIndexItem!=-1 && m_saveSelectIndexItem<m_pItemListBox->ListNodeMax())
				{
					if(m_rcShowDesc.PtInRect(ptLocalMouse))
					{
						Run(EID_GET);
					}
				}
	
				int DoubleClickIndex = DoubleClickIndex = m_pItemListBox->PopDoubleClickIndex();
				if(DoubleClickIndex!=-1 && DoubleClickIndex<m_pItemListBox->ListNodeMax())
				{
					if(m_rcItemListBOX.PtInRect(ptLocalMouse))
					{
						Run(EID_GET);
					}
				}
			}
			break;

		case M_LEFTBUTTON_DOWN:	
			if(m_rcItemListBOX.PtInRect(ptLocalMouse))
			{		
				bMoveWindows = false;
			}

			if(bMoveWindows && gpC_mouse_pointer->GetPickUpItem() == NULL && re && gC_vs_ui.inventory_mode==NULL)
			{
				MoveReady();
				SetOrigin(_x, _y);
				break;
			}
			break;

		case M_LEFTBUTTON_UP:
			CancelPushState();
			break;
	}

	bool bClickMessage = (message==M_LEFTBUTTON_DOWN||message==M_LB_DOUBLECLICK);

	// Select Item..
	if(!ItemList_bt&&bClickMessage&&m_pItemListBox->GetSelectListIndex()!=-1&&m_pItemListBox->GetSelectListIndex()<m_pItemListBox->ListNodeMax())
	{
		if(m_saveSelectIndexItem!=m_pItemListBox->GetSelectListIndex()) // Clicking the same item does not display it dynamically.
		{
			Set_UIEffect_Frame_Zero();
			m_saveSelectIndexItem = m_pItemListBox->GetSelectListIndex();
			Dynamic_DescItem_Insert(m_saveSelectIndexItem);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// KeyboardControl
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::KeyboardControl(UINT message, UINT key, long extra)
{
	// Use the scan code in case the IME is toggled.

	if (message == WM_KEYDOWN)
	{
		if (!gC_vs_ui.IsAccelMode() && !gC_vs_ui.IsInputHotkey() && key==VK_ESCAPE)
		{
			Run(EID_CLOSE);
		}
	}
}

//-----------------------------------------------------------------------------
// Show
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Show()
{
	CT_Done("WnSh: C_VS_UI_PREMIUM_GIVE_ITEM::Show ENTER pthis=%p", (void*)this);
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		if(GetAttributes()->alpha)
		{
			m_pC_main_spk->BltLockedAlpha(x,y,0,g_pUserOption->ALPHA_DEPTH);
		}
		else
		{
			m_pC_main_spk->BltLocked(x,y,0);
		}
		gpC_base->m_p_DDSurface_back->Unlock();
	}

	RECT Rc;
	SetRect(&Rc,x+m_rcShowDesc.left,y+m_rcShowDesc.top,x+m_rcShowDesc.right,y+m_rcShowDesc.bottom);
	gpC_base->m_p_DDSurface_back->FillRect(&Rc,0);

	// 아이템 이미지 출력
	for(int i = 0; i < m_Give_Item_List.size(); i++)
	{
		int _Class = m_Give_Item_List[i]->GetItem()->GetItemClass();
		int _Type = m_Give_Item_List[i]->GetItem()->GetItemType();
		if((*g_pItemTable).GetSize()<=_Class)	{ _Class = 0; }
		if((*g_pItemTable)[_Class].GetSize()<=_Type)	{ _Type = 0; }

		const char *pAccelName = (*g_pItemTable)[_Class][_Type].HName.GetString();
		const int n_InventoryFrameID = (*g_pItemTable)[_Class][_Type].InventoryFrameID;
		
		// 리미티드 아이템색상
		if(m_Give_Item_List[i]->GetItem()->IsTimeLimitStatic())
		{
			COLORREF temp_color = CIndexSprite::ColorSet[m_Give_Item_List[i]->GetItem()->GetSpecialColorItemColorset()][16];
			COLORREF name_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
			m_pItemListBox->SetListStringColor(i,name_color);
		}

		if(m_saveSelectIndexItem==i)
		{
			bool isDynamicEnd = true;	// (true == Dynamic Show)
			
#ifdef _DEBUG
			//isDynamicEnd = false;		// It is recommended to set this to false during debugging.
#endif //_DEBUG

			if(isDynamicEnd)
				isDynamicEnd = Dynamic_DescItem_Show();
			else //if(IsDescSpkList[i])
				ShowDesc(m_rcShowDesc.left,m_rcShowDesc.top,m_saveSelectIndexItem,0,0,m_rcShowDesc.right-m_rcShowDesc.left,m_rcShowDesc.bottom-m_rcShowDesc.top,32);

			POINT eff_pt;
			eff_pt.x = x+50;
			eff_pt.y = y+140;
			
			// If there is no image information -> Use the inventory image
			/*
			if(!IsDescSpkList[i])
			{
				gpC_item->BltLocked(x+54+((110-gpC_item->GetWidth(n_InventoryFrameID))/2), y+60+((110-gpC_item->GetHeight(n_InventoryFrameID))/2), n_InventoryFrameID);
			}
			//*/

			if(isDynamicEnd)
			{
				DrawUIEffect(&eff_pt,(int)EFFECTSPRITETYPE_GRAND_MASTER_ALPHA_SLAYER_150,0,3,true);
			}
		}
	}
	
	Rc.left += 3;
	Rc.top += 3;
	Rc.right -= 3;
	Rc.bottom -= 3;

//	gpC_base->m_p_DDSurface_back->DrawRect(&Rc,0xffff);

	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		m_pC_button_group->Show();
		gpC_base->m_p_DDSurface_back->Unlock();
	}
	
	SHOW_WINDOW_ATTR;
}

//-----------------------------------------------------------------------------
// ShowOption - Not currently in use
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::ShowOption(MItem *p_item, int x, int y)
{
	char sz_buf[50];
	
	int vx;
	int px = x, py = y;

	enum 
	{
		SMALL_FONT_Y_GAP = 16,
	};
	
	BOOL fOptionCheck=FALSE;
	if(!p_item->IsEmptyItemOptionList()&& p_item->GetItemClass() != ITEM_CLASS_CODE_SHEET )
	{
		const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemOptionList();
		std::list<TYPE_ITEM_OPTION>::const_iterator itr=optionList.begin();				
				
		while(itr != optionList.end() && *itr < g_pItemOptionTable->GetSize())
		{
			ITEMOPTION_INFO& optionInfo=(*g_pItemOptionTable)[*itr];
			char pPartName[20];
			strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
			if(strstr(pPartName,"MP") != NULL)
			{
				if(g_eRaceInterface == RACE_VAMPIRE)
					*strstr(pPartName,"MP") = 'H';
				else if(g_eRaceInterface == RACE_OUSTERS)
					*strstr(pPartName,"MP") = 'E';
			}
			
			int PlusPoint = optionInfo.PlusPoint; // Applicable figures
			
			if (pPartName)
			{
				if(!fOptionCheck)
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, RGB(192, 192, 255));
					fOptionCheck=TRUE;
				}
				
				if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
				{
					sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
					strcat(sz_buf, "%");
				}
				else
					if(
						optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
						optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
					{
						sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
						strcat(sz_buf, "%");
					}
					else
						sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GOLD);//RGB_YELLOW);
			}
			py += SMALL_FONT_Y_GAP;
			itr++;
		}
	}
}

//-----------------------------------------------------------------------------
// ShowLimit - Not currently in use
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::ShowLimit(MPremiumGiveItemInfo *p_GiveItem, int x, int y)
{
	int vx;
	int strX = x, strY = y;

	if(p_GiveItem->GetTimeLimit() > 0)
	{
		bool bContinue = false;		
		char temp[256];
		vx = g_PrintColorStr(strX, strY, (*g_pGameStringTable)[UI_STRING_MESSAGE_LEFT_TIME].GetString(), gpC_base->m_item_desc_pi, RGB(192, 192, 255) );
		memset( temp, 0, sizeof( temp ) );
		
		DWORD TimeLimit = p_GiveItem->GetTimeLimit();
		
		int days = TimeLimit / 60 / 60 / 24;
		int hours = (TimeLimit / 60 / 60) % 24;
		int minutes = (TimeLimit / 60 ) % 60;
		int seconds = TimeLimit % 60;
		
		
		if(days > 0|| bContinue )
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_DAY].GetString() , days );
			strcat ( temp, tempDay );				
		}
		if(hours > 0|| bContinue)
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_HOUR].GetString() , hours );
			strcat ( temp, tempDay );				
		}
		if(minutes || bContinue)
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_MINUTE].GetString() , minutes );
			strcat ( temp, tempDay );				
		}
		if(seconds)
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_SECOND].GetString() , seconds );
			strcat ( temp, tempDay );				
		}
		g_PrintColorStr(vx,strY, temp,gpC_base->m_item_desc_pi,RGB(255,255,255));
	}
}

//-----------------------------------------------------------------------------
// ShowDesc
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::ShowDesc(int desc_x, int desc_y, int desc_item_num, int src_x, int src_y, int src_w, int src_h, int src_alpha)
{
	return;
/*	if(desc_item_num>=(*m_Give_Item_Spk).GetSize())
		return;

	if(src_x<0)
		src_x = 0;

	if(src_y<0)
		src_y = 0;

	// 출력 영역주기
	if( desc_x>=m_rcShowDesc.right||desc_x<m_rcShowDesc.left||desc_y>=m_rcShowDesc.bottom||desc_y<m_rcShowDesc.top ) { return; }
	
	desc_x-=src_x;
	desc_y-=src_y;

	int src_width = (*m_Give_Item_Spk)[desc_item_num].GetWidth()-src_x;
	int src_height = (*m_Give_Item_Spk)[desc_item_num].GetHeight()-src_y;

	if(src_width<0)
	{
		if(src_w != -10000) // -10000이면 디폴트 크기로
			src_width = 0;
	}
	if(src_height<0)
	{
		if(src_h != -10000) // -10000이면 디폴트 크기로
			src_height = 0;
	}
	if(src_width>src_w)
	{
		src_width = src_w;
	}
	if(src_height>src_h)
	{
		src_height = src_h;
	}

	Rect rect(src_x, src_y, src_width, src_height);
	
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		S_SURFACEINFO surface_info;
		SetSurfaceInfo(&surface_info, gpC_base->m_p_DDSurface_back->GetDDSD());
		
		RECT rt;
		rt.left = max(-(desc_x+x), rect.x);
		rt.top = max(-(desc_y+y), rect.y);
		rt.right = min(rect.x+rect.w, (g_pUserInformation->iResolution_x-desc_x)-x);
		rt.bottom = min(rect.y+rect.h, (g_pUserInformation->iResolution_y-desc_y)-y);

		if(rt.left < rt.right && rt.top < rt.bottom)
		{
			WORD * p_dest = (WORD *)surface_info.p_surface+desc_x+x+rt.left;
			p_dest = (WORD *)((BYTE *)p_dest+((desc_y+y)+rt.top)*surface_info.pitch);

			// 이미지정보가 없는것은 -> 검은색 선물상자로
			if(!IsDescSpkList[desc_item_num])
				desc_item_num = m_Give_Item_List.size();

			if(m_bDescOver)
			{
				int src_color = g_CurrentFrame%28;
			
				if(src_color > 14)
				{ src_color = 28-src_color; }

				src_color += 210;

				(*m_Give_Item_Spk)[desc_item_num].BltColorSetClipWidth(p_dest, surface_info.pitch, &rt, src_color);
			}
			else
			{
				(*m_Give_Item_Spk)[desc_item_num].BltAlphaClipWidth(p_dest, surface_info.pitch, &rt, src_alpha);
			}
		}
		gpC_base->m_p_DDSurface_back->Unlock();
	}	*/
}

//-----------------------------------------------------------------------------
// Dynamic_DescItem_Insert
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Dynamic_DescItem_Insert(int _Select_Item)
{
	DYNAMIC_DESCITEM_MAP_INFO *dynamic_descitem = new DYNAMIC_DESCITEM_MAP_INFO(m_rcShowDesc.right-m_rcShowDesc.left,m_rcShowDesc.bottom-m_rcShowDesc.top);

	dynamic_descitem->set_item_id(_Select_Item);
	dynamic_descitem->SetOriginal(m_rcShowDesc.left,m_rcShowDesc.top,32.0f);
	dynamic_descitem->SetStartingPoint(m_rcShowDesc.left,m_rcShowDesc.top,m_id_DynamicDesc_Process);
	dynamic_descitem->SetSpeed(((float)m_Dynamic_DescItem_List.size()*0.06f)+dynamic_descitem->get_speed());

	m_Dynamic_DescItem_List.push_back(dynamic_descitem);
}

//-----------------------------------------------------------------------------
// Dynamic_DescItem_Process
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Dynamic_DescItem_Process()
{
	if(m_Dynamic_DescItem_List.size()==0)
		return;

	std::vector<DYNAMIC_DESCITEM_MAP_INFO *>::iterator itt_dynamic_descitem = m_Dynamic_DescItem_List.begin();

	int _count = 0;

	while(itt_dynamic_descitem!=m_Dynamic_DescItem_List.end())
	{
		if(_count==0||_count==1)
		{
			(*itt_dynamic_descitem)->Process(m_id_DynamicDesc_Process);
		}
		
		_count++;
		itt_dynamic_descitem++;
	}
	
	if(m_Dynamic_DescItem_List.size()>=2)
	{
		itt_dynamic_descitem = m_Dynamic_DescItem_List.begin();
		
		if((*itt_dynamic_descitem)->Is_end())
		{
			DYNAMIC_DESCITEM_MAP_INFO* p_dynamic_descitem = (*itt_dynamic_descitem);
	
			if((*itt_dynamic_descitem)->IsSet_EndingPoint())
			{
				m_Dynamic_DescItem_List.erase(itt_dynamic_descitem);
				delete p_dynamic_descitem;
				p_dynamic_descitem = NULL;
			}

			if(m_Dynamic_DescItem_List.size()>1)
			{
				(*itt_dynamic_descitem)->SetEndingPoint(m_rcShowDesc.left,m_rcShowDesc.top,m_id_DynamicDesc_Process);
				(*itt_dynamic_descitem)->SetSpeed(((float)m_Dynamic_DescItem_List.size()*0.06f)+(*itt_dynamic_descitem)->get_speed());
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Dynamic_DescItem_Show
//-----------------------------------------------------------------------------
bool C_VS_UI_PREMIUM_GIVE_ITEM::Dynamic_DescItem_Show()
{
	bool is_end = true;

	if(m_Dynamic_DescItem_List.size()==0)
		return is_end;

	std::vector<DYNAMIC_DESCITEM_MAP_INFO *>::iterator itt_dynamic_descitem = m_Dynamic_DescItem_List.begin();

	while(itt_dynamic_descitem!=m_Dynamic_DescItem_List.end())
	{
		for(int i=0; i<(*itt_dynamic_descitem)->get_h(); i++)
		for(int j=0; j<(*itt_dynamic_descitem)->get_w(); j++)
		{
			ShowDesc((*itt_dynamic_descitem)->ImageMap[i][j].get_x(),(*itt_dynamic_descitem)->ImageMap[i][j].get_y(),(*itt_dynamic_descitem)->get_item_id(),j,i,1,1,(*itt_dynamic_descitem)->ImageMap[i][j].get_color());

			if(!(*itt_dynamic_descitem)->ImageMap[i][j].Is_end())
			{
				is_end = false;
			}
		}
		itt_dynamic_descitem++;
	}

	return is_end;
}

//-----------------------------------------------------------------------------
// Set_DynamicDesc_Process
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Set_DynamicDesc_Process(int _DynamicDesc_Process)
{
	std::vector<DYNAMIC_DESCITEM_MAP_INFO *>::iterator itt_dynamic_descitem = m_Dynamic_DescItem_List.begin();

	if(m_Dynamic_DescItem_List.size()==1&&itt_dynamic_descitem!=m_Dynamic_DescItem_List.end()&&(*itt_dynamic_descitem)->Is_end())
	{
		m_id_DynamicDesc_Process = _DynamicDesc_Process;

		// If there are more than the implemented processes
		if(m_id_DynamicDesc_Process>=4)
			m_id_DynamicDesc_Process = 0;
	}

	Dynamic_DescItem_Insert(m_saveSelectIndexItem);
}

//-----------------------------------------------------------------------------
// AcquireFirstSequence
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::AcquireFirstSequence()
{
}

//-----------------------------------------------------------------------------
// UnacquireMouseFocus
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::UnacquireMouseFocus()
{
	m_pC_button_group->UnacquireMouseFocus();
}

//-----------------------------------------------------------------------------
// WindowEventReceiver
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::WindowEventReceiver(id_t event)
{
}

//-----------------------------------------------------------------------------
// IsPixel
//-----------------------------------------------------------------------------
bool C_VS_UI_PREMIUM_GIVE_ITEM::IsPixel(int _x, int _y)
{
	if(m_pC_main_spk)
	{
		return m_pC_main_spk->IsPixel(_x-x, _y-y);
	}
	return false;
}

//-----------------------------------------------------------------------------
// Give_Item_List_ClearAll
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Give_Item_List_ClearAll()
{
	m_Give_Item_List.clear();
	m_Give_Item_List.resize(0);

	// Delete m_Dynamic_DescItem_List
	std::vector<DYNAMIC_DESCITEM_MAP_INFO *>::iterator itt_dynamic_descitem = m_Dynamic_DescItem_List.begin();

	while (itt_dynamic_descitem != m_Dynamic_DescItem_List.end())
	{
		DYNAMIC_DESCITEM_MAP_INFO* p_dynamic_descitem = (*itt_dynamic_descitem);

		delete p_dynamic_descitem;
		p_dynamic_descitem = NULL;

		itt_dynamic_descitem++;
	}

	m_Dynamic_DescItem_List.clear();
	m_Dynamic_DescItem_List.resize(0);
	// End Delete m_Dynamic_DescItem_List
}

//-----------------------------------------------------------------------------
// Give_Item_List_Info_Insert
//-----------------------------------------------------------------------------
void C_VS_UI_PREMIUM_GIVE_ITEM::Give_Item_List_Info_Insert(MPremiumGiveItemInfo *pItem)
{
	m_Give_Item_List.push_back(pItem);
}

//-----------------------------------------------------------------------------
// Give_Item_List_Size
//-----------------------------------------------------------------------------
int C_VS_UI_PREMIUM_GIVE_ITEM::Give_Item_List_Size()
{
	return m_Give_Item_List.size();
}

//-----------------------------------------------------------------------------
// ItemImage_Loding
// After calling `Give_Item_List_Info_Insert()` for all items, you must call `ItemImage_Loading`.
//-----------------------------------------------------------------------------
bool C_VS_UI_PREMIUM_GIVE_ITEM::ItemImage_Loding()
{
	CRarFile	m_pack_file;
	m_pack_file.SetRAR(RPK_ITEM, RPK_PASSWORD);
	if(!m_pack_file.IsSet()) return false;

//	m_saveSelectIndexItem = 0;

	DeleteNew(m_Give_Item_Spk);
	DeleteNewArray(IsDescSpkList);
	
	m_Give_Item_Spk = new CSpritePack;
	m_Give_Item_Spk->Init( m_Give_Item_List.size()+1 );

	IsDescSpkList = new BOOL[m_Give_Item_List.size()];

	bool re;
	int i=0;

	for(;i<m_Give_Item_List.size(); i++)
	{
		int Item_FrameID = 0; // Default 0
		char sz_buf1 [256];

		std::string AddedFileName;
		std::string temp_string;

		int _Class = m_Give_Item_List[i]->GetItem()->GetItemClass();
		int _Type = m_Give_Item_List[i]->GetItem()->GetItemType();
		if((*g_pItemTable).GetSize()<=_Class)			{ _Class = 0; }
		if((*g_pItemTable)[_Class].GetSize()<=_Type)	{ _Type = 0; }

		AddedFileName += (*g_pItemTable)[_Class][_Type].EName;
		AddedFileName += ".txt";

		assert(AddedFileName.c_str());

		if(m_pack_file.Open(AddedFileName.c_str()))
		{	
			m_pack_file.GetString(sz_buf1, 256);
			temp_string = sz_buf1;
		}
		else
		{
			IsDescSpkList[i] = false;
		}

		std::string tmemstr = sz_buf1;

		int _fnum = tmemstr.find("%TU");

		if(_fnum != -1) // 튜닝태그
		{
			tmemstr = tmemstr.c_str()+(_fnum+4);
			if(tmemstr.find(".txt")==-1)
				tmemstr += ".txt";

			if(m_pack_file.Open(tmemstr.c_str()))
			{
				m_pack_file.GetString(sz_buf1, 256);
	
				std::string temp_string;
				temp_string = sz_buf1;

				if(sz_buf1[0] == '%')
				{
					Item_FrameID = atoi(temp_string.c_str()+1);
				}
			}

			IsDescSpkList[i] = true;
		}
		else if(temp_string[0] == '%') // Default image tag
		{
			Item_FrameID = atoi(temp_string.c_str()+1);
			IsDescSpkList[i] = true;
		}
		else
		{
			IsDescSpkList[i] = false;
		}

		re = m_Give_Item_Spk->LoadFromFileData( i, Item_FrameID, SPK_ITEM, SPKI_ITEM );
		assert(re);
	}

	// If there is no image information, it will appear as a black gift boxs
	re = m_Give_Item_Spk->LoadFromFileData( i, 811, SPK_ITEM, SPKI_ITEM );
	assert(re);

	m_pack_file.Release();
	return true;
}
#endif //__PREMIUM_GIVE_ITEM_UI
