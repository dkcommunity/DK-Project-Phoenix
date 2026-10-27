//////////////////////////////////////////////////////////////////////////////
// Filename    : OUTRAGEBOOSTER.h
// Written by  : excel96 
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_OUTRAGEBOOSTER2__
#define __EFFECT_OUTRAGEBOOSTER2__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class OUTRAGEBOOSTER
//////////////////////////////////////////////////////////////////////////////

class EffectOUTRAGEBOOSTER2 : public Effect 
{
public:
	EffectOUTRAGEBOOSTER2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_OUTRAGEBOOSTER2; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	int getSTRPenalty(void) const { return m_STRPenalty; }
	void setSTRPenalty(int penalty) { m_STRPenalty = penalty; }

	int getINTPenalty(void) const { return m_INTPenalty; }
	void setINTPenalty(int penalty) { m_INTPenalty = penalty; }

	int getDEXPenalty(void) const { return m_DEXPenalty; }
	void setDEXPenalty(int penalty) { m_DEXPenalty = penalty; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

private:
	int m_STRPenalty;
	int m_DEXPenalty;
	int m_INTPenalty;
	ObjectID_t   m_UserObjectID;
};

#endif // __EFFECT_OUTRAGEBOOSTER__
