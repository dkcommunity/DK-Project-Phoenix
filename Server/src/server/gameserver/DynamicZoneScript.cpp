#include "ScriptManager.h"
/////////////////////////////////////////////////////////////////////////////
// DynamicZoneScript.cpp
/////////////////////////////////////////////////////////////////////////////

#include "DynamicZoneManager.h"
#include "DynamicZoneScript.h"
#include "DynamicZoneInfo.h"
#include "PlayerCreature.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "PCManager.h"
#include "EffectManager.h"
#include "EffectDeleteTile.h"
#include "EffectTilePortal.h"
#include "DB.h"
#include "Assert.h"
#include "Monster.h"
#include "MonsterManager.h"
#include "MonsterAI.h"
#include "Timeval.h"
#include "Properties.h"
#include "Tile.h"

#include "script/DEScriptZone.h"
#include "EventTransport.h"

#include "DynamicZoneScriptGroup.h"
#include "GamePlayer.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"

#include <stdio.h>
#include <stdlib.h>

DynamicZoneScript::DynamicZoneScript()
{
	m_pDEScriptZone = new DEScriptZone(g_pPublicScriptManager->getLuaState(), m_pZone);
}

DynamicZoneScript::~DynamicZoneScript()
{
	char strLog[256];
	sprintf(strLog, "DynamicZoneScript : remove, ScriptName=%s", getScriptName().c_str());
	filelog("CreateDynamicZone.log", strLog );
	
	g_pDynamicZoneManager->returnDynamicZoneID(getZoneID());

	SAFE_DELETE(m_pDEScriptZone);
}

void DynamicZoneScript::makeDynamicZone()
{
	m_pDEScriptZone->Load( 
		(char *)(g_pConfig->getProperty("HomePath") + "/script/zone/" + 
				getScriptName() + 
				".lua").c_str() 
	);

	m_pDEScriptZone->NotifyInitializeVariables();
	m_TemplateZoneID = m_pDEScriptZone->GetTemplateZoneID();
	m_StartPosX = m_pDEScriptZone->GetStartPosX();
	m_StartPosY = m_pDEScriptZone->GetStartPosY();

	getCurrentTime(m_EmptyStartTime);

	DynamicZone::makeDynamicZone();
	
	if ( m_pDEScriptZone->IsStaticZone() )
	{
		DynamicZoneScriptGroup* pDynamicZoneScriptGroup;
		
		pDynamicZoneScriptGroup = (DynamicZoneScriptGroup*)g_pDynamicZoneManager->getDynamicZoneGroup(DYNAMIC_ZONE_SCRIPT);
		if ( pDynamicZoneScriptGroup != NULL )
		{
			pDynamicZoneScriptGroup->RegistStaticZone(getScriptName(), this);
		}
	}
	
	char strLog[256];
	sprintf(strLog, "DynamicZoneScript : ScriptName=%s", getScriptName().c_str());
	filelog("CreateDynamicZone.log", strLog );
}

void DynamicZoneScript::init()
{
	cout << "DynamicZoneScript(ZoneID = " << m_ZoneID << ")" << " init()" << endl;

	DynamicZone::init();

	m_pDEScriptZone->Init(m_pZone);
	m_pDEScriptZone->NotifyInitialize();

}

void DynamicZoneScript::heartbeat()
{
	if ( !m_pDEScriptZone->IsStaticZone() && m_pZone->getPCManager()->getTotalPCNum() == 0 )
	{
		Timeval curTime;
		Timeval diffTime;

		getCurrentTime(curTime);

		diffTime = timediff(curTime, m_EmptyStartTime); 

		if ( m_pDEScriptZone->GetIgnoreEmptyTimeOut() || diffTime.tv_sec > m_pDEScriptZone->GetEmptyTimeOut() )
		{
			SetDeleteFlag(true);
		}
	}
	else
	{
		m_pDEScriptZone->UpdateTime();
		
		getCurrentTime(m_EmptyStartTime);

		SetDeleteFlag(false);
	}
}

void DynamicZoneScript::TransportCreature(Creature *pCreature)
{
	if ( pCreature == NULL )
	{
		return;
	}
	
	switch( m_pDEScriptZone->GetEntranceType() )
	{
		case DEScriptZone::ENTRANCE_TYPE_PARTY :
			{
				ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(getZoneID());
														
				if ( getZoneID() != 0 && pZoneInfo != NULL )
				{
					LocalPartyManager* pLPM = NULL;
					
					int PartyID = 0;
					ZoneCoord_t X = 0;
					ZoneCoord_t Y = 0;
					
					PartyID = pCreature->getPartyID();
	
					if ( PartyID != 0 )
					{
						pLPM = pCreature->getLocalPartyManager();
	
						Party *pParty = pLPM->getParty(PartyID);
						
						map<string, Creature*> MemberMap = pParty->getMemberMap();
						map<string, Creature*>::iterator MemberMapIter;
						
						Creature *pMemberCreature;
					
						MemberMapIter = MemberMap.begin();
						for( ; MemberMapIter != MemberMap.end() ; ++MemberMapIter)
						{
							pMemberCreature = MemberMapIter->second;
							if ( pMemberCreature != NULL )
							{
								m_pDEScriptZone->NotifySetStartPos(pMemberCreature, X, Y);
								transportCreature(pMemberCreature, getZoneID(), X, Y, true);
							}
						}
					}
					else
					{
						m_pDEScriptZone->NotifySetStartPos(pCreature, X, Y);
						transportCreature(pCreature, getZoneID(), X, Y, true);
					}
				}
			}
			break;
		case DEScriptZone::ENTRANCE_TYPE_LOBBY :
			// not yet
			break;
		case DEScriptZone::ENTRANCE_TYPE_INDIVISUAL :
		default :
			{
				ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(getZoneID());
				ZoneCoord_t X = 0;
				ZoneCoord_t Y = 0;

				if ( getZoneID() != 0 && pZoneInfo != NULL )
				{
					m_pDEScriptZone->NotifySetStartPos(pCreature, X, Y);
					transportCreature(pCreature, getZoneID(), X, Y, true);
				}
			}
			break;
	}
}

void DynamicZoneScript::OnAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature)
{
	m_pDEScriptZone->NotifyAttackCreature(pAttackerCreature, pDefenderCreature);
}

void DynamicZoneScript::OnKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature)
{
	m_pDEScriptZone->NotifyKillCreature(pAttackerCreature, pDeadCreature);
}

void DynamicZoneScript::OnPK(Creature *pAttackerCreature, Creature *pDeadCreature)
{
	m_pDEScriptZone->NotifyPK(pAttackerCreature, pDeadCreature);
}

bool DynamicZoneScript::OnBeforeEnterZone(Creature *pCreature)
{
	return m_pDEScriptZone->NotifyBeforeEnterZone(pCreature);
}

void DynamicZoneScript::OnAfterEnterZone(Creature *pCreature)
{
	m_pDEScriptZone->NotifyAfterEnterZone(pCreature);
}

void DynamicZoneScript::OnBeforeLeaveZone(Creature *pCreature)
{
	return m_pDEScriptZone->NotifyBeforeLeaveZone(pCreature);
}

void DynamicZoneScript::OnAddMonster(Monster *pMonster)
{
	return m_pDEScriptZone->NotifyAddMonster(pMonster);
}

bool DynamicZoneScript::OnResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord)
{
	return m_pDEScriptZone->NotifyResurrectCreature(pCreature, zoneCoord);
}

bool DynamicZoneScript::OnSay(Creature *pCreature, string& strMesg)
{
	return m_pDEScriptZone->NotifySay(pCreature, strMesg);
}

void DynamicZoneScript::OnAfterLeaveZone(Creature *pCreature)
{
	m_pDEScriptZone->NotifyAfterLeaveZone(pCreature);
}

bool DynamicZoneScript::OnUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	return m_pDEScriptZone->NotifyUseItemInGear(pCreature, ItemClass, ItemType);
}

bool DynamicZoneScript::OnUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	return m_pDEScriptZone->NotifyUseItemInInventory(pCreature, ItemClass, ItemType);
}

bool DynamicZoneScript::OnUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	return m_pDEScriptZone->NotifyUseItemInQuick(pCreature, ItemClass, ItemType);
}

bool DynamicZoneScript::OnUseInventorySkill(Creature *pCreature, SkillType_t SkillType)
{
	return m_pDEScriptZone->NotifyUseInventorySkill(pCreature, SkillType);
}

bool DynamicZoneScript::OnUseObjectSkill(Creature *pCreature, SkillType_t SkillType)
{
	return m_pDEScriptZone->NotifyUseObjectSkill(pCreature, SkillType);
}

bool DynamicZoneScript::OnUseNamedSkill(Creature *pCreature, SkillType_t SkillType)
{
	return m_pDEScriptZone->NotifyUseNamedSkill(pCreature, SkillType);
}

bool DynamicZoneScript::OnUseSelfSkill(Creature *pCreature, SkillType_t SkillType)
{
	return m_pDEScriptZone->NotifyUseSelfSkill(pCreature, SkillType);
}

bool DynamicZoneScript::OnUseTileSkill(Creature *pCreature, SkillType_t SkillType)
{
	return m_pDEScriptZone->NotifyUseTileSkill(pCreature, SkillType);
}

bool DynamicZoneScript::OnBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType)
{
	return m_pDEScriptZone->NotifyBeforeUseMonsterSkill(pCreature, SkillType);
}

void DynamicZoneScript::OnAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType)
{
	m_pDEScriptZone->NotifyAfterUseMonsterSkill(pCreature, SkillType);	
}
	
DEFINE_DYNAMIC_ZONE_FACTORY( DynamicZoneScript, DYNAMIC_ZONE_SCRIPT )


