//#define __3D_IMAGE_OBJECT__					// by sonee
//----------------------------------------------------------------------
// MTopViewDraw.cpp
//----------------------------------------------------------------------
//
// 복잡한 Draw 함수 부분만 따로 빼놓는다.
// 작업하기가 영 불편해서 
//----------------------------------------------------------------------
#pragma warning(disable:4786)
#include "Client_PCH.h"
#include "DX3D.h"
#include <math.h>
#include <list>
#include <stdio.h>
#include <fstream>
#include "MZone.h"
#include "MCreature.h"
#include "MFakeCreature.h"
#include "MPlayer.h"
#include "MItem.h"
#include "MImageObject.h"
#include "MAnimationObject.h"
#include "MShadowAnimationObject.h"
#include "DarkAssert.h"

#include "MMovingEffect.h"
#include "MScreenEffect.h"
#include "MTopView.h"
#include "MItemTable.h"
#include "MCreatureTable.h"
#include "MWeather.h"
#include "CMessageArray.h"
#include "DXLib.h"
#include "SP.h"
#include "FL2.h"
//#include "2D.h"
#include "ClientConfig.h"
#include "VS_UI_BASE.h"
#include "VS_UI_Mouse_Pointer.h"
#include "UIFunction.h"
#include "AddonDef.h"
#include "MZoneTable.h"
#include "MGameTime.h"
#include "UserOption.h"
#include "UserInformation.h"
#include "SpriteIDDef.h"
#include "MGameStringTable.h"
//#include "MZoneInfo.h"
#include "MObjectSelector.h"
#include "MPortal.h"
#include "EffectSpriteTypeDef.h"
#include "MScreenEffectManager.h"
#include "MEffectSpriteTypeTable.h"
#include "Profiler.h"
//#include "MFileDef.h"
#include "Properties.h"
#include "MEventManager.h"
#include "DebugInfo.h"
#include "MItemOptionTable.h"
#include "Client.h"
#include "SkillDef.h"
#include "vs_ui_item.h"
#include "MTopViewDraw.inl"

#include "OperatorOption.h"

extern "C" void Phoenix_TextPerf_CountSurfaceUnlockLock();
extern "C" int Phoenix_CreatureDrawAudit_ShouldLog(int creatureType);
extern "C" void Phoenix_CreatureDrawAudit_Log(const char* format, ...);

#define	SHIFT_SNIPPING			2

#ifdef __METROTECH_TEST__
	extern bool g_bLight;
#endif

#ifdef OUTPUT_DEBUG
	int g_WipeValue = 0;
	int g_WipeStep = 0;
#endif


static int g_nRocketPosValue[4] = {0,2,4,2};
extern int defaultSkinColor;
extern int defaultCoatColor;
extern int defaultTrouserColor;

namespace {
	POINT pointTemp;
	RECT rect;
};

//by viva
enum
{
ACTION_ADVANCEMENT_SLAYER171_STOP_SWORD_MACE_CROSS = 100,
ACTION_ADVANCEMENT_SLAYER171_MOVE_SWORD_MACE_CROSS,
ACTION_ADVANCEMENT_SLAYER171_MAGIC_ATTACK,
ACTION_ADVANCEMENT_SLAYER171_DRAINED,
ACTION_ADVANCEMENT_SLAYER171_DIE,
ACTION_ADVANCEMENT_SLAYER171_DAMAGED_SWORD,
ACTION_ADVANCEMENT_SLAYER171_DAMAGED_BLADE,
ACTION_ADVANCEMENT_SLAYER171_DAMAGED_GUN,
ACTION_ADVANCEMENT_SLAYER171_DAMAGED_CROSS_MACE,
ACTION_ADVANCEMENT_SLAYER171_BIKE_MOVE,
ACTION_ADVANCEMENT_SLAYER171_BIKE_STOP,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_SLOW,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_FAST,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_SLOW,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_FAST,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SWORD_SLOW,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SWORD_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SWORD_FAST,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_BLADE_SLOW,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_BLADE_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_ATTACK_BLADE_FAST,
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_SLOW,
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_FAST,
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_SLOW,
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_FAST,
ACTION_ADVANCEMENT_SLAYER171_STOP_GUN,
ACTION_ADVANCEMENT_SLAYER171_STOP_MACE_AND_CROSS,
ACTION_ADVANCEMENT_SLAYER171_STOP_BLADE,
ACTION_ADVANCEMENT_SLAYER171_MOVE_GUN,
ACTION_ADVANCEMENT_SLAYER171_MOVE_MACE_AND_CROSS,
ACTION_ADVANCEMENT_SLAYER171_MOVE_BLADE,
ACTION_ADVANCEMENT_SLAYER171_MAGIC,
ACTION_ADVANCEMENT_SLAYER171_SKILL_GUN_SLOW,
ACTION_ADVANCEMENT_SLAYER171_SKILL_GUN_NORMAL,
ACTION_ADVANCEMENT_SLAYER171_SKILL_GUN_FAST,
ACTION_ADVANCEMENT_SLAYER171_SPECIAL,
ACTION_ADVANCEMENT_SLAYER171_SPECIAL_2,
ACTION_ADVANCEMENT_SLAYER171_SLIDING,
ACTION_ADVANCEMENT_SLAYER171_SLIDING_END,

};
int AdvancementSlayerActionConvTable171[ ACTION_MAX_SLAYER ] = 
{
ACTION_STAND,										//0
ACTION_MOVE,										//1
-1,													//2
ACTION_ADVANCEMENT_SLAYER171_MAGIC,					//3
ACTION_DAMAGED,										//4
ACTION_ADVANCEMENT_SLAYER171_DRAINED,				//5
ACTION_ADVANCEMENT_SLAYER171_DIE,					//6
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_NORMAL,	//7
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_NORMAL,	//8
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_NORMAL,	//9
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_NORMAL,	//10
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SWORD_NORMAL,	//11
ACTION_ADVANCEMENT_SLAYER171_ATTACK_BLADE_NORMAL,	//12
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_NORMAL,	//13
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_NORMAL,	//14
ACTION_ADVANCEMENT_SLAYER171_BIKE_MOVE,				//15
ACTION_ADVANCEMENT_SLAYER171_BIKE_STOP,				//16
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_SLOW,	//17
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_FAST,	//18
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_SLOW,	//19
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_FAST,	//20
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_SLOW,	//21
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_FAST,	//22
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_SLOW,	//23
ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_FAST,	//24
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SWORD_SLOW,		//25
ACTION_ADVANCEMENT_SLAYER171_ATTACK_SWORD_FAST,		//26
ACTION_ADVANCEMENT_SLAYER171_ATTACK_BLADE_SLOW,		//27
ACTION_ADVANCEMENT_SLAYER171_ATTACK_BLADE_FAST,		//28
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_SLOW,		//29
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_FAST,		//30
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_SLOW,		//31
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_FAST,		//32
ACTION_STAND,										//33
ACTION_STAND,										//34
ACTION_STAND,										//35
ACTION_MOVE,										//36
ACTION_ADVANCEMENT_SLAYER171_MAGIC,					//37
ACTION_ADVANCEMENT_SLAYER171_SKILL_SWORD_NORMAL,	//38	
ACTION_ADVANCEMENT_SLAYER171_SKILL_BLADE_NORMAL,	//39
};

int AdvancementSlayerActionConvTable201[ ACTION_MAX_SLAYER ] = 
{
	ACTION_STAND,	// 따로 조건 변환 필요
	ACTION_MOVE,	// 따로 조건 변환 필요
//	-1,	// 따로 조건 변환 필요
    ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_NORMAL,
    ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_NORMAL,	
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_NORMAL,	
	ACTION_ADVANCEMENT_SLAYER_MAGIC,
	ACTION_DAMAGED,
	ACTION_ADVANCEMENT_SLAYER_DRAINED,
	ACTION_ADVANCEMENT_SLAYER_DIE,

	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_BIKE_MOVE,
	ACTION_ADVANCEMENT_SLAYER_BIKE_STOP,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_FAST,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_FAST,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_FAST,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW,		//SMG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_FAST,		//SMG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_FAST,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_FAST,
	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_FAST,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_FAST,
	ACTION_STAND,
	ACTION_STAND,
	ACTION_STAND,
	ACTION_MOVE,
	ACTION_ADVANCEMENT_SLAYER_MAGIC,
	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_NORMAL,
};

//end

int AdvancementOustersActionConvTable[ ACTION_MAX_SLAYER ] =
{
	ACTION_STAND,		// 따로 조건 변환 필요 0
	ACTION_MOVE,		// 따로 조건 변환 필요 1
	ACTION_ADVANCEMENT_OUSTERS_ATTACK_NORMAL,//2
	ACTION_ADVANCEMENT_OUSTERS_MAGIC,//3
	ACTION_ADVANCEMENT_OUSTERS_DAMAGED,//4
	ACTION_ADVANCEMENT_OUSTERS_DRAINED,//5
	ACTION_ADVANCEMENT_OUSTERS_DIE,//6
	ACTION_STAND,		// 따로 조건 변환 필요	(서있기)7
	ACTION_MOVE,		// 따로 조건 변환 필요	(걷기)8
	ACTION_ADVANCEMENT_OUSTERS_SKILL_NORMAL,//9
	ACTION_ADVANCEMENT_OUSTERS_MAGIC_ATTACK,//10
	ACTION_ADVANCEMENT_OUSTERS_ABSORB_SOUL,//11
	ACTION_ADVANCEMENT_OUSTERS_FAST_MOVE_STOP,//12		// 따로 조건 변환 필요	(공중정지)
	ACTION_ADVANCEMENT_OUSTERS_FAST_MOVE,//13		    // 따로 조건 변환 필요  (공중이동)
	ACTION_ADVANCEMENT_OUSTERS_ATTACK_SLOW,//14
	ACTION_ADVANCEMENT_OUSTERS_ATTACK_FAST,//15
	ACTION_ADVANCEMENT_OUSTERS_SKILL_SLOW,//16
	ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST,//17
#if __CONTENTS(__FAST_TRANSFORTER)
	ACTION_ADVANCEMENT_OUSTERS_WING_MOVE,//18
	ACTION_ADVANCEMENT_OUSTERS_WING_STOP,//19
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
	ACTION_ADVANCEMENT_OUSTERS_UNICORN_MOVE,//20
	ACTION_ADVANCEMENT_OUSTERS_UNICORN_STOP,//21
#endif //__SECOND_TRANSFORTER
	-1,//22
};

int AdvancementSlayerActionConvTable[ ACTION_MAX_SLAYER ] = 
{
	ACTION_STAND,	// 따로 조건 변환 필요
	ACTION_MOVE,	// 따로 조건 변환 필요
	-1,	// 따로 조건 변환 필요
	ACTION_ADVANCEMENT_SLAYER_MAGIC,
	ACTION_DAMAGED,
	ACTION_ADVANCEMENT_SLAYER_DRAINED,
	ACTION_ADVANCEMENT_SLAYER_DIE,

	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_NORMAL,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_NORMAL,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_NORMAL,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_NORMAL,		//SMG

	ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_NORMAL,

	ACTION_ADVANCEMENT_SLAYER_BIKE_MOVE,
	ACTION_ADVANCEMENT_SLAYER_BIKE_STOP,
	
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_FAST,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_FAST,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_FAST,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW,		//SMG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_FAST,		//SMG

	ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_FAST,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_FAST,
	
	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_FAST,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_FAST,

	ACTION_STAND,
	ACTION_STAND,
	ACTION_STAND,

	ACTION_MOVE,
	
	ACTION_ADVANCEMENT_SLAYER_MAGIC,

	ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_NORMAL,
};

//by viva vampire action
int GetAdvancementVampireAction171[] =
{
	0,	//0
	1,	//1
	2,	//2
	3,	//3
	4,	//4
	5,	//5
	13,	//6
	12,	//7
	8,	//8
	9,	//9
	10,	//10
	11,	//11
	6,	//12 //ok
	7,	//13 //ok
	-1,	//14
	-1,	//15
	-1,	//16
};
//by viva ousters action
int GetAdvancementOustersAction171[] = 
{
	0,	//0
	1,	//1
	6,	//2
	7,	//3
	12,	//4
	13,	//5
	14,	//6
	15,	//7
	16,	//8
	17,	//9
	8,	//10 //test -1
	2,	//11
	9,	//12
	4,	//13
	5,	//14
	3,	//15	ok??
	8,	//16
	10,	//17
	11,	//18
	18, //19
	19, //20
	20,	//21
	21, //22
};

int AdvancementVampireActionConvTable[ ACTION_MAX_SLAYER ] =
{
	ACTION_ADVANCEMENT_STOP,
	ACTION_ADVANCEMENT_MOVE,
	ACTION_ADVANCEMENT_ATTACK_NORMAL,
	ACTION_ADVANCEMENT_MAGIC,
	ACTION_ADVANCEMENT_DAMAGED,
	ACTION_ADVANCEMENT_DRAINED,
	ACTION_ADVANCEMENT_DIE,
	ACTION_ADVANCEMENT_DRAIN,
	ACTION_ADVANCEMENT_ATTACK_SLOW,		// 8		
	ACTION_ADVANCEMENT_ATTACK_FAST,		// 9
	ACTION_ADVANCEMENT_ATTACK_NORMAL,	// 10
	ACTION_ADVANCEMENT_ATTACK_SLOW,		// 11
	ACTION_ADVANCEMENT_ATTACK_FAST,		// 12
	ACTION_ADVANCEMENT_ATTACK_NORMAL,	// 13
	ACTION_ADVANCEMENT_ATTACK_SLOW,		// 14
	ACTION_ADVANCEMENT_ATTACK_FAST,		// 15
	ACTION_ADVANCEMENT_MAGIC,			// 16
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,

	-1,
	-1,
	-1, // 30
	-1,
	-1,
	-1,
	-1,
};


/*
int AdvancementVampireActionConvTable[ ACTION_MAX_SLAYER ] =
{
	ACTION_ADVANCEMENT_STOP,
	ACTION_ADVANCEMENT_MOVE,
	ACTION_ADVANCEMENT_ATTACK_NORMAL,
	ACTION_ADVANCEMENT_MAGIC,
	ACTION_ADVANCEMENT_DAMAGED,
	ACTION_ADVANCEMENT_DRAINED,
	ACTION_ADVANCEMENT_DIE,
	ACTION_ADVANCEMENT_DRAIN,
	ACTION_ADVANCEMENT_ATTACK_SLOW,				
	ACTION_ADVANCEMENT_ATTACK_FAST,
	ACTION_ADVANCEMENT_SKILL_SLOW,		// 10
	ACTION_ADVANCEMENT_SKILL_NORMAL,	// 11
	ACTION_ADVANCEMENT_SKILL_FAST,		// 12
	ACTION_ADVANCEMENT_MAGIC_ATTACK,
	ACTION_ADVANCEMENT_SPECIAL,
	ACTION_ADVANCEMENT_CREATE_WEAPON,
	ACTION_ADVANCEMENT_DESTROY_WEAPON,	// 16
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,

	-1,
	-1,
	-1, // 30
	-1,
	-1,
	-1,
	-1,
};
*/

/*
int AdvancementOustersActionConvTable[ ACTION_MAX_SLAYER ] =
{
	ACTION_STAND,		// 따로 조건 변환 필요
	ACTION_MOVE,		// 따로 조건 변환 필요
	ACTION_ADVANCEMENT_OUSTERS_COMBO_NORMAL , //ACTION_ADVANCEMENT_OUSTERS_ATTACK_NORMAL,
	ACTION_ADVANCEMENT_OUSTERS_MAGIC,
	ACTION_ADVANCEMENT_OUSTERS_DAMAGED,
	ACTION_ADVANCEMENT_OUSTERS_DRAINED,
	ACTION_ADVANCEMENT_OUSTERS_DIE,
	ACTION_STAND,		// 따로 조건 변환 필요	(서있기)
	ACTION_MOVE,		// 따로 조건 변환 필요	(걷기)
	ACTION_ADVANCEMENT_OUSTERS_COMBO_NORMAL,		//ACTION_ADVANCEMENT_OUSTERS_SKILL_NORMAL,
	ACTION_ADVANCEMENT_OUSTERS_MAGIC_ATTACK,
	ACTION_ADVANCEMENT_OUSTERS_ABSORB_SOUL,
	ACTION_ADVANCEMENT_OUSTERS_FAST_MOVE_STOP,		// 따로 조건 변환 필요	(공중정지)
	ACTION_ADVANCEMENT_OUSTERS_FAST_MOVE,		    // 따로 조건 변환 필요  (공중이동)
	ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW ,			// ACTION_ADVANCEMENT_OUSTERS_ATTACK_SLOW,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST,			// ACTION_ADVANCEMENT_OUSTERS_ATTACK_FAST,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW,			//ACTION_ADVANCEMENT_OUSTERS_SKILL_SLOW,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST ,			//ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST,
	-1,
};

int AdvancementSlayerActionConvTable[ ACTION_MAX_SLAYER ] = 
{
	ACTION_STAND,	// 따로 조건 변환 필요
	ACTION_MOVE,	// 따로 조건 변환 필요
	-1,	// 따로 조건 변환 필요
	ACTION_ADVANCEMENT_SLAYER_MAGIC,
	ACTION_DAMAGED,
	ACTION_ADVANCEMENT_SLAYER_DRAINED,
	ACTION_ADVANCEMENT_SLAYER_DIE,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_NORMAL,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_NORMAL,		//AR
	ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_NORMAL,		//ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_COMBO_BLADE_NORMAL,		//ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_NORMAL,
	-1,													//폭탄 던지기
	ACTION_ADVANCEMENT_SLAYER_BIKE_MOVE,
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_NORMAL,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_NORMAL,		//SMG
	ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_NORMAL,		//ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_COMBO_BLADE_NORMAL,		//ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_NORMAL,
	-1,													//성수 던지기
	ACTION_ADVANCEMENT_SLAYER_BIKE_STOP,				//18
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_FAST,		//SR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_FAST,		//AR
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_FAST,		//SG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW,		//SMG
	ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_FAST,		//SMG
	ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW,			//ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_FAST,			//ACTION_ADVANCEMENT_SLAYER_ATTACK_SWORD_FAST,
	ACTION_ADVANCEMENT_SLAYER_COMBO_BLADE_SLOW,			//ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_COMBO_BLADE_FAST,			//ACTION_ADVANCEMENT_SLAYER_ATTACK_BLADE_FAST,
	ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW,			//ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_FAST,			//ACTION_ADVANCEMENT_SLAYER_SKILL_SWORD_FAST,
	ACTION_ADVANCEMENT_SLAYER_COMBO_BLADE_SLOW,			//ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_COMBO_BLADE_FAST,			//ACTION_ADVANCEMENT_SLAYER_SKILL_BLADE_FAST,
};
	
int AdvancementVampireActionConvTable[ ACTION_MAX_SLAYER ] =
{
	ACTION_ADVANCEMENT_STOP,
	ACTION_ADVANCEMENT_MOVE,
	ACTION_ADVANCEMENT_COMBO_NORMAL ,	//ACTION_ADVANCEMENT_ATTACK_NORMAL,
	ACTION_ADVANCEMENT_MAGIC,
	ACTION_ADVANCEMENT_DAMAGED,
	ACTION_ADVANCEMENT_DRAINED,
	ACTION_ADVANCEMENT_DIE,
	ACTION_ADVANCEMENT_DRAIN,
	ACTION_ADVANCEMENT_COMBO_SLOW,		//ACTION_ADVANCEMENT_ATTACK_SLOW,				
	ACTION_ADVANCEMENT_COMBO_FAST,		//ACTION_ADVANCEMENT_ATTACK_FAST,
	ACTION_ADVANCEMENT_COMBO_SLOW,		//ACTION_ADVANCEMENT_SKILL_SLOW,		// 10
	ACTION_ADVANCEMENT_COMBO_NORMAL,	//ACTION_ADVANCEMENT_SKILL_NORMAL,	// 11
	ACTION_ADVANCEMENT_COMBO_FAST ,		//ACTION_ADVANCEMENT_SKILL_FAST,		// 12
	ACTION_ADVANCEMENT_MAGIC_ATTACK,
	ACTION_ADVANCEMENT_SPECIAL,
	ACTION_ADVANCEMENT_CREATE_WEAPON,
	ACTION_ADVANCEMENT_DESTROY_WEAPON,	// 16
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1, // 30
	-1,
	-1,
	-1,
	-1,
};


int AdvanceComboActionFrameTable[7][3][4] = 
{
	// 슬레이어 남 / 여자 검  
	13,26,39,58, 10,21,32,49, 8,17,26,41 ,
	13,26,39,58, 10,21,32,49, 8,17,26,41 ,

	// 슬레이어 남 / 여자 도
	14,29,44,65, 12,25,38,57, 10,21,32,49 ,
	14,29,44,65, 12,25,38,57, 10,21,32,49 ,

	// 벰파이어 남 / 여자
	12,25,38,57, 10,21,32,49, 8,17,26,41,  
	12,26,40,59, 10,21,32,49, 8,17,26,41,  

    // 아우스터  
	12,26,40,59, 10,21,32,49, 8,17,26,41 ,
};


int AdvanceComboNextActionTable[12] = 
{
	// 슬레이어
	ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW,
	ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_NORMAL,
	ACTION_ADVANCEMENT_SLAYERN_COMBO_CANCEL_SWARD_FAST,

	ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_BLADE_SLOW,
	ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_BLADE_NORMAL,
	ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_BLADE_FAST,
	
	// 벰파이어
	ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW ,//ACTION_ADVANCEMENT_COMBO_CANCEL_SLOW,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW,//ACTION_ADVANCEMENT_COMBO_CANCEL_NORMAL,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW,//ACTION_ADVANCEMENT_COMBO_CANCEL_FAST,
    // 아우스터
	ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_NORMAL,
	ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_FAST,
};




int AdvanceComboNextRepeatActionTable[12] = 
{
	// 슬레이어
	ACTION_SLAYER_SWORD_SLOW,
	ACTION_SLAYER_SWORD,
	ACTION_SLAYER_SWORD_FAST,

	ACTION_SLAYER_BLADE_SLOW,
	ACTION_SLAYER_BLADE,
	ACTION_SLAYER_BLADE_FAST,
	
	// 벰파이어
	ACTION_VAMPIRE_ATTACK_SLOW ,
	ACTION_ATTACK,
	ACTION_VAMPIRE_ATTACK_FAST,
    // 아우스터
	ACTION_OUSTERS_ATTACK_SLOW,
	ACTION_ATTACK,
	ACTION_OUSTERS_ATTACK_FAST,
};


int AdvanceComboCancelFrameTable[5][3][3] = 
{
	// 슬레이어 남 / 여자 
	1,3,5, 1,3,5, 1,3,5,
	1,3,5, 1,3,5, 1,3,5,
	// 벰파이어 남 / 여자 
	1,3,5, 1,3,5, 1,3,5,
	1,3,5, 1,3,5, 1,3,5,
    // 아우스터
	1,3,5, 1,3,5, 1,3,5,
};
*/

BYTE GetCreatureActionCountMax( const MCreature* pCreature, int action );

int GetAdvancementVampireActionFromVampireAction( int CurAction, const MCreature* pCreature )
{
	if( CurAction >= ADVANCEMENT_ACTION_START )
		return CurAction;

	return AdvancementVampireActionConvTable[ CurAction ];
}

//by viva
int GetAdvancementPartFromItemClass171( ITEM_CLASS itemClass, TYPE_FRAMEID frameID, int itemType)
{
	switch( itemClass )
	{
	case ITEM_CLASS_MOTORCYCLE :
		{
			if (frameID == ADDONID_WING_BIKE)
				return ADVANCEMENT_WING_BIKE;
			else if (frameID == ADDONID_HOVER_VEHICLE)
				return ADVANCEMENT_HOVER_VEHICLE;
			else if (frameID == ADDONID_AIR_MOTO && itemType == 10)
				return ADVANCEMENT_AIR_MOTO;
			else if(frameID == AC_BIKE_2_COLOR)
				return UNKOWN_2;
			else if(itemType == 11)
				return ADVANCEMENT_APQ_VEHICLE;
//			return AC_BIKE_1;
			return UNKOWN_2;
		}
		
	case ITEM_CLASS_SWORD :
		{
			if(itemType == 21 || itemType == 22 || itemType == 23)
				return ADVANCEMENT_SWORD_2;

			return ADVANCEMENT_SWORD_1;
		}

	case ITEM_CLASS_BLADE :
		{
			if(itemType == 21 || itemType == 22 || itemType == 23)
				return ADVANCEMENT_BLADE_2;

			return ADVANCEMENT_BLADE_1;
		}

	case ITEM_CLASS_AR :
		{
			if(itemType == 21 || itemType == 22 || itemType == 23)
				return ADVANCEMENT_GUN_AR_2;

			return ADVANCEMENT_GUN_AR_1;
		}

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
		return ADVANCEMENT_GUN_SR_1;

	case ITEM_CLASS_SR :
		{
			if(itemType == 21 || itemType == 22 || itemType == 23)
				return ADVANCEMENT_GUN_SR_2;

			return ADVANCEMENT_GUN_SR_1;
		}

	case ITEM_CLASS_MACE :
		{
			if(itemType == 19 || itemType == 20 || itemType == 21)
				return ADVANCEMENT_MACE_2;

			return ADVANCEMENT_MACE_1;
		}

	case ITEM_CLASS_CROSS :
		{
			if(itemType == 19 || itemType == 20 || itemType == 21)
				return ADVANCEMENT_CROSS_2;

			return ADVANCEMENT_CROSS_1;
		}

	case ITEM_CLASS_SHIELD :
		{
			if(itemType == 16 || itemType == 17 || itemType == 18)
				return ADVANCEMENT_SHIELD_2;

			return ADVANCEMENT_SHIELD_1;
		}

	case ITEM_CLASS_HELM :
		{
//			if(itemType == 16 || itemType == 17)
//				return ADVANCEMENT_HELMET_2;

			return ADVANCEMENT_HELMET_1;
		}
    case ITEM_CLASS_TROUSER :
	case ITEM_CLASS_COAT :
		{
			if(itemType == 34 || itemType == 35 || itemType == 37 || itemType == 36)
				return ADVANCEMENT_COAT_2;

			return ADVANCEMENT_COAT_1;
		}
		
	/*case ITEM_CLASS_TROUSER :
		{
			//if(itemType ==34 || itemType ==35 || itemType == 37 || itemType == 36)
			//	return ADVANCEMENT_TROUSER_2;

			return ADVANCEMENT_TROUSER_1;
		}*/

	case ITEM_CLASS_SHOULDER_ARMOR :
		return ADVANCEMENT_SHOULDER_1;
	}

	return -1;
}

int GetAdvancementPartFromItemClass( ITEM_CLASS itemClass, TYPE_FRAMEID frameID)
{
//	assert(false && "님하 즐드333");

	switch( itemClass )
	{
	case ITEM_CLASS_MOTORCYCLE :
		{
#if __CONTENTS(__FAST_TRANSFORTER)
			if (frameID == ADDONID_WING_BIKE)
			{
				return AC_WING_BIKE;
			}
			else
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
			if (frameID == ADDONID_HOVER_VEHICLE)
			{
				return AC_HOVER_VEHICLE;
			}
			//else
			//if (frameID == ADDONID_AIR_MOTO)
			//{
			//	return ADVANCEMENT_AIR_MOTO;
			//}		
			//else 
			//if(frameID == ADDONID_APQ_VEHICLE)
			//{
			//	return ADVANCEMENT_APQ_VEHICLE;
			//}
			else
#endif //__SECOND_TRANSFORTER
			if(frameID == AC_BIKE_2_COLOR)
				return AC_BIKE_2_COLOR;
			return AC_BIKE_1;
		}
		
	case ITEM_CLASS_SWORD :
		return AC_SWORD;

	case ITEM_CLASS_BLADE :
		return AC_BLADE;

	case ITEM_CLASS_AR :
		return AC_GUN_AR;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_SR :
		return AC_GUN_SR;

	case ITEM_CLASS_MACE :
		return AC_MACE;

	case ITEM_CLASS_CROSS :
		return AC_CROSS;

	case ITEM_CLASS_SHIELD :
		return AC_SHIELD;

	case ITEM_CLASS_HELM :
		return AC_HELMET;

	case ITEM_CLASS_COAT :
	case ITEM_CLASS_TROUSER :
		return AC_BODY;

	case ITEM_CLASS_SHOULDER_ARMOR :
		return AC_SHOULDER;
	}

	return -1;
}

int GetAdvancementPartFromItemClass201( ITEM_CLASS itemClass, TYPE_FRAMEID frameID, int itemType)
{
//	assert(false && "님하 즐드333");
   
	switch( itemClass )
	{
	case ITEM_CLASS_MOTORCYCLE :
		{
			if (frameID == ADDONID_WING_BIKE)
				return ADVANCEMENT201_WING_BIKE;
			else if (frameID == ADDONID_HOVER_VEHICLE)
				return ADVANCEMENT201_HOVER_VEHICLE;
			else if (frameID == ADDONID_AIR_MOTO)
				return ADVANCEMENT201_AIR_MOTO;
			else if(frameID == AC_BIKE_2_COLOR)
//				return AC_BIKE_2_COLOR;
				return UNKOWN_1;
//			return AC_BIKE_1;
			return UNKOWN_1;
		}
		
	case ITEM_CLASS_SWORD :
		{
			if(itemType == 23 || itemType == 24 || itemType == 25 || itemType == 26)
				return ADVANCEMENT201_SWORD_1;

			return ADVANCEMENT_SWORD_1;
		}

	case ITEM_CLASS_BLADE :
		{
			if(itemType == 23 || itemType == 24 || itemType == 25 || itemType == 26)
				return ADVANCEMENT201_BLADE_1;

			return ADVANCEMENT_BLADE_1;
		}

	case ITEM_CLASS_AR :
		{
			if(itemType == 23 || itemType == 24 || itemType == 25 || itemType == 26)
				return ADVANCEMENT201_GUN_AR_1;

			return ADVANCEMENT_GUN_AR_1;
		}

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
		return ADVANCEMENT201_GUN_SR_1;

	case ITEM_CLASS_SR :
		{
			if(itemType == 23 || itemType == 24 || itemType == 25 || itemType == 26)
				return ADVANCEMENT201_GUN_SR_1;

			return ADVANCEMENT_GUN_SR_1;
		}

	case ITEM_CLASS_MACE :
		{
			if(itemType == 21 || itemType == 22 || itemType == 23 || itemType == 24)
				return ADVANCEMENT201_MACE_1;

			return ADVANCEMENT_MACE_1;
		}

	case ITEM_CLASS_CROSS :
		{
			if(itemType == 21 || itemType == 22 || itemType == 23 || itemType == 24)
				return ADVANCEMENT201_CROSS_1;

			return ADVANCEMENT_CROSS_1;
		}

	case ITEM_CLASS_SHIELD :
		{
			if(itemType == 18 || itemType == 19 || itemType == 20)
				return ADVANCEMENT201_SHIELD_1;

			return ADVANCEMENT_SHIELD_1;
		}

	case ITEM_CLASS_HELM :
		{

			return ADVANCEMENT201_HELMET_1;
		}

	case ITEM_CLASS_COAT :
		{
			if(itemType == 36 || itemType == 37 || itemType == 38 || itemType == 39 || itemType == 40 || itemType == 41 || itemType == 42 || itemType == 43)
				return ADVANCEMENT201_COAT_1;

		//	return ADVANCEMENT_COAT_1;
		}
		
	case ITEM_CLASS_TROUSER :
		{
			if(itemType == 37 || itemType == 36 || itemType == 38 || itemType == 39 || itemType == 40 || itemType == 41)
				return ADVANCEMENT201_TROUSER_1;
		}

	case ITEM_CLASS_SHOULDER_ARMOR :
		return ADVANCEMENT_SHOULDER_1;
	}

	return -1;
}
int GetAdvancementPartFromItemClass231( ITEM_CLASS itemClass, TYPE_FRAMEID frameID, int itemType)
{
	//	assert(false && "님하 즐드333");

	switch( itemClass )
	{
	case ITEM_CLASS_MOTORCYCLE :
		{
			//if (frameID == ADDONID_WING_BIKE)
			//	return ADVANCEMENT201_WING_BIKE;
			//else if (frameID == ADDONID_HOVER_VEHICLE)
			//	return ADVANCEMENT201_HOVER_VEHICLE;
			//else if (frameID == ADDONID_AIR_MOTO)
			//	return ADVANCEMENT201_AIR_MOTO;
			//else if(frameID == AC_BIKE_2_COLOR)
			//	//				return AC_BIKE_2_COLOR;
			//	return UNKOWN_1;
			////			return AC_BIKE_1;
			return UNKOWN_1;
		}

	case ITEM_CLASS_SWORD :
		{
			if(itemType >= 27)
				return ADVANCEMENT201_SWORD_1;

			return ADVANCEMENT_SWORD_1;
		}

	case ITEM_CLASS_BLADE :
		{
			if(itemType >= 27)
				return ADVANCEMENT201_BLADE_1;

			return ADVANCEMENT_BLADE_1;
		}

	case ITEM_CLASS_AR :
		{
			if(itemType >= 27)
				return ADVANCEMENT201_GUN_AR_1;

			return ADVANCEMENT_GUN_AR_1;
		}

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
		return ADVANCEMENT201_GUN_SR_1;

	case ITEM_CLASS_SR :
		{
			if(itemType >= 27)
				return ADVANCEMENT201_GUN_SR_1;

			return ADVANCEMENT_GUN_SR_1;
		}

	case ITEM_CLASS_MACE :
		{
			if(itemType >= 25)
				return ADVANCEMENT201_MACE_1;

			return ADVANCEMENT_MACE_1;
		}

	case ITEM_CLASS_CROSS :
		{
			if(itemType >= 25)
				return ADVANCEMENT201_CROSS_1;

			return ADVANCEMENT_CROSS_1;
		}

	case ITEM_CLASS_SHIELD :
		{
			if(itemType >= 21)
				return ADVANCEMENT201_SHIELD_1;

			return ADVANCEMENT_SHIELD_1;
		}

	case ITEM_CLASS_HELM :
		{

			return ADVANCEMENT201_HELMET_1;
		}

	case ITEM_CLASS_COAT :
		{
			if(itemType == 44 || itemType == 45  || itemType == 46 || itemType == 47)
				return ADVANCEMENT201_COAT_1;

			//	return ADVANCEMENT_COAT_1;
		}

	case ITEM_CLASS_TROUSER :
		{
			if(itemType == 42 || itemType == 43 || itemType == 44 || itemType == 45)
				return ADVANCEMENT201_TROUSER_1;
		}

	case ITEM_CLASS_SHOULDER_ARMOR :
		return ADVANCEMENT_SHOULDER_1;
	}

	return -1;
}

int	GetAdvancementSlayerStandActionFromItemClass( ITEM_CLASS itemclass )
{
	switch( itemclass )
	{
	case ITEM_CLASS_SWORD :
		return ACTION_ADVANCEMENT_SLAYER_STOP_SWORD;

	case ITEM_CLASS_BLADE :
		return ACTION_ADVANCEMENT_SLAYER_STOP_BLADE;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_AR :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER_STOP_GUN;		
	}

	return ACTION_ADVANCEMENT_SLAYER_STOP_MACE_AND_CROSS;
}

int GetAdvancementSlayerMoveActionFromItemClass( ITEM_CLASS itemclass )
{
	switch( itemclass )
	{
	case ITEM_CLASS_SWORD :
		return ACTION_ADVANCEMENT_SLAYER_MOVE_SWORD;

	case ITEM_CLASS_BLADE :
		return ACTION_ADVANCEMENT_SLAYER_MOVE_BLADE;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_AR :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER_MOVE_GUN;
	}

	return ACTION_ADVANCEMENT_SLAYER_MOVE_MACE_AND_CROSS;
}

int GetAdvancementSlayerDamagedActionFromItemClass( ITEM_CLASS itemclass )
{
	switch( itemclass )
	{
	case ITEM_CLASS_SWORD :
		return ACTION_ADVANCEMENT_SLAYER_DAMAGED_SWORD;

	case ITEM_CLASS_BLADE :
		return ACTION_ADVANCEMENT_SLAYER_DAMAGED_BLADE;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_AR :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER_DAMAGED_GUN;
	}

	return ACTION_ADVANCEMENT_SLAYER_DAMAGED_CROSS_MACE;
}

int GetAdvancementSlayerAttackActionFromItemClass( ITEM_CLASS itemClass, MCreature* pCreature )
{
	switch( itemClass )
	{
	case ITEM_CLASS_AR :
		return ACTION_ADVANCEMENT_SLAYER_ATTACK_AR_GUN_SLOW + pCreature->GetWeaponSpeed();
	
	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER_ATTACK_SR_GUN_SLOW + pCreature->GetWeaponSpeed();
	}

	return -1;
}
//by viva
//-----------------------Stand
int	GetAdvancementSlayerStandActionFromItemClass171( ITEM_CLASS itemclass )
{
	switch( itemclass )
	{
	case ITEM_CLASS_SWORD :
		return ACTION_ADVANCEMENT_SLAYER171_STOP_MACE_AND_CROSS;
	case ITEM_CLASS_BLADE :
		return ACTION_ADVANCEMENT_SLAYER171_STOP_BLADE;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_AR :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER171_STOP_GUN;
	}

	return ACTION_ADVANCEMENT_SLAYER171_STOP_SWORD_MACE_CROSS ;
}
//------------------------------Move
int GetAdvancementSlayerMoveActionFromItemClass171( ITEM_CLASS itemclass )
{
	switch( itemclass )
	{
	case ITEM_CLASS_SWORD :
		return ACTION_ADVANCEMENT_SLAYER171_MOVE_MACE_AND_CROSS;
	case ITEM_CLASS_BLADE :
		return ACTION_ADVANCEMENT_SLAYER171_MOVE_BLADE;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_AR :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER171_MOVE_GUN;
	}

	return ACTION_ADVANCEMENT_SLAYER171_MOVE_SWORD_MACE_CROSS;
}
//---------------------------Damaged
int GetAdvancementSlayerDamagedActionFromItemClass171( ITEM_CLASS itemclass )
{
	switch( itemclass )
	{
	case ITEM_CLASS_SWORD :
		return ACTION_ADVANCEMENT_SLAYER171_DAMAGED_SWORD;

	case ITEM_CLASS_BLADE :
		return ACTION_ADVANCEMENT_SLAYER171_DAMAGED_BLADE;

	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_AR :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER171_DAMAGED_GUN;
	}

	return ACTION_ADVANCEMENT_SLAYER_DAMAGED_CROSS_MACE;
}
//----------------------Attack
int GetAdvancementSlayerAttackActionFromItemClass171( ITEM_CLASS itemClass, MCreature* pCreature )
{
	switch( itemClass )
	{
	case ITEM_CLASS_AR :
		return ACTION_ADVANCEMENT_SLAYER171_ATTACK_AR_GUN_SLOW + pCreature->GetWeaponSpeed();
	
	case ITEM_CLASS_SG :
	case ITEM_CLASS_SMG :
	case ITEM_CLASS_SR :
		return ACTION_ADVANCEMENT_SLAYER171_ATTACK_SR_GUN_SLOW + pCreature->GetWeaponSpeed();
	}

	return -1; 
}
//-----------------------------------
int ConvAdvancementSlayerActionFromSlayerAction171(int CurAction, MCreatureWear* pCreatureWear)
{
	Assert( pCreatureWear != NULL );

	if( CurAction ==130)
		return ACTION_ADVANCEMENT_SLAYER171_MAGIC_ATTACK;
	if( ADVANCEMENT_ACTION_START <= CurAction )
		return CurAction;

	const MCreatureWear::ADDON_INFO& addon = pCreatureWear->GetAddonInfo( ADDON_RIGHTHAND );

	switch( CurAction )
	{
	case ACTION_DAMAGED :
		return GetAdvancementSlayerDamagedActionFromItemClass171( addon.ItemClass );

	case ACTION_STAND :
		return GetAdvancementSlayerStandActionFromItemClass171( addon.ItemClass );		
		
	case ACTION_MOVE :
		return GetAdvancementSlayerMoveActionFromItemClass171( addon.ItemClass );
/*
	case ACTION_SLAYER_GUN_SR :
	case ACTION_SLAYER_GUN_AR :
	case ACTION_SLAYER_GUN_SG :
	case ACTION_SLAYER_GUN_SMG :
		case ACTION_SLAYER_GUN_SR_SLOW :
		return GetAdvancementSlayerAttackActionFromItemClass( addon.ItemClass, pCreatureWear );*/
	}	

	return AdvancementSlayerActionConvTable171[ CurAction ];
}

int ConvAdvancementSlayerActionFromSlayerAction201(int CurAction, MCreatureWear* pCreatureWear)
{
	Assert( pCreatureWear != NULL );

	if( ADVANCEMENT_ACTION_START <= CurAction )
		return CurAction;

	const MCreatureWear::ADDON_INFO& addon = pCreatureWear->GetAddonInfo( ADDON_RIGHTHAND );

	switch( CurAction )
	{
	case ACTION_DAMAGED :
		return GetAdvancementSlayerDamagedActionFromItemClass( addon.ItemClass );

	case ACTION_STAND :
		return GetAdvancementSlayerStandActionFromItemClass( addon.ItemClass );		
		
	case ACTION_MOVE :
		return GetAdvancementSlayerMoveActionFromItemClass( addon.ItemClass );
/*
	case ACTION_SLAYER_GUN_SR :
	case ACTION_SLAYER_GUN_AR :
	case ACTION_SLAYER_GUN_SG :
	case ACTION_SLAYER_GUN_SMG :
		case ACTION_SLAYER_GUN_SR_SLOW :
		return GetAdvancementSlayerAttackActionFromItemClass( addon.ItemClass, pCreatureWear );*/
	}	

	return AdvancementSlayerActionConvTable201[ CurAction ];
}

int ConvAdvancementSlayerActionFromSlayerAction( int CurAction, MCreatureWear* pCreatureWear )
{
	Assert( pCreatureWear != NULL );

	if( ADVANCEMENT_ACTION_START <= CurAction )
		return CurAction;

	const MCreatureWear::ADDON_INFO& addon = pCreatureWear->GetAddonInfo( ADDON_RIGHTHAND );

	switch( CurAction )
	{
	case ACTION_DAMAGED :
		return GetAdvancementSlayerDamagedActionFromItemClass( addon.ItemClass );

	case ACTION_STAND :
		return GetAdvancementSlayerStandActionFromItemClass( addon.ItemClass );		
		
	case ACTION_MOVE :
		return GetAdvancementSlayerMoveActionFromItemClass( addon.ItemClass );
/*
	case ACTION_SLAYER_GUN_SR :
	case ACTION_SLAYER_GUN_AR :
	case ACTION_SLAYER_GUN_SG :
	case ACTION_SLAYER_GUN_SMG :
		case ACTION_SLAYER_GUN_SR_SLOW :
		return GetAdvancementSlayerAttackActionFromItemClass( addon.ItemClass, pCreatureWear );*/
	}	

	return AdvancementSlayerActionConvTable[ CurAction ];
};

int ConvAdvancementOustersActionFromOusterAction( int CurAction, bool bChakram )
{
	if( ADVANCEMENT_ACTION_START <= CurAction )
		return CurAction;

	switch( CurAction )
	{
	case ACTION_STAND :
		if( bChakram )
			return ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP;
		else
			return ACTION_ADVANCEMENT_OUSTERS_WRISTLET_STOP;
		
	case ACTION_MOVE :
		if( bChakram )
			return ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_MOVE;
		else
			return ACTION_ADVANCEMENT_OUSTERS_WRISTLET_MOVE;
	}
	
#if __CONTENTS(__SECOND_TRANSFORTER)
	if( CurAction > ACTION_OUSTERS_UNICORN_MOVE )
		return -1;
#else
	if( CurAction > ACTION_OUSTERS_WING_MOVE )
		return -1;
#endif //__SECOND_TRANSFORTER

	return AdvancementOustersActionConvTable[ CurAction ];
}


bool IsEscapeDrawCreatureFunction( MCreature* pCreature )
{
	bool InGroundElemental = pCreature->IsOusters() && pCreature->IsPlayer() && g_pPlayer->IsOusters() == false
							&& pCreature->IsInGroundElemental();

	bool bGhost = pCreature->HasEffectStatus(EFFECTSTATUS_GHOST) || g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_DRAW_CREATURE)
		|| pCreature->IsGhost(1) && pCreature->IsGhost(2) && pCreature->IsGhost(4);

		
	// 2004, 6, 7 sobeit add start - 펫일 경우 주인인 다크니스 안에 있으면 안보이게..
	bool bPet1 = false;
	if(pCreature->IsFakeCreature())
	{
		MCreature *OwnerCreature = g_pZone->GetCreature(((MFakeCreature*)pCreature)->GetOwnerID());
		if(NULL != OwnerCreature)
		{
			if(OwnerCreature->IsInDarkness())
				bPet1 = true;
		}
	}

	bool bBlind = false;
	if(
		(g_pPlayer->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE) || 
		g_pPlayer->HasEffectStatus(EFFECTSTATUS_FLARE )||
		g_pPlayer->HasEffectStatus(EFFECTSTATUS_BLINDNESS))
		&& pCreature != g_pPlayer 
		)
	{
		// 거리에 따라서 출력 여부를 결정한다.
		int sx,sy,ex,ey;

		sx = g_pPlayer->GetX() - 1;
		ex = g_pPlayer->GetX() + 1;

		sy = g_pPlayer->GetY() - 1;
		ey = g_pPlayer->GetY() + 1;

		if( pCreature->GetX() < sx || pCreature->GetX() > ex ||
			pCreature->GetY() < sy || pCreature->GetY() > ey )
			bBlind = true;
	}

	return InGroundElemental || bGhost || bPet1 || bBlind ;
}

//----------------------------------------------------------------------
// Draw Creature : character 출력하기	
//----------------------------------------------------------------------
// pSurface의 pPoint에 pCreature를 출력한다.
//----------------------------------------------------------------
void
MTopView::DrawCreature(POINT* pPoint, MCreature* pCreature)
{
	CT_Done("DC: ENTER pCreature=%p", (void*)pCreature);
	// EFFECTSTATUS_GHOST 에 걸려있으면 남도 못보고 나도 못본다
	//DEBUG_ADD_FORMAT("[DrawCreature] Start %s %s %s", pCreature->IsGhost(1)?"true":"false", pCreature->IsGhost(2)?"true":"false", pCreature->IsGhost(4)?"true":"false");
	if( IsEscapeDrawCreatureFunction( pCreature ) ) {
		CT_Done("DC: IsEscape=true, EXIT");
		return;
	}

	// 2004, 6, 7 sobeit add end
	__BEGIN_PROFILE("DrawCreature");

	int body, action, direction, frame, creature_type, frameCount;

	CT_Done("DC: pre GetCreatureFrameCount (virtual)");
	frameCount  = pCreature->GetCreatureFrameCount();
	CT_Done("DC: post GetCreatureFrameCount=%d", frameCount);

	for(int FrameIndex = 0; FrameIndex< frameCount ; FrameIndex++)
	{
		CT_Done("DC: iter %d pre virtual chain", FrameIndex);
		body		= pCreature->GetCreatureFrameID(FrameIndex);
		action		= pCreature->GetAction();
		direction	= pCreature->GetDirection();
		frame		= pCreature->GetFrame();//%m_CreatureFPK[body][action][direction].GetCount();
		creature_type = pCreature->GetCreatureType();
		CT_Done("DC: iter %d post body/action/dir/frame/type chain", FrameIndex);

		// FAZ 4 RESTORE (2026-05-18): IsPlayer()/IsSlayer() non-virtual data-driven
		// (MCreature.h:149/159 — g_pCreatureTable lookup). GetClassType() ==
		// CLASS_PLAYER yeterli defansif gate (non-virtual, AV riski yok).
		// 2026-05-18 BUILD FIX: __try/__except kaldirildi. MString + std::string
		// C++ object unwinding gerektirir; SEH ile celisir (C2712 error).
		// Slayer Hellbike ride'da arka tekerlek fire/exhaust frame override restore.
		if (pCreature->GetClassType() == MCreature::CLASS_PLAYER
			&& pCreature->IsPlayer() && pCreature->IsSlayer())
		{
			CT_Done("DC: iter %d Hellbike branch entered (player+slayer)", FrameIndex);
			const MCreatureWear::ADDON_INFO& info =  ((MCreatureWear*)pCreature)->GetAddonInfo(ADDON_MOTOR);

			if ( info.bAddon )
			{
				MString strName = (*g_pItemTable)[info.ItemClass][info.ItemType].HName;
				std::string strSTDName = strName.GetString();

				if ( strSTDName.find("왕폭곬苦") != -1 && frame != 0 )//왕폭곬苦칡辜났
				{
					if ( frame == 3 )
					{
						frame = frame - 1;
						CT_Done("DC: iter %d Hellbike frame override 3->2", FrameIndex);
					}
					//std::ofstream file("Log\\motor.log", std::ios_base::out | std::ios_base::app);
	               // file	<< "Frame="<< frame << '\n';
				}
			//	if ( strSTDName.find("각탕펜") != -1 && frame != 0 )//왕폭곬苦칡辜났
			//	{
			//	//	if ( frame == 20 )
			////			frame = frame + 16;
			//		frameCount = 1;
			//		std::ofstream file("Log\\motor1.log", std::ios_base::out | std::ios_base::app);
	  //              file	<< "Frame="<< frameCount << '\n';
			//	}

			}
		}
		//if(pCreature->GetCreatureType()== 1730)
		//{
		//	if(action == 0)
		//	{
		//		  frame =18;
		//	}
		//}
	//if(pCreature->GetCreatureType()==1561)	   // 맒속땡鱗槻벎
	//{
	//	int m_Action = pCreature->GetAction();
	//	char szBuf [128];
	//	{
	//		if(m_Action==ACTION_STAND)
	//		{
	//			sprintf(szBuf, "界岺%d",m_Action);
	//			g_pSystemMessage->Add( szBuf );		
	//			pCreature->RemoveEffectStatus(EFFECTSTATUS_unknow767)  ;
	//			pCreature->AddEffectStatus(EFFECTSTATUS_unknow766, 0xFFFF);
	//			
	//		}
	//		if(m_Action== ACTION_MOVE)
	//		{
	//			sprintf(szBuf, "盧땡%d",m_Action);
	//			g_pSystemMessage->Add( szBuf );
	//			pCreature->RemoveEffectStatus(EFFECTSTATUS_unknow766);
	//			pCreature->AddEffectStatus(EFFECTSTATUS_unknow767, 0xFFFF);
	//		}

	//	}
	//}
	//	if(action>ACTION_OUSTERS_MAGIC_ATTACK)
	//		int i = 0;
		//----------------------------------------------------------
		// Charm에 걸려 있는 캐릭터..
		// 흡혈당하는 동작으로 표현..
		//----------------------------------------------------------
		if (pCreature->HasEffectStatus(EFFECTSTATUS_CHARM))
		{
			action = ACTION_DRAINED;
			frame = 6 + (frame & 0x07);		// 흡혈 동작에서 반복 frame의 시작이 6이다.		
		}
		if(pCreature->HasEffectStatus(EFFECTSTATUS_BIKE_CRASH))
			frame = 0;
		
		CT_Done("DC: iter %d pre IsSlayerCharacter (virtual)", FrameIndex);
		bool isSlayerCharacter	= pCreature->IsSlayerCharacter();
		CT_Done("DC: iter %d post IsSlayerCharacter=%d, pre IsOusters", FrameIndex, (int)isSlayerCharacter);
		//bool isOustersCharacter = pCreature->IsOusters();

		bool isOustersCharacter = (pCreature->IsOusters() && pCreature->GetCreatureType() != CREATURETYPE_OUSTERS_OPERATOR
								&&	creature_type != 717 && creature_type != 721 && creature_type != 723);
		CT_Done("DC: iter %d post IsOusters chain=%d, pre IsVampireCharacter", FrameIndex, (int)isOustersCharacter);
		bool isVampireCharacter = pCreature->IsVampireCharacter();
		CT_Done("DC: iter %d post IsVampireCharacter=%d", FrameIndex, (int)isVampireCharacter);
		if (Phoenix_CreatureDrawAudit_ShouldLog(creature_type))
		{
			Phoenix_CreatureDrawAudit_Log(
				"DrawCreature iter=%d ptr=%p id=%d type=%d class=%d body=%d action=%d dir=%d frame=%d frameCount=%d slayer=%d ousters=%d vampire=%d npc=%d playerOnly=%d operator=%d adv=%d hallu=%d dead=%d",
				FrameIndex, (void*)pCreature, (int)pCreature->GetID(), creature_type,
				(int)pCreature->GetClassType(), body, action, direction, frame,
				frameCount, (int)isSlayerCharacter, (int)isOustersCharacter,
				(int)isVampireCharacter, (int)pCreature->IsNPC(),
				(int)pCreature->IsPlayerOnly(), (int)pCreature->IsOperator(),
				(int)pCreature->IsAdvancementClass(), (int)pCreature->IsHallu(),
				(int)pCreature->IsDead());
		}
							
		
		// 할루가 걸렸으면 액션 프레임등을 바꾸는데... 단 늑대 상태나 박쥐 상태에서는 
		// 바꾸지 않도록 한다. 일단 기획팀에 의해서 주석처리해놓았다.
		if (pCreature->IsHallu()
	//		&& !pCreature->HasEffectStatus( EFFECTSTATUS_TRANSFORM_TO_BAT )
	//		&& !pCreature->HasEffectStatus( EFFECTSTATUS_TRANSFORM_TO_WOLF )
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
	#ifdef __METROTECH_TEST__
			&& !g_bLight
	#endif
			)
		{
			// Body draw cannot depend on the shadow pass to refresh hallucination
			// action/frame. Some render paths skip or reorder shadow drawing.
			pCreature->DetermineHalluActionFrame();

			isSlayerCharacter = false;
			isOustersCharacter = false;

			body	= pCreature->GetHalluCreatureFrameID();
			action	= pCreature->GetHalluAction();
			frame	= pCreature->GetHalluFrame();
			creature_type = pCreature->GetHalluCreatureType();
		}

		//////////////////////////////////////////////////////
		// 2005.11.21 by chyaya
		// 할루시네이션에 걸렸을 경우 캐릭터를 몬스터로 처리하기 때문에
		// 승직 캐릭터로 취급하지 않고 일반 캐릭터로 취급해 처리한다.
		// 아래의 변수를 사용하여 하드 코딩했다 -_-;;

		bool bHallu = pCreature->IsHallu();
		///if(g_pPlayer->IsLockMode() && pCreature == pCreature  && bHallu)
		//	g_pPlayer->UnSetLockMode(); 

		//
		//////////////////////////////////////////////////////

		bool bGildreDead = false;
		bool bSlayerPet_ShowTurret = false;
		GENERATE_OPTION  OutLineOption = GENERATE_ALL;
		// 2004, 7, 23 sobeit modify start

		// 2004.12.9 created exception processing function by sonee
		std::map<DWORD, DrawCreatureExceptionProc >::iterator itr = m_ExceptionProcMap.find( creature_type );

		if( m_ExceptionProcMap.end() != itr )
			itr->second( pCreature, action, frame, direction );

		switch(creature_type)
		{		
		case 717:
		case 721:
			if(pCreature->IsDead())
			{
				if(pCreature->GetActionCount()>=pCreature->GetActionCountMax())
					bGildreDead = true;
			}
			break;
		case 702:
		case 703:
		case 704:
		case 1562:
			if( action == ACTION_STAND || action == ACTION_MOVE )
				bSlayerPet_ShowTurret = true;
			break;
		case 726: // 성문 4종
		case 727:
		case 728:
		case 729:
			{
				MCreature* TempCreature = g_pZone->GetCreature(m_SelectCreatureID);
				if(TempCreature != NULL)
				{
					if(	TempCreature->GetCreatureType() == 726 ||
						TempCreature->GetCreatureType() == 727 ||
						TempCreature->GetCreatureType() == 728 ||
						TempCreature->GetCreatureType() == 729 )
					{
						if(creature_type == 726)
							OutLineOption = GENERATE_EXCEPT_RIGHT;
						else if(creature_type == 727 || creature_type == 728 )
							OutLineOption = GENERATE_EXCEPT_SIDE;
						else if(creature_type == 729)
							OutLineOption = GENERATE_EXCEPT_LEFT;
					}
				}
			}
			break;
		case 753 :
			if(pCreature->IsDead())
			{
				action = ACTION_DIE;
				if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
			}
			break;
		case 1358 :
			if(pCreature->IsDead())
			{
				action = ACTION_DIE;
			if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
	        }
			break;

		case CREATURETYPE_ROCKET_LUNCHER:// 로켓 런쳐
#if	__CONTENTS(__SECOND_RARE_SKILL)
		case CREATURETYPE_XRL_ROCKET_LUNCHER:
#endif //__SECOND_RARE_SKILL
			{
				if(direction == 0 || direction == 4)
				{
					pPoint->y -= g_nRocketPosValue[frame];
//					pPoint->y -= sin((double)pCreature->GetMoveCount()*3.141592/(double)pCreature->GetMoveCountMax())*5;
				}
				else 
				{
					pPoint->x -= g_nRocketPosValue[frame];
//					pPoint->x -= sin((double)pCreature->GetMoveCount()*3.141592/(double)pCreature->GetMoveCountMax())*5;
				}
			}
			frame = 0;
			action = ACTION_MOVE;
		
			break;
//20081124 승직 캐릭터가 변신했을때 마법 공격 액션이 들어오면 바꿔 준다.
		case CREATURETYPE_WER_WOLF://긴의빈땡鱗
			if(action == 113)
			{
				action = ACTION_ATTACK;
				pCreature->SetAction(action);
			}
			break;
		case 769:
			if(pCreature->IsDead())
				return;
			break;
		case 2004:
		case 2005:
		case 2006:
		case 2007:
			{
				if(pCreature->IsDead())
				{
					action = ACTION_DIE;
					direction = 0;
					if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
				}
				else
				{
					action = ACTION_STAND;
					frame = 0;
					direction = 0;
				}
			}
			break;
		case 809:	// 미클리즈 촉수는 땅에 박혀있기 때문에 -_-... ㅈㅅ
			{
				CSprite* pSprite = &m_EtcSPK[ SPRITEID_CREATURE_BURROW ];	
				
				//----------------------------------------
				// 좌표 보정
				//----------------------------------------
				int cx = 4;
				int cy = 4;
				pointTemp.x = pPoint->x + cx;
				pointTemp.y = pPoint->y + cy;

				m_pSurface->BltSprite(&pointTemp, pSprite);
			}
			break;
// 20081110 서큐버스를 밤낮에 따라 다른 그래픽으로 보이게 하는 테스트 코드
//		case 1055:
//			{
//				if(g_pGameTime->GetHour() > 0 && g_pGameTime->GetHour() <= 12 && creature_type != 1043)
//				{
//					creature_type = 1043;
//					pCreature->SetCreatureType(1043);
//				}
//				else if(g_pGameTime->GetHour() > 12 && g_pGameTime->GetHour() <= 24 && creature_type != 1055)
//				{
//					creature_type = 1055;
//					pCreature->SetCreatureType(1055);
//				}
//			}
		}
		// 2004, 7, 23 sobeit modify end
		
		// 2004, 11, 22, sobeit add start - set afire 관련
		//int		ComboActionCnt = pCreature->GetCombo();
		//int		MaxFrame       = pCreature->GetActionCountMax();
		if(pCreature->GetID() != g_pPlayer->GetID())
		{
			if(pCreature->GetSpecialActionInfo() == SKILL_SET_AFIRE || pCreature->GetSpecialActionInfo() == SKILL_SET_AFIRE_2 )
			{
				if(action == ACTION_ATTACK					||
				   action == ACTION_VAMPIRE_ATTACK_SLOW		||
				   action == ACTION_VAMPIRE_ATTACK_FAST		||
				   action == ACTION_VAMPIRE_ATTACK_2		||
				   action == ACTION_VAMPIRE_ATTACK_2_SLOW	||
				   action == ACTION_VAMPIRE_ATTACK_2_FAST	||
				   action == ACTION_VAMPIRE_ATTACK_3		||
				   action == ACTION_VAMPIRE_ATTACK_3_SLOW	||
				   action == ACTION_VAMPIRE_ATTACK_3_FAST)
				{
					action = ACTION_ATTACK;
					frame = 3+((frame)%4);
				}
			}
			// 콤보 Action 일때 처리  Sjheon 2005.06.10 Add
			//if(pCreature->IsAdvancementClass())  // 콤보 기술은 승직 케렉터 만 가능하다  Sjheon 2005.06.10
			//	frame = GetAdvancementComboFrame(pCreature ,frame , action , ComboActionCnt , MaxFrame) ; 
			// 콤보 Action 일때 처리  Sjheon 2005.06.10 End
		}
		else
		{
			if(pCreature->GetUsedActionInfo() == SKILL_SET_AFIRE || pCreature->GetUsedActionInfo() == SKILL_SET_AFIRE_2)
			{
				if(action == ACTION_ATTACK					||
				   action == ACTION_VAMPIRE_ATTACK_SLOW		||
				   action == ACTION_VAMPIRE_ATTACK_FAST		||
				   action == ACTION_VAMPIRE_ATTACK_2		||
				   action == ACTION_VAMPIRE_ATTACK_2_SLOW	||
				   action == ACTION_VAMPIRE_ATTACK_2_FAST	||
				   action == ACTION_VAMPIRE_ATTACK_3		||
				   action == ACTION_VAMPIRE_ATTACK_3_SLOW	||
				   action == ACTION_VAMPIRE_ATTACK_3_FAST)
				{
					action = ACTION_ATTACK;
					frame = 3+((frame)%4);
				}
			}
            // 콤보 Action 일때 처리  Sjheon 2005.06.10 Add
			//if(pCreature->IsAdvancementClass())  // 콤보 기술은 승직 케렉터 만 가능하다  Sjheon 2005.06.10
			//	frame = GetAdvancementComboFrame(pCreature ,frame , action , ComboActionCnt , MaxFrame) ; 
			// 콤보 Action 일때 처리  Sjheon 2005.06.10 End
		}
		// 2004, 11, 22, sobeit add end - set afire 관련
		// 성물은 무조건 아래, 크리스마스트리


		if(pCreature->IsFakeDie())
			frame = 0;
	
		int	creatureID			= pCreature->GetID();

		bool bPlayerVampire = 
				(*g_pCreatureSpriteTable)[SafeSpriteType0(creature_type)].IsPlayerVampireOnlySprite()
	//			&& (pCreature==g_pPlayer || pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
				&& (pCreature->GetActionCount()>=pCreature->GetActionCountMax()	)
//				&& !pCreature->IsExistAttachEffect()//20090302 이펙트가 coma가 있는데 비어 있을리가 있나요 따라서 제거
				&& pCreature->IsDead();

		bool bTeenVersion = 
				(
					g_pUserInformation->GoreLevel == false
					&& !(
					creature_type >= 526 && creature_type <= 549 || 
					creature_type >= 371 && creature_type <= 376 || 
					creature_type >= 560 && creature_type <= 563 || 
					creature_type == 482 || 
					creature_type == 650 || 
					creature_type == 819 ||	// 터프한 눈사람
					creature_type == 670 || 
					creature_type == 672 ||
					creature_type == 673 ||
					creature_type == 730 || // 웨이 포인트1
					creature_type == 731 || // 웨이 포인트2
					creature_type == 732 || // 웨이 포인트3
					creature_type == 636 // 생선 가게 아저씨

					)	// 성물 // 크리스 마스 트리
					|| creature_type >= 377 && creature_type <= 386	// 다크 가디언은 먼지로 표시
					|| creature_type == 480 
				)
				&&
				(
					pCreature->GetActionCount()>=pCreature->GetActionCountMax()/2
					&& pCreature->IsDead()
				)
				&&
				(
					creature_type < 431		// 바토리
					|| creature_type > 436	// 테페즈
				);

		//---------------------------------------------------------
		// PC Vampire인 경우 죽으면 '먼지'로 표현한다.
		//
		// player는 죽어도 아이템으로 변하지 않기 때문에..
		// 틴버전에선 모든 시체를 '먼지'로 표현한다.
		//---------------------------------------------------------	
		if (bPlayerVampire || bTeenVersion || bGildreDead )
		{
			{
				CSprite* pSprite;
				
				if(pCreature->IsSlayer())
				{
					pSprite = &m_EtcSPK[ SPRITEID_SLAYER_CROSS ];
					// 좌표 보정
					pointTemp.x = pPoint->x - 5+20;
					pointTemp.y = pPoint->y - 30;				
				}
				//else if((*g_pCreatureSpriteTable)[(*g_pCreatureTable)[creature_type].SpriteType].IsPlayerVampireOnlySprite())
				else if((*g_pCreatureSpriteTable)[SafeSpriteType0(creature_type)].IsPlayerVampireOnlySprite())
				{
					pSprite = &m_EtcSPK[ SPRITEID_VAMPIRE_DUST ];
					// 좌표 보정
					pointTemp.x = pPoint->x - 5;
					pointTemp.y = pPoint->y;				
				}
				else
				{
					pSprite = &m_EtcSPK[ SPRITEID_MONSTER_DUST ];
					// 좌표 보정
					pointTemp.x = pPoint->x - 5;
					pointTemp.y = pPoint->y;				
				}

				//---------------------------------------- 	
				// 크리처 이름을 출력하자 
				//---------------------------------------- 	
				if(pCreature->IsShowNameCreature())
				{
					AddDrawNameCreature(pPoint, pCreature, true);
				}
				else if (m_SelectItemID == creatureID || m_SelectCreatureID == creatureID)
				{
					m_SOMOutlineColor = m_ColorOutlineItem;
					
					m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
					
					m_SOM.Generate();

					m_pSurface->BltSpriteOutline( &m_SOM, m_SOMOutlineColor );
					
					//CalculateChatStringPoint(pPoint, pCreature);
					AddDrawNameCreature(pPoint, pCreature, true);
					m_pSelectedCreature = pCreature;
				}
				else
				{
					m_pSurface->BltSprite(&pointTemp, pSprite);	
				}			

				//------------------------------------------------
				// 죽은 캐릭터이면 선택 사각형을
				// 한 Tile 크기로 제한한다.
				//------------------------------------------------
				RECT rect;
				rect.left	= pPoint->x;
				rect.top	= pPoint->y;
				rect.right	= pPoint->x + TILE_X;
				rect.bottom = pPoint->y + TILE_Y;
				if(FrameIndex ==0 )	
					pCreature->SetScreenRect( &rect );
				
	#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("EndDrawCreature-S");
	#endif
				
				
				if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()/2)
				{
					if (pCreature->IsExistAttachEffect())
					{	
	#ifdef OUTPUT_DEBUG_DRAW_PROCESS
						DEBUG_ADD("DrawAttachEffect");
	#endif
						
						pointTemp = *pPoint;
						//pointTemp.x += pCreature->GetSX();
						//pointTemp.y += pCreature->GetSY();
						
						DrawAttachEffect(&pointTemp, 
							pCreature->GetAttachEffectIterator(), 
							pCreature->GetAttachEffectSize(),
							pCreature,
							1);	// foreground출력
						
	#ifdef OUTPUT_DEBUG_DRAW_PROCESS
						DEBUG_ADD("DAE ok");
	#endif
					}
				}
				m_pSurface->Unlock();
				const int firstY = DrawChatString(&pointTemp, pCreature, pCreature->GetChatColor());//CDirectDraw::Color(26,26,26));		
				pointTemp.y = firstY;
				DrawCreatureHPModify(&pointTemp, pCreature);
				m_pSurface->Lock();	
				Phoenix_TextPerf_CountSurfaceUnlockLock();
				return;
			}
		}

		//------------------------------------------------
		// Slayer인 경우 Darkness 안을 볼 수 없다.
		//------------------------------------------------
		if (
			!( creature_type >= 526 && creature_type <= 549 || 
			   creature_type == 670 || 
			   creature_type == 672 ||
			   creature_type == 673 ||
			   creature_type >= 371 && creature_type <= 376 || 
			   creature_type >= 560 && creature_type <= 563
			 ) &&
			(pCreature->IsInDarkness() && !pCreature->IsNPC() && !g_pPlayer->IsVampire() && g_pPlayer!=pCreature
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_LIGHTNESS )
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)//RALLSER 籃絞棍충옵鹿였돕絞쟁
			&& g_pZone->GetID() != 3001
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
	#ifdef __METROTECH_TEST__
			&& !g_bLight 
	#endif
			)
			)
		{
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("EndDrawCreature-D");
			#endif

			return;
		}

		//------------------------------------------------
		//
		// 땅 속에 파묻힌 애들은 고정된 그림..
		//
		//------------------------------------------------
		// 2004, 04, 24 sobeit modify - 질드레 레어 ghost는 땅속 크리쳐지만 고정된 그림이 아님..^^;
		if (pCreature->IsUndergroundCreature() && pCreature->GetCreatureType() != CREATURETYPE_GHOST)
		{
			DrawUndergroundCreature(pPoint, pCreature );
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD( "End DrawCreature" );
			#endif		

			return;
		}

		//----------------------------------------------------------
		// 머리가 없는 경우
		//----------------------------------------------------------
		if (!pCreature->HasHead()
			&& pCreature->GetActionCount()==pCreature->GetActionCountMax()
			&& g_pUserInformation->GoreLevel == true)
		{
			if (action==ACTION_DIE)
			{
				// 실제로는..마지막 frame + 1이다.
				//frame	= (*g_pCreatureTable)[creature_type].GetActionCount( action );
				frame	= GetCreatureActionCountMax( pCreature, action);
			}
		}

		// Teen버전인 경우 죽는 동작 애니메이션 안함
		// 처음엔 마지막프레임전까지의 동작만 0으로 세팅했는데 저 위쪽에서 재로 찍을 경우 return해버려서 의미없음
		if(g_pUserInformation->GoreLevel == false && action == ACTION_DIE
			&&	(
					creature_type < 431		// 바토리
					|| creature_type > 436	// 테페즈
				)
			)
		{
			frame = 0;
		}
		
		// Creature가 존재하는 높이만큼 빼준다.
		pPoint->y -= pCreature->GetZ();

		if(!pCreature->IsNPC() &&  pCreature->HasEffectStatus(EFFECTSTATUS_DIVINE_GUIDANCE))
		{	
			if(isSlayerCharacter)
			{
				if( pCreature->IsAdvancementClass() && !bHallu )
					DrawDivineGuidanceAdvancementClassSlayerCharacter( pPoint, pCreature, direction );
				else
					DrawDivineGuidanceSlayerCharacter( pPoint, pCreature, direction );
			}
			else if(isOustersCharacter)
			{
				if( pCreature->IsAdvancementClass() && !bHallu )
					DrawDivineGuidanceAdvancementClassOustersCharacter( pPoint, pCreature, direction );
				else
					DrawDivineGuidanceOustersCharacter( pPoint, pCreature, direction );				
			}
			else if(isVampireCharacter)
			{
				if( pCreature->IsAdvancementClass() && !bHallu )
					DrawDivineGuidanceAdvancementClassVampireCharacter( pPoint, pCreature, direction, body );
				else
					DrawDivineGuidanceVampireCharacter( pPoint, pCreature, direction, body );
			}
			else
			{
				DrawDivineGuidanceCreature( pPoint, pCreature, direction, body );
			}
		}

		//----------------------------------------
		// 바닥에 붙은 Effect 출력
		//----------------------------------------
		if (pCreature->IsExistGroundAttachEffect())
		{				
			pointTemp = *pPoint;
			//pointTemp.x += pCreature->GetSX();
			//pointTemp.y += pCreature->GetSY();

			DrawAttachEffect(&pointTemp, pCreature->GetGroundAttachEffectIterator(), pCreature->GetGroundAttachEffectSize(), pCreature);
		}

		//---------------------------------------------------------------
		//
		//                  Background AttachEffectNode 출력
		//
		//---------------------------------------------------------------
		if (pCreature->IsExistAttachEffect())
		{				
	//		DEBUG_ADD("BackAttachEffect");

			pointTemp = *pPoint;
			//pointTemp.x += pCreature->GetSX();
			//pointTemp.y += pCreature->GetSY();

			DrawAttachEffect(&pointTemp, 
								pCreature->GetAttachEffectIterator(), 
								pCreature->GetAttachEffectSize(),
								pCreature,
								2);	// background출력

	//		DEBUG_ADD("BAE ok");
		}

		//---------------------------------------------------------------
		//
		//                  Mouse로 선택 --> 외곽선
		//
		//---------------------------------------------------------------

		//------------------------------------------------
		// Mouse가 가리키고 있는 Creature
		//------------------------------------------------
		
		if (m_SelectCreatureID == creatureID || OutLineOption)
		{			
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("Sel Creature");
			#endif

			// SpriteOutlineManager에 추가
			if (pCreature->IsNPC()							// NPC인 경우
				// 바토리인 경우는 안된다. 하드코딩 - -;;
				&& !(creature_type==217)
				|| IsRequestMode()								// trade mode인 경우
					// player만 된다.
					//pCreature->GetCreatureType()<=CREATURETYPE_VAMPIRE_FEMALE				
				//	&& (*g_pCreatureSpriteTable)[(*g_pCreatureTable)[creature_type].SpriteType].IsPlayerOnlySprite()
				&& (*g_pCreatureSpriteTable)[SafeSpriteType0(creature_type)].IsPlayerOnlySprite()
				|| !IsRequestMode()							// trade할려고 추적중인 creature인 경우
					&& g_pPlayer->IsRequestMode() 
					&& g_pPlayer->IsTraceCreature() 
					&& g_pPlayer->GetTraceID()==creatureID)	
			{
				m_SOMOutlineColor = m_ColorOutlineNPC;
			}
			else if (g_pObjectSelector->CanAttack( pCreature )
				|| creature_type==217)		// 바토리인 경우. 하드코딩 - -;;
			{
				m_SOMOutlineColor = m_ColorOutlineAttackPossible;
			}
			else
			{
				m_SOMOutlineColor = m_ColorOutlineAttackImpossible;
			}

			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("drawBody");
			#endif
			
			//----------------------------------------
			//
			//         몸이 있는 경우에 출력
			//
			//---------------------------------------- 		
//			if (!isSlayerCharacter && !isOustersCharacter && body != FRAMEID_NULL)
			if (body != FRAMEID_NULL)
			{
				bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
				bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
				bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;
				bool bGhost = pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST;
#if __CONTENTS(__FAST_TRANSFORTER)
				bool bFlitterMouse = pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE;
				bool bNede = pCreature->GetCreatureType() == CREATURETYPE_NEDE;
				bool bKltl = pCreature->GetCreatureType() == CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				bool bShapeOfDemon = pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON;
#endif //__SECOND_TRANSFORTER
				bool bTransform = bBat || bWolf || bWerWolf || bHallu || bGhost
#if __CONTENTS(__FAST_TRANSFORTER)
				|| bFlitterMouse
				|| bNede
				|| bKltl
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				|| bShapeOfDemon
#endif //__SECOND_TRANSFORTER
				;

				// body 수정 필요
				if(pCreature->IsAdvancementClass() && !bTransform)
				{
					if (Phoenix_CreatureDrawAudit_ShouldLog(creature_type))
					{
						Phoenix_CreatureDrawAudit_Log(
							"DrawCreature dispatch SelectedAdvVampire type=%d body=%d action=%d dir=%d frame=%d frameIndex=%d transform=%d hallu=%d",
							creature_type, body, action, direction, frame, FrameIndex,
							(int)bTransform, (int)bHallu);
					}
					DrawSelectedAdvancementVampireCreature(
						pPoint, pCreature, action, direction, frame, 0, FrameIndex );
				}
				else
				{
					bool player = pCreature->IsPlayerOnly() &&
								  !pCreature->IsOperator()	&&
								  !bTransform;

					if (Phoenix_CreatureDrawAudit_ShouldLog(creature_type))
					{
						Phoenix_CreatureDrawAudit_Log(
							"DrawCreature dispatch SelectedVampire type=%d body=%d action=%d dir=%d frame=%d frameIndex=%d player=%d transform=%d hallu=%d",
							creature_type, body, action, direction, frame, FrameIndex,
							(int)player, (int)bTransform, (int)bHallu);
					}
					DrawSelectedVampireCreature(
						pPoint, pCreature, action, direction, frame, body,
						FrameIndex, player);
				}
			}
			else
			{
				//----------------------------------------------------
				// 캐릭터의 선택 사각형 영역을 지워준다.
				//----------------------------------------------------
				pCreature->ClearScreenRect();
			}
			
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("draw clothes");
			#endif

			//----------------------------------------
			//
			//            복장 출력
			//
			//----------------------------------------
			// 모든 입을 수 있는 종류의 옷 중에서 
			// 입고 있는 옷을 확인해서 출력해줘야 한다.		
			if (isSlayerCharacter)
			{		
				// 2004, 9, 15, sobeit add start  - install turret
				if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
					DrawInstallTurret( pPoint, pCreature, action, direction, frame, FrameIndex );
				else if ( pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_MK1) )//  MK1꿎桿
					DrawInstallMK1( pPoint, pCreature, action, direction, frame, FrameIndex ); // MK1꿎桿
				else	
				{
					if( pCreature->IsAdvancementClass() && !bHallu )
						DrawSelectedAdvancementSlayerCreature(pPoint, pCreature, action, direction, frame );
					else
						DrawSelectedSlayerCreature( pPoint, pCreature, action, direction, frame );
				}
			}
			else if(isOustersCharacter)
			{
				if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_1))
					DrawOusterNewWings1( pPoint, pCreature, action, direction, frame );
				else if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_2))
					DrawOusterNewWings2( pPoint, pCreature, action, direction, frame );
				else
				{
					if( pCreature->IsAdvancementClass() && !bHallu )
						DrawSelectedAdvancementOustersCreature(pPoint, pCreature, action, direction, frame );
					else
						DrawSelectedOustersCreature( pPoint, pCreature, action, direction, frame );
				}
			}
			
			// 슬레 펫인 경우는 터렛 찍어줘야 된다-ㅅ-;;;; 하드하드..아아-_-/~
			if( bSlayerPet_ShowTurret )
			{
				DrawCentauroTurret( pPoint, pCreature, action, direction, frame , body);				
			}
			m_SOM.Generate(OutLineOption);
			
	//			// 2004, 8, 18 sobeit add start - 공성전 성문 타겟팅때문에 여차여차 추가..
	//		enum GENERATE_OPTION{
	//			GENERATE_ALL = 0,
	//			GENERATE_EXCEPT_LEFT,	// 왼쪽 아웃라인은 제외
	//			GENERATE_EXCEPT_RIGHT,  // 오른쪽 아웃라인은 제외
	//			GENERATE_EXCEPT_SIDE,	// 양 사이드 아웃라인은 제외
	//			// -_- 위,아래도 만들어 놔야 하나..-_-;
	//		};
	//		// 2004, 8, 18 sobeit add start
			//if (CDirect3D::IsHAL() || 
			//	DarkBits==0 || 
			//	g_pPlayer->GetID()==pCreature->GetID())
			//{	
			
				if (pCreature->IsInvisible() || pCreature->IsFakeDie()
	//				#ifdef OUTPUT_DEBUG
	//					||	g_pDXInput->KeyDown(DIK_RCONTROL)
	//				#endif
					)
				{
					// slayer의 invisible인 snipping
					if (pCreature->HasEffectStatus(EFFECTSTATUS_SNIPPING_MODE) || pCreature->HasEffectStatus(EFFECTSTATUS_SNIPPING_MODE_2))
					{
						m_pSurface->BltSpriteOutlineDarkness( &m_SOM,  m_SOMOutlineColor, SHIFT_SNIPPING );
					}
					m_pSurface->BltSpriteOutlineOnly( &m_SOM,  m_SOMOutlineColor );
				}
				else
				{
					// 어둡게 찍기
					if (pCreature->IsFade())
					{
						m_pSurface->BltSpriteOutlineDarkness( &m_SOM,  m_SOMOutlineColor, 1 );
					}
					else
					{
						m_pSurface->BltSpriteOutline( &m_SOM,  m_SOMOutlineColor );
					}
				}
				//m_pSurface->BltSpriteOutlineOnly( &m_SOM,  0xF800 );
			//}
			//else
			//{
			//	m_pSurface->BltSpriteOutlineDarkness( &m_SOM, m_SOMOutlineColor, DarkBits );
			//}		
				m_SOM.Clear();
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("drawSelOK");
			#endif
		}
		//------------------------------------------------
		//
		//       일반적인 Creature 출력
		//
		//------------------------------------------------
		else
		{
			
			//------------------------------------------------
			//
			//         Body출력
			//
			//------------------------------------------------
			//if (!isSlayerCharacter && !isOustersCharacter && body != FRAMEID_NULL)
			if (body != FRAMEID_NULL)
			{
				bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
				bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
				bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;
				bool bGhost = pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST;
#if __CONTENTS(__FAST_TRANSFORTER)
				bool bFlitterMouse = pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE;
				bool bNede = pCreature->GetCreatureType() == CREATURETYPE_NEDE;
				bool bKltl = pCreature->GetCreatureType() == CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				bool bShapeOfDemon = pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON;
#endif //__SECOND_TRANSFORTER
				bool bTransform = bBat || bWolf || bWerWolf || bHallu || bGhost
#if __CONTENTS(__FAST_TRANSFORTER)
				|| bFlitterMouse
				|| bNede
				|| bKltl
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				|| bShapeOfDemon
#endif //__SECOND_TRANSFORTER
				;


				if( pCreature->IsAdvancementClass() && !bTransform)
				{
					DrawAdvancementClassVampireCharacter(
						pPoint, pCreature, action, direction, frame, body, FrameIndex);
				}
				else
				{
					bool player = pCreature->IsPlayerOnly() &&
								  !pCreature->IsOperator()	&&
								  !bTransform;

					DrawVampireCharacter(
						pPoint, pCreature, action, direction, frame, body,
						FrameIndex, player);
				}
			}
			else
			{
				//----------------------------------------------------
				// 캐릭터의 선택 사각형 영역을 지워준다.
				//----------------------------------------------------
				pCreature->ClearScreenRect();		
			}
			
			//----------------------------------------
			//
			//            복장 출력
			//
			//----------------------------------------
			// 모든 입을 수 있는 종류의 옷 중에서 
			// 입고 있는 옷을 확인해서 출력해줘야 한다.
			//WORD colorSet = pCreature->GetAttachEffectColor();
							
			//if (pCreature->IsWear())
			if (isSlayerCharacter)
			{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
				if( (pCreature->IsAdvancementClass() || pCreature->IsRide_Fast_Transforter()) && !bHallu )
#else
				if( pCreature->IsAdvancementClass() && !bHallu )
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
					DrawAdvancementClassSlayerCharacter( pPoint, pCreature, action, direction, frame, FrameIndex );
				else
					DrawSlayerCharacter( pPoint, pCreature, action, direction, frame, FrameIndex );
			}
			else if(isOustersCharacter)
			{
				if( pCreature->IsAdvancementClass() && !bHallu )
					DrawAdvancementClassOustersCharacter( pPoint, pCreature, action, direction, frame );
				else
					DrawOustersCharacter( pPoint, pCreature, action, direction, frame );
			}

			// 슬레 펫인 경우는 터렛 찍어줘야 된다-ㅅ-;;;; 하드하드..아아-_-/~
			if( bSlayerPet_ShowTurret )
			{
				DrawCentauroTurret( pPoint, pCreature, action, direction, frame , body);				
			}
		}


		//---------------------------------------------------------------
		//
		//                  AttachEffectNode 출력
		//
		//---------------------------------------------------------------
		if (pCreature->IsExistAttachEffect())
		{	
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("DrawAttachEffect");
			#endif

			pointTemp = *pPoint;
			//pointTemp.x += pCreature->GetSX();
			//pointTemp.y += pCreature->GetSY();

			DrawAttachEffect(&pointTemp, 
								pCreature->GetAttachEffectIterator(), 
								pCreature->GetAttachEffectSize(),
								pCreature,
								1);	// foreground출력

			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("DAE ok");
			#endif
		}



		//pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		//pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
		bool bPlayerParty = (pCreature->IsPlayerParty() 
								&& pCreature!=g_pPlayer
								// 할루 걸리면 파티원을 못 알아본다.
								&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION));	

		//------------------------------------------------
		// Chat String 출력
		//------------------------------------------------

		pointTemp.x = pPoint->x;
		pointTemp.y = pPoint->y - pCreature->GetHeight();

		if(pCreature->IsShowNameCreature())
		{
			AddDrawNameCreature(&pointTemp, pCreature);
		}
		else if (m_SelectCreatureID == creatureID)
		{
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("SelectedCreature");
			#endif

			//CalculateChatStringPoint(&pointTemp, pCreature);
			AddDrawNameCreature(&pointTemp, pCreature);
			m_pSelectedCreature = pCreature;	
		}
		else
		{		
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("noSelCre");
			#endif

			//------------------------------------------------
			// 파티원인 경우 HP 출력해준다.
			//------------------------------------------------
			if (bPlayerParty)
			{			
				DrawPartyHP( &pointTemp, pCreature );

				pointTemp.y = pPoint->y - pCreature->GetHeight() - m_EtcSPK[SPRITEID_PARTY_HP].GetHeight();
			}
			else
			{
				pointTemp.y = pPoint->y - pCreature->GetHeight();
			}

			pointTemp.x = pPoint->x;			

			//
			// 이거 부하가 꽤 걸린다... 쩝~~
			//
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("DrawChat");
			#endif

			m_pSurface->Unlock();
			const int firstY = DrawChatString(&pointTemp, pCreature, pCreature->GetChatColor());//CDirectDraw::Color(26,26,26));		
			pointTemp.y = firstY;
			DrawCreatureHPModify(&pointTemp, pCreature);
			m_pSurface->Lock();	
			Phoenix_TextPerf_CountSurfaceUnlockLock();

			// 2004, 8, 18, sobeit add start - 공성전 길드마크 표시
			DrawGuildMarkInSiegeWar(pCreature,pointTemp.y);
			// 2004, 8, 18, sobeit add end
		}	

		//------------------------------------------------
		// 파티원의 시야를 보여준다.
		//------------------------------------------------
		if (bPlayerParty)
		{
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("partySight");
			#endif

			int pX = pCreature->GetPixelX() - m_FirstZonePixel.x + TILE_X_HALF;
			int pY = pCreature->GetPixelY() - m_FirstZonePixel.y - TILE_Y;

			//int addLight = (pCreature->GetCreatureType()==CREATURETYPE_BAT ? 3 : 0);
	//		int addLight = (pCreature->IsFlyingCreature() ? 3 : 0);
			// player만큼의 시야라고 생각한다. - -;
			int creatureLight = g_pPlayer->GetLightSight();// + addLight);
			
			if (CDirect3D::IsHAL())
			{
				AddLightFilter3D( pX, 
									pY - (pCreature->IsFlyingCreature()? 72:0 ),	//g_pPlayer->GetZ(), 
									creatureLight, 
									false,	// screenPixel좌표			
									true);	// 무조건 출력해야하는 빛	

				// 오토바이 불빛
				ADD_MOTORCYCLE_LIGHT_XY_3D( pCreature, pX, pY, true );
			}
			else
			{
				AddLightFilter2D( pX, 
									pY - pCreature->GetZ(), 
									creatureLight,  
									false,	// screenPixel좌표			
									true);	// 무조건 출력해야하는 빛

				// 오토바이 불빛
				ADD_MOTORCYCLE_LIGHT_XY_2D( pCreature, pX, pY, true );
			}
		}
		//------------------------------------------------
		// 파티원 아닌 경우에...
		//------------------------------------------------
		else
		{
			#ifdef OUTPUT_DEBUG_DRAW_PROCESS
				DEBUG_ADD("noPParty?");
			#endif

			// 오토바이 불빛
			if (CDirect3D::IsHAL())
			{		
				ADD_MOTORCYCLE_LIGHT_3D( pCreature, false );
			}
			else
			{
				ADD_MOTORCYCLE_LIGHT_2D( pCreature, false );
			}
		}

		if(gpC_item != NULL && pCreature != NULL && pCreature->GetHeadSkin() != 0)
		{
			int spriteID = pCreature->GetHeadSkin();
			int pX = pCreature->GetPixelX() - m_FirstZonePixel.x + TILE_X_HALF-gpC_item->GetWidth(spriteID)/2;
			int pY = pCreature->GetPixelY() - m_FirstZonePixel.y - TILE_Y -18-gpC_item->GetHeight(spriteID);
			CIndexSprite::SetUsingColorSet(377, 0);
			gpC_item->BltLocked(pX, pY, spriteID);
		}
	}
	
	if(g_pUserInformation->attrOperator.GetValue() &&
	   g_pOperatorOption->bShowCreatureInfo)
	{
		char str[128];
		sprintf(str, "name=%s, oid=%d", pCreature->GetName(), (int)pCreature->GetID());

		CPoint pt(pCreature->GetPixelX() - m_FirstZonePixel.x,
				  pCreature->GetPixelY() - pCreature->GetHeight() - m_FirstZonePixel.y);
		
		DRAWTEXT_NODE* pNode = new DRAWTEXT_NODE_HEAP(pt.x, pt.y, str, 0x00FFFFFF, 3);

		AddText( pNode );
	}


	__END_PROFILE("DrawCreature")

	#ifdef OUTPUT_DEBUG_DRAW_PROCESS
		DEBUG_ADD( "End DrawCreature" );
	#endif
//	DEBUG_ADD("[DrawCreature] End");

}

void	MTopView::DrawSelectedSlayerCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;

	action = SlayerActionSpecialization(pCreatureWear, action);

	// Creature의 Action에 맞는 add-on을 출력한다.
	//action = pCreature->GetAction();	
	WORD clothes;
	BYTE clothesType;
	
	for (int i=0; i<ADDON_MAX; i++)
	{
		// Creature의 현재 방향에 따라서...
		// 옷을 출력해주는 순서가 다를 수 있다.
		clothesType = MCreatureWear::s_AddonOrder[pCreature->GetDirection()][i];
		
		// i번째 종류의 옷을 입고 있다면 출력해 준다.
		const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(clothesType);
		
		if (addonInfo.bAddon)
		{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
			if (ShouldSkipClassicAddonMotorDraw(addonInfo.ItemClass, addonInfo.ItemType))
				continue;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
			clothes = addonInfo.FrameID;

			CCreatureFramePack& addonFPK = pCreatureWear->IsMale() ? m_AddonManFPK : m_AddonWomanFPK;
			CIndexSpritePack&	addonSPK = pCreatureWear->IsMale() ? m_AddonManSPK : m_AddonWomanSPK;
			
			FRAME_ARRAY &FA = addonFPK[clothes][action][direction];
			
			// 있는 동작인 경우
			if (FA.GetSize() > frame)
			{
				CFrame &Frame = FA[frame];					
				int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
				int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
				int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

				Assert(sprite != SPRITEID_NULL && "MTopView::DrawSelectedSlayerCreature() : 슬레이어를 그리려는데 spriteID가 NULL이삼");
				
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
				
				CIndexSprite* pSprite = &addonSPK[ sprite ];
				
				// 복장Sprite가 초기화 되지 않은 경우
				//						if (pSprite->IsNotInit())
				//						{
				//							LoadFromFileAddonSPK( clothes, action );
				//							//continue;
				//						}
				
				//---------------------------------------- 
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				pCreature->AddScreenRect( &rect );
				
				// SpriteOutlineManager에 추가
				//CIndexSprite::SetUsingColorSet((pCreature->GetID()+3*(clothesType,1),7)%MAX_COLORSET, ((pCreature->GetID()*pCreature->GetID(),7*(clothesType+2)))%MAX_COLORSET);
				//CIndexSprite::SetUsingColorSet((c1+3*(clothesType+1)+7)%MAX_COLORSET, ((c2*c2+7*(clothesType+2)))%MAX_COLORSET);
				//---------------------------------------- 
				// 캐릭터 부분 색깔이 바뀌는 경우
				//---------------------------------------- 
				if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
				{
					m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
				}
				//---------------------------------------- 
				// 원래색대로..
				//---------------------------------------- 
				else
				{
					int colorSet1 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet1);
					int colorSet2 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet2);

					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
				}
			}
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
					//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
					//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
					if ( clothesType == ADDON_MOTOR 
						&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
						&& (addonInfo.ItemType == 8
#if __CONTENTS(__SECOND_TRANSFORTER)
						||addonInfo.ItemType == 9
						||addonInfo.ItemType == 10
						||addonInfo.ItemType == 11
#endif //__SECOND_TRANSFORTER
						))
						break;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
		}
	}
}



void	MTopView::DrawSelectedOustersCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	//  챠크람이 있다
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	
	int tempAction = action;
	if(bChakram == false)
	{
		switch(tempAction)
		{
		case ACTION_STAND:
			tempAction = ACTION_OUSTERS_STAND;
			break;
			
		case ACTION_MOVE:
			tempAction = ACTION_OUSTERS_MOVE;
			break;
		}
	}
	
	// 몸을 찍고 챠크람을 찍는다
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	const MCreatureWear::ADDON_INFO& bootsAddonInfo = pCreatureWear->GetAddonInfo(ADDON_TROUSER);
	
	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		int clothes = addonInfo.FrameID;
		
		
		FRAME_ARRAY &FA = m_OustersFPK[clothes][tempAction][direction];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CIndexSprite* pSprite = &m_OustersSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 선택 사각형 영역 설정
			//---------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			pCreature->AddScreenRect( &rect );
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				CIndexSprite::SetUsingColorSetOnly( 0, pCreature->GetBodyColor1() );	// 머리색
				
				int coatColor = addonInfo.ColorSet2, Colorset;
				if( coatColor == QUEST_ITEM_COLOR || coatColor == UNIQUE_ITEM_COLOR )
					coatColor = MItem::GetSpecialColorItemColorset( coatColor );
				else if(coatColor >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					coatColor =  pCreature->GetItemOptionExtColorSet(coatColor) ;

				CIndexSprite::SetUsingColorSetOnly( 1, coatColor );	// 옷색
				if(bChakram == false)	// 리스틀릿을 차고 있으면
				{
					if(addonInfoChakram.bAddon)
					{
						Colorset = addonInfoChakram.ColorSet2;
						if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔								
					}
					else
					{
						Colorset = defaultCoatColor;								
						if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
				}
				else
				{
					Colorset = defaultCoatColor;
					if(addonInfoChakram.bAddon)
						Colorset = addonInfoChakram.ColorSet2;
					if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
				}
				
				if(bootsAddonInfo.bAddon)
				{
					Colorset = bootsAddonInfo.ColorSet2;
					if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 부츠색
				}
				else
				{
					Colorset = defaultCoatColor;
					
					if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 팔
				}
				
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
			}						
			
		}
	}
	
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		int clothes = addonInfoChakram.FrameID;
		
		FRAME_ARRAY &FA = m_OustersFPK[clothes][tempAction][direction];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CIndexSprite* pSprite = &m_OustersSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfoChakram.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				int colorset1, colorset2;
				
				colorset1 = addonInfoChakram.ColorSet1;
				colorset2 = addonInfoChakram.ColorSet2;
				if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
					colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
				else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;

				if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
					colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
				else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;
				
				CIndexSprite::SetUsingColorSet(colorset1, colorset2);
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
			}						
			
		}
	}
}

void	MTopView::DrawVampireCharacter( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex, bool player )
{
	//------------------------------------------------
	// 관 출력정보
	//------------------------------------------------
	int casketValue = pCreature->GetCasketCount();
	int creature_type = pCreature->GetCreatureType();
	const bool auditEnabled = (Phoenix_CreatureDrawAudit_ShouldLog(creature_type) != 0);
	if (auditEnabled)
	{
		Phoenix_CreatureDrawAudit_Log(
			"DrawVampire ENTER ptr=%p type=%d player=%d action=%d dir=%d frame=%d body=%d frameindex=%d",
			(void*)pCreature, creature_type, (int)player, action, direction, frame, body,
			frameindex);
	}
	bool bInCasket = pCreature->IsInCasket();
	bool bCasketOnly = (bInCasket && casketValue==0);
	if (auditEnabled)
	{
		Phoenix_CreatureDrawAudit_Log(
			"DrawVampire CASKET type=%d casketValue=%d inCasket=%d casketOnly=%d",
			creature_type, casketValue, (int)bInCasket, (int)bCasketOnly);
	}
	
	BOOL	bAffterEffect = FALSE ; 
	//------------------------------------------------------------
	// 잔상 그리는지 여부 처리 
	//------------------------------------------------------------
	if(pCreature->HasEffectStatus( EFFECTSTATUS_DARK_FORCE))
	{
			if( ((action >=  ACTION_MOVE				&& action <=  ACTION_ATTACK)  ||
				(action >=  ACTION_VAMPIRE_ATTACK_SLOW	&& action <=  ACTION_VAMPIRE_ATTACK_FAST) 
//				|| (action >=  ACTION_VAMPIRE_SKILL_ATTACK	&& action <=  ACTION_VAMPIRE_SKILL_ATTACK_FAST)
				)
				)
			{
				//bAffterEffect = TRUE ; 
			}
	}

	//if(direction != 6 && bAffterEffect)
	//	DrawAfterVampire( pPoint, pCreature, action, direction, frame , body );

	//----------------------------------------------------
	// 완전히 관만 출력하는 경우는 몸 출력을 안한다.
	//----------------------------------------------------
	if (!bCasketOnly)
	{
		/*
		if(action == ACTION_VAMPIRE_DRAIN)
		{
			action = ACTION_MAGIC;
			int maxframe = GetCreatureActionCountMax(pCreature, action);
			frame = frame % maxframe;
		}
		*/

		//////////////////////////////////////////////////////////////////////////
		// Set PC Vampire Coat Start - chyaya 2007.05.15

		int		drawAction	= action;
		int		drawBody	= body;

		if(player)
		{
			MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
			
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

			bool bCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT;
			
			if(bCoat)
			{
				drawBody	= addonInfo.FrameID;
			}

			BYTE actionStep = pCreature->GetActionStep();
	
			drawAction = MCreature::GetVampireActionStep(drawAction, actionStep);
		}


		CCreatureFramePack* pFramePack = NULL;

		if(player)
		{
			pFramePack = &m_VampireFPK;
		}
		else
		{
			pFramePack = &m_CreatureFPK;
		}

		// Set PC Vampire Coat End - chyaya 2007.05.15
		//////////////////////////////////////////////////////////////////////////

		if (auditEnabled)
		{
			int packSize = pFramePack ? (int)pFramePack->GetSize() : -1;
			int actionCount = -1;
			int directionCount = -1;
			int faSize = -1;
			if (pFramePack && drawBody >= 0 && drawBody < packSize)
			{
				ACTION_FRAME_ARRAY& auditActions = (*pFramePack)[(TYPE_FRAMEID)drawBody];
				actionCount = auditActions.GetSize();
				if (drawAction >= 0 && drawAction < actionCount)
				{
					DIRECTION_FRAME_ARRAY& auditDirections = auditActions[(BYTE)drawAction];
					directionCount = auditDirections.GetSize();
					if (direction >= 0 && direction < directionCount)
					{
						FRAME_ARRAY& auditFrames = auditDirections[(BYTE)direction];
						faSize = auditFrames.GetSize();
					}
				}
			}
			Phoenix_CreatureDrawAudit_Log(
				"DrawVampire PRE_FRAME_ACCESS type=%d player=%d packSize=%d drawBody=%d drawAction=%d dir=%d frame=%d actionCount=%d directionCount=%d faSize=%d",
				creature_type, (int)player, packSize, drawBody, drawAction, direction,
				frame, actionCount, directionCount, faSize);
		}

		FRAME_ARRAY& FA = (*pFramePack)[drawBody][drawAction][direction];//(*pFramePack)[drawBody][drawAction][direction];

		if (FA.GetSize() > frame)
		{
			CFrame& Frame =	FA[frame];

			int sprite	= Frame.GetSpriteID();	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
			int cx		= Frame.GetCX();	//m_CreatureFPK[body][action][direction][frame].GetCX(),
			int cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			if (auditEnabled)
			{
				Phoenix_CreatureDrawAudit_Log(
					"DrawVampire FRAME_OK type=%d sprite=%d cx=%d cy=%d faSize=%d",
					creature_type, sprite, cx, cy, (int)FA.GetSize());
			}

// #ifdef _DEBUG
// 			if(player)
// 			{
// 				if(drawAction == ACTION_ATTACK					||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_SLOW		||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_FAST		||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_2		||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_2_SLOW	||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_2_FAST	||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_3		||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_3_SLOW	||
// 				   drawAction == ACTION_VAMPIRE_ATTACK_3_FAST)
// 				{
// 					char szBuf[128];
// 					sprintf(szBuf, "ACTION=%d, FRAME=%d SPRITE=%d\n", drawAction, frame, sprite);
// 					OutputDebugString(szBuf);
// 				}
// 			}
// #endif
			
			// 좌표 보정
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

			CIndexSprite* pSprite = NULL;

			if(player)
			{
				pSprite = &m_VampireSPK[ sprite ];
			}
			else
			{
				pSprite = &GetCreatureSprite(sprite, drawBody);
			}
			if (auditEnabled)
			{
				Phoenix_CreatureDrawAudit_Log(
					"DrawVampire SPRITE_PTR type=%d sprite=%d drawBody=%d player=%d pSprite=%p isNotInit=%d",
					creature_type, sprite, drawBody, (int)player, (void*)pSprite,
					pSprite ? (int)pSprite->IsNotInit() : -1);
			}
			
			//---------------------------------------- 
			// 캐릭터 선택 사각형 영역 설정
			//---------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			if(frameindex==0 )
				pCreature->SetScreenRect( &rect );
			
			// Player가 추적중인 Character 표시
			//if (g_pPlayer->GetTraceCreatureID()==pCreature->GetID())			
			
			//CIndexSprite::SetUsingColorSet(c1%MAX_COLORSET, c2%MAX_COLORSET);
			
			//-----------------------------------------------------------
			// CutHeight
			//-----------------------------------------------------------
			bool bCutHeight = pCreature->IsCutHeight();
			int previousClipBottom;
			if (bCutHeight)
			{
				previousClipBottom = m_pSurface->GetClipBottom();
				m_pSurface->SetClipBottom( rect.bottom - TILE_Y_HALF );
				
				pointTemp.y += pCreature->GetCutHeightCount() - TILE_Y_HALF;
			}
			
			//-----------------------------------------------------------
			// 투명 상태 출력 			
			//-----------------------------------------------------------
			// vampire
			if (pCreature->IsFakeDie())
			{
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();

				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우					
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					// IndexSprite에도 EFFECT...넣고...
					// 스르륵~ 단계적으로 나타나게/없어지게 해야됨.. - -;
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
				}
				else
				{
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
						colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER

					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					int netValue = pCreature->GetFakeDieCount()/5;
					
					CSpriteSurface::s_Value1 = netValue;
					
					CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_NET );
					CIndexSprite::SetEffect( CIndexSprite::EFFECT_NET );
					
					m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
					
				}
			}
			else if (pCreature->IsInvisible())
			{
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();
			
				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우					
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					// IndexSprite에도 EFFECT...넣고...
					// 스르륵~ 단계적으로 나타나게/없어지게 해야됨.. - -;
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
				}
				else
				{
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
						colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER
					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

					CIndexSprite::SetUsingColorSetOnly(0, colorSet1);	// 피부
					CIndexSprite::SetUsingColorSetOnly(1, colorSet2);	// 옷
					CIndexSprite::SetUsingColorSetOnly(2, 390);			// 머리
					
					int wipeValue = pCreature->GetInvisibleCount();
					
					if (wipeValue==0)
					{
						m_pSurface->BltIndexSprite(&pointTemp, pSprite);							
					}
					else if (wipeValue==64)
					{
						CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_SIMPLE_OUTLINE );
						CIndexSprite::SetEffect( CIndexSprite::EFFECT_SIMPLE_OUTLINE );
						
						m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
					}
					else
					{
						CSpriteSurface::s_Value1 = wipeValue;
						
						CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_WIPE_OUT );
						CIndexSprite::SetEffect( CIndexSprite::EFFECT_WIPE_OUT );
						
						m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
						
						//CSpriteSurface::s_Value1 = wipeValue/10+1;
						
						//CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_NET );
						//CIndexSprite::SetEffect( CIndexSprite::EFFECT_NET );
						
						//m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
					}							
				}
			}					
			else
			{	
				//-----------------------------------------------------------
				// 빠르게 움직이는 중
				//-----------------------------------------------------------
				if (pCreature->IsFastMove() || pCreature->IsBloodyZenith())
				{
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;

					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

					
					CIndexSprite::SetUsingColorSetOnly(0, colorSet1);	// 피부
					CIndexSprite::SetUsingColorSetOnly(1, colorSet2);	// 옷
					CIndexSprite::SetUsingColorSetOnly(2, 390);			// 머리
					
					POINT pointTemp2;
					
					//CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_NET );
					//CIndexSprite::SetEffect( CIndexSprite::EFFECT_NET );
					
					int fastMoveShadowMax = 2;
					
					if(creature_type == CREATURETYPE_WILD_WOLF || creature_type == CREATURETYPE_DARK_WOLF)
						fastMoveShadowMax = 6;
					if(pCreature->IsBloodyZenith())
					{
						fastMoveShadowMax = pCreature->GetBloodyZenithCount();
						pCreature->UpdateBloodyZenith();
					}
					for(int fastMoveShadow = fastMoveShadowMax; fastMoveShadow > 0; fastMoveShadow--)
					{
						// 1 pixel skip
						pointTemp2.x = pointTemp.x - (pCreature->GetCX() >> 1)*fastMoveShadow;
						pointTemp2.y = pointTemp.y - (pCreature->GetCY() >> 1)*fastMoveShadow;
						if(pCreature->IsBloodyZenith())
						{
							pointTemp2.y -= sin((double)max(0, pCreature->GetMoveCount()-fastMoveShadow)*3.141592/(double)pCreature->GetMoveCountMax())*100;
						}
						
						//CSpriteSurface::s_Value1 = 1;
						//m_pSurface->BltIndexSpriteEffect(&pointTemp2, pSprite);
						m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite, min(2, fastMoveShadow));
					}
					
				}
				
				if(pCreature->IsBloodyZenith())
				{
					pointTemp.y -= sin((double)pCreature->GetMoveCount()*3.141592/(double)pCreature->GetMoveCountMax())*100;
				}
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST
#if __CONTENTS(__SECOND_TRANSFORTER) || __CONTENTS(__FAST_TRANSFORTER) 
						|| pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON || pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE || pCreature->GetCreatureType() == CREATURETYPE_NEDE  || pCreature->GetCreatureType() == CREATURETYPE_KLTL
#endif //__SECOND_TRANSFORTER
					)
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();
				
				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우					
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
				}
				//---------------------------------------- 
				// 정상적인 색깔 출력
				//---------------------------------------- 					
				else
				{


					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
						colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER
					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;


					CIndexSprite::SetUsingColorSetOnly(0, colorSet1);	// 피부
					CIndexSprite::SetUsingColorSetOnly(1, colorSet2);	// 옷
					CIndexSprite::SetUsingColorSetOnly(2, 390);			// 머리
					ShadowNAlphaCreatureDraw(pCreature, pPoint, pointTemp, pSprite, action, direction, frame, drawBody , FA);
				}
			}


			if (bCutHeight)
			{
				m_pSurface->SetClipBottom( previousClipBottom );
			}

			// 2004, 10, 28, sobeit add start  - 몬스터 킬 퀘스트 해당 몬스터에 표시.
			if(pCreature->IsAlive() && (g_CurrentFrame&0x04)&&UI_IsMonsterKillQuest_Monster(SafeSpriteType0(pCreature->GetCreatureType())))
			//if(pCreature->IsAlive() && (g_CurrentFrame&0x04))
			{
				UI_ShowTargetArrow(pPoint->x + 12 ,pPoint->y- pCreature->GetHeight()-20);
			}
		// 2004, 10, 28, sobeit add end


		}


		//////////////////////////////////////////////////////////////////////////
		// Draw PC Vampire Weapon Start - chyaya 2007.05.15
		
		if(player)	
		{
			MCreatureWear *pCreatureWear = dynamic_cast<MCreatureWear *>(pCreature);
			
			if(pCreatureWear && !pCreatureWear->IsGhost(2))
			{
				const MCreatureWear::ADDON_INFO& addonInfoWeapon = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
				bool bWeapon = addonInfoWeapon.bAddon && addonInfoWeapon.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON;

				const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

				if(bWeapon)
				{
					int drawWeapon = addonInfo.FrameID % 2;

					FRAME_ARRAY &FA = m_VampireFPK[drawWeapon][drawAction][direction];
					
					// 있는 동작인 경우
					if (FA.GetSize() > frame)
					{
						CFrame &Frame = FA[frame];					
						int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
						int cx		= Frame.GetCX();		//m_AddonFPK[clothes][action][direction][frame].GetCX();
						int cy		= Frame.GetCY();		//m_AddonFPK[clothes][action][direction][frame].GetCY();
						
						CIndexSprite* pSprite = &m_VampireSPK[ sprite ];					
						
						pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
						pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

						//---------------------------------------- 
						// 캐릭터 선택 사각형 영역 설정
						//---------------------------------------- 	
						rect.left	= pointTemp.x;
						rect.top	= pointTemp.y;
						rect.right	= rect.left + pSprite->GetWidth();
						rect.bottom = rect.top + pSprite->GetHeight();
						//				if(frameindex==0 )
						//					pCreature->SetScreenRect( &rect );
						//-----------------------------------------------------------
						// CutHeight
						//-----------------------------------------------------------
						bool bCutHeight = pCreature->IsCutHeight();
						int previousClipBottom;
						if (bCutHeight)
						{
							previousClipBottom = m_pSurface->GetClipBottom();
							m_pSurface->SetClipBottom( rect.bottom - TILE_Y_HALF );
							
							pointTemp.y += pCreature->GetCutHeightCount() - TILE_Y_HALF;
						}
						   
						int colorset1, colorset2;
						colorset1 = addonInfoWeapon.ColorSet1;
						colorset2 = addonInfoWeapon.ColorSet2;
						
						if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
							colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
						else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;  //MItem::GetSpecialColorItemColorset( Colorset );
						
						if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
							colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
						else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;  //MItem::GetSpecialColorItemColorset( Colorset );
						
						CIndexSprite::SetUsingColorSet(colorset1, colorset2);


						// 어둡게 찍기
						if (pCreature->IsFade())
						{
							m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
						}
						else
						{
							m_pSurface->BltIndexSprite(&pointTemp, pSprite);
						}
						
						if (bCutHeight)
						{
							m_pSurface->SetClipBottom( previousClipBottom );
						}
						
					} // if (FA.GetSize() > frame)

				} // if(bWeapon)

			} // if(pCreatureWear && && !pCreatureWear->IsGhost(2))

		} // if(player)

		// Draw PC Vampire Weapon End - chyaya 2007.05.15
		//////////////////////////////////////////////////////////////////////////
		
	}
		
	//----------------------------------------------------
	// 관 출력
	//----------------------------------------------------
	if (bInCasket)
	{
		// 그림자 때문에 *2해야한다.
		int casketID = SPRITEID_CASKET_1 + pCreature->GetCasketType()*2;
		
		if (casketID < m_EtcSPK.GetSize())
		{
			CSprite* pSprite = &m_EtcSPK[casketID];
			
			if (pSprite!=NULL)
			{
				// tile의 중심에서 세운다.
				pointTemp.x = pPoint->x + TILE_X_HALF - (pSprite->GetWidth()>>1);
				pointTemp.y = pPoint->y + TILE_Y - pSprite->GetHeight();
				
				//---------------------------------------- 
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				if(frameindex==0 )
					pCreature->SetScreenRect( &rect );
				
				if (casketValue==0)
				{
					m_pSurface->BltSprite(&pointTemp, pSprite);						
				}
				else if (casketValue==64)
				{
					CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_SIMPLE_OUTLINE );
					
					m_pSurface->BltSpriteEffect(&pointTemp, pSprite);
				}
				else
				{
					CSpriteSurface::s_Value1 = casketValue;
					
					CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_WIPE_OUT );
					
					m_pSurface->BltSpriteEffect(&pointTemp, pSprite);
				}
			}
		}
	}		
}





void	MTopView::DrawOustersCharacter( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	//  챠크람이 있다
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	
	int tempAction = action;
	if(bChakram == false)
	{
		switch(tempAction)
		{
		case ACTION_STAND:
			tempAction = ACTION_OUSTERS_STAND;
			break;
			
		case ACTION_MOVE:
			tempAction = ACTION_OUSTERS_MOVE;
			break;
		}
	}
	
	// 몸을 찍고 챠크람을 찍는다
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	const MCreatureWear::ADDON_INFO& bootsAddonInfo = pCreatureWear->GetAddonInfo(ADDON_TROUSER);
	
	BOOL	bAffterEffect  = FALSE ; 

	//------------------------------------------------------------
	// 잔상 그리는지 여부 처리 
	//------------------------------------------------------------
	if ( (pCreature->HasEffectStatus( EFFECTSTATUS_NATURAL_FORCE)	|| 
		  pCreature->HasEffectStatus( EFFECTSTATUS_FIRE_FORCE)		||
		  pCreature->HasEffectStatus( EFFECTSTATUS_EARTH_FORCE) )
	)
	{
			if( ((action >=  ACTION_MOVE				&& action <=  ACTION_ATTACK)  ||
				(action >=  ACTION_OUSTERS_ATTACK_SLOW	&& action <=  ACTION_OUSTERS_CHAKRAM_FAST)  ||
				(action >=  ACTION_OUSTERS_MOVE)
				) 
			)
			{
				//bAffterEffect = TRUE ; 
			}
	}


//	if(direction != 6 && bAffterEffect)
//		DrawAfterOusters( pPoint, pCreature, action, direction, frame );

	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		//				DEBUG_ADD("[DrawCreature] DrawCloth");
		int clothes = addonInfo.FrameID;
		
		
		FRAME_ARRAY &FA = m_OustersFPK[clothes][tempAction][direction];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CIndexSprite* pSprite = &m_OustersSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 선택 사각형 영역 설정
			//--x-------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			pCreature->AddScreenRect( &rect );
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
					addonInfo.EffectColorSet);
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
//				신규 이동수단일 경우... BodyColor1 대신.. WingColor를 '옷색'(1)에 지정하고, 바로 그려준다.
				if (pCreature->GetMoveDevice () == MCreature::MOVE_DEVICE_WING_SYLPH)
				{

					CIndexSprite::SetUsingColorSetOnly( 1, pCreature->GetWingColor());
					// 어둡게 찍기
					if (pCreature->IsFade())
					{
						m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
					}
					else
					{
						m_pSurface->BltIndexSprite(&pointTemp, pSprite);
					}
				}
#endif //__FAST_TRANSFORTER || __SECOND_TRANSFORTER

				CIndexSprite::SetUsingColorSetOnly( 0, pCreature->GetBodyColor1() );	// 머리색
				
				int Colorset;
				
				Colorset = addonInfo.ColorSet2;
				//Colorset = 495 ; 
				if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR)
					Colorset =  MItem::GetSpecialColorItemColorset( Colorset );
				else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;  //MItem::GetSpecialColorItemColorset( Colorset );
				CIndexSprite::SetUsingColorSetOnly( 1, Colorset );	// 옷색
				
				if(bChakram == false)	// 리스틀릿을 차고 있으면
				{
					if(addonInfoChakram.bAddon)
					{
						Colorset = addonInfoChakram.ColorSet2;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;  //MItem::GetSpecialColorItemColorset( Colorset );
						
						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
					else
					{
						Colorset = defaultCoatColor;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );

						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;  //MItem::GetSpecialColorItemColorset( Colorset );

						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
				}
				else
				{
					Colorset = defaultCoatColor;
					if(addonInfoChakram.bAddon)
						Colorset = addonInfoChakram.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;  //MItem::GetSpecialColorItemColorset( Colorset );

					CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					
				}
				
				if(bootsAddonInfo.bAddon)
				{
					Colorset = bootsAddonInfo.ColorSet1;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;  //MItem::GetSpecialColorItemColorset( Colorset );

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 부츠색
				}
				else
				{
					Colorset = defaultCoatColor ;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;  //MItem::GetSpecialColorItemColorset( Colorset );

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 팔
				}
				
				// 어둡게 찍기
				if (pCreature->IsFade())
				{
					m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
				}
				else
				{
					m_pSurface->BltIndexSprite(&pointTemp, pSprite);
				}
				//if(direction == 6 && bAffterEffect)
				//	DrawAfterOusters( pPoint, pCreature, action, direction, frame );
			}			
		}
	}
	
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		//				DEBUG_ADD("[DrawCreature] DrawChakram");
		int clothes = addonInfoChakram.FrameID;
		
		FRAME_ARRAY &FA = m_OustersFPK[clothes][tempAction][direction];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			//					DEBUG_ADD_FORMAT("[DrawCreature] FA.GetSize() > frame %d %d", FA.GetSize(), frame);
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CIndexSprite* pSprite = &m_OustersSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfoChakram.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
					addonInfoChakram.EffectColorSet);
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				int colorset1, colorset2;
				colorset1 = addonInfoChakram.ColorSet1;
				colorset2 = addonInfoChakram.ColorSet2;
				
				if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
					colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
				else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;  //MItem::GetSpecialColorItemColorset( Colorset );

				if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
					colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
				else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;  //MItem::GetSpecialColorItemColorset( Colorset );

				CIndexSprite::SetUsingColorSet( colorset1, colorset2 );
				// 어둡게 찍기
				if (pCreature->IsFade())
				{
					m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
				}
				else
				{
					m_pSurface->BltIndexSprite(&pointTemp, pSprite);
				}
				//if(direction == 6 && bAffterEffect)
				//	DrawAfterOusters( pPoint, pCreature, action, direction, frame );
			}						
			
		}
		else
		{
			DEBUG_ADD_FORMAT("[DrawCreature] FA.GetSize() <= frame %d %d %d %d", FA.GetSize(), frame, pCreatureWear->GetActionCount(), pCreatureWear->GetActionCountMax());
			//					DEBUG_ADD_FORMAT("[DrawCreature] FA.GetSize() <= frame %d %d %d", clothes, tempAction, direction);
		}
	}
	
#ifdef OUTPUT_DEBUG_DRAW_PROCESS
	DEBUG_ADD("dcOK");
#endif
}
//52011

void	MTopView::DrawAdvancementClassOustersCharacter( 
				POINT* pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	//  챠크람이 있다
	if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_1))
	{
		DrawOusterNewWings1( pPoint, pCreature, action, direction, frame );
	}
	else if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_2))
	{
		DrawOusterNewWings2( pPoint, pCreature, action, direction, frame );
	}
	else
	{
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	bool bNewChakram = bChakram && (addonInfoChakram.ItemType == 21 || addonInfoChakram.ItemType == 22 || addonInfoChakram.ItemType == 23 || addonInfoChakram.ItemType == 24 || addonInfoChakram.ItemType == 25 || addonInfoChakram.ItemType == 26);	//by rallser201
	bool b231NewChakram = bChakram && (addonInfoChakram.ItemType == 27);

	int tempAction = ConvAdvancementOustersActionFromOusterAction( action, bChakram );

	if( tempAction == -1 )
		return;
	else
		tempAction -= ADVANCEMENT_ACTION_START;
	
	// 2005.08.12 Sjheon 콤보 스킬 관련 Add
	/*
	int		iComboCnt = pCreature->GetCombo();

	if((action >= (ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW - ADVANCEMENT_ACTION_START))  && (iComboCnt < 4)) //  ; && iComboCnt > 0 ))
	{
		//frame -= 8 ; 
		WORD  fCreatureType	= action - 	(ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW  - ADVANCEMENT_ACTION_START)  ; 

		WORD  fWeaponSpeed = pCreature->GetWeaponSpeed() ;

		WORD Frame = AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][iComboCnt - 1] + 1 ; 

		if(frame > (Frame - 1) )
		{
			frame -= Frame  - ((iComboCnt -1) *2); 
			action += 3 ; 
		}
	}
	*/
	// 2005.08.12 Sjheon 콤보 스킬 관련 End	
	

	// 몸을 찍고 챠크람을 찍는다
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	const MCreatureWear::ADDON_INFO& bootsAddonInfo = pCreatureWear->GetAddonInfo(ADDON_TROUSER);

	BOOL	bAffterEffect  = FALSE ; 

	//------------------------------------------------------------
	// 잔상 그리는지 여부 처리 
	//------------------------------------------------------------
	if ( (pCreature->HasEffectStatus( EFFECTSTATUS_NATURAL_FORCE)  || //by rallser 팽폭
		  pCreature->HasEffectStatus( EFFECTSTATUS_FIRE_FORCE)	||
		  pCreature->HasEffectStatus( EFFECTSTATUS_EARTH_FORCE))
	)
	{
		int AdvancementAction = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP + action ; 

		//if( ((AdvancementAction >=  ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_MOVE	  && 
		//	 AdvancementAction <=  ACTION_ADVANCEMENT_OUSTERS_WRISTLET_MOVE)  ||
		//	(AdvancementAction >=  ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW      && 
		//	 AdvancementAction <=  ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST)     ||
		//	(AdvancementAction >=  ACTION_ADVANCEMENT_OUSTERS_MAGIC_ATTACK)	  ||	
		//	(AdvancementAction >=  	ACTION_ADVANCEMENT_OUSTERS_MAGIC)		  ||	
		//	(AdvancementAction >=  	ACTION_ADVANCEMENT_OUSTERS_FAST_MOVE))
		//)	
		//{
			//bAffterEffect  = TRUE ; 
		//}

	}
	//------------------------------------------------------------

	//if(direction != 6 && bAffterEffect)
	//	DrawAfterACOusters( pPoint, pCreature, action, direction, frame );
	

	bool bNewCoat = addonInfo.ItemClass == ITEM_CLASS_OUSTERS_COAT && (addonInfo.ItemType == 18 || addonInfo.ItemType == 19 || addonInfo.ItemType == 20 || addonInfo.ItemType == 21);//by rallser201 ouster
	bool b231NewCoat = addonInfo.ItemClass == ITEM_CLASS_OUSTERS_COAT && (addonInfo.ItemType == 22 || addonInfo.ItemType == 23) ;

	tempAction = GetAdvancementOustersAction171[tempAction];

	if(tempAction == -1)	return;

	if(tempAction == 19 || tempAction == 18 || tempAction == 20 || tempAction == 21)
	{
		bNewCoat = false;
		b231NewCoat = false;
	}
	CCreatureFramePack& oustersFPK = b231NewCoat ? m_AdvancementOusters231FPK :(bNewCoat ? m_AdvancementOusters201FPK : m_AdvancementOusters171FPK);
	CIndexSpritePack& oustersSPK = b231NewCoat ? m_AdvancementOusters231SPK:(bNewCoat ? m_AdvancementOusters201SPK : m_AdvancementOusters171SPK);
	

	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{

		int clothes = 1;


		if(bNewCoat == true)
		{
			if(addonInfo.ItemType == 18 || addonInfo.ItemType == 19 || addonInfo.ItemType == 20 || addonInfo.ItemType == 21)
			
				clothes = 2;
		
             
		}
		else if(bNewCoat == false)
		{
		  if(addonInfo.ItemType == 17)
	        
			  	 clothes = 3;
		  else if(b231NewCoat)
		  {
			  if(addonInfo.ItemType == 22 || addonInfo.ItemType == 23)
				  clothes = 2;
		  } 
			else 

				clothes = 2;
		}
		//	else if(bNewCoat == false && bNewCoat2 == true)
			
			
		//	  if(addonInfo.ItemType == 16)
		//		oustersFPK = ousters2FPK;
	     //	    oustersSPK = ousters2SPK;
		//		clothes = 3;

			

		
		//FRAME_ARRAY &FA = m_OustersFPK[clothes][tempAction][direction];
		FRAME_ARRAY &FA = oustersFPK[ clothes ][ tempAction ][ direction ];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			//CIndexSprite* pSprite = &m_OustersSPK[ sprite ];					
			CIndexSprite* pSprite = &oustersSPK[ sprite ];
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 선택 사각형 영역 설정
			//---------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			pCreature->AddScreenRect( &rect );
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
					addonInfo.EffectColorSet);
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				CIndexSprite::SetUsingColorSetOnly( 0, pCreature->GetBodyColor1() );	// 머리색
				
				int Colorset;
				
				Colorset = addonInfo.ColorSet2;
				
				if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
					Colorset = MItem::GetSpecialColorItemColorset( Colorset );
				CIndexSprite::SetUsingColorSetOnly( 1, Colorset );	// 옷색
				
				if(bChakram == false)	// 리스틀릿을 차고 있으면
				{
					if(addonInfoChakram.bAddon)
					{
						Colorset = addonInfoChakram.ColorSet2;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
					else
					{
						Colorset = defaultCoatColor;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
				}
				else
				{
					Colorset = defaultCoatColor;
					if(addonInfoChakram.bAddon)
						Colorset = addonInfoChakram.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

					CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					
				}
				
				if(bootsAddonInfo.bAddon)
				{
					Colorset = bootsAddonInfo.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 부츠색
				}
				else
				{
					Colorset = defaultCoatColor ;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 팔
				}
				
				
				// 어둡게 찍기
				if (pCreature->IsFade())
				{
					m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
				}
				else if(pCreature->IsOusterGhost())
				{
					POINT pointTemp2;
					int fastMoveShadowMax = pCreature->GetOusterGhostCount();
					pCreature->UpdateOusterGhost();
					for(int fastMoveShadow = fastMoveShadowMax; fastMoveShadow > 0; fastMoveShadow--)
					{
						pointTemp2.x = pointTemp.x - (pCreature->GetCX() >> 1)*fastMoveShadow;
						pointTemp2.y = pointTemp.y - (pCreature->GetCY() >> 1)*fastMoveShadow;
						m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite, min(2, fastMoveShadow));
					}

				}
				else
				{
					m_pSurface->BltIndexSprite(&pointTemp, pSprite);
				}
				//if(direction == 6 && bAffterEffect)
				//	DrawAfterACOusters( pPoint, pCreature, action, direction, frame );
			}			
		}
    }

	
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		//				DEBUG_ADD("[DrawCreature] DrawChakram");
		int clothes = addonInfoChakram.FrameID;
		
		//FRAME_ARRAY &FA = m_OustersFPK[clothes][tempAction][direction];

		if(bNewCoat == true && bNewChakram == true)//t111
		   clothes = 0;
			//if(bNewChakram == true)
			//	clothes = 1;
			else if(bNewCoat == true && bNewChakram == false)
				clothes = 0;
				else if(bNewCoat == false && bNewChakram == true)
				clothes = 1;
				else if(bNewCoat == false && bNewChakram == false)
		//else
			    clothes = 0;
		
		

		FRAME_ARRAY &FA = oustersFPK[ clothes ][ tempAction ][ direction ];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			//					DEBUG_ADD_FORMAT("[DrawCreature] FA.GetSize() > frame %d %d", FA.GetSize(), frame);
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			//CIndexSprite* pSprite = &m_OustersSPK[ sprite ];					
			CIndexSprite* pSprite = &oustersSPK[ sprite ];
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfoChakram.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
					addonInfoChakram.EffectColorSet);
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				int colorset1, colorset2;
				colorset1 = addonInfoChakram.ColorSet1;
				colorset2 = addonInfoChakram.ColorSet2;
				
				if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
					colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
				else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;

				if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
					colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
				else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;

				CIndexSprite::SetUsingColorSet( colorset1, colorset2 );

				

				// 어둡게 찍기
				if (pCreature->IsFade())
				{
					m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
				}
				else if(pCreature->IsOusterGhost())
				{
					POINT pointTemp2;
					int fastMoveShadowMax = pCreature->GetOusterGhostCount();
					pCreature->UpdateOusterGhost();
					for(int fastMoveShadow = fastMoveShadowMax; fastMoveShadow > 0; fastMoveShadow--)
					{
						pointTemp2.x = pointTemp.x - (pCreature->GetCX() >> 1)*fastMoveShadow;
						pointTemp2.y = pointTemp.y - (pCreature->GetCY() >> 1)*fastMoveShadow;
						m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite, min(2, fastMoveShadow));
					}

				}
				else
				{
					m_pSurface->BltIndexSprite(&pointTemp, pSprite);
				}
				//if(direction == 6 && bAffterEffect)
				//	DrawAfterACOusters( pPoint, pCreature, action, direction, frame );
			}						
			
		}
		else
		{
			DEBUG_ADD_FORMAT("[DrawCreature] FA.GetSize() <= frame %d %d %d %d", FA.GetSize(), frame, pCreatureWear->GetActionCount(), pCreatureWear->GetActionCountMax());
			//					DEBUG_ADD_FORMAT("[DrawCreature] FA.GetSize() <= frame %d %d %d", clothes, tempAction, direction);
		}
	}
	}

#ifdef OUTPUT_DEBUG_DRAW_PROCESS
	DEBUG_ADD("dcOK");
#endif
}


int		MTopView::SlayerActionSpecialization(const MCreatureWear* creatureWear, int originAction) const
{
	Assert(creatureWear->IsSlayerCharacter() && "슬레이어가 아닌 다른 종족이 슬레이어 액션 특수화에 들어왔삼");

	const MCreatureWear::ADDON_INFO& rightHandAddon = creatureWear->GetAddonInfo( ADDON_RIGHTHAND );

	const bool bUseGun =	rightHandAddon.ItemClass == ITEM_CLASS_AR ||
							rightHandAddon.ItemClass == ITEM_CLASS_SR ||
							rightHandAddon.ItemClass == ITEM_CLASS_SG ||
							rightHandAddon.ItemClass == ITEM_CLASS_SMG;
	const bool bUseSword =	rightHandAddon.ItemClass == ITEM_CLASS_SWORD;
	const bool bUseBlade =	rightHandAddon.ItemClass == ITEM_CLASS_BLADE;
	const bool bUseMace	=	rightHandAddon.ItemClass == ITEM_CLASS_MACE;
	const bool bUseCross =	rightHandAddon.ItemClass == ITEM_CLASS_CROSS;

	// 전투 상태일 때와 일반 상태일 때 서 있는 Action이 다르다.
	if(originAction == ACTION_STAND &&
	   creatureWear->IsBattleStand())
	{
		if(bUseGun)			return ACTION_SLAYER_BATTLE_STAND_GUN;
		else if(bUseSword)	return ACTION_SLAYER_BATTLE_STAND_SWORD;
		else if(bUseBlade)	return ACTION_SLAYER_BATTLE_STAND_BLADE;
	}

	// 총을 들고 걸을 때
	if(originAction == ACTION_MOVE && bUseGun)
	{
		return ACTION_SLAYER_MOVE_GUN;
	}

	// 메이스나 십자가 무기가 아니면 ACTION_MAGIC 모션이 없다.
	if(originAction == ACTION_MAGIC && !bUseMace && !bUseCross)
	{
		return ACTION_SLAYER_MAGIC_CASTING;
	}
		
	return originAction;
}


void	MTopView::DrawSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int FrameIndex )
{
	if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
		DrawInstallTurret( pPoint, pCreature, action, direction, frame, FrameIndex );
	else
	{
		MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;	
		
		// Creature의 Action에 맞는 add-on을 출력한다.
		//action = pCreature->GetAction();
		
		WORD clothes;
		BYTE clothesType;
		
		BOOL	bAffterEffect  = FALSE ; 
		//------------------------------------------------------------
		// 잔상 그리는지 여부 처리 
		//------------------------------------------------------------
		if ( (pCreature->HasEffectStatus( EFFECTSTATUS_HOLY_SWORD) || 
			  pCreature->HasEffectStatus( EFFECTSTATUS_HOLY_BLADE) ||
			  pCreature->HasEffectStatus( EFFECTSTATUS_BURST_GUN)  ||
			  pCreature->HasEffectStatus( EFFECTSTATUS_HOLY_MACE))
		)
		{
				if( ((action >=  ACTION_MOVE				&& action <=  ACTION_ATTACK)  ||
//					(action >=  ACTION_SLAYER_GUN_SR	    && action <=  ACTION_SLAYER_THROW_WEAPON)  ||
//					(action >=  ACTION_SLAYER_GUN_SG	    && action <=  ACTION_SLAYER_THROW_POTION)  ||
					(action >=  ACTION_SLAYER_GUN_SR_SLOW	&& action <=  ACTION_SLAYER_BLADE_2_FAST) ||
					(action >=  ACTION_SLAYER_SWORD_2_REPEAT && action <=  ACTION_SLAYER_BLADE_2_REPEAT)) && (direction != 2) 
				)
				{	
					//bAffterEffect = TRUE ; 
				}
					
		}

		// Slayer Action 중 특수하게 처리해야 하는 것들을 묶는다.
		action = SlayerActionSpecialization(pCreatureWear, action);
		
		//------------------------------------------------------------
		// Fade Out 출력
		//------------------------------------------------------------
 		if (pCreature->HasEffectStatus( EFFECTSTATUS_FADE_OUT ))
 		{
 			DrawFadeOut( pPoint, pCreature, action, direction, frame);
 		}
		
	//	if(direction != 6 && bAffterEffect)
	//		DrawAfterSlayer( pPoint, pCreature, action, direction, frame );


		//------------------------------------------------------------
		// Fast Move 출력
		//------------------------------------------------------------
		if (pCreature->IsFastMove())
		{
			DrawFastMove( pPoint, pCreature, action, direction, frame );						
		}
		//------------------------------------------------------------
		// Invisible  - Slayer는 snipping mode에서 투명이다.
		//------------------------------------------------------------
		else if (pCreature->IsInvisible())
		{
			DrawInvisible( pPoint, pCreature, action, direction, frame );
		}
		//------------------------------------------------------------
		// 무기 잔상 기술
		//------------------------------------------------------------
 		else if (
 			(pCreature->HasEffectStatus( EFFECTSTATUS_DANCING_SWORD )
 			|| pCreature->HasEffectStatus( EFFECTSTATUS_GHOST_BLADE )
 			|| pCreature->HasEffectStatus( EFFECTSTATUS_REDIANCE ) && pCreature->GetAction() == ACTION_STAND
 			)
 			)
 		{
 				DrawWeaponFadeOut( pPoint, pCreature, action, direction, frame );			
 		}
		//------------------------------------------------------------
		// 일반적인 출력
		//------------------------------------------------------------
		else
		{
			for (int i=0; i<ADDON_MAX; i++)
			{

				// Creature의 현재 방향에 따라서...
				// 옷을 출력해주는 순서가 다를 수 있다.
				clothesType = MCreatureWear::s_AddonOrder[pCreature->GetDirection()][i];
				// i번째 종류의 옷을 입고 있다면 출력해 준다.
				const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(clothesType);

				if (addonInfo.bAddon)
				{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
					if (ShouldSkipClassicAddonMotorDraw(addonInfo.ItemClass, addonInfo.ItemType))
						continue;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
					clothes = addonInfo.FrameID;

					CCreatureFramePack& addonFPK = pCreature->IsMale() ? m_AddonManFPK : m_AddonWomanFPK;
					CIndexSpritePack&	addonSPK = pCreature->IsMale() ? m_AddonManSPK : m_AddonWomanSPK;

					FRAME_ARRAY &FA = addonFPK[clothes][action][direction];
					//m_AddonSPK.LoadFromFilePart(DFA[LEFT][0].GetSpriteID(), DFA[LEFTUP][FA.GetSize()-1].GetSpriteID());
					
					// 있는 동작인 경우
					if (FA.GetSize() > frame)
					{
						CFrame &Frame = FA[frame];					
						int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
						int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
						int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

						Assert(sprite != SPRITEID_NULL && "MTopView::DrawSlayerCharacter() : 슬레이어를 그리려는데 spriteID가 NULL이삼");
						
						CIndexSprite* pSprite = &addonSPK[ sprite ];
						
						
						pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
						pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
						
						//---------------------------------------- 
						// 캐릭터 선택 사각형 영역 설정
						//---------------------------------------- 	
						rect.left	= pointTemp.x;
						rect.top	= pointTemp.y;
						rect.right	= rect.left + pSprite->GetWidth();
						rect.bottom = rect.top + pSprite->GetHeight();
						pCreature->AddScreenRect( &rect );
						

						//---------------------------------------- 
						// 캐릭터 부분 색깔이 바뀌는 경우
						//---------------------------------------- 
						if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
						{
							m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
								addonInfo.EffectColorSet);
						}
						//---------------------------------------- 
						// 정상적인 캐릭터 출력
						//---------------------------------------- 
						else
						{
							int colorSet1 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet1);
							int colorSet2 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet2);

							CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
							
							//if(direction == 6 && bAffterEffect)
							//	DrawAfterSlayer( pPoint, pCreature, action, direction, frame );
							
							// 어둡게 찍기
							if (pCreature->IsFade())
							{
								m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
							}
							else
							{
								m_pSurface->BltIndexSprite(&pointTemp, pSprite);
							}
						}						
						
						// 정상적인 출력
						//CIndexSprite::SetUsingColorSet( addonInfo.ColorSet1, addonInfo.ColorSet2 );
						//m_pSurface->BltIndexSprite(&pointTemp, pSprite);
						
					}
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
					//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
					//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
					if ( clothesType == ADDON_MOTOR 
						&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
						&& (addonInfo.ItemType == 8
#if __CONTENTS(__SECOND_TRANSFORTER)
						||addonInfo.ItemType == 9
						||addonInfo.ItemType == 10
						||addonInfo.ItemType == 11
#endif //__SECOND_TRANSFORTER
						))
						break;

#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				}

			}
		}
	}
#ifdef OUTPUT_DEBUG_DRAW_PROCESS
DEBUG_ADD("dcOK");
#endif
}

void	MTopView::DrawAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int FrameIndex )//zhuans
{
	int action_old = action;
	if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
		DrawInstallTurret( pPoint, pCreature, action, direction, frame, FrameIndex );
    else if ( pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_MK1) )// MK1꿎桿
		DrawInstallMK1( pPoint, pCreature, action, direction, frame, FrameIndex );//侶몸변鑒乖菱성畇寧몸 뚤？ MK1꿎桿
	else
	{

		// ToT 시간없다.. 하드 코딩.. by sonee
		action = ConvAdvancementSlayerActionFromSlayerAction( action, dynamic_cast< MCreatureWear* >(pCreature) );

		if( action == -1 )
			return;
		else
			action -= ADVANCEMENT_ACTION_START;


		MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;

		int clothes;
		BYTE clothesType;
		BOOL	bAffterEffect  = FALSE ; 
		//------------------------------------------------------------
		// 잔상 그리는지 여부 처리 
		//------------------------------------------------------------
		if ( (pCreature->HasEffectStatus( EFFECTSTATUS_HOLY_SWORD) || 
			pCreature->HasEffectStatus( EFFECTSTATUS_HOLY_BLADE) ||
			pCreature->HasEffectStatus( EFFECTSTATUS_BURST_GUN)  || 	
			pCreature->HasEffectStatus( EFFECTSTATUS_HOLY_MACE))
			)
		{
			int AdvancementAction = ACTION_ADVANCEMENT_SLAYER_STOP_SWORD + action ; 
		}
		//------------------------------------------------------------

		//------------------------------------------------------------
		// Fade Out 출력
		//------------------------------------------------------------
		if (pCreature->HasEffectStatus( EFFECTSTATUS_FADE_OUT ))
		{
			DrawFadeOutForACSlayer( pPoint, pCreature, action_old, direction, frame);	//by viva slayer action_old		
		}

		//	if(direction == 2 && bAffterEffect)
		//	{
		//		DrawAfterACSlayer( pPoint, pCreature, action, direction, frame );
		//	}


		//------------------------------------------------------------
		// Fast Move 출력
		//------------------------------------------------------------
		if (pCreature->IsFastMove())
		{
			DrawFastMoveForACSlayer( pPoint, pCreature, action_old, direction, frame );		//by viva slayer action_old			
		}
		//------------------------------------------------------------
		// Invisible  - Slayer는 snipping mode에서 투명이다.
		//------------------------------------------------------------
		else if (pCreature->IsInvisible())
		{
			DrawInvisibleForACSlayer( pPoint, pCreature, action_old, direction, frame );	//by viva slayeraction_old			
		}
		//------------------------------------------------------------
		// 무기 잔상 기술
		//------------------------------------------------------------
		else if (
			(pCreature->HasEffectStatus( EFFECTSTATUS_DANCING_SWORD )
			|| pCreature->HasEffectStatus( EFFECTSTATUS_GHOST_BLADE )
			|| pCreature->HasEffectStatus( EFFECTSTATUS_REDIANCE ) && pCreature->GetAction() == ACTION_STAND
			)
			)
		{
			DrawWeaponFadeOutForACSlayer( pPoint, pCreature, action_old, direction, frame );	////by viva slayer 聃쥣裂독
		}
		//------------------------------------------------------------
		// 일반적인 출력
		//------------------------------------------------------------
		else
		{
			//by viva slayer
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( AC_ADDON_BODY );
			bool bNewCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_COAT &&
				(addonInfo.ItemType == 36 || addonInfo.ItemType == 37 || addonInfo.ItemType == 38 || addonInfo.ItemType == 39 || addonInfo.ItemType == 40 || addonInfo.ItemType == 41 || addonInfo.ItemType == 42 || addonInfo.ItemType == 43);//|| addonInfo.ItemType == 34 || addonInfo.ItemType == 35);//t222
			bool b231NewCoat =	addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_COAT && (addonInfo.ItemType == 44 || addonInfo.ItemType == 45 || addonInfo.ItemType == 46 || addonInfo.ItemType == 47);
			if(bNewCoat == true && b231NewCoat == false)
			{
				action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );
				if( action == -1 )
					return;
				else
					action -= ADVANCEMENT_ACTION_START;

				CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerMan201FPK : m_AdvancementSlayerWoman201FPK;//cfpk
				CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerMan201SPK : m_AdvancementSlayerWoman201SPK;//ispk
			
				CCreatureFramePack& slayerMoToFPK = m_AdvancementSlayerWoman171FPK;//cfpk
				CIndexSpritePack& addonMoToISPK = m_AdvancementSlayerWoman171SPK;//ispk

				bool showbody = true;

				for( int i = 0; i < AC_ADDON_MAX; ++i )
				{
					clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

					const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( clothesType );

					if(clothesType == 4)
					{
						clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );

						if(clothesType == AC_ADDON_MOTOR 
							&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
							&& (addonInfo.ItemType == 11))
						{
							action = 9;
						}

						if( clothes == -1 ) continue;

						if(clothes == 23 || clothes == 24 || clothes == 25 || clothes == 26 || clothes == 27 || clothes == 28)
						{
							showbody = false;
						}
						FRAME_ARRAY& FA = slayerMoToFPK[ clothes ][ action ][ direction ];
						int size = slayerMoToFPK.GetSize();
						if( FA.GetSize() > frame )
						{
							CFrame &Frame = FA[frame];					
							int sprite	= Frame.GetSpriteID();
							int cx		= Frame.GetCX();
							int cy		= Frame.GetCY();

							CIndexSprite* pSprite = &addonMoToISPK[ sprite ];
							pointTemp.x = pPoint->x + cx;
							pointTemp.y = pPoint->y + cy;

							rect.left	= pointTemp.x;
							rect.top	= pointTemp.y;
							rect.right	= rect.left + pSprite->GetWidth();
							rect.bottom = rect.top + pSprite->GetHeight();
							pCreature->AddScreenRect( &rect );

							if (addonInfo.bEffectColor)	
							{
								m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
									addonInfo.EffectColorSet);
							}
							else
							{
								if( addonInfo.bAddon )
								{
									int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
									if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
									{
										colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
									{
										colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
								}
								else
									CIndexSprite::SetUsingColorSet( 375, 375 );


								if (pCreature->IsFade())
								{	
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}	
						}
					}
					if( addonInfo.bAddon || clothesType == AC_ADDON_BODY && clothesType != 4)
					{
						if(showbody == false)
							break;
						clothes = GetAdvancementPartFromItemClass201( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );

						if( clothes == -1 ) continue;


a1:						FRAME_ARRAY& FA = slayerFPK[ clothes ][ action ][ direction ];
						int size = slayerFPK.GetSize();
						if( FA.GetSize() > frame )
						{
							CFrame &Frame = FA[frame];					
							int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
							int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
							int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

							CIndexSprite* pSprite = &addonISPK[ sprite ];
							pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
							pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

							//---------------------------------------- 
							// 캐릭터 선택 사각형 영역 설정
							//---------------------------------------- 	
							rect.left	= pointTemp.x;
							rect.top	= pointTemp.y;
							rect.right	= rect.left + pSprite->GetWidth();
							rect.bottom = rect.top + pSprite->GetHeight();
							pCreature->AddScreenRect( &rect );

							//---------------------------------------- 
							// 캐릭터 부분 색깔이 바뀌는 경우
							//---------------------------------------- 
							if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
							{
								m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
									addonInfo.EffectColorSet);
							}
							//---------------------------------------- 
							// 정상적인 캐릭터 출력
							//---------------------------------------- 
							else
							{
								if( addonInfo.bAddon )
								{
									int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
									if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
									{
										colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
									{
										colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
								}
								else
									CIndexSprite::SetUsingColorSet( 375, 375 );


								//	if(direction != 2 && bAffterEffect)
								//	{
								//		DrawAfterACSlayer( pPoint, pCreature, action, direction, frame );
								//	}

								if (pCreature->IsFade())
								{	
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}						
						}
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
						if ( clothesType == AC_ADDON_MOTOR 
							&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
							&& (addonInfo.ItemType == 8
#if __CONTENTS(__SECOND_TRANSFORTER)
							||addonInfo.ItemType == 9
							||addonInfo.ItemType == 10
							||addonInfo.ItemType == 11
#endif //__SECOND_TRANSFORTER
							))
							break;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

						if(clothes == 19 || clothes == 20)
						{
							clothes = clothes+2;
							goto a1;
						}
					}
				}
			}
			else if(bNewCoat == false && b231NewCoat == true)
			{
				action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );
				if( action == -1 )
					return;
				else
					action -= ADVANCEMENT_ACTION_START;

				CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerMan231FPK : m_AdvancementSlayerWoman231FPK;//cfpk
				CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerMan231SPK : m_AdvancementSlayerWoman231SPK;//ispk

				CCreatureFramePack& slayerMoToFPK = m_AdvancementSlayerWoman171FPK;//cfpk
				CIndexSpritePack& addonMoToISPK = m_AdvancementSlayerWoman171SPK;//ispk

				bool showbody = true;

				for( int i = 0; i < AC_ADDON_MAX; ++i )
				{
					clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

					const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( clothesType );

					if(clothesType == 4)
					{
						clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );

						if(clothesType == AC_ADDON_MOTOR 
							&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
							&& (addonInfo.ItemType == 11))
						{
							action = 9;
						}

						if( clothes == -1 ) continue;

						if(clothes == 23 || clothes == 24 || clothes == 25 || clothes == 26 || clothes == 27 || clothes == 28)
						{
						 showbody = false;
						}
						FRAME_ARRAY& FA = slayerMoToFPK[ clothes ][ action ][ direction ];
						int size = slayerMoToFPK.GetSize();
						if( FA.GetSize() > frame )
						{
							CFrame &Frame = FA[frame];					
							int sprite	= Frame.GetSpriteID();
							int cx		= Frame.GetCX();
							int cy		= Frame.GetCY();

							CIndexSprite* pSprite = &addonMoToISPK[ sprite ];
							pointTemp.x = pPoint->x + cx;
							pointTemp.y = pPoint->y + cy;

							rect.left	= pointTemp.x;
							rect.top	= pointTemp.y;
							rect.right	= rect.left + pSprite->GetWidth();
							rect.bottom = rect.top + pSprite->GetHeight();
							pCreature->AddScreenRect( &rect );

							if (addonInfo.bEffectColor)	
							{
								m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
									addonInfo.EffectColorSet);
							}
							else
							{
								if( addonInfo.bAddon )
								{
									int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
									if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
									{
										colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
									{
										colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
								}
								else
									CIndexSprite::SetUsingColorSet( 375, 375 );


								if (pCreature->IsFade())
								{	
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}	
						}
					}
					if( addonInfo.bAddon || clothesType == AC_ADDON_BODY && clothesType != 4)
					{
						if(showbody == false)
							break;
						clothes = GetAdvancementPartFromItemClass231( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );

						if( clothes == -1 ) continue;

a3:						FRAME_ARRAY& FA = slayerFPK[ clothes ][ action ][ direction ];
						int size = slayerFPK.GetSize();
						if( FA.GetSize() > frame )
						{
							CFrame &Frame = FA[frame];					
							int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
							int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
							int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

							CIndexSprite* pSprite = &addonISPK[ sprite ];
							pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
							pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

							//---------------------------------------- 
							// 캐릭터 선택 사각형 영역 설정
							//---------------------------------------- 	
							rect.left	= pointTemp.x;
							rect.top	= pointTemp.y;
							rect.right	= rect.left + pSprite->GetWidth();
							rect.bottom = rect.top + pSprite->GetHeight();
							pCreature->AddScreenRect( &rect );

							//---------------------------------------- 
							// 캐릭터 부분 색깔이 바뀌는 경우
							//---------------------------------------- 
							if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
							{
								m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
									addonInfo.EffectColorSet);
							}
							//---------------------------------------- 
							// 정상적인 캐릭터 출력
							//---------------------------------------- 
							else
							{
								if( addonInfo.bAddon )
								{
									int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
									if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
									{
										colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
									{
										colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
								}
								else
									CIndexSprite::SetUsingColorSet( 375, 375 );


								//	if(direction != 2 && bAffterEffect)
								//	{
								//		DrawAfterACSlayer( pPoint, pCreature, action, direction, frame );
								//	}

								if (pCreature->IsFade())
								{	
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}						
						}
//#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
//						if ( clothesType == AC_ADDON_MOTOR 
//							&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
//							&& (addonInfo.ItemType == 8
//#if __CONTENTS(__SECOND_TRANSFORTER)
//							||addonInfo.ItemType == 9
//							||addonInfo.ItemType == 10
//#endif //__SECOND_TRANSFORTER
//							))
//							break;
//#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
//
						if(clothes == 19 || clothes == 20)
						{
							clothes = clothes+2;
							goto a3;
						}
					}
				}
			}
			else	//by viva
			{
				action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );
				if( action == -1 )
					return;
				else
					action -= ADVANCEMENT_ACTION_START;
				CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerMan171FPK : m_AdvancementSlayerWoman171FPK;
				CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerMan171SPK : m_AdvancementSlayerWoman171SPK;
				for( int i = 0; i < AC_ADDON_MAX; ++i )
				{
					clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

					const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( clothesType );

					if( addonInfo.bAddon || clothesType == AC_ADDON_BODY )
					{
						clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );
						if(clothesType == AC_ADDON_MOTOR 
							&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
							&& (addonInfo.ItemType == 11))
						{
							action = 9;
						}
						if( clothes == -1 ) continue;


a2:						FRAME_ARRAY& FA = slayerFPK[ clothes ][ action ][ direction ];
						int size = slayerFPK.GetSize();
						if( FA.GetSize() > frame )
						{
							CFrame &Frame = FA[frame];					
							int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
							int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
							int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

							CIndexSprite* pSprite = &addonISPK[ sprite ];
							pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
							pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

							//---------------------------------------- 
							// 캐릭터 선택 사각형 영역 설정
							//---------------------------------------- 	
							rect.left	= pointTemp.x;
							rect.top	= pointTemp.y;
							rect.right	= rect.left + pSprite->GetWidth();
							rect.bottom = rect.top + pSprite->GetHeight();
							pCreature->AddScreenRect( &rect );

							//---------------------------------------- 
							// 캐릭터 부분 색깔이 바뀌는 경우
							//---------------------------------------- 
							if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
							{
								m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
									addonInfo.EffectColorSet);
							}
							//---------------------------------------- 
							// 정상적인 캐릭터 출력
							//---------------------------------------- 
							else
							{
								if( addonInfo.bAddon )
								{
									int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
									if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
									{
										colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
									{
										colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}
									CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
								}
								else
									CIndexSprite::SetUsingColorSet( 375, 375 );


								if (pCreature->IsFade())
								{	
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}						
						}
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
						if ( clothesType == AC_ADDON_MOTOR 
							&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
							&& (addonInfo.ItemType == 8
#if __CONTENTS(__SECOND_TRANSFORTER)
							||addonInfo.ItemType == 9
							||addonInfo.ItemType == 10
							||addonInfo.ItemType == 11
#endif //__SECOND_TRANSFORTER
							))
						//	action = action -1;
						//	std::ofstream file("Log\\motor.log", std::ios_base::out | std::ios_base::app);
	                   //     file	<< "action="<< action << '\n';
							break;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
						if(clothes == 19 || clothes == 20)
						{
							clothes = clothes+2;
							goto a2;
						}
					}
				}
			}
		}
	}
}
//by viva vampire
void	MTopView::DrawAdvancementClassVampireCharacter( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex )
{
	//------------------------------------------------
	// 관 출력정보
	//------------------------------------------------
	int casketValue = pCreature->GetCasketCount();
	int creature_type = pCreature->GetCreatureType();
	bool bInCasket = pCreature->IsInCasket();
	bool bCasketOnly = (bInCasket && casketValue==0);

	action = GetAdvancementVampireActionFromVampireAction( action, pCreature );

	if( action == -1 )
		return;
	else
		action -= ADVANCEMENT_ACTION_START;
	
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	bool bCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT;
	bool bNewCoat = bCoat && (addonInfo.ItemType == 32 || addonInfo.ItemType == 33 || addonInfo.ItemType == 34 || addonInfo.ItemType == 35 || addonInfo.ItemType == 36|| addonInfo.ItemType == 37 || addonInfo.ItemType == 38|| addonInfo.ItemType == 39);	//rallser201V
	bool b231NewCoat = bCoat && (addonInfo.ItemType == 40|| addonInfo.ItemType == 41 || addonInfo.ItemType == 42 || addonInfo.ItemType == 43);
	const MCreatureWear::ADDON_INFO& addonInfo2 = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bNewWeapon = addonInfo2.bAddon && addonInfo2.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON && (addonInfo2.ItemType == 26 || addonInfo2.ItemType == 27 || addonInfo2.ItemType == 28 || addonInfo2.ItemType == 29 || addonInfo2.ItemType == 30 || addonInfo2.ItemType == 31);
	bool b231NewWeapon = addonInfo2.bAddon && addonInfo2.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON && (addonInfo2.ItemType >= 32); 

	int clothes = 0;
	if(bNewCoat == true)
	{
	    if(addonInfo.ItemType == 32 || addonInfo.ItemType == 33|| addonInfo.ItemType == 34|| addonInfo.ItemType == 35|| addonInfo.ItemType == 36|| addonInfo.ItemType == 37|| addonInfo.ItemType == 38|| addonInfo.ItemType == 39)
	
		clothes = 0;
	}
	else if(b231NewCoat == true)
	{
		if(addonInfo.ItemType == 40 || addonInfo.ItemType == 41 || addonInfo.ItemType == 42 || addonInfo.ItemType == 43)
			clothes = 0;
	}
	else if(!bNewCoat && !b231NewCoat)
	{
		if(addonInfo.ItemType == 31 || addonInfo.ItemType == 30)
		    clothes = 1;	//	
		else
        clothes =0;		
	}


		action = GetAdvancementVampireAction171[action];
		if(action == -1)	return;
	

	//	CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? (bNewCoat ? m_AdvancementVampireMan201FPK : m_AdvancementVampireMan171FPK) : (bNewCoat ? m_AdvancementVampireWoman201FPK : m_AdvancementVampireWoman171FPK);
	//	CIndexSpritePack& advanceVampireSPK = pCreature->IsMale() ? (bNewCoat ? m_AdvancementVampireMan201SPK : m_AdvancementVampireMan171SPK) : (bNewCoat ? m_AdvancementVampireWoman201SPK : m_AdvancementVampireWoman171SPK);

	 CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? (b231NewCoat ? m_AdvancementVampireMan231FPK :(bNewCoat ? m_AdvancementVampireMan201FPK : m_AdvancementVampireMan171FPK)) : (b231NewCoat ? m_AdvancementVampireWoman231FPK : (bNewCoat ? m_AdvancementVampireWoman201FPK : m_AdvancementVampireWoman171FPK));
	 CIndexSpritePack& advanceVampireSPK = pCreature->IsMale() ? (b231NewCoat ? m_AdvancementVampireMan231SPK : (bNewCoat ? m_AdvancementVampireMan201SPK : m_AdvancementVampireMan171SPK)) : (b231NewCoat ? m_AdvancementVampireWoman231SPK : (bNewCoat ? m_AdvancementVampireWoman201SPK : m_AdvancementVampireWoman171SPK));

	BOOL	bAffterEffect  = FALSE ; 
	int iAction = GetAdvancementVampireActionFromVampireAction(action , pCreature) ; 
	//------------------------------------------------------------
	// 잔상 그리는지 여부 처리 
	//------------------------------------------------------------
	if ( (pCreature->HasEffectStatus( EFFECTSTATUS_DARK_FORCE))
	)
	{
		int AdvancementAction = ACTION_ADVANCEMENT_STOP + action ; 

	}
	if (!bCasketOnly)
	{
		FRAME_ARRAY& FA = advanceVampireFPK[clothes][action][direction];
		
		if (FA.GetSize() > frame)
		{
			CFrame& Frame =	FA[frame];
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			

			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

			
			CIndexSprite* pSprite = &advanceVampireSPK[ sprite ];
			
			//---------------------------------------- 
			// 캐릭터 선택 사각형 영역 설정
			//---------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			if(frameindex==0)
				pCreature->SetScreenRect( &rect );
			//-----------------------------------------------------------
			// CutHeight
			//-----------------------------------------------------------
			bool bCutHeight = pCreature->IsCutHeight();
			int previousClipBottom;	
			if (bCutHeight)
			{
				previousClipBottom = m_pSurface->GetClipBottom();
				m_pSurface->SetClipBottom( rect.bottom - TILE_Y_HALF);
				
				pointTemp.y += pCreature->GetCutHeightCount() - TILE_Y_HALF;
			}
			
			//-----------------------------------------------------------
			// 투명 상태 출력 			
			//-----------------------------------------------------------
			// vampire
			if (pCreature->IsFakeDie())
			{
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();		
				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우					
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
				}
				else
				{
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;

					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					int netValue = pCreature->GetFakeDieCount()/5;
					
					CSpriteSurface::s_Value1 = netValue;
					
					CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_NET );
					CIndexSprite::SetEffect( CIndexSprite::EFFECT_NET );
					
					m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
					
				}
			}
			else if (pCreature->IsInvisible())
			{
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();
			
				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우					
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					// IndexSprite에도 EFFECT...넣고...
					// 스르륵~ 단계적으로 나타나게/없어지게 해야됨.. - -;
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
				}
				else
				{
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
						colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
						colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
						colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
						colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER
					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;
					
					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					int wipeValue = pCreature->GetInvisibleCount();
					
					if (wipeValue==0)
					{
						m_pSurface->BltIndexSprite(&pointTemp, pSprite);							
					}
					else if (wipeValue==64)
					{
						CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_SIMPLE_OUTLINE );
						CIndexSprite::SetEffect( CIndexSprite::EFFECT_SIMPLE_OUTLINE );
						
						m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
					}
					else
					{
						CSpriteSurface::s_Value1 = wipeValue;
						
						CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_WIPE_OUT );
						CIndexSprite::SetEffect( CIndexSprite::EFFECT_WIPE_OUT );
						
						m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
					}							
				}
			}					
			else
			{	
				//-----------------------------------------------------------
				// 빠르게 움직이는 중
				//-----------------------------------------------------------
				if (pCreature->IsFastMove() || pCreature->IsBloodyZenith())
				{
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
					
					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;
					
					
					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					POINT pointTemp2;
					
					int fastMoveShadowMax = 2;
					if(pCreature->IsBloodyZenith())
					{
						fastMoveShadowMax = pCreature->GetBloodyZenithCount();
						pCreature->UpdateBloodyZenith();
					}
					for(int fastMoveShadow = fastMoveShadowMax; fastMoveShadow > 0; fastMoveShadow--)
					{
						// 1 pixel skip
						pointTemp2.x = pointTemp.x - (pCreature->GetCX() >> 1)*fastMoveShadow;
						pointTemp2.y = pointTemp.y - (pCreature->GetCY() >> 1)*fastMoveShadow;
						if(pCreature->IsBloodyZenith())
						{
							pointTemp2.y -= sin((double)max(0, pCreature->GetMoveCount()-fastMoveShadow)*3.141592/(double)pCreature->GetMoveCountMax())*100;
						}
						
						m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite, min(2, fastMoveShadow));
					}
					
				}
				
				if(pCreature->IsBloodyZenith())
				{
					pointTemp.y -= sin((double)pCreature->GetMoveCount()*3.141592/(double)pCreature->GetMoveCountMax())*100;
				}
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();		

				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우					
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
				}
				//---------------------------------------- 
				// 정상적인 색깔 출력
				//---------------------------------------- 					
				else
				{				
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
						colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER
					
					if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;
					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					int shadowCount = pCreature->GetShadowCount();
					
					//----------------------------------------
					// Shadow출력
					//----------------------------------------
					if (shadowCount)
					{
						bool bDrawShadowFirst =
							(direction <= 4)		||
							creature_type == 764	||
							creature_type == 765	||
							creature_type == 2014	||
							creature_type == 2015	||
							creature_type == 891;

						if (!bDrawShadowFirst)
						{
							if (pCreature->IsFade())
							{
								m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
							}
							else
							{
								m_pSurface->BltIndexSprite(&pointTemp, pSprite);
							}
						}

						// 0, 1, 2, 3, 4, 5
						const int darkbits[6] =
						{
							0, 1, 1, 2, 2, 3
						};
						
						POINT pointTemp2;
						
						// 이동이면 좌표도 바뀐다
						int cxStep = 0, cyStep = 0, cxInc =0 , cyInc = 0;
						if (action==ACTION_MOVE)
						{
							cxInc = -pCreature->GetCX();
							cyInc = -pCreature->GetCY();
						}
						
						
						int faNum = FA.GetSize();
						for (int f=1; f<=shadowCount; f++)
						{
							int frame_f = frame-f;
							
							//---------------------------------------- 
							// f frame 전
							//---------------------------------------- 
							if (frame_f < 0)
							{
								frame_f += faNum;
							}
							
							CFrame &Frame_f = FA[frame_f];
							int sprite_f	= Frame_f.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
							
							CIndexSprite* pSprite_f = &GetCreatureSprite(sprite_f, pCreature->GetCreatureFrameID(0));
							
							int cx_f		= Frame_f.GetCX();
							int cy_f		= Frame_f.GetCY();
							
							pointTemp2.x = pPoint->x + cx_f + cxStep;
							pointTemp2.y = pPoint->y + cy_f + cyStep;
							cxStep += cxInc;
							cyStep += cyInc;
							
							m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite_f, darkbits[f]);																		

						}
						
						if (bDrawShadowFirst)
						{
							if (pCreature->IsFade())
							{
								m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
							}
							else
							{
								m_pSurface->BltIndexSprite(&pointTemp, pSprite);
							}

						}
					}
					//----------------------------------------
					// 몸만 찍기
					//----------------------------------------
					else
					{
						// 어둡게 찍기
						if (pCreature->IsFade())
						{
							m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
						}
						else
						{
							m_pSurface->BltIndexSprite(&pointTemp, pSprite);
							
						}
					//	if(direction == 6 && bAffterEffect)
					//	{
					//		if( iAction != -1 )
					//		{
					//		//	iAction -= ADVANCEMENT_ACTION_START;
					//			DrawAfterACVampire( pPoint, pCreature, iAction, direction, frame );
					//		}
					//		
					//	}
					}
				}
			}

			if (bCutHeight)
			{
				m_pSurface->SetClipBottom( previousClipBottom );
			}

			// 2004, 10, 28, sobeit add start  - 몬스터 킬 퀘스트 해당 몬스터에 표시.
			if(pCreature->IsAlive() && (g_CurrentFrame&0x04)&&UI_IsMonsterKillQuest_Monster(SafeSpriteType0(pCreature->GetCreatureType())))
			//if(pCreature->IsAlive() && (g_CurrentFrame&0x04))
			{
				UI_ShowTargetArrow(pPoint->x + 12 ,pPoint->y- pCreature->GetHeight()-20);
			}
		// 2004, 10, 28, sobeit add end
		}

		bool bWeapon = true;

		bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
		bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
		bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;
		bool bGhost = pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST;
#if __CONTENTS(__FAST_TRANSFORTER)
		bool bFlitterMouse = pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE;
		bool bNede = pCreature->GetCreatureType() == CREATURETYPE_NEDE;
		bool bKltl = pCreature->GetCreatureType() == CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		bool bShapeOfDemon = pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON;

#endif //__SECOND_TRANSFORTER
		//if( bWeapon && !bBat && !bWolf && !bWerWolf )
		if( bWeapon && !bBat && !bWolf && !bWerWolf && !bGhost
#if __CONTENTS(__FAST_TRANSFORTER)
			&& !bFlitterMouse
			&& !bNede
			&& !bKltl
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
			&& !bShapeOfDemon

#endif //__SECOND_TRANSFORTER
			)
		{
			int clothes_weapon = 1;//t111
			//if(bNewCoat == true && bNewWeapon == true)
			//
			//	clothes_weapon = 2;
   //            else if(bNewCoat == true && bNewWeapon == false)
			//	clothes_weapon = 2;
			//	else if(bNewCoat == false && bNewWeapon == true)
			//	clothes_weapon = 3;
			//	else if(bNewCoat == false && bNewWeapon == false)
			//    clothes_weapon = 2;

				if(bNewCoat && bNewWeapon)
					  clothes_weapon =2;
				else if(bNewCoat && !bNewWeapon)
					  clothes_weapon =2;
				else if(!bNewCoat && bNewWeapon)
					  clothes_weapon = 3;
				else if(!bNewCoat && !bNewWeapon)
				{
					if(bNewCoat)
					  clothes_weapon = 1 ;
					else

					clothes_weapon = 2;
				}

			

			FRAME_ARRAY& FAWEAPON = advanceVampireFPK[clothes_weapon][action][direction];
			if (FAWEAPON.GetSize() > frame)
			{
				CFrame& Frame =	FAWEAPON[frame];
				int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
					cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
					cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
				
				// 좌표 보정
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
				
				CIndexSprite* pSprite = &advanceVampireSPK[ sprite ];
				
				//---------------------------------------- 
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
//				if(frameindex==0 )
//					pCreature->SetScreenRect( &rect );
				//-----------------------------------------------------------
				// CutHeight
				//-----------------------------------------------------------
				bool bCutHeight = pCreature->IsCutHeight();
				int previousClipBottom;
				if (bCutHeight)
				{
					previousClipBottom = m_pSurface->GetClipBottom();
					m_pSurface->SetClipBottom( rect.bottom - TILE_Y_HALF );
					
					pointTemp.y += pCreature->GetCutHeightCount() - TILE_Y_HALF;
				}
				
				//-----------------------------------------------------------
				// 투명 상태 출력 			
				//-----------------------------------------------------------
				// vampire
				if (pCreature->IsFakeDie())
				{
					WORD colorSet = pCreature->GetAttachEffectColor();
					
					//---------------------------------------- 
					// 캐릭터 전체 색깔이 바뀌는 경우					
					//---------------------------------------- 
					if (colorSet < MAX_COLORSET)
					{
						int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
						m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
					}
					else
					{
						int netValue = pCreature->GetFakeDieCount()/5;
						
						CSpriteSurface::s_Value1 = netValue;
						
						CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_NET );
						CIndexSprite::SetEffect( CIndexSprite::EFFECT_NET );
						
						m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);

					}
				}
				else if (pCreature->IsInvisible())
				{
					WORD colorSet = pCreature->GetAttachEffectColor();
					
					//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();
#if __CONTENTS(__FAST_TRANSFORTER)
				if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet = pCreature->GetWingColor();
				if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet = pCreature->GetWingColor();
				if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
					colorSet = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER
					//---------------------------------------- 
					// 캐릭터 전체 색깔이 바뀌는 경우					
					//---------------------------------------- 
					if (colorSet < MAX_COLORSET)
					{
						// IndexSprite에도 EFFECT...넣고...
						// 스르륵~ 단계적으로 나타나게/없어지게 해야됨.. - -;
						int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
						m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
					}
					else
					{
						int wipeValue = pCreature->GetInvisibleCount();
						
						if (wipeValue==0)
						{
							m_pSurface->BltIndexSprite(&pointTemp, pSprite);							
						}
						else if (wipeValue==64)
						{
							CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_SIMPLE_OUTLINE );
							CIndexSprite::SetEffect( CIndexSprite::EFFECT_SIMPLE_OUTLINE );
							
							m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
						}
						else
						{
							CSpriteSurface::s_Value1 = wipeValue;
							
							CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_WIPE_OUT );
							CIndexSprite::SetEffect( CIndexSprite::EFFECT_WIPE_OUT );
							
							m_pSurface->BltIndexSpriteEffect(&pointTemp, pSprite);
						}
					}
				}					
				else
				{	
					//-----------------------------------------------------------
					// 빠르게 움직이는 중
					//-----------------------------------------------------------
					if (pCreature->IsFastMove() || pCreature->IsBloodyZenith())
					{
						POINT pointTemp2;
						
						int fastMoveShadowMax = 2;
						if(pCreature->IsBloodyZenith())
						{
							fastMoveShadowMax = pCreature->GetBloodyZenithCount();
							pCreature->UpdateBloodyZenith();
						}
						for(int fastMoveShadow = fastMoveShadowMax; fastMoveShadow > 0; fastMoveShadow--)
						{
							// 1 pixel skip
							pointTemp2.x = pointTemp.x - (pCreature->GetCX() >> 1)*fastMoveShadow;
							pointTemp2.y = pointTemp.y - (pCreature->GetCY() >> 1)*fastMoveShadow;
							if(pCreature->IsBloodyZenith())
							{
								pointTemp2.y -= sin((double)max(0, pCreature->GetMoveCount()-fastMoveShadow)*3.141592/(double)pCreature->GetMoveCountMax())*100;
							}
							
							m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite, min(2, fastMoveShadow));
						}
						
					}
					
					if(pCreature->IsBloodyZenith())
					{
						pointTemp.y -= sin((double)pCreature->GetMoveCount()*3.141592/(double)pCreature->GetMoveCountMax())*100;
					}
					WORD colorSet = pCreature->GetAttachEffectColor();
					
					//---------------------------------------- 
					// 캐릭터 전체 색깔이 바뀌는 경우					
					//---------------------------------------- 
					if (colorSet < MAX_COLORSET)
					{
						int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
						m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, nExtColorSet);
					}
					//---------------------------------------- 
					// 정상적인 색깔 출력
					//---------------------------------------- 					
					else
					{				
						int shadowCount = pCreature->GetShadowCount();
						
						//----------------------------------------
						// Shadow출력
						//----------------------------------------
						if (shadowCount)
						{
							bool bDrawShadowFirst =
								(direction <= 4)		||
								creature_type == 764	||
								creature_type == 765	||
								creature_type == 2014	||
								creature_type == 2015	||
								creature_type == 891;

							if (!bDrawShadowFirst)
							{
								if (pCreature->IsFade())
								{
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}
							
							// 0, 1, 2, 3, 4, 5
							const int darkbits[6] =
							{
								0, 1, 1, 2, 2, 3
							};
							
							POINT pointTemp2;
							
							// 이동이면 좌표도 바뀐다
							int cxStep = 0, cyStep = 0, cxInc =0 , cyInc = 0;
							if (action==ACTION_MOVE)
							{
								cxInc = -pCreature->GetCX();
								cyInc = -pCreature->GetCY();
							}
							
							
							int faNum = FA.GetSize();
							for (int f=1; f<=shadowCount; f++)
							{
								int frame_f = frame-f;
								
								//---------------------------------------- 
								// f frame 전
								//---------------------------------------- 
								if (frame_f < 0)
								{
									frame_f += faNum;
								}
								
								CFrame &Frame_f = FA[frame_f];
								int sprite_f	= Frame_f.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
								
								CIndexSprite* pSprite_f = &GetCreatureSprite(sprite_f, pCreature->GetCreatureFrameID(0));
								
								int cx_f		= Frame_f.GetCX();
								int cy_f		= Frame_f.GetCY();
								
								pointTemp2.x = pPoint->x + cx_f + cxStep;
								pointTemp2.y = pPoint->y + cy_f + cyStep;
								cxStep += cxInc;
								cyStep += cyInc;
								
								m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite_f, darkbits[f]);																		
							}
							
							if (bDrawShadowFirst)
							{
								if (pCreature->IsFade())
								{
									m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
								}
								else
								{
									m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								}
							}
						}
						//----------------------------------------
						// 몸만 찍기
						//----------------------------------------
						else
						{
							// 어둡게 찍기
							if (pCreature->IsFade())
							{
								m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
							}
							else
							{
								m_pSurface->BltIndexSprite(&pointTemp, pSprite);
								
							}
						//	if(direction == 6 && bAffterEffect)
						//	{
						//		if( iAction != -1 )
						//		{
						//			//iAction -= ADVANCEMENT_ACTION_START;
						//			DrawAfterACVampire( pPoint, pCreature, iAction, direction, frame );
						//		}
						//	}			
						}
					}
				}


				if (bCutHeight)
				{
					m_pSurface->SetClipBottom( previousClipBottom );
				}

			}
		}
	}
		
	//----------------------------------------------------
	// 관 출력
	//----------------------------------------------------
	if (bInCasket)
	{
		// 그림자 때문에 *2해야한다.
		int casketID = SPRITEID_CASKET_1 + pCreature->GetCasketType()*2;
		
		if (casketID < m_EtcSPK.GetSize())
		{
			CSprite* pSprite = &m_EtcSPK[casketID];
			
			if (pSprite!=NULL)
			{
				// tile의 중심에서 세운다.
				pointTemp.x = pPoint->x + TILE_X_HALF - (pSprite->GetWidth()>>1);
				pointTemp.y = pPoint->y + TILE_Y - pSprite->GetHeight();
				
				//---------------------------------------- 
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x; 
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				if(frameindex==0 )
					pCreature->SetScreenRect( &rect );
				
				if (casketValue==0)
				{
					m_pSurface->BltSprite(&pointTemp, pSprite);					
				}
				else if (casketValue==64)
				{
					CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_SIMPLE_OUTLINE );
					
					m_pSurface->BltSpriteEffect(&pointTemp, pSprite);
				}
				else
				{
					CSpriteSurface::s_Value1 = casketValue;
					
					CSpriteSurface::SetEffect( CSpriteSurface::EFFECT_WIPE_OUT );
					
					m_pSurface->BltSpriteEffect(&pointTemp, pSprite);
				}
			}
		}
	}
}

void	
MTopView::DrawSelectedVampireCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex, bool player )
{
	const int auditCreatureType = pCreature ? (int)pCreature->GetCreatureType() : -1;
	const bool auditEnabled = Phoenix_CreatureDrawAudit_ShouldLog(auditCreatureType) != 0;
	if (auditEnabled)
	{
		Phoenix_CreatureDrawAudit_Log(
			"DrawSelectedVampire ENTER ptr=%p id=%d type=%d action=%d dir=%d frame=%d body=%d frameIndex=%d player=%d",
			(void*)pCreature, pCreature ? (int)pCreature->GetID() : -1, auditCreatureType,
			action, direction, frame, body, frameindex, (int)player);
	}
	//------------------------------------------------
	// 관 출력정보
	//------------------------------------------------
	int casketValue = pCreature->GetCasketCount();
	bool bInCasket = pCreature->IsInCasket();
	bool bCasketOnly = (bInCasket && casketValue==0);
	if (auditEnabled)
	{
		Phoenix_CreatureDrawAudit_Log(
			"DrawSelectedVampire casket casketValue=%d inCasket=%d casketOnly=%d",
			casketValue, (int)bInCasket, (int)bCasketOnly);
	}
	
	if (!bCasketOnly)
	{
		//////////////////////////////////////////////////////////////////////////
		// Set PC Vampire Coat Start - chyaya 2007.05.15

		int		drawAction	= action;
		int		drawBody	= body;

		if(player)
		{
			MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
			
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
			
			bool bCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT;
			
			if(bCoat)
			{
				drawBody	= addonInfo.FrameID;
			}

			BYTE actionStep = pCreature->GetActionStep();
	
			drawAction = MCreature::GetVampireActionStep(drawAction, actionStep);
		}

		CCreatureFramePack* pFramePack = NULL;

		if(player)
		{
			pFramePack = &m_VampireFPK;
		}
		else
		{
			pFramePack = &m_CreatureFPK;
		}

		// Set PC Vampire Coat End - chyaya 2007.05.15
		//////////////////////////////////////////////////////////////////////////

		if (auditEnabled)
		{
			int packSize = pFramePack ? (int)pFramePack->GetSize() : -1;
			int actionCount = -1;
			int directionCount = -1;
			int faSize = -1;
			if (pFramePack != NULL && drawBody >= 0 && drawBody < packSize)
			{
				ACTION_FRAME_ARRAY& auditActions = (*pFramePack)[(TYPE_FRAMEID)drawBody];
				actionCount = auditActions.GetSize();
				if (drawAction >= 0 && drawAction < actionCount)
				{
					DIRECTION_FRAME_ARRAY& auditDirections = auditActions[(BYTE)drawAction];
					directionCount = auditDirections.GetSize();
					if (direction >= 0 && direction < directionCount)
					{
						FRAME_ARRAY& auditFrames = auditDirections[(BYTE)direction];
						faSize = auditFrames.GetSize();
					}
				}
			}
			Phoenix_CreatureDrawAudit_Log(
				"DrawSelectedVampire PRE_FRAME_ACCESS type=%d player=%d packSize=%d drawBody=%d drawAction=%d dir=%d frame=%d actionCount=%d directionCount=%d faSize=%d",
				auditCreatureType, (int)player, packSize, drawBody, drawAction, direction,
				frame, actionCount, directionCount, faSize);
		}
		FRAME_ARRAY& FA = (*pFramePack)[drawBody][drawAction][direction];
		
		if (FA.GetSize() > frame)
		{			
			CFrame&	Frame = FA[frame];
			
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			if (auditEnabled)
			{
				Phoenix_CreatureDrawAudit_Log(
					"DrawSelectedVampire FRAME_OK type=%d sprite=%d cx=%d cy=%d faSize=%d",
					auditCreatureType, sprite, cx, cy, (int)FA.GetSize());
			}
			
			CIndexSprite* pSprite = NULL;

			if(player)
			{
				pSprite = &m_VampireSPK[ sprite ];
			}
			else
			{
				pSprite = &GetCreatureSprite(sprite, drawBody);
			}
			if (auditEnabled)
			{
				Phoenix_CreatureDrawAudit_Log(
					"DrawSelectedVampire SPRITE_PTR type=%d sprite=%d drawBody=%d player=%d pSprite=%p isNotInit=%d",
					auditCreatureType, sprite, drawBody, (int)player, (void*)pSprite,
					pSprite ? (int)pSprite->IsNotInit() : -1);
			}
			
			//					if (pSprite->IsNotInit())
			//					{
			//						LoadFromFileCreatureActionSPK( body, action );
			//					}
			
			// 좌표 보정
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();			
			
			//---------------------------------------- 		
			// 캐릭터 선택 사각형 영역 설정
			//---------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			if(frameindex ==0 )
				pCreature->SetScreenRect( &rect );
			
			WORD colorSet = pCreature->GetAttachEffectColor();
			
			//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();
			//---------------------------------------- 
			// 캐릭터 전체 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (colorSet < MAX_COLORSET)
			{
				int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, nExtColorSet );
			}					
			//---------------------------------------- 
			// 정상적인 출력
			//---------------------------------------- 										
			else
			{					
				int colorSet1 = pCreature->GetBodyColor1();
				int colorSet2 = pCreature->GetBodyColor2();
				
				if ( colorSet1 == UNIQUE_ITEM_COLOR || colorSet1 == QUEST_ITEM_COLOR )
					colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
				else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
				else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet1 = pCreature->GetWingColor();
				else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet1 = pCreature->GetWingColor();
				else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
					colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER

				if ( colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR )
					colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
				else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;
				CIndexSprite::SetUsingColorSetOnly(0, colorSet1);	// 피부
				CIndexSprite::SetUsingColorSetOnly(1, colorSet2);	// 옷
				CIndexSprite::SetUsingColorSetOnly(2, 390);			// 머리
				
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
			}
			
			
			int shadowCount = pCreature->GetShadowCount();
			if (auditEnabled)
			{
				Phoenix_CreatureDrawAudit_Log(
					"DrawSelectedVampire SHADOW_START type=%d shadowCount=%d action=%d frame=%d faSize=%d",
					auditCreatureType, shadowCount, action, frame, (int)FA.GetSize());
			}
			
			//----------------------------------------
			// Shadow출력
			//----------------------------------------
			if (shadowCount)
			{
				// 0, 1, 2, 3, 4, 5
				const int darkbits[6] =
				{
					0, 1, 1, 2, 2, 3
				};
				
				POINT pointTemp2;
				
				// 이동이면 좌표도 바뀐다
				int cxStep = 0, cyStep = 0, cxInc =0 , cyInc = 0;
				if (action==ACTION_MOVE)
				{
					cxInc = -pCreature->GetCX();
					cyInc = -pCreature->GetCY();
				}
				
				
				int faNum = FA.GetSize();
				for (int f=1; f<=shadowCount; f++)
				{
					int frame_f = frame-f;
					
					//---------------------------------------- 
					// f frame 전
					//---------------------------------------- 
					if (frame_f < 0)
					{
						frame_f += faNum;
					}
					
					CFrame &Frame_f = FA[frame_f];
					int sprite_f	= Frame_f.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					if (auditEnabled)
					{
						Phoenix_CreatureDrawAudit_Log(
							"DrawSelectedVampire SHADOW_FRAME type=%d f=%d frame_f=%d sprite_f=%d drawBody=%d",
							auditCreatureType, f, frame_f, sprite_f, drawBody);
					}
					
					CIndexSprite* pSprite_f = &GetCreatureSprite(sprite_f, drawBody);
					if (auditEnabled)
					{
						Phoenix_CreatureDrawAudit_Log(
							"DrawSelectedVampire SHADOW_SPRITE_PTR type=%d f=%d sprite_f=%d pSprite_f=%p isNotInit=%d",
							auditCreatureType, f, sprite_f, (void*)pSprite_f,
							pSprite_f ? (int)pSprite_f->IsNotInit() : -1);
					}
					
					//							if (pSprite_f->IsNotInit())
					//							{	
					//								LoadFromFileCreatureActionSPK(body, action);
					//							}
					
					int cx_f		= Frame_f.GetCX();
					int cy_f		= Frame_f.GetCY();
					
					pointTemp2.x = pPoint->x + cx_f + cxStep;
					pointTemp2.y = pPoint->y + cy_f + cyStep;
					cxStep += cxInc;
					cyStep += cyInc;
					
#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명
					if(shadowCount)
						m_pSurface->BltIndexSpriteAlpha(&pointTemp2, pSprite_f, 10);
					else
						m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite_f, darkbits[f]);																
#else if
					m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite_f, darkbits[f]);																
#endif
				}
			}	// shadowcount	


			//////////////////////////////////////////////////////////////////////////
			// Draw PC Vampire Weapon Start - chyaya 2007.05.15
			
			if(player)	
			{
				MCreatureWear *pCreatureWear = dynamic_cast<MCreatureWear *>(pCreature);
				
				if(pCreatureWear && !pCreatureWear->IsGhost(2))
				{
					const MCreatureWear::ADDON_INFO& addonInfoWeapon = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
					bool bWeapon = addonInfoWeapon.bAddon && addonInfoWeapon.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON;
					
					const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
					
					if(bWeapon)
					{
						int drawWeapon = addonInfo.FrameID % 2;
						
						FRAME_ARRAY &FA = m_VampireFPK[drawWeapon][drawAction][direction];
						
						// 있는 동작인 경우
						if (FA.GetSize() > frame)
						{
							CFrame &Frame = FA[frame];					
							int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
							int cx		= Frame.GetCX();		//m_AddonFPK[clothes][action][direction][frame].GetCX();
							int cy		= Frame.GetCY();		//m_AddonFPK[clothes][action][direction][frame].GetCY();
							
							CIndexSprite* pSprite = &m_VampireSPK[ sprite ];					
							
							pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
							pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
							
							int colorset1, colorset2;
							
							colorset1 = addonInfoWeapon.ColorSet1;
							colorset2 = addonInfoWeapon.ColorSet2;
							if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
								colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
							else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
								colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;
							
							if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
								colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
							else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
								colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;
							
							CIndexSprite::SetUsingColorSet(colorset1, colorset2);
							m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
							
						} // if (FA.GetSize() > frame)
						
					} // if(bWeapon)
					
				} // if(pCreatureWear && && !pCreatureWear->IsGhost(2))
				
			} // if(player)
			
			// Draw PC Vampire Weapon End - chyaya 2007.05.15
			//////////////////////////////////////////////////////////////////////////
			
		}
	}

	//----------------------------------------------------
	// 관 출력
	//----------------------------------------------------
	if (bInCasket)
	{
		// 그림자 때문에 *2한다.
		int casketID = SPRITEID_CASKET_1 + pCreature->GetCasketType()*2;
		
		if (casketID < m_EtcSPK.GetSize())
		{
			CSprite* pSprite = &m_EtcSPK[casketID];
			
			if (pSprite!=NULL)
			{
				// tile의 중심에서 세운다.
				pointTemp.x = pPoint->x + TILE_X_HALF - (pSprite->GetWidth()>>1);
				pointTemp.y = pPoint->y + TILE_Y - pSprite->GetHeight();
				
				//---------------------------------------- 
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				if(frameindex ==0 )
					pCreature->SetScreenRect( &rect );
				
				//---------------------------------------- 	
				// 무조건 다 출력
				//---------------------------------------- 	
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );						
			}
		}
	}
}

void	MTopView::DrawSelectedAdvancementVampireCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame, int body, int frameindex )
{
		//------------------------------------------------
	// 관 출력정보
	//------------------------------------------------
	int casketValue = pCreature->GetCasketCount();
	bool bInCasket = pCreature->IsInCasket();
	bool bCasketOnly = (bInCasket && casketValue==0);
	int creature_type = pCreature->GetCreatureType();

	action = GetAdvancementVampireActionFromVampireAction( action, pCreature );

	if( action == -1 )
		return;
	else
		action -= ADVANCEMENT_ACTION_START;

	//CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? m_AdvancementVampireManFPK : m_AdvancementVampireWomanFPK;
	//CIndexSpritePack& advanceVampireSPK = pCreature->IsMale() ? m_AdvancementVampireManSPK : m_AdvancementVampireWomanSPK;	

	//by viva vampire
	int clothes = 0, clothes_weapon = 1;
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	bool bCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT;
	bool bNewCoat = bCoat && (addonInfo.ItemType == 32 || addonInfo.ItemType == 33 || addonInfo.ItemType == 34|| addonInfo.ItemType == 35|| addonInfo.ItemType == 36|| addonInfo.ItemType == 37|| addonInfo.ItemType == 38 || addonInfo.ItemType == 39);	//rallser201V
	bool b231NewCoat = bCoat && (addonInfo.ItemType == 40|| addonInfo.ItemType == 41  || addonInfo.ItemType == 42 || addonInfo.ItemType == 43);
	const MCreatureWear::ADDON_INFO& addonInfo2 = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bNewWeapon = addonInfo2.bAddon && addonInfo2.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON && (addonInfo2.ItemType == 26 || addonInfo2.ItemType == 27 || addonInfo2.ItemType == 28 || addonInfo2.ItemType == 29 || addonInfo2.ItemType == 30 || addonInfo2.ItemType == 31);
	bool b231NewWeapon = addonInfo2.bAddon && addonInfo2.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON && (addonInfo2.ItemType >= 32); 

	if(b231NewCoat == false)
	{
	   	if(bNewCoat == true && bNewWeapon == true)
		{
			if(addonInfo.ItemType == 32 || addonInfo.ItemType == 33 || addonInfo.ItemType == 34|| addonInfo.ItemType == 35|| addonInfo.ItemType == 36|| addonInfo.ItemType == 37 || addonInfo.ItemType == 38|| addonInfo.ItemType == 39)
				clothes = 0;

			clothes_weapon = 2;
		}
		else if(bNewCoat == true && bNewWeapon == false)
		{

			if(addonInfo.ItemType == 32 || addonInfo.ItemType == 33 || addonInfo.ItemType == 34|| addonInfo.ItemType == 35|| addonInfo.ItemType == 36|| addonInfo.ItemType == 37 || addonInfo.ItemType == 38|| addonInfo.ItemType == 39)
				clothes = 0;

			clothes_weapon = 2;
		}
		else if(bNewCoat == false && bNewWeapon == true)
		{

			if(addonInfo.ItemType == 30 || addonInfo.ItemType == 31)
				clothes = 1;
			//    clothes_weapon = 3;
			else
				clothes =0;	
				clothes_weapon = 2;
			
		}
			else if(bNewCoat == false && bNewWeapon == false)
		{

			if(addonInfo.ItemType == 30 || addonInfo.ItemType == 31)
				clothes = 1;
			//    clothes_weapon = 3;
		else
				clothes =0;	
				clothes_weapon = 2;
			
		}
	}
	else
	{
		clothes = 0;
		clothes_weapon =1;
	}

	action = GetAdvancementVampireAction171[action];
	if(action == -1)	return;

	CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? (b231NewCoat ? m_AdvancementVampireMan231FPK :(bNewCoat ? m_AdvancementVampireMan201FPK : m_AdvancementVampireMan171FPK)) : (b231NewCoat ? m_AdvancementVampireWoman231FPK : (bNewCoat ? m_AdvancementVampireWoman201FPK : m_AdvancementVampireWoman171FPK));
	CIndexSpritePack& advanceVampireSPK = pCreature->IsMale() ? (b231NewCoat ? m_AdvancementVampireMan231SPK : (bNewCoat ? m_AdvancementVampireMan201SPK : m_AdvancementVampireMan171SPK)) : (b231NewCoat ? m_AdvancementVampireWoman231SPK : (bNewCoat ? m_AdvancementVampireWoman201SPK : m_AdvancementVampireWoman171SPK));

	
	if (!bCasketOnly)
	{	
		for( int i = 0; i <= 1; ++i )
		{
			int k = 0;
			if(i == 0)	k = clothes;
			else		k = clothes_weapon;
			// 0:body 1: 무기 착탈에 따라 나중에 수정 
			FRAME_ARRAY& FA = advanceVampireFPK[k][action][direction];
			
			if (FA.GetSize() > frame)
			{			
				CFrame&	Frame = FA[frame];

				int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
					cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
					cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
				
				CIndexSprite* pSprite = &advanceVampireSPK[ sprite ];
				
				//					if (pSprite->IsNotInit())
				//					{
				//						LoadFromFileCreatureActionSPK( body, action );
				//					}
				//by viva xy select
				// 좌표 보정
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();			
				
				//---------------------------------------- 		
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				if(frameindex ==0 && i == 0)
					pCreature->SetScreenRect( &rect );
				
				WORD colorSet = pCreature->GetAttachEffectColor();
				
				//if( pCreature->GetCreatureType() == CREATURETYPE_BAT && pCreature->GetBatColor() != 0xFFFF)
				if((pCreature->GetCreatureType() == CREATURETYPE_BAT || pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST) 
					&& pCreature->GetBatColor() != 0xFFFF)
					colorSet = pCreature->GetBatColor();
				//---------------------------------------- 
				// 캐릭터 전체 색깔이 바뀌는 경우
				//---------------------------------------- 
				if (colorSet < MAX_COLORSET)
				{
					int nExtColorSet = pCreature->GetItemOptionExtColorSet(colorSet);
					m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, nExtColorSet );
				}					
				//---------------------------------------- 
				// 정상적인 출력
				//---------------------------------------- 										
				else
				{					
					int colorSet1 = pCreature->GetBodyColor1();
					int colorSet2 = pCreature->GetBodyColor2();
					
					if ( colorSet1 == UNIQUE_ITEM_COLOR || colorSet1 == QUEST_ITEM_COLOR )
						colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
					else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;
#if __CONTENTS(__FAST_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_NEDE)
					colorSet1 = pCreature->GetWingColor();
					else if (pCreature->GetCreatureType() == CREATURETYPE_KLTL)
						colorSet1 = pCreature->GetWingColor();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					else if (pCreature->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
					colorSet1 = pCreature->GetWingColor();

#endif //__SECOND_TRANSFORTER
					if ( colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR )
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
					else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
				}
				
				
				int shadowCount = pCreature->GetShadowCount();
				
				//----------------------------------------
				// Shadow출력
				//----------------------------------------
				if (shadowCount)
				{
					// 0, 1, 2, 3, 4, 5
					const int darkbits[6] =
					{
						0, 1, 1, 2, 2, 3
					};
					
					POINT pointTemp2;
					
					// 이동이면 좌표도 바뀐다
					int cxStep = 0, cyStep = 0, cxInc =0 , cyInc = 0;
					if (action==ACTION_MOVE)
					{
						cxInc = -pCreature->GetCX();
						cyInc = -pCreature->GetCY();
					}
					
					
					int faNum = FA.GetSize();
					for (int f=1; f<=shadowCount; f++)
					{
						int frame_f = frame-f;
						
						//---------------------------------------- 
						// f frame 전
						//---------------------------------------- 
						if (frame_f < 0)
						{
							frame_f += faNum;
						}
						
						CFrame &Frame_f = FA[frame_f];
						int sprite_f	= Frame_f.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
						
						CIndexSprite* pSprite_f = &advanceVampireSPK[ sprite_f ];
						
						//							if (pSprite_f->IsNotInit())
						//							{	
						//								LoadFromFileCreatureActionSPK(body, action);
						//							}
						
						int cx_f		= Frame_f.GetCX();
						int cy_f		= Frame_f.GetCY();
						
						pointTemp2.x = pPoint->x + cx_f + cxStep;
						pointTemp2.y = pPoint->y + cy_f + cyStep;
						cxStep += cxInc;
						cyStep += cyInc;
						
						m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite_f, darkbits[f]);																
					}
				}	// shadowcount			
			}
		}
	}

	//----------------------------------------------------
	// 관 출력
	//----------------------------------------------------
	if (bInCasket)
	{
		// 그림자 때문에 *2한다.
		int casketID = SPRITEID_CASKET_1 + pCreature->GetCasketType()*2;
		
		if (casketID < m_EtcSPK.GetSize())
		{
			CSprite* pSprite = &m_EtcSPK[casketID];
			
			if (pSprite!=NULL)
			{
				// tile의 중심에서 세운다.
				pointTemp.x = pPoint->x + TILE_X_HALF - (pSprite->GetWidth()>>1);
				pointTemp.y = pPoint->y + TILE_Y - pSprite->GetHeight();
				
				//---------------------------------------- 
				// 캐릭터 선택 사각형 영역 설정
				//---------------------------------------- 	
				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				if(frameindex ==0 )
					pCreature->SetScreenRect( &rect );
				
				//---------------------------------------- 	
				// 무조건 다 출력
				//---------------------------------------- 	
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );						
			}
		}
	}
}

void	MTopView::DrawSelectedAdvancementSlayerCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	int action_old = action;
	action = ConvAdvancementSlayerActionFromSlayerAction( action, dynamic_cast< MCreatureWear* >(pCreature) );

	if( action == -1 )
		return;
	else
		action -= ADVANCEMENT_ACTION_START;

	MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;
	// Creature의 Action에 맞는 add-on을 출력한다.
	//action = pCreature->GetAction();	
	int clothes;
	BYTE clothesType;

	//CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerManFPK : m_AdvancementSlayerWomanFPK;
	//CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerManSPK : m_AdvancementSlayerWomanSPK;
	//by viva slayer
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( AC_ADDON_BODY );
	bool bNewCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_COAT &&
		(addonInfo.ItemType == 36 || addonInfo.ItemType == 37 || addonInfo.ItemType == 38 || addonInfo.ItemType == 39 || addonInfo.ItemType == 40 || addonInfo.ItemType == 41 || addonInfo.ItemType == 42 || addonInfo.ItemType == 43);//|| addonInfo.ItemType == 34 || addonInfo.ItemType == 35);//t222
	bool b231NewCoat =	addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_COAT && (addonInfo.ItemType == 44 || addonInfo.ItemType == 45  || addonInfo.ItemType == 46 || addonInfo.ItemType == 47);
	if(bNewCoat == true && b231NewCoat == false)
	{
		action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );
		if( action == -1 )
			return;
		else
			action -= ADVANCEMENT_ACTION_START;

	CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerMan201FPK : m_AdvancementSlayerWoman201FPK;//cfpk
	CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerMan201SPK : m_AdvancementSlayerWoman201SPK;//ispk
			

	CCreatureFramePack& slayerMoToFPK = m_AdvancementSlayerWoman171FPK;//cfpk
	CIndexSpritePack& addonMoToISPK = m_AdvancementSlayerWoman171SPK;//ispk

	bool showbody = true;

	for( int i = 0; i < AC_ADDON_MAX; ++i )
	{
		clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

		const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( clothesType );

		if(clothesType == 4)
		{
			clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );

			if(clothesType == AC_ADDON_MOTOR 
				&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
				&& (addonInfo.ItemType == 11))
			{
				action = 9;
			}

			if( clothes == -1 ) continue;

			if(clothes == 23 || clothes == 24 || clothes == 25 || clothes == 26 || clothes == 27 || clothes == 28)
			{
				showbody = false;
			}
			FRAME_ARRAY& FA = slayerMoToFPK[ clothes ][ action ][ direction ];
			int size = slayerMoToFPK.GetSize();
			if( FA.GetSize() > frame )
			{
				CFrame &Frame = FA[frame];					
				int sprite	= Frame.GetSpriteID();
				int cx		= Frame.GetCX();
				int cy		= Frame.GetCY();

				CIndexSprite* pSprite = &addonMoToISPK[ sprite ];
				pointTemp.x = pPoint->x + cx;
				pointTemp.y = pPoint->y + cy;

				rect.left	= pointTemp.x;
				rect.top	= pointTemp.y;
				rect.right	= rect.left + pSprite->GetWidth();
				rect.bottom = rect.top + pSprite->GetHeight();
				pCreature->AddScreenRect( &rect );

				if (addonInfo.bEffectColor)	
				{
					m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
						addonInfo.EffectColorSet);
				}
				else
				{
					if( addonInfo.bAddon )
					{
						int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
						if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
						{
							colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
							if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
								colorSet1 = colorSet2;
						}
						else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						{
							colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
							if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
								colorSet1 = colorSet2;
						}
						CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					}
					else
						CIndexSprite::SetUsingColorSet( 375, 375 );


					if (pCreature->IsFade())
					{	
						m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
					}
					else
					{
						m_pSurface->BltIndexSprite(&pointTemp, pSprite);
					}
				}	
			}
		}

		if( addonInfo.bAddon || clothesType == AC_ADDON_BODY && clothesType != 4)
		{
			if(showbody == false)
				break;
				// 지금은 파트 그대로를 사용한다. 추후에 옷이 추가 되었을때 다음 변수를 세팅 해서 쓴다.
				// addonInfo.FrameID 에 옷 정보를 넣고 세팅하여 쓰면된다.

				clothes = GetAdvancementPartFromItemClass201( addonInfo.ItemClass , addonInfo.FrameID, addonInfo.ItemType);

				if( clothes == -1 )
					continue;

a1:			FRAME_ARRAY &FA = slayerFPK[clothes][action][direction];

				// 있는 동작인 경우
				if (FA.GetSize() > frame)
				{
					CFrame &Frame = FA[frame];					
					int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
					int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

					pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
					pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

					CIndexSprite* pSprite = &addonISPK[ sprite ];

					// 복장Sprite가 초기화 되지 않은 경우
					//						if (pSprite->IsNotInit())
					//						{
					//							LoadFromFileAddonSPK( clothes, action );
					//							//continue;
					//						}

					//---------------------------------------- 
					// 캐릭터 선택 사각형 영역 설정
					//---------------------------------------- 	
					rect.left	= pointTemp.x;
					rect.top	= pointTemp.y;
					rect.right	= rect.left + pSprite->GetWidth();
					rect.bottom = rect.top + pSprite->GetHeight();
					pCreature->AddScreenRect( &rect );

					// SpriteOutlineManager에 추가
					//CIndexSprite::SetUsingColorSet((pCreature->GetID()+3*(clothesType,1),7)%MAX_COLORSET, ((pCreature->GetID()*pCreature->GetID(),7*(clothesType+2)))%MAX_COLORSET);
					//CIndexSprite::SetUsingColorSet((c1+3*(clothesType+1)+7)%MAX_COLORSET, ((c2*c2+7*(clothesType+2)))%MAX_COLORSET);
					//---------------------------------------- 
					// 캐릭터 부분 색깔이 바뀌는 경우
					//---------------------------------------- 
					if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
					{
						m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
					}
					//---------------------------------------- 
					// 원래색대로..
					//---------------------------------------- 
					else
					{
						int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
						if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
						{
							if(colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
								colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );

							if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
								colorSet1 = colorSet2;
						}							
						else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						{
							colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

							if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
								colorSet1 = colorSet2;
						}			

						CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
						m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
					}
				}
//#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
//				//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
//				//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
//				if ( clothesType == AC_ADDON_MOTOR 
//					&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
//					&& (addonInfo.ItemType == 8
//#if __CONTENTS(__SECOND_TRANSFORTER)
//					||addonInfo.ItemType == 9
//					||addonInfo.ItemType == 10
//#endif //__SECOND_TRANSFORTER
//					))
//					break;
//#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				if(clothes == 19 || clothes == 20)
				{
					clothes = clothes+2;
					goto a1;
				}
			}
		}
	}
	else if(bNewCoat == false && b231NewCoat == true)
				{
					action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );
					if( action == -1 )
						return;
					else
						action -= ADVANCEMENT_ACTION_START;

					CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerMan231FPK : m_AdvancementSlayerWoman231FPK;//cfpk
					CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerMan231SPK : m_AdvancementSlayerWoman231SPK;//ispk


					CCreatureFramePack& slayerMoToFPK = m_AdvancementSlayerWoman171FPK;//cfpk
					CIndexSpritePack& addonMoToISPK = m_AdvancementSlayerWoman171SPK;//ispk

					bool showbody = true;

					for( int i = 0; i < AC_ADDON_MAX; ++i )
					{
						clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

						const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( clothesType );

						if(clothesType == 4)
						{
							clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );

							if(clothesType == AC_ADDON_MOTOR 
								&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
								&& (addonInfo.ItemType == 11))
							{
								action = 9;
							}

							if( clothes == -1 ) continue;

							if(clothes == 23 || clothes == 24 || clothes == 25 || clothes == 26 || clothes == 27 || clothes == 28)
							{
								showbody = false;
							}

							FRAME_ARRAY& FA = slayerMoToFPK[ clothes ][ action ][ direction ];
							int size = slayerMoToFPK.GetSize();
							if( FA.GetSize() > frame )
							{
								CFrame &Frame = FA[frame];					
								int sprite	= Frame.GetSpriteID();
								int cx		= Frame.GetCX();
								int cy		= Frame.GetCY();

								CIndexSprite* pSprite = &addonMoToISPK[ sprite ];
								pointTemp.x = pPoint->x + cx;
								pointTemp.y = pPoint->y + cy;

								rect.left	= pointTemp.x;
								rect.top	= pointTemp.y;
								rect.right	= rect.left + pSprite->GetWidth();
								rect.bottom = rect.top + pSprite->GetHeight();
								pCreature->AddScreenRect( &rect );

								if (addonInfo.bEffectColor)	
								{
									m_pSurface->BltIndexSpriteColorSet(&pointTemp, pSprite, 
										addonInfo.EffectColorSet);
								}
								else
								{
									if( addonInfo.bAddon )
									{
										int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
										if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
										{
											colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
											if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
												colorSet1 = colorSet2;
										}
										else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
										{
											colorSet2 = pCreature->GetItemOptionExtColorSet(colorSet2) ;
											if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
												colorSet1 = colorSet2;
										}
										CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
									}
									else
										CIndexSprite::SetUsingColorSet( 375, 375 );


									if (pCreature->IsFade())
									{	
										m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
									}
									else
									{
										m_pSurface->BltIndexSprite(&pointTemp, pSprite);
									}
								}	
							}
						}

						if( addonInfo.bAddon || clothesType == AC_ADDON_BODY && clothesType != 4)
						{
							if(showbody == false)
								break;
							// 지금은 파트 그대로를 사용한다. 추후에 옷이 추가 되었을때 다음 변수를 세팅 해서 쓴다.
							// addonInfo.FrameID 에 옷 정보를 넣고 세팅하여 쓰면된다.

							clothes = GetAdvancementPartFromItemClass231( addonInfo.ItemClass , addonInfo.FrameID, addonInfo.ItemType);

							if( clothes == -1 )
								continue;

a3:							FRAME_ARRAY &FA = slayerFPK[clothes][action][direction];

							// 있는 동작인 경우
							if (FA.GetSize() > frame)
							{
								CFrame &Frame = FA[frame];					
								int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
								int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
								int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

								pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
								pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

								CIndexSprite* pSprite = &addonISPK[ sprite ];

								// 복장Sprite가 초기화 되지 않은 경우
								//						if (pSprite->IsNotInit())
								//						{
								//							LoadFromFileAddonSPK( clothes, action );
								//							//continue;
								//						}

								//---------------------------------------- 
								// 캐릭터 선택 사각형 영역 설정
								//---------------------------------------- 	
								rect.left	= pointTemp.x;
								rect.top	= pointTemp.y;
								rect.right	= rect.left + pSprite->GetWidth();
								rect.bottom = rect.top + pSprite->GetHeight();
								pCreature->AddScreenRect( &rect );

								// SpriteOutlineManager에 추가
								//CIndexSprite::SetUsingColorSet((pCreature->GetID()+3*(clothesType,1),7)%MAX_COLORSET, ((pCreature->GetID()*pCreature->GetID(),7*(clothesType+2)))%MAX_COLORSET);
								//CIndexSprite::SetUsingColorSet((c1+3*(clothesType+1)+7)%MAX_COLORSET, ((c2*c2+7*(clothesType+2)))%MAX_COLORSET);
								//---------------------------------------- 
								// 캐릭터 부분 색깔이 바뀌는 경우
								//---------------------------------------- 
								if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
								{
									m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
								}
								//---------------------------------------- 
								// 원래색대로..
								//---------------------------------------- 
								else
								{
									int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
									if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
									{
										if(colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
											colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );

										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}							
									else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
									{
										colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

										if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
											colorSet1 = colorSet2;
									}			

									CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
									m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
								}
							}
							if(clothes == 19 || clothes == 20)
							{
								clothes = clothes+2;
								goto a3;
							}
						}
					}
				}
	else
	{ 
				action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );
				if( action == -1 )
					return;
				else
					action -= ADVANCEMENT_ACTION_START;
				CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerMan171FPK : m_AdvancementSlayerWoman171FPK;
				CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerMan171SPK : m_AdvancementSlayerWoman171SPK;
		for (int i=0; i<AC_ADDON_MAX; i++)
		{
			// Creature의 현재 방향에 따라서...
			// 옷을 출력해주는 순서가 다를 수 있다.
			clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

			// i번째 종류의 옷을 입고 있다면 출력해 준다.
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( clothesType );

			if (addonInfo.bAddon || i == AC_ADDON_BODY )
			{
				// 지금은 파트 그대로를 사용한다. 추후에 옷이 추가 되었을때 다음 변수를 세팅 해서 쓴다.
				// addonInfo.FrameID 에 옷 정보를 넣고 세팅하여 쓰면된다.

			clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass, addonInfo.FrameID, addonInfo.ItemType );//t222


				if(clothesType == AC_ADDON_MOTOR 
					&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
					&& (addonInfo.ItemType == 11))
				{
					action = 9;
				}

				if( clothes == -1 ) continue;


a2:					FRAME_ARRAY &FA = slayerFPK[clothes][action][direction];

				// 있는 동작인 경우
				if (FA.GetSize() > frame)
				{
					CFrame &Frame = FA[frame];					
					int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
					int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();

					pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
					pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

					CIndexSprite* pSprite = &addonISPK[ sprite ];

					// 복장Sprite가 초기화 되지 않은 경우
					//						if (pSprite->IsNotInit())
					//						{
					//							LoadFromFileAddonSPK( clothes, action );
					//							//continue;
					//						}

					//---------------------------------------- 
					// 캐릭터 선택 사각형 영역 설정
					//---------------------------------------- 	
					rect.left	= pointTemp.x;
					rect.top	= pointTemp.y;
					rect.right	= rect.left + pSprite->GetWidth();
					rect.bottom = rect.top + pSprite->GetHeight();
					pCreature->AddScreenRect( &rect );

					// SpriteOutlineManager에 추가
					//CIndexSprite::SetUsingColorSet((pCreature->GetID()+3*(clothesType,1),7)%MAX_COLORSET, ((pCreature->GetID()*pCreature->GetID(),7*(clothesType+2)))%MAX_COLORSET);
					//CIndexSprite::SetUsingColorSet((c1+3*(clothesType+1)+7)%MAX_COLORSET, ((c2*c2+7*(clothesType+2)))%MAX_COLORSET);
					//---------------------------------------- 
					// 캐릭터 부분 색깔이 바뀌는 경우
					//---------------------------------------- 
					if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
					{
						m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
					}
					//---------------------------------------- 
					// 원래색대로..
					//---------------------------------------- 
					else
					{
						int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
						if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
						{
							if(colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
								colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );

							if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
								colorSet1 = colorSet2;
						}							
						else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						{
							colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

							if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
								colorSet1 = colorSet2;
						}			

						CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
						m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
					}
				}
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
				//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
				//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
				if ( clothesType == AC_ADDON_MOTOR 
					&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
					&& (addonInfo.ItemType == 8
#if __CONTENTS(__SECOND_TRANSFORTER)
					||addonInfo.ItemType == 9
					||addonInfo.ItemType == 10
#endif //__SECOND_TRANSFORTER
					))
					break;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
					if(clothes == 19 || clothes == 20)
						{
							clothes = clothes+2;
							goto a2;
						}
			}
		}
	}
}

void	MTopView::DrawSelectedAdvancementOustersCreature( POINT* pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_1))
		DrawOusterNewWings1( pPoint, pCreature, action, direction, frame );
	else if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_2))
		DrawOusterNewWings2( pPoint, pCreature, action, direction, frame );
	else
	{
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	bool bNewChakram = bChakram && (addonInfoChakram.ItemType == 21 || addonInfoChakram.ItemType == 22 || addonInfoChakram.ItemType == 23 || addonInfoChakram.ItemType == 24 || addonInfoChakram.ItemType == 25 || addonInfoChakram.ItemType == 26);	//by rallser201
	bool b231NewChakram = bChakram && (addonInfoChakram.ItemType == 27);

	int tempAction = ConvAdvancementOustersActionFromOusterAction( action, bChakram );

	if( tempAction == -1 )
		return;
	else
		tempAction -= ADVANCEMENT_ACTION_START;

	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	const MCreatureWear::ADDON_INFO& bootsAddonInfo = pCreatureWear->GetAddonInfo(ADDON_TROUSER);

	bool bNewCoat = addonInfo.ItemClass == ITEM_CLASS_OUSTERS_COAT && (addonInfo.ItemType == 18 || addonInfo.ItemType == 19 || addonInfo.ItemType == 20 || addonInfo.ItemType == 21);//by rallser201 ouster
	bool b231NewCoat = addonInfo.ItemClass == ITEM_CLASS_OUSTERS_COAT && (addonInfo.ItemType >= 22) ;



	tempAction = GetAdvancementOustersAction171[tempAction];

	if(tempAction == -1)	return;

	if(tempAction == 19 || tempAction == 18 || tempAction == 20 || tempAction == 21)
	{
		bNewCoat = false;
		b231NewCoat = false;
	}

	CCreatureFramePack& oustersFPK = b231NewCoat ? m_AdvancementOusters231FPK :(bNewCoat ? m_AdvancementOusters201FPK : m_AdvancementOusters171FPK);
	CIndexSpritePack& oustersSPK = b231NewCoat ? m_AdvancementOusters231SPK:(bNewCoat ? m_AdvancementOusters201SPK : m_AdvancementOusters171SPK);
	
	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		int clothes = 1;		

		if(bNewCoat == true)
		{
			if(addonInfo.ItemType == 18 || addonInfo.ItemType == 19 || addonInfo.ItemType == 20 || addonInfo.ItemType == 21)

				clothes = 2;


		}
		else if(bNewCoat == false)
		{
			if(addonInfo.ItemType == 17)

				clothes = 3;
			else if(b231NewCoat)
			{
				if(addonInfo.ItemType >= 22)
					clothes = 2;
			} 
			else 

				clothes = 2;
		}
		
		FRAME_ARRAY &FA = oustersFPK[clothes][tempAction][direction];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CIndexSprite* pSprite = &oustersSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 선택 사각형 영역 설정
			//---------------------------------------- 	
			rect.left	= pointTemp.x;
			rect.top	= pointTemp.y;
			rect.right	= rect.left + pSprite->GetWidth();
			rect.bottom = rect.top + pSprite->GetHeight();
			pCreature->AddScreenRect( &rect );
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfo.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				CIndexSprite::SetUsingColorSetOnly( 0, pCreature->GetBodyColor1() );	// 머리색
				
				int coatColor = addonInfo.ColorSet2, Colorset;
				if( coatColor == QUEST_ITEM_COLOR || coatColor == UNIQUE_ITEM_COLOR )
					coatColor = MItem::GetSpecialColorItemColorset( coatColor );
				else if(coatColor >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					coatColor =  pCreature->GetItemOptionExtColorSet(coatColor) ;
				CIndexSprite::SetUsingColorSetOnly( 1, coatColor );	// 옷색
				if(bChakram == false)	// 리스틀릿을 차고 있으면
				{
					if(addonInfoChakram.bAddon)
					{
						Colorset = addonInfoChakram.ColorSet2;
						if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔								
					}
					else
					{
						Colorset = defaultCoatColor;								
						if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
				}
				else
				{
					Colorset = defaultCoatColor;
					if(addonInfoChakram.bAddon)
						Colorset = addonInfoChakram.ColorSet2;
					if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
				}
				
				if(bootsAddonInfo.bAddon)
				{
					Colorset = bootsAddonInfo.ColorSet2;
					if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 부츠색
				}
				else
				{
					Colorset = defaultCoatColor;
					
					if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// 팔
				}
				
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
			}						
			
		}
	}
	
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		int clothes = 0;

		if(bNewCoat == true && bNewChakram == true)
			clothes = 0;
		else if(bNewCoat == true && bNewChakram == false)
			clothes = 0;
		else if(bNewCoat == false && bNewChakram == true)
			clothes = 1;
		else if(bNewCoat == false && bNewChakram == false)
			clothes = 0;
		
		
		
		FRAME_ARRAY &FA = oustersFPK[clothes][tempAction][direction];
		
		// 있는 동작인 경우
		if (FA.GetSize() > frame)
		{
			CFrame &Frame = FA[frame];					
			int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
			int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
			
			CIndexSprite* pSprite = &oustersSPK[ sprite ];					
			
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			
			//---------------------------------------- 
			// 캐릭터 부분 색깔이 바뀌는 경우
			//---------------------------------------- 
			if (addonInfoChakram.bEffectColor)	//colorSet < MAX_COLORSET)
			{
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite, addonInfo.EffectColorSet );
			}
			//---------------------------------------- 
			// 정상적인 캐릭터 출력
			//---------------------------------------- 
			else
			{
				int colorset1, colorset2;
				
				colorset1 = addonInfoChakram.ColorSet1;
				colorset2 = addonInfoChakram.ColorSet2;
				if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
					colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
				else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;

				if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
					colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
				else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;
				
				
				CIndexSprite::SetUsingColorSet(colorset1, colorset2);
				m_SOM.Add( pointTemp.x, pointTemp.y, pSprite );
			}						
			
		}
	}
	}
}

/*
int	MTopView::GetAdvancementComboFrame(MCreature* pCreature,int &Frame , int &action , int &ComboActionCnt, int &MaxFrame)
{
	int	 frame = Frame	;
	int  iStartFrame	= 0	 ;
	int	 iCancelAction	= 0  ;
	WORD fAttect_Cancel_Type = 0 ; 
	WORD fWeaponType	= 0 ; 
	WORD fWeaponSpeed	= 0 ;
	WORD fCreatureType	= 0 ; 	
	BYTE byCurrenCombo	= 0 ;
	
	
	if(pCreature->IsSlayer())  // 슬레이어 
	{
		if( ((action >= ACTION_SLAYER_SWORD) && (action <= ACTION_SLAYER_BLADE)) ||
			((action >= ACTION_SLAYER_SWORD_2) && (action <= ACTION_SLAYER_BLADE_2)) ||
			((action >= ACTION_SLAYER_SWORD_SLOW) && (action <= ACTION_SLAYER_BLADE_2_FAST)) ||
			((action >= ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW) && (action <= ACTION_ADVANCEMENT_SLAYERN_COMBO_CANCEL_SWARD_FAST))
		)
		{
			//WORD fWeaponType = 3 ;  //(*g_pActionInfoTable)[action].GetWeaponType();
			
			// 칼 
			if( (action == ACTION_SLAYER_SWORD) || (action == ACTION_SLAYER_SWORD_2) ||
				(action == ACTION_SLAYER_SWORD_SLOW) || (action == ACTION_SLAYER_SWORD_2_SLOW) ||
				(action == ACTION_SLAYER_SWORD_FAST) || (action == ACTION_SLAYER_SWORD_2_FAST) ||
				((action >= ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW) && (action <= ACTION_ADVANCEMENT_SLAYERN_COMBO_CANCEL_SWARD_FAST)) 
			)
			{	
				//if(!(fWeaponType == FLAG_ACTIONINFO_WEAPON_SWORD || fWeaponType == FLAG_ACTIONINFO_WEAPON_BLADE))
				//	return frame;
				fWeaponType = 0 ; // 칼 
				//if(fWeaponType == FLAG_ACTIONINFO_WEAPON_BLADE) fWeaponType = 3 ; // 도
			}
			// 도 
			else  // action >= ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW) && (action <= ACTION_ADVANCEMENT_SLAYERN_COMBO_CANCEL_SWARD_FAST))
			{
				//fWeaponType = 0 ; // 칼
				
				fWeaponType = 3 ; // 도
			}

			//fWeaponType = 0 ; // 칼 
			//fWeaponSpeed = 1 ; 

			/*if(action == ACTION_SLAYER_SWORD_SLOW   || action == ACTION_SLAYER_BLADE_SLOW || 
               action == ACTION_SLAYER_SWORD_2_SLOW || action == ACTION_SLAYER_BLADE_2_SLOW ||   
			   action == ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW || action == ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_BLADE_SLOW)   
				fWeaponSpeed = 0 ; 
			else if(action == ACTION_SLAYER_SWORD_FAST   || action == ACTION_SLAYER_BLADE_FAST || 
               action == ACTION_SLAYER_SWORD_2_FAST || action == ACTION_SLAYER_BLADE_2_FAST ||   
			   action == ACTION_ADVANCEMENT_SLAYERN_COMBO_CANCEL_SWARD_FAST || action == ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_BLADE_SLOW)   
				fWeaponSpeed = 2 ; 
			*/
			/*fWeaponSpeed = pCreature->GetWeaponSpeed() ;

			fCreatureType = 0  ;
			if(pCreature->IsMale()) 	fCreatureType = 1 ;

			if(fWeaponType == 3)	fCreatureType += 2 ; 
		
			
			//fCreatureType = 0 ; // 슬레이어 
			
			if(!(action >= ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW) && (action <= ACTION_ADVANCEMENT_SLAYERN_COMBO_CANCEL_SWARD_FAST))
			{
				fAttect_Cancel_Type = 1 ;
				if(ComboActionCnt >= 1 && ComboActionCnt <= 3)
				{
					iCancelAction = AdvanceComboNextActionTable[fWeaponSpeed + fWeaponType] ;
				}
			}
			else
			{
				fAttect_Cancel_Type = 2 ;
			}
		}
	}
	else if(pCreature->IsVampire()) // 벰파이어 
	{
		if( (action == ACTION_ATTACK) ||
			((action >= ACTION_VAMPIRE_ATTACK_SLOW) && (action <= ACTION_VAMPIRE_SKILL_ATTACK_FAST)) ||
			((action >= ACTION_ADVANCEMENT_COMBO_CANCEL_SLOW) && (action <= ACTION_ADVANCEMENT_COMBO_CANCEL_FAST))
		)
		{
			fCreatureType = 5 ;
			if(pCreature->IsMale()) 	fCreatureType = 4 ;
			

			fWeaponSpeed = pCreature->GetWeaponSpeed() ;
			
			
			if(!(action >= ACTION_ADVANCEMENT_COMBO_CANCEL_SLOW) && (action <= ACTION_ADVANCEMENT_COMBO_CANCEL_FAST))
			{
				fAttect_Cancel_Type = 1 ;
				if(ComboActionCnt >= 1 && ComboActionCnt <= 3)
				{
					iCancelAction = AdvanceComboNextActionTable[ 6 + fWeaponSpeed + fWeaponType] ;
				}
			}
			else
			{
				fAttect_Cancel_Type = 2 ;
			}
		}
	}
	
	else if(pCreature->IsOusters()) // 아우스터 
	{
		if( (action == ACTION_ATTACK) ||
			(action == ACTION_OUSTERS_CHAKRAM) || 
			((action >= ACTION_OUSTERS_ATTACK_SLOW) && (action <= ACTION_OUSTERS_CHAKRAM_FAST)) ||
			((action >= ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW) && (action <= ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_FAST))
		)
		{
			fWeaponSpeed = pCreature->GetWeaponSpeed() ;
			fCreatureType = 6 ; // 아우스터 
			
			if(!(action >= ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW) && (action <= ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_FAST))
			{
				fAttect_Cancel_Type = 1 ;
				if(ComboActionCnt >= 1 && ComboActionCnt <= 3)
				{
					iCancelAction = AdvanceComboNextActionTable[ 9 + fWeaponSpeed + fWeaponType] ;
				}
			}
			else
			{
				fAttect_Cancel_Type = 2 ;
			}
		}
	}
	
	if(fAttect_Cancel_Type == 1)  // Attack 동작 
	{	
		if(ComboActionCnt < 1 || ComboActionCnt >4) return frame ; 

		if(ComboActionCnt < 4)
		{
			if(pCreature  == g_pPlayer)
			{
				if(g_pPlayer->IsRepeatAction())
					pCreature->SetActionCountMax(AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1] + 2)  ; 
				else
				pCreature->SetActionCountMax(AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1])  ; 
			}
		}

		//AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][3] = (MaxFrame - 1) ;
		frame = Frame % (AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1] + 1)  ;
		
		//======================================================================================================
		// 현재 콤보값 가져오기 
		//======================================================================================================
		if(frame <= AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][0])
			byCurrenCombo = 1 ; 
		else if(frame <= AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][1] )
			byCurrenCombo = 2 ; 
		else if(frame <= AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][2] )
			byCurrenCombo = 3 ; 
		else
			byCurrenCombo = 4 ; 
		//======================================================================================================
		
		// 크리쳐가 죽었을때 와 타격 이펙트 처리  Sjheon 2005.07.14 Add
		MCreature* TargetCreature = NULL;
		TargetCreature = g_pZone->GetCreature(pCreature->GetTraceID());
		
		if(TargetCreature)
		{
			if(TargetCreature->IsDead())	// 현재 콤보 기술을  동작을 할 콤보 스텝으로 바꾼다 
			{
				ComboActionCnt	= byCurrenCombo ; 
			}
			else
			{
				//int originalSkill = g_pPlayer->GetUsedActionInfo() ; 
				if(byCurrenCombo && (frame >= (AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][byCurrenCombo - 1] - 2)))
				{
					int iCurAction = pCreature->GetCurrentCombo() ; 
					if(iCurAction != byCurrenCombo)
					{
						pCreature->UseSkill_Effect() ; 
						pCreature->SetCurrentCombo(byCurrenCombo);
					}
				}
			}
		}
		// 크리쳐가 죽었을때 와 타격 이펙트 처리  Sjheon 2005.07.14 End 

		if(frame == AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1] )
		{  
			if(ComboActionCnt != 4)
			{
				pCreature->SetAction(iCancelAction) ;  //  + iComboType) ;	
			}
			pCreature->SetCurrentCombo(0)	;
			//bSendAction[0] = bSendAction[1] = bSendAction[2] = bSendAction[3] = false  ; 
		}
	}
	
	else if(fAttect_Cancel_Type == 2 && ComboActionCnt != 4) // 취소 동작 
	{
		if(ComboActionCnt < 1 || ComboActionCnt >4) return frame ; 
		if(ComboActionCnt != 1)  iStartFrame = AdvanceComboCancelFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 2] +1 ;  
		frame = iStartFrame + Frame % (AdvanceComboCancelFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1] + 1)  ;
		//pCreature->SetFrame(frame) ; 
		
		/*if(frame == (AdvanceComboCancelFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1] - 1)  && !pCreature->GetComCencel())
		{
			pCreature->SetAction(pCreature->GetAction()) ;
			pCreature->SetComboCencel(TRUE) ;
			//Sleep(280) ;

		}
		else if(frame == (AdvanceComboCancelFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1] - 1)  && !pCreature->GetComCencel1())
		{
			pCreature->SetAction(pCreature->GetAction()) ;
			pCreature->SetComboCencel1(TRUE) ;
			//Sleep(280) ;

		}

		else*//*	if(frame == AdvanceComboCancelFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1])
		{	
			if(pCreature  == g_pPlayer)
			{
				if(g_pPlayer->IsRepeatAction())
				{
					pCreature->SetAction(AdvanceComboNextRepeatActionTable[fWeaponSpeed + fWeaponType]) ; //ACTION_STAND)	;
					//pCreature->SetCurrentCombo(ComboActionCnt)	;	// 엑션 Step을 초기화 한
					pCreature->SetCurrentCombo(1)	;	// 엑션 Step을 초기화 한
					//pCreature->SetCombo(0);
					//OutputDebugString("asdfsdf재헌\n") ; 
				}
				else
				{
					pCreature->SetActionCountMax(AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1])  ; 
					pCreature->SetAction(ACTION_STAND)	;
					pCreature->SetCurrentCombo(0)	;	// 엑션 Step을 초기화 한
				}	
			}
			else
			{
				pCreature->SetAction(ACTION_STAND)	;
				pCreature->SetCurrentCombo(0)	;	// 엑션 Step을 초기화 한
			}
			//pCreature->SetCurrentCombo(0)	;	// 엑션 Step을 초기화 한
			pCreature->SetCombo(0);
			//pCreature->SetComboCencel(FALSE) ;
			//pCreature->SetComboCencel1(FALSE) ;


			//pCreature->id
			//OutputDebugString("sdafasf"); 
			//pCreature->SetAction(ACTION_SLAYER_SWORD) ; //ACTION_STAND)	;
			//pCreature->SetCurrentCombo(1)	;	// 엑션 Step을 초기화 한다.
			//if(pCreature == g_pPlayer)
			//{
			//	g_pPlayer->SetComboSkillSend(TRUE) ;
			//}
			
		}
		
	}
	return  frame;
}*/

/*
int	MTopView::GetAdvancementComboFrame(MCreature* pCreature,int &Frame , int &action , int &ComboActionCnt, int &MaxFrame)
{
	int	 frame = Frame	;
	int  iStartFrame	= 0	 ;
	int	 iCancelAction	= 0  ;
	WORD fAttect_Cancel_Type = 0 ; 
	WORD fWeaponType	= 0 ; 
	WORD fWeaponSpeed	= 0 ;
	WORD fCreatureType	= 0 ; 	
	BYTE byCurrenCombo	= 0 ;
	

	int iAdvanceAction = 0 ; 
	
//	if(ComboActionCnt < 4) return frame  ; 
	
//	if(ComboActionCnt >= 0) return frame  ; 

	if(pCreature->IsSlayer())  // 슬레이어 
	{
		if(action >= ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW)
			iAdvanceAction = action ; 
		else 	
			iAdvanceAction = AdvancementSlayerActionConvTable[action];
		
		if(iAdvanceAction >= ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW)
		{
			int iComboIdx = iAdvanceAction - ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW ;
			iComboIdx = iComboIdx / 3 ; 
			if(iComboIdx == 0 || iComboIdx == 2)
			   fWeaponType = 0 ; // 칼 	
			else if(iComboIdx == 1 || iComboIdx == 3)
			   fWeaponType = 3 ; // 도
			
			fWeaponSpeed = pCreature->GetWeaponSpeed() ;

			fCreatureType = 0  ;
			if(pCreature->IsMale()) 	fCreatureType = 1 ;

			if(fWeaponType == 3)	fCreatureType += 2 ; 
		
			
			//fCreatureType = 0 ; // 슬레이어 
			
			if(!(iAdvanceAction >= ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_SWARD_SLOW) && (iAdvanceAction <= ACTION_ADVANCEMENT_SLAYER_COMBO_CANCEL_BLADE_FAST))
			{
				fAttect_Cancel_Type = 1 ;
				if(ComboActionCnt >= 1 && ComboActionCnt <= 3) 
				{
					iCancelAction = AdvanceComboNextActionTable[fWeaponSpeed + fWeaponType] ;
				}
			}
			else
			{
				//fAttect_Cancel_Type = 2 ;
			}
		}
		
	}		
	else if(pCreature->IsVampire()) // 벰파이어 
	{	
		if(action >= ACTION_ADVANCEMENT_COMBO_SLOW)
			iAdvanceAction = action ; 
		else 		
			iAdvanceAction = AdvancementVampireActionConvTable[action];
			
		if(iAdvanceAction >= ACTION_ADVANCEMENT_COMBO_SLOW)
		{
			fCreatureType = 5 ;
			if(pCreature->IsMale()) 	fCreatureType = 4 ;
			

			fWeaponSpeed = pCreature->GetWeaponSpeed() ;
			
			
			if(!(iAdvanceAction >= ACTION_ADVANCEMENT_COMBO_CANCEL_SLOW) && (iAdvanceAction <= ACTION_ADVANCEMENT_COMBO_CANCEL_FAST))
			{
				fAttect_Cancel_Type = 1 ;
				if(ComboActionCnt >= 1 && ComboActionCnt <= 3)
				{
					iCancelAction = AdvanceComboNextActionTable[ 6 + fWeaponSpeed + fWeaponType] ;
				}
			}
			else
			{
				//fAttect_Cancel_Type = 2 ;
			}	
		}

	}		
	else if(pCreature->IsOusters()) // 아우스터 
	{
		if(action >= ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW)
			iAdvanceAction = action ; 
		else 		
			iAdvanceAction	= AdvancementOustersActionConvTable[ action ]; 
		
		if(iAdvanceAction >= ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW)
		{
			fWeaponSpeed = pCreature->GetWeaponSpeed() ;
			fCreatureType = 6 ; // 아우스터 
			
			if(!(iAdvanceAction >= ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_SLOW) && (iAdvanceAction <= ACTION_ADVANCEMENT_OUSTERS_COMBO_CANCEL_FAST))
			{
				fAttect_Cancel_Type = 1 ;
				if(ComboActionCnt >= 1 && ComboActionCnt <= 3)
				{
					iCancelAction = AdvanceComboNextActionTable[ 9 + fWeaponSpeed + fWeaponType] ;
				}
			}
			else
			{
				//fAttect_Cancel_Type = 2 ;
			}
		}
	}
	
	if(fAttect_Cancel_Type == 1)  // Attack 동작 
	{	
		
		
		//else
	///	{
		//if(ComboActionCnt < 1 || ComboActionCnt >4) return frame ; 
		if(ComboActionCnt < 4)
			pCreature->SetActionCountMax(AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1]+2)  ; 
		else 
			pCreature->SetActionCountMax(AdvanceComboActionFrameTable[fCreatureType][fWeaponSpeed][3])  ; 
		// 크리쳐가 죽었을때 와 타격 이펙트 처리  Sjheon 2005.07.14 Add
	
		//}
		
		MCreature* TargetCreature = NULL;
		TargetCreature = g_pZone->GetCreature(pCreature->GetTraceID());
		if(TargetCreature)
		{
			if(TargetCreature->IsDead())	// 현재 콤보 기술을  동작을 할 콤보 스텝으로 바꾼다 
			{
				//pCreature->SetAction(ACTION_STAND)	;
				pCreature->SetCombo(1);
			}
		}
		
		if(action ==  ACTION_ATTACK  && 	ComboActionCnt == 0)
		{
			pCreature->SetCombo(1);
		}
		else if((action ==  ACTION_SLAYER_BLADE_2 || action == ACTION_SLAYER_BLADE_2_FAST || action == ACTION_SLAYER_SWORD_2_FAST)  && 	ComboActionCnt == 0)
		{
			pCreature->SetCombo(2);
		}
		//else if(action ==  ACTION_ATTACK && 	ComboActionCnt == 0)   
		//{
		//	pCreature->SetCombo(1);
		//}
	}
	return  frame;
}*/

//////////////////////////////////////////////////////////////////////////
//ShadowNAlphaCreatureDraw
//어둡게 찍기와 alpha로 그리는 것 함수화 해서 뺐음
//잔상찍는것도 긴밀하게 연관되어 있어서 같이 해결함
//alpha와 alphashadow는 effectstatus를 통해서 세팅되며
//fade, fadeshadow 와 shadowconut는 creature 인포를 통해서 제어 한다.
//alpha, fade는 서로 세불양립 하게 쓰도록 하자
//isfade()는 페이드 인 페이드 아웃의 mTopview함수와 이름이 같으니 주의해서 본다.
//////////////////////////////////////////////////////////////////////////

void 
MTopView::ShadowNAlphaCreatureDraw(MCreature* pCreature, POINT* pPoint, POINT& pointTemp, CIndexSprite* pSprite, int action, int direction, int frame, int drawBody, FRAME_ARRAY& FA)
{
	int shadowCount = pCreature->GetShadowCount();

	BOOL bCreatureAlpha = pCreature->GetAlpha();
	BOOL bCreatureShadowAlpha = pCreature->GetAlpha();

	bool bCreatureFade = pCreature->IsFade();
	bool bCreatureShadowFade = pCreature->IsFade();
	//----------------------------------------
	// 잔상 출력 잔상을 먼저 찍고 본체를 나중에 찍는다.
	//----------------------------------------
	int creature_type = pCreature->GetCreatureType();
	if (shadowCount)
	{
		bool bDrawShadowFirst =
			(direction <= 4)		||
			creature_type == 764	||
			creature_type == 765	||
			creature_type == 2014	||
			creature_type == 2015	||
			creature_type == 891;

		// 0, 1, 2, 3, 4, 5
		const int darkbits[6] =
		{
			0, 1, 1, 2, 2, 3
		};
		
		POINT pointTemp2;
		
		// 이동이면 좌표도 바뀐다
		int cxStep = 0, cyStep = 0, cxInc =0 , cyInc = 0;
		if (action==ACTION_MOVE)
		{
			cxInc = -pCreature->GetCX();
			cyInc = -pCreature->GetCY();
		}
		
		
		int faNum = FA.GetSize();
		for (int f=1; f<=shadowCount; f++)
		{
			int frame_f = frame-f;
			
			//---------------------------------------- 
			// f frame 전
			//---------------------------------------- 
			if (frame_f < 0)
			{
				frame_f += faNum;
			}
			
			CFrame &Frame_f = FA[frame_f];
			int sprite_f	= Frame_f.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
			
			CIndexSprite* pSprite_f = &GetCreatureSprite(sprite_f, drawBody);
			
			//									if (pSprite_f->IsNotInit())
			//									{
			//										LoadFromFileCreatureActionSPK( body, action );
			//									}
			
			int cx_f		= Frame_f.GetCX();
			int cy_f		= Frame_f.GetCY();
			
			pointTemp2.x = pPoint->x + cx_f + cxStep;
			pointTemp2.y = pPoint->y + cy_f + cyStep;
			cxStep += cxInc;
			cyStep += cyInc;
			
			if (bCreatureShadowFade)
				m_pSurface->BltIndexSpriteDarkness(&pointTemp2, pSprite_f, darkbits[f]);																		
#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명
			else if(bCreatureShadowAlpha)
			{
				m_pSurface->BltIndexSpriteAlpha(&pointTemp2, pSprite_f, 10);
			}
#endif // 	__TIPOJYU_CASTLE
			else
			{
				m_pSurface->BltIndexSprite(&pointTemp2, pSprite_f);
			}
		}
		if (!bDrawShadowFirst)
		{
			if (bCreatureFade)
			{
				m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
			}
#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명
			else if(bCreatureAlpha)
			{
				m_pSurface->BltIndexSpriteAlpha(&pointTemp, pSprite, 10);
			}
#endif //__TIPOJYU_CASTLE
			else
			{
				m_pSurface->BltIndexSprite(&pointTemp, pSprite);
			}

		}
		else
		{
			if (bCreatureFade)
			{
				m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
			}
#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명
			else if(bCreatureAlpha)
			{
					m_pSurface->BltIndexSpriteAlpha(&pointTemp, pSprite, 10);
			}
#endif // 	__TIPOJYU_CASTLE
			else
			{
				m_pSurface->BltIndexSprite(&pointTemp, pSprite);
			}
		}
	}
	//----------------------------------------
	// 몸만 찍기
	//----------------------------------------
	else
	{
		if (bCreatureFade)
		{
			m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 1);
		}
#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명
		else if(bCreatureAlpha)
		{
			m_pSurface->BltIndexSpriteAlpha(&pointTemp, pSprite, 10);
		}
#endif // 	__TIPOJYU_CASTLE
		else
		{
			m_pSurface->BltIndexSprite(&pointTemp, pSprite);
		}
	}
}
