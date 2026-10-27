//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMephisto.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_RAGE_OF_BLOOD__
#define __EFFECT_RAGE_OF_BLOOD__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMephisto
//////////////////////////////////////////////////////////////////////////////

class EffectRAGEOFBLOOD : public Effect 
{
public:
	EffectRAGEOFBLOOD(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_RAGE_OF_BLOOD; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	//int getToHitBonus() const { return m_ToHitBonus; }
	//void setToHitBonus( int toHitBonus ) { m_ToHitBonus = toHitBonus; }

	int getDefenseBonus() const { return m_DefenseBonus; }
	void setDefenseBonus( int defenseBonus ) { m_DefenseBonus = defenseBonus; }

	int getBonus() throw() { return m_Bonus;}
	void setBonus(int Bonus) throw(Error) { m_Bonus = Bonus;}

private:
	//int m_ToHitBonus;
	int m_DefenseBonus;
	int m_Bonus;
};

#endif // __EFFECT_MEPHISTO__
