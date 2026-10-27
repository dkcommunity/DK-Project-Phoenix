#ifndef __SIMPLE_QUEST_REWARD_MANAGER_H__
#define __SIMPLE_QUEST_REWARD_MANAGER_H__

#include "RewardClassInfoManager.h"

class SimpleQuestRewardManager : public RewardClassInfoManager
{
public:
	SimpleQuestRewardManager(NPC* pNPC) : RewardClassInfoManager(pNPC) { }
	~SimpleQuestRewardManager() { }

public:
	void load(NPCID_t npcID) throw(Error);
};

#endif// __SIMPLE_QUEST_REWARD_MANAGER_H__
