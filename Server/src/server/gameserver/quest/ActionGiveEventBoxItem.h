//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveEventBoxItem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_GIVE_EVENT_BOX_ITEM_H__
#define __ACTION_GIVE_EVENT_BOX_ITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGiveEventBoxItem
//////////////////////////////////////////////////////////////////////////////

class ActionGiveEventBoxItem : public Action 
{
public:
	ActionGiveEventBoxItem();
	~ActionGiveEventBoxItem();

	virtual ActionType_t getActionType() const { return ACTION_GIVE_EVENT_BOX_ITEM; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
};


//////////////////////////////////////////////////////////////////////////////
// class ActionGiveEventBoxItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionGiveEventBoxItemFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_GIVE_EVENT_BOX_ITEM; }
	virtual string getActionName() const { return "GiveEventBoxItem"; }
	virtual Action* createAction() const { return new ActionGiveEventBoxItem(); }
};

#endif
