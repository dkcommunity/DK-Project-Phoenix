//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyShout.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_ADV2TRUECLOLOROFVAMPLOAD__
#define __EFFECT_ADV2TRUECLOLOROFVAMPLOAD__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectADV2THEROAROFAWAKENWARRIOR
//////////////////////////////////////////////////////////////////////////////

class EffectADV2TRUECLOLOROFVAMPLOAD : public Effect 
{
public:
	EffectADV2TRUECLOLOROFVAMPLOAD(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setHPBonus(HP_t hpbonus) { m_HPBonus = hpbonus; }
	HP_t getHPBonus() { return m_HPBonus; }

	int getSTRBonus() const { return m_STRBonus; }
	void setSTRBonus( int STRBonus ) { m_STRBonus = STRBonus; }

	int getDEXBonus() const { return m_DEXBonus; }
	void setDEXBonus( int DEXBonus ) { m_DEXBonus = DEXBonus; }

	int getINTBonus() const { return m_INTBonus; }
	void setINTBonus( int INTBonus ) { m_INTBonus = INTBonus; }
private:
	HP_t m_HPBonus;
	int m_STRBonus;
	int m_DEXBonus;
	int m_INTBonus;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectADV2THEROAROFAWAKENWARRIOR
//////////////////////////////////////////////////////////////////////////////

class EffectADV2TRUECLOLOROFVAMPLOADLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD; }
	virtual string getEffectClassName() const throw() { return "EffectADV2TRUECLOLOROFVAMPLOAD"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif // __EFFECT_ADV2TRUECLOLOROFVAMPLOAD
