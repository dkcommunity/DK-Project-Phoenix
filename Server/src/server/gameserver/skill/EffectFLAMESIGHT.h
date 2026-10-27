//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFLAMESIGHT.h
// Written by  : excel96
// Description : 
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FLAME_SIGHT__
#define __EFFECT_FLAME_SIGHT__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFLAMESIGHT
//////////////////////////////////////////////////////////////////////////////

class EffectFLAMESIGHT : public Effect 
{
public:
	EffectFLAMESIGHT(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_FLAME_SIGHT; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	//bool canSeeHide() const { return true; }
	//bool canSeeInvisibility() const { return m_Level >= 15; }
//	bool canSeeSniping() const { return m_Level >= 25; }

	string toString() const throw();

public:
	void setLevel( uint level ) { m_Level = level; }
	uint getLevel() const { return m_Level; }

private:
	uint	m_Level;
};
#endif // __EFFECT_FLAMESIGHT
