//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEdgeArch.h
// Written by  : 
// Description : EdgeArch에 의한 방어력 하강 effect
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EDGE_ARCH__
#define __EFFECT_EDGE_ARCH__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectEdgeArch
//////////////////////////////////////////////////////////////////////////////

class EffectEdgeArch : public Effect 
{
public:
	EffectEdgeArch(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EDGE_ARCH; }

	void affect() throw(Error) ;
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	void setTick(Turn_t Tick) throw() { m_Tick = Tick; }
	Turn_t getTick() const throw() { return m_Tick; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }


private:
	Turn_t	m_Tick;
	ObjectID_t m_UserObjectID;
};

#endif // __EFFECT_EDGE_ARCH__
