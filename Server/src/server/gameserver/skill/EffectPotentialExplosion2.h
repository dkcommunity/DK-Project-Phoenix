//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPotentialExplosion.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_POTENTIAL_EXPLOSION2__
#define __EFFECT_POTENTIAL_EXPLOSION2__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPotentialExplosion2
//////////////////////////////////////////////////////////////////////////////

class EffectPotentialExplosion2 : public Effect 
{
public:
	EffectPotentialExplosion2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_POTENTIAL_EXPLOSION2; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
	void unaffect() throw(Error); 
	void unaffect(Item* pItem) throw(Error) {}

	string toString() const throw();

public:
	int getDiffSTR(void) throw() { return m_diffSTR;}
	int getDiffDEX(void) throw() { return m_diffDEX;}
	void setDiffSTR(int diffSTR) throw(Error) { m_diffSTR = diffSTR;}
	void setDiffDEX(int diffDEX) throw(Error) { m_diffDEX = diffDEX;}

private :
	int m_diffSTR;
	int m_diffDEX;
};

#endif // __EFFECT_POTENTIAL_EXPLOSION2__
