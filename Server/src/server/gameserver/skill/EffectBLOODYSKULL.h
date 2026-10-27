//////////////////////////////////////////////////////////////////////////////
// Filename    : BLOODYSKULL.h
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BLOODY_SKULL__
#define __EFFECT_BLOODY_SKULL__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBLOODYSKULL
//////////////////////////////////////////////////////////////////////////////

class EffectBLOODYSKULL : public Effect 
{
public:
	EffectBLOODYSKULL(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer = false) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BLOODY_SKULL; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	int getDamage(void) const { return m_Damage; }
	void setDamage(int damage) { m_Damage = damage; }

	Turn_t getDelay() const { return m_Delay; }
	void setDelay(Turn_t Delay) { m_Delay = Delay; }

//	string getCasterName(void) const { return m_CasterName; }
//	void setCasterName(const string & CasterName ) { m_CasterName = CasterName; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

	void setSplashRatio(int index, uint ratio) { m_SplashRatio[index] = ratio; }

private:
	int     m_Damage;
	Turn_t  m_Delay;
//	string  m_CasterName;
	ObjectID_t   m_UserObjectID;
	bool	m_bPlayer;			// À¯Àú°¡ »ç¿ëÇÑ ±âŒúÀÎ°¡?
	uint	m_SplashRatio[3];
};

//////////////////////////////////////////////////////////////////////////////
// class EffectBLOODYSKULLLoader
//////////////////////////////////////////////////////////////////////////////

class EffectBLOODYSKULLLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_BLOODY_SKULL; }
	virtual string getEffectClassName() const throw() { return "EffectBLOODYSKULL"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectBLOODYSKULLLoader* g_pEffectBLOODYSKULLLoader;

#endif // __EFFECT_BLOODY_SKULL__
