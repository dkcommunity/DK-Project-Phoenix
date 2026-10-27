#include "Viewable.h"

void Viewable::removeViewEffect( EffectID_t eType, Level_t level )
{
	for ( list<ViewEffect>::iterator itr = begin() ; itr != end() ; ++itr )
	{
		if ( itr->getEffectType() == eType && itr->getLevel() == level )
		{
			m_ViewEffectList.erase( itr );
			return;
		}
	}

	filelog("ViewBug.log", "%s : ( %d, %d )", __PRETTY_FUNCTION__, eType, level );
	cout << "view bug ¹ß»ý" << endl;
}
