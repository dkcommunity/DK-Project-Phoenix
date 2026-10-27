#ifndef __ACTION_RECALL_PARTY_H__
#define __ACTION_RECALL_PARTY_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionRecallParty
//////////////////////////////////////////////////////////////////////////////

class ActionRecallParty : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_RECALL_PARTY; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

};


//////////////////////////////////////////////////////////////////////////////
// class ActionRecallPartyFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionRecallPartyFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_RECALL_PARTY; }
	virtual string getActionName() const { return "RecallParty"; }
	virtual Action* createAction() const { return new ActionRecallParty(); }

};


#endif
