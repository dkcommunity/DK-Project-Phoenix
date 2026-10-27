//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPromotionButterflyWing.h
// Written by  : svi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PROMOTION_BUTTERFLY_WING__
#define __EFFECT_PROMOTION_BUTTERFLY_WING__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPromotionButterflyWing
//////////////////////////////////////////////////////////////////////////////

class EffectPromotionButterflyWing : public Effect
{
public:
	EffectPromotionButterflyWing(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PROMOTION_BUTTERFLY_WING; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create( const string& ownerID ) throw(Error);
	virtual void destroy( const string& ownerID ) throw(Error);
	virtual void save( const string& ownerID ) throw(Error);
	
	bool isPossibleAffect () throw(Error); 
	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectPromotionButterflyWingLoader
//////////////////////////////////////////////////////////////////////////////

class EffectPromotionButterflyWingLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_PROMOTION_BUTTERFLY_WING; }
	virtual string getEffectClassName() const throw() { return "EffectPromotionButterflyWing"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectPromotionButterflyWingLoader* g_pEffectPromotionButterflyWingLoader;

#endif 
