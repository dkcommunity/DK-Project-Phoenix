//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMagicShield2.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MAGIC_SHIELD2__
#define __EFFECT_MAGIC_SHIELD2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMagicShield2
//////////////////////////////////////////////////////////////////////////////

class EffectMagicShield2 : public Effect 
{
public:
	EffectMagicShield2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MAGIC_SHIELD2; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectMagicShield2Loader
//////////////////////////////////////////////////////////////////////////////

class EffectMagicShield2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MAGIC_SHIELD2; }
	virtual string getEffectClassName() const throw() { return "EffectMagicShield2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_MAGIC_SHIELD2__
