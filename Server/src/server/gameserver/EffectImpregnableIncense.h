//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectImpregnableIncense.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_IMPREGNABLE_INCENSE__
#define __EFFECT_IMPREGNABLE_INCENSE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectImpregnableIncense
//////////////////////////////////////////////////////////////////////////////

class EffectImpregnableIncense : public Effect
{
public:
	EffectImpregnableIncense(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_IMPREGNABLE_INCENSE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

private:
};


//////////////////////////////////////////////////////////////////////////////
// class EffectImpregnableIncenseLoader
//////////////////////////////////////////////////////////////////////////////

class EffectImpregnableIncenseLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_IMPREGNABLE_INCENSE; }
	virtual string getEffectClassName() const throw() { return "EffectImpregnableIncense"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectImpregnableIncenseLoader* g_pEffectImpregnableIncenseLoader;

#endif 

