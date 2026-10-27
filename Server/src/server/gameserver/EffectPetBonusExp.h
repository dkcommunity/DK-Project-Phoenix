//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPetBonusExp.h
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PET_BONUS_EXP__
#define __EFFECT_PET_BONUS_EXP__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPetBonusExp
//////////////////////////////////////////////////////////////////////////////

class EffectPetBonusExp : public Effect 
{
public:
	EffectPetBonusExp(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PET_BONUS_EXP; }

	void affect() throw(Error) { }
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create(const string & name) throw(Error);
	virtual void destroy(const string & name) throw(Error);
	virtual void save(const string & name) throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectPetBonusExpLoader
//////////////////////////////////////////////////////////////////////////////

class EffectPetBonusExpLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_PET_BONUS_EXP; }
	virtual string getEffectClassName() const throw() { return "EffectPetBonusExp"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectPetBonusExpLoader* g_pEffectPetBonusExpLoader;

#endif // __EFFECT_BONUS_EXP__
