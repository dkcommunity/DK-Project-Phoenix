//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckLevelWarReward.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_CHECK_LEVEL_WAR_REWARD_H__
#define __ACTION_CHECK_LEVEL_WAR_REWARD_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionCheckLevelWarReward
//////////////////////////////////////////////////////////////////////////////

class ActionCheckLevelWarReward : public Action 
{
	struct EVENT_ITEM_TEMPLATE
	{
		Item::ItemClass		ItemClass;
		ItemType_t			ItemType;
		string				ItemName;
		string				ItemOption;
		int					Ratio;
	};

public:
	ActionCheckLevelWarReward();
	~ActionCheckLevelWarReward();

	virtual ActionType_t getActionType() const { return ACTION_CHECK_LEVEL_WAR_REWARD; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	vector<EVENT_ITEM_TEMPLATE*>		m_SlayerLevelWarRewardItemInfo; // 슬레이어 아이템 정보
	vector<EVENT_ITEM_TEMPLATE*>		m_VampireLevelWarRewardItemInfo; // 뱀파이어 아이템 정보
	vector<EVENT_ITEM_TEMPLATE*>		m_OustersLevelWarRewardItemInfo; // 아우스터즈 아이템 정보
	string						m_Type;
	int							m_QuestItemNum;
	int							m_SlayerTotalRatio; //20071106 슬레이어 이벤트 아이템의 총 확률
	int							m_VampireTotalRatio; //20071106 뱀파이어 이벤트 아이템의 총 확률
	int							m_OustersTotalRatio; //20071106 아우스터즈 이벤트 아이템의 총 확률
};


//////////////////////////////////////////////////////////////////////////////
// class ActionCheckLevelWarRewardFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionCheckLevelWarRewardFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_CHECK_LEVEL_WAR_REWARD; }
	virtual string getActionName() const { return "CheckLevelWarReward"; }
	virtual Action* createAction() const { return new ActionCheckLevelWarReward(); }
};

#endif
