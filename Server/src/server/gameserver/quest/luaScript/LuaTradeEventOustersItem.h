#ifndef __LUA_TRADE_EVENT_OUSTERS_ITEM_H__
#define __LUA_TRADE_EVENT_OUSTERS_ITEM_H__

#include "LuaSelectItem.h"

class LuaTradeEventOustersItem : public LuaSelectItem {
public :
	LuaTradeEventOustersItem(LuaState* pState);

	virtual void	prepare();

	int		getLevel() const	{ return m_Level; }
	void	setLevel(int sum) 	{ m_Level = sum; }

protected :
	int		m_Level;
};

#endif

