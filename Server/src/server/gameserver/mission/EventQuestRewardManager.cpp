
#include "EventQuestRewardManager.h"
#include "DB.h"
#include "Item.h"
#include "RandomRewardClass.h"
#include "SlayerWeaponRewardClass.h"
#include "QuestAdvanceRewardInfo.h"
#include "ItemRewardInfo.h"

// 2026-05-10: name-based -> NPCID-based (legacy SQL'ler yorum icinde, aktif degil)
void EventQuestRewardManager::load(NPCID_t npcID) throw(Error)
{
	__BEGIN_TRY

	(void)npcID;  // unused — tum SQL'ler yorum icinde
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		// �̰� �Ⱦ��°Ŵ�.
		
/*		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult;

		pResult = pStmt->executeQuery("SELECT RewardClass, RewardID, QuestLevel FROM EventQuestRewardInfo WHERE NPC='%s'", name.c_str());
		
		while( pResult->next() )
		{
			int index = 0;

			RewardClass_t	rClass	= (RewardClass_t)		pResult->getInt( ++index );
			RewardID_t		rID		= (RewardID_t	)		pResult->getInt( ++index );
			int				qLevel	= (int)					pResult->getInt( ++index );

			if ( m_RewardClasses[rClass] != NULL )
			{
				cout << "EventQuestRewardManager : �� �Ȱ����� �ΰ��� ����־�" << (uint)rClass << endl;
				Assert( false );
			}

			m_RewardClasses[rClass] = new RandomRewardClass( rClass );
			QuestAdvanceRewardInfo* pRI = new QuestAdvanceRewardInfo( rID, rClass, qLevel, qLevel+1 );
			m_RewardClasses[rClass]->addRewardInfo( pRI );
		}*/

		/*pResult = pStmt->executeQuery("SELECT RewardClass, RewardID, IClass, IType, OptionType, TimeLimitSec FROM ItemRewardInfo WHERE NPC='%s'", name.c_str());

		while (pResult->next()) 
		{
			int index = 0;

			RewardClass_t	rClass	= (RewardClass_t)		pResult->getInt( ++index );
			RewardID_t		rID		= (RewardID_t	)		pResult->getInt( ++index );
			Item::ItemClass	iClass	= (Item::ItemClass)		pResult->getInt( ++index );
			ItemType_t		iType	= (ItemType_t)			pResult->getInt( ++index );
			string			option	= 						pResult->getString( ++index );
			DWORD			time	= (DWORD)				pResult->getInt( ++index );

			if ( m_RewardClasses[ rClass ] == NULL ){
				cout << "EventQuestRewardManager : �̳��� �� �� ���� �̺�Ʈ����Ʈ�ڳ�" << (uint)rClass << endl;
				Assert( false );
			//	m_RewardClasses[rClass] = new RandomRewardClass( rClass );
			//	cout << "NPC : " << name << ", RewardClass : " << (uint)rClass << endl;
			}

			ItemRewardInfo* pItemRI = new ItemRewardInfo( rID, rClass, iClass, iType, option, time );
			m_RewardClasses[rClass]->addRewardInfo( pItemRI );
		}

		pResult = pStmt->executeQuery("SELECT RewardClass, RewardID, IClass, IType, OptionType, TimeLimitSec FROM SlayerWeaponRewardInfo WHERE NPC='%s'", name.c_str());

		while (pResult->next())
		{
			int index = 0;

			RewardClass_t	rClass	= (RewardClass_t)		pResult->getInt( ++index );
			RewardID_t		rID		= (RewardID_t	)		pResult->getInt( ++index );
			Item::ItemClass	iClass	= (Item::ItemClass)		pResult->getInt( ++index );
			ItemType_t		iType	= (ItemType_t)			pResult->getInt( ++index );
			string			option	= 						pResult->getString( ++index );
			DWORD			time	= (DWORD)				pResult->getInt( ++index );

			if ( m_RewardClasses[ rClass ] == NULL ){
				cout << "EventQuestRewardManager : �̳��� �� �� ���� �̺�Ʈ����Ʈ�ڳ�" << endl;
				Assert( false );
			}

			ItemRewardInfo* pItemRI = new ItemRewardInfo( rID, rClass, iClass, iType, option, time );
			m_RewardClasses[rClass]->addRewardInfo( pItemRI );
		}*/

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}
