//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMenegrothDoungeon.h
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MENEGROTH_DOUNGEON__
#define __EFFECT_MENEGROTH_DOUNGEON__

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMenegrothDoungeon
//////////////////////////////////////////////////////////////////////////////

class EffectMenegrothDoungeon : public Effect 
{
public:
	EffectMenegrothDoungeon(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MENEGROTH_DOUNGEON; }

	void affect(Creature* pCreature) throw(Error);
	void affect() throw(Error) {}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:

private:

};

//////////////////////////////////////////////////////////////////////////////
// class EffectMenegrothDoungeonLoader
//////////////////////////////////////////////////////////////////////////////

class EffectMenegrothDoungeonLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON; }
	virtual string getEffectClassName() const throw() { return "EffectMenegrothDoungeon"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif // __EFFECT_MENEGROTH_DOUNGEON__
