//////////////////////////////////////////////////////////////////////////////
// Filename    : EventBlitzRewardManager.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_BLITZ_REWARD_MANAGER_H__
#define __EVENT_BLITZ_REWARD_MANAGER_H__

#include "Types.h"
#include "Exception.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// class EventBlitzRewardManager
//////////////////////////////////////////////////////////////////////////////

class EventBlitzRewardManager
{
	struct EVENT_ITEM_TEMPLATE
	{
		Item::ItemClass		ItemClass;
		ItemType_t			ItemType;
		string				ItemName;
		string				ItemOption;
		int					LimitTime;
		int					ItemCount;
		int					Ratio;
	};

public:
	EventBlitzRewardManager() {}
	~EventBlitzRewardManager() throw(Error);
	void Load();
	Item* getEventBlitzReward(Creature * pCreature, int Group, int& limitTime, _TPOINT& pt) throw (Error);
private :
	vector<EVENT_ITEM_TEMPLATE*>		*m_EventBlitzRewardItemInfo; // 슬레이어 아이템 정보
	int							*m_TotalRatio; //20080901 슬레이어 이벤트 아이템의 총 확률
	int							m_maxGroup; // 20080425 여러 개의 그룹에 대해 확률을 정할 수 있게 개선을 위해 추가

};

extern EventBlitzRewardManager* g_pEventBlitzRewardManager;

#endif
