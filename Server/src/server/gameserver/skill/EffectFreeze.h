//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFreeze.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FREEZE__
#define __EFFECT_FREEZE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFreeze
//////////////////////////////////////////////////////////////////////////////

class EffectFreeze : public Effect 
{
public:
	EffectFreeze(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_FREEZE; }

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
// class EffectFreezeLoader
//////////////////////////////////////////////////////////////////////////////

class EffectFreezeLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_FREEZE; }
	virtual string getEffectClassName() const throw() { return "EffectFreeze"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_FREEZE__
