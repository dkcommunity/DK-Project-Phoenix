//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGLORYGROUND.h
// Written by  : rallser
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_GLORYGROUND__
#define __EFFECT_GLORYGROUND__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectGLORYGROUND
//////////////////////////////////////////////////////////////////////////////

class EffectGLORYGROUND : public Effect 
{
public:
	EffectGLORYGROUND(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_GLORYGROUND; }

	void affect() throw(Error);
	void affect(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void unaffect() throw(Error);

	string toString() const throw();

public:

	Damage_t getDamage() const { return m_Damage; }
	void setDamage( Damage_t damage ) { m_Damage = damage; }
	
	void setDelay(Turn_t delay) { m_Delay = delay; }
	Turn_t getDelay() const { return m_Delay; }

private:

	Turn_t m_Delay;
	Damage_t m_Damage;
};

#endif
