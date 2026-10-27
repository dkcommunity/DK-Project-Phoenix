#include "Item.h"
#include "Skill.h"

#include <map>
#include <string>

class SkillCastingTime
{
public :
	int iCastingTimeForNonAdv;
	int iCastingTimeForAdv;
};

typedef map<Item::ItemClass, SkillCastingTime> TSkillCastingTimeMap;

class SkillCastingTimeManager;

class SkillCastingTimeList
{
protected :
	TSkillCastingTimeMap m_SkillCastingTimeMap;
	
	bool m_bHasDefaultCastingTime;
	SkillCastingTime m_DefaultCastingTime;
	
public :
	SkillCastingTimeList();
	
protected :
	void SetDefault(SkillCastingTime& castingTime);
	void Set(Item::ItemClass IClass, SkillCastingTime& castingTime);
	
	bool GetSkillTime(Item::ItemClass IClass, SkillCastingTime& castingTime);
	
	friend class SkillCastingTimeManager;
};

class SkillCastingTimeManager
{
public :
	typedef vector<string> TParams;
	typedef vector<TParams> TParamList;
	
	SkillCastingTimeManager();
	~SkillCastingTimeManager();

protected :
	typedef map<SkillType_t, SkillCastingTimeList*> TSkillCastingTimeList;
	
	TSkillCastingTimeList m_SkillCastingTimeList;

protected :
	SkillCastingTimeList *GetSkillCastingTimeList(SkillType_t SkillType);
	
	void SetDefault(SkillType_t SkillType, SkillCastingTime& castingTime);
	void Set(SkillType_t SkillType, Item::ItemClass IClass, SkillCastingTime& castingTime);

public :
	void ParseSkillCastingTime(string& strCastingTimeParam, TParamList& ParamList);
	void SetDefault(SkillType_t SkillType, TParamList& ParamList);
	void Set(SkillType_t SkillType, TParamList& ParamList);
	
	bool GetSkillTime(SkillType_t SkillType, Item::ItemClass IClass, SkillCastingTime& castingTime);

};

extern SkillCastingTimeManager g_SkillCastingTimeManager;
