#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "VS_UI.h"

#include "VS_UI_filepath.h"
#include "VS_UI_PetMixingForge.h"
#include "MGameStringTable.h"
#include "UserOption.h"
#include "MItemOptionTable.h"

#include "MPlayer.h"
#include "MFakeCreature.h"


#if __CONTENTS(__PET_MIXINGFORGE)
C_VS_UI_PET_MIXING_FORGE::C_VS_UI_PET_MIXING_FORGE()
{	

	m_pC_back_spk = NULL;
	m_pC_back_spk = new C_SPRITE_PACK(SPK_PET_MIXING_FORGE);

	Set( 100, 150, m_pC_back_spk->GetWidth(MAIN_WINDOW), m_pC_back_spk->GetHeight(MAIN_WINDOW));

	g_RegisterWindow(this);

	int w_x, w_y;	


	switch( g_eRaceInterface )
	{
	case RACE_VAMPIRE :
		m_pC_inventory_spk = new C_SPRITE_PACK(SPK_VAMPIRE_INVENTORY);
		w_x = 350;
		w_y = 150;		
		m_my_grid_rect.Set(17+w_x, 150+19, C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X*C_VS_UI_INVENTORY::GRID_X, C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y*C_VS_UI_INVENTORY::GRID_Y);
		break;
	case RACE_SLAYER :
		m_pC_inventory_spk = new C_SPRITE_PACK(SPK_SLAYER_INVENTORY);
		w_x = 350;
		w_y = 150;		
		m_my_grid_rect.Set(13+w_x, 150+26, C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X*C_VS_UI_INVENTORY::GRID_X, C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y*C_VS_UI_INVENTORY::GRID_Y);		
		break;
	case RACE_OUSTERS :
		m_pC_inventory_spk = new C_SPRITE_PACK(SPK_OUSTERS_INVENTORY);
		w_x = 350;
		w_y = 150;		
		m_my_grid_rect.Set(25+w_x, 150+35, C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X*C_VS_UI_INVENTORY::GRID_X, C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y*C_VS_UI_INVENTORY::GRID_Y);
		break;
	}

	AttrTopmost(true);
	//AttrKeyboardControl(true);
	
	//공통버튼
	m_pC_button_group = new ButtonGroup(this);
	
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(16, 120, m_pC_back_spk->GetWidth(C_VS_UI_PET_MIXING_FORGE::BUTTON_OK), 
		m_pC_back_spk->GetHeight(BUTTON_OK), OK_ID, this, BUTTON_OK));
	
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(114, 120, m_pC_back_spk->GetWidth(C_VS_UI_PET_MIXING_FORGE::BUTTON_CANCEL),
		m_pC_back_spk->GetHeight(BUTTON_CANCEL), CANCEL_ID, this, BUTTON_CANCEL));

	m_p_select_item[0] = NULL;
	m_p_select_item[1] = NULL;
	
}

//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE::~C_VS_UI_PET_MIXING_FORGE
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_PET_MIXING_FORGE::~C_VS_UI_PET_MIXING_FORGE()
{
	g_UnregisterWindow( this );

	DeleteNew(m_pC_back_spk);
	DeleteNew(m_pC_button_group);
	DeleteNew(m_pC_inventory_spk);		
}

//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE::ShowButtonWidget
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button)
{
	static char* m_trace_window_button_string[2] = 
	{
		(*g_pGameStringTable)[UI_STRING_MESSAGE_PETMIXING_FORGE_OK].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_PETMIXING_FORGE_CANCEL].GetString(),
	};
	
	switch(p_button->GetID())
	{
	case CANCEL_ID :
		g_descriptor_manager.Set(DID_INFO,p_button->x+x,p_button->y+y,(void *)m_trace_window_button_string[1],0,0);
		break;
	case OK_ID :
		if( m_p_select_item[0] != NULL && m_p_select_item[1] != NULL )
		{
			g_descriptor_manager.Set(DID_INFO,p_button->x+x,p_button->y+y,(void *)m_trace_window_button_string[0],0,0);
		}
		break;		
	}		
}


//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE_WINDOW::ShowButtonWidget
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	if(p_button->m_image_index == BUTTON_OK )
	{
		if( m_p_select_item[0] != NULL && m_p_select_item[1] != NULL )
		{
			if(p_button->GetFocusState())
			{
				if(p_button->GetPressState())
					m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index+2);
				else
					m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index+1);
			} else
				m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index);
		}
		else
		{
			m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index+3);
		}
	}
	else
	{
		if(p_button->GetFocusState())
		{
			if(p_button->GetPressState())
				m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index+2);
			else
				m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index+1);
		} else
			m_pC_back_spk->Blt(x+p_button->x,y+p_button->y,p_button->m_image_index);
	}
}

//-----------------------------------------------------------------------------
// WindowEventReceiver
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::WindowEventReceiver(id_t event)
{
}


//-----------------------------------------------------------------------------
// Run
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::Run(id_t id)
{
	switch (id)
	{
	case OK_ID:
		{
			if( m_p_select_item[0] != NULL && m_p_select_item[1] != NULL )
				gpC_base->SendMessage(UI_USE_PET_MIXING_FORGE, (int)m_p_select_item[0]->GetID(), (int)m_p_select_item[1]->GetID(), NULL);
		}		
		break;
		
	case CANCEL_ID:
		gpC_base->SendMessage(UI_CLOSE_PET_MIXING_FORGE);
		//EMPTY_MOVE;
		break;
	}
}

//-----------------------------------------------------------------------------
// MouseControl
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_PET_MIXING_FORGE::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	_x-=x; _y-=y;	
	bool re = m_pC_button_group->MouseControl(message, _x, _y);
	
	const MItem * p_selected_item;

	switch(message)
	{
	case M_MOVING:
		if(m_p_select_item[0] != NULL && _x > 30 && _x < m_p_select_item[0]->GetGridWidth()*30+30 && _y > 27 && _y < m_p_select_item[0]->GetGridHeight()*30+27 )
		{
			if(m_p_select_item[0] != NULL)
				g_descriptor_manager.Set(DID_ITEM, x+_x, y+_y, (void *)m_p_select_item[0]);
				
		} else
		if( m_p_select_item[1] != NULL && _x > 107 && _x < m_p_select_item[1]->GetGridWidth() * 30 + 107&& _y > 27 && _y < m_p_select_item[1]->GetGridHeight()*30+27)
		{
			if(m_p_select_item[1] != NULL)
				g_descriptor_manager.Set(DID_ITEM, x+_x, y+_y, (void *)m_p_select_item[1]);
		}

		int i;
		int loop;
		int px, py;		

		if (gpC_mouse_pointer->GetPickUpItem())
			loop = ITEM_REF_POINT_COUNT;
		else
		{
			loop = 1;
		}

		for (int i=0; i < loop; i++)
		{
			if (loop == ITEM_REF_POINT_COUNT)
			{
				px = g_item_ref_point[i].x+gpC_mouse_pointer->GetPointerX();
				py = g_item_ref_point[i].y+gpC_mouse_pointer->GetPointerY();
			}
			else
			{
				px = _x+x;
				py = _y+y;
			}

			// search grid
			int distance_x = px - m_my_grid_rect.x;
			int distance_y = py - m_my_grid_rect.y;

			if (distance_x >= 0 && distance_x < m_my_grid_rect.w && 
				 distance_y >= 0 && distance_y < m_my_grid_rect.h)
			{
				if(gpC_Imm && m_focus_grid_x != distance_x/C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X || m_focus_grid_y != distance_y/C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y)
					gpC_Imm->ForceUI(CImm::FORCE_UI_GRID);
				m_focus_grid_x = distance_x/C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X;
				m_focus_grid_y = distance_y/C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y;

				if (loop == ITEM_REF_POINT_COUNT)
				{
					// item이 grid 영역에 어느정도 들어오면 안으로 위치시킨다.
					const MItem * p_pickup_item = gpC_mouse_pointer->GetPickUpItem();
					int a, b;
					switch (i)
					{
						case 0: // left up - first!
							a = m_focus_grid_x+p_pickup_item->GetGridWidth()-C_VS_UI_INVENTORY::GRID_X;
							b = m_focus_grid_y+p_pickup_item->GetGridHeight()-C_VS_UI_INVENTORY::GRID_Y;
							if (a > 0)
								m_focus_grid_x -= a;
							if (b > 0)
								m_focus_grid_y -= b;
							break;

						case 1: // right up
							m_focus_grid_x = 0;
							b = m_focus_grid_y+p_pickup_item->GetGridHeight()-C_VS_UI_INVENTORY::GRID_Y;
							if (b > 0)
								m_focus_grid_y -= b;
							break;

						case 2: // left down
							m_focus_grid_y = 0;
							a = m_focus_grid_x+p_pickup_item->GetGridWidth()-C_VS_UI_INVENTORY::GRID_X;
							if (a > 0)
								m_focus_grid_x -= a;
							break;

						case 3: // right down
							m_focus_grid_y = 0;
							if (m_focus_grid_x+1 <= p_pickup_item->GetGridHeight())
								m_focus_grid_x = 0;
					}
				}

				p_selected_item = g_pInventory->GetItem(m_focus_grid_x, m_focus_grid_y);				
				if (p_selected_item != NULL)
						g_descriptor_manager.Set(DID_ITEM, GetFocusedItemGridX(p_selected_item), GetFocusedItemGridY(p_selected_item), (void *)p_selected_item);

				return true;
			}			
		}
		if(gpC_Imm && m_focus_grid_x != NOT_SELECTED || m_focus_grid_y != NOT_SELECTED)
			gpC_Imm->ForceUI(CImm::FORCE_UI_GRID);
		m_focus_grid_x = NOT_SELECTED;
		m_focus_grid_y = NOT_SELECTED;
		break;
		
	case M_LEFTBUTTON_DOWN:
	case M_LB_DOUBLECLICK:		
		break;

	case M_RIGHTBUTTON_DOWN:
		{
			bool ret = Check(m_my_grid_rect.x, m_my_grid_rect.y);

			if (ret)
				EMPTY_MOVE;
		}
		break;
	}
	return true;
}

//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::KeyboardControl(UINT message, UINT key, long extra)
{
	if(!(message == WM_KEYDOWN && key == VK_RETURN ))
		Window::KeyboardControl(message, key, extra);

	if (message == WM_KEYDOWN)
	{
		switch (key)
		{
		case VK_ESCAPE :
			gpC_base->SendMessage(UI_CLOSE_PET_MIXING_FORGE);
			break;		
		}
	}
}

//-----------------------------------------------------------------------------
// Show
//
//
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::Show()
{
	CT_Done("WnSh: C_VS_UI_PET_MIXING_FORGE::Show ENTER");
	m_pC_back_spk->Blt(x,y,MAIN_WINDOW);
	m_pC_button_group->Show();

	m_pC_button_group->ShowDescription();
	SHOW_WINDOW_ATTR;

	//글자 찍을것들을 기억해뒀다가 한꺼번에 찍는다.
	std::vector<RECT>	vNumRect;
	std::vector<int>	vNum;
	
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		switch (g_eRaceInterface)
		{
		case RACE_OUSTERS :
			m_pC_inventory_spk->BltLocked(350, 150, INVENTORY_WINDOW);
			break;
		case RACE_VAMPIRE :
			m_pC_inventory_spk->BltLocked(350, 150, INVENTORY_WINDOW);			
			m_pC_inventory_spk->BltLocked(355, 150+m_pC_inventory_spk->GetHeight(INVENTORY_WINDOW), INVENTORY_WINDOW_BOTTOM);
			break;
		case RACE_SLAYER :
			m_pC_inventory_spk->BltLocked(350, 150, INVENTORY_WINDOW);
			break;
		}
		
		//----------------------------------------------------------------		
		// show My trade Item		
		//----------------------------------------------------------------
		g_pInventory->SetBegin();

		const MItem * p_selected_item = g_pInventory->GetItem(m_focus_grid_x, m_focus_grid_y);
		int   check_index = 0;

		while (g_pInventory->IsNotEnd())
		{
			const MItem * p_item = g_pInventory->Get();

			// p_item은 NULL이 반드시 아니다. 왜냐하면 존재하는 것만 Get()하기 때문이다.
			assert(p_item);

			// frame id -> sprite id
			TYPE_FRAMEID frame_id = p_item->GetInventoryFrameID();

			int item_x = GetFocusedItemGridX(p_item);
			int item_y = GetFocusedItemGridY(p_item);

			int print_y = item_y + p_item->GetGridHeight()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y - 12;
			int print_x = item_x + p_item->GetGridWidth()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X-1;

			// Item이 놓여있는 영역 표시
			for (int j = 0; j < p_item->GetGridHeight(); j++)
				for (int i = 0; i < p_item->GetGridWidth(); i++)
				{
					//------------------------------------------------------------
					// 교환할려는 아이템인 경우
					//------------------------------------------------------------
					if( m_v_check[check_index] )//if (p_item->IsTrade())
					{
						m_pC_inventory_spk->BltLocked(item_x+C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X*i, item_y+C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y*j, INVENTORY_ITEMBACK);
					}					
				}

			item_x += (p_item->GetGridWidth()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X)/2-gpC_item->GetWidth(frame_id)/2;
			item_y += (p_item->GetGridHeight()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y)/2-gpC_item->GetHeight(frame_id)/2;
			
			if (p_selected_item && (p_selected_item->GetID() == p_item->GetID()))
			{
				if(p_item->IsQuestItem())
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetQuestItemColorset(), 0);
				else
				if(p_item->IsUniqueItem())
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetUniqueItemColorset(), 0);
				else				
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetItemOptionColorSet(), 0);
				gpC_item->BltLockedOutline(item_x, item_y, WHITE, frame_id);
			}
			else
			{
				if(p_item->IsQuestItem() )
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetQuestItemColorset(), 0);
				else					
				if(p_item->IsUniqueItem())
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetUniqueItemColorset(), 0);
				else				
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetItemOptionColorSet(), 0);

				//------------------------------------------------------------
				// 교환할려는 아이템인 경우
				//------------------------------------------------------------
				if( m_v_check[check_index] )
				{
					if (p_item->IsAffectStatus())
					{
						// frame id -> sprite id
						gpC_item->BltLocked(item_x, item_y, frame_id);
					}
					else
					{
						gpC_item->BltLockedColorSet(item_x, item_y, frame_id, ITEM_DISABLE_COLOR_SET);
					}
				}
				//------------------------------------------------------------
				// 교환할려는 아이템이 아닌 경우
				//------------------------------------------------------------
				else
				{
					gpC_item->BltLockedDarkness(item_x, item_y, frame_id, 1 );
				}
			}

			//아이템 개수 찍을 alphabox 숫자는 찍지 않는다 저장해 뒀다가 나중에 몰아서 한꺼번에 찍는다
			if(p_item->IsPileItem() || p_item->IsChargeItem())
			{
				RECT rt;
				rt.right = print_x;
				int depth = 0, number;
				for(  depth = 0, number = p_item->GetNumber(); number > 0; number/=10, depth++);
				if(depth == 0) depth = 1;
				rt.left = print_x - 7*depth;
				rt.top = print_y;
				rt.bottom = rt.top+12;
				
				rt.left = max(0, rt.left);
				rt.top = max(0, rt.top);
				
				if(rt.left < rt.right && rt.top < rt.bottom)DrawAlphaBox(&rt, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);

				vNumRect.push_back(rt);
				vNum.push_back(p_item->GetNumber());
				
			}
			g_pInventory->Next();
			check_index++;
		}		

		for(int i=0; i<2; i++)
		{
			if( m_p_select_item[i] != NULL )
			{
				if(m_p_select_item[i]->IsQuestItem() )
					CIndexSprite::SetUsingColorSet(m_p_select_item[i]->GetQuestItemColorset(), 0);
				else
				if(m_p_select_item[i]->IsUniqueItem())
					CIndexSprite::SetUsingColorSet(m_p_select_item[i]->GetUniqueItemColorset(), 0);
				else				
					CIndexSprite::SetUsingColorSet(m_p_select_item[i]->GetItemOptionColorSet(), 0);

				TYPE_FRAMEID frame_id = m_p_select_item[i]->GetInventoryFrameID();
				gpC_item->BltLocked( 100+18+(i*98) + (m_p_select_item[i]->GetGridWidth()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X)/2-gpC_item->GetWidth(frame_id)/2, 
									 150+41+ (m_p_select_item[i]->GetGridHeight()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y)/2-gpC_item->GetHeight(frame_id)/2, 
									 frame_id);
			}			
		}
		
		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	m_pC_button_group->ShowDescription();

	char sz_num[10];
	COLORREF markColor = RGB(220, 220, 220);//RGB(140, 140, 255);
	for(int i = 0; i < vNum.size(); i++)
	{
		wsprintf(sz_num, "%d", vNum[i]);
		g_PrintColorStr(vNumRect[i].left, vNumRect[i].top, sz_num, gpC_base->m_item_desc_pi, markColor);
	}
	g_FL2_ReleaseDC();

	SHOW_WINDOW_ATTR;
}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::Start()
{
	PI_Processor::Start();
	m_pC_button_group->Init();

	AttrPin(true);
	gpC_window_manager->AppearWindow(this);

	assert( g_pInventory != NULL );
	g_pInventory->SetBegin();
	while( g_pInventory->IsNotEnd() )
	{
		m_v_check.push_back(0);
		g_pInventory->Next();
	}	
	
	m_focus_grid_x = NOT_SELECTED;
	m_focus_grid_y = NOT_SELECTED;
}

void	C_VS_UI_PET_MIXING_FORGE::Finish()
{
	PI_Processor::Finish();	
	gpC_window_manager->DisappearWindow(this); 		
}

//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_PET_MIXING_FORGE::Process()
{

}

//-----------------------------------------------------------------------------
// IsPixel
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_PET_MIXING_FORGE::IsPixel(int _x, int _y)
{
	return true;
}

//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE::Check
//
// 교환할려는 item을 체크한다. 
// 이미 체크되어있다면.. 취소한다.
//
// grid_start_x, grid_start_y는 igrid 시작점이다. 이것을 참조하여 item
// (x, y)를 구한다.
//-----------------------------------------------------------------------------
bool C_VS_UI_PET_MIXING_FORGE::Check(int grid_start_x, int grid_start_y)
{
	MItem * p_item = g_pInventory->GetItem(m_focus_grid_x, m_focus_grid_y);
	MItem * p_item2;
	bool  b_check = false;
	if( p_item != NULL)
	{	
#ifdef _LIB
		MPetItem *pPetItem = (MPetItem *)p_item;
		int msg = -1;
		int petLevel = p_item->GetNumber();
		
		//까놓구 말해서 펫 아이템이 소환 됐는지 안됬는지 알수 있는 방법이 이것뿐이다.
		MFakeCreature *pFakeCreature = NULL;
		pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(g_pPlayer->GetPetID());
		MPetItem *pSummonPetItem = NULL;
		if(pFakeCreature) 
		{
			pSummonPetItem = pFakeCreature->GetPetItem();
		}
		
		
		if(p_item->GetItemClass() != ITEM_CLASS_PET_ITEM)
			msg = UI_STRING_MESSAGE_PETMIXING_NOT_PETITEM;
		//여기에 조합 여부 검사추가 예정 아직 정해지지 않아서....
		else
		if(pPetItem->GetPetMixingOptionListCount())
			msg = UI_STRING_MESSAGE_PETMIXING_DIDMIX;
		else//펫이 죽었다면 안된다.
		if(p_item->GetGrade() != -1)
			msg = UI_STRING_MESSAGE_PETMIXING_DIE;
		else
		if(pSummonPetItem == p_item)
			msg = UI_STRING_MESSAGE_PETMIXING_SUMMONING;
		else//렙 낮음
		if(petLevel < 11)
			msg = UI_STRING_MESSAGE_PETMIXING_LOWLEVEL;
		else//40 ~ 49는 재료가 될수 없다.
		if(petLevel < 50 && petLevel >= 40)
			msg = UI_STRING_MESSAGE_PETMIXING_CANNOTLEVEL;
		/*********************************************************

			TO DO : 추후 옵션 체크가 더 필요 할 땐 여기에 추가
			
		**********************************************************/


		if(msg != -1)
		{
			gpC_base->SendMessage(UI_CLOSE_PET_MIXING_FORGE, msg);
			return false;
		}
		
#endif
		// 20090525 diesirace 여기까지 오면 일단 에러 상황은 없다고 판단한다. 만약 여기서 에러 체크가 안될 상황이 생기면 위쪽에서 에러 처리 해주도록 하자.
		if(petLevel >= 50)
		{
			if(m_p_select_item[0] == NULL)
			{
				m_p_select_item[0] = p_item;
				b_check = true;
			}
			else
			{
				if( p_item == m_p_select_item[0] )
				{
					m_p_select_item[0] = NULL;
					b_check = true;
				}
			}
		}
		else
			if(petLevel >= 11 && petLevel <= 39)
			{
				if(m_p_select_item[1] == NULL)
				{
					m_p_select_item[1] = p_item;
					b_check = true;
				}
				else
				{
					if( p_item == m_p_select_item[1] )
					{
						m_p_select_item[1] = NULL;
						b_check = true;
					}
				}
			}
			
			
			if( b_check )
			{
				int check_index=0;
				g_pInventory->SetBegin();
				while (g_pInventory->IsNotEnd())
				{
					p_item2 = g_pInventory->Get();
					if( p_item == p_item2 )
					{
						m_v_check[check_index] = !m_v_check[check_index];
					}		
					g_pInventory->Next();
					check_index++;
				}
			}	
			
			return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE::GetFocusedItemGridX
//
// 
//-----------------------------------------------------------------------------
int C_VS_UI_PET_MIXING_FORGE::GetFocusedItemGridX(const MItem * p_item)
{
	assert(p_item);

	if (p_item == NULL)
		return 0;

	return m_my_grid_rect.x+C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X*p_item->GetGridX();
}

//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE::GetFocusedItemGridY
//
// 
//-----------------------------------------------------------------------------
int C_VS_UI_PET_MIXING_FORGE::GetFocusedItemGridY(const MItem * p_item)
{
	assert(p_item);

	if (p_item == NULL)
		return 0;

	return m_my_grid_rect.y+C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y*p_item->GetGridY();
}
#endif //__PET_MIXINGFORGE
