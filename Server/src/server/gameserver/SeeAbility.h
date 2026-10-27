#ifndef __SEE_ABILITY_H__
#define __SEE_ABILITY_H__

#include "Types.h"

class Creature;

class SeeAbility
{
public:
	SeeAbility();

	bool canSee(Creature* pTarget) const;

	bool canSeeInvisibility() const;
	bool canSeeHide() const;
	bool canSeeMine() const;
	bool canSeeSniping() const;
};

#endif
