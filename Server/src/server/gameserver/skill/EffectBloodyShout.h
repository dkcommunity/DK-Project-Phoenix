//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyShout.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BLOODY_SHOUT__
#define __EFFECT_BLOODY_SHOUT__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBloodyShout
//////////////////////////////////////////////////////////////////////////////

class EffectBloodyShout : public Effect 
{
public:
	EffectBloodyShout(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BLOODY_SHOUT; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setHPBonus(HP_t hpbonus) { m_HPBonus = hpbonus; }
	HP_t getHPBonus() { return m_HPBonus; }
private:
	HP_t m_HPBonus;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectBloodyShoutLoader
//////////////////////////////////////////////////////////////////////////////

class EffectBloodyShoutLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_BLOODY_SHOUT; }
	virtual string getEffectClassName() const throw() { return "EffectBloodyShout"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_BLOODY_SHOUT__
