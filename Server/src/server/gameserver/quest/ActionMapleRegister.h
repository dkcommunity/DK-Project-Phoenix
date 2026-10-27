//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionMapleRegister.h
// Written By  : smallheart
// Description : 2008년 가을(10월) 이벤트
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_MAPLE_REGISTER_H__
#define __ACTION_MAPLE_REGISTER_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRegister
//////////////////////////////////////////////////////////////////////////////

class ActionMapleRegister : public Action 
{
public:
	ActionMapleRegister();
	~ActionMapleRegister();
	
	virtual ActionType_t getActionType() const { return ACTION_MAPLE_REGISTER; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

};


//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRegisterFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionMapleRegisterFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_MAPLE_REGISTER; }
	virtual string getActionName() const { return "MapleRegister"; }
	virtual Action* createAction() const { return new ActionMapleRegister(); }
};

#endif
