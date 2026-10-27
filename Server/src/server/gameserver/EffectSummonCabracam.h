//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSummonCabracam.h
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SUMMON_CABRACAM__
#define __EFFECT_SUMMON_CABRACAM__

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSummonCabracam
//////////////////////////////////////////////////////////////////////////////

class EffectSummonCabracam : public Effect 
{
public:
	EffectSummonCabracam(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MENEGROTH_SUMMON_CABRACAM; }

	void affect(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);
	void affect() throw(Error);

	void unaffect(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:

private:

	bool m_bAlreadyAffect;
	Timeval m_FirstAffectTime;

};

//////////////////////////////////////////////////////////////////////////////
// class EffectSummonCabracamLoader
//////////////////////////////////////////////////////////////////////////////

class EffectSummonCabracamLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MENEGROTH_SUMMON_CABRACAM; }
	virtual string getEffectClassName() const throw() { return "EffectSummonCabracam"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif // __EFFECT_SUMMON_CABRACAM__
