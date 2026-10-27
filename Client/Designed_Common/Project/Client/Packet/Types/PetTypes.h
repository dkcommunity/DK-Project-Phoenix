#ifndef __PET_TYPES_H__
#define __PET_TYPES_H__

#include "SystemTypes.h"

enum PetType
{
	PET_NONE = 0,
	PET_COMMON 				= 1,
	PET_PACKAGE 			= 2,
	PET_CENTAURO 			= 3,
	PET_STIRGE 				= 4,
	PET_PIXIE 				= 5,			
	PET_LUPENG 				= 6,			//
	PET_MILITARY_BEAR 		= 7,    
	PET_GARGOYLE 			= 8,	
	PET_FIRE_ELEMENTAL 		= 9,
	PET_SUCCUBUS 			= 10,
	PET_INCUBUS 			= 11,	//2008.12.01 wlzzi
	PET_MASHIMARO 			= 12,	//12 //by viva
	PET_TABBYCAT 			= 13,	//13
	PET_WEALTHYCAT 			= 14,	//14
	PET_SIAMINGCAT 			= 15,	//15
    PET_Hua 			= 16,//16
    PET_Ruffian1 			= 17,//17
    PET_Ruffian2 			= 18,//18
    PET_DeadBody 			= 19,//19
    PET_TurningDead 			= 20,//20
    PET_Kid 			= 21,//21
    PET_Soldier 			= 22,//22
    PET_Captain 			= 23,//23
    PET_Canine 			= 24,//24
    PET_Degula 			= 25,    //25
};

enum PetFoodFunctionType
{
	FOOD_FUNCTION_NONE = 0,
	FOOD_FUNCTION_LUCKY = 1,
	FOOD_FUNCTION_INTERPRETER = 2,
	FOOD_FUNCTION_SMART = 3,
};
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
