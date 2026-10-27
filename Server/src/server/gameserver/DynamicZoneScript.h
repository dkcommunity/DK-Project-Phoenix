/////////////////////////////////////////////////////////////////////////////
// DynamicZoneScript.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNAMIC_ZONE_SCRIPT_H__
#define __DYNAMIC_ZONE_SCRIPT_H__

#include "Types.h"
#include "DynamicZone.h"

class DEScriptZone;

///////////////////////////////////////////////////////////
// class DynamicZoneScript
///////////////////////////////////////////////////////////
class DynamicZoneScript : public DynamicZone
{
public:
	DynamicZoneScript();
	virtual ~DynamicZoneScript();

public:
	virtual void init();		// 다이나믹 존 초기화
	virtual void heartbeat();	// 다이나믹 존 하트비트

public:
	void setScriptName(string& strScriptName) 
	{
		m_strScriptName = strScriptName;
	}
	string& getScriptName()
	{
		return m_strScriptName;
	}
	
	void TransportCreature(Creature *pCreature);
	
	virtual void makeDynamicZone();
	
	virtual bool IsGiveExperience() { return true; }

protected:
	virtual void OnAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature);
	virtual void OnKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature);
	virtual void OnPK(Creature *pAttackerCreature, Creature *pDeadCreature);
	virtual bool OnBeforeEnterZone(Creature *pCreature);
	virtual void OnAfterEnterZone(Creature *pCreature);

	virtual void OnBeforeLeaveZone(Creature *pCreature);
	virtual void OnAfterLeaveZone(Creature *pCreature);
	
	virtual void OnAddMonster(Monster *pMonster);
	virtual bool OnResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord);
	virtual bool OnSay(Creature *pCreature, string& strMesg);
	
	virtual bool OnUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	virtual bool OnUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	virtual bool OnUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	
	virtual bool OnUseInventorySkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool OnUseObjectSkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool OnUseNamedSkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool OnUseSelfSkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool OnUseTileSkill(Creature *pCreature, SkillType_t SkillType);

	virtual bool OnBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType);
	virtual void OnAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType);
	
protected:
	string m_strScriptName;
	DEScriptZone *m_pDEScriptZone;

	Timeval m_EmptyStartTime;	// 캐릭터 없는 상태 시작 시각 
	
};


DECLARE_DYNAMIC_ZONE_FACTORY( DynamicZoneScript )

#endif

