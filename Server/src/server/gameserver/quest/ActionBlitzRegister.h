//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionBlitzRegister.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_BLITZ_REGISTER_H__
#define __ACTION_BLITZ_REGISTER_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRegister
//////////////////////////////////////////////////////////////////////////////

class ActionBlitzRegister : public Action 
{
public:
	ActionBlitzRegister();
	~ActionBlitzRegister();

	virtual ActionType_t getActionType() const { return ACTION_BLITZ_REGISTER; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

};


//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRegisterFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionBlitzRegisterFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_BLITZ_REGISTER; }
	virtual string getActionName() const { return "BlitzRegister"; }
	virtual Action* createAction() const { return new ActionBlitzRegister(); }
};

#endif
