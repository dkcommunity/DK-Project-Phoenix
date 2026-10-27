#ifndef __VIEW_ABILITY_H__
#define __VIEW_ABILITY_H__

#include "Types.h"
#include "Assert.h"
#include <map>
#include <vector>

class Creature;
class Viewable;

class ViewAbility
{
	typedef map<EffectID_t, list<Level_t> > EFFECTVIEWABILITYMAP;
	EFFECTVIEWABILITYMAP m_EffectViewAbilities;

public:
	ViewAbility() { }

	bool canSee(Creature* pCreature) const;
	void addCanSeeEffect( EffectID_t eType, Level_t level = 0 ) { m_EffectViewAbilities[eType].push_back(level); }
	void removeCanSeeEffect( EffectID_t eType, Level_t level = 0 )
	{
		list<Level_t>& effectLevels = m_EffectViewAbilities[eType];
		list<Level_t>::iterator itr = find( effectLevels.begin(), effectLevels.end(), level );
		if ( itr != effectLevels.end() ) effectLevels.erase( itr );
		else
		{
			filelog("ViewBug.log", "%s : ( %d, %d )", __PRETTY_FUNCTION__, eType, level );
			cout << "view bug 발생" << endl;
		}
	}

	bool canSeeEffect( EffectID_t eType, Level_t level ) const;
	bool canSee(const Viewable* pViewable) const;
};

#endif
