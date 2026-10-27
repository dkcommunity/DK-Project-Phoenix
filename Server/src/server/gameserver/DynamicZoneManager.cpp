/////////////////////////////////////////////////////////////////////////////
// DynamicZoneManager.cpp
// 서버에서 생겼다 사라졌다하는 존을 관리하는 넘
/////////////////////////////////////////////////////////////////////////////

// include files
#include "DynamicZoneManager.h"
#include "DynamicZoneGroup.h"
#include "DynamicZoneScriptGroup.h"
#include "DynamicZoneInfo.h"
#include "Assert.h"

// global variable
DynamicZoneManager* g_pDynamicZoneManager = NULL;

// 새로 만들기 시작할 DynamicZoneID 시작 위치
// 만들때마다 1 씩 증가
const ZoneID_t StartDynamicZoneID = 15001;

// constructor
DynamicZoneManager::DynamicZoneManager()
	: m_DynamicZoneID( StartDynamicZoneID )
{
	m_Mutex.setName( "DynamicZoneManager" );
}

// destructor
DynamicZoneManager::~DynamicZoneManager()
{
	clear();
}

void DynamicZoneManager::init()
{
	// DynamicZoneGroup 추가. 수동
	
	{
		// 제단 입구
		DynamicZoneGroup* pDynamicZoneGroup = new DynamicZoneGroup();
		pDynamicZoneGroup->setDynamicZoneType( DYNAMIC_ZONE_GATE_OF_ALTER );
		pDynamicZoneGroup->setTemplateZoneID( g_pDynamicZoneInfoManager->getDynamicZoneInfo( DYNAMIC_ZONE_GATE_OF_ALTER )->getTemplateZoneID() );

		addDynamicZoneGroup( pDynamicZoneGroup );
	}

	{
		// 피의 제단
		DynamicZoneGroup* pDynamicZoneGroup = new DynamicZoneGroup();
		pDynamicZoneGroup->setDynamicZoneType( DYNAMIC_ZONE_ALTER_OF_BLOOD );
		pDynamicZoneGroup->setTemplateZoneID( g_pDynamicZoneInfoManager->getDynamicZoneInfo( DYNAMIC_ZONE_ALTER_OF_BLOOD )->getTemplateZoneID() );

		addDynamicZoneGroup( pDynamicZoneGroup );
	}

	{
		// 슬레이어 심연의 거울
		DynamicZoneGroup* pDynamicZoneGroup = new DynamicZoneGroup();
		pDynamicZoneGroup->setDynamicZoneType( DYNAMIC_ZONE_SLAYER_MIRROR_OF_ABYSS );
		pDynamicZoneGroup->setTemplateZoneID( g_pDynamicZoneInfoManager->getDynamicZoneInfo( DYNAMIC_ZONE_SLAYER_MIRROR_OF_ABYSS )->getTemplateZoneID() );

		addDynamicZoneGroup( pDynamicZoneGroup );
	}

	{
		// 뱀파이어 심연의 거울
		DynamicZoneGroup* pDynamicZoneGroup = new DynamicZoneGroup();
		pDynamicZoneGroup->setDynamicZoneType( DYNAMIC_ZONE_VAMPIRE_MIRROR_OF_ABYSS );
		pDynamicZoneGroup->setTemplateZoneID( g_pDynamicZoneInfoManager->getDynamicZoneInfo( DYNAMIC_ZONE_VAMPIRE_MIRROR_OF_ABYSS )->getTemplateZoneID() );

		addDynamicZoneGroup( pDynamicZoneGroup );
	}

	{
		// 아우스터즈 심연의 거울
		DynamicZoneGroup* pDynamicZoneGroup = new DynamicZoneGroup();
		pDynamicZoneGroup->setDynamicZoneType( DYNAMIC_ZONE_OUSTERS_MIRROR_OF_ABYSS );
		pDynamicZoneGroup->setTemplateZoneID( g_pDynamicZoneInfoManager->getDynamicZoneInfo( DYNAMIC_ZONE_OUSTERS_MIRROR_OF_ABYSS )->getTemplateZoneID() );

		addDynamicZoneGroup( pDynamicZoneGroup );
	}

	{
		DynamicZoneScriptGroup* pDynamicZoneScriptGroup = new DynamicZoneScriptGroup();
		pDynamicZoneScriptGroup->setDynamicZoneType( DYNAMIC_ZONE_SCRIPT );

		addDynamicZoneGroup( pDynamicZoneScriptGroup );
	}

	{
		DynamicZoneGroup* pDynamicZoneGroup = new DynamicZoneGroup();
		pDynamicZoneGroup->setDynamicZoneType( DYNAMIC_ZONE_MOBA );
		pDynamicZoneGroup->setTemplateZoneID( g_pDynamicZoneInfoManager->getDynamicZoneInfo( DYNAMIC_ZONE_MOBA )->getTemplateZoneID() );

		addDynamicZoneGroup( pDynamicZoneGroup );
	}

}

void DynamicZoneManager::clear()
{
	HashMapDynamicZoneGroupItor itr = m_DynamicZoneGroups.begin();
	HashMapDynamicZoneGroupItor endItr = m_DynamicZoneGroups.end();

	for ( ; itr != endItr; ++itr )
	{
		SAFE_DELETE( itr->second );
	}

	m_DynamicZoneGroups.clear();
}

void DynamicZoneManager::addDynamicZoneGroup( DynamicZoneGroup* pDynamicZoneGroup )
{
	Assert( pDynamicZoneGroup != NULL );

	HashMapDynamicZoneGroupItor itr = m_DynamicZoneGroups.find( pDynamicZoneGroup->getDynamicZoneType() );

	if ( itr != m_DynamicZoneGroups.end() )
	{
		cerr << "Duplicated DynamicZoneGroup. DynamicZoneManager::addDynamicZoneGroup" << endl;
		Assert( false );
	}

	m_DynamicZoneGroups[ pDynamicZoneGroup->getDynamicZoneType() ] = pDynamicZoneGroup;
}

DynamicZoneGroup* DynamicZoneManager::getDynamicZoneGroup( int dynamicZoneType )
{
	HashMapDynamicZoneGroupItor itr = m_DynamicZoneGroups.find( dynamicZoneType );

	if ( itr == m_DynamicZoneGroups.end() )
	{
		return NULL;
	}

	return itr->second;
}

ZoneID_t DynamicZoneManager::getNewDynamicZoneID()
{
	ZoneID_t zoneID = 0;

	__ENTER_CRITICAL_SECTION( m_Mutex )

	if ( m_UsedDynamicZoneIDList.size() > 0 )
	{
		zoneID = m_UsedDynamicZoneIDList.front(); 
		m_UsedDynamicZoneIDList.pop_front(); 
	}
	else
	{
		zoneID = m_DynamicZoneID++;
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	return zoneID;
}

void DynamicZoneManager::returnDynamicZoneID(ZoneID_t ZoneID)
{
	__ENTER_CRITICAL_SECTION( m_Mutex )
	
	m_UsedDynamicZoneIDList.push_back(ZoneID);

	__LEAVE_CRITICAL_SECTION( m_Mutex )
}

bool DynamicZoneManager::isDynamicZone( ZoneID_t zoneID )
{
	return zoneID >= StartDynamicZoneID;
}

