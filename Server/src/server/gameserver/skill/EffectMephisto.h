//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMephisto.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MEPHISTO__
#define __EFFECT_MEPHISTO__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMephisto
//////////////////////////////////////////////////////////////////////////////

class EffectMephisto : public Effect 
{
public:
	EffectMephisto(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MEPHISTO; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	int getToHitBonus() const { return m_ToHitBonus; }
	void setToHitBonus( int toHitBonus ) { m_ToHitBonus = toHitBonus; }

	int getDefenseBonus() const { return m_DefenseBonus; }
	void setDefenseBonus( int defenseBonus ) { m_DefenseBonus = defenseBonus; }

	int getProtectionBonus() const { return m_ProtectionBonus; }
	void setProtectionBonus( int protectionBonus ) { m_ProtectionBonus = protectionBonus; }

private:
	int m_ToHitBonus;
	int m_DefenseBonus;
	int m_ProtectionBonus;
};

#endif // __EFFECT_MEPHISTO__
