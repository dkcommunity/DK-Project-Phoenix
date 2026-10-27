//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSatelliteBombPoint2.h
// Written by  : rallser
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SATELLITE_BOMB_POINT_2__
#define __EFFECT_SATELLITE_BOMB_POINT_2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSatelliteBombPoint2
//////////////////////////////////////////////////////////////////////////////

class EffectSatelliteBombPoint2 : public Effect 
{
public:
	EffectSatelliteBombPoint2(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SATELLITE_BOMB_POINT_2; }

	void affect() throw(Error){}

	void unaffect() throw(Error);

	string toString() const throw();

private:
};

#endif
