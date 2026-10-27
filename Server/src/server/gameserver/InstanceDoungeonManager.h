#ifndef _INSTANCE_DOUNGEON_MANAGER_H_
#define _INSTANCE_DOUNGEON_MANAGER_H_

#include "Types.h"

#include "DynamicZoneManager.h"
#include "DynamicZoneInfo.h"
#include "DynamicZoneScriptGroup.h"
#include "DynamicZoneScript.h"

#include <vector>
#include <map>
#include <string>

class InstanceDoungeonItem;
class InstanceDoungeonUnit;
class InstanceDoungeonManager;

typedef vector<InstanceDoungeonItem> TInstanceDoungeonItemList; 
typedef map<string, InstanceDoungeonUnit> TInstanceDoungeonUnitMap;

class InstanceDoungeonItem
{
public :
	BYTE GetIndex() { return m_iIndex; }
	string& GetName() { return m_strName; }
	string& GetScript() { return m_strScript;}
		
protected :
	BYTE m_iIndex;
	string m_strName;
	string m_strScript;
	
	friend class InstanceDoungeonManager; 
	
};

class InstanceDoungeonUnit
{
public :
	InstanceDoungeonUnit();
	~InstanceDoungeonUnit();
	
	string& GetTitle() { return m_strTitle;} 
	string& GetDesc() { return m_strDesc;}
	
	TInstanceDoungeonItemList* GetDoungeonItemList();
	InstanceDoungeonItem* GetDoungeonItem(BYTE iDoungeonIndex);
	
protected :
	string m_strTitle;
	string m_strDesc;
	
	TInstanceDoungeonItemList m_InstanceDoungeonItemList;
	
	friend class InstanceDoungeonManager;
	
};

class InstanceDoungeonManager
{
public :
	InstanceDoungeonManager();
	~InstanceDoungeonManager();
	
	InstanceDoungeonUnit* GetDoungeonUnit(const string& strDoungeonID);
	void load();
	
	static DynamicZoneScript *CreateInstanceDoungeon(string& strScriptName);
	static DynamicZoneScript *CreateInstanceDoungeon(const char *strDungeonID, int iDungeonIndex);
	
protected :
	
	TInstanceDoungeonUnitMap m_InstanceDoungeonUnitMap;
};

extern InstanceDoungeonManager g_InstanceDoungeonManager;

#endif /* _INSTANCE_DOUNGEON_MANAGER_H_ */
