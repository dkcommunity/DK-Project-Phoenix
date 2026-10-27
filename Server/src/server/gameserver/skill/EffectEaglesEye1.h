//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEaglesEye1.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EAGLES_EYE1__
#define __EFFECT_EAGLES_EYE1__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectEaglesEye1
//////////////////////////////////////////////////////////////////////////////

class EffectEaglesEye1 : public Effect 
{
public:
	EffectEaglesEye1(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EAGLES_EYE1; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setLevel( SkillLevel_t level ) { m_SkillLevel = level; }
	SkillLevel_t getLevel() const { return m_SkillLevel; }

private:
	SkillLevel_t	m_SkillLevel;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectEaglesEye1Loader
//////////////////////////////////////////////////////////////////////////////

class EffectEaglesEye1Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_EAGLES_EYE1; }
	virtual string getEffectClassName() const throw() { return "EffectEaglesEye1"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_EAGLES_EYE1__
