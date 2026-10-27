//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeSealingStone.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_TRADE_SEALING_STONE_H__
#define __ACTION_TRADE_SEALING_STONE_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionTradeSealingStone
//////////////////////////////////////////////////////////////////////////////

class ActionTradeSealingStone : public Action 
{
public:
	ActionTradeSealingStone();
	~ActionTradeSealingStone();

	virtual ActionType_t getActionType() const { return ACTION_TRADE_SEALING_STONE; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
};


//////////////////////////////////////////////////////////////////////////////
// class ActionTradeSealingStoneFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionTradeSealingStoneFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_TRADE_SEALING_STONE; }
	virtual string getActionName() const { return "TradeSealingStone"; }
	virtual Action* createAction() const { return new ActionTradeSealingStone(); }
};

#endif
