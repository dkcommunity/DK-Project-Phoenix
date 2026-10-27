///////////////////////////////////////////////////////////////////////////
// File : HeroItemManager.cpp
///////////////////////////////////////////////////////////////////////////

// include files
#include "HeroItemManager.h"
#include "ItemFactoryManager.h"
#include "ZoneUtil.h"
#include "Zone.h"
#include "GlobalItemPositionLoader.h"
#include "EffectVampireHeroItem.h"
#include "EffectSlayerHeroItem.h"
#include "EffectOustersHeroItem.h"
#include "ShrineInfoManager.h"

#include <stdio.h>

const int nHeroItems = 12;

///////////////////////////////////////////////////////////////////////////
// class HeroItemManager
///////////////////////////////////////////////////////////////////////////
HeroItemManager::HeroItemManager()
{
	__BEGIN_TRY

	m_SlayerHeroItems.reserve( nHeroItems );
	m_VampireHeroItems.reserve( nHeroItems );
	m_OustersHeroItems.reserve( nHeroItems );
	m_SlayerDefaultPositions.reserve( nHeroItems );
	m_VampireDefaultPositions.reserve( nHeroItems );
	m_OustersDefaultPositions.reserve( nHeroItems );

	initDefaultPositions();

	__END_CATCH
}

HeroItemManager::~HeroItemManager()
{
	__BEGIN_TRY
	__END_CATCH
}

void HeroItemManager::init()
throw(Error)
{
	__BEGIN_TRY

		createAllHeroItems();

	__END_CATCH
}

void HeroItemManager::initDefaultPositions()
throw(Error)
{
	__BEGIN_TRY

		// ���� ���۽� HeroItem �� ���� ��ġ ����.
	m_SlayerDefaultPositions[0].set(   73,  38, 122 );		
	m_SlayerDefaultPositions[1].set(   73,  38, 124 );		// �����̾� / �ƴ㼭 / 28 / 124
	m_SlayerDefaultPositions[2].set(   73,  38, 126 );		// �����̾� / �ƴ㼭 / 20 / 124
	m_SlayerDefaultPositions[3].set(   73,  38, 128 );		// �����̾� / �ƴ㼭 / 24 / 118
	m_SlayerDefaultPositions[4].set( 1201,  60,  68 );		// �����̾� / ��Ÿ�ν� / 60 / 68
	m_SlayerDefaultPositions[5].set( 1201,  64,  68 );		// �����̾� / ��Ÿ�ν� / 64 / 68
	m_SlayerDefaultPositions[6].set( 1201,  62,  65 );		// �����̾� / ��Ÿ�ν� / 62 / 65
	m_SlayerDefaultPositions[7].set( 1201,  62,  71 );		// �����̾� / ��Ÿ�ν� / 62 / 71
	m_SlayerDefaultPositions[8].set( 1203,  83,  50 );		// �����̾� / ��Ƽ���� / 83 / 50
	m_SlayerDefaultPositions[9].set( 1203,  85,  47 );		// �����̾� / ��Ƽ���� / 85 / 47
	m_SlayerDefaultPositions[10].set( 1203,  85,  53 );		// �����̾� / ��Ƽ���� / 85 / 53
	m_SlayerDefaultPositions[11].set( 1203,  87,  50 );		// �����̾� / ��Ƽ���� / 87 / 50

	m_VampireDefaultPositions[0].set(   71, 97, 126 );		// �����̾� / �ƴ㵿 / 111 / 133
	m_VampireDefaultPositions[1].set(   71, 97, 128 );		// �����̾� / �ƴ㵿 / 111 / 136
	m_VampireDefaultPositions[2].set(   71, 97, 130 );		// �����̾� / �ƴ㵿 / 106 / 128
	m_VampireDefaultPositions[3].set(   71, 97, 132 );		// �����̾� / �ƴ㵿 / 115 / 128
	m_VampireDefaultPositions[4].set( 1204,  60,  69 );		// �����̾� / ���Ƹ����� / 60 / 69
	m_VampireDefaultPositions[5].set( 1204,  62,  66 );		// �����̾� / ���Ƹ����� / 62 / 66
	m_VampireDefaultPositions[6].set( 1204,  62,  72 );		// �����̾� / ���Ƹ����� / 62 / 72
	m_VampireDefaultPositions[7].set( 1204,  64,  69 );		// �����̾� / ���Ƹ����� / 64 / 69
	m_VampireDefaultPositions[8].set( 1202,  83,  52 );		// �����̾� / �׸�Ƽ�콺 / 83 / 52
	m_VampireDefaultPositions[9].set( 1202,  79,  52 );		// �����̾� / �׸�Ƽ�콺 / 79 / 52
	m_VampireDefaultPositions[10].set( 1202,  81,  49 );		// �����̾� / �׸�Ƽ�콺 / 81 / 49
	m_VampireDefaultPositions[11].set( 1202,  81,  55 );		// �����̾� / �׸�Ƽ�콺 / 81 / 55

	m_OustersDefaultPositions[0].set(   72,  65, 162 );		// �ƿ콺���� / �ƴ� / 66 / 176
	m_OustersDefaultPositions[1].set(   72,  66, 162 );		// �ƿ콺���� / �ƴ� / 62 / 172
	m_OustersDefaultPositions[2].set(   72,  67, 162 );		// �ƿ콺���� / �ƴ� / 66 / 179
	m_OustersDefaultPositions[3].set(   72,  68, 162 );		// �ƿ콺���� / �ƴ� / 70 / 172
	m_OustersDefaultPositions[4].set( 1205,  60,  69 );		// �ƿ콺���� / ��Ÿ���� / 60 / 69
	m_OustersDefaultPositions[5].set( 1205,  62,  66 );		// �ƿ콺���� / ��Ÿ���� / 62 / 66
	m_OustersDefaultPositions[6].set( 1205,  62,  71 );		// �ƿ콺���� / ��Ÿ���� / 62 / 71
	m_OustersDefaultPositions[7].set( 1205,  64,  69 );		// �ƿ콺���� / ��Ÿ���� / 64 / 69
	m_OustersDefaultPositions[8].set( 1206,  82,  54 );		// �ƿ콺���� / ���ø��콺 / 82 / 54
	m_OustersDefaultPositions[9].set( 1206,  82,  55 );		// �ƿ콺���� / ���ø��콺 / 82 / 55
	m_OustersDefaultPositions[10].set( 1206,  80,  52 );		// �ƿ콺���� / ���ø��콺 / 80 / 52
	m_OustersDefaultPositions[11].set( 1206,  84,  52 );		// �ƿ콺���� / ���ø��콺 / 84 / 52

	__END_CATCH
}

void HeroItemManager::createAllHeroItems()
throw(Error)
{
	__BEGIN_TRY

		// FAZ 4 - Tek hero item hatasi tum init'i kapatmasin: try/catch + zone null check.
		for ( int i=0; i<nHeroItems; ++i )
		{
			try {
				Zone* pZone = getZoneByZoneID( m_SlayerDefaultPositions[i].id );
				if (pZone == NULL) { m_SlayerHeroItems[i] = NULL; continue; }

				list<OptionType_t> optionNull;
				Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 1, optionNull );
				if (pItem == NULL) { m_SlayerHeroItems[i] = NULL; continue; }
				WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
				if (pWarItem == NULL) { m_SlayerHeroItems[i] = NULL; continue; }

				char strZoneID[10];
				sprintf( strZoneID, "%u", pZone->getZoneID() );
				pZone->registerObject( pWarItem );
				pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_SlayerDefaultPositions[i].x, m_SlayerDefaultPositions[i].y );
				pWarItem->setIndex( i );
				m_SlayerHeroItems[i] = pWarItem;
			} catch (...) {
				cout << "[Warn] SlayerHeroItem[" << i << "] creation failed — skip" << endl;
				m_SlayerHeroItems[i] = NULL;
			}
		}

		for ( int i=0; i<nHeroItems; ++i )
		{
			try {
				Zone* pZone = getZoneByZoneID( m_VampireDefaultPositions[i].id );
				if (pZone == NULL) { m_VampireHeroItems[i] = NULL; continue; }

				list<OptionType_t> optionNull;
				Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 2, optionNull );
				if (pItem == NULL) { m_VampireHeroItems[i] = NULL; continue; }
				WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
				if (pWarItem == NULL) { m_VampireHeroItems[i] = NULL; continue; }

				char strZoneID[10];
				sprintf( strZoneID, "%u", pZone->getZoneID() );
				pZone->registerObject( pWarItem );
				pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_VampireDefaultPositions[i].x, m_VampireDefaultPositions[i].y );
				pWarItem->setIndex( i );
				m_VampireHeroItems[i] = pWarItem;
			} catch (...) {
				cout << "[Warn] VampireHeroItem[" << i << "] creation failed — skip" << endl;
				m_VampireHeroItems[i] = NULL;
			}
		}

		for ( int i=0; i<nHeroItems; ++i )
		{
			try {
				Zone* pZone = getZoneByZoneID( m_OustersDefaultPositions[i].id );
				if (pZone == NULL) { m_OustersHeroItems[i] = NULL; continue; }

				list<OptionType_t> optionNull;
				Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 3, optionNull );
				if (pItem == NULL) { m_OustersHeroItems[i] = NULL; continue; }
				WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
				if (pWarItem == NULL) { m_OustersHeroItems[i] = NULL; continue; }

				char strZoneID[10];
				sprintf( strZoneID, "%u", pZone->getZoneID() );
				pZone->registerObject( pWarItem );
				pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_OustersDefaultPositions[i].x, m_OustersDefaultPositions[i].y );
				pWarItem->setIndex( i );
				m_OustersHeroItems[i] = pWarItem;
			} catch (...) {
				cout << "[Warn] OustersHeroItem[" << i << "] creation failed — skip" << endl;
				m_OustersHeroItems[i] = NULL;
			}
		}
		__END_CATCH
}

void HeroItemManager::addAllHeroItemsToZone()
throw(Error)
{
	__BEGIN_TRY

		// �������� ������ �ʱ� ��ġ�� ���´�.
		int nSlayerHeroItem = 12 - g_pShrineInfoManager->getBloodBibleCount(RACE_SLAYER);
	cout << "nSlayerHeroItem : " << nSlayerHeroItem << endl;
		for ( int i=0; i<nSlayerHeroItem; ++i )
		{
			Zone* pZone = getZoneByZoneID( m_SlayerDefaultPositions[i].id );
			Assert( pZone != NULL );

			Item* pItem = m_SlayerHeroItems[i];
			Assert( pItem != NULL );

			// ���� ������ �߰��ϱ�
			pZone->addItemDelayed( pItem, m_SlayerDefaultPositions[i].x, m_SlayerDefaultPositions[i].y );
			cout << "SlayerHeroItem Count : " << i << endl;
		}

		int nVampireHeroItem = 12 - g_pShrineInfoManager->getBloodBibleCount(RACE_VAMPIRE);
		cout << "nVampireHeroItem : " << nVampireHeroItem << endl;
		// �������� ������ �ʱ� ��ġ�� ���´�.
		for ( int i=0; i<nVampireHeroItem; ++i )
		{
			Zone* pZone = getZoneByZoneID( m_VampireDefaultPositions[i].id );
			Assert( pZone != NULL );

			Item* pItem = m_VampireHeroItems[i];
			Assert( pItem != NULL );

			// ���� ������ �߰��ϱ�
			pZone->addItemDelayed( pItem, m_VampireDefaultPositions[i].x, m_VampireDefaultPositions[i].y );
			cout << "VampireHeroItem Count : " << i << endl;
		}

		int nOustersHeroItem = 12 - g_pShrineInfoManager->getBloodBibleCount(RACE_OUSTERS);
		cout << "nOustersHeroItem : " << nOustersHeroItem << endl;
		// �������� ������ �ʱ� ��ġ�� ���´�.
		for ( int i=0; i<nOustersHeroItem; ++i )
		{
			Zone* pZone = getZoneByZoneID( m_OustersDefaultPositions[i].id );
			Assert( pZone != NULL );

			Item* pItem = m_OustersHeroItems[i];
			Assert( pItem != NULL );

			// ���� ������ �߰��ϱ�
			pZone->addItemDelayed( pItem, m_OustersDefaultPositions[i].x, m_OustersDefaultPositions[i].y );
			cout << "OustersHeroItem Count : " << i << endl;
		}

		__END_CATCH
}

void HeroItemManager::removeAllHeroItems()
throw(Error)
{
	__BEGIN_TRY

		// �����ϴ� ��� �巡�� �����̸� �� �� ������ ���ش�.
		for ( int i=0; i<nHeroItems; ++i )
		{
			Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
			ItemID_t		ItemID		= m_SlayerHeroItems[i]->getItemID();

			GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

			if ( pItemPosition == NULL )
				return;

			// War �����忡�� ȣ�� �ǹǷ� �� �ɾ�� �ȴ�.
			pItemPosition->popItem( true );
		}

		// �����ϴ� ��� �巡�� �����̸� �� �� ������ ���ش�.
		for ( int i=0; i<nHeroItems; ++i )
		{
			Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
			ItemID_t		ItemID		= m_VampireHeroItems[i]->getItemID();

			GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

			if ( pItemPosition == NULL )
				return;

			// War �����忡�� ȣ�� �ǹǷ� �� �ɾ�� �ȴ�.
			pItemPosition->popItem( true );
		}

		// �����ϴ� ��� �巡�� �����̸� �� �� ������ ���ش�.
		for ( int i=0; i<nHeroItems; ++i )
		{
			Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
			ItemID_t		ItemID		= m_OustersHeroItems[i]->getItemID();

			GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

			if ( pItemPosition == NULL )
				return;

			// War �����忡�� ȣ�� �ǹǷ� �� �ɾ�� �ȴ�.
			pItemPosition->popItem( true );
		}


		__END_CATCH
}

void HeroItemManager::warpToDefaultPosition( Creature* pCreature )
throw(Error)
{
	__BEGIN_TRY

		// pCreature �� �巡�� ���̸� ������ �ִ� ũ�����̴�.
		// �巡�� ���̸� ������ �ִ� ����ڰ� �װų� �����ų� �Ҷ� �Ҹ���.
		// �巡�� ���̸� ���� ��ġ�� �̵���Ű�� �ڵ��.
		// GlobalItemPositionLoader �� ����ؼ� �������� ���� ��ġ���� �����Ѵ�.
		// ���� ���� �����͸� �����ͼ� �������� �ִ´�.
		// �� �Լ��� ����ڰ� � �ൿ�� �Ҷ� �Ҹ��� �ڵ��̹Ƿ� ZoneGroup ���� ȣ��Ǵ� �ڵ��̴�.
		// �׷��� ���� �����ͷ� �۾��� �� �� ZoneGroup �� ���غ��� �۾��� �Ѵ�.
	if(pCreature->isSlayer())
	{
		EffectSlayerHeroItem* pEffect = dynamic_cast<EffectSlayerHeroItem*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_SLAYER_HERO_ITEM ));
		Assert( pEffect != NULL );

		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= pEffect->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		Item* pItem = pItemPosition->popItem( false );

		if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM && pItem->getItemType() == 1)
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
		EffectVampireHeroItem* pEffect = dynamic_cast<EffectVampireHeroItem*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM ));

		Assert( pEffect != NULL );

		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= pEffect->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		Item* pItem = pItemPosition->popItem( false );

		if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM && pItem->getItemType() == 2)
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
		EffectOustersHeroItem* pEffect = dynamic_cast<EffectOustersHeroItem*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM ));

		Assert( pEffect != NULL );

		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= pEffect->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		Item* pItem = pItemPosition->popItem( false );

		if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM && pItem->getItemType() == 3)
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

HeroItemManager* g_pHeroItemManager = NULL;
