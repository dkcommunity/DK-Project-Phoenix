#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "VS_UI_Control.h"


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// class PositionInfo //
//////////////////////////////////////////////////////////////////////
PositionInfo::PositionInfo()
{
	m_info_data = "";
}

PositionInfo::~PositionInfo()
{
	m_rpk_file.Release();
}

bool PositionInfo::ParsingData(const char* pszRpkFile, const char* pszDataFile, const char* pszPassWord /*= RPK_PASSWORD*/)
{
	if (pszRpkFile == "" || pszDataFile == "")
		return false;

	m_rpk_file.SetRAR(pszRpkFile,pszPassWord);
	m_rpk_file.Open(pszDataFile);

	m_info_data = m_rpk_file.Read(m_rpk_file.GetSize());

	return true;
}

bool PositionInfo::GetData(const char* pszKeyWord, int& value)
{
	size_t pos = m_info_data.find(pszKeyWord);

	return true;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// class C_VS_UI_MENU //
//////////////////////////////////////////////////////////////////////

C_VS_UI_MENU::C_VS_UI_MENU(int _x /*= 0*/, int _y /*= 0*/, MENU_VISUAL_STYLE style /*= MVS_MS*/)
{
	// To do : 윈도우가 시작되어 데이터들을 초기화 시킨다.

	g_RegisterWindow(this);

	x = _x;
	y = _y;

	m_menu_style = style;
	m_sub_menu_open = false;
	m_command_handling = false;

	m_command_count = 0;
	m_current_command = -1;

	m_max_string_lengh = 0;
	m_select_box_height = 17;
	m_command_pos_x_gap = 2;
	m_command_pos_y_gap = 3;

	m_current_sub_menu = NULL;

	memset(&m_menu_box,0,sizeof m_menu_box);
	memset(&m_select_box,0,sizeof m_select_box);

	m_v_main_command.clear();
	m_map_sub_menu.clear();

	SetPrintInfo("VS_UI_CONTROL_LN_73",&gpC_base->m_dialog_msg_pi);
}

C_VS_UI_MENU::~C_VS_UI_MENU()
{
	// To do : 윈도우가 종료 될때 메모리 해제등을 실행한다.

	g_UnregisterWindow(this);

	// 서브 메뉴 정리해 주고..
	{
		MAP_SUB_MENU::iterator itr = m_map_sub_menu.begin();
		for (; itr != m_map_sub_menu.end(); ++itr)
		{
			if (itr->second != NULL)
			{
				itr->second->Finish();
				DeleteNew(itr->second);
			}
		}
		m_map_sub_menu.clear();
	}
	// 메인 커맨드도 정리해 준다...모두 동적으로 할당 했기 때문에...
	{
		VEC_MENU_COMMAND::iterator itr = m_v_main_command.begin();
		for (; itr != m_v_main_command.end(); ++itr)
		{
			DeleteNew(*itr);
		}
		m_v_main_command.clear();
	}
}

void C_VS_UI_MENU::Start()
{
	// To do : 윈도우의 프로세스를 가동 시킨다.

	PI_Processor::Start();
	gpC_window_manager->AppearWindow(this);

	EMPTY_MOVE;
}

void C_VS_UI_MENU::Finish()
{
	// To do : 윈도우가 종료 될때 프로세스를 정리한다.

	PI_Processor::Finish();
	gpC_window_manager->DisappearWindow(this);

	// 현재 메뉴가 종료되면 당연히 서브 메뉴도 종료 시켜준다.
	if (m_current_sub_menu != NULL)
	{
		m_current_sub_menu->Finish();
		m_current_sub_menu = NULL;
	}
}

void C_VS_UI_MENU::Show()
{
	CT_Done("WnSh: C_VS_UI_MENU::Show ENTER pthis=%p", (void*)this);
	// To do : 메뉴를 드로잉 한다.

	if (!Running()) return;

	DrawBackGround();
	DrawSelectBox();
	DrawInputCommand();
	
	// 여기다 한번 더 그려주는 이유는 서브 메뉴가 항상 그 상위 메뉴 보다는 위에 그려져야 하기 때문이다..
	// 하지만 역시 두번 그리는 것은..그닥 좋지 않다..
	// 현재 윈도우 매니져 구조상 어쩔수 없는...윈도우 매니져는 건드리지 않도록 한다.
	// 아무래도 예외 처리가 너무 많아지기 때문에..
	if (m_current_sub_menu != NULL) 
	{
		m_current_sub_menu->Show();
	}
}

void C_VS_UI_MENU::Process()
{
	// To do : 업데이트를 처리

	// 마우스가 현재 UI를 벗어나면 현재 위치한 명령어를 알지 못하므로
	// 여기서 초기화 해준다.
	if (!m_sub_menu_open) m_current_command = -1;
}

bool C_VS_UI_MENU::MouseControl(UINT message, int _x, int _y)
{
	// To do : 마우스 이벤트를 처리

	// 프르세스가 종료 되었는데도 계속 해서 메시지가 와서 처리가 되면 안되기 때문에..
	if (!Running()) return false;

	m_current_command = CheckInButton(_x,_y);

	switch (message)
	{
	case M_MOVING:
		GenerateSubMenu(m_current_command);
		return false;
	case M_RIGHTBUTTON_DOWN:
		return true;
	case M_LEFTBUTTON_UP: case M_LB_DOUBLECLICK:
		if (m_current_command == -1 || m_current_command >= m_command_count)
			return true;
		// 선택된 명령어의 처리가 정의 되어 있는 함수 포인터를 작동 시킨다.
		if (m_v_main_command[m_current_command]->style == MENU_COMMAND::CS_SUBMENU)
			return false;
		else if (m_v_main_command[m_current_command]->style == MENU_COMMAND::CS_NONE && !m_command_handling)
		{
			m_command_handling = true;
			LButtonDown(m_current_command);
			return true;
		}
	}
	return false;
}

void C_VS_UI_MENU::KeyboardControl(UINT message, UINT key, long extra)
{
	if (message == WM_KEYDOWN)
	{
		switch (key)
		{
		case VK_ESCAPE:
			gpC_base->SendMessage(UI_DESTROY,0,0,(void*)m_window_name.c_str());
			break;		
		}
	}	
}

void C_VS_UI_MENU::DrawBackGround()
{
	// To do : 메뉴의 배경 이미지를 그린다.

	// 스타일에 따라 메뉴의 배경색 선택박스 문자열 등의 색상을 달리 한다.
	WORD backcolor = GRAY;
	BYTE alpha = 0;
	switch (m_menu_style)
	{
	case MVS_BL:
		backcolor = BLACK;
		alpha = 15;
		break;
	}
	
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// 메뉴 배경
		gpC_base->m_p_DDSurface_back->BltColorAlpha(&m_menu_box,backcolor,alpha);
		gpC_base->m_p_DDSurface_back->Unlock();

		// 외곽선은 그리는 방식도 틀리므로..
		if (m_menu_style == MVS_MS)
		{
			// 외곽선 - 바깥
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left,m_menu_box.top,m_menu_box.right-m_menu_box.left,WHITE);
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left,m_menu_box.top+(m_menu_box.bottom-m_menu_box.top),m_menu_box.right-m_menu_box.left,DARKGRAY);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left,m_menu_box.top,m_menu_box.bottom-m_menu_box.top,WHITE);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left+(m_menu_box.right-m_menu_box.left),m_menu_box.top,m_menu_box.bottom-m_menu_box.top,DARKGRAY);

			// 외곽선 - 안쪽
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left+1,m_menu_box.top+1,m_menu_box.right-m_menu_box.left-2,WHITE);
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left+1,m_menu_box.top+(m_menu_box.bottom-m_menu_box.top)-1,m_menu_box.right-m_menu_box.left-2,DARKGRAY);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left+1,m_menu_box.top+1,m_menu_box.bottom-m_menu_box.top-2,WHITE);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left+(m_menu_box.right-m_menu_box.left)-1,m_menu_box.top+1,m_menu_box.bottom-m_menu_box.top-2,DARKGRAY);
		}
		else if (m_menu_style == MVS_BL)
		{
			// 외곽선 - 바깥
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left+1,m_menu_box.top,m_menu_box.right-m_menu_box.left-2,YELLOW);
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left+1,m_menu_box.top+(m_menu_box.bottom-m_menu_box.top),m_menu_box.right-m_menu_box.left-2,DARKYELLOW);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left,m_menu_box.top+1,m_menu_box.bottom-m_menu_box.top-2,YELLOW);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left+(m_menu_box.right-m_menu_box.left)+1,m_menu_box.top,m_menu_box.bottom-m_menu_box.top-2,DARKYELLOW);

			// 외곽선 - 안쪽
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left+1,m_menu_box.top+1,m_menu_box.right-m_menu_box.left-2,YELLOW);
			gpC_base->m_p_DDSurface_back->HLine(m_menu_box.left+1,m_menu_box.top+(m_menu_box.bottom-m_menu_box.top)-1,m_menu_box.right-m_menu_box.left-2,DARKYELLOW);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left+1,m_menu_box.top+1,m_menu_box.bottom-m_menu_box.top-2,YELLOW);
			gpC_base->m_p_DDSurface_back->VLine(m_menu_box.left+(m_menu_box.right-m_menu_box.left)-1,m_menu_box.top+1,m_menu_box.bottom-m_menu_box.top-2,DARKYELLOW);
		}
	}
}

void C_VS_UI_MENU::DrawSelectBox()
{
	// To do : 선택된 박스의 이미지를 보여준다.

	// 스타일에 따라 메뉴의 배경색 선택박스 문자열 등의 색상을 달리 한다.
	COLORREF selbox = SKY_BLUE;
	switch (m_menu_style)
	{
	case MVS_BL:
		selbox = WHITE;
		break;
	}
	
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// 선택 박스
		if (m_current_command >= 0 && m_current_command < m_command_count) 
		{
			gpC_base->m_p_DDSurface_back->BltColorAlpha(&m_select_box,selbox,0);
		}
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_VS_UI_MENU::DrawInputCommand()
{
	// To do : 입력된 명령어를 보여준다.

	// 스타일에 따라 메뉴의 배경색 선택박스 문자열 등의 색상을 달리 한다.
	COLORREF cmdtext = RGB_BLACK, settext = RGB_WHITE;
	switch (m_menu_style)
	{
	case MVS_BL:
		cmdtext = RGB_WHITE;
		settext = RGB_BLACK;
		break;
	}

	g_FL2_GetDC();

	int x_gap = 10, y_gap = m_command_pos_y_gap + 3, index = 0;
	VEC_MENU_COMMAND::iterator itr = m_v_main_command.begin();
	for (; itr != m_v_main_command.end();)
	{
		// 명령어
		if (index == m_current_command)	
			g_PrintColorStr(x+x_gap,y+y_gap,(*itr)->command.c_str(),*m_menu_print_info,settext);
		else
			g_PrintColorStr(x+x_gap,y+y_gap,(*itr)->command.c_str(),*m_menu_print_info,cmdtext);

		// 선택박스의 높이 만큼씩 그린다.
		y_gap += m_select_box_height;
		++itr;
		++index;
	}

	g_FL2_ReleaseDC();
}

void C_VS_UI_MENU::SetPrintInfo(const char* font, PrintInfo* gInfo)
{
	m_menu_print_info = gInfo;
}

void C_VS_UI_MENU::GenerateSubMenu(int getsubcommand)
{
	// To do : 서브 메뉴 출력을 위한 작업을 진행한다.

	if (m_v_main_command.empty())
		return;

	VEC_MENU_COMMAND::iterator mainCmd = m_v_main_command.begin() + getsubcommand;
	if (mainCmd == m_v_main_command.end())
		return;

	if (getsubcommand != -1 && getsubcommand < m_command_count && (*mainCmd)->style == MENU_COMMAND::CS_SUBMENU)
	{	// 커맨드가 서브 메뉴 출력이라면 해당 커맨드의 서브 메뉴를 찾고 실행 시킨다.
		MAP_SUB_MENU::iterator itr = m_map_sub_menu.find((*mainCmd)->id);
		if (itr != m_map_sub_menu.end())
		{
			if (!itr->second->Running())	// running을 체크 하는건 이미 실행되고 있는 서브 메뉴에 대해 계속 Start 함수가 호출 되는 것을 막기 위한 것이다.
			{
				// 먼저 떠 있던 서브 메뉴는 종료 시키고...
				if (m_current_sub_menu != NULL)
				{
					m_current_sub_menu->Finish();
				}
				// 새로운 서브 메뉴를 실행 시킨다.
				itr->second->SetPosition(m_select_box.right-2,m_select_box.top);
				itr->second->Start();
				m_current_sub_menu = itr->second;
				m_sub_menu_open = true;
			}
		}
	}
	else if (getsubcommand != -1 && getsubcommand < m_command_count)
	{	// 마우스가 다른 명령어를 가리키고 있다면 당연히 현재 떠 있는 서브 메뉴는 종료 시킨다.
		if (m_current_sub_menu != NULL)
		{
			m_current_sub_menu->Finish();
			m_current_sub_menu = NULL;
			m_sub_menu_open = false;
		}
	}
}

bool C_VS_UI_MENU::LButtonDown(int cmd)
{
	if (m_v_main_command[m_current_command]->pExecFn)
		m_v_main_command[m_current_command]->pExecFn(NULL);
	gpC_base->SendMessage(UI_DESTROY,0,0,(void*)GetWindowName().c_str());
	return true;
}

int C_VS_UI_MENU::CheckInButton(int _x, int _y)
{
	// To do : 현재 명령어에 버튼이 위치 하는지 체크

	int index = -1;

	if (m_menu_box.PtInRect(CPoint(_x,_y)))
	{
		// 현재 커맨드의 사각 영역을 구한다.
		int gap = _y - (y + m_command_pos_y_gap);	// m_command_pos_y_gap은 위 아래 공백 개념
		index = gap / m_select_box_height;	// 눌려진 명령어 인덱스
		if (index >= m_command_count) index = m_command_count - 1;
		int sx = x + m_command_pos_x_gap;
		int sy = (y + m_command_pos_y_gap) + (index * m_select_box_height);
		int sw = sx + w - (m_command_pos_x_gap * 2);
		int sh = sy + m_select_box_height;
		m_select_box.SetRect(sx,sy,sw,sh);
	}
	else
	{
		if (m_current_sub_menu != NULL)
		{	// 현재 선택된 커맨드의 서브 메뉴가 출력 되어 있다면 그 커맨드는 마우스 오버 상태가 되어야 한다.
			// MS의 윈도우 팝업 메뉴를 참고하여 작업...
			if (m_current_sub_menu->Running())
			{
				index = m_current_command;
			}
		}
	}
	return index;
}

void C_VS_UI_MENU::SetPosition(int _x, int _y, MENU_POSITION pos /*= MP_RIGHTBOTTOM*/)
{
	// To do : 메뉴의 출력될 위치를 설정한다.

	switch (pos)
	{
	case MP_RIGHTBOTTOM:
		break;
	case MP_RIGHTTOP:
		break;
	case MP_LEFTBOTTOM:
		break;
	case MP_LEFTTOP:
		break;
	}
	x = _x; y = _y; 
	m_menu_box.SetRect(x, y, (x + m_max_string_lengh + 20), (y + (m_command_count * m_select_box_height) + (m_command_pos_y_gap * 2)));
	Set(m_menu_box.left, m_menu_box.top, m_menu_box.right - m_menu_box.left, m_menu_box.bottom - m_menu_box.top);
}

void C_VS_UI_MENU::AddCommand(const char* pszCommand, id_t id, void (*pExecFn)(void* void_ptr) /*= NULL*/, int style /* = MENU_COMMAND::CS_NONE */)
{
	// To do : 커맨드 추가 작업을 수행한다.
	// pszCommand는 명령어 문구
	// id는 명령어의 아이디
	// pExecFn는 해당 명령어의 수행 함수 포인터
	// style은 명령어의 스타일을 지정. ex) style = MENU_COMMAND::CS_SUBMENU일 경우 해당 명령어는 서브 메뉴를 출력한다.

	// 명령어를 추가로 입력 할때 가장 긴 명령어를 기준으로 그려질 메뉴창의 크기를 구한다.
	int len = g_GetStringWidth(pszCommand,m_menu_print_info->hfont);
	m_max_string_lengh = len > m_max_string_lengh ? len : m_max_string_lengh;

	MENU_COMMAND* pMenuCommand = new MENU_COMMAND;
	pMenuCommand->command = pszCommand;
	pMenuCommand->id = id;
	pMenuCommand->style = style;
	pMenuCommand->pExecFn = pExecFn;
	m_v_main_command.push_back(pMenuCommand);
	m_command_count = m_v_main_command.size();

	m_menu_box.SetRect(x, y, (x + m_max_string_lengh + 20), (y + (m_command_count * m_select_box_height) + (m_command_pos_y_gap * 2)));
	Set(m_menu_box.left, m_menu_box.top, m_menu_box.right - m_menu_box.left, m_menu_box.bottom - m_menu_box.top);
}

void C_VS_UI_MENU::SetSubMenu(id_t id, C_VS_UI_MENU*& pSubMenu)
{
	// To do : 서브 명령어의 메뉴를 설정한다.

	m_map_sub_menu[id] = pSubMenu;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// class C_VS_UI_LISTVIEW //
//////////////////////////////////////////////////////////////////////////

bool CompareTextValueAs(LTVITEM* lA, LTVITEM* lB)
{
	// 오름차순 정렬
	if (strcmp(lA->text.find(lA->sort_column_index)->second.c_str(),lB->text.find(lB->sort_column_index)->second.c_str()) < 0)
		return true;
	else if (strcmp(lA->text.find(lA->sort_column_index)->second.c_str(),lB->text.find(lB->sort_column_index)->second.c_str()) == 0)
		return false;
	else return false;
}

bool CompareTextValueDs(LTVITEM* lA, LTVITEM* lB)
{
	// 내림차순 정렬
	if (strcmp(lA->text.find(lA->sort_column_index)->second.c_str(),lB->text.find(lB->sort_column_index)->second.c_str()) < 0)
		return false;
	else if (strcmp(lA->text.find(lA->sort_column_index)->second.c_str(),lB->text.find(lB->sort_column_index)->second.c_str()) == 0)
		return false;
	else return true;
}

C_VS_UI_LISTVIEW::C_VS_UI_LISTVIEW()
{
	// To do : 윈도우가 시작되어 데이터들을 초기화 시킨다.

	m_pParent = NULL;
	m_pColumn = NULL;
	
	m_select_type = ST_SINGLE;
	m_style_flag = LSFBACKGROUNDNOTTRANS;
	m_header_align = CHALEFT | CHATOP;
	m_sort_state = 0;
	m_vertical_scroll = false;
	m_horizon_scroll = false;
	m_generate_scroll = false;
	m_column_count = 0;
	m_click_column = -1;
	m_view_begin_item_index = 0;
	m_item_height = 23;
	m_view_max_height = 1;
	m_text_x_space = 0;
	m_text_y_space = 0;
	m_view_item_count = m_view_max_height / m_item_height - 1;

	m_v_item.clear();
	memset(&m_old_mouse_pos,-1,sizeof m_old_mouse_pos);

	SetPrintInfo("VS_UI_CONTROL_LN_507",gpC_base->m_dialog_msg_pi);

	Set(0,0,0,0);
	m_background.SetRect(0,0,0,0);
	memset(&m_in_pos,0,sizeof m_in_pos);
}

C_VS_UI_LISTVIEW::C_VS_UI_LISTVIEW(Window* pParent, int _x, int _y, int _w, int _h, DWORD style /*= LSFBACKGROUNDNOTTRANS*/, SELECT_TYPE type /*= ST_SINGLE*/)
{
	// To do : 윈도우가 시작되어 데이터들을 초기화 시킨다.
	
	m_pParent = pParent;
	m_pColumn = NULL;
	
	m_select_type = type;
	m_style_flag = style;
	m_header_align = CHALEFT | CHATOP;
	m_sort_state = 0;
	m_vertical_scroll = false;
	m_horizon_scroll = false;
	m_generate_scroll = false;
	m_column_count = 0;
	m_click_column = -1;
	m_view_begin_item_index = 0;
	m_item_height = 23;
	m_view_max_height = _h;
	m_text_x_space = 0;
	m_text_y_space = 0;
	m_view_item_count = _h / m_item_height - 1;

	m_v_item.clear();
	memset(&m_old_mouse_pos,-1,sizeof m_old_mouse_pos);

	SetPrintInfo("VS_UI_CONTROL_LN_540",gpC_base->m_dialog_msg_pi);

	if (m_pParent)
	{
		Set(m_pParent->x+_x,m_pParent->y+_y,_w,_h);
		m_background.SetRect(m_pParent->x+_x,m_pParent->y+_y,m_pParent->x+_x+_w,m_pParent->y+_y+_h);
		m_in_pos.SetRect(_x,_y,_x+_w,_y+h);
	}
	else
	{
		Set(_x,_y,_w,_h);
		m_background.SetRect(_x,_y,_x+_w,_y+_h);
		memset(&m_in_pos,0,sizeof m_in_pos);
	}

	if (!m_pParent)
	{
		g_RegisterWindow(this);
		gpC_window_manager->AppearWindow(this);
	}
}

C_VS_UI_LISTVIEW::~C_VS_UI_LISTVIEW()
{
	// To do : 윈도우가 종료 될때 메모리 해제등을 실행한다.

	if (!m_pParent)
	{
		gpC_window_manager->DisappearWindow(this);
		g_UnregisterWindow(this);
	}

	// 지금은 gpC_base->m_dialog_msg_pi의 포인터를 가져다 쓰므로 
	// 제거는 배제 하자.
	// 나중에 폰트 변경이 나오면 그때 수정.
//	if (m_listview_print_info != NULL)
//	{
//		DeleteNew(m_listview_print_info);
//		m_listview_print_info = NULL;
//	}

	// 동적으로 생성된 칼럼헤더 제거
	if (m_pColumn != NULL)
	{
		if (m_column_count > 1)
		{
			DeleteNewArray(m_pColumn);
			m_pColumn = NULL;
		}
		else
		{
			DeleteNew(m_pColumn);
			m_pColumn = NULL;
		}
	}

	// 아이템 리스트 제거
	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if (*itr != NULL)
		{
			DeleteNew(*itr);
			*itr = NULL;
		}
	}
	m_v_item.clear();
}

void C_VS_UI_LISTVIEW::Start()
{
	// To do : 윈도우의 프로세스를 가동 시킨다.

	PI_Processor::Start();
	//EMPTY_MOVE;
}

void C_VS_UI_LISTVIEW::Finish()
{
	// To do : 윈도우가 종료 될때 프로세스를 정리한다.

	PI_Processor::Finish();
	SetSelectItem(false);
}

void C_VS_UI_LISTVIEW::Show()
{
	CT_Done("WnSh: C_VS_UI_LISTVIEW::Show ENTER pthis=%p", (void*)this);
	// To do : 리스트 뷰를 드로잉 한다.

	if (!Running()) return;

	if (m_pParent)
	{
		x = m_pParent->x+m_in_pos.left;
		y = m_pParent->y+m_in_pos.top;
		m_background.SetRect(m_pParent->x+m_in_pos.left,m_pParent->y+m_in_pos.top,m_pParent->x+m_in_pos.left+w,m_pParent->y+m_in_pos.top+h);
	}
	else
	{
		m_background.SetRect(x,y,x+w,y+h);
	}

	DrawListView();
	DrawScrollBar();
}

void C_VS_UI_LISTVIEW::Process()
{
	// To do : 업데이트를 처리

	if (!Running()) return;

	if (m_vertical_scroll && !m_generate_scroll)
	{
		GenerateVerticalScroll();
		m_generate_scroll = true;
	}
}

bool C_VS_UI_LISTVIEW::MouseControl(UINT message, int _x, int _y)
{
	// To do : 마우스 이벤트를 처리

	if (!Running()) return false;

	_x -= x; _y -= y;
	if (m_vertical_scroll) 
	{
		if (ProcessVerticalScroll(message,_x,_y))
			return true;
	}

	CheckInColumn(message,_x,_y);
	CheckInItem(message,_x,_y);

	switch (message)
	{
	case M_WHEEL_UP:
		if (m_vertical_scroll)
		{
			--m_scroll_data.bar_pos;
			if (m_scroll_data.bar_pos < 0) m_scroll_data.bar_pos = 0;
			UpdateVerticalScrollBar();
			ShiftScrollBar(m_scroll_data.bar_pos);
		}
		break;
	case M_WHEEL_DOWN:
		if (m_vertical_scroll)
		{
			++m_scroll_data.bar_pos;
			if (m_scroll_data.bar_pos > (m_v_item.size() - m_view_item_count)) m_scroll_data.bar_pos = m_v_item.size() - m_view_item_count;
			UpdateVerticalScrollBar();
			ShiftScrollBar(m_scroll_data.bar_pos);
		}
		break;
	}

	return true;
}

void C_VS_UI_LISTVIEW::DrawListView()
{
	// To do : 리스트 뷰의 기본 이미지를 출력한다.

	if ((m_background.left < 0 && m_background.right < 0) || (m_background.top < 0 && m_background.bottom < 0))
	{
		return;
	}

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// 바탕 화면
		// 배경이 투명인지 아닌지 체크하고 그린다.
		if (m_style_flag & LSFBACKGROUNDNOTTRANS) gpC_base->m_p_DDSurface_back->BltColorAlpha(m_background,WHITE,0);
		gpC_base->m_p_DDSurface_back->Unlock();	
		
		// 윈도우 외곽선이 설정 되어 있다면 그린다.
		if (m_style_flag & LSFOUTLINE) gpC_base->m_p_DDSurface_back->DrawRect(m_background,BLACK);
	}

	// 칼럼 헤더
	int text_x = x + 3;
	int text_y = y + 3;
	for (int i = 0; i < m_column_count; ++i)
	{
		// 헤더 박스 위치
		CRect header_box(m_background.left+m_pColumn[i].x,m_background.top+m_pColumn[i].y,m_background.left+m_pColumn[i].x+m_pColumn[i].w,m_background.top+m_pColumn[i].y+m_pColumn[i].h);

		// 문자열의 정렬에 따라 x, y 좌표가 바뀐다.
		if (m_header_align & CHACENTER)
		{
			text_x = m_background.left + m_pColumn[i].x + ((m_pColumn[i].w / 2) - (g_GetStringWidth(m_pColumn[i].text.c_str(),m_listview_print_info->hfont) / 2));
			text_y = m_background.top + m_pColumn[i].y + ((m_pColumn[i].h / 2) - 5); 
		}

		// 0보다 작으면 이미지가 이상해 진다.
		if (header_box.left < 0 && header_box.right < 0)
			continue;
		if (header_box.top < 0 && header_box.bottom < 0)
			continue;
		// 크기가 없는 칼럼은 그리지 않는다.
		if (m_pColumn[i].w <= 0)
			continue;

		// 칼럼 헤더가 입체적 이미지라면..
		if (m_style_flag & LSFHEADER3DVIEW)
		{
			if(gpC_base->m_p_DDSurface_back->Lock())
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(header_box,GRAY,0);
				gpC_base->m_p_DDSurface_back->Unlock();
			}

			if (m_click_column == i)
			{
				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(header_box.left,header_box.top,header_box.right-header_box.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->HLine(header_box.left,header_box.top+(header_box.bottom-header_box.top),header_box.right-header_box.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left,header_box.top,header_box.bottom-header_box.top,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left+(header_box.right-header_box.left),header_box.top,header_box.bottom-header_box.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(header_box.left+1,header_box.top+1,header_box.right-header_box.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->HLine(header_box.left+1,header_box.top+(header_box.bottom-header_box.top)-1,header_box.right-header_box.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left+1,header_box.top+1,header_box.bottom-header_box.top-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left+(header_box.right-header_box.left)-1,header_box.top+1,header_box.bottom-header_box.top-2,GRAY);

				g_FL2_GetDC();
				text_x += 1;
				text_y += 1;
				g_PrintColorStr(text_x,text_y,m_pColumn[i].text.c_str(),*m_listview_print_info,RGB_BLACK);
				g_FL2_ReleaseDC();
			}
			else
			{
				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(header_box.left,header_box.top,header_box.right-header_box.left,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(header_box.left,header_box.top+(header_box.bottom-header_box.top),header_box.right-header_box.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left,header_box.top,header_box.bottom-header_box.top,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left+(header_box.right-header_box.left),header_box.top,header_box.bottom-header_box.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(header_box.left+1,header_box.top+1,header_box.right-header_box.left-2,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(header_box.left+1,header_box.top+(header_box.bottom-header_box.top)-1,header_box.right-header_box.left-2,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left+1,header_box.top+1,header_box.bottom-header_box.top-2,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(header_box.left+(header_box.right-header_box.left)-1,header_box.top+1,header_box.bottom-header_box.top-2,DARKGRAY);

				g_FL2_GetDC();
				g_PrintColorStr(text_x,text_y,m_pColumn[i].text.c_str(),*m_listview_print_info,RGB_BLACK);
				g_FL2_ReleaseDC();
			}
		}
		else
		{
			if(gpC_base->m_p_DDSurface_back->Lock())
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(header_box,GRAY,25);
				gpC_base->m_p_DDSurface_back->Unlock();
			}

			// 칼럼 헤더 외곽선 - 바깥
			gpC_base->m_p_DDSurface_back->DrawRect(header_box,BLACK);

			// 칼럼 문구
			g_FL2_GetDC();
			g_PrintColorStr(text_x,text_y,m_pColumn[i].text.c_str(),*m_listview_print_info,RGB_BLACK);
			g_FL2_ReleaseDC();
		}
	}

	// 선택된 박스를 사용 했을때..
	if (m_style_flag & LSFITEMSELECTBOX)
	{
		int index = 1, select_index = 0;
		for (VEC_LTVITEM::iterator itr = m_v_item.begin(); itr != m_v_item.end(); ++itr)
		{
			if (index > (m_view_item_count + m_view_begin_item_index)) break;
			if ((*itr)->select)
			{
				select_index = index - m_view_begin_item_index;
				if ((select_index < 1) || select_index > m_view_item_count) break;
				if(gpC_base->m_p_DDSurface_back->Lock())
				{
					gpC_base->m_p_DDSurface_back->BltColorAlpha(CRect(m_background.left+1,m_background.top+1+(select_index*m_item_height),m_background.left+w,m_background.top+1+(select_index*m_item_height)+22),YELLOWGREEN,0);
					gpC_base->m_p_DDSurface_back->Unlock();			
				}
			}
			++index;
		}
	}

	// 프레임 내부의 라인이 설정 되었다면...
	if (m_style_flag & LSFFRAMELINE)
	{
		// 세로선
		for (int i = 1; i < m_column_count; ++i)
		{
			gpC_base->m_p_DDSurface_back->VLine(m_background.left+m_pColumn[i].x,m_background.top,h,BLACK);
		}
		// 가로선
		for (int i = 2; i <= static_cast<int>(h/m_item_height); ++i)
		{
			gpC_base->m_p_DDSurface_back->HLine(m_background.left,m_background.top+(i*m_item_height),w,BLACK);
		}
	}

	g_FL2_GetDC();

	// 아이템
	int y_gap = 3, y_space = 3, index = 0;
	for (VEC_LTVITEM::iterator itr = m_v_item.begin() + m_view_begin_item_index; itr != m_v_item.end(); ++itr)
	{
		if (index >= m_view_item_count) break;
		for (int i = 0; i < m_column_count; ++i)
		{
			if ((*itr)->text.find(i) == (*itr)->text.end()) 
				continue;
			// 크기가 없는 칼럼의 아이템은 그리지 않는다.
			if (m_pColumn[i].w <= 0)
				continue;
			if ((m_style_flag & LSFITEMSELECTBOX) && (*itr)->select)
				g_PrintColorStr(m_background.left+m_pColumn[i].x+3,m_background.top+m_pColumn[i].y+m_pColumn[i].h+y_gap+y_space,(*itr)->text.find(i)->second.c_str(),*m_listview_print_info,RGB_LIGHTBLUE);
			else
				g_PrintColorStr(m_background.left+m_pColumn[i].x+3,m_background.top+m_pColumn[i].y+m_pColumn[i].h+y_gap+y_space,(*itr)->text.find(i)->second.c_str(),*m_listview_print_info,RGB_BLACK);
		}
		y_gap += m_item_height;
		++index;
	}

	g_FL2_ReleaseDC();
}

void C_VS_UI_LISTVIEW::DrawScrollBar()
{
	// To do : 리스트 뷰의 스크롤을 드로잉 한다.

	if (!m_vertical_scroll) return;

	if ((m_background.left < 0 && m_background.right < 0) || (m_background.top < 0 && m_background.bottom < 0))
	{
		return;
	}

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// 스크롤 바탕
		gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.background_area,DARKGRAY,18);
		
		if (m_style_flag & LSFHEADER3DVIEW)
		{
			// 상단 버튼 
			if (m_scroll_data.up_button_push)
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.up_button_area,GRAY,0);

				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left,m_scroll_data.up_button_area.top,m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left,m_scroll_data.up_button_area.top+(m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top),m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left,m_scroll_data.up_button_area.top,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left+(m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left),m_scroll_data.up_button_area.top,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left+1,m_scroll_data.up_button_area.top+1,m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left+1,m_scroll_data.up_button_area.top+(m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top)-1,m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left+1,m_scroll_data.up_button_area.top+1,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left+(m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left)-1,m_scroll_data.up_button_area.top+1,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top-2,GRAY);
			}
			else
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.up_button_area,GRAY,0);

				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left,m_scroll_data.up_button_area.top,m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left,m_scroll_data.up_button_area.top+(m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top),m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left,m_scroll_data.up_button_area.top,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left+(m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left),m_scroll_data.up_button_area.top,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left+1,m_scroll_data.up_button_area.top+1,m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left-2,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.up_button_area.left+1,m_scroll_data.up_button_area.top+(m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top)-1,m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left-2,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left+1,m_scroll_data.up_button_area.top+1,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top-2,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.up_button_area.left+(m_scroll_data.up_button_area.right-m_scroll_data.up_button_area.left)-1,m_scroll_data.up_button_area.top+1,m_scroll_data.up_button_area.bottom-m_scroll_data.up_button_area.top-2,DARKGRAY);
			}
			// 하단 버튼	
			if (m_scroll_data.dw_button_push)
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.dw_button_area,GRAY,0);

				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left,m_scroll_data.dw_button_area.top,m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left,m_scroll_data.dw_button_area.top+(m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top),m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left,m_scroll_data.dw_button_area.top,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left+(m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left),m_scroll_data.dw_button_area.top,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left+1,m_scroll_data.dw_button_area.top+1,m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left+1,m_scroll_data.dw_button_area.top+(m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top)-1,m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left+1,m_scroll_data.dw_button_area.top+1,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left+(m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left)-1,m_scroll_data.dw_button_area.top+1,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top-2,GRAY);
			}
			else
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.dw_button_area,GRAY,0);

				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left,m_scroll_data.dw_button_area.top,m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left,m_scroll_data.dw_button_area.top+(m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top),m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left,m_scroll_data.dw_button_area.top,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left+(m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left),m_scroll_data.dw_button_area.top,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left+1,m_scroll_data.dw_button_area.top+1,m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left-2,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.dw_button_area.left+1,m_scroll_data.dw_button_area.top+(m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top)-1,m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left-2,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left+1,m_scroll_data.dw_button_area.top+1,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top-2,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.dw_button_area.left+(m_scroll_data.dw_button_area.right-m_scroll_data.dw_button_area.left)-1,m_scroll_data.dw_button_area.top+1,m_scroll_data.dw_button_area.bottom-m_scroll_data.dw_button_area.top-2,DARKGRAY);
			}
			// 스크롤 바
			if (m_scroll_data.bar_button_push)
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.bar_button_area,GRAY,0);

				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left,m_scroll_data.bar_button_area.top,m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left,m_scroll_data.bar_button_area.top+(m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top),m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left,m_scroll_data.bar_button_area.top,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left+(m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left),m_scroll_data.bar_button_area.top,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left+1,m_scroll_data.bar_button_area.top+1,m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left+1,m_scroll_data.bar_button_area.top+(m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top)-1,m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left+1,m_scroll_data.bar_button_area.top+1,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top-2,GRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left+(m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left)-1,m_scroll_data.bar_button_area.top+1,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top-2,GRAY);
			}
			else
			{
				gpC_base->m_p_DDSurface_back->BltColorAlpha(m_scroll_data.bar_button_area,GRAY,0);

				// 외곽선 - 바깥
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left,m_scroll_data.bar_button_area.top,m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left,m_scroll_data.bar_button_area.top+(m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top),m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left,m_scroll_data.bar_button_area.top,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left+(m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left),m_scroll_data.bar_button_area.top,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top,DARKGRAY);

				// 외곽선 - 안쪽
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left+1,m_scroll_data.bar_button_area.top+1,m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left-2,WHITE);
				gpC_base->m_p_DDSurface_back->HLine(x+m_scroll_data.bar_button_area.left+1,m_scroll_data.bar_button_area.top+(m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top)-1,m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left-2,DARKGRAY);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left+1,m_scroll_data.bar_button_area.top+1,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top-2,WHITE);
				gpC_base->m_p_DDSurface_back->VLine(x+m_scroll_data.bar_button_area.left+(m_scroll_data.bar_button_area.right-m_scroll_data.bar_button_area.left)-1,m_scroll_data.bar_button_area.top+1,m_scroll_data.bar_button_area.bottom-m_scroll_data.bar_button_area.top-2,DARKGRAY);
			}
		}
		else
		{
			// 상단 버튼 
			CRect bt(x+m_scroll_data.up_button_area.left,y+m_scroll_data.up_button_area.top,x+m_scroll_data.up_button_area.left+m_scroll_data.up_button_area.right,y+m_scroll_data.up_button_area.top+m_scroll_data.up_button_area.bottom);
			gpC_base->m_p_DDSurface_back->BltColorAlpha(bt,GRAY,0);
			// 하단 버튼	
			bt.SetRect(x+m_scroll_data.dw_button_area.left,y+m_scroll_data.dw_button_area.top,x+m_scroll_data.dw_button_area.left+m_scroll_data.dw_button_area.right,y+m_scroll_data.dw_button_area.top+m_scroll_data.dw_button_area.bottom);
			gpC_base->m_p_DDSurface_back->BltColorAlpha(bt,GRAY,0);
			// 스크롤 바
			bt.SetRect(x+m_scroll_data.bar_button_area.left,y+m_scroll_data.bar_button_area.top,x+m_scroll_data.bar_button_area.left+m_scroll_data.bar_button_area.right,y+m_scroll_data.bar_button_area.top+m_scroll_data.bar_button_area.bottom);
			gpC_base->m_p_DDSurface_back->BltColorAlpha(bt,DARKGRAY,0);
		}
		gpC_base->m_p_DDSurface_back->Unlock();	
	}
}

int C_VS_UI_LISTVIEW::CheckInColumn(UINT message, int _x, int _y)
{
	// To do : 칼럼이 눌렸을 때 해당 칼럼의 인덱스를 얻어와 정렬을 수행한다.

	// 마우스 왼쪽 버튼이 띄어 졌을때 일단 선택된 칼럼을 모두 해제 한다.
//	if (message == M_LEFTBUTTON_UP)
//		m_click_column = -1;
	
	for (int i = 0; i < m_column_count; ++i)
	{
		if (m_pColumn[i].IsInRect(_x,_y))
		{	
			switch (message)
			{
			case M_LEFTBUTTON_DOWN: case M_LB_DOUBLECLICK:
				m_click_column = i;
				break;
			case M_LEFTBUTTON_UP:
				ClickColumnProc(m_click_column);
				m_click_column = -1;
				break;
			}
			return m_click_column;
		}
	}
	return m_click_column;
}

int C_VS_UI_LISTVIEW::CheckInItem(UINT message, int _x, int _y)
{
	// To do : 선택된 아이템의 인덱스를 얻어와 해당 프로시저를 수행한다.

	int current_pos = _y / m_item_height;
	if (current_pos > 0 && current_pos <= m_view_item_count && current_pos <= m_v_item.size())
	{
		current_pos += m_view_begin_item_index;
		if (_x >= 0 && _x <= w)
		{
			switch (message)
			{
			case M_LEFTBUTTON_DOWN:
				SetSelectItem(false);
				SetSelectItem(true,current_pos-1);
				LbclkItemProc(m_v_item[current_pos-1]);
				break;
			case M_LB_DOUBLECLICK:
				SetSelectItem(false);
				SetSelectItem(true,current_pos-1);
				LdbclkItemProc(m_v_item[current_pos-1]);
				break;
			case M_RIGHTBUTTON_DOWN:
				RbclkItemProc(m_v_item[current_pos-1]);
				break;
			}
			return current_pos - 1;
		}
	}
	else if (!m_background.PtInRect(CPoint(_x+x,_y+y))
		|| !m_scroll_data.background_area.PtInRect(CPoint(_x+x,_y+y)))
	{
		if (message == M_LEFTBUTTON_DOWN
		|| message == M_LB_DOUBLECLICK)
		{
			SetSelectItem(false);
		}
	}
	return current_pos;
}

void C_VS_UI_LISTVIEW::ClickColumnProc(int index)
{
	// To do : 선택된 컬럼 헤더의 기능을 수행한다.

	SearchClickColumnHeader(index);

	// 정렬이 설정 되어 있어야 아이템 정렬을 한다.
	if (m_style_flag & LSFSORT)
	{
		SortColumnItem();
	}
}

void C_VS_UI_LISTVIEW::SetSelectItem(bool state /*= true*/, int index /*= -1*/)
{
	// To do : 아이템 선택을 설정한다.
	// state는 선택 상태
	// index는 해당 아이템의 인덱스. -1일때는 전체를 설정한다.

	if (index == -1)
	{
		VEC_LTVITEM::iterator itr = m_v_item.begin();
		for (; itr != m_v_item.end(); ++itr)
		{
			(*itr)->select = state;
		}
	}
	else
	{
		VEC_LTVITEM::iterator itr = m_v_item.begin() + index;
		if (itr != m_v_item.end())
		{
			(*itr)->select = state;
		}
	}
}

void C_VS_UI_LISTVIEW::SearchClickColumnHeader(int index)
{
	// To do : 컬럼이 눌렸을 때 해당 컬럼의 정렬을 위해서 눌려진 컬럼의 인덱스를 설정한다.

	if (index < 0 || index >= m_column_count)
		return;

	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		(*itr)->sort_column_index = index;
	}
}

bool C_VS_UI_LISTVIEW::InitColumnHeader(int num, DWORD align /*= CHACENTER*/)
{
	// To do : 컬럼 헤더의 초기 설정을 한다.
	// num은 만들어질 컬럼의 개수

	m_header_align = align;

	if (num == 0)
		return false;
	else
	{
		if (m_pColumn != NULL)
		{
			if (m_column_count > 1)
			{
				DeleteNewArray(m_pColumn);
				m_pColumn = NULL;
			}
			else
			{
				DeleteNew(m_pColumn);
				m_pColumn = NULL;
			}
			m_column_count = 0;
		}
		
		m_column_count = num;
		m_pColumn = new LTVCOLUMNHEADER[num];
	}
	return true;
}

void C_VS_UI_LISTVIEW::SetColumnHeader(const char* pText, int index, int width)
{
	// To do : 각 컬럼 헤더의 설정을 한다.

	if (m_pColumn != NULL && index >= 0 && index < m_column_count)
	{
		m_pColumn[index].text = pText;
		m_pColumn[index].y = 0;
		m_pColumn[index].w = width;
		m_pColumn[index].h = m_item_height;
	}

	for (int i = 0; i < m_column_count; ++i)
	{
		if (i == 0)
			m_pColumn[i].x = 0;
		else
			m_pColumn[i].x = m_pColumn[i-1].x + m_pColumn[i-1].w;
	}
}

void C_VS_UI_LISTVIEW::InsertColumnItem(const char* pText, int item_num, int column_num)
{
	// To do : 아이템을 추가하는 작업을 한다.
	// pText는 아이템의 문구
	// item_num은 생성될 아이템의 인덱스. 이미 있다면 내용을 바꾼다.
	// column_num은 추가될 아이템의 컬럼 인덱스

	if (m_column_count == 0 || column_num < 0 || column_num >= m_column_count)
		return;

	// 배열내에 생성된 아이템 정보가 없다면 생성을 먼저 해줘야 한다.
	if (item_num >= static_cast<int>(m_v_item.size()))
	{
		LTVITEM* pItem = new LTVITEM;
		m_v_item.push_back(pItem);
	}

	VEC_LTVITEM::iterator itr = m_v_item.begin() + item_num;
	(*itr)->text[column_num] = pText;

	m_view_max_height = m_v_item.size() * m_item_height;
	m_view_begin_item_index = 0;

	if ((m_view_max_height + m_item_height) > h)
	{
		if (!m_vertical_scroll)
		{
			m_vertical_scroll = true;
		}
	}
	else
	{
		m_vertical_scroll = false;
		m_generate_scroll = false;
	}
}

void C_VS_UI_LISTVIEW::DeleteColumnItem(int index)
{
	// To do : 해당 인덱스의 아이템을 지운다.

	VEC_LTVITEM::iterator itr = m_v_item.begin() + index;
	if (itr != m_v_item.end())
	{
		if (*itr != NULL)
		{
			m_v_item.erase(itr);
			//DeleteNewArray(*itr);
			//*itr = NULL;
		}
	}

	m_view_max_height = m_v_item.size() * m_item_height;
	m_view_begin_item_index = 0;

	if ((m_view_max_height + m_item_height) > h)
	{
		if (!m_vertical_scroll)
		{
			m_vertical_scroll = true;
		}
	}
	else
	{
		m_vertical_scroll = false;
		m_generate_scroll = false;
	}
}

void C_VS_UI_LISTVIEW::DeleteColumnItem(const char* pText)
{
	// To do : 해당 문구의 아이템을 지운다.

	if (pText == "") return;
	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if ((*itr)->text.find(0)->second.compare(pText) == 0)
		{
			//LTVITEM* item = (*itr);
			m_v_item.erase(itr);
			//DeleteNewArray(item);
			//item = NULL;
			break;;
		}
	}

	m_view_max_height = m_v_item.size() * m_item_height;
	m_view_begin_item_index = 0;

	if ((m_view_max_height + m_item_height) > h)
	{
		if (!m_vertical_scroll)
		{
			m_vertical_scroll = true;
		}
	}
	else
	{
		m_vertical_scroll = false;
		m_generate_scroll = false;
	}
}

void C_VS_UI_LISTVIEW::DeleteToSelectItem()
{
	// To do : 선택된 아이템을 지운다.

	int index = 0;
	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if ((*itr)->select)
		{
			DeleteColumnItem(index);
		}
		++index;
	}

	m_view_max_height = m_v_item.size() * m_item_height;
	m_view_begin_item_index = 0;

	if ((m_view_max_height + m_item_height) > h)
	{
		if (!m_vertical_scroll)
		{
			m_vertical_scroll = true;
		}
	}
	else
	{
		m_vertical_scroll = false;
		m_generate_scroll = false;
	}
}

void C_VS_UI_LISTVIEW::DeleteAllItem()
{
	// To do : 모든 아이템을 지운다.

	// 아이템 리스트 제거
	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if (*itr != NULL)
		{
			DeleteNew(*itr);
			*itr = NULL;
		}
	}
	m_v_item.clear();
	m_view_max_height = m_v_item.size() * m_item_height;
	m_view_begin_item_index = 0;

	if ((m_view_max_height + m_item_height) > h)
	{
		if (!m_vertical_scroll)
		{
			m_vertical_scroll = true;
		}
	}
	else
	{
		m_vertical_scroll = false;
		m_generate_scroll = false;
	}
}

int C_VS_UI_LISTVIEW::GetSelectItemSize()
{
	// To do : 선택된 아이템의 개수 
	// 선택된 아이템의 개수를 반환하는 이유는 다중 선택이 있기 때문이다.

	int count = 0;
	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if ((*itr)->select)
			++count;
	}
	return count;
}

int C_VS_UI_LISTVIEW::GetItemIndexByName(const char* pText)
{
	// To do : 문구의 내용으로 아이템의 인덱스를 얻어온다.

	int index = 0;
	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if ((*itr)->text[0].compare(pText) == 0)
		{
			return index;
		}
		++index;
	}
	return -1;
}

const char* C_VS_UI_LISTVIEW::GetItemNameBytSelect(int column_index)
{
	// To do : 선택된 아이템으로부터 아이템의 문구를 얻어온다.
	// column_index는 얻고 싶은 컬럼의 인덱스

	VEC_LTVITEM::iterator itr = m_v_item.begin();
	for (; itr != m_v_item.end(); ++itr)
	{
		if ((*itr)->select)
			return (*itr)->text.find(column_index)->second.c_str();
	}
	return "";
}

const char* C_VS_UI_LISTVIEW::GetItemNameByIndex(int index, int column)
{
	// To do : 해당 인덱스로 아이템의 문구를 얻어온다.

	VEC_LTVITEM::iterator itr = m_v_item.begin() + index;
	if (itr != m_v_item.end())
	{
		return (*itr)->text.find(column)->second.c_str();
	}
	return "";
}

void C_VS_UI_LISTVIEW::SortColumnItem()
{
	// To do : 각 아이템을 현재 선택된 컬럼을 기준으로 정렬한다.

	if (m_sort_state ^= 1)
		std::sort(m_v_item.begin(),m_v_item.end(),CompareTextValueAs);
	else
		std::sort(m_v_item.begin(),m_v_item.end(),CompareTextValueDs);
}

void C_VS_UI_LISTVIEW::SetPrintInfo(const char* font, PrintInfo& crtprintinfo)
{
	// To do : 폰트를 설정 할수 있도록 한다.

	// 폰트를 변경 할수 있도록 추후에 코드를 수정하자.
	m_listview_print_info = &crtprintinfo;
	return;
	// 폰트 설정을 해준다.
	m_listview_print_info = new PrintInfo;
	m_listview_print_info->hfont = CreateFont(13,0,0,0,FW_BOLD,FALSE,FALSE,
									0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_ROMAN,
									font);
	m_listview_print_info->text_color = gpC_base->m_dialog_msg_pi.text_color;
	m_listview_print_info->text_align = gpC_base->m_dialog_msg_pi.text_align;
	m_listview_print_info->bk_mode = gpC_base->m_dialog_msg_pi.bk_mode;
	m_listview_print_info->back_color = gpC_base->m_dialog_msg_pi.back_color;
}

void C_VS_UI_LISTVIEW::ShiftScrollBar(int pos)
{
	// To do : 리스트의 스크롤 바 위치를 변화 시킨다.

	m_view_begin_item_index = pos;
	if (m_view_begin_item_index < 0)
		m_view_begin_item_index = 0;
	else if ((m_view_begin_item_index + m_view_item_count) > m_v_item.size())
		m_view_begin_item_index = m_v_item.size() - m_view_item_count;
}

void C_VS_UI_LISTVIEW::UpdateVerticalScrollBar(bool bar /*= false*/, int bar_pos /*= 0*/)
{
	// To do : 스크롤 바의 크기는 리스트가 늘거나 줄때마다 그 크기가 변화하므로 업데이트를 해줘야 한다.

	int outside_item_cnt = m_v_item.size() - m_view_item_count;
	float tap_height = static_cast<float>(m_scroll_data.back_height) / outside_item_cnt;
	if (!bar)
	{
		m_scroll_data.bar_button_area.top = m_scroll_data.back_pos_y + (static_cast<float>((static_cast<float>(m_scroll_data.bar_pos) * tap_height)) - m_scroll_data.bar_button_size.cy);
	}
	else
	{
		m_scroll_data.bar_button_area.top += bar_pos;
		m_scroll_data.bar_pos = m_scroll_data.bar_button_area.top / tap_height;

		if (m_scroll_data.bar_pos < 0) m_scroll_data.bar_pos = 0;
		else if (m_scroll_data.bar_pos > (m_v_item.size() - m_view_item_count)) m_scroll_data.bar_pos = m_v_item.size() - m_view_item_count;
	}

	// 스크롤 바가 넘어가지 말아야 할 제한선
	if (m_scroll_data.bar_button_area.top < m_scroll_data.back_pos_y)
		m_scroll_data.bar_button_area.top = m_scroll_data.back_pos_y;
	else if (m_scroll_data.bar_button_area.top >= ((m_scroll_data.back_pos_y + m_scroll_data.back_height) - m_scroll_data.bar_button_size.cy)) 
		m_scroll_data.bar_button_area.top = m_scroll_data.back_pos_y + m_scroll_data.back_height - m_scroll_data.bar_button_size.cy;

	m_scroll_data.bar_button_area.bottom = m_scroll_data.bar_button_area.top + m_scroll_data.bar_button_size.cy;
}

void C_VS_UI_LISTVIEW::UpdateHorizonScrollBar(bool bar /*= false*/, int bar_pos /*= 0*/)
{
	// To do : 스크롤 바의 크기는 리스트가 늘거나 줄때마다 그 크기가 변화하므로 업데이트를 해줘야 한다.
}

void C_VS_UI_LISTVIEW::GenerateVerticalScroll()
{
	// To do : 세로 스크롤의 초기 위치 및 크기 등을 정의 한다.

	int pos_x = w - 15;
	int pos_y = 0;
	int back_w = 15;
	int back_h = h;
	{// 스크롤 배경
		m_scroll_data.background_area.SetRect(pos_x,pos_y,pos_x+back_w,pos_y+back_h);
	}

	int ud_button_h = m_item_height;
	int up_button_y = pos_y;
	int dw_button_y = back_h - m_item_height;
	{
		// 상단 버튼
		m_scroll_data.up_button_area.SetRect(pos_x,up_button_y,pos_x+back_w,up_button_y+ud_button_h);
		// 하단 버튼
		m_scroll_data.dw_button_area.SetRect(pos_x,dw_button_y,pos_x+back_w,dw_button_y+ud_button_h);
	}

	int rest_cnt = ((m_view_max_height - h) / m_item_height) + 1;
	int bar_button_y = up_button_y + ud_button_h;
	int bar_button_h = (dw_button_y - bar_button_y) - ((dw_button_y - bar_button_y) * (static_cast<float>(rest_cnt) / 20.0f));
	{// 스크롤 바
		m_scroll_data.bar_button_size.cx = back_w;
		m_scroll_data.bar_button_size.cy = bar_button_h;
		m_scroll_data.bar_button_area.SetRect(pos_x,bar_button_y,pos_x+back_w,bar_button_y+bar_button_h);
	}

	m_scroll_data.bar_pos = 0;
	m_scroll_data.back_pos_y = bar_button_y;
	m_scroll_data.back_height = back_h - (ud_button_h * 2);
	m_scroll_data.old_time = timeGetTime();
}

void C_VS_UI_LISTVIEW::GenerateHorizonScorll()
{
	// To do : 가로 스크롤의 초기 위치 및 크기 등을 정의 한다.
}

bool C_VS_UI_LISTVIEW::ProcessVerticalScroll(UINT message, int _x, int _y)
{
	// To do : 세로 스크롤의 기능을 처리한다.

	if (message == M_LEFTBUTTON_DOWN)
	{
		if (m_scroll_data.up_button_area.PtInRect(CPoint(_x,_y)))
		{// 상단 버튼
			m_scroll_data.up_button_push = true;
		}
		else if (m_scroll_data.dw_button_area.PtInRect(CPoint(_x,_y)))
		{// 하단 버튼
			m_scroll_data.dw_button_push = true;
		}
		else if (m_scroll_data.bar_button_area.PtInRect(CPoint(_x,_y)))
		{// 스크롤 바
			m_scroll_data.bar_button_push = true;
			m_old_mouse_pos.x = _x;
			m_old_mouse_pos.y = _y;
		}
		else if (m_scroll_data.background_area.PtInRect(CPoint(_x,_y)))
		{// 스크롤 배경
			m_scroll_data.background_push = true;
			m_old_mouse_pos.x = m_scroll_data.bar_button_area.left;
			m_old_mouse_pos.y = m_scroll_data.bar_button_area.top;
		}
		m_scroll_data.old_time = 0;
	}
	else if (message == M_LEFTBUTTON_UP)
	{
		m_scroll_data.PushInit();
	}

	if (m_scroll_data.up_button_push)
	{// 상단 버튼
		if (timeGetTime() - m_scroll_data.old_time > 100)
		{
			m_scroll_data.up_button_push = true;
			--m_scroll_data.bar_pos;
			if (m_scroll_data.bar_pos < 0) 
				m_scroll_data.bar_pos = 0;

			UpdateVerticalScrollBar();
			ShiftScrollBar(m_scroll_data.bar_pos);

			m_scroll_data.old_time = timeGetTime();
			return true;
		}
	}
	else if (m_scroll_data.dw_button_push)
	{// 하단 버튼
		if (timeGetTime() - m_scroll_data.old_time > 100)
		{
			m_scroll_data.dw_button_push = true;
			++m_scroll_data.bar_pos;
			if (m_scroll_data.bar_pos > (m_v_item.size() - m_view_item_count)) 
				m_scroll_data.bar_pos = m_v_item.size() - m_view_item_count;

			UpdateVerticalScrollBar();
			ShiftScrollBar(m_scroll_data.bar_pos);

			m_scroll_data.old_time = timeGetTime();
			return true;
		}
	}
	else if (m_scroll_data.bar_button_push)
	{// 막대 버튼
		if (_y >= m_scroll_data.back_pos_y && _y <= (m_scroll_data.back_pos_y + m_scroll_data.back_height))
		{
			int move_x = _x - m_old_mouse_pos.x;
			int move_y = _y - m_old_mouse_pos.y;
			int old_bar_pos = m_scroll_data.bar_pos;

			UpdateVerticalScrollBar(true,move_y);
			ShiftScrollBar(m_scroll_data.bar_pos);

			m_old_mouse_pos.x = _x;
			m_old_mouse_pos.y = _y;
		}
		return true;
	}
	else if (m_scroll_data.background_push)
	{// 바탕
		if (_y >= m_scroll_data.back_pos_y && _y <= (m_scroll_data.back_pos_y + m_scroll_data.back_height))
		{
			int move_x = _x - m_old_mouse_pos.x;
			int move_y = _y - m_old_mouse_pos.y;

			int old_bar_pos = m_scroll_data.bar_pos;

			UpdateVerticalScrollBar(true,move_y);
			ShiftScrollBar(m_scroll_data.bar_pos);

			m_old_mouse_pos.x = _x;
			m_old_mouse_pos.y = _y;
		}
		return true;
	}
	return false;
}

bool C_VS_UI_LISTVIEW::ProcessHorizonScroll(UINT message, int _x, int _y)
{
	// To do : 가로 스크롤의 기능을 처리한다.

	return false;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// class C_VS_UI_CHECKBOX //
//////////////////////////////////////////////////////////////////////////

C_VS_UI_CHECKBOX::C_VS_UI_CHECKBOX()
{
	// To do : 윈도우가 시작되어 데이터들을 초기화 시킨다.

	m_parent = NULL;

	x = 0;
	y = 0;
	w = 10;
	h = 10;

	m_check = false;
	m_back_color = WHITE;
}

C_VS_UI_CHECKBOX::C_VS_UI_CHECKBOX(Window* pParent, int _x, int _y)
{
	// To do : 윈도우가 시작되어 데이터들을 초기화 시킨다.

	m_parent = pParent;
	if (m_parent)
	{
		m_parent_pos_x = &m_parent->x;
		m_parent_pos_y = &m_parent->y;
		m_parent_size_cx = &m_parent->w;
		m_parent_size_cy = &m_parent->y;
	}

	x = _x;
	y = _y;
	w = 10;
	h = 10;

	m_check = false;
	m_back_color = WHITE;
}

C_VS_UI_CHECKBOX::~C_VS_UI_CHECKBOX()
{
	// To do : 윈도우가 종료 될때 메모리 해제등을 실행한다.

	Finish();
}

void C_VS_UI_CHECKBOX::Start()
{
	// To do : 윈도우의 프로세스를 가동 시킨다.

	PI_Processor::Start();
	if (m_parent)
	{
		g_RegisterWindow(this);
		gpC_window_manager->AppearWindow(this);
	}
}

void C_VS_UI_CHECKBOX::Finish()
{
	// To do : 윈도우가 종료 될때 프로세스를 정리한다.

	PI_Processor::Finish();
	if (m_parent)
	{
		gpC_window_manager->DisappearWindow(this);
		g_UnregisterWindow(this);
	}
}

void C_VS_UI_CHECKBOX::Process()
{
	// To do : 업데이트를 처리

	if (!Running()) return;
}

void C_VS_UI_CHECKBOX::Show()
{
	CT_Done("WnSh: C_VS_UI_CHECKBOX::Show ENTER pthis=%p", (void*)this);
	// To do : 체크 박스를 드로잉 한다.

	if (!Running()) return;

	CRect check_box, in_box_line;
	
	if (m_parent)
	{
		check_box.SetRect((*m_parent_pos_x + x),(*m_parent_pos_y + y),(*m_parent_pos_x + x + w),(*m_parent_pos_y + y + h));
		in_box_line.SetRect(check_box.left+1,check_box.top+1,check_box.right-1,check_box.bottom-1);
	}
	else
	{
		check_box.SetRect(x,y,x + w,y + h);
		in_box_line.SetRect(check_box.left+1,check_box.top+1,check_box.right-1,check_box.bottom-1);
	}

	if ((check_box.left < 0 && check_box.right < 0) || (check_box.top < 0 && check_box.bottom < 0))
	{
		return;
	}

	if (gpC_base->m_p_DDSurface_back->Lock())
	{// 체크 박스
		gpC_base->m_p_DDSurface_back->BltColorAlpha(check_box,m_back_color,0);
		gpC_base->m_p_DDSurface_back->Unlock();

		gpC_base->m_p_DDSurface_back->DrawRect(in_box_line,GRAY);
		gpC_base->m_p_DDSurface_back->DrawRect(check_box,BLACK);
	}

	if (m_check)
	{// 체크 표시
		gpC_base->m_p_DDSurface_back->Line(in_box_line.left+1,in_box_line.top+4,in_box_line.left+4,in_box_line.top+7,BLACK);
		gpC_base->m_p_DDSurface_back->Line(in_box_line.left+8,in_box_line.top+1,in_box_line.left+4,in_box_line.top+7,BLACK);
		gpC_base->m_p_DDSurface_back->Line(in_box_line.left+1,in_box_line.top+5,in_box_line.left+4,in_box_line.top+8,BLACK);
		gpC_base->m_p_DDSurface_back->Line(in_box_line.left+8,in_box_line.top+2,in_box_line.left+4,in_box_line.top+8,BLACK);
	}
}

bool C_VS_UI_CHECKBOX::MouseControl(UINT message, int _x, int _y)
{
	// To do : 마우스 이벤트를 처리

	if (!Running()) 
		return false;

	if (message == M_LEFTBUTTON_UP)
	{
		m_back_color = WHITE;
	}
	if (IsPixel(_x,_y))
	{
		if (message == M_LEFTBUTTON_DOWN)
		{
			m_back_color = GRAY;
			return false;
		}
		if (message == M_LEFTBUTTON_UP)
		{
			m_back_color = WHITE;
			m_check ^= 1;
			return false;
		}
	}

	return true;
}

void C_VS_UI_CHECKBOX::SetParent(Window* pParent)
{
	// To do : 상위 윈도우 설정

	m_parent = pParent;
	if (m_parent)
	{
		m_parent_pos_x = &m_parent->x;
		m_parent_pos_y = &m_parent->y;
		m_parent_size_cx = &m_parent->w;
		m_parent_size_cy = &m_parent->y;
	}
}
