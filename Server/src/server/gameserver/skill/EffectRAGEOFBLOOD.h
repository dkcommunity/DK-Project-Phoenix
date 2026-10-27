//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectStoneSkin.hRAGEOFBLOOD
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_RAGE_OF_BLOOD__
#define __EFFECT_RAGE_OF_BLOOD__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectStoneSkin
//////////////////////////////////////////////////////////////////////////////

class EffectRAGEOFBLOOD : public Effect 
{
public:
	EffectRAGEOFBLOOD(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_RAGE_OF_BLOOD; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	int getBonus() throw() { return m_Bonus;}
	void setBonus(int Bonus) throw(Error) { m_Bonus = Bonus;}

private:
	int m_Bonus;
};

#endif // __EFFECT_STONE_SKIN__
