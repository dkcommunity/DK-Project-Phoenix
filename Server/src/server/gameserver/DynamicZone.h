/////////////////////////////////////////////////////////////////////////////
// DynamicZone.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNAMIC_ZONE_H__
#define __DYNAMIC_ZONE_H__

#include "Types.h"
#include "Timeval.h"
#include "Creature.h"
#include "Monster.h"
#include "DynamicZoneGroup.h"

enum DYNAMIC_ZONE_STATUS
{
	DYNAMIC_ZONE_STATUS_READY = 0,
	DYNAMIC_ZONE_STATUS_RUNNING,

	DYNAMIC_ZONE_STATUS_MAX
};

class Zone;

///////////////////////////////////////////////////////////
// class DynamicZone
///////////////////////////////////////////////////////////
class DynamicZone
{
public:
	DynamicZone();
	virtual ~DynamicZone();

public:
	virtual void init() {}		// 다이나믹 존 초기화
	virtual void heartbeat() {}	// 다이나믹 존 하트비트

public:
	// 새로 DynamicZone을 만들어서 ZoneGroup 에 추가한다.
	virtual void makeDynamicZone();

	DynamicZoneGroup *getDynamicZoneGroup() { return m_pDynamicZoneGroup; }
	void setDynamicZoneGroup(DynamicZoneGroup *pDynamicZoneGroup) { m_pDynamicZoneGroup = pDynamicZoneGroup; }

	ZoneID_t getTemplateZoneID() const { return m_TemplateZoneID; }
	void setTemplateZoneID( ZoneID_t templateZoneID ) { m_TemplateZoneID = templateZoneID; }

	ZoneID_t getZoneID() const { return m_ZoneID; }
	void setZoneID( ZoneID_t zoneID ) { m_ZoneID = zoneID; }

	int getStatus() const { return m_Status; }
	void setStatus( int status ) { m_Status = status; }

	ZoneCoord_t GetStartPosX() { return m_StartPosX; }
	ZoneCoord_t GetStartPosY() { return m_StartPosY; }

	bool GetDeleteFlag() { return m_bDeleteFlag; }
	void SetDeleteFlag(bool bDeleteFlag) { m_bDeleteFlag = bDeleteFlag; }

	virtual void NotifyAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature);
	virtual void NotifyKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature);
	virtual void NotifyPK(Creature *pAttackerCreature, Creature *pDeadCreature);
	virtual bool NotifyBeforeEnterZone(Creature *pCreature);
	virtual void NotifyAfterEnterZone(Creature *pCreature);

	virtual void NotifyBeforeLeaveZone(Creature *pCreature);
	virtual void NotifyAfterLeaveZone(Creature *pCreature);
	
	virtual void NotifyAddMonster(Monster *pMonster);
	virtual bool NotifyResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord);
	virtual bool NotifySay(Creature *pCreature, string& strMesg);
	
	virtual bool NotifyUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	virtual bool NotifyUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	virtual bool NotifyUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	
	virtual bool NotifyUseInventorySkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool NotifyUseObjectSkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool NotifyUseNamedSkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool NotifyUseSelfSkill(Creature *pCreature, SkillType_t SkillType);
	virtual bool NotifyUseTileSkill(Creature *pCreature, SkillType_t SkillType);
	
	bool NotifyBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType);
	void NotifyAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType);
		
	virtual bool IsGiveExperience() { return false; }

protected :
	virtual void OnAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature) {} 
	virtual void OnKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature) {}
	virtual void OnPK(Creature *pAttackerCreature, Creature *pDeadCreature) {}
	virtual bool OnBeforeEnterZone(Creature *pCreature) { return true; }
	virtual void OnAfterEnterZone(Creature *pCreature) {}

	virtual void OnBeforeLeaveZone(Creature *pCreature) {}
	virtual void OnAfterLeaveZone(Creature *pCreature) {}
	
	virtual void OnAddMonster(Monster *pMonster) {}
	virtual bool OnResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord) { return false;}
	virtual bool OnSay(Creature *pCreature, string& strMesg)  { return true;}

	virtual bool OnUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType) { return true;}
	virtual bool OnUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType) { return true;}
	virtual bool OnUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType) { return true;}
	
	virtual bool OnUseInventorySkill(Creature *pCreature, SkillType_t SkillType) { return true;}
	virtual bool OnUseObjectSkill(Creature *pCreature, SkillType_t SkillType) { return true;}
	virtual bool OnUseNamedSkill(Creature *pCreature, SkillType_t SkillType) { return true;}
	virtual bool OnUseSelfSkill(Creature *pCreature, SkillType_t SkillType) { return true;}
	virtual bool OnUseTileSkill(Creature *pCreature, SkillType_t SkillType) { return true;}

	virtual bool OnBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType) { return true;}
	virtual void OnAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType) { }
	
protected:
	DynamicZoneGroup *m_pDynamicZoneGroup;

	ZoneID_t m_TemplateZoneID;
	ZoneID_t m_ZoneID;
	int m_Status;
	Zone* m_pZone;
	Timeval m_Deadline;

	bool m_bDeleteFlag;	// 존의 제거 여부를 설정한다. 필요에 따라 존을 제거하기 위함. 

	ZoneCoord_t m_StartPosX;
	ZoneCoord_t m_StartPosY;
};

///////////////////////////////////////////////////////////
// class DynamicZoneFactory
///////////////////////////////////////////////////////////
class DynamicZoneFactory
{
public:
	virtual ~DynamicZoneFactory() {}
	virtual int getDynamicZoneType() const = 0;
	virtual string getDynamicZoneName() const = 0;
	virtual DynamicZone* createDynamicZone() const = 0;
};

///////////////////////////////////////////////////////////
// DynamicZoneFactory macro
///////////////////////////////////////////////////////////
#define DECLARE_DYNAMIC_ZONE_FACTORY( DYNAMIC_ZONE ) 						\
	class DYNAMIC_ZONE##Factory	: public DynamicZoneFactory					\
	{																		\
	public:																	\
		int getDynamicZoneType() const;										\
		string getDynamicZoneName() const { return "##DYNAMIC_ZONE##"; }	\
		DynamicZone* createDynamicZone() const { return new DYNAMIC_ZONE; }	\
	};

#define DEFINE_DYNAMIC_ZONE_FACTORY( DYNAMIC_ZONE, DYNAMIC_ZONE_ID )					\
	int DYNAMIC_ZONE##Factory::getDynamicZoneType() const { return DYNAMIC_ZONE_ID; }

#endif

