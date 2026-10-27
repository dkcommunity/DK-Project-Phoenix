#ifndef __VS_UI_INDOUNG_NPC_DIALOG_H__
#define __VS_UI_INDOUNG_NPC_DIALOG_H__


#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Dialog.h"
#include "VS_UI_DESC.h"

enum ENUMTYPE_INDUONSELECT
{
	COMPENSATION_DLG,		//보상 다이얼로그
	ENTRANCE_DLG,			//입장 다이얼로그
};

struct S_INDOUN_DLG_INFO
{
	WORD							wButtonSize;	//버튼의 갯수
	WORD							wObjectID;
	WORD							wNpcID;

	string							strDlgTitleName;
	string							strNpcName;
	string							strDesc;		//설명

	string							strExp;
	string							strRankExp;
	string							strDuration;

	std::vector<string>				mapButtonNameList;

	void Realese()
	{
		wButtonSize		= 0;	//버튼의 갯수
		wObjectID		= 0;
		wNpcID			= 0;

		strDlgTitleName.clear();
		strDlgTitleName.resize(0);

		strNpcName.clear();
		strNpcName.resize(0);

		strDesc.clear();
		strDesc.resize(0);

		strExp.clear();
		strExp.resize(0);

		strRankExp.clear();
		strRankExp.resize(0);

		strDuration.clear();
		strDuration.resize(0);

		mapButtonNameList.clear();
		mapButtonNameList.resize(0);
	}
};

class C_VS_UI_INDOUN_DIALOG : public Window, public Exec, public ButtonVisual
{
private :

private:
	int						m_id;
	int						m_iSprite;
	ButtonGroup*			m_pC_button_group;
	CSpritePack				m_p_NPCFace;
	MString					m_name;

	std::vector<string>		m_vtDectList;
#if __CONTENTS(__BLITZ_COUPON_EVENT)
	std::vector<string>		m_vtDectShowList;
#endif //__BLITZ_COUPON_EVENT
	
	WORD					m_wDialog_Title_Postion_X;			//타이틀 이름의 위치
	WORD					m_wDialog_Title_Postion_Y;
	WORD					m_wDialog_Width;					//다이얼로그 크기(다이얼로그는 센터로 잡았음.)
	WORD					m_wDialog_Heigth;

	WORD					m_wSelectButton_Position_X;			//버튼의 위치
	WORD					m_wSelectButton_Position_Y;
	WORD					m_wSelectButton_Width;				//버튼의 Width
	WORD					m_wSelectButton_Height;
	WORD					m_wSelectButton_ID;					//버튼 ID

	WORD					m_wPicture_Position_X;				//사진의 위치
	WORD					m_wPicture_Position_Y;

	WORD					m_wDesc_Postion_X;					//설명의 위치
	WORD					m_wDesc_Postion_Y;
	WORD					m_wDesc_Line_Widht;					//설명 라인 길이의 최대 수
	WORD					m_wDesc_Line_Height;				//설명 라인 길이의 최소 수

	WORD					m_wNpcName_Position_X;
	WORD					m_wNpcName_Position_Y;

	WORD					m_wExp_position_X;
	WORD					m_wExp_position_Y;

	WORD					m_wRankExp_position_X;
	WORD					m_wRankExp_position_Y;

	WORD					m_wDuration_Position_X;
	WORD					m_wDuration_Position_Y;

#if __CONTENTS(!__DYNAMIC_UI)
	WORD					m_SelectButtonIdex;
#endif //__DYNAMIC_UI

	S_INDOUN_DLG_INFO		m_Indoun_Dlg_Info;

	ENUMTYPE_INDUONSELECT	m_etIndounSelect;

#if __CONTENTS(__BLITZ_COUPON_EVENT)
	float					m_tx;
	int						m_Picture_Alpha;
#endif //__BLITZ_COUPON_EVENT
	
	enum	//spk 인덱스
	{
		DLG_BOX,
		SELECT_BUTTON1,
		SELECT_BUTTON2,
	};
	
	enum EXEC_ID
	{
		INDOUN_NPC_DIALOG_CLOSE = 1000,		//인던의 인덱스 번호 때문에(인던이 1000층까지 있게 된다면 더 늘려 주자.)
		SELECT_BUTTON_RUN,
	};
public:
	C_VS_UI_INDOUN_DIALOG::C_VS_UI_INDOUN_DIALOG();
	C_VS_UI_INDOUN_DIALOG(S_INDOUN_DLG_INFO	Indoun_Dlg_Info, ENUMTYPE_INDUONSELECT etIndounSelect);

	~C_VS_UI_INDOUN_DIALOG();

	void	Show();
	void	Start();
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);	


	void	Finish();
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	bool	IsPixel(int _x, int _y);

	void	SetIndounNpcDlgInfo(S_INDOUN_DLG_INFO Indoun_Npc_Dlg_Info, ENUMTYPE_INDUONSELECT etIndounSelect);
private:
	void	InitStartDlgInfo();		//
	void	InitPicture();
	void	InitButton();
	void	InitDesc();
	void	InitStarDlg();			//입장 할때 정보 설정
	void	InitFinishDlg();		//보상 받을 때 정보 설정

	void	Picture_Show();			//NPC 사진의 위치
#if __CONTENTS(!__DYNAMIC_UI)
	void	ButtonName_Show();
#endif //__DYNAMIC_UI
	void	TitleName_Show();
	void	NPCName_Show();
	void	Desc_Show();

	void	Exp_Show();
	void	RankExp_Show();
	void	Duration_Show();
};

#endif