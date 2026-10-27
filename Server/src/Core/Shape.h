//////////////////////////////////////////////////////////////////////////////
// Filename    : Shape.h
// Written by  : excel96
// Description : 
// ?¥á???????? ????????? ????? ??????? ?? ????? ?????.
// ?????? ???? ???? ???? ????? ??????? ????????.
//////////////////////////////////////////////////////////////////////////////

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "Types.h"
#include "Exception.h"

BYTE getShoulderType(ItemType_t IType) throw();
PantsType getPantsType(ItemType_t IType) throw();
JacketType getJacketType(ItemType_t IType) throw();
HelmetType getHelmetType(ItemType_t IType) throw();
ShieldType getShieldType(ItemType_t IType) throw();
MotorcycleType getMotorcycleType(ItemType_t IType) throw();
// Hover / Heli Injector (type 9, 11) ? bat transform benzeri hava araci
bool isAirMotorcycleItemType(ItemType_t IType) throw();
WeaponType getSwordType(ItemType_t IType) throw();	//by viva
WeaponType getBladeType(ItemType_t IType) throw();
WeaponType getSRType(ItemType_t IType) throw();
WeaponType getARType(ItemType_t IType) throw();
WeaponType getCrossType(ItemType_t IType) throw();
WeaponType getMaceType(ItemType_t IType) throw();

// ???????
VampireCoatType getVampireCoatType(ItemType_t IType) throw();
VampireArmType	getVampireArmType(ItemType_t IType) throw();

// ??²J???
OustersCoatType getOustersCoatType(ItemType_t IType) throw();
OustersArmType getOustersArmType(ItemType_t IType) throw();

#endif
