#include "FriendManager.h"
#include "DBExecuteManager.h"

FriendManager *g_pFriendManager = NULL;

FriendListManager::FriendListManager(const string& strName)
{
	m_strName = strName;
}

FriendListManager::~FriendListManager()
{
	IndividualOption *pIndividualOption;
	
	TIndividualOptionMap::iterator iter;
	iter = m_IndividualOptionMap.begin();
	for(; iter != m_IndividualOptionMap.end() ; ++iter )
	{
		pIndividualOption = iter->second;
		
		SAFE_DELETE(pIndividualOption);
	}
	
	m_IndividualOptionMap.clear();
}

void FriendListManager::LoadGlobalOption()
{
	Statement* pStmt = NULL;
	
	m_GlobalOption.SetAllowToRequestAddition(true);
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT AllowToRequestAddition FROM FriendGlobalOption WHERE Name='%s'", 
			m_strName.c_str()
		);
		
		if ( pResult->next() )
		{
			if ( pResult->getInt(1) == 0)
			{
				m_GlobalOption.SetAllowToRequestAddition(false);			
			}
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)	
}

void FriendListManager::LoadFriendList()
{
	LoadGlobalOption();
	
	Statement* pStmt = NULL;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT FriendName, Memo FROM FriendList WHERE Name='%s'", 
			m_strName.c_str()
		);
		
		string strFriendName;
		
		string strMemo;
	
		while( pResult->next() )
		{
			strFriendName = pResult->getString(1);
			
			strMemo = pResult->getString(2);
			
			m_FriendFWList[strFriendName] = strFriendName;
			
			IndividualOption *pIndividualOption = new IndividualOption;
			pIndividualOption->SetMemo(strMemo);
			
			m_IndividualOptionMap[strFriendName] = pIndividualOption;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT Name FROM FriendWaitList WHERE FriendName='%s'", 
			m_strName.c_str()
		);
		
		string strFriendName;
		
		while( pResult->next() )
		{
			strFriendName = pResult->getString(1);
			
			m_FriendWaitList[strFriendName] = strFriendName;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
}

void FriendListManager::AddFriend(const string& strFriendName)
{
	GetFriendFWList()[strFriendName] = strFriendName;
	
	IndividualOption *pIndividualOption = new IndividualOption;
	pIndividualOption->SetMemo("");
	
	m_IndividualOptionMap[strFriendName] = pIndividualOption;

}

void FriendListManager::DelFriend(const string& strFriendName)
{
	{
		TFriendList::iterator iter;
		iter = GetFriendFWList().find(strFriendName);
		if ( iter != GetFriendFWList().end() )
		{
			GetFriendFWList().erase(iter);
		}
	}
	
	{
		TIndividualOptionMap::iterator iter;
		iter = m_IndividualOptionMap.find(strFriendName);
		if ( iter != m_IndividualOptionMap.end() )
		{
			IndividualOption *pIndividualOption;
			
			pIndividualOption = iter->second;
			
			m_IndividualOptionMap.erase(iter);
			
			SAFE_DELETE(pIndividualOption);
		}
		
	}
}

bool FriendListManager::IsExistFriend(const string& strFriendName)
{
	TFriendList::iterator iter;
	
	iter =  GetFriendFWList().find(strFriendName);
	if ( iter != GetFriendFWList().end() )
	{
		// 郴 模备 格废俊 模备 粮犁
		
		return true;
	}

	return false;
}

void FriendListManager::AddFriendWaitList(const string& strFriendName)
{
	GetFriendWaitList()[strFriendName] = strFriendName;
}

void FriendListManager::DelFriendWaitList(const string& strFriendName)
{
	TFriendList::iterator iter;
	iter = GetFriendWaitList().find(strFriendName);
	if ( iter != GetFriendWaitList().end() )
	{
		GetFriendWaitList().erase(iter);
	}	
}

bool FriendListManager::IsExistFriendWaitList(const string& strFriendName)
{
	TFriendList::iterator iter;
	
	iter =  GetFriendWaitList().find(strFriendName);
	if ( iter != GetFriendWaitList().end() )
	{
		// 郴 模备 格废俊 模备 粮犁
		
		return true;
	}

	return false;	
}

void FriendListManager::UpdateGlobalOption(const GlobalOption& option)
{
	m_GlobalOption = option;
	
	Statement *pStmt;
	
	bool bIsExist = false;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT * FROM FriendGlobalOption WHERE Name='%s'", m_strName.c_str());
		
		if ( pResult->getRowCount() > 0 )
		{
			bIsExist = true;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	ostringstream os;
	
	if ( bIsExist )
	{
		os << "UPDATE FriendGlobalOption "
			<< " SET AllowToRequestAddition=" << (int)option.GetAllowToRequestAddition() 
			<< " WHERE Name='" << m_strName << "'";
	}
	else
	{
		os << "INSERT FriendGlobalOption(Name, AllowToRequestAddition) VALUES('" 
			<< m_strName << "', " 
			<< (int)option.GetAllowToRequestAddition() << ")";
	}
	
	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery(os.str().c_str());
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)	
}

FriendListManager::IndividualOption* FriendListManager::GetIndividualOption(const string& strFriendName)
{
	IndividualOption *pIndividualOption = NULL;
				
	TIndividualOptionMap::iterator iter;
	iter = m_IndividualOptionMap.find(strFriendName);
	if ( iter != m_IndividualOptionMap.end() )
	{
		pIndividualOption = iter->second;		
	}
			
	return pIndividualOption;
	
}

void FriendListManager::UpdateIndividualOption(const string& strFriendName, const IndividualOption& option)
{
	IndividualOption *pIndividualOption = GetIndividualOption(strFriendName);
	if ( pIndividualOption != NULL )
	{
		pIndividualOption->SetMemo(option.GetMemo());
	}
	
	Statement *pStmt;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"UPDATE FriendList SET Memo='%s' WHERE Name='%s' AND FriendName='%s'", 
			ReplaceDBString(option.GetMemo()).c_str(),
			m_strName.c_str(),
			strFriendName.c_str()
		);
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

///////////////////////////////////////////////////////////////////////////////
//

void FriendManager::LoadFriendList(const string& strName)
{
	FriendListManager *pFriendListManager = GetFriendListManager(strName);
	
	if ( pFriendListManager == NULL )
	{
		pFriendListManager = CreateFriendListManager(strName);
	}
	
	pFriendListManager->LoadFriendList();
}

void FriendManager::UnloadFriendList(const string& strName)
{
	RemoveFriendListManager(strName);
}

FriendListManager *FriendManager::CreateFriendListManager(const string& strName)
{
	FriendListManager *pFriendListManager = new FriendListManager(strName);
	
	m_FriendListManagerMap[strName] = pFriendListManager;
	
	return pFriendListManager;
}

void FriendManager::RemoveFriendListManager(const string& strName)
{
	FriendListManager *pFriendListManager = NULL;
	TFriendListManagerMap::iterator iter;
		
	iter = m_FriendListManagerMap.find(strName);
	if ( iter != m_FriendListManagerMap.end() )
	{
		pFriendListManager = iter->second;
		SAFE_DELETE(pFriendListManager);
		
		m_FriendListManagerMap.erase(iter);
	}
}

FriendListManager *FriendManager::GetFriendListManager(const string& strName)
{
	FriendListManager *pFriendListManager = NULL;
	
	TFriendListManagerMap::iterator iter;
	
	iter = m_FriendListManagerMap.find(strName);
	if ( iter != m_FriendListManagerMap.end() )
	{
		pFriendListManager = iter->second;
	}
	else
	{
		pFriendListManager = NULL; 
	}
	
	return pFriendListManager;
}

bool FriendManager::IsExistName(const string& strName)
{
	Statement* pStmt = NULL;
	bool bIsExist = false;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT Name FROM Slayer WHERE Name='%s'", 
			ReplaceDBString(strName).c_str()
		);
		
		if ( pResult->getRowCount() > 0 )
		{
			bIsExist = true;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	return bIsExist;
}

void FriendManager::AddFriendInDB(const string& strName, const string& strFriendName)
{
	TDBExecuteUnitKey DBExecuteUnitKey = g_GameDBExecuteManager.AllocateExecuteUnit(0);
	
	{
		ostringstream os;
		
		os << "INSERT FriendList(Name, FriendName, Memo) VALUES('" 
			<< strName << "', '" 
			<< strFriendName << "', '')";
			
		g_GameDBExecuteManager.RequestExecQuery(DBExecuteUnitKey, os.str());
	}	
}

void FriendManager::DelFriendInDB(const string& strName, const string& strFriendName)
{
	TDBExecuteUnitKey DBExecuteUnitKey = g_GameDBExecuteManager.AllocateExecuteUnit(0);
	
	{
		ostringstream os;
		
		os << "DELETE FROM FriendList WHERE Name='" <<  strName << "' AND FriendName='" << strFriendName << "'"; 
		g_GameDBExecuteManager.RequestExecQuery(DBExecuteUnitKey, os.str());
	}	
}

int FriendManager::GetFriendListCountInDB(const string& strName)
{
	int iListCount = 0;
	
	Statement* pStmt = NULL;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT count(FriendName) FROM FriendList WHERE Name='%s'", 
			strName.c_str()
		);
		
		pResult->next();
		
		iListCount = pResult->getInt(1); 
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	return iListCount;
}

bool FriendManager::IsExistFriendInDB(const string& strName, const string& strFriendName)
{
	Statement* pStmt = NULL;
	bool bIsExist = false;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT FriendName FROM FriendList WHERE Name='%s' AND FriendName='%s'", 
			strName.c_str(),
			strFriendName.c_str()
		);
		
		if ( pResult->getRowCount() > 0 )
		{
			bIsExist = true;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	return bIsExist;
}

void FriendManager::AddFriendToWaitListInDB(const string& strName, const string& strFriendName)
{
	TDBExecuteUnitKey DBExecuteUnitKey = g_GameDBExecuteManager.AllocateExecuteUnit(0);
	
	{
		ostringstream os;
		
		os << "INSERT FriendWaitList(Name, FriendName, ReqDate) VALUES('" 
			<< strName << "', '" 
			<< strFriendName << "', now())";
			
		g_GameDBExecuteManager.RequestExecQuery(DBExecuteUnitKey, os.str());
	}
}

void FriendManager::DelFriendToWaitListInDB(const string& strName, const string& strFriendName)
{
	TDBExecuteUnitKey DBExecuteUnitKey = g_GameDBExecuteManager.AllocateExecuteUnit(0);
	
	{
		ostringstream os;
		
		os << "DELETE FROM FriendWaitList WHERE Name='" <<  strName << "' AND FriendName='" << strFriendName << "'";
		
		g_GameDBExecuteManager.RequestExecQuery(DBExecuteUnitKey, os.str());
	}
}

int FriendManager::GetFriendWaitListCountInDB(const string& strName)
{
	int iListCount = 0;
	
	Statement* pStmt = NULL;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT count(Name) FROM FriendWaitList WHERE FriendName='%s'", 
			strName.c_str()
		);
		
		pResult->next();
		
		iListCount = pResult->getInt(1); 
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	return iListCount;
}

bool FriendManager::IsExistFriendToWaitListInDB(const string& strName, const string& strFriendName)
{
	Statement* pStmt = NULL;
	bool bIsExistInWaitList = false;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT FriendName FROM FriendWaitList WHERE Name='%s' AND FriendName='%s'", 
			strName.c_str(),
			strFriendName.c_str()
		);
		
		if ( pResult->getRowCount() > 0 )
		{
			bIsExistInWaitList = true;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	return bIsExistInWaitList;
}

RaceType FriendManager::GetRace(const string& strName)
{
	Statement* pStmt = NULL;
	string strRace;
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT Race FROM Slayer WHERE Name='%s'", 
			strName.c_str()
		);
		
		if ( pResult->next() )
		{
			strRace = pResult->getString(1);	
		}
		else
		{
			strRace = "";
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	if ( strRace.compare("SLAYER") == 0 )
	{
		return RACE_SLAYER;
	}
	else if ( strRace.compare("VAMPIRE") == 0 )
	{
		return RACE_VAMPIRE;
	}
	else if ( strRace.compare("OUSTERS") == 0 )
	{
		return RACE_OUSTERS; 
	}
	
	return RACE_MAX;

}

FriendListManager::GlobalOption FriendManager::GetGlobalOption(const string& strName)
{
	FriendListManager::GlobalOption option;
	
	FriendListManager *pFriendListManager = GetFriendListManager(strName);
	
	if ( pFriendListManager != NULL )
	{
		option = pFriendListManager->GetGlobalOption();
	}
	else
	{
		FriendListManager friendListManager(strName);
		friendListManager.LoadGlobalOption();
		
		option = friendListManager.GetGlobalOption();
	}
	
	return option;
}

FriendInfo *FriendManager::AddFriendInfo(const string& strName)
{
	RemoveFriendInfo(strName);
	
	FriendInfo *pFriendInfo = new FriendInfo;
	
	m_FriendInfoMap[strName] = pFriendInfo;
	
	return pFriendInfo;
}
	
void FriendManager::RemoveFriendInfo(const string& strName)
{
	TFriendInfoMap::iterator iter;
			
	iter = m_FriendInfoMap.find(strName);
	if ( iter != m_FriendInfoMap.end() )
	{
		FriendInfo *pFriendInfo;
		
		pFriendInfo = iter->second;
		
		m_FriendInfoMap.erase(iter);
		
		SAFE_DELETE(pFriendInfo);
	}
}

FriendInfo *FriendManager::GetFriendInfo(const string& strName)
{
	TFriendInfoMap::iterator iter;
	
	iter = m_FriendInfoMap.find(strName);
	if ( iter != m_FriendInfoMap.end() )
	{
		return iter->second;
	}
	
	static FriendInfo defaultInfo;
	return &defaultInfo;
}
