//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMenegrothTrap1.h
// Written by  : rappi76 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MENEGROTH_TRAP_1__
#define __EFFECT_MENEGROTH_TRAP_1__

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__ 

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMenegrothTrap1
//////////////////////////////////////////////////////////////////////////////

class EffectMenegrothTrap1 : public Effect 
{
public:
	EffectMenegrothTrap1(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MENEGROTH_TRAP_1; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw(Error);
	void unaffect(Item* pItem) throw(Error) {}
	void unaffect() throw(Error);

	void setDamage(int iMinDamage, int iMaxDamage);

	string toString() const throw();

public:

private:
	int m_iMinDamage;
	int m_iMaxDamage;

	void affectCreature(Creature *pCreature);

};

//////////////////////////////////////////////////////////////////////////////
// class EffectMenegrothTrap1Loader
//////////////////////////////////////////////////////////////////////////////

class EffectMenegrothTrap1Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MENEGROTH_TRAP_1; }
	virtual string getEffectClassName() const throw() { return "EffectMenegrothTrap1"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif // __EFFECT_MENEGROTH_TRAP_1__
