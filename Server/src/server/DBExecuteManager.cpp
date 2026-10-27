#include <stdio.h>

#include "DB.h"
#include "Properties.h"
#include "CondVar.h"

#include "DBExecuteManager.h"

DBExecuteManager g_GameDBExecuteManager;

DBExecuteUnit::DBExecuteUnit(int iIndex)
{
	m_QueryQueueMutex.setName("DBExecuteUnit.QueryQueue");
	m_iIndex = iIndex;
	
	m_bRunning = false;
	m_pConnection = NULL;
	
	m_iReferCount = 0;
	
	m_iRequestQueryCount = 0;
	m_iExecuteQueryCount = 0;
}

DBExecuteUnit::~DBExecuteUnit() throw()
{
	SAFE_DELETE(m_pConnection);
}

void DBExecuteUnit::SetConnection(Connection *pConnection)
{
	m_pConnection = pConnection;	
}

void DBExecuteUnit::RequestExecQuery(const string& strQuery)
{
	try
	{
		__ENTER_CRITICAL_SECTION(m_QueryQueueMutex)
		
		m_QueryQueue.push_back(strQuery);
		
		m_QueryQueueCond.signal();
		
		__LEAVE_CRITICAL_SECTION(m_QueryQueueMutex)
	}
	catch(Throwable& t)
	{
		cout << t.toString() << endl;
	}
	
}

void DBExecuteUnit::ProcessExecQuery()
{
	Statement *pStmt;
	
	string strQuery;
	bool bIsEmptyQueue = true;
	int iRemainQueueCount;

	__ENTER_CRITICAL_SECTION(m_QueryQueueMutex)

	// ���������� ���� DummyQuery ����
	// FAZ 4 (2026-05-06): SELECT 1 ping defansif catch — END_DB macro
	// `throw(const char*)` worker thread'i terminate edebilir.
	if ( m_QueryQueue.size() == 0 )
	{
		try
		{
			BEGIN_DB
			{
				pStmt = m_pConnection->createStatement();
				pStmt->executeQuery("SELECT 1");

				SAFE_DELETE(pStmt)
			}
			END_DB(pStmt)
		}
		catch (Throwable & t)
		{
			char strFileName[32];
			sprintf(strFileName, "DBExecute.%d.log", GetIndex());
			filelog(strFileName, "DummyQuery SELECT 1 Throwable: %s", t.toString().c_str());
		}
		catch (const char* msg)
		{
			char strFileName[32];
			sprintf(strFileName, "DBExecute.%d.log", GetIndex());
			filelog(strFileName, "DummyQuery SELECT 1 const char* rethrow: %s", msg ? msg : "(null)");
		}
		catch (...)
		{
			char strFileName[32];
			sprintf(strFileName, "DBExecute.%d.log", GetIndex());
			filelog(strFileName, "DummyQuery SELECT 1 unknown exception (worker thread protected)");
		}

		bIsEmptyQueue = true;
	}
	else
	{
		bIsEmptyQueue = false;
	}

	__LEAVE_CRITICAL_SECTION(m_QueryQueueMutex)
	
	if ( bIsEmptyQueue )
	{
		return;
	}
	
	char strFileName[32];
	sprintf(strFileName, "DBExecute.%d.log", GetIndex());
	
	while( true )
	{
		__ENTER_CRITICAL_SECTION(m_QueryQueueMutex)
		
		iRemainQueueCount = m_QueryQueue.size();
		
		if ( m_QueryQueue.size() > 0 )
		{
			strQuery = *m_QueryQueue.begin();
			m_QueryQueue.pop_front();

			bIsEmptyQueue = false;
		}
		else
		{
			bIsEmptyQueue = true;
		}
		
		__LEAVE_CRITICAL_SECTION(m_QueryQueueMutex)
		
		if ( bIsEmptyQueue )
		{
			return;
		}
	
		// by svi
		//filelog(strFileName, "Index=%d QueueCount=%d Query=%s", m_iIndex, iRemainQueueCount, strQuery.c_str() );
		
		try
		{
			BEGIN_DB
			{
				pStmt = m_pConnection->createStatement();
				pStmt->executeQuery(strQuery);

				SAFE_DELETE(pStmt)
			}
			END_DB(pStmt)
		}
		catch (Throwable & t)
		{
			filelog(strFileName, "DBExecuteUnit::ProcessExecQuery() %s", t.toString().c_str());
		}
		// Legacy call path'leri hala const char* firlatabilir. END_DB artik
		// SQLQueryException firlatir; bu catch sadece eski throw site'lari icin kalir.
		catch (const char* msg)
		{
			filelog(strFileName, "DBExecuteUnit::ProcessExecQuery() const char* rethrow: %s", msg ? msg : "(null)");
		}
		catch (const std::exception& e)
		{
			filelog(strFileName, "DBExecuteUnit::ProcessExecQuery() std::exception: %s", e.what());
		}
		catch (...)
		{
			filelog(strFileName, "DBExecuteUnit::ProcessExecQuery() unknown exception (worker thread protected)");
		}
	}
}

int DBExecuteUnit::GetQueueCount()
{
	int iQueueCount;
	
	__ENTER_CRITICAL_SECTION(m_QueryQueueMutex)
	iQueueCount = m_QueryQueue.size();
	__LEAVE_CRITICAL_SECTION(m_QueryQueueMutex)
	
	return iQueueCount;
}

void DBExecuteUnit::run () throw () 
{
	__BEGIN_TRY

	m_bRunning = true;
	
	struct timeval now;
	struct timespec ts;
	    
	while( m_bRunning )
	{
		gettimeofday(&now, NULL);
		
	    ts.tv_sec = now.tv_sec + (60 * 5);
	    ts.tv_nsec = 0;
		    
	    try
	    {
			__ENTER_CRITICAL_SECTION(m_QueryQueueMutex)
			m_QueryQueueCond.timedwait(m_QueryQueueMutex, &ts);
			__LEAVE_CRITICAL_SECTION(m_QueryQueueMutex)
	    }
	    catch (CondVarException & t)
	    {
	    	// timeout �߻�
	    }
		catch (Throwable & t) 
		{
			cerr << t.toString() << endl;
			throw;
		}
		
		ProcessExecQuery();
	}
	
	__END_CATCH
}

void DBExecuteUnit::stop () throw ( Error )
{
	m_bRunning = false;

	try
	{
		__ENTER_CRITICAL_SECTION(m_QueryQueueMutex)
		m_QueryQueueCond.signal();
		__LEAVE_CRITICAL_SECTION(m_QueryQueueMutex)
	}
	catch(Throwable& t)
	{
		cout << t.toString() << endl;
	}
}

DBExecuteManager::DBExecuteManager()
{
	m_DBExecuteUnitListMutex.setName("DBExecuteManager.DBExecuteUnitListMutex");
}

DBExecuteManager::~DBExecuteManager()
{
	Term();
}

void DBExecuteManager::Init(int iUnitCount)
{
	m_iUnitCount = iUnitCount;
	
	DBExecuteUnit *pDBExecuteUnit;
	Connection* pConnection;
	
	for(int i = 0; i < m_iUnitCount; ++i)
	{
		pDBExecuteUnit = new DBExecuteUnit(i);
		pConnection = new Connection(m_strHost, m_strDBName, m_strUser, m_strPassword, m_uiPort);
		
		pDBExecuteUnit->SetConnection(pConnection);
		
		m_DBExecuteUnitList.push_back(pDBExecuteUnit);
	}
	
	TDBExecuteUnitList::iterator iter;
	iter = m_DBExecuteUnitList.begin();
	for( ; iter != m_DBExecuteUnitList.end(); ++iter )
	{
		pDBExecuteUnit = (DBExecuteUnit *)(*iter);
		pDBExecuteUnit->start();
	}

}

void DBExecuteManager::Term()
{
	DBExecuteUnit *pDBExecuteUnit;
	
	TDBExecuteUnitList::iterator iter;
	iter = m_DBExecuteUnitList.begin();
	for( ; iter != m_DBExecuteUnitList.end(); ++iter )
	{
		pDBExecuteUnit = (DBExecuteUnit *)(*iter);
		
		pDBExecuteUnit->stop();
		
		try
		{
			Thread::join(pDBExecuteUnit);
		}
		catch (Throwable & t) 
		{
			cerr << t.toString() << endl;
		}
		
		SAFE_DELETE(pDBExecuteUnit)
	}
	
	m_DBExecuteUnitList.clear();
}

void DBExecuteManager::SetDBConnectionInfo(string strHost, string strDBName, string strUser, string strPassword, uint uiPort)
{
	m_strHost = strHost;
	m_strDBName = strDBName;
	m_strUser = strUser;
	m_strPassword = strPassword;
	m_uiPort = uiPort;
}

void DBExecuteManager::RequestExecQuery(const TDBExecuteUnitKey DBExecuteUnitKey, const string& strQuery)
{
	int iIndex;
	
	TUnitAllocMap::iterator iter;
	
	__ENTER_CRITICAL_SECTION(m_DBExecuteUnitListMutex);
	
	iter = m_UnitAllocMap.find(DBExecuteUnitKey);
	
	if ( iter != m_UnitAllocMap.end() )
	{
		iIndex = iter->second; 
	}
	else
	{
		iIndex = AllocateExecuteUnit_NOLOCKED(DBExecuteUnitKey);
	}
		
	if ( m_DBExecuteUnitList[iIndex] != NULL )
	{
		m_DBExecuteUnitList[iIndex]->RequestExecQuery(strQuery);
	}
	
	__LEAVE_CRITICAL_SECTION(m_DBExecuteUnitListMutex);
}

TDBExecuteUnitKey DBExecuteManager::AllocateExecuteUnit_NOLOCKED(const TDBExecuteUnitKey DBExecuteUnitKey)
{
	unsigned int iIndex;
	TDBExecuteUnitKey iMinIndex = 0;
	
	int iMinReferCount = 0;
	
	DBExecuteUnit *pDBExecuteUnit;
	
	for(iIndex = 0 ; iIndex < m_DBExecuteUnitList.size() ; ++iIndex)
	{
		pDBExecuteUnit = m_DBExecuteUnitList[iIndex];
		if ( pDBExecuteUnit == NULL )
		{
			continue;
		}
		
		if ( iIndex == 0 )
		{
			iMinReferCount = pDBExecuteUnit->GetReferCount();
			iMinIndex = iIndex;
		}
		else
		{
			if ( iMinReferCount > pDBExecuteUnit->GetReferCount() )
			{
				iMinReferCount = pDBExecuteUnit->GetReferCount();
				iMinIndex = iIndex;
			}
		}
	}
	
	pDBExecuteUnit = m_DBExecuteUnitList[iMinIndex];
	pDBExecuteUnit->IncreaseReferCount();
	
	m_UnitAllocMap[DBExecuteUnitKey] = iMinIndex;

	{
	  // by svi
	  /*
		char strFileName[32];
		sprintf(strFileName, "DBExecute.%d.log", pDBExecuteUnit->GetIndex());

		filelog(strFileName, "Allocate ExecuteUnit Key=%d UnitIndex=%d ReferCount=%d", 
			DBExecuteUnitKey, iMinIndex, pDBExecuteUnit->GetReferCount() 
		);
		*/
	}
	
	return iMinIndex;
}

TDBExecuteUnitKey DBExecuteManager::AllocateExecuteUnit(const TDBExecuteUnitKey DBExecuteUnitKey)
{
	TDBExecuteUnitKey iMinIndex = 0;
	
	__ENTER_CRITICAL_SECTION(m_DBExecuteUnitListMutex);
	
	iMinIndex = AllocateExecuteUnit_NOLOCKED(DBExecuteUnitKey);
	
	__LEAVE_CRITICAL_SECTION(m_DBExecuteUnitListMutex);
	
	return iMinIndex;
	
}

void DBExecuteManager::DeallocateExecuteUnit(const TDBExecuteUnitKey DBExecuteUnitKey)
{
	__ENTER_CRITICAL_SECTION(m_DBExecuteUnitListMutex);
	
	TUnitAllocMap::iterator iter;
	
	iter = m_UnitAllocMap.find(DBExecuteUnitKey);
	
	if ( iter != m_UnitAllocMap.end() )
	{
		DBExecuteUnit *pDBExecuteUnit;
		
		pDBExecuteUnit = m_DBExecuteUnitList[iter->second];
		if ( pDBExecuteUnit != NULL )
		{
			pDBExecuteUnit->DecreaseReferCount();

			// by svi
			/*
			char strFileName[32];
			sprintf(strFileName, "DBExecute.%d.log", pDBExecuteUnit->GetIndex());
			filelog(strFileName, "Deallocate ExecuteUnit Key=%d UnitIndex=%d ReferCount=%d", 
				DBExecuteUnitKey, iter->second , pDBExecuteUnit->GetReferCount() 
			);
			*/
		}
		else
		{
			filelog("DBExecute.log", "Deallocate ExecuteUnit Key=%d UnitIndex=%d ReferCount=%s", 
				DBExecuteUnitKey, iter->second , "<unknown>" 
			);
		}
		
		m_UnitAllocMap.erase(iter);
		
	}
	
	__LEAVE_CRITICAL_SECTION(m_DBExecuteUnitListMutex);
}
