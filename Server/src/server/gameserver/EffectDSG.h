//// kim sung yong DSG effect 20080226 ////

#ifndef __EFFECT_DSG__
#define __EFFECT_DSG__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectDSG
//////////////////////////////////////////////////////////////////////////////

class EffectDSG : public Effect 
{
public:
	EffectDSG(Creature* pCreature) throw(Error);

public:
	virtual EffectClass getEffectClass() const throw() { return EFFECT_CLASS_DSG; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectDSGLoader
//////////////////////////////////////////////////////////////////////////////

class EffectDSGLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_DSG; }
	virtual string getEffectClassName() const throw() { return "EffectDSG"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectDSGLoader* g_pEffectDSGLoader;

#endif // __EFFECT_DSG__

