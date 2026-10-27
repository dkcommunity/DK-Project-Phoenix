//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectIntimateGrail.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_INTIMATE_GRAIL__
#define __EFFECT_INTIMATE_GRAIL__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectIntimateGrail
//////////////////////////////////////////////////////////////////////////////

class EffectIntimateGrail : public Effect 
{
public:
	EffectIntimateGrail(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_INTIMATE_GRAIL; }

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

#endif // __EFFECT_INTIMATE_GRAIL__
