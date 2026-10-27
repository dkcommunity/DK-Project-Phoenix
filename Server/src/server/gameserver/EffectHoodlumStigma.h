//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHoodlumStigma.h
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HOODLUM_STIGMA__
#define __EFFECT_HOODLUM_STIGMA__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHoodlumStigma
//////////////////////////////////////////////////////////////////////////////

class EffectHoodlumStigma : public Effect 
{
public:
	EffectHoodlumStigma(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HOODLUM_STIGMA; }

	void affect() throw(Error) { }
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create(const string & ownerID) throw(Error);
	virtual void destroy(const string & ownerID) throw(Error);
	virtual void save(const string & ownerID) throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectHoodlumStigmaLoader
//////////////////////////////////////////////////////////////////////////////

class EffectHoodlumStigmaLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_HOODLUM_STIGMA; }
	virtual string getEffectClassName() const throw() { return "EffectHoodlumStigma"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectHoodlumStigmaLoader* g_pEffectHoodlumStigmaLoader;

#endif // __EFFECT_HOODLUM_STIGMA__
