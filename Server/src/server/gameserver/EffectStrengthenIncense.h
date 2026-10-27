//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectStrengthenIncense.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_STRENGTHEN_INCENSE__
#define __EFFECT_STRENGTHEN_INCENSE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectStrengthenIncense
//////////////////////////////////////////////////////////////////////////////

class EffectStrengthenIncense : public Effect
{
public:
	EffectStrengthenIncense(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_STRENGTHEN_INCENSE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

private:
};


//////////////////////////////////////////////////////////////////////////////
// class EffectStrengthenIncenseLoader
//////////////////////////////////////////////////////////////////////////////

class EffectStrengthenIncenseLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_STRENGTHEN_INCENSE; }
	virtual string getEffectClassName() const throw() { return "EffectStrengthenIncense"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectStrengthenIncenseLoader* g_pEffectStrengthenIncenseLoader;

#endif 

