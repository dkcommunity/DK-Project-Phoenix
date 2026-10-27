///////////////////////////////////////////////////////////////////////////
// File : LevelWarItemManager.cpp
///////////////////////////////////////////////////////////////////////////

// include files
#include "LevelWarItemManager.h"
#include "ItemFactoryManager.h"
#include "ZoneUtil.h"
#include "Zone.h"
#include "GlobalItemPositionLoader.h"
#include "EffectVampireLevelWarHeroItem.h"
#include "EffectSlayerLevelWarHeroItem.h"
#include "EffectOustersLevelWarHeroItem.h"
#include "ShrineInfoManager.h"

#include <stdio.h>
#include "LevelWarManager.h"
const int nLeveWarItems = 3;

///////////////////////////////////////////////////////////////////////////
// class LevelWarItemManager
///////////////////////////////////////////////////////////////////////////
LevelWarItemManager::LevelWarItemManager()
{
	__BEGIN_TRY

	m_SlayerHeroItems.resize( nLeveWarItems, NULL );
	m_VampireHeroItems.resize( nLeveWarItems, NULL );
	m_OustersHeroItems.resize( nLeveWarItems, NULL );
	m_SlayerDefaultPositions.resize( nLeveWarItems );
	m_VampireDefaultPositions.resize( nLeveWarItems );
	m_OustersDefaultPositions.resize( nLeveWarItems );

	m_Level = 0;

	__END_CATCH
}

LevelWarItemManager::~LevelWarItemManager()
{
	__BEGIN_TRY
	__END_CATCH
}

void LevelWarItemManager::init(int level, LevelWarManager* pLevelWarManager)
throw(Error)
{
	__BEGIN_TRY
	m_pLeveWarManager = pLevelWarManager;
	m_Level = level;
	initDefaultPositions();
	createAllHeroItems();

	__END_CATCH
}

void LevelWarItemManager::initDefaultPositions()
throw(Error)
{
	__BEGIN_TRY

		// 전쟁 시작시 HeroItem 의 시작 위치 설정.

	if(m_Level == 0)
		throw ("LevelWarItemManager Error Level");

	switch(m_Level)
	{
	case 1:
		{
			m_SlayerDefaultPositions[0].set(   1131,  12, 12 );		
			m_SlayerDefaultPositions[1].set(   1131,  13, 13 );		
			m_SlayerDefaultPositions[2].set(   1131,  14, 14 );		

			m_VampireDefaultPositions[0].set(   1131, 115, 11 );	
			m_VampireDefaultPositions[1].set(   1131, 117, 11 );
			m_VampireDefaultPositions[2].set(   1131, 119, 11 );

			m_OustersDefaultPositions[0].set(   1131,  13, 108 );
			m_OustersDefaultPositions[1].set(   1131,  13, 110 );
			m_OustersDefaultPositions[2].set(   1131,  13, 112 );
		}
		break;
	case 2:
		{
			m_SlayerDefaultPositions[0].set(   1132,  12, 12 );		
			m_SlayerDefaultPositions[1].set(   1132,  13, 13 );		
			m_SlayerDefaultPositions[2].set(   1132,  14, 14 );		

			m_VampireDefaultPositions[0].set(   1132, 115, 11 );	
			m_VampireDefaultPositions[1].set(   1132, 117, 11 );
			m_VampireDefaultPositions[2].set(   1132, 119, 11 );

			m_OustersDefaultPositions[0].set(   1132,  13, 108 );
			m_OustersDefaultPositions[1].set(   1132,  13, 110 );
			m_OustersDefaultPositions[2].set(   1132,  13, 112 );
		}
		break;
	case 3:
		{
			m_SlayerDefaultPositions[0].set(   1133,  12, 12 );		
			m_SlayerDefaultPositions[1].set(   1133,  13, 13 );		
			m_SlayerDefaultPositions[2].set(   1133,  14, 14 );		

			m_VampireDefaultPositions[0].set(   1133, 115, 11 );	
			m_VampireDefaultPositions[1].set(   1133, 117, 11 );
			m_VampireDefaultPositions[2].set(   1133, 119, 11 );

			m_OustersDefaultPositions[0].set(   1133,  13, 108 );
			m_OustersDefaultPositions[1].set(   1133,  13, 110 );
			m_OustersDefaultPositions[2].set(   1133,  13, 112 );
		}
		break;
	case 4:
		{
			m_SlayerDefaultPositions[0].set(   1134,  12, 12 );		
			m_SlayerDefaultPositions[1].set(   1134,  13, 13 );		
			m_SlayerDefaultPositions[2].set(   1134,  14, 14 );		

			m_VampireDefaultPositions[0].set(   1134, 115, 11 );	
			m_VampireDefaultPositions[1].set(   1134, 117, 11 );
			m_VampireDefaultPositions[2].set(   1134, 119, 11 );

			m_OustersDefaultPositions[0].set(   1134,  13, 108 );
			m_OustersDefaultPositions[1].set(   1134,  13, 110 );
			m_OustersDefaultPositions[2].set(   1134,  13, 112 );
		}
		break;
	}



	__END_CATCH
}

void LevelWarItemManager::createAllHeroItems()
throw(Error)
{
	__BEGIN_TRY

		// 일단 Hero Item 을 생성한다.
		for ( int i=0; i<nLeveWarItems; ++i )
		{
			list<OptionType_t> optionNull;
			Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 4, optionNull );
			Assert( pItem != NULL );

			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			Assert( pWarItem != NULL );

			Zone* pZone = getZoneByZoneID( m_SlayerDefaultPositions[i].id );
			Assert( pZone != NULL );

			char strZoneID[10];
			sprintf( strZoneID, "%u", pZone->getZoneID() );

			pZone->registerObject( pWarItem );
			pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_SlayerDefaultPositions[i].x, m_SlayerDefaultPositions[i].y );

			pWarItem->setIndex( i );

			m_SlayerHeroItems[i] = pWarItem;
		}

		// 일단 Hero Item 을 생성한다.
		for ( int i=0; i<nLeveWarItems; ++i )
		{
			list<OptionType_t> optionNull;
			Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 5, optionNull );
			Assert( pItem != NULL );

			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			Assert( pWarItem != NULL );

			Zone* pZone = getZoneByZoneID( m_VampireDefaultPositions[i].id );
			Assert( pZone != NULL );

			char strZoneID[10];
			sprintf( strZoneID, "%u", pZone->getZoneID() );

			pZone->registerObject( pWarItem );
			pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_VampireDefaultPositions[i].x, m_VampireDefaultPositions[i].y );

			pWarItem->setIndex( i );

			m_VampireHeroItems[i] = pWarItem;
		}

		// 일단 Hero Item 을 생성한다.
		for ( int i=0; i<nLeveWarItems; ++i )
		{
			list<OptionType_t> optionNull;
			Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 6, optionNull );
			Assert( pItem != NULL );

			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			Assert( pWarItem != NULL );

			Zone* pZone = getZoneByZoneID( m_OustersDefaultPositions[i].id );
			Assert( pZone != NULL );

			char strZoneID[10];
			sprintf( strZoneID, "%u", pZone->getZoneID() );

			pZone->registerObject( pWarItem );
			pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_OustersDefaultPositions[i].x, m_OustersDefaultPositions[i].y );

			pWarItem->setIndex( i );

			m_OustersHeroItems[i] = pWarItem;
		}
		__END_CATCH
}

void LevelWarItemManager::addAllHeroItemsToZone()
throw(Error)
{
	__BEGIN_TRY

		// 아이템을 각자의 초기 위치에 놓는다.
		int nSlayerHeroItem = 3 - m_pLeveWarManager->getSweeperWinPoint(RACE_SLAYER);
	cout << "LevelWar nSlayerHeroItem : " << nSlayerHeroItem << endl;
		for ( int i=0; i<nSlayerHeroItem; ++i )
		{
			Zone* pZone = getZoneByZoneID( m_SlayerDefaultPositions[i].id );
			Assert( pZone != NULL );

			Item* pItem = m_SlayerHeroItems[i];
			Assert( pItem != NULL );

			// 존에 아이템 추가하기
			pZone->addItemDelayed( pItem, m_SlayerDefaultPositions[i].x, m_SlayerDefaultPositions[i].y );
			cout << "LevelWar SlayerHeroItem Count : " << i << endl;
		}

		int nVampireHeroItem = 3 - m_pLeveWarManager->getSweeperWinPoint(RACE_VAMPIRE);
		cout << "LevelWar nVampireHeroItem : " << nVampireHeroItem << endl;
		// 아이템을 각자의 초기 위치에 놓는다.
		for ( int i=0; i<nVampireHeroItem; ++i )
		{
			Zone* pZone = getZoneByZoneID( m_VampireDefaultPositions[i].id );
			Assert( pZone != NULL );

			Item* pItem = m_VampireHeroItems[i];
			Assert( pItem != NULL );

			// 존에 아이템 추가하기
			pZone->addItemDelayed( pItem, m_VampireDefaultPositions[i].x, m_VampireDefaultPositions[i].y );
			cout << "LevelWar VampireHeroItem Count : " << i << endl;
		}

		int nOustersHeroItem = 3 - m_pLeveWarManager->getSweeperWinPoint(RACE_OUSTERS);
		cout << "LevelWar nOustersHeroItem : " << nOustersHeroItem << endl;
		// 아이템을 각자의 초기 위치에 놓는다.
		for ( int i=0; i<nOustersHeroItem; ++i )
		{
			Zone* pZone = getZoneByZoneID( m_OustersDefaultPositions[i].id );
			Assert( pZone != NULL );

			Item* pItem = m_OustersHeroItems[i];
			Assert( pItem != NULL );

			// 존에 아이템 추가하기
			pZone->addItemDelayed( pItem, m_OustersDefaultPositions[i].x, m_OustersDefaultPositions[i].y );
			cout << "LevelWar OustersHeroItem Count : " << i << endl;
		}

		__END_CATCH
}

void LevelWarItemManager::removeAllHeroItems()
throw(Error)
{
	__BEGIN_TRY

		// 존재하는 모든 드래곤 아이이를 볼 수 없도록 없앤다.
		for ( int i=0; i<nLeveWarItems; ++i )
		{
			Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
			ItemID_t		ItemID		= m_SlayerHeroItems[i]->getItemID();

			GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

			if ( pItemPosition == NULL )
				return;

			// War 쓰레드에서 호출 되므로 락 걸어야 된다.
			pItemPosition->popItem( false );
		}

		// 존재하는 모든 드래곤 아이이를 볼 수 없도록 없앤다.
		for ( int i=0; i<nLeveWarItems; ++i )
		{
			Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
			ItemID_t		ItemID		= m_VampireHeroItems[i]->getItemID();

			GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

			if ( pItemPosition == NULL )
				return;

			// War 쓰레드에서 호출 되므로 락 걸어야 된다.
			pItemPosition->popItem( false );
		}

		// 존재하는 모든 드래곤 아이이를 볼 수 없도록 없앤다.
		for ( int i=0; i<nLeveWarItems; ++i )
		{
			Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
			ItemID_t		ItemID		= m_OustersHeroItems[i]->getItemID();

			GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

			if ( pItemPosition == NULL )
				return;

			// War 쓰레드에서 호출 되므로 락 걸어야 된다.
			pItemPosition->popItem( false );
		}


		__END_CATCH
}

void LevelWarItemManager::warpToDefaultPosition( Creature* pCreature )
throw(Error)
{
	__BEGIN_TRY

		// pCreature 는 드래곤 아이를 가지고 있는 크리쳐이다.
		// 드래곤 아이를 가지고 있던 사용자가 죽거나 나가거나 할때 불린다.
		// 드래곤 아이를 원래 위치로 이동시키는 코드다.
		// GlobalItemPositionLoader 를 사용해서 아이템을 원래 위치에서 제거한다.
		// 원래 존의 포인터를 가져와서 아이템을 넣는다.
		// 이 함수는 사용자가 어떤 행동을 할때 불리는 코드이므로 ZoneGroup 에서 호출되는 코드이다.
		// 그래서 존의 포인터로 작업을 할 때 ZoneGroup 을 비교해보고 작업을 한다.
	if(pCreature->isSlayer())
	{
		EffectSlayerLevelWarHeroItem* pEffect = dynamic_cast<EffectSlayerLevelWarHeroItem*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM ));
		Assert( pEffect != NULL );

		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= pEffect->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		Item* pItem = pItemPosition->popItem( false );

		if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM && pItem->getItemType() == 4)
		{
			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			Assert( pWarItem != NULL );

			int index = pWarItem->getIndex();

			Zone* pZone = pCreature->getZone();
			Assert( pZone != NULL );

			Zone* pTargetZone = getZoneByZoneID( m_SlayerDefaultPositions[index].id );
			Assert( pTargetZone != NULL );

			if ( pZone->getZoneGroup() == pTargetZone->getZoneGroup() )
				pTargetZone->addItem( pItem, m_SlayerDefaultPositions[index].x, m_SlayerDefaultPositions[index].y );
			else
				pTargetZone->addItemDelayed( pItem, m_SlayerDefaultPositions[index].x, m_SlayerDefaultPositions[index].y );
		}
	}
	else if(pCreature->isVampire())
	{
		EffectVampireLevelWarHeroItem* pEffect = dynamic_cast<EffectVampireLevelWarHeroItem*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM ));

		Assert( pEffect != NULL );

		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= pEffect->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		Item* pItem = pItemPosition->popItem( false );

		if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM && pItem->getItemType() == 5)
		{
			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			Assert( pWarItem != NULL );

			int index = pWarItem->getIndex();

			Zone* pZone = pCreature->getZone();
			Assert( pZone != NULL );

			Zone* pTargetZone = getZoneByZoneID( m_VampireDefaultPositions[index].id );
			Assert( pTargetZone != NULL );

			if ( pZone->getZoneGroup() == pTargetZone->getZoneGroup() )
				pTargetZone->addItem( pItem, m_VampireDefaultPositions[index].x, m_VampireDefaultPositions[index].y );
			else
				pTargetZone->addItemDelayed( pItem, m_VampireDefaultPositions[index].x, m_VampireDefaultPositions[index].y );
		}
	}
	else if(pCreature->isOusters())
	{
		EffectOustersLevelWarHeroItem* pEffect = dynamic_cast<EffectOustersLevelWarHeroItem*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM ));

		Assert( pEffect != NULL );

		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= pEffect->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		Item* pItem = pItemPosition->popItem( false );

		if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM && pItem->getItemType() == 6)
		{
			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			Assert( pWarItem != NULL );

			int index = pWarItem->getIndex();

			Zone* pZone = pCreature->getZone();
			Assert( pZone != NULL );

			Zone* pTargetZone = getZoneByZoneID( m_OustersDefaultPositions[index].id );
			Assert( pTargetZone != NULL );

			if ( pZone->getZoneGroup() == pTargetZone->getZoneGroup() )
				pTargetZone->addItem( pItem, m_OustersDefaultPositions[index].x, m_OustersDefaultPositions[index].y );
			else
				pTargetZone->addItemDelayed( pItem, m_OustersDefaultPositions[index].x, m_OustersDefaultPositions[index].y );
		}
	}

	__END_CATCH
}


