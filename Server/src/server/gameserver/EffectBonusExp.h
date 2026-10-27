//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBonusExp.h
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BONUS_EXP__
#define __EFFECT_BONUS_EXP__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBonusExp
//////////////////////////////////////////////////////////////////////////////

class EffectBonusExp : public Effect 
{
public:
	EffectBonusExp(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BONUS_EXP; }

	void affect() throw(Error) { }
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create(const string & name) throw(Error);
	virtual void destroy(const string & name) throw(Error);
	virtual void save(const string & name) throw(Error);

	string toString() const throw();
	
	void SetBonusRate(float fRate) { m_fBonusRate = fRate; }
	float GetBonusRate() { return m_fBonusRate; }
	
protected :
	float m_fBonusRate;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectBonusExpLoader
//////////////////////////////////////////////////////////////////////////////

class EffectBonusExpLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_BONUS_EXP; }
	virtual string getEffectClassName() const throw() { return "EffectBonusExp"; }

public:
	virtual void load(Creature* pCreature) throw(Error);

};

extern EffectBonusExpLoader* g_pEffectBonusExpLoader;

#endif // __EFFECT_BONUS_EXP__
