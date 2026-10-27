//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFuriousIncense.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FURIOUS_INCENSE__
#define __EFFECT_FURIOUS_INCENSE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFuriousIncense
//////////////////////////////////////////////////////////////////////////////

class EffectFuriousIncense : public Effect
{
public:
	EffectFuriousIncense(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_FURIOUS_INCENSE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

private:
};


//////////////////////////////////////////////////////////////////////////////
// class EffectFuriousIncenseLoader
//////////////////////////////////////////////////////////////////////////////

class EffectFuriousIncenseLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_FURIOUS_INCENSE; }
	virtual string getEffectClassName() const throw() { return "EffectFuriousIncense"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectFuriousIncenseLoader* g_pEffectFuriousIncenseLoader;

#endif 

