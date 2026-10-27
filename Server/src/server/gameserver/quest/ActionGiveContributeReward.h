//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveContributeReward.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_GIVE_CONTRIBUTE_REWARD_H__
#define __ACTION_GIVE_CONTRIBUTE_REWARD_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGiveHellGardenCrossItem
//////////////////////////////////////////////////////////////////////////////

class ActionGiveContributeReward : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_GIVE_CONTRIBUTE_REWARD; }
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

class ActionGiveContributeRewardFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_GIVE_CONTRIBUTE_REWARD; }
	virtual string getActionName() const { return "GiveContributeReward"; }
	virtual Action* createAction() const { return new ActionGiveContributeReward(); }
};

#endif
