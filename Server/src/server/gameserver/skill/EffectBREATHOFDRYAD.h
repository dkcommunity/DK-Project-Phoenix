//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyShout.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BREATHOFDRYAD__
#define __EFFECT_BREATHOFDRYAD__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBREATHOFDRYAD
//////////////////////////////////////////////////////////////////////////////

class EffectBREATHOFDRYAD : public Effect 
{
public:
	EffectBREATHOFDRYAD(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BREATHOFDRYAD; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setBonus( uint bonus ) { m_Bonus = bonus; }
	uint getBonus() const { return m_Bonus; }

private:
	uint	m_Bonus;
};


#endif // __EFFECT_BREATHOFDRYAD__
