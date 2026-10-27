#ifndef __QUEST_INFO_MANAGER_H__
#define __QUEST_INFO_MANAGER_H__

#include "Types.h"
#include "Exception.h"

#include "Gpackets/GCNPCResponse.h"

#include "QuestInfo.h"
#include <map>

//class QuestInfo;
class QuestStatus;
class PlayerCreature;
class NPC;

class QuestInfoManager
{
protected:
	typedef map<QuestID_t, QuestInfo*> QuestInfoHashMap;

public:
	QuestInfoManager( NPC* pNPC ) { m_QuestInfos.clear(); m_pOwnerNPC = pNPC; }
	virtual ~QuestInfoManager() throw();

public:
	void		clear() throw(Error);
	// 2026-05-10: name -> NPCID (NPC.Name degisikligi quest sistemi bozmasin)
	virtual void		load( NPCID_t npcID = 0 ) throw(Error) = 0;
	void		addQuestInfo(QuestInfo* pQI) throw(Error);

	QuestMessage	canExecuteQuest( QuestID_t qID, PlayerCreature* pPC ) const throw(Error);
	QuestMessage	startQuest( QuestID_t qID, PlayerCreature* pPC ) throw(Error);
	QuestStatus*	makeQuestStatus( QuestID_t qID, PlayerCreature* pPC ) const throw(Error);

	bool			isEventQuest( QuestID_t qID ) const;

	// OutITR�� container<QuestID_t>::output_iterator Ÿ���̾�� �Ѵ�.
	template <class OutITR>
	void		getEventQuestIDs( int questLevel, PlayerCreature* pPC, OutITR outItr ) const throw(Error)
	{
		__BEGIN_TRY

		QuestInfoHashMap::const_iterator itr = m_QuestInfos.begin();
		QuestInfoHashMap::const_iterator endItr = m_QuestInfos.end();

		for ( ; itr != endItr ; ++itr )
		{
			QuestInfo* qInfo = dynamic_cast<QuestInfo*> (itr->second);
//			QuestID_t qID = dynamic_cast<QuestID_t> (itr->first);
//			if ( qInfo->isEventQuest() && qInfo->getQuestLevel() == questLevel && canExecuteQuest( qID, pPC ) == START_SUCCESS )
			if ( qInfo->isEventQuest() && qInfo->getQuestLevel() == questLevel && canExecuteQuest( itr->first, pPC ) == START_SUCCESS )
//				(*outItr++) = qID;
				(*outItr++) = itr->first;
		}

		__END_CATCH
	}

	// OutITR�� container<QuestID_t>::output_iterator Ÿ���̾�� �Ѵ�.
	template <class OutITR>
	void		getPossibleQuestIDs( PlayerCreature* pPC, OutITR outItr ) const throw(Error)
	{
		__BEGIN_TRY

		QuestInfoHashMap::const_iterator itr = m_QuestInfos.begin();
		QuestInfoHashMap::const_iterator endItr = m_QuestInfos.end();

		for ( ; itr != endItr ; ++itr )
			if ( canExecuteQuest( itr->first, pPC ) == START_SUCCESS )
				(*outItr++) = itr->first;

		__END_CATCH
	}

	// *InITR �� QuestID_t, *OutITR�� QuestInfo*
	template <class InITR, class OutITR>
	void		getMonsterKillQuests( InITR b, InITR e, OutITR t )
	{
		QuestInfoHashMap::const_iterator endItr = m_QuestInfos.end();
		for ( InITR i = b ; i != e ; ++i )
		{
			QuestInfoHashMap::const_iterator itr = m_QuestInfos.find(*i);
			if ( itr != endItr )
			{
				QuestInfo* pQI = itr->second;
				if ( pQI->isMonsterKillQuest() ) (*t++) = pQI;
			}
		}
	}

private:
	NPC*					m_pOwnerNPC;
	QuestInfoHashMap		m_QuestInfos;
};

#endif // __QUEST_INFO_MANAGER_H__
