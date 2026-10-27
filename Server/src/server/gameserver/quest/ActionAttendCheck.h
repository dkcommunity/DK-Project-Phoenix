//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveComebackEventItem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_ATTENT_CHECK_H__
#define __ACTION_ATTENT_CHECK_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGiveComebackEventItem
//////////////////////////////////////////////////////////////////////////////

class ActionAttendCheck : public Action 
{
public:
	ActionAttendCheck();
	~ActionAttendCheck();

	virtual ActionType_t getActionType() const { return ACTION_ATTEND_CHECK; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

};


//////////////////////////////////////////////////////////////////////////////
// class ActionGiveComebackEventItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionAttendCheckFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_ATTEND_CHECK; }
	virtual string getActionName() const { return "AttendCheck"; }
	virtual Action* createAction() const { return new ActionAttendCheck(); }
};

#endif

