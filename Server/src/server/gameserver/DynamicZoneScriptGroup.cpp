/////////////////////////////////////////////////////////////////////////////
// DynamicZoneScriptGroup.cpp
/////////////////////////////////////////////////////////////////////////////

// include files
#include "DynamicZoneScriptGroup.h"
#include "DynamicZone.h"
#include "DynamicZoneScript.h"
#include "DynamicZoneManager.h"
#include "DynamicZoneFactoryManager.h"
#include "DynamicZoneInfo.h"
#include "Assert.h"

///////////////////////////////////////////////////////////
// class DynamicZoneScriptGroup
///////////////////////////////////////////////////////////


DynamicZoneScript* DynamicZoneScriptGroup::makeDynamicZone(string& strScriptName)
{
	if ( getDynamicZoneType() != DYNAMIC_ZONE_SCRIPT )
	{
		return NULL;
	}

	DynamicZoneScript* pDynamicZoneScript = GetStaticZone(strScriptName);
	
	// Static 설정이 되어 있는 경우 이미 만들어진 존으로 이동시킨다.
	if ( pDynamicZoneScript != NULL )
	{
		return pDynamicZoneScript;
	}

	pDynamicZoneScript = dynamic_cast<DynamicZoneScript *>(g_pDynamicZoneFactoryManager->createDynamicZone(getDynamicZoneType()));
	if (pDynamicZoneScript == NULL) { cout << "[Warn] DynamicZoneScript: createDynamicZone returned NULL for " << strScriptName << endl; return NULL; }

	pDynamicZoneScript->setScriptName(strScriptName);
	pDynamicZoneScript->setDynamicZoneGroup(this);
	pDynamicZoneScript->setTemplateZoneID( getTemplateZoneID());
	pDynamicZoneScript->setZoneID( g_pDynamicZoneManager->getNewDynamicZoneID() );
	pDynamicZoneScript->setStatus( DYNAMIC_ZONE_STATUS_RUNNING );
	pDynamicZoneScript->makeDynamicZone();
	addDynamicZone( pDynamicZoneScript );

	return pDynamicZoneScript;

}

DynamicZoneScript * DynamicZoneScriptGroup::GetStaticZone(string& strScriptName)
{
	if ( m_StaticZoneList.find(strScriptName) != m_StaticZoneList.end() )
	{
		return  m_StaticZoneList[strScriptName]; 
	}
	
	return NULL;
}

void DynamicZoneScriptGroup::RegistStaticZone(string& strScriptName, DynamicZoneScript *pDynamicZoneScript)
{
	if ( GetStaticZone(strScriptName) == NULL )
	{
		m_StaticZoneList[strScriptName] = pDynamicZoneScript; 
	}
}
