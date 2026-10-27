//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeLeather.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_TRADE_LEATHER_H__
#define __ACTION_TRADE_LEATHER_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionTradeLeather
//////////////////////////////////////////////////////////////////////////////

class ActionTradeLeather : public Action 
{
public:
	ActionTradeLeather();
	~ActionTradeLeather();

	virtual ActionType_t getActionType() const { return ACTION_TRADE_LEATHER; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
};


//////////////////////////////////////////////////////////////////////////////
// class ActionTradeLeatherFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionTradeLeatherFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_TRADE_LEATHER; }
	virtual string getActionName() const { return "TradeLeather"; }
	virtual Action* createAction() const { return new ActionTradeLeather(); }
};

#endif
