//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectExtremeForceScroll.h
// Written by  : wlzzi - 2008.10.28
// Description : 프리미엄 개편 관련 신규 아이템
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EXTREME_FORCE_SCROLL__
#define __EFFECT_EXTREME_FORCE_SCROLL__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectExtremeForceScroll
//////////////////////////////////////////////////////////////////////////////

class EffectExtremeForceScroll : public Effect
{
public:
	EffectExtremeForceScroll(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EXTREME_FORCE_SCROLL; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create( const string& ownerID ) throw(Error);
	virtual void destroy( const string& ownerID ) throw(Error);
	virtual void save( const string& ownerID ) throw(Error);

	bool isPossibleAffect () throw(Error);
	string toString() const throw();

private:
};

//////////////////////////////////////////////////////////////////////////////
// class EffectExtremeForceScrollLoader
//////////////////////////////////////////////////////////////////////////////

class EffectExtremeForceScrollLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_EXTREME_FORCE_SCROLL; }
	virtual string getEffectClassName() const throw() { return "EffectExtremeForceScroll"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectExtremeForceScrollLoader* g_pEffectExtremeForceScrollLoader;

#endif 
