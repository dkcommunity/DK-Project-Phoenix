//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckComebackEventItem200604.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_CHECK_COMEBACK_EVENT_ITEM_200604_H__
#define __ACTION_CHECK_COMEBACK_EVENT_ITEM_200604_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionCheckEventItemDialog
//////////////////////////////////////////////////////////////////////////////

class ActionCheckComebackEventItem200604 : public Action 
{
public:
	ActionCheckComebackEventItem200604();
	~ActionCheckComebackEventItem200604();

	virtual ActionType_t getActionType() const { return ACTION_CHECK_COMEBACK_EVENT_ITEM_200604; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	string					m_EventCheckType;
	ScriptID_t				m_EventScriptID;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionCheckEventItemDialogFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionCheckComebackEventItem200604Factory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_CHECK_COMEBACK_EVENT_ITEM_200604; }
	virtual string getActionName() const { return "CheckComebackEventItem200604"; }
	virtual Action* createAction() const { return new ActionCheckComebackEventItem200604(); }
};

#endif
