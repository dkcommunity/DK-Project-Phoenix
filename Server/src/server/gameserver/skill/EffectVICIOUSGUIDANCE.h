//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectVICIOUSGUIDANCE.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_VICIOUS_GUIDANCE__
#define __EFFECT_VICIOUS_GUIDANCE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectVICIOUSGUIDANCE
//////////////////////////////////////////////////////////////////////////////

class EffectVICIOUSGUIDANCE : public Effect 
{
public:
	EffectVICIOUSGUIDANCE(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_VICIOUS_GUIDANCE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);
	string toString() const throw();

public:
	HP_t getPoint() const throw() { return m_Point; }
	void setPoint(HP_t Point) throw() { m_Point = Point; }

	Turn_t getTick() const throw() { return m_Tick; }
	void setTick(Turn_t Tick) throw() { m_Tick = Tick; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

private:
	HP_t    m_Point;
	Turn_t	m_Tick;
	ObjectID_t m_UserObjectID;
};

#endif // __EFFECT_VICIOUSGUIDANCE__
