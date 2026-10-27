//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGambleSystem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_GAMBLE_SYSTEM_H__
#define __ACTION_GAMBLE_SYSTEM_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGambleSystem
//////////////////////////////////////////////////////////////////////////////

class ActionGambleSystem : public Action 
{
public:
	ActionGambleSystem();
	~ActionGambleSystem();

	virtual ActionType_t getActionType() const { return ACTION_GAMBLE_SYSTEM; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	int							m_SelectID;
	ScriptID_t					m_NextGambleScriptID;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionGambleSystemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionGambleSystemFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_GAMBLE_SYSTEM; }
	virtual string getActionName() const { return "GambleSystem"; }
	virtual Action* createAction() const { return new ActionGambleSystem(); }
};

#endif
