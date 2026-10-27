#ifndef __EFFECT_ADV2ADVENTBLADE__
#define __EFFECT_ADV2ADVENTBLADE__

#include "Effect.h"
#include "EffectLoader.h"

class EffectADV2ADVENTBLADE : public Effect 
{
public:
	EffectADV2ADVENTBLADE(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2ADVENTBLADE; }

	void affect() throw(Error){}

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	void setReflection( int ref ) { m_Reflection = ref; }
	int getReflection() const { return m_Reflection; }

private:
	int m_Reflection;
};


class EffectADV2ADVENTBLADELoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_ADV2ADVENTBLADE; }
	virtual string getEffectClassName() const throw() { return "EffectADV2ADVENTBLADE"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif
