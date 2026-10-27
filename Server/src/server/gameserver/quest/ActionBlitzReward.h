//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionBlitzReward.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_BLITZ_REWARD_H__
#define __ACTION_BLITZ_REWARD_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRegister
//////////////////////////////////////////////////////////////////////////////

class ActionBlitzReward : public Action 
{
public:
	ActionBlitzReward();
	~ActionBlitzReward();

	virtual ActionType_t getActionType() const { return ACTION_BLITZ_REWARD; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;
private:
	int m_BlitzRewardCount;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRewardFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionBlitzRewardFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_BLITZ_REWARD; }
	virtual string getActionName() const { return "BlitzReward"; }
	virtual Action* createAction() const { return new ActionBlitzReward(); }
};

#endif
