//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionMapleReward.h
// Written By  : smallheart
// Description : 2008년 가을(10월)이벤트 - 메이플 문양
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_MAPLE_REWARD_H__
#define __ACTION_MAPLE_REWARD_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionBlitzRegister
//////////////////////////////////////////////////////////////////////////////

class ActionMapleReward : public Action 
{
public:
	ActionMapleReward();
	~ActionMapleReward();

	virtual ActionType_t getActionType() const { return ACTION_MAPLE_REWARD; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;
private:
	int m_MapleRewardCount;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionMapleRewardFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionMapleRewardFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_MAPLE_REWARD; }
	virtual string getActionName() const { return "MapleReward"; }
	virtual Action* createAction() const { return new ActionMapleReward(); }
};

#endif