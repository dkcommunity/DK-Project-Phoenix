//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMysteryPotion.h
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MYSTERY_POTION__
#define __EFFECT_MYSTERY_POTION__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMysteryPotion
//////////////////////////////////////////////////////////////////////////////

class EffectMysteryPotion : public Effect 
{
public:
	EffectMysteryPotion(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MYSTERY_POTION; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create(const string & name) throw(Error);
	virtual void destroy(const string & name) throw(Error);
	virtual void save(const string & name) throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectMysteryPotionLoader
//////////////////////////////////////////////////////////////////////////////

class EffectMysteryPotionLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MYSTERY_POTION; }
	virtual string getEffectClassName() const throw() { return "EffectMysteryPotion"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectMysteryPotionLoader* g_pEffectMysteryPotionLoader;

#endif // __EFFECT_MYSTERY_POTION__
