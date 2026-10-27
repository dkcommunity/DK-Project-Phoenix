//// kim sung yong 계급경험치 effect 20080310 ////

#ifndef __EFFECT_RANK_BONUS_EXP_RED__
#define __EFFECT_RANK_BONUS_EXP_RED__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectRankBonusExp
//////////////////////////////////////////////////////////////////////////////

class EffectRankBonusExpRed : public Effect 
{
public:
	EffectRankBonusExpRed(Creature* pCreature) throw(Error);

public:
	EffectClass getEffectClass() const throw() { return EFFECT_CLASS_RANK_BONUS_EXP_RED; }

	void affect() throw(Error) { }
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create(const string & name) throw(Error);
	virtual void destroy(const string & name) throw(Error);
	virtual void save(const string & name) throw(Error);

	string toString() const throw();
};

//////////////////////////////////////////////////////////////////////////////
// class EffectRankBonusExpLoader
//////////////////////////////////////////////////////////////////////////////

class EffectRankBonusExpRedLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_RANK_BONUS_EXP_RED; }
	virtual string getEffectClassName() const throw() { return "EffectRankBonusExpRed"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectRankBonusExpRedLoader* g_pEffectRankBonusExpRedLoader;

#endif // __EFFECT_RANK_BONUS_EXP_RED__
