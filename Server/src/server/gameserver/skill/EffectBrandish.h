//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBrandish.h
// Written by  : elca@ewestsoft.com
// Description : 성직마법 Brandish의 Effect를 처리해주기 위한 클래스이다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BRANDISH__
#define __EFFECT_BRANDISH__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBrandish
//////////////////////////////////////////////////////////////////////////////

class EffectBrandish : public Effect 
{
public:
	EffectBrandish(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BRANDISH; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	Turn_t getDelay() const { return m_Delay; }
	void setDelay(Turn_t Delay) { m_Delay = Delay; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

private:
	Turn_t  m_Delay;
	ObjectID_t   m_UserObjectID;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectBrandishLoader
//////////////////////////////////////////////////////////////////////////////

class EffectBrandishLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_BRANDISH; }
	virtual string getEffectClassName() const throw() { return "EffectBrandish"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectBrandishLoader* g_pEffectBrandishLoader;

#endif // __EFFECT_BRANDISH__
