//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGREYFELLForceScroll.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_GREYFELL_FORCE_SCROLL__
#define __EFFECT_GREYFELL_FORCE_SCROLL__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectGREYFELLForceScroll
//////////////////////////////////////////////////////////////////////////////

class EffectGreyfellForceScroll : public Effect
{
public:
	EffectGreyfellForceScroll(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_GREYFELL_FORCE_SCROLL; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create( const string& ownerID ) throw(Error);
	virtual void destroy( const string& ownerID ) throw(Error);
	virtual void save( const string& ownerID ) throw(Error);

	bool isPossibleAffect () throw(Error); // 20081029 wlzzi - Extreme Force Scroll과 중첩사용 불가 처리	
	string toString() const throw();

private:
};

//////////////////////////////////////////////////////////////////////////////
// class EffectGREYFELLForceScrollLoader
//////////////////////////////////////////////////////////////////////////////

class EffectGreyfellForceScrollLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL; }
	virtual string getEffectClassName() const throw() { return "EffectGreyfellForceScroll"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectGreyfellForceScrollLoader* g_pEffectGreyfellForceScrollLoader;

#endif 
