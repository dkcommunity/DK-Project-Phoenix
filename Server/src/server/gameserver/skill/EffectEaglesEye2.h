//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEaglesEye2.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EAGLES_EYE2__
#define __EFFECT_EAGLES_EYE2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectEaglesEye2
//////////////////////////////////////////////////////////////////////////////

class EffectEaglesEye2 : public Effect 
{
public:
	EffectEaglesEye2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EAGLES_EYE2; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectEaglesEye2Loader
//////////////////////////////////////////////////////////////////////////////

class EffectEaglesEye2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_EAGLES_EYE2; }
	virtual string getEffectClassName() const throw() { return "EffectEaglesEye2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_EAGLES_EYE2__
