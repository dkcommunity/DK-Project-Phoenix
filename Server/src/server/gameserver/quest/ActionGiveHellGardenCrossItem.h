//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveHellGardenCrossItem.h
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_GIVE_HELLGARDEN_CROSS_ITEM_H__
#define __ACTION_GIVE_HELLGARDEN_CROSS_ITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGiveHellGardenCrossItem
//////////////////////////////////////////////////////////////////////////////

class ActionGiveHellGardenCrossItem : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_GIVE_HELLGARDEN_CROSS_ITEM; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;
public:
	int getType(void) const { return m_Type; }
	void setType(int type) { m_Type = type; }

private:
	ItemType_t m_Type; 
}
;


//////////////////////////////////////////////////////////////////////////////
// class ActionGiveHellGardenCrossItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionGiveHellGardenCrossItemFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_GIVE_HELLGARDEN_CROSS_ITEM; }
	virtual string getActionName() const { return "GiveHellGardenCrossItem"; }
	virtual Action* createAction() const { return new ActionGiveHellGardenCrossItem(); }
};

#endif
