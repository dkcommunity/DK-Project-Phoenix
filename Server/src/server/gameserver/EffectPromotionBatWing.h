//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPromotionBatWing.h
// Written by  : svi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PROMOTION_BAT_WING__
#define __EFFECT_PROMOTION_BAT_WING__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPromotionBatWing
//////////////////////////////////////////////////////////////////////////////

class EffectPromotionBatWing : public Effect
{
public:
	EffectPromotionBatWing(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PROMOTION_BAT_WING; }

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
// class EffectPromotionBatWingLoader
//////////////////////////////////////////////////////////////////////////////

class EffectPromotionBatWingLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_PROMOTION_BAT_WING; }
	virtual string getEffectClassName() const throw() { return "EffectPromotionBatWing"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectPromotionBatWingLoader* g_pEffectPromotionBatWingLoader;

#endif 
