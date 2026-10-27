//////////////////////////////////////////////////////////////////////////////
// ActionOpenAuctionHouse.h
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_OPEN_AUCTION_HOUSE_H__
#define __ACTION_OPEN_AUCTION_HOUSE_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

class ActionOpenAuctionHouse : public Action
{
public:
	virtual ActionType_t getActionType() const { return ACTION_OPEN_AUCTION_HOUSE; }
	virtual void read(PropertyBuffer& propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;
};

class ActionOpenAuctionHouseFactory : public ActionFactory
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_OPEN_AUCTION_HOUSE; }
	virtual string getActionName() const { return "OpenAuctionHouse"; }
	virtual Action* createAction() const { return new ActionOpenAuctionHouse(); }
};

#endif
