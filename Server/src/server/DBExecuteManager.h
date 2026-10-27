#ifndef _DB_EXECUTE_MANAGER_H_
#define _DB_EXECUTE_MANAGER_H_

#include <deque>
#include <map>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

#include "CondVar.h"
#include "Thread.h"


#include "DB.h"

class DBExecuteUnit : public Thread {
protected:
  mutable Mutex m_QueryQueueMutex;
  CondVar m_QueryQueueCond;

  int m_iIndex;

  deque<string> m_QueryQueue;
  bool m_bRunning;

  Connection *m_pConnection;

  int m_iReferCount;

  // 통계 관련 변수
  unsigned long m_iRequestQueryCount; // 최근 쿼리 요청수
  unsigned long m_iExecuteQueryCount; // 최근 쿼리 실행수

public:
  DBExecuteUnit(int iIndex = -1);
  virtual ~DBExecuteUnit() throw();

  virtual void run() throw();
  virtual void stop() throw(Error);

  void SetConnection(Connection *pConnection);
  void RequestExecQuery(const string &strQuery);
  void ProcessExecQuery();

  int GetQueueCount();
  int GetReferCount() { return m_iReferCount; }
  int GetIndex() { return m_iIndex; }

  void IncreaseReferCount() { ++m_iReferCount; }
  void DecreaseReferCount() { --m_iReferCount; }
};

typedef unsigned int TDBExecuteUnitKey;
typedef map<TDBExecuteUnitKey, int> TUnitAllocMap;
typedef vector<DBExecuteUnit *> TDBExecuteUnitList;

class DBExecuteManager {
protected:
  mutable Mutex m_DBExecuteUnitListMutex;

  TDBExecuteUnitList m_DBExecuteUnitList;

  string m_strHost;
  string m_strDBName;
  string m_strUser;
  string m_strPassword;
  uint m_uiPort;
  int m_iUnitCount;

  TUnitAllocMap m_UnitAllocMap;

public:
  DBExecuteManager();
  virtual ~DBExecuteManager();

  void Init(int iUnitCount = 1);
  void Term();
  void SetDBConnectionInfo(string strHost, string strDBName, string strUser,
                           string strPassword, uint uiPort);
  void RequestExecQuery(const TDBExecuteUnitKey iKey, const string &strQuery);

  TDBExecuteUnitKey AllocateExecuteUnit(const TDBExecuteUnitKey iKey);
  TDBExecuteUnitKey
  AllocateExecuteUnit_NOLOCKED(const TDBExecuteUnitKey DBExecuteUnitKey);
  void DeallocateExecuteUnit(const TDBExecuteUnitKey iKey);

  string &GetHost() { return m_strHost; }
  string &GetDBName() { return m_strDBName; }
  string &GetUser() { return m_strUser; }
  string &GetPassword() { return m_strPassword; }
  uint GetPort() { return m_uiPort; }
};

extern DBExecuteManager g_GameDBExecuteManager;

#endif /* _DB_EXECUTE_MANAGER_H_ */
