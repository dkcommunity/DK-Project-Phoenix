//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHyperactiveIncense.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HYPERACTIVE_INCENSE__
#define __EFFECT_HYPERACTIVE_INCENSE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHyperactiveIncense
//////////////////////////////////////////////////////////////////////////////

class EffectHyperactiveIncense : public Effect
{
public:
	EffectHyperactiveIncense(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HYPERACTIVE_INCENSE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

private:
};


//////////////////////////////////////////////////////////////////////////////
// class EffectHyperactiveIncenseLoader
//////////////////////////////////////////////////////////////////////////////

class EffectHyperactiveIncenseLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_HYPERACTIVE_INCENSE; }
	virtual string getEffectClassName() const throw() { return "EffectHyperactiveIncense"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectHyperactiveIncenseLoader* g_pEffectHyperactiveIncenseLoader;

#endif 

