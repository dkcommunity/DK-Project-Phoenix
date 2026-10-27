//--------------------------------------------------------------------------------
// QuestManager.cpp
//--------------------------------------------------------------------------------

#include "Quest.h"
#include "QuestManager.h"

//--------------------------------------------------------------------------------
// constructor / destructor
//--------------------------------------------------------------------------------
QuestManager::QuestManager()
{
	__BEGIN_TRY
		
	__END_CATCH
}

QuestManager::~QuestManager()
{
	__BEGIN_TRY
		
	release();	

	__END_CATCH	
}

//--------------------------------------------------------------------------------
// release
//--------------------------------------------------------------------------------
void
QuestManager::release()
{
	__BEGIN_TRY

	QUEST_LIST::iterator itr = m_Quests.begin();

	for (; itr!=m_Quests.end(); itr++)
	{
		Quest* pQuest = *itr;
		SAFE_DELETE(pQuest);
	}

	__END_CATCH
}

//--------------------------------------------------------------------------------
// add Quest
//--------------------------------------------------------------------------------
bool		
QuestManager::addQuest(Quest* pQuest)
{
	__BEGIN_TRY

	m_Quests.push_back( pQuest );

	return true;
		
	__END_CATCH
}

//--------------------------------------------------------------------------------
// check Event
//--------------------------------------------------------------------------------
Quest*		
QuestManager::checkEvent(QuestEvent* pQuestEvent)
{
	__BEGIN_TRY

	QUEST_LIST::iterator itr = m_Quests.begin();

	for (; itr!=m_Quests.end(); itr++)
	{
		Quest* pQuest = *itr;
		if (pQuest->getState()==Quest::STATE_ACTIVE
			&& pQuest->checkSuccess( pQuestEvent ))
		{
			return pQuest;
		}
	}

	__END_CATCH

	return NULL;	
}

//--------------------------------------------------------------------------------
// remove CompleteQuest
//--------------------------------------------------------------------------------
Quest*      
QuestManager::removeCompleteQuest()
{
	__BEGIN_TRY

	QUEST_LIST::iterator itr = m_Quests.begin();

	for (; itr!=m_Quests.end(); itr++)
	{
		Quest* pQuest = *itr;
		if (pQuest->getState()==Quest::STATE_COMPLETE)
		{
			m_Quests.erase( itr );
			return pQuest;
		}
	}

	__END_CATCH

	return NULL;
}

//--------------------------------------------------------------------------------
// heartbeat
//--------------------------------------------------------------------------------
void		
QuestManager::heartbeat()
{
	__BEGIN_TRY

	QUEST_LIST::iterator itr = m_Quests.begin();

	for (; itr!=m_Quests.end(); itr++)
	{
		Quest* pQuest = *itr;
		pQuest->heartbeat();

		if (pQuest->getState()==Quest::STATE_END)
		{
			SAFE_DELETE(pQuest);
			m_Quests.erase(itr++);
		}
	}

	__END_CATCH
}
