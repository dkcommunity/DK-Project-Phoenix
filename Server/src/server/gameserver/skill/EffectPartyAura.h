//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAura.h
// Written by  : excel96 
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA__
#define __EFFECT_PARTY_AURA__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAura
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAura : public Effect 
{
public:
	EffectPartyAura(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	// get/set HP
	void setHP( bool bHP = true ) { m_bHP = bHP; }
	bool isHP() const { return m_bHP; }

	// get/set Defense
	void setDefense( bool bDefense = true ) { m_bDefense = bDefense; }
	bool isDefense() const { return m_bDefense; }

	// get/set ToHit
	void setToHit( bool bToHit = true ) { m_bToHit = bToHit; }
	bool isToHit() const { return m_bToHit; }

	// get/set Luck
	void setLuck( bool bLuck = true ) { m_bLuck = bLuck; }
	bool isLuck() const { return m_bLuck; }

	// get/set Damage
	void setDamage( bool bDamage = true ) { m_bDamage = bDamage; }
	bool isDamage() const { return m_bDamage; }

	// get/set Resist
	void setResist( bool bResist = true ) { m_bResist = bResist; }
	bool isResist() const { return m_bResist; }

	// get/set party size
	size_t getPartySize() const { return m_PartySize; }
	void setPartySize( size_t partySize ) { m_PartySize = partySize; }

private:
	bool	m_bHP;
	bool	m_bDefense;
	bool	m_bToHit;
	bool	m_bLuck;
	bool	m_bDamage;
	bool	m_bResist;
	size_t	m_PartySize;
};

#endif // __EFFECT_PARTY_AURA__

