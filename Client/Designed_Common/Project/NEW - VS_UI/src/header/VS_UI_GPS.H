/*-----------------------------------------------------------------------------

	VS_UI_GameCommon.h

	Game common interface.

	Common:
				Chatting interface
				NPC
				Exchange

	2000.8.11. KJTINC

-----------------------------------------------------------------------------*/


#ifndef	__VS_UI_GPS_H__
#define	__VS_UI_GPS_H__

#include "VS_UI_Base.h"
#include "VS_UI_Util.h"
#include "VS_UI_description.h"
#include "VS_UI_ExtraDialog.h"

#include "ClientFunction.h"
#include "MZone.h"

#include <vector>	//by larosel

#include "TickTimer.h"

//#define SLAYER_FONT_COLOR				RGB(186, 189, 191)//RGB(20, 70, 0)	// by larosel
//#define VAMPIRE_FONT_COLOR				RGB(186, 189, 191)//RGB(255, 255, 255)

#define ITEM_DISABLE_COLOR_SET			40

#include "MGpsTable.h"

#if __CONTENTS(__TITLE_UI_RENWEAL)
	#include "VS_UI_NewTitle.h"
#else	//__TITLE_UI_RENWEAL
	#include "VS_UI_Title.h"
#endif //__TITLE_UI_RENWEAL

//-----------------------------------------------------------------------------
// class C_VS_UI_GPS
//
// vampire/slayer GPS class
//-----------------------------------------------------------------------------

//#include "VS_UI_GameSlayer.h"
class C_VS_UI_SLAYER_QUICKITEM;
class C_VS_UI_OUSTERS_QUICKITEM;

extern bool gbl_mine_progress;

enum GPS_MAP_SPK_INDEX	//
{
	GPS_MAP,
};

struct S_GPS_BASE_INFO	//각 클래스에서 가지고 있는 기본 정보로 위치와 spk인덱스 그리고 숨기고 보이는 값을 가지고 있다.
{
	int iX;
	int iY;
	int iTopmust;		//보이는 우선순위를 정해주려 했는데 지금은 사용하지 않는다. 추후에 사용 될까?
	int iWidht;
	int iHeight;
	C_SPRITE_PACK *spk; 
	WORD wSpkIndex;
	bool bHide;	
};

struct S_GPS_GAME_MAP_INFO	//게임 상의 맵 ID와 크기를 설정할 자료형
{
	S_GPS_GAME_MAP_INFO() : wID(0), wMapSize_Width(0), wMapSize_Height(0) {}

	WORD		wID;
	WORD		wMapSize_Width;
	WORD		wMapSize_Height;
	void Release()
	{
		wID					= 0;
		wMapSize_Width		= 0;
		wMapSize_Height		= 0;
	}
};

struct S_GPS_NPC_INFO		//NPC의 정보를 설정할 자료형
{
	S_GPS_NPC_INFO() : wID(0), wPos_X(0), wPos_Y(0), wSpkIndex(0) {}

	WORD					wID;
	string					pchName;
	WORD					wPos_X;
	WORD					wPos_Y;
	string					pchXY;
	string					chExplanation; // NPC 설명
	WORD					wSpkIndex;

	S_GPS_GAME_MAP_INFO	S_Gps_Game_Map_Info;

	void Release()
	{
		wID				= 0;
		wPos_X			= 0;
		wPos_Y			= 0;
		wSpkIndex		= 0;

		pchXY.clear();
		pchXY.resize(0);

		chExplanation.clear(); // NPC 설명
		chExplanation.resize(0);

		pchName.clear();
		pchName.resize(0);

		S_Gps_Game_Map_Info.Release();
	}
};

struct S_GPS_POTAL_INFO		//포탈 정보를 설정할 자료형
{
	S_GPS_POTAL_INFO() : wID(0), wPos_X(0), wPos_Y(0), wSpkIndex(0) {}

	WORD			wID;
	WORD			wPos_X;
	WORD			wPos_Y;
	string			chExplanation; // 포탈 설명
	WORD			wSpkIndex;

	S_GPS_GAME_MAP_INFO	S_Gps_Game_Map_Info;

	void Release()
	{
		wID					= 0;
		wPos_X				= 0;
		wPos_Y				= 0;
		wSpkIndex			= 0;

		chExplanation.clear(); // 포탈 설명
		chExplanation.resize(0); // 포탈 설명
		S_Gps_Game_Map_Info.Release();
	}
};

struct S_GPS_MONSTER_INFO
{
	S_GPS_MONSTER_INFO() : wZoneID(0), wDunGeonID(0), wMonsterLv1(0), wMonsterLv2(0), wExplanationCount(0) {}

	string			pchFieldName;
	string			pchMonsterLv;
	WORD			wZoneID;			//월드맵 상에 보여지는 필드
	WORD			wDunGeonID;			//던전 ID
	WORD			wMonsterLv1;
	WORD			wMonsterLv2;
	string			pchExplanation[20];	//던전이 20층 이상 있을까?
	WORD			wExplanationCount;	//설명 개수

	void	Release()
	{
		pchFieldName.clear();
		pchFieldName.resize(0);
		pchMonsterLv.clear();
		pchMonsterLv.resize(0);
		wZoneID						= 0;			//월드맵 상에 보여지는 필드
		wDunGeonID					= 0;			//던전 ID
		wMonsterLv1					= 0;
		wMonsterLv2					= 0;
		wExplanationCount			= 0;	//설명 개수

		for(int i = 0; i < 20; i++)
		{
			pchExplanation[i].clear();
			pchExplanation[i].resize(0);
		}
	}
};

struct S_GPS_ZONE_NAME_ID
{
	S_GPS_ZONE_NAME_ID() : wID(0) {}

	string		pchName;
	WORD		wID;

	void Release()
	{
		wID			= 0;
		pchName.clear();
		pchName.resize(0);
	}
};


struct S_QUESTICON_INFO
{
	S_QUESTICON_INFO() : wQuestID(0),  wPos_X(0), wPos_Y(0), wIndex(0), bQuestShow(true), btStatus(0){}

	WORD		wQuestID;

	WORD		wPos_X;
	WORD		wPos_Y;
	WORD		wIndex;		// 퀘스트가 대기중인지 실행 중인지 설정된 값에서 더해진 Index 값이다.

	bool		bQuestShow;

	BYTE		btStatus;

	std::string	strQuestName_WorldView;
	std::string	strQuestExplanation_WorldView;
	std::string	strQuestName_GpsView;
	std::string	strQuestExplanation_GpsView;
	std::string	strQuestCompensation;

	S_GPS_GAME_MAP_INFO	S_Gps_Game_Map_Info;

	void Release()
	{
		wQuestID				= 0;

		wPos_X					= 0;
		wPos_Y					= 0;
		wIndex					= 0;
		bQuestShow				= true;
		btStatus				= 0;

		strQuestName_WorldView.clear();
		strQuestExplanation_WorldView.clear();
		strQuestName_GpsView.clear();
		strQuestExplanation_GpsView.clear();
		strQuestCompensation.clear();

		strQuestName_WorldView.resize(0);
		strQuestExplanation_WorldView.resize(0);
		strQuestName_GpsView.resize(0);
		strQuestExplanation_GpsView.resize(0);
		strQuestCompensation.resize(0);

		S_Gps_Game_Map_Info.Release();
	}
};

#if __CONTENTS(__GPS_ADD)

class	C_GPS_BOX				
{
public:
	C_GPS_BOX();
	~C_GPS_BOX();

	void	SetBox(S_GPS_BASE_INFO Gps_Base_Info);
	void	BoxView();

private:
	S_GPS_BASE_INFO m_Gps_Base_Info;
};


class	C_GPS_BUTTON			//GPS_MANAGER에서만 사용 하며 종료 버튼과 리스트 보기 버튼으로 사용 된다.
{
public:
	C_GPS_BUTTON();
	~C_GPS_BUTTON();

	void	SetButton(S_GPS_BASE_INFO Gps_Base_Info);
	void	ButtonView();
	bool	ButtonClick(int iPos_X, int iPos_Y, bool bClick, int iSpkIndex1, int iSpkIndex2);
	
private:
	S_GPS_BASE_INFO m_Gps_Base_Info;
	WORD	m_wButtonID;
};


class	C_GPS_MAP
{
public:
	C_GPS_MAP();
	~C_GPS_MAP();
	void	SetMap(S_GPS_BASE_INFO Gps_Base_Info, int iZoneID = 0, C_SPRITE_PACK *IconSpk = NULL);
	void	MapView();											//GPS안에 맵을 보여준다.
	void	IconView();											//ICON을 보여준다.
	void	ZoneNameTextView();
	void	UserPosTextView();
	void	GameMapToGpsMapNpcPos(S_GPS_NPC_INFO &Gps_Npc_Info);		//월드맵과 GPS의 크기를 계산해서 NPC나 던젼의 위치를 찾는다. 
	void	GameMapToGpsMapPotalPos(S_GPS_POTAL_INFO &Gps_Potal_Info);	//월드맵과 GPS의 크기를 계산해서 NPC나 던젼의 위치를 찾는다.

#if __CONTENTS(__QUEST_RENEWAL)	//GPS	GameMapToGpsMapQuestPos() 선언	
	void	GameMapToGpsMapQuestPos(S_QUESTICON_INFO &Gps_Quest_Info);	//월드맵과 GPS의 크기를 계산해서 Quest의 위치를 찾는다. 
	void	SetQuestInfo(vector<S_QUESTICON_INFO>	Gps_Quest_Info);
	void	AddQuestInfo(S_QUESTICON_INFO QuestInfo);
	void	ModifyQuestStatus(BYTE btStatus, DWORD dwQuestID);
	void	GpsQuestInfoDelete();

	vector<S_QUESTICON_INFO>	GetQuestInfo();

	bool	GetWorldMapZoneIDExistenceFind(DWORD dwZoneID);					//월드 맵 ID와 일치 하는지 검사해서 결과를 넘겨준다.
#endif	// __QUEST_RENEWAL

	void	GpsNpcInfoInit();									//NPC의 정보들을 설정 해준다.
	void	GpsPortalInfoInit();									//포탈의 정보들을 설정 해준다.
	void	GpsZoneInfoInit();									//맵에 대한 정보(월드맵에서 클릭 되거나 리스트에서 클릭 될때에 보여지는 맵 정보)설정해 준다.
	void	GpsMonsterInfoInit();								//리스트에 항목으로 사용될 몬스터 정보 목록
	void	SetGpsUserInfo(S_GPS_NPC_INFO Gps_User_Info);		//사용자의 위치를 설정 해준다.
	void	SetGpsPartyInfo();								//파티 정보를 설정해 준다.

#if __CONTENTS(__QUEST_RENEWAL)
	bool	GetIconRollOverExplanationView(int iPox_X = 0, int iPox_Y = 0, bool bButtonClcik	= false);	//마우스가 롤 오버되면 설명 보여준다.
#else
	bool	GetIconRollOverExplanationView(int iPox_X, int iPox_Y);	//마우스가 롤 오버되면 설명 보여준다.
#endif

#if __CONTENTS(__MOVE_ZONE)
	bool	SetMoveZone(int iPox_X, int iPox_Y);
#endif //__MOVE_ZONE
	
	void	SetSelectMapIndex(int iZoneID);						//ZoneID로 spk에서 사용될 Index를 구해준다. 
	int		GetSelectMapIndex();
	S_GPS_MONSTER_INFO	GetMonsterInfo();

private:
	S_GPS_BASE_INFO				m_Gps_Base_Info;

	S_GPS_NPC_INFO				m_Gps_User_Info;
	vector<S_GPS_NPC_INFO>		m_Gps_Party_Info;

	vector<S_GPS_NPC_INFO>		m_Gps_Npc_Info;
	vector<S_GPS_POTAL_INFO>	m_Gps_Potal_Info;
	vector<S_GPS_ZONE_NAME_ID>	m_ZoneInfo;			
	int							m_iZoneID;

	C_SPRITE_PACK				*m_pIconSpk;
	S_DEFAULT_HELP_STRING		m_Npc_string;

	int							m_iUserNpcGameMap_X;
	int							m_iUserNpcGameMap_Y;
	int							m_iSelectMapIndex;

	bool						m_bInfoInit;
	vector<S_GPS_MONSTER_INFO>	m_MonsterInfo;
	char						m_chXY[20];

	vector<S_QUESTICON_INFO>	m_Gps_Quest_Info;
	vector<S_QUESTICON_INFO>	m_Gps_Quest_Info_temp;
	std::string					m_strTemp;
};


class	C_GPS_SCROLL
{
public:
	C_GPS_SCROLL();
	~C_GPS_SCROLL();
	void	ScrollView();
	void	ScrollUpButton();
	void	ScrollDownButton();

	void	SetScroll(S_GPS_BASE_INFO Track_Info, S_GPS_BASE_INFO UpButton_Info, S_GPS_BASE_INFO DownButton_Info, S_GPS_BASE_INFO Bar_Info, int iRange);
	void	TrackBarMove();

	bool	GetScrollUpButtonRollOver(int iPos_X = 0, int iPos_Y = 0, bool bClick = false, bool bWheel = false);
	bool	GetScrollDownButtonRollOver(int iPos_X = 0, int iPos_Y = 0, bool bClick = false, bool bWheel = false);
	int		GetScrollCount();
	void	UpButtonShow(bool bShow);
	void	DownButtonShow(bool bShow);
private:
	S_GPS_BASE_INFO m_Track_Info;
	S_GPS_BASE_INFO m_UpButton_Info;
	S_GPS_BASE_INFO m_DownButton_Info;
	S_GPS_BASE_INFO m_Bar_Info;

	int				m_iRange;
	int				m_iTrackCount;

	float			m_fTrackBarMoveSize;		//버튼 클릭시 이동할 수 있는 트랙바의 크기
	float			m_fTrackBartmp_Y;
	bool			m_bUpButtonShow;
	bool			m_bDownButtonShow;
};



class	C_GPS_LIST : public C_GPS_SCROLL
{
public:
	C_GPS_LIST();
	~C_GPS_LIST();
	void	ListView();
	void	SetList(S_GPS_BASE_INFO Gps_Base_Info);					//리스트의 시작 위치 SKP등을 설정 한다.
	void	ItemTextView();											//아이템 목록을 보여줄 함수
	int		GetItemCount();											//아이템의 개수를 넘겨줄 함수
	bool	ItemClick(int iPos_X, int iPos_Y, bool bMouseClick);	//아이템이 클릭 되었는지 확이해주는 함수
	void	SetListItem();											//아이템 정보를 초기화 해줄 함수
	int		GetZoneID();											//클릭된 아이템의 ZoneID를 넘겨줄 함수

	S_GPS_MONSTER_INFO	GetSelectListItem();						//클릭된 아이템의 몬스터 정보를 넘겨줄 함수

private:
	S_GPS_BASE_INFO m_Gps_Base_Info;
	
	int		m_iLinePos_X;
	int		m_iLinePos_Y;
	int		m_iListHeight;

	int		m_iItemCount;											//리스트에 입력된 목록의 갯수
	bool	m_bfirst;
	bool	m_bMouseClick;
	int		m_iListFocus;											//리스트 항목의 포커스(마우스 롤 오버인 상태)
	int		m_iListSelectFocus;										//선택된 리스트 항목의 포커스(마우스 클릭된 상태)
	int		m_iZoneID;

	struct S_LIST_ITEM_POS											//하나의 항목이 가지고 있어야할 정보
	{
		int		iItemPos_X;		//목록이 보여질 위치
		int		iItemPos_Y;		//목록이 보여질 위치
		int		iListHeight;
		int		iListWidht;
	};

	vector<S_GPS_MONSTER_INFO>	m_ListItem;
	S_GPS_MONSTER_INFO			m_SelectListItem;					//리턴값
	S_LIST_ITEM_POS				m_Lip[6];							//보여지는 리스트의 항목 개수는 6개이다.
	vector<std::string>			m_ItemName;							//리스트에 입력될 목록의 이름
	int m_iItemViewStartIndex;										//보여질 리스트 항목의 시작 인덱스
};



class	C_LISTMANAGER : public C_GPS_LIST, public C_GPS_BOX
{
public:
	C_LISTMANAGER();
	~C_LISTMANAGER();
	void	ListManagerView(bool bSurfaceLock);						
	void	SetListManager(S_GPS_BASE_INFO Gps_Base_Info);
	void	SetListShow(bool bListShow);							//리스트를 보여줄 것인지 설정 한다.
	bool	GetListShow();
private:
	S_GPS_BASE_INFO m_Gps_Base_Info;
	S_GPS_BASE_INFO m_Gps_ListViewButton_Info;
	bool			m_bListShow;
};


class	C_GPSMANAGER: public C_GPS_BOX, public C_GPS_MAP
{
public:
	C_GPSMANAGER();
	~C_GPSMANAGER();
	void	SetGPSManager(S_GPS_BASE_INFO Box_Base_Info, C_SPRITE_PACK *mapspk, int iMapIndex = 0, int iZoneID = 0);
	void	GPSManagerView(bool bSurfaceLock);
	void	SetMonsterLv(int iMinLv = 0, int iMaxLv = 0);
	void	SetMonsterLv(string strMonsterLv);
	void	MonsterLvUint(int iMinLv = 0, int iMaxLv = 0);			//몬스터 레벨을 단위별로 구한다.
	void	MonsterLvView();

public:
	C_GPS_BUTTON		m_Gps_Close_Button;
	C_GPS_BUTTON		m_Gps_ListView_Button;

private:
	S_GPS_BASE_INFO		m_Gps_Base_Info;
	S_GPS_MONSTER_INFO	m_SelectListItem;

	char				m_chMonster[255];
	char				m_chMonsterLv[255];
	bool				bMonsterLvView;
};




class C_VS_UI_GPS : public Window, public Exec, public ButtonVisual
{
private:
	CSpriteSurface			* m_p_gps_surface;
//	ButtonGroup *			m_pC_button_group;
	S_GPS_BASE_INFO			m_GPS_Base_Info;
	S_GPS_BASE_INFO			m_GpsManager_Base_Info;
	S_GPS_BASE_INFO			m_IconManager_Base_Info;
	S_GPS_BASE_INFO			m_ListManager_Base_Info;

	C_SPRITE_PACK			*m_pmapspk;
	int						m_iGpsMapIndex;
		
	bool					m_bl_refresh;

	int						m_mouse_move_x;	//GPS에서는 마우스 이동된 포인트 좌표 값으로 사용될 변수
	int						m_mouse_move_y;
	int						m_mouse_down_x;
	int						m_mouse_down_y;
	int						m_mouse_move_back_x;
	int						m_mouse_move_back_y;
	int						m_mouse_up_x;
	int						m_mouse_up_y;
	bool					m_mouse_move;

	bool					m_bStartGps;

	//timer
	DWORD						m_dw_gps_prev_tickcount;
	DWORD						m_dw_gps_millisec;

	bool						m_bgpsDisableZone;		// 미니맵을 출력하지 않는 맵이다.
	int							m_iZoneID;				// 맵 ID

	S_GPS_MONSTER_INFO			m_SelectListItem;

	bool						m_bGpsShow;
	bool						m_bGpsMove;
	bool						TimerGPS();

	enum EXEC_ID
	{
		ALPHA_ID,
		PUSHPIN_ID,
	};

	enum MOUSE_MOVE_SIZE
	{
		MOUSE_MOVE_POS	= 800,
	};

public:
	C_GPSMANAGER			m_C_GpsManager;
	C_LISTMANAGER			m_C_ListManager;

public:
	void SetBlock(int x,int y);
	C_VS_UI_GPS();
	~C_VS_UI_GPS();

// 	static	int GetZoneNumber(int zone_id);
// 	static	char * GetZoneName(int zone_id);

	void	SetFlagArea(POINT pt);
	void	SetZone(int iZoneID);

	void	TogglePushPin() { Run(PUSHPIN_ID); }

	void	UnacquireMouseFocus() 
	{
//		m_pC_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
//		m_pC_button_group->CancelPushState();
	}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	MouseControlExtra(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
//	void	Process();
	void	Start();
//	void	Finish();
	void	NonActive();
	void	Active(int iMapIndex	= 0, int iMapID = 0);

	void	ResetBoardPosition();

	void	SetObjectPos();	//객체의 좌표를 설정 분리한다.

	void	DesableZone(bool bZone);

	bool	GetGpsShow();
	void	GpsInfoReView();
};
#endif //__GPS_ADD


#endif