//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectChaoticExp.h
// Written by  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_CHAOTIC_EXP__
#define __EFFECT_CHAOTIC_EXP__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectChaoticExp
//////////////////////////////////////////////////////////////////////////////

class EffectChaoticExp : public Effect
{
public:
	EffectChaoticExp(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_CHAOTIC_STONE_EXP; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error); 
	void unaffect(Creature* pCreature) throw(Error);

	virtual void create( const string& ownerID ) throw(Error);
	virtual void destroy( const string& ownerID ) throw(Error);
	virtual void save( const string& ownerID ) throw(Error);
	
	void SetBonusRate(float fRate) { m_fBonusRate = fRate; }
	float GetBonusRate() { return m_fBonusRate; }

	bool isPossibleAffect () throw(Error); 
	string toString() const throw();

protected :
	float m_fBonusRate;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectChaoticExpLoader
//////////////////////////////////////////////////////////////////////////////

class EffectChaoticExpLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_CHAOTIC_STONE_EXP; }
	virtual string getEffectClassName() const throw() { return "EffectChaoticExp"; }

public:
	virtual void load( Creature* pCreature ) throw(Error);
};

extern EffectChaoticExpLoader* g_pEffectChaoticExpLoader;

#endif 

