//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectCabracamAltar.h
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_CABRACAM_ALTAR__
#define __EFFECT_CABRACAM_ALTAR__

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectCabracamAltar
//////////////////////////////////////////////////////////////////////////////

class EffectCabracamAltar : public Effect 
{
public:
	EffectCabracamAltar(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR; }

	void affect(Creature* pCreature) throw(Error);
	void affect() throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:

private:

};

//////////////////////////////////////////////////////////////////////////////
// class EffectCabracamAltarLoader
//////////////////////////////////////////////////////////////////////////////

class EffectCabracamAltarLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR; }
	virtual string getEffectClassName() const throw() { return "EffectCabracamAltar"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif // __EFFECT_CABRACAM_ALTAR__
