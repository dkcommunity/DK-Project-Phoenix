#include "ViewAbility.h"
#include "Viewable.h"
#include "Creature.h"

bool ViewAbility::canSeeEffect( EffectID_t eType, Level_t level ) const
{
	EFFECTVIEWABILITYMAP::const_iterator mitr = m_EffectViewAbilities.find(eType);
	if ( mitr == m_EffectViewAbilities.end() ) return false;

	const list<Level_t>& effectLevels = mitr->second;
	list<Level_t>::const_iterator itr = effectLevels.begin();

	for ( ; itr != effectLevels.end() ; ++itr )
	{
		// 0이면 레벨에 관계없이 보는거다.
		if ( *itr == 0 || *itr >= level ) return true;
	}

	return false;
}

bool ViewAbility::canSee(const Viewable* pViewable) const
{
	if ( pViewable == NULL ) return false;
	list<ViewEffect>::const_iterator itr = pViewable->begin();

	for ( ; itr != pViewable->end() ; ++itr )
	{
		if ( !canSeeEffect( itr->getEffectType(), itr->getLevel() ) ) return false;
	}

	return true;
}

bool ViewAbility::canSee(Creature* pCreature) const
{
	return canSee( pCreature->getViewable() );
}
