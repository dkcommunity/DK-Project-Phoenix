//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectALLYSTERWIND.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_ADV2THERAGEOFGUARDIANSPRIT__
#define __EFFECT_ADV2THERAGEOFGUARDIANSPRIT__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2THERAGEOFGUARDIANSPRIT
//////////////////////////////////////////////////////////////////////////////

class EffectADV2THERAGEOFGUARDIANSPRIT : public Effect 
{
public:
	EffectADV2THERAGEOFGUARDIANSPRIT(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setHPBonus(HP_t HPbonus) { m_HPBonus = HPbonus; }
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

#endif // __EFFECT_ADV2THERAGEOFGUARDIANSPRIT__
