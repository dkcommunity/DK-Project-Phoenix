//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPromotionAngelFlutter.h
// Written by  : svi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PROMOTION_ANGEL_FLUTTER__
#define __EFFECT_PROMOTION_ANGEL_FLUTTER__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPromotionAngelFlutter
//////////////////////////////////////////////////////////////////////////////

class EffectPromotionAngelFlutter : public Effect
{
public:
	EffectPromotionAngelFlutter(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PROMOTION_ANGEL_FLUTTER; }

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
// class EffectPromotionAngelFlutterLoader
//////////////////////////////////////////////////////////////////////////////

class EffectPromotionAngelFlutterLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_PROMOTION_ANGEL_FLUTTER; }
	virtual string getEffectClassName() const throw() { return "EffectPromotionAngelFlutter"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectPromotionAngelFlutterLoader* g_pEffectPromotionAngelFlutterLoader;

#endif 
