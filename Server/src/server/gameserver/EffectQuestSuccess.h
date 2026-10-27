#ifndef __EFFECT_QUESTSUCCESS__
#define __EFFECT_QUESTSUCCESS__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectQuestSuccess
//////////////////////////////////////////////////////////////////////////////

class EffectQuestSuccess : public Effect 
{
public:
	EffectQuestSuccess(Creature* pCreature) throw(Error);

public:
	virtual EffectClass getEffectClass() const throw() { return EFFECT_CLASS_QUESTSUCCESS; }

	void affect() throw(Error) { }
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

};

#endif // __EFFECT_QUESTSUCCESS__

