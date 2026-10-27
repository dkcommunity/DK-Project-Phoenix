//--------------------------------------------------------------------------------
// QuestFactoryManager.h
//--------------------------------------------------------------------------------

#ifndef __QUEST_FACTORY_MANAGER_H__
#define __QUEST_FACTORY_MANAGER_H__

#include "Types.h"
#include "Exception.h"
#include "Quest.h"
class QuestCreateInfo;

//--------------------------------------------------------------------------------
// QuestFactory
//--------------------------------------------------------------------------------
class QuestFactory {
public :
	virtual Quest::QuestType	getQuestType() const = 0;
	virtual Quest*				create(const QuestCreateInfo* qcInfo=NULL) const = 0;
};

//--------------------------------------------------------------------------------
// QuestFactoryManager
//--------------------------------------------------------------------------------
class QuestFactoryManager {
public :
	QuestFactoryManager();
	~QuestFactoryManager();

	static QuestFactoryManager*	getInstance()
	{
		static QuestFactoryManager		QPFM;
		return &QPFM;	
	}

	Quest*		create(Quest::QuestType qptype, const QuestCreateInfo* qcInfo=NULL) const;

	
protected :
	void		addFactory(QuestFactory* pQuestFactory);
	
private :
	QuestFactory*		m_pQuestFactories[Quest::QUEST_MAX];
};

#endif
