//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectIntimateGrail2.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_INTIMATE_GRAIL_2__
#define __EFFECT_INTIMATE_GRAIL_2__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectIntimateGrail2
//////////////////////////////////////////////////////////////////////////////

class EffectIntimateGrail2 : public Effect 
{
public:
	EffectIntimateGrail2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_INTIMATE_GRAIL_2; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
	void unaffect() throw(Error); 
	void unaffect(Item* pItem) throw(Error) {}

	string toString() const throw();

public:
	int getSkillLevel(void) const { return m_SkillLevel; }
	void setSkillLevel(int level) { m_SkillLevel = level; }

	int getHPBonus() const { return m_HPBonus; }
	void setHPBonus( int hpbonus ) { m_HPBonus = hpbonus; }

	int getDefenseBonus() const { return m_DefenseBonus; }
	void setDefenseBonus( int defensebonus ) { m_DefenseBonus = defensebonus; }

private :
	int m_SkillLevel;
	int	m_HPBonus;
	int	m_DefenseBonus;
};

#endif // __EFFECT_INTIMATE_GRAIL_2__
