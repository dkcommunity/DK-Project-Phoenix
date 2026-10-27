#ifndef __EFFECT_GIVE_POINT__
#define __EFFECT_GIVE_POINT__

#include "Effect.h"
#include "EffectLoader.h"

class EffectGivePoint : public Effect 
{
public:
	EffectGivePoint(Creature* pCreature) throw(Error);

public:
	virtual EffectClass getEffectClass() const throw() { return EFFECT_GIVE_POINT; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();
};

class EffectGivePointLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_GIVE_POINT; }
	virtual string getEffectClassName() const throw() { return "EffectGivePoint"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectGivePointLoader* g_pEffectGivePointLoader;

#endif

