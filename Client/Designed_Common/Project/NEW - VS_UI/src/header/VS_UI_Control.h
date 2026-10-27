/*
	2009.07.13 Create VS_UI_Control.h by Li,Kwon-Hwak

	Description : 다크 에덴의 컨트롤 UI를 선언해 놓았다.

	- 09.07.13일 기준 Menu Control, List View Control, Check Box Control
*/

#pragma once

#include "VS_UI_Base.h"
#include "VS_UI_widget.h"
#include "RarFile.h"

//////////////////////////////////////////////////////////////////////
// class PositionInfo
// 작성일 : 09.06.08
// 작성자 : 이 권 확
// 기  능 : UI의 좌표정보를 외부에서 정의하여 데이터를 얻어온다.
//////////////////////////////////////////////////////////////////////

class PositionInfo
{
protected:
	CRarFile			m_rpk_file;
	std::string			m_info_data;

public:
	PositionInfo();
	virtual ~PositionInfo();

	bool ParsingData(const char* pszRpkFile, const char* pszDataFile, const char* pszPassWord = "1x700");
	bool GetData(const char* pszKeyWord, int& value);
};


//////////////////////////////////////////////////////////////////////
// class C_VS_UI_MENU												
// 작성일 : 09.05.27
// 작성자 : 이 권 확
// 기  능 : 메뉴 UI에 대한 객체이다.
//////////////////////////////////////////////////////////////////////

struct MENU_COMMAND
{
	enum COMMAND_STYLE
	{
		CS_NONE,
		CS_SUBMENU,
		CS_CHECK
	};

	std::string command;	// 명령어 문자열
	id_t id;				// 명령어 아이디
	int style;				// 명령어 스타일

	void (*pExecFn)(void* void_ptr);		// 명령어 처리를 위한 함수 포인터

	MENU_COMMAND()
	{
		style = CS_NONE;
		pExecFn = NULL;
	}
};

class C_VS_UI_MENU : public Window
{
private:
	void DrawBackGround();
	void DrawSelectBox();
	void DrawInputCommand();

	int CheckInButton(int _x, int _y);	// 마우스 포인터가 어느 커맨드를 가리키고 있는지 처리

protected:
	typedef std::vector<MENU_COMMAND*> VEC_MENU_COMMAND;
	typedef std::map<id_t,C_VS_UI_MENU*> MAP_SUB_MENU; 

	void SetPrintInfo(const char* font, PrintInfo* gInfo);

	virtual void GenerateSubMenu(int getsubcommand);
	virtual bool LButtonDown(int cmd);

public:
	enum MENU_VISUAL_STYLE		// 보여질 윈도우의 스타일 목록
	{
		MVS_MS,					// MS 윈도우 스타일
		MVS_BL					// 블랙 컨셉
	};
	enum MENU_POSITION			// 메뉴가 보여지게 될 위치
	{
		MP_RIGHTBOTTOM,			// 우측 아래
		MP_RIGHTTOP,			// 우측 위
		MP_LEFTBOTTOM,			// 좌측 아래
		MP_LEFTTOP				// 좌측 위
	};

public:
	C_VS_UI_MENU(int _x = 0, int _y = 0, MENU_VISUAL_STYLE style = MVS_MS);
	virtual ~C_VS_UI_MENU();

	// 기본적으로 상위 클래스를 상속 받아 실행 하는 함수들
	virtual void Start();
	virtual void Finish();
	virtual void Show();
	virtual void Process();
	virtual bool MouseControl(UINT message, int _x, int _y);
	virtual void KeyboardControl(UINT message, UINT key, long extra);
	virtual bool IsPixel(int _x, int _y)
	{ 
		if (!Running()) return false;
		return IsInRect(_x, _y);
	}

	// 외부 인터페이스 함수
	virtual void SetPosition(int _x, int _y, MENU_POSITION pos = MP_RIGHTBOTTOM);
	virtual void AddCommand(const char* pszCommand, id_t id, void (*pExecFn)(void* void_ptr) = NULL, int style = MENU_COMMAND::CS_NONE);
	virtual void SetSubMenu(id_t id, C_VS_UI_MENU*& pSubMenu);
	virtual void SetMenuStyle(MENU_VISUAL_STYLE style) { m_menu_style = style; }

private:
	PrintInfo*				m_menu_print_info;		// 커맨드 폰트를 정의
	MENU_VISUAL_STYLE		m_menu_style;			// 윈도우 스타일

protected:
	VEC_MENU_COMMAND		m_v_main_command;		// 메인 메뉴의 커맨드
	MAP_SUB_MENU			m_map_sub_menu;			// 서브 메뉴의 커맨드

	bool					m_sub_menu_open;		// 서브 메뉴가 열려 있는지 체크
	bool					m_command_handling;		// 명령어 처리가 되었는지 체크

	int						m_command_count;		// 전체 명령어 개수
	int						m_current_command;		// 현재 선택 되거나 마우스 오버가 되어진 커맨드
	int						m_max_string_lengh;		// 가장긴 커맨드의 픽셀 길이
	int						m_select_box_height;	// 선택 박스의 높이
	int						m_command_pos_x_gap;	// 명령어 위치가 x 좌표로 부터 갖는 격차
	int						m_command_pos_y_gap;	// 명령어 위치가 y 좌표로 부터 갖는 격차

	CRect					m_menu_box;				// 그려질 메뉴창의 크기
	CRect					m_select_box;			// 커맨드의 선택 영역창 크기

	C_VS_UI_MENU*			m_current_sub_menu;		// 현재 보여지고 있는 서브 메뉴창
};


//////////////////////////////////////////////////////////////////////
// class C_VS_UI_LISTVIEW												
// 작성일 : 09.06.01
// 작성자 : 이 권 확
// 기  능 : 리스트 뷰에 대한 객체이다.
//////////////////////////////////////////////////////////////////////

struct LTVCOLUMNHEADER : public Rect
{
	std::string text;	// 칼럼헤더에 표시 해줄 문자열
	int index_num;		// 해당 칼럼 인덱스 번호
};

struct LTVITEM
{
	std::map<int,std::string> text;		// 아이템에 들어갈 문자열, 앞쪽 인덱스는 컬럼 번호
	bool select;						// 아이템이 선택 되어진 상태인지..
	int sort_column_index;				// 정렬을 위한 현재 클릭된 칼럼 인덱스

	LTVITEM()
	{
		text.clear();
		select = false;
		sort_column_index = 0;
	}
};

struct SCROLL_DATA
{
	CRect background_area;
	CRect up_button_area;
	CRect dw_button_area;
	CRect bar_button_area;
	SIZE bar_button_size;

	bool background_push;
	bool up_button_push;
	bool dw_button_push;
	bool bar_button_push;

	int bar_pos;			// 막대의 위치
	int back_pos_y;			// 마우스 이벤트를 받는 스크롤 배경의 y 좌표
	int back_height;		// 상하 버튼을 제외한 높이

	DWORD old_time;			// 버튼이 눌렸을때 바로 처리가 되면 안되므로..

	SCROLL_DATA()
	{
		memset(&background_area,0,sizeof CRect);
		memset(&up_button_area,0,sizeof CRect);
		memset(&dw_button_area,0,sizeof CRect);
		memset(&bar_button_area,0,sizeof CRect);
		memset(&bar_button_size,0, sizeof SIZE);

		background_push = false;
		up_button_push = false;
		dw_button_push = false;
		bar_button_push = false;

		bar_pos = 0;
		back_pos_y = 0;
		back_height = 0;
	}

	void PushInit()
	{
		background_push = false;
		up_button_push = false;
		dw_button_push = false;
		bar_button_push = false;
	}
};

bool CompareTextValueAs(LTVITEM* lA, LTVITEM* lB);
bool CompareTextValueDs(LTVITEM* lA, LTVITEM* lB);

class C_VS_UI_LISTVIEW : public Window
{
private:
	int CheckInColumn(UINT message, int _x, int _y);
	int CheckInItem(UINT message, int _x, int _y);
	void SetSelectItem( bool state = true, int index = -1);		// 디폴트 인자 값인 -1은 전부를 얘기한다.
	void SearchClickColumnHeader(int index);
	void SortColumnItem();
	void SetPrintInfo(const char* font, PrintInfo& crtprintinfo);

protected:
	typedef std::vector<LTVITEM*> VEC_LTVITEM;

	virtual void DrawListView();
	virtual void DrawScrollBar();

	virtual void ShiftScrollBar(int pos);
	virtual void UpdateVerticalScrollBar(bool bar = false, int bar_pos = 0);
	virtual void UpdateHorizonScrollBar(bool bar = false, int bar_pos = 0);

	// 스크롤 생성 & 처리
	virtual void GenerateVerticalScroll();
	virtual void GenerateHorizonScorll();
	virtual bool ProcessVerticalScroll(UINT message, int _x, int _y);
	virtual bool ProcessHorizonScroll(UINT message, int _x, int _y);

	// 선택된 칼럼의 인덱스를 처리한다.
	virtual void ClickColumnProc(int index); 

	// 선택된 아이템을 처리 - 상속을 받아 재정의 하여 사용 하도록 한다.
	virtual void LbclkItemProc(LTVITEM* item) {}		// 왼쪽 클릭된 아이템을 처리한다..
	virtual void LdbclkItemProc(LTVITEM* item) {}		// 왼쪽 더블 클릭된 아이템을 처리한다..
	virtual void RbclkItemProc(LTVITEM* item) {}		// 오른쪽 버튼으로 클릭된 아이템을 처리한다..

public:
	enum SELECT_TYPE	// 아이템 선택 유형
	{
		ST_SINGLE,		// 단일 선택
		ST_MULTI		// 다중 선택
	};
	enum COLUMN_HEADER_ALIGN
	{
		CHACENTER = 0x01,
		CHALEFT = 0x02,
		CHARIGHT = 0x04,
		CHATOP = 0x08,
		CHABOTTOM = 0x10,
	};
	enum LVT_STYLE_FLAG
	{
		LSFBACKGROUNDNOTTRANS = 0x01,	// 배경 투명
		LSFHEADER3DVIEW = 0x02,			// 컬럼 헤더 입체화
		LSFOUTLINE = 0x04,				// 아웃 라인 생성
		LSFFRAMELINE = 0x08,			// 내부 프레임 라인 생성
		LSFITEMSELECTBOX = 0x10,		// 아이템 선택박스
		LSFSORT = 0x20,					// 정렬 여부
	};

public:
	C_VS_UI_LISTVIEW();
	C_VS_UI_LISTVIEW(Window* pParent, int _x, int _y, int _w, int _h, DWORD style = LSFBACKGROUNDNOTTRANS, SELECT_TYPE type = ST_SINGLE);
	virtual ~C_VS_UI_LISTVIEW();

	// 기본적으로 상위 클래스를 상속 받아 실행 하는 함수들
	virtual void Start();
	virtual void Finish();
	virtual void Show();
	virtual void Process();
	virtual bool IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	virtual bool MouseControl(UINT message, int _x, int _y);	

	// 외부 인터페이스 함수
	bool InitColumnHeader(int num, DWORD align = CHALEFT | CHATOP);
	void SetColumnHeader(const char* pText, int index, int width);
	void InsertColumnItem(const char* pText, int item_num, int column_num);
	void DeleteColumnItem(int index);
	void DeleteColumnItem(const char* pText);
	void DeleteToSelectItem();
	void DeleteAllItem();
	
	int GetItemSize() { return m_v_item.size(); }
	int GetItemViewSize() { return m_view_item_count; }
	int GetBeginItemIndex() { return m_view_begin_item_index; }
	int GetSelectItemSize();
	int GetItemIndexByName(const char* pText);
	const char* GetItemNameBytSelect(int column_index);
	const char* GetItemNameByIndex(int index, int column);

protected:
	PrintInfo*				m_listview_print_info;		// 리스트 뷰에 사용될 폰트 정보
	Window*					m_pParent;					// 부모 클래스
	LTVCOLUMNHEADER*		m_pColumn;					// 칼럼

	VEC_LTVITEM				m_v_item;					// 아이템 하나에 대한 구조체
	SELECT_TYPE				m_select_type;				// 아이템 선택 유형 - 단일 선택 or 다중 선택
	SCROLL_DATA				m_scroll_data;				// 스크롤에 대한 데이터

	char					m_sort_state;				// 정렬 상태 - 오름차순인지 내림차순인지
	bool					m_vertical_scroll;			// 세로 스크롤 바가 생성 되었는지 체크
	bool					m_horizon_scroll;			// 가로 스크롤 바가 생성 되었는지 체크
	bool					m_generate_scroll;			// 스크롤 생성

	int						m_column_count;				// 칼럼의 개수
	int						m_click_column;				// 클릭된 칼럼 번호
	int						m_view_begin_item_index;	// 처음에 보여지는 아이템의 시작 인덱스
	int						m_view_item_count;			// 보여지는 아이템 개수
	
	int						m_item_height;				// 각 아이템의 높이
	int						m_view_max_height;			// 리스트 뷰의 보여지지 않는 부분까지의 전체 높이
	int						m_text_x_space;				// 문자열 x 공간
	int						m_text_y_space;				// 문자열 y 공간
	
	CPoint					m_old_mouse_pos;			// 마우스 포지션
	CRect					m_background;				// 리스트 뷰의 전체 크기
	CRect					m_in_pos;					// 내부 포지션?
	DWORD					m_style_flag;				// 리스트 뷰 스타일
	DWORD					m_header_align;				// 칼럼 헤더 정렬
};


//////////////////////////////////////////////////////////////////////
// class C_VS_UI_CHECKBOX												
// 작성일 : 09.06.25
// 작성자 : 이 권 확
// 기  능 : 체크 박스
//////////////////////////////////////////////////////////////////////

class C_VS_UI_CHECKBOX : public Window
{
public:
	C_VS_UI_CHECKBOX();
	C_VS_UI_CHECKBOX(Window* pParent, int _x, int _y);
	virtual ~C_VS_UI_CHECKBOX();

	void Start();
	void Finish();
	void Process();
	void Show();
	bool MouseControl(UINT message, int _x, int _y);	
	bool IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x,_y); }

	void SetParent(Window* pParent);
	void SetPos(int _x, int _y) { x = _x; y = _y; w = 10; h = 10; }
	void SetCheck(bool check) { m_check = check; }
	bool GetCheck() const { return m_check; }

private:
	Window*			m_parent;
	bool			m_check;

	int*			m_parent_pos_x;
	int*			m_parent_pos_y;
	int*			m_parent_size_cx;
	int*			m_parent_size_cy;

	DWORD			m_back_color;
};
