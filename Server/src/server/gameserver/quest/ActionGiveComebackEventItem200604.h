//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveComebackEventItem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_GIVE_COMEBACK_EVENT_ITEM_200604_H__
#define __ACTION_GIVE_COMEBACK_EVENT_ITEM_200604_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGiveComebackEventItem
//////////////////////////////////////////////////////////////////////////////

class ActionGiveComebackEventItem200604 : public Action 
{
public:
	ActionGiveComebackEventItem200604();
	~ActionGiveComebackEventItem200604();

	virtual ActionType_t getActionType() const { return ACTION_GIVE_COMEBACK_EVENT_ITEM_200604; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	void	executeComebackItem(Creature* pCreature1, Creature* pCreature2 = NULL);
	void	executeComebackPremiumItem(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	string	m_EventType;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionGiveComebackEventItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionGiveComebackEventItem200604Factory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_GIVE_COMEBACK_EVENT_ITEM_200604; }
	virtual string getActionName() const { return "GiveComebackEventItem200604"; }
	virtual Action* createAction() const { return new ActionGiveComebackEventItem200604(); }
};

#endif
