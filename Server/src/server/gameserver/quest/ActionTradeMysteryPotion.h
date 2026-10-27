//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeMysteryPotion.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_TRADE_MYSTERY_POTION_H__
#define __ACTION_TRADE_MYSTERY_POTION_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionTradeMysteryPotion
//////////////////////////////////////////////////////////////////////////////

class ActionTradeMysteryPotion : public Action 
{
public:
	ActionTradeMysteryPotion();
	~ActionTradeMysteryPotion();

	virtual ActionType_t getActionType() const { return ACTION_TRADE_MYSTERY_POTION; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
};


//////////////////////////////////////////////////////////////////////////////
// class ActionTradeMysteryPotionFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionTradeMysteryPotionFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_TRADE_MYSTERY_POTION; }
	virtual string getActionName() const { return "TradeMysteryPotion"; }
	virtual Action* createAction() const { return new ActionTradeMysteryPotion(); }
};

#endif
