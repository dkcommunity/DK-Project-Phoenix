//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectWOLFROAR.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_WOLFROAR__
#define __EFFECT_WOLFROAR__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectWOLFROAR
//////////////////////////////////////////////////////////////////////////////

class EffectWOLFROAR : public Effect 
{
public:
	EffectWOLFROAR(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_WOLFROAR; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	//void setHPBonus(HP_t hpbonus) { m_HPBonus = hpbonus; }
	//HP_t getHPBonus() { return m_HPBonus; }

	int getSTRBonus() const { return m_STRBonus; }
	void setSTRBonus( int STRBonus ) { m_STRBonus = STRBonus; }

	int getDEXBonus() const { return m_DEXBonus; }
	void setDEXBonus( int DEXBonus ) { m_DEXBonus = DEXBonus; }

	int getINTBonus() const { return m_INTBonus; }
	void setINTBonus( int INTBonus ) { m_INTBonus = INTBonus; }
private:
	//HP_t m_HPBonus;
	int m_STRBonus;
	int m_DEXBonus;
	int m_INTBonus;
};

#endif // __EFFECT_WOLFROAR
