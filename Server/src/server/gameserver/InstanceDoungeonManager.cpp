#include "InstanceDoungeonManager.h"
#include "DB.h"

InstanceDoungeonManager g_InstanceDoungeonManager;

InstanceDoungeonUnit::InstanceDoungeonUnit()
{
}

InstanceDoungeonUnit::~InstanceDoungeonUnit()
{
	m_InstanceDoungeonItemList.clear();
}
	
TInstanceDoungeonItemList* InstanceDoungeonUnit::GetDoungeonItemList()
{
	return &m_InstanceDoungeonItemList;
}

InstanceDoungeonItem* InstanceDoungeonUnit::GetDoungeonItem(BYTE iDoungeonIndex)
{

	//cout<<m_InstanceDoungeonItemList.size()<<endl;
	if ( iDoungeonIndex > m_InstanceDoungeonItemList.size()-1 )
	{
		return NULL;
	}
	
	return &m_InstanceDoungeonItemList[iDoungeonIndex];	
}

InstanceDoungeonManager::InstanceDoungeonManager()
{

}

InstanceDoungeonManager::~InstanceDoungeonManager()
{
	m_InstanceDoungeonUnitMap.clear();
}

InstanceDoungeonUnit* InstanceDoungeonManager::GetDoungeonUnit(const string& strDoungeonID)
{
	if (m_InstanceDoungeonUnitMap.find(strDoungeonID) != m_InstanceDoungeonUnitMap.end() )
	{
		return &m_InstanceDoungeonUnitMap[strDoungeonID];
	}
	
	return NULL;
}

void InstanceDoungeonManager::load()
{
	InstanceDoungeonUnit unit;
	
	Statement* pStmt = NULL;
	BEGIN_DB
	{
		string strDoungeonID;

		// create statement
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery(
			"SELECT DoungeonID, DoungeonTitle, DoungeonDesc FROM InstanceDoungeonInfo");

		while (pResult->next()) 
		{
			uint i = 0;

			strDoungeonID = pResult->getString(++i);
			
			unit.m_strTitle = pResult->getString(++i);
			unit.m_strDesc = pResult->getString(++i);
			
			m_InstanceDoungeonUnitMap[strDoungeonID] = unit;
		}
	
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)
	
	BEGIN_DB
	{
		InstanceDoungeonItem item;
		
		string strDoungeonID;
		bool bCreateOnStart;

		// create statement
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery(
			"SELECT DoungeonID, DoungeonIndex, DoungeonName, DoungeonScript, CreateOnStart FROM InstanceDoungeonList");
		
		while (pResult->next()) 
		{
			uint i = 0;
			
			strDoungeonID = pResult->getString(++i);
			item.m_iIndex = pResult->getInt(++i);
			item.m_strName = pResult->getString(++i);
			item.m_strScript = pResult->getString(++i);
			bCreateOnStart =  pResult->getInt(++i);
			
			m_InstanceDoungeonUnitMap[strDoungeonID].m_InstanceDoungeonItemList.push_back(item);
			
			if ( bCreateOnStart )
			{
				InstanceDoungeonManager::CreateInstanceDoungeon(strDoungeonID.c_str(), item.m_iIndex);
			}
		}
	
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)	
}

DynamicZoneScript *InstanceDoungeonManager::CreateInstanceDoungeon(string& strScriptName)
{
	DynamicZoneScriptGroup *pDynamicZoneScriptGroup = dynamic_cast<DynamicZoneScriptGroup *>(g_pDynamicZoneManager->getDynamicZoneGroup(DYNAMIC_ZONE_SCRIPT));
	if (pDynamicZoneScriptGroup == NULL) { cout << "[Warn] no DynamicZoneScriptGroup" << endl; return NULL; }
	DynamicZoneScript *pDynamicZoneScript = pDynamicZoneScriptGroup->makeDynamicZone(strScriptName);
	
	return pDynamicZoneScript;
}

DynamicZoneScript *InstanceDoungeonManager::CreateInstanceDoungeon(const char *strDungeonID, int iDungeonIndex)
{
	string strScriptName;
	
	strScriptName = strDungeonID;
	
	InstanceDoungeonUnit *pInstanceDoungeonUnit = g_InstanceDoungeonManager.GetDoungeonUnit(strDungeonID);
	
	if ( pInstanceDoungeonUnit == NULL )
	{
		return NULL;
	}
	
	InstanceDoungeonItem *pItem = pInstanceDoungeonUnit->GetDoungeonItem(iDungeonIndex);
	
	if ( pItem == NULL )
	{
	cout<<"a:--"<<iDungeonIndex<<endl;
		return NULL;
	}
	
	strScriptName = strScriptName + "/" + pItem->GetScript();
	
	return CreateInstanceDoungeon(strScriptName);

}
