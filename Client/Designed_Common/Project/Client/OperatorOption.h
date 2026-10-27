//----------------------------------------------------------------------
// OperatorOption.h
//----------------------------------------------------------------------
// 운영자용 옵션
//----------------------------------------------------------------------

#ifndef	__OPERATOROPTION_H__
#define	__OPERATOROPTION_H__


class OperatorOption
{
public :
	OperatorOption();
	~OperatorOption();

	void	SetDefault();

	void	SaveToFile(const char* filename);
	bool	LoadFromFile(const char* filename);

public :
	bool	bShowModifyHP;			// HP 변화량 채팅창에 보기
	bool	bShowCreatureInfo;		// 크리처 oid 보기
	bool	bShowImageObjectInfo;	// 정지 오브젝트 iid, sid, vp 보기
	bool	bHideAttachEffect;		// 몸에 붙는 이팩트 숨기기
	bool	bHideSpeechBalloon;		// 말풍선 숨기기
	bool	bShowAddEffect;			// EffectStatus 추가되는거 보기
	bool	bShowModifyEXP;			// EXP 변화량 보기
	bool	bSlideScreenShot;		// 슬라이드 스크린샷 찍기
	bool	bShowSkillRange;		// 스킬 사용 범위 보기
	bool	bShowModifyRANKEXP;		// 계급경험치 변화량 보기
	bool	bRecordCharName;		// 캐릭터 이름 txt파일로 저장하기
	bool	bCanRootingGhost;		// 고스트 모드일 때 아이템 루팅 가능

};


extern	OperatorOption*		g_pOperatorOption;

extern	void	UI_AddChatToHistory(char* str, char* sz_id, int cond, DWORD color);


#endif

