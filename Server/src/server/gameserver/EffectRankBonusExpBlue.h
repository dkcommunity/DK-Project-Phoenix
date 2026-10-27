//// kim sung yong 계급경험치 effect 20080310 ////

#ifndef __EFFECT_RANK_BONUS_EXP_BLUE__
#define __EFFECT_RANK_BONUS_EXP_BLUE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectRankBonusExpBlue
//////////////////////////////////////////////////////////////////////////////

class EffectRankBonusExpBlue : public Effect 
{
public:
	EffectRankBonusExpBlue(Creature* pCreature) throw(Error);

public:
	EffectClass getEffectClass() const throw() { return EFFECT_CLASS_RANK_BONUS_EXP_BLUE; }

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
// class EffectRankBonusExpBlueLoader
//////////////////////////////////////////////////////////////////////////////

class EffectRankBonusExpBlueLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_RANK_BONUS_EXP_BLUE; }
	virtual string getEffectClassName() const throw() { return "EffectRankBonusExpBlue"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectRankBonusExpBlueLoader* g_pEffectRankBonusExpBlueLoader;

#endif // __EFFECT_RANK_BONUS_EXP_BLUE__
