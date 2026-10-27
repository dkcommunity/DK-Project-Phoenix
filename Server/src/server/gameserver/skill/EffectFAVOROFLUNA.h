//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFAVOROFLUNA.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FAVOROFLUNA__
#define __EFFECT_FAVOROFLUNA__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFAVOROFLUNA
//////////////////////////////////////////////////////////////////////////////

class EffectFAVOROFLUNA : public Effect 
{
public:
	EffectFAVOROFLUNA(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_FAVOROFLUNA; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
	void unaffect() throw(Error); 
	void unaffect(Item* pItem) throw(Error) {}

	string toString() const throw();

public:
	int getRESBonus() const { return m_RESBonus; }
	void setRESBonus( int resbonus ) { m_RESBonus = resbonus; }

	int getHPBonus() const { return m_HPBonus; }
	void setHPBonus( int hpbonus ) { m_HPBonus = hpbonus; }

	int getDefenseBonus() const { return m_DefenseBonus; }
	void setDefenseBonus( int defensebonus ) { m_DefenseBonus = defensebonus; }

private :
	int     m_RESBonus;
	int	m_HPBonus;
	int	m_DefenseBonus;
};

#endif // __EFFECT_FAVOROFLUNA__
