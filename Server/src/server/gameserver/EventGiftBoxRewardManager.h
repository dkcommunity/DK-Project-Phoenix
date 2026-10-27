//////////////////////////////////////////////////////////////////////////////
// Filename    : EventGiftBoxRewardManager.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_GIFTBOX_REWARD_MANAGER_H__
#define __EVENT_GIFTBOX_REWARD_MANAGER_H__

#include "Types.h"
#include "Exception.h"
#include "Item.h"

#include <map>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// class EventGiftBoxRewardManager
//////////////////////////////////////////////////////////////////////////////

class EventGiftBoxRewardManager
{
	struct EVENT_ITEM_TEMPLATE
	{
		Item::ItemClass		ItemClass;
		ItemType_t			ItemType;
		string				ItemName;
		string				ItemOption;
		int					LimitTime;
		int					Ratio;
		int			MinGrade;//by rallser
		int			MaxGrade;//by rallser
	};

	typedef vector<EVENT_ITEM_TEMPLATE> TEventItemTemplateList;
	typedef map<int, TEventItemTemplateList> TRewardItemInfoList;
	
public:
	EventGiftBoxRewardManager() {}
	~EventGiftBoxRewardManager() throw(Error);
	void Load();
	Item* getEventGiftBoxReward(Creature * pCreature, int Group, int& limitTime) throw (Error);
private :
	
	TRewardItemInfoList m_SlayerEventGiftBoxRewardItemInfo;
	TRewardItemInfoList m_VampireEventGiftBoxRewardItemInfo;
	TRewardItemInfoList m_OustersEventGiftBoxRewardItemInfo;
	
	map<int, int> m_SlayerTotalRatio;
	map<int, int> m_VampireTotalRatio;
	map<int, int> m_OustersTotalRatio;

};

extern EventGiftBoxRewardManager* g_pEventGiftBoxRewardManager;

#endif
