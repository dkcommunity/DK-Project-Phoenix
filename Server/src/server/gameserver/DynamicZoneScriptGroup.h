/////////////////////////////////////////////////////////////////////////////
// DynamicZoneScriptGroup.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNAMIC_ZONE_SCRIPT_GROUP_H__
#define __DYNAMIC_ZONE_SCRIPT_GROUP_H__

#include "Types.h"
#include "DynamicZoneGroup.h"

#include <map>

// forward declaration
class DynamicZone;
class DynamicZoneScript;

///////////////////////////////////////////////////////////
// class DynamicZoneScriptGroup
///////////////////////////////////////////////////////////
class DynamicZoneScriptGroup : public DynamicZoneGroup
{
public:
	DynamicZoneScriptGroup() : DynamicZoneGroup() {}
	virtual ~DynamicZoneScriptGroup() {}

public:
	DynamicZoneScript* makeDynamicZone(string& strScriptName);
	
	DynamicZoneScript* GetStaticZone(string& strScriptName);
	void RegistStaticZone(string& strScriptName, DynamicZoneScript *pDynamicZoneScript);
	
protected :
	map<string, DynamicZoneScript*> m_StaticZoneList;

};

#endif

