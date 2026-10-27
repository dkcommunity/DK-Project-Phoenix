//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMagicShield1.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MAGIC_SHIELD1__
#define __EFFECT_MAGIC_SHIELD1__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMagicShield1
//////////////////////////////////////////////////////////////////////////////

class EffectMagicShield1 : public Effect 
{
public:
	EffectMagicShield1(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MAGIC_SHIELD1; }

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
// class EffectMagicShield1Loader
//////////////////////////////////////////////////////////////////////////////

class EffectMagicShield1Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MAGIC_SHIELD1; }
	virtual string getEffectClassName() const throw() { return "EffectMagicShield1"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_MAGIC_SHIELD1__
