#include "EventQuestInfoManager.h"
#include "RewardClass.h"
#include "RewardClassInfoManager.h"
#include "MonsterKillQuestInfo.h"
#include "GatherItemQuestInfo.h"
#include "MeetNPCQuestInfo.h"
#include "MiniGameQuestInfo.h"
#include "Assert.h"
#include "DB.h"

// 2026-05-10: name-based -> NPCID-based
void EventQuestInfoManager::load(NPCID_t npcID) throw(Error)
{
	__BEGIN_TRY

	clear();

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery(
				"SELECT QuestID, Race, MaxGrade, MinGrade, TimeLimitSec, RewardClass, TargetSType, IsChief, Goal, EventQuest, QuestLevel FROM MonsterKillQuestInfo WHERE NPCID = %d",
				(int)npcID);

		while (pResult->next()) 
		{
			int index = 0;
			QuestID_t		qID			= (QuestID_t) 	pResult->getInt( ++index );
			Race_t			race		= (Race_t)		pResult->getInt( ++index );
			QuestGrade_t	MaxGrade	= (QuestGrade_t)pResult->getInt( ++index );
			QuestGrade_t	MinGrade	= (QuestGrade_t)pResult->getInt( ++index );
			DWORD			timeLimit	= (DWORD)		pResult->getInt( ++index );
			RewardClass_t	rewardClass	= (RewardClass_t)pResult->getInt(++index );
			SpriteType_t	monsterSType= (SpriteType_t)pResult->getInt( ++index );
			bool			isChief		= ( pResult->getInt( ++index ) == 0 )?false:true;
			int				killCount	= (int)			pResult->getInt( ++index );
			bool			isEventQuest=				pResult->getInt( ++index ) != 0;
			int				questLevel	= (int)			pResult->getInt( ++index );

//			RewardClass* pRC = g_pRewardClassInfoManager->getRewardClass( rewardClass );
//			Assert( pRC != NULL );

			MonsterKillQuestInfo* pMonsterKillQI = new MonsterKillQuestInfo( qID, race, MaxGrade, MinGrade, timeLimit, rewardClass, monsterSType, isChief, killCount );
			pMonsterKillQI->setEventQuest( isEventQuest );
			pMonsterKillQI->setQuestLevel( questLevel );
			addQuestInfo( pMonsterKillQI );

			cout << "Loading Quest Info : " << pMonsterKillQI->toString() << endl;
		}

		pResult = pStmt->executeQuery(
				"SELECT QuestID, Race, MaxGrade, MinGrade, TimeLimitSec, RewardClass, TargetIClass, TargetIType, Goal, EventQuest, QuestLevel FROM GatherItemQuestInfo WHERE NPCID = %d",
				(int)npcID);

		while (pResult->next()) 
		{
			int index = 0;
			QuestID_t		qID			= (QuestID_t) 	pResult->getInt( ++index );
			Race_t			race		= (Race_t)		pResult->getInt( ++index );
			QuestGrade_t	MaxGrade	= (QuestGrade_t)pResult->getInt( ++index );
			QuestGrade_t	MinGrade	= (QuestGrade_t)pResult->getInt( ++index );
			DWORD			timeLimit	= (DWORD)		pResult->getInt( ++index );
			RewardClass_t	rewardClass	= (RewardClass_t)pResult->getInt(++index );
			Item::ItemClass iClass 		= (Item::ItemClass)pResult->getInt( ++index );
			ItemType_t		iType		= (ItemType_t)pResult->getInt( ++index );
			int				Count		= (int)			pResult->getInt( ++index );
			bool			isEventQuest=				pResult->getInt( ++index ) != 0;
			int				questLevel	= (int)			pResult->getInt( ++index );

			GatherItemQuestInfo* pGatherItemQI = new GatherItemQuestInfo( qID, race, MaxGrade, MinGrade, timeLimit, rewardClass, iClass, iType, Count );
			pGatherItemQI->setEventQuest( isEventQuest );
			pGatherItemQI->setQuestLevel( questLevel );
			addQuestInfo( pGatherItemQI );

			cout << "Loading Quest Info : " << pGatherItemQI->toString() << endl;
		}

		// MeetNPCQuestInfo bind kolonu: NPCID_BIND (TargetNPCID/SecondNPCID quest oyunundaki diger NPC'lerin ID'si, FARKLI semantic)
		pResult = pStmt->executeQuery(
				"SELECT QuestID, Race, MaxGrade, MinGrade, TimeLimitSec, RewardClass, TargetNPCID, SecondNPCID, EventQuest, QuestLevel FROM MeetNPCQuestInfo WHERE NPCID_BIND = %d",
				(int)npcID);

		while (pResult->next())
		{
			int index = 0;
			QuestID_t		qID			= (QuestID_t) 	pResult->getInt( ++index );
			Race_t			race		= (Race_t)		pResult->getInt( ++index );
			QuestGrade_t	MaxGrade	= (QuestGrade_t)pResult->getInt( ++index );
			QuestGrade_t	MinGrade	= (QuestGrade_t)pResult->getInt( ++index );
			DWORD			timeLimit	= (DWORD)		pResult->getInt( ++index );
			RewardClass_t	rewardClass	= (RewardClass_t)pResult->getInt(++index );
			NPCID_t			targetNPCID = (NPCID_t)		pResult->getInt( ++index );  // 2026-05-10: shadowing fix (parametre npcID)
			NPCID_t			secondNPCID = (NPCID_t)		pResult->getInt( ++index );
			bool			isEventQuest=				pResult->getInt( ++index ) != 0;
			int				questLevel	= (int)			pResult->getInt( ++index );

			MeetNPCQuestInfo* pMeetNPCQI = new MeetNPCQuestInfo( qID, race, MaxGrade, MinGrade, timeLimit, rewardClass, targetNPCID, secondNPCID );
			pMeetNPCQI->setEventQuest( isEventQuest );
			pMeetNPCQI->setQuestLevel( questLevel );
			addQuestInfo( pMeetNPCQI );

			cout << "Loading Quest Info : " << pMeetNPCQI->toString() << endl;
		}

		pResult = pStmt->executeQuery(
				"SELECT QuestID, Race, MaxGrade, MinGrade, TimeLimitSec, RewardClass, GameType, EventQuest, QuestLevel FROM MiniGameQuestInfo WHERE NPCID = %d",
				(int)npcID);

		while (pResult->next())
		{
			int index = 0;
			QuestID_t		qID			= (QuestID_t) 	pResult->getInt( ++index );
			Race_t			race		= (Race_t)		pResult->getInt( ++index );
			QuestGrade_t	MaxGrade	= (QuestGrade_t)pResult->getInt( ++index );
			QuestGrade_t	MinGrade	= (QuestGrade_t)pResult->getInt( ++index );
			DWORD			timeLimit	= (DWORD)		pResult->getInt( ++index );
			RewardClass_t	rewardClass	= (RewardClass_t)pResult->getInt(++index );
			int				GameType	= (int)			pResult->getInt( ++index );
			bool			isEventQuest=				pResult->getInt( ++index ) != 0;
			int				questLevel	= (int)			pResult->getInt( ++index );

			MiniGameQuestInfo* pMiniGameQI = new MiniGameQuestInfo( qID, race, MaxGrade, MinGrade, timeLimit, rewardClass, GameType );
			pMiniGameQI->setEventQuest( isEventQuest );
			pMiniGameQI->setQuestLevel( questLevel );
			addQuestInfo( pMiniGameQI );

			cout << "Loading Quest Info : " << pMiniGameQI->toString() << endl;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt)

	__END_CATCH
}
