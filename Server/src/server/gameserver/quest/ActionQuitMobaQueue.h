//////////////////////////////////////////////////////////////////////////////
// ActionQuitMobaQueue.h
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_QUIT_MOBA_QUEUE_H__
#define __ACTION_QUIT_MOBA_QUEUE_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

class ActionQuitMobaQueue : public Action
{
public:
	virtual ActionType_t getActionType() const { return ACTION_QUIT_MOBA_QUEUE; }
	virtual void read(PropertyBuffer& propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;
};

class ActionQuitMobaQueueFactory : public ActionFactory
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_QUIT_MOBA_QUEUE; }
	virtual string getActionName() const { return "QuitMobaQueue"; }
	virtual Action* createAction() const { return new ActionQuitMobaQueue(); }
};

#endif
