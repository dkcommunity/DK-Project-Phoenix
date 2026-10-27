//////////////////////////////////////////////////////////////////////////////
// Filename    : Shape.cpp
// Written by  : excel96
// Description : 
// ?¥á??? ????? ????????? ????? ??????? ?? ????? ?????.
// ?????? ???? ???? ???? ????? ??????? ????????.
//////////////////////////////////////////////////////////////////////////////

#include "Shape.h"

WeaponType getSwordType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
        case 23:
        case 24:
        case 25:
 	case 26:
	case 27:
 	case 28:
	case 29:
	//	return WEAPON_SWORD_3;
	case 21:
	case 22:
		return WEAPON_SWORD_2;
	default:
		return WEAPON_SWORD;
	}
	return WEAPON_SWORD;
}
WeaponType getBladeType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
	case 23:
        case 24:
        case 25:
        case 26:
	case 27:
 	case 28:
	case 29:
	//	return WEAPON_BLADE_3;
	case 21:
	case 22:
		return WEAPON_BLADE_2;
	default:
		return WEAPON_BLADE;
	}
	return WEAPON_BLADE;
}
WeaponType getSRType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
        case 24:
        case 25:
	case 23:
	//	return WEAPON_SR_3;
	case 21:
	case 22:
        case 26:
	case 27:
 	case 28:
	case 29:
		return WEAPON_SR_2;
	default:
		return WEAPON_SR;
	}
	return WEAPON_SR;
}
WeaponType getARType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
	case 23:
        case 24:
        case 25:
        case 26:
	case 27:
 	case 28:
	case 29:
	//	return WEAPON_AR_3;
	case 21:
	case 22:
		return WEAPON_AR_2;
	default:
		return WEAPON_AR;
	}
	return WEAPON_AR;
}
WeaponType getCrossType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
	case 21:
	case 22:
	case 23:
	//	return WEAPON_CROSS_3;
	case 19:
	case 20:
	case 24:
	case 25:
 	case 26:
	case 27:
		return WEAPON_CROSS_2;
	default:
		return WEAPON_CROSS;
	}
	return WEAPON_CROSS;
}
WeaponType getMaceType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
	case 22:
	case 23:
	case 21:
	//	return WEAPON_MACE_3;
	case 19:
	case 20:
	case 24:
	case 25:
 	case 26:
	case 27:
		return WEAPON_MACE_2;
	default:
		return WEAPON_MACE;
	}
	return WEAPON_MACE;
}
BYTE getShoulderType(ItemType_t IType)
	throw()
{
	return 1;
}

PantsType getPantsType(ItemType_t IType)
	throw()
{
	switch (IType)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return PANTS1;
		case 4:
		case 5:
		case 6:
		case 7:
		case 30: case 31:	//????? ?????? ????
			return PANTS2;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 28: case 29:	//??????? ????
			return PANTS3;             
                case 36: case 37:
		case 38: case 39:
		case 40: case 41:
                        return PANTS5;
		case 42: case 43:
		case 44: case 45:
                        return PANTS6;
		case 22:
		case 23:
   
		default:
//			throw ("getPantsType() : unknown pants type");
			return PANTS4;
			break;
	}

	return PANTS_BASIC;
}

JacketType getJacketType(ItemType_t IType)
	throw()
{
	switch (IType)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return JACKET1;
		case 4:
		case 5:
		case 6:
		case 7:
		case 30: case 31:	//????? ?????? ????
			return JACKET2;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 28: case 29:	//??? ??? ????
			return JACKET3;
		case 24 : case 25 : // ADV 1
		case 26 : case 27 : // ADV 11
		case 32 : case 33 :	// ADV 21
			return JACKET5;	//by viva 171
		case 34 : case 35 : 
			return JACKET6;	//by viva 181
		case 36 : case 37 :  case 38 :  case 39 :  case 40 :  case 41 :  case 42 :  case 43 :  
			return JACKET7;	//by RALLSER 201
		case 44 :  case 45 : 
		case 46 :  case 47: 
			return JACKET8;
		case 22:
		case 23:
		default:
//			throw ("getJacketType() : unknown jacket type");
			return JACKET4;
			break;
	}

	return JACKET_BASIC;
}

HelmetType getHelmetType(ItemType_t IType)
	throw()
{
	switch (IType)
	{
		case 0:
		case 1:
		case 2:
			return HELMET1;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 15:	//?????? ????
//			throw ("getHelmetType() : unknown helmet type");
			return HELMET2;
		case 16:
			return HELMET4;	//by viva 171
		case 17:
			return HELMET5;	//by viva 181
		case 18:
		case 19:
		case 20:
			return HELMET6;	//by RALLSER 201
		case 21:
		case 22:
			return HELMET7;
		case 12:
		default:
			return HELMET3;
			break;
	}

	return HELMET_NONE;
}

ShieldType getShieldType(ItemType_t IType)
	throw()
{
	switch (IType)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			return SHIELD1;
		case 16:
			return SHIELD3;	//by viva 171
		case 17:
			return SHIELD4;	//by viva 181
		case 18:
		case 19:
		case 20:
			return SHIELD5;	//by RALLSER 201
		case 21:
		case 22:
			return SHIELD6;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 15:	//??????
		default:
//			throw ("getShieldType() : unknown shield type");
			return SHIELD2;
			break;
	}

	return SHIELD_NONE;
}

VampireCoatType getVampireCoatType(ItemType_t IType)
	throw()
{
	switch (IType)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return VAMPIRE_COAT1;
		case 4:
		case 5:
		case 6:
		case 7:
		case 26: case 27:	//?????? ??????? ?¥ê?/?¡À? ??? ????
			return VAMPIRE_COAT2;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 24: case 25:	//???©ª??? ??/?? ????
			return VAMPIRE_COAT3;

		case 20: case 21: //adv 1
		case 22: case 23: //adv 11
		case 28: case 29:	// adv 21
			return VAMPIRE_COAT5;//by viva 171
		case 30:
		case 31:
			return VAMPIRE_COAT6;//by viva 181
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
			return VAMPIRE_COAT7;//by viva 181
		case 40:
		case 41:
		case 42:
		case 43:
			return VAMPIRE_COAT8;
		case 18:
		case 19:
		default:
//			throw ("getVampireCoatType() : unknown VampireCoat type");
			return VAMPIRE_COAT4;
			break;
	}

	return VAMPIRE_COAT_BASIC;
}
VampireArmType getVampireArmType(ItemType_t IType)
	throw()
{
	switch(IType)
	{
	case 28:
	case 29:
	case 30:
	case 31:
		return VAMPIRE_ARM_WEAPON4;
	case 32:
	case 33:
	case 34:
		return VAMPIRE_ARM_WEAPON5;
	case 26:
		return VAMPIRE_ARM_WEAPON2;
	case 27:
		return VAMPIRE_ARM_WEAPON3;
	default:
		return VAMPIRE_ARM_WEAPON;
		break;
	}
	return VAMPIRE_ARM_WEAPON;
}

bool isAirMotorcycleItemType(ItemType_t IType)
	throw()
{
	return IType == 9 || IType == 11;
}

MotorcycleType getMotorcycleType(ItemType_t IType)
	throw()
{
	switch (IType)
	{
		case 11 :
                case 10 :
		case 9 :
		case 8 :
			return MOTORCYCLE_WING;
		case 7:
			return MOTORCYCLE3;
		case 6:
			return MOTORCYCLE2;
		case 0:
		case 5:
		default:
			return MOTORCYCLE1;
			break;
	}

	return MOTORCYCLE_NONE;
}

OustersCoatType getOustersCoatType(ItemType_t IType)
	throw()
{
	switch ( IType )
	{
		case 0:
		case 1:
		case 2:
		case 15:	//??????? ????????? ??? ????
			return OUSTERS_COAT1;
		case 3:
		case 4:
		case 5:
			return OUSTERS_COAT2;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 14: 	//??????? ???? 
			return OUSTERS_COAT3;
		case 12: //151
		case 13: //161
		case 16: //171
			return OUSTERS_COAT5;	//by viva 171
		case 17:
			return OUSTERS_COAT6;	//by viva 181
		case 18:
		case 19:
		case 20:
		case 21:
			return OUSTERS_COAT7;	//by RALLSER 201
		case 22:
		case 23:
			return OUSTERS_COAT8;
		case 11:
		default:
			return OUSTERS_COAT4;
	}

	return OUSTERS_COAT_BASIC;
}

OustersArmType getOustersArmType(ItemType_t IType)
	throw()
{
	switch ( IType )
	{
	case 21:
		return OUSTERS_ARM_CHAKRAM_171;
	case 22:
		return OUSTERS_ARM_CHAKRAM_181;
	case 23:
	case 24:
	case 25:
	case 26:
		return OUSTERS_ARM_CHAKRAM_201;
	case 27:
	case 28:
	case 29:
		return OUSTERS_ARM_CHAKRAM_231;
	default:
		return OUSTERS_ARM_CHAKRAM;
	}

	return OUSTERS_ARM_CHAKRAM;
}

