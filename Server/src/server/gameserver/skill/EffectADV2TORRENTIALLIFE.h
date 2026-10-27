//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectADV2TORRENTIALLIFE.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_ADV2TORRENTIALLIFE__
#define __EFFECT_ADV2TORRENTIALLIFE__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectADV2TORRENTIALLIFE
//////////////////////////////////////////////////////////////////////////////

class EffectADV2TORRENTIALLIFE : public Effect 
{
public:
	EffectADV2TORRENTIALLIFE(Creature* pCreature) throw(Error);

public:
	EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2TORRENTIALLIFE; }

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

	int getBonus() const { return m_Bonus; }
	void setBonus(int bonus) { m_Bonus = bonus; }

private :
	int m_Bonus;
};

#endif // __EFFECT_ADV2TORRENTIALLIFE__
