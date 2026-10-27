//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionResetAttribute.h
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_RESET_ATTRIBUTE_H__
#define __ACTION_RESET_ATTRIBUTE_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionResetAttribute
//////////////////////////////////////////////////////////////////////////////

class ActionResetAttribute : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_RESET_ATTRIBUTE; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

};


//////////////////////////////////////////////////////////////////////////////
// class ActionResetAttributeFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionResetAttributeFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_RESET_ATTRIBUTE; }
	virtual string getActionName() const { return "ResetAttribute"; }
	virtual Action* createAction() const { return new ActionResetAttribute(); }
};

#endif
