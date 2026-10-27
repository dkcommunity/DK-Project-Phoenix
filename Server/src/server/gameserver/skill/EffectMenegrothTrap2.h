//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMenegrothTrap2.h
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MENEGROTH_TRAP_2__
#define __EFFECT_MENEGROTH_TRAP_2__

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__ 

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMenegrothTrap2
//////////////////////////////////////////////////////////////////////////////

class EffectMenegrothTrap2 : public Effect 
{
public:
	EffectMenegrothTrap2(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MENEGROTH_TRAP_2; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
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
// class EffectMenegrothTrap2Loader
//////////////////////////////////////////////////////////////////////////////

class EffectMenegrothTrap2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MENEGROTH_TRAP_2; }
	virtual string getEffectClassName() const throw() { return "EffectMenegrothTrap2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif // __EFFECT_MENEGROTH_TRAP_2__
