//--------------------------------------------------------------------------------
// SimpleQuest.h
//--------------------------------------------------------------------------------
//
// {START}
//			create() --> [WAIT]
//
// [WAIT]
//			take() --> [ACTIVE]
//
// [ACTIVE]
//			processActive()
//			{
//				checkFail() --> givePenalty() --> {END}
//				checkSuccess() --> [COMPLETE]
//			}
// 
// [COMPLETE]
//			checkComplete() --> giveReward() --> {END}
//
// {END}
//			destroy()
//
//
//--------------------------------------------------------------------------------
//
// 일단 Quest는 gameserver상에서만 생성된다.
// 그러다가. 누가 Quest를 받으면, DB에 생성한다.
// Quest받을 조건은 이미 Quest를 받기 이전에 체크되기 때문에
// DB에 저장할 필요가 없다.
//
// DB table: < SimpleQuest >
//
// pOwner		varchar,	// 이 퀘스트를 받은 사람
// QuestType 	int, 
// Objective 	text,
// Bonus 		text,
// Penalty 		text,
// Deadline		datetime,
//--------------------------------------------------------------------------------


#ifndef __SIMPLE_QUEST_H__
#define __SIMPLE_QUEST_H__

#include "Types.h"
#include "Exception.h"
#include "Quest.h"
#include "Timeval.h"
#include "Creature.h"
#include "QuestPrice.h"
#include <list>


class SimpleQuest : public Quest {
public :
	SimpleQuest();
	virtual ~SimpleQuest();

	void 				setDeadline (Turn_t delay);
	Duration_t		 	getRemainDuration() const;

	//virtual void		create() = 0;
	virtual void        save() = 0;
	virtual void		destroy();

	//virtual bool		checkSuccess(const QuestEvent* pQuestEvent) const = 0;
	virtual bool		checkFail();
	//virtual bool		checkComplete() const = 0;

	virtual void		take(Creature* pCreature, bool bNewQuest=true);
	virtual void		giveReward();
	virtual void		givePenalty();

	virtual void		heartbeat();

	void				setOwner(Creature* pOwner)	{ m_pOwner = pOwner; }
	void				setAvailableSecond(int sec)	{ m_AvailableSecond = sec; }
	void				addReward(QuestPrice* pQuestPrice);
	void				addPenalty(QuestPrice* pQuestPrice);

public :
	virtual void		setObjective(const string& text) = 0;
	virtual string		getObjectiveToString() const = 0;
	virtual void		addReward(const string& text);
	virtual void		addPenalty(const string& text);
	virtual string		getRewardToString() const;
	virtual string		getPenaltyToString() const;

	virtual string		toString() const = 0;

protected :
	virtual void		processActive();
	virtual void		processFail();

protected :
	Creature*			m_pOwner;
	int					m_AvailableSecond;
	Timeval				m_Deadline;
	QuestPrice*			m_pReward;
	QuestPrice*			m_pPenalty;
};

#endif

