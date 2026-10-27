#ifndef _DESCRIPTZONE_H_
#define _DESCRIPTZONE_H_

#include "DEScript.h"
#include "Zone.h"

#include "Gpackets/GCSystemMessage.h"

#include <string>
#include <map>


class DEScriptTimer
{
protected : 

	int m_iTimerID;

	long m_lPeriodTime;	// unit - milliseconds
	Timeval m_NextTime;

friend class DEScriptZone;
};

typedef map<int, DEScriptTimer> TDEScriptTimer;
typedef map<string, int> TScriptZoneCount; 

class DEScriptZone : public DEScript
{
public :
	enum EntranceType
	{
		ENTRANCE_TYPE_NONE = 0,
		ENTRANCE_TYPE_INDIVISUAL = 1,
		ENTRANCE_TYPE_PARTY = 2,
		ENTRANCE_TYPE_LOBBY = 3
	};
	
	enum TimerType
	{
		TIMER_TYPE_ONESHOT = 0,
		TIMER_TYPE_PERIODIC = 1
	};
	
public :
	DEScriptZone(struct lua_State *L, Zone* pZone);
	virtual ~DEScriptZone();
	struct lua_State* m_pLuaState;

	void Init(Zone *pZone);

	void UpdateTime();

public :
	// 외부 공개 되어야 하는 Notify
	void NotifyInitializeVariables();
	void NotifyInitialize();

	void NotifyAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature);
	void NotifyKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature);
	void NotifyPK(Creature *pAttackerCreature, Creature *pDeadCreature);
	void NotifyTimer(int iTimerID);

	bool NotifyBeforeEnterZone(Creature *pCreature);
	void NotifyAfterEnterZone(Creature *pCreature);

	void NotifyBeforeLeaveZone(Creature *pCreature);
	void NotifyAfterLeaveZone(Creature *pCreature);
	
	void NotifyAddMonster(Monster *pMonster);
	bool NotifyResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord);
	bool NotifySay(Creature *pCreature, string& strMesg);
	
	bool NotifyUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	bool NotifyUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	bool NotifyUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType);
	
	bool NotifyUseInventorySkill(Creature *pCreature, SkillType_t SkillType);
	bool NotifyUseObjectSkill(Creature *pCreature, SkillType_t SkillType);
	bool NotifyUseNamedSkill(Creature *pCreature, SkillType_t SkillType);
	bool NotifyUseSelfSkill(Creature *pCreature, SkillType_t SkillType);
	bool NotifyUseTileSkill(Creature *pCreature, SkillType_t SkillType);
	
	bool NotifyBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType);
	void NotifyAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType);
	
	void NotifySetStartPos(Creature *pCreature, ZoneCoord_t& StartX, ZoneCoord_t& StartY);

public :
	string GetName() { return m_Name; }
	ZoneID_t GetTemplateZoneID() { return m_TemplateZoneID; }
	ZoneCoord_t GetStartPosX() { return m_StartPosX; }
	ZoneCoord_t GetStartPosY() { return m_StartPosY; }
	EntranceType GetEntranceType() { return m_EntranceType; }
	bool IsStaticZone() { return m_bStaticZone; }
	Duration_t GetEmptyTimeOut() { return m_EmptyTimeOut; }
	
	ZoneID_t GetZoneID();
	int GetScriptZoneCount();
	bool GetIgnoreEmptyTimeOut();
	
	void KnockbackCreature(lua_tinker::table TargetCreatureInfo, ZoneCoord_t iOriginX, ZoneCoord_t iOriginY);
	bool MoveFastMonster(lua_tinker::table CreatureInfo, lua_tinker::table MovePosInfo);
	
protected :
	Zone *m_pZone;

	TDEScriptTimer m_TimerList;
	vector<int> m_KillTimerPlanList;

	string m_Name;
	ZoneID_t m_TemplateZoneID;
	ZoneCoord_t m_StartPosX;
	ZoneCoord_t m_StartPosY;
	EntranceType m_EntranceType;
	bool m_bStaticZone;
	Duration_t m_EmptyTimeOut;
	Dir_t m_StartDir;
	
	bool m_bIgnoreEmptyTimeOut;
	
	static TScriptZoneCount m_ScriptZoneCountMap;

	void BroadcastSystemMessage(SystemMessageType iMesgType, const char *strMesg);
	void BroadcastSystemMessageInZone(SystemMessageType iMesgType, const char *strMesg);
	void SendSystemMessage(lua_tinker::table CreatureInfo, SystemMessageType iMesgType, const char *strMesg);
	void TransportCreature(lua_tinker::table CreatureInfo, ZoneID_t ZoneID, ZoneCoord_t X, ZoneCoord_t Y);
	void TraverseCreature();
	void TraverseMonster();
	void BroadcastGCSay(lua_tinker::table CreatureInfo, const char *strMsg, long lColor);
	void BroadcastGCGlobalChat(lua_tinker::table CreatureInfo, const char *strMsg, long lColor);
	
	void SetIgnoreEmptyTimeOut(bool bIgnoreEmptyTimeOut);
	
	lua_tinker::table AddMonster(lua_tinker::table MonsterInfo);
	bool HaveItem(lua_tinker::table CreatureInfo, lua_tinker::table HaveItemInfo);
	bool TakeItem(lua_tinker::table CreatureInfo, lua_tinker::table TakeItemInfo);
	bool GiveItem(lua_tinker::table CreatureInfo, lua_tinker::table GiveItemInfo);
        bool GiveGoodsItem(lua_tinker::table CreatureInfo,unsigned int GiveGoodsItemNumber, unsigned int GiveGoodsItemCount);//add by rallser
	bool GiveGoodsItemForLogOut(char *Type,char *Account,char *Name, unsigned int GiveGoodsItemNumber, unsigned int GiveGoodsItemCount);//add by rallser
	void GiveWarPoint(lua_tinker::table CreatureInfo,unsigned int Point);//by rallser

	void SetTimer(int iTimerID, long lPeriodTime, TimerType Type);
	void KillTimer(int iTimerID);
	
	lua_tinker::table AddEffectToTile(lua_tinker::table EffectCreateInfo);
	lua_tinker::table AddEffectToCreature(lua_tinker::table EffectCreateInfo, lua_tinker::table CreatureInfo);
	void AddSpecificEffect(lua_tinker::table CreatureInfo);//by rallser tian jia gu ding xiao gu
        void AddTimeEffect(lua_tinker::table EffectCreateInfo, lua_tinker::table CreatureInfo);//by rallser  tian jia shi jian xiao gu
        void RemoveEffectFromCreature(lua_tinker::table EffectCreateInfo, lua_tinker::table CreatureInfo);// by rallser  qu chu syou xiao gu
	void GiveExpAdv(lua_tinker::table CreatureInfo,unsigned int amount);//by rallser
	void GiveRankExp(lua_tinker::table CreatureInfo,unsigned int amount);//by rallser

	void GiveDianJuan(lua_tinker::table CreatureInfo,unsigned int amount);
	void GiveJiFen(lua_tinker::table CreatureInfo,unsigned int amount);

	void setFlag(lua_tinker::table CreatureInfo);//by rallser
	void removeFlag(lua_tinker::table CreatureInfo);//by rallser
	void AddEffectToScreen(lua_tinker::table EffectCreateInfo, lua_tinker::table CreatureInfo);
	
	void RemoveEffect(lua_tinker::table EffectInfo);
	void SetBlocked(ZoneCoord_t PosX, ZoneCoord_t PosY);
	void ClearBlocked(ZoneCoord_t PosX, ZoneCoord_t PosY);
	bool IsBlocked(ZoneCoord_t PosX, ZoneCoord_t PosY);
	
	bool FindPosition(MonsterType_t monsterType, lua_tinker::table PositionInfo);
	
	ZoneID_t CreateInstanceDoungeon(const char *strScriptName);
	void SetPKZone(bool bPK);

	void addNPC(lua_tinker::table NpcInfo);
		
	void recallparty(lua_tinker::table CreatureInfo,ZoneCoord_t X,ZoneCoord_t Y);

	lua_tinker::table GetCreature(ObjectID_t objectID);
	lua_tinker::table GetMonster(ObjectID_t objectID);
	void MonsterUseSkill(lua_tinker::table CreatureInfo,lua_tinker::table MonsterInfo);
//add by rallser

	
};

#endif /* _DESCRIPTZONE_H_ */
