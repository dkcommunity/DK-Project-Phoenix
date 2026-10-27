/////////////////////////////////////////////////////////////////////////////
// DynamicZone.cpp
/////////////////////////////////////////////////////////////////////////////

// include files
#include "DynamicZone.h"
#include "DynamicZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZoneGroupManager.h"
#include <stdio.h>

DynamicZone::DynamicZone()
{
	m_pDynamicZoneGroup = NULL;
	m_pZone = NULL;

	m_StartPosX = 15;
	m_StartPosY = 15;

	m_bDeleteFlag = false;
}

DynamicZone::~DynamicZone()
{
	char strLog[256];
	sprintf(strLog, 
		"DynamicZone : remove, ZoneID=%d, TemplateZoneID=%d",
		m_ZoneID, m_TemplateZoneID
	);	
	filelog("CreateDynamicZone.log", strLog);
}

void DynamicZone::makeDynamicZone()
{

	ZoneInfo* pTemplateZoneInfo = g_pZoneInfoManager->getZoneInfo( m_TemplateZoneID );
	Assert( pTemplateZoneInfo != NULL );

	char temp[128];

	// make zone info
	ZoneInfo* pZoneInfo = new ZoneInfo();
	pZoneInfo->setZoneID( m_ZoneID );
	pZoneInfo->setZoneGroupID( pTemplateZoneInfo->getZoneGroupID() );
	pZoneInfo->setZoneType( pTemplateZoneInfo->getZoneType() );
	pZoneInfo->setZoneLevel( pTemplateZoneInfo->getZoneLevel() );
	pZoneInfo->setZoneAccessMode( pTemplateZoneInfo->getZoneAccessMode() );
	pZoneInfo->setZoneOwnerID( pTemplateZoneInfo->getZoneOwnerID() );
	pZoneInfo->setPayPlay( pTemplateZoneInfo->isPayPlay() );
//	pZoneInfo->setPremiumZone( pTemplateZoneInfo->isPremiumZone() );
	pZoneInfo->setPremiumZoneType( pTemplateZoneInfo->getPremiumZoneType() );	//1223 wlzzi - 프리미엄 존 혜택 복구
	pZoneInfo->setPKZone( pTemplateZoneInfo->isPKZone() );
	pZoneInfo->setNoPortalZone( pTemplateZoneInfo->isNoPortalZone() );
	pZoneInfo->setHolyLand( pTemplateZoneInfo->isHolyLand() );
	pZoneInfo->setAvailable( pTemplateZoneInfo->isAvailable() );
	pZoneInfo->setOpenLevel( pTemplateZoneInfo->getOpenLevel() );
	pZoneInfo->setSMPFilename( pTemplateZoneInfo->getSMPFilename() );
	pZoneInfo->setSSIFilename( pTemplateZoneInfo->getSSIFilename() );
	sprintf( temp, "%s%u", pTemplateZoneInfo->getFullName().c_str(), m_ZoneID );
	pZoneInfo->setFullName( temp );
	sprintf( temp, "%s%u", pTemplateZoneInfo->getShortName().c_str(), m_ZoneID );
	pZoneInfo->setShortName( temp );

	// add zone info to g_pZoneInfoManager
	g_pZoneInfoManager->addZoneInfo( pZoneInfo );

	// make zone and add to ZoneGroup
	Zone* pZone = new Zone( pZoneInfo->getZoneID() );
	Assert( pZone != NULL );

	// DynamicZone set to Zone
	pZone->setDynamicZone( this );

	// m_pZone setting
	m_pZone = pZone;

	// init zone
	pZone->init();

	// set ZoneGroup
	ZoneGroup* pZoneGroup = g_pZoneGroupManager->getZoneGroup( pZoneInfo->getZoneGroupID() );
	Assert( pZoneGroup != NULL );

	pZone->setZoneGroup( pZoneGroup );
	pZoneGroup->addZone( pZone );
	
	char strLog[256];
	sprintf(strLog, 
		"DynamicZone : create, ZoneID=%d, TemplateZoneID=%d",
		m_ZoneID, m_TemplateZoneID
	);
	filelog("CreateDynamicZone.log", strLog);
}

void DynamicZone::NotifyAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature)
{
	OnAttackCreature(pAttackerCreature, pDefenderCreature);
}

void DynamicZone::NotifyKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature)
{
	OnKillCreature(pAttackerCreature, pDeadCreature);
}

void DynamicZone::NotifyPK(Creature *pAttackerCreature, Creature *pDeadCreature)
{
	OnPK(pAttackerCreature, pDeadCreature);
}

bool DynamicZone::NotifyBeforeEnterZone(Creature *pCreature)
{
	return OnBeforeEnterZone(pCreature);
}

void DynamicZone::NotifyAfterEnterZone(Creature *pCreature)
{
	OnAfterEnterZone(pCreature);
}

void DynamicZone::NotifyBeforeLeaveZone(Creature *pCreature)
{
	OnBeforeLeaveZone(pCreature);
}

void DynamicZone::NotifyAfterLeaveZone(Creature *pCreature)
{
	OnAfterLeaveZone(pCreature);
}

void DynamicZone::NotifyAddMonster(Monster *pMonster)
{
	OnAddMonster(pMonster);
}

bool DynamicZone::NotifyResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord)
{
	return OnResurrectCreature(pCreature, zoneCoord);
}

bool DynamicZone::NotifySay(Creature *pCreature, string& strMesg)
{
	return OnSay(pCreature, strMesg);
}

bool DynamicZone::NotifyUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	return OnUseItemInGear(pCreature, ItemClass, ItemType);
}

bool DynamicZone::NotifyUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	return OnUseItemInInventory(pCreature, ItemClass, ItemType);
}

bool DynamicZone::NotifyUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	return OnUseItemInQuick(pCreature, ItemClass, ItemType);
}

bool DynamicZone::NotifyUseInventorySkill(Creature *pCreature, SkillType_t SkillType)
{
	return OnUseInventorySkill(pCreature, SkillType);
}

bool DynamicZone::NotifyUseObjectSkill(Creature *pCreature, SkillType_t SkillType)
{
	return OnUseObjectSkill(pCreature, SkillType);
}

bool DynamicZone::NotifyUseNamedSkill(Creature *pCreature, SkillType_t SkillType)
{
	return OnUseNamedSkill(pCreature, SkillType);
}

bool DynamicZone::NotifyUseSelfSkill(Creature *pCreature, SkillType_t SkillType)
{
	return OnUseSelfSkill(pCreature, SkillType);
}

bool DynamicZone::NotifyUseTileSkill(Creature *pCreature, SkillType_t SkillType)
{
	return OnUseTileSkill(pCreature, SkillType);
}

bool DynamicZone::NotifyBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType)
{
	return OnBeforeUseMonsterSkill(pCreature, SkillType);
}
void DynamicZone::NotifyAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType)
{
	OnAfterUseMonsterSkill(pCreature, SkillType);
}
	
