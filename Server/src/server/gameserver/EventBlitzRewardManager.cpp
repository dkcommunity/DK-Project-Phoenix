////////////////////////////////////////////////////////////////////////////////
// Filename    : EventBlitzRewardManager.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "EventBlitzRewardManager.h"
#include "Item.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "NPC.h"
#include "Player.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "Zone.h"
#include "VariableManager.h"
#include "FlagSet.h"
#include "DB.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCUseOK.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////
// EventBlitzRewardManager
////////////////////////////////////////////////////////////////////////////////
void EventBlitzRewardManager::Load()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result*    pResult = NULL;
	m_maxGroup = 0;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		// FAZ 4 - MySQL 8 fix: `Groups` reserved keyword.
		pResult = pStmt->executeQuery("SELECT max(`Groups`) FROM EventBlitzRewardItemInfo");

		pResult->next();
		m_maxGroup = pResult->getInt(1) + 1;

		m_EventBlitzRewardItemInfo = new vector<EVENT_ITEM_TEMPLATE*>[m_maxGroup];
		m_TotalRatio = new int[m_maxGroup];

		for(int group = 0; group < m_maxGroup; group++)
		{
			m_TotalRatio[group] = 0;
		

			// FAZ 4 - MySQL 8 fix: `Groups` reserved keyword.
			pResult = pStmt->executeQuery("SELECT Orders, ItemClass, ItemType, OptionType, LimitTime, ItemCount, Ratio FROM EventBlitzRewardItemInfo WHERE `Groups` = %d ORDER BY Orders ASC", group);

			int rowCount = pResult->getRowCount();
			if(rowCount <= 0)
			{
				SAFE_DELETE(pStmt);
				throw Error("EventBlitzRewardManager.cpp, EventBlitzRewardItemInfo ���� �ϳ��� �������� �ʽ��ϴ�");
			}

			m_EventBlitzRewardItemInfo[group].reserve(rowCount);
			while(pResult->next())
			{
				int i = 0;
				int order = pResult->getInt(++i);
				Item::ItemClass itemClass = (Item::ItemClass)pResult->getInt(++i);
				int itemType = pResult->getInt(++i);
				string option = pResult->getString(++i);
				int LimitTime = pResult->getInt(++i);
				int ItemCount = pResult->getInt(++i);
				int ratio = pResult->getInt(++i);
				
				EVENT_ITEM_TEMPLATE *pEventItemTemplate = new EVENT_ITEM_TEMPLATE;
												
				pEventItemTemplate->ItemClass = itemClass;
				pEventItemTemplate->ItemType = itemType;
				pEventItemTemplate->ItemOption = option;
				pEventItemTemplate->LimitTime = LimitTime;
				pEventItemTemplate->ItemCount = ItemCount;
				pEventItemTemplate->Ratio = ratio;
				
				m_EventBlitzRewardItemInfo[group].push_back(pEventItemTemplate);
								
				m_TotalRatio[group] += ratio;
				cout << "EventBlitzRewardItemInfo order : " << order << ", itemClass : " << itemClass << ", itemType : " << itemType << ", option : " << option << ", itemcount : " << ItemCount << ", Ratio : " << ratio << endl;
			}


		
		}

		for(int i = 0; i < m_maxGroup; i++)
		{
			cout << "Group " << i << " m_TotalRatio : " << m_TotalRatio[i] << endl;
			cout << "m_EventBlitzRewardItemInfo[" << i << "].size(): " << m_EventBlitzRewardItemInfo[i].size() << endl;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt);
	
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
EventBlitzRewardManager::~EventBlitzRewardManager()
throw(Error)
{
	for(int j = 0; j < m_maxGroup; j++)
	{
		for ( int i=0; i<m_EventBlitzRewardItemInfo[j].size(); ++i )
		{
			SAFE_DELETE( m_EventBlitzRewardItemInfo[j][i] );
		}
	}
	SAFE_DELETE_ARRAY(m_EventBlitzRewardItemInfo);
}

////////////////////////////////////////////////////////////////////////////////
// �׼��� �����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
Item* EventBlitzRewardManager::getEventBlitzReward(Creature * pCreature, int Group, int& limitTime, _TPOINT& pt) 
	throw (Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	int Sum = 0;
	int Ratio = 0;
	int array = 0;
	int LimitTime = 0;
	ItemInfo* pItemInfo = NULL;
	list<OptionType_t> options;

	Ratio = rand()%m_TotalRatio[Group] + 1;
	array = 0;
	Sum = 0;

	for( int i = 0; i < m_EventBlitzRewardItemInfo[Group].size(); ++i )
	{
		Sum += m_EventBlitzRewardItemInfo[Group][i]->Ratio;
		if( Sum >= Ratio )
		{
			array = i; 
			break;
		}
	}		
	pItemInfo = g_pItemInfoManager->getItemInfo( m_EventBlitzRewardItemInfo[Group][array]->ItemClass, m_EventBlitzRewardItemInfo[Group][array]->ItemType );
	Assert( pItemInfo != NULL );

	cout << "array : " << array << endl;

	if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
	{
		return NULL;
	}


	if(m_EventBlitzRewardItemInfo[Group][array]->ItemOption.size() > 0)
		makeOptionList( m_EventBlitzRewardItemInfo[Group][array]->ItemOption, options );
	
	LimitTime = m_EventBlitzRewardItemInfo[Group][array]->LimitTime;
	
	Item* pItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), pItemInfo->getItemType(), options );
	Assert( pItem != NULL );

	limitTime = LimitTime;
	pItem->setNum(m_EventBlitzRewardItemInfo[Group][array]->ItemCount);
	return pItem;
	__END_CATCH
}


// global variable declaration
EventBlitzRewardManager* g_pEventBlitzRewardManager = NULL;


