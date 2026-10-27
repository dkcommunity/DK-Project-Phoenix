//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectCurseOfBlood.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_CURSE_OF_BLOOD__
#define __EFFECT_CURSE_OF_BLOOD__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectCurseOfBlood
//////////////////////////////////////////////////////////////////////////////

class EffectCurseOfBlood : public Effect 
{
public:
	EffectCurseOfBlood(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_CURSE_OF_BLOOD; }

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
// class EffectCurseOfBloodLoader
//////////////////////////////////////////////////////////////////////////////

class EffectCurseOfBloodLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_CURSE_OF_BLOOD; }
	virtual string getEffectClassName() const throw() { return "EffectCurseOfBlood"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_CURSE_OF_BLOOD__
