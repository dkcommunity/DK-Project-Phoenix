//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectWarpMonster.h
// Written by  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_WARP_MONSTER_H__
#define __EFFECT_WARP_MONSTER_H__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectWarpMonster;
// 일정 시간이 지나서 바닥의 아이템은 사라지고
// 다른 존으로 옮겨지게 된다.
//////////////////////////////////////////////////////////////////////////////

class EffectWarpMonster : public Effect 
{
public:
	EffectWarpMonster(Zone* pZone, ObjectID_t monsterObjectID, ZoneCoord_t x, ZoneCoord_t y, Dir_t dir) throw(Error);
	virtual ~EffectWarpMonster() throw();

public:
	virtual EffectClass getEffectClass() const throw() { return EFFECT_CLASS_WARP_MONSTER; }

	virtual void affect() throw(Error);
	virtual void unaffect() throw(Error);

	// get debug string
	virtual string toString() const throw();

private:
	ObjectID_t	m_MonsterObjectID;
	ZoneCoord_t m_X; 
	ZoneCoord_t m_Y;
	Dir_t		m_Direction;
};

#endif
