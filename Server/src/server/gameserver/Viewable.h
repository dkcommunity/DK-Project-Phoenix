#ifndef __VIEWABLE_H__
#define __VIEWABLE_H__

#include "Types.h"
#include <list>

#include "ViewEffect.h"

class Viewable
{
	list<ViewEffect> m_ViewEffectList;

public:
	Viewable() { }

	void addViewEffect( const ViewEffect& effect ) { m_ViewEffectList.push_back(effect); }
	void addViewEffect( EffectID_t eType, Level_t level ) { addViewEffect( ViewEffect( eType, level ) ); }
	void removeViewEffect( EffectID_t eType, Level_t level );

	list<ViewEffect>::iterator begin() { return m_ViewEffectList.begin(); }
	list<ViewEffect>::iterator end() { return m_ViewEffectList.end(); }

	list<ViewEffect>::const_iterator begin() const { return m_ViewEffectList.begin(); }
	list<ViewEffect>::const_iterator end() const { return m_ViewEffectList.end(); }
};

#endif
