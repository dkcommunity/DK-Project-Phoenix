/*-----------------------------------------------------------------------------

	VS_UI_WorldMap.h

	World Map Interface.

	2007.01.15 chyaya

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_WORLDMAP_H__
#define __VS_UI_WORLDMAP_H____

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Dialog.h"
#include "VS_UI_DESC.h"

#include "MZoneTable.h"

#if __CONTENTS(__GPS_ADD)
#include "VS_UI_GPS.H"
#endif	//__GPS_ADD

struct WORLDMAP_UI_INFO
{
public:
	typedef std::vector<MINIMAP_PORTAL>	MINIMAP_PORTAL_VEC;

public:
	void SetPortal(CRect& rect, TYPE_ZONEID zoneid) {
		MINIMAP_PORTAL mp;
		mp.position = rect;
		mp.zoneID	= zoneid;
		portalVec.push_back(mp);
	}

public:
	MINIMAP_PORTAL_VEC	portalVec;
	CSize				mapSize;
};




/*
struct WORLDMAP_SELECT_INFO
{

};
*/
//-----------------------------------------------------------------------------
// C_VS_UI_WORLD_MAP
//
//-----------------------------------------------------------------------------
class C_VS_UI_WORLD_MAP : public Window, public Exec, public ButtonVisual
{
public:
	enum { WINDOW_APPEAR_SOUND = SOUND_ITEM_MOVE_SCROLL };

	typedef std::map<TYPE_ZONEID, WORLDMAP_UI_INFO>		WORLDMAP_UI_INFO_MAP;
	
private:
	enum EXEC_ID
	{
		WORLDMAP_CLOSE,
#if __CONTENTS(__GPS_ADD)
		GPS_VIEW,
#endif // __GPS_ADD
	};

#if __CONTENTS(__GPS_ADD)
	struct S_SELECT_INFO
	{
		CRect	rt;
		int		iSpkIndex;
		int		iZoneID;
	};
#endif // __GPS_ADD

	enum WORLDMAP_SPK_INDEX
	{
		WORLDMAP_HELEA, 
		WORLDMAP_CLOSE_BUTTON,
		WORLDMAP_CLOSE_BUTTON_HILIGHTED,
		WORLDMAP_CLOSE_BUTTON_PUSHED,
	};

	enum WORLDMAPICON_SPK_INDEX
	{
		WORLDMAPICON_SLAYER,
		WORLDMAPICON_VAMPIRE,
		WORLDMAPICON_OUSTERS,
		WORLDMAPICON_OTHERS,
		WORLDMAPICON_QUEST_WAIT,
		WORLDMAPICON_QUEST_RUN,
	};

	enum SELECTMAP_INDEX
	{
		MAP_INDEX	= 4,	// spk파일에서 선택 그림은 WORLDMAP_CLOSE_BUTTON_PUSHED가 마지막값 그러므로 그보다 하나큰값
	};

	ButtonGroup*				m_pC_button_group;
	
	C_SPRITE_PACK*				m_pC_world_map_spk;
	C_SPRITE_PACK*				m_pC_world_map_icon_spk;

	static WORLDMAP_UI_INFO_MAP	s_WorldMapUIInfoMap;
	static int					s_NowZoneID;

	CPoint						m_ptPlayerPosition;

	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_millisec;

	int							m_DrawColor;
	int							m_DrawColorAdd;

	CPoint						m_ptStartOffset;

	S_DEFAULT_HELP_STRING		m_HelpString;

	char						m_PartyNameBuf[20];
	char						m_PartyDescBuf[50];
	
	int							m_iGpsButton_Pos_X;
	int							m_iGpsButton_Pos_y;
	int							m_iGpsButton_Width;
	int							m_iGpsButton_Hight;

#if __CONTENTS(__GPS_ADD)
	int							m_iSelectMapIndex_Click;
	int							m_iSelectMapIndex;
	S_SELECT_INFO				m_S_SelectInfo[WORLDMAP_SELECT_TIMOR_SE+1];
	int							m_iZoneID;
	int							m_iBackZoneID;					//맵 밖을 클릭 할때에 
	int							m_iBackSelectMapIndex_Click;	//맵 밖을 클릭 할때에 죤 ID가 91(브랑코 NE가 된다 이것을 방지하기 위해 만든 변수
#endif //__GPS_ADD

#if __CONTENTS(__QUEST_RENEWAL)	// C_VS_UI_WORLD_MAP		m_QuestIcon_Info;
	S_DEFAULT_HELP_STRING		m_Quest_String;

	int							m_iQuestPos_X;
	int							m_iQuestPos_Y;

	vector<S_QUESTICON_INFO>	m_QuestIcon_Info;

	std::string					m_strPayZone;
#endif // __QUEST_RENEWAL

public:
	C_VS_UI_WORLD_MAP();
	~C_VS_UI_WORLD_MAP();

	void	Start();
	void	Finish();
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	void	Show();

	void	DrawPortal(CSpriteSurface *pDescSurface,
					   TYPE_ZONEID zoneID, const CSize& sizeMap,
					   const CRect& rcWorldMap, const CPoint& ptPotalPos);

	void	DrawCharacter(CSpriteSurface *pDescSurface,
						  TYPE_ZONEID zoneID, const CSize& sizeMap, const CRect& rcWorldMap,
						  const CPoint& ptCharacterPos, int nIconIndex);

	bool	IsPixel(int _x, int _y);

	void	SetPlayerXY(int px, int py);
	
	bool	ColorTimer();
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	ShowMouseDescription(int _x, int _y);

	static WORLDMAP_UI_INFO&	GetWorldMapUIInfo(int mapID) { return s_WorldMapUIInfoMap[mapID]; }
	static void					SetNowZoneID(int zoneID);

#if __CONTENTS(__QUEST_RENEWAL)	// C_VS_UI_WORLD_MAP		QuestIconShow() 선언;
	void	QuestIconShow();
	void	GameMapToSelectMap(int &iPos_X, int &iPos_Y, S_QUESTICON_INFO Quest_Info);
	void	QuestDescription(int iMouse_X, int iMouse_Y);

	void	GpsToBringQuestInfo();
#endif	//  __QUEST_RENEWAL
};


#endif	//__VS_UI_WORLDMAP_H__