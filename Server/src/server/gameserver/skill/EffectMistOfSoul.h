//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMistOfSoul.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MIST_OF_SOUL__
#define __EFFECT_MIST_OF_SOUL__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMistOfSoul
//////////////////////////////////////////////////////////////////////////////

class EffectMistOfSoul : public Effect 
{
public:
	EffectMistOfSoul(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MIST_OF_SOUL; }

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
// class EffectMistOfSoulLoader
//////////////////////////////////////////////////////////////////////////////

class EffectMistOfSoulLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MIST_OF_SOUL; }
	virtual string getEffectClassName() const throw() { return "EffectMistOfSoul"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_MIST_OF_SOUL__
