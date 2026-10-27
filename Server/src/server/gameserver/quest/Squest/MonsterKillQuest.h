//--------------------------------------------------------------------------------
// MonsterKillQuest.h
//--------------------------------------------------------------------------------

#ifndef __MONSTER_KILL_QUEST_H__
#define __MONSTER_KILL_QUEST_H__

#include "MonsterKillQuest.h"
#include "SimpleQuest.h"
#include "QuestFactoryManager.h"
#include "QuestUtil.h"
#include "MonsterSelector.h"
#include <list>
#include <vector>

//--------------------------------------------------------------------------------
// MonsterKillQuest
//--------------------------------------------------------------------------------
class MonsterKillQuest : public SimpleQuest, public MonsterSelector {
public :
	MonsterKillQuest(const MonsterSelector& mtc)
	: MonsterSelector(mtc)
	{
	}
	MonsterKillQuest(MonsterType_t monsterType, SpriteType_t spriteType, int number)
	: MonsterSelector(monsterType, spriteType, number)
	{
	}

	virtual ~MonsterKillQuest() {}

	virtual QuestType	getQuestType() const 	{ return QUEST_MONSTER_KILL; }

	virtual void		create();
	virtual void        save();

	virtual bool		checkSuccess(const QuestEvent* pQuestEvent);
	virtual bool		checkComplete();

public :
	virtual void        setObjective(const string& text)	{ MonsterSelector::setMonster(text); }
	virtual string      getObjectiveToString() const		{ return MonsterSelector::toString(); }

	string				toString() const;

};

//--------------------------------------------------------------------------------
// MonsterKillQuestFactory
//--------------------------------------------------------------------------------
class MonsterKillQuestFactory : public QuestFactory {
public :
	virtual Quest::QuestType	getQuestType() const { return Quest::QUEST_MONSTER_KILL; }
	virtual Quest*				create(const QuestCreateInfo* qcInfo=NULL) const;	

	static void		initMonsterTypeInfos();
	
private :
	typedef vector<MonsterSelector>	MonsterTypeInfos;	// 레벨에 따른 몬스터 종류

	static MonsterTypeInfos	m_MonsterTypeInfos;
};

#endif

