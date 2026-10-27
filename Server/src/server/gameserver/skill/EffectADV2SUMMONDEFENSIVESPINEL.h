#ifndef __EFFECT_ADV2SUMMONDEFENSIVESPINEL__
#define __EFFECT_ADV2SUMMONDEFENSIVESPINEL__

#include "Effect.h"
#include "EffectLoader.h"

class EffectADV2SUMMONDEFENSIVESPINEL : public Effect 
{
public:
	EffectADV2SUMMONDEFENSIVESPINEL(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2SUMMONDEFENSIVESPINEL; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif
