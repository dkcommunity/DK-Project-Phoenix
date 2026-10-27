///////////////////////////////////////////////////////////////////////////
// File : DragonEyeManager.cpp
///////////////////////////////////////////////////////////////////////////

// include files
#include "DragonEyeManager.h"
#include "ItemFactoryManager.h"
#include "ZoneUtil.h"
#include "Zone.h"
#include "GlobalItemPositionLoader.h"
#include "EffectDragonEye.h"

#include <stdio.h>

const int nDragonEyes = 9;

///////////////////////////////////////////////////////////////////////////
// class DragonEyeManager
///////////////////////////////////////////////////////////////////////////
DragonEyeManager::DragonEyeManager()
{
	__BEGIN_TRY

	m_DragonEyes.reserve( nDragonEyes );
	m_DefaultPositions.reserve( nDragonEyes );

	initDefaultPositions();

	__END_CATCH
}

DragonEyeManager::~DragonEyeManager()
{
	__BEGIN_TRY
	__END_CATCH
}

void DragonEyeManager::init()
	throw(Error)
{
	__BEGIN_TRY

	createAllDragonEyes();

	__END_CATCH
}

void DragonEyeManager::initDefaultPositions()
	throw(Error)
{
	__BEGIN_TRY

	// ���� ���۽� DragonEye �� ���� ��ġ ����.
	m_DefaultPositions[0].set(   73,  24, 121 );		// �����̾� / �ƴ㼭 / 24 / 121
	m_DefaultPositions[1].set( 1201,  62,  68 );		// �����̾� / ��Ÿ�ν� / 62 / 68
	m_DefaultPositions[2].set( 1203,  85,  50 );		// �����̾� / ��Ƽ���� / 85 / 50
	m_DefaultPositions[3].set(   71, 111, 124 );		// �����̾� / �ƴ㵿 / 111 / 124
	m_DefaultPositions[4].set( 1204,  62,  69 );		// �����̾� / ���Ƹ����� / 62 / 69
	m_DefaultPositions[5].set( 1202,  81,  52 );		// �����̾� / �׸�Ƽ�콺 / 81 / 52 
	m_DefaultPositions[6].set(   72,  66, 168 );		// �ƿ콺���� / �ƴ� / 66 / 168
	m_DefaultPositions[7].set( 1205,  62,  69 );		// �ƿ콺���� / ��Ÿ���� / 62 / 69
	m_DefaultPositions[8].set( 1206,  82,  52 );		// �ƿ콺���� / ���ø��콺 / 82 / 52

	__END_CATCH
}

void DragonEyeManager::createAllDragonEyes()
	throw(Error)
{
	__BEGIN_TRY

	// �ϴ� �巡�� ���� �������� �����Ѵ�.
	for ( int i=0; i<nDragonEyes; ++i )
	{
		// FAZ 4 - Tek dragon eye hatasi tum init'i kapatmasin: try/catch + zone null check.
		try {
			Zone* pZone = getZoneByZoneID( m_DefaultPositions[i].id );
			if (pZone == NULL)
			{
				cout << "[Warn] DragonEye[" << i << "] zoneID=" << (int)m_DefaultPositions[i].id
				     << " not loaded — skip" << endl;
				m_DragonEyes[i] = NULL;
				continue;
			}

			list<OptionType_t> optionNull;
			Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_WAR_ITEM, 0, optionNull );
			if (pItem == NULL) { m_DragonEyes[i] = NULL; continue; }

			WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
			if (pWarItem == NULL) { m_DragonEyes[i] = NULL; continue; }

			char strZoneID[10];
			sprintf( strZoneID, "%u", pZone->getZoneID() );

			pZone->registerObject( pWarItem );
			pWarItem->create( strZoneID, STORAGE_ZONE, pZone->getZoneID(), m_DefaultPositions[i].x, m_DefaultPositions[i].y );

			pWarItem->setIndex( i );

			m_DragonEyes[i] = pWarItem;
		} catch (...) {
			cout << "[Warn] DragonEye[" << i << "] creation failed — skip" << endl;
			m_DragonEyes[i] = NULL;
		}
	}

	__END_CATCH
}

void DragonEyeManager::addAllDragonEyesToZone()
	throw(Error)
{
	__BEGIN_TRY

	// �������� ������ �ʱ� ��ġ�� ���´�.
	for ( int i=0; i<nDragonEyes; ++i )
	{
		Zone* pZone = getZoneByZoneID( m_DefaultPositions[i].id );
		Assert( pZone != NULL );

		Item* pItem = m_DragonEyes[i];
		Assert( pItem != NULL );

		// ���� ������ �߰��ϱ�
		pZone->addItemDelayed( pItem, m_DefaultPositions[i].x, m_DefaultPositions[i].y );
	}

	__END_CATCH
}

void DragonEyeManager::removeAllDragonEyes()
	throw(Error)
{
	__BEGIN_TRY

	// �����ϴ� ��� �巡�� �����̸� �� �� ������ ���ش�.
	for ( int i=0; i<nDragonEyes; ++i )
	{
		Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
		ItemID_t		ItemID		= m_DragonEyes[i]->getItemID();

		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

		if ( pItemPosition == NULL )
			return;

		// War �����忡�� ȣ�� �ǹǷ� �� �ɾ�� �ȴ�.
		pItemPosition->popItem( true );
	}

	__END_CATCH
}

void DragonEyeManager::warpToDefaultPosition( Creature* pCreature )
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
	EffectDragonEye* pEffect = dynamic_cast<EffectDragonEye*>(pCreature->getEffectManager()->findEffect( Effect::EFFECT_CLASS_DRAGON_EYE ));
	Assert( pEffect != NULL );

	Item::ItemClass ItemClass	= Item::ITEM_CLASS_WAR_ITEM;
	ItemID_t		ItemID		= pEffect->getItemID();

	GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( ItemClass, ItemID );

	if ( pItemPosition == NULL )
		return;

	Item* pItem = pItemPosition->popItem( false );

	if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_WAR_ITEM )
	{
		WarItem* pWarItem = dynamic_cast<WarItem*>(pItem);
		Assert( pWarItem != NULL );

		int index = pWarItem->getIndex();

		Zone* pZone = pCreature->getZone();
		Assert( pZone != NULL );

		Zone* pTargetZone = getZoneByZoneID( m_DefaultPositions[index].id );
		Assert( pTargetZone != NULL );

		if ( pZone->getZoneGroup() == pTargetZone->getZoneGroup() )
			pTargetZone->addItem( pItem, m_DefaultPositions[index].x, m_DefaultPositions[index].y );
		else
			pTargetZone->addItemDelayed( pItem, m_DefaultPositions[index].x, m_DefaultPositions[index].y );
	}

	__END_CATCH
}

DragonEyeManager* g_pDragonEyeManager = NULL;
