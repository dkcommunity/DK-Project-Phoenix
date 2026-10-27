#ifndef __PET_TYPES_H__
#define __PET_TYPES_H__

#include "SystemTypes.h"

enum PetType
{
	PET_NONE = 0,
	PET_COMMON,
	PET_PACKAGE,
	PET_CENTAURO,
	PET_STIRGE,
	PET_PIXIE,			
	PET_LUPENG,			//±«µµ·çÆØ(Æë±Ï)
	//2007.11.20 kim sung yong
	PET_MILITARY_BEAR,    
	//2007.11.20 kim sung yong
	PET_GARGOYLE,	
	PET_FIRE_ELEMENTAL,
	PET_SUCCUBUS,
	PET_INCUBUS,	//2008.12.01 wlzzi
	PET_MASHIMARO,	//12 //by viva
	PET_TABBYCAT,	//13
	PET_WEALTHYCAT,	//14
	PET_SIAMINGCAT,	//15
        PET_Hua,//16
        PET_Ruffian1,//17
        PET_Ruffian2,//18
        PET_DeadBody,//19
        PET_TurningDead,//20
        PET_Kid,//21
        PET_Soldier,//22
        PET_Captain,//23
        PET_Canine,//24
        PET_Degula,    //25
        PET_yml,    //26
        PET_xk2,    //27
        PET_qml,    //28
        PET_hxz,    //29
        PET_CML,    //29
        PET_TIANSHI,    //29
        PET_STS,    //29
	PET_King,//30
};

//20090109 wlzzi - Çª´õ±â´ÉÅ¸ÀÔ
enum PetFoodFunctionType
{
	FOOD_FUNCTION_NONE = 0,
	FOOD_FUNCTION_LUCKY = 1,
	FOOD_FUNCTION_INTERPRETER = 2,
	FOOD_FUNCTION_SMART = 3,	//20090410 ksym555 ½º¸¶Æ® Çªµå
};
//-
typedef BYTE PetType_t;
const uint szPetType = szBYTE;

typedef BYTE PetLevel_t;
const uint szPetLevel = szBYTE;

typedef DWORD PetExp_t;
const uint szPetExp = szDWORD;

typedef WORD PetHP_t;
const uint szPetHP = szWORD;

typedef BYTE PetAttr_t;
const uint szPetAttr = szBYTE;

typedef BYTE PetAttrLevel_t;
const uint szPetAttrLevel = szBYTE;

#endif
