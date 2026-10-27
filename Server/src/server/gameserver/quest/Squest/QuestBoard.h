#ifndef __QUEST_BOARD_H__
#define __QUEST_BOARD_H__

#include "Types.h"
#include "Exception.h"
#include <map>

class Quest;
class GCNPCAskDynamic;
class NPC;
class Script;

class QuestBoard {
public :
	typedef map<QuestID_t, Quest*>		QUESTS;

public :
	QuestBoard();
	virtual ~QuestBoard();

	void		init(int num);
	void		release();

	void		regenerate(int num);

	bool		add(Quest* pQuest);	
	Quest*		remove(QuestID_t qid);
	
	void		setScript(Script* pScript) const;
	//void		makeNPCAskPacket(GCNPCAskDynamic& npcAskPacket) const;

	const QUESTS& getQuests() const { return m_Quests; }

private :
	NPC*	m_pNPC;
	QUESTS	m_Quests;
};


#endif
