//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectExplosionRocketLauncher.h
// Written by  : bigheart	 
// Description : 20080407
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EXPLOSION_ROCKET_LAUNCHER__
#define __EFFECT_EXPLOSION_ROCKET_LAUNCHER__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectExplosionRocketLauncher
//////////////////////////////////////////////////////////////////////////////

class EffectExplosionRocketLauncher : public Effect 
{
public:
	EffectExplosionRocketLauncher(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EXPLOSION_ROCKET_LAUNCHER; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	string toString() const throw();

public:
	HP_t getPoint() const throw() { return m_Point; }
	void setPoint(HP_t Point) throw() { m_Point = Point; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

	Level_t getSkillLevel() const { return m_SkillLevel; }
	void setSkillLevel( Level_t skillLevel ) { m_SkillLevel = skillLevel; }

private:
	HP_t    m_Point;
	ObjectID_t m_UserObjectID;
	Level_t	m_SkillLevel;
};

#endif // __EFFECT_EXPLOSION_ROCKET_LAUNCHER__
