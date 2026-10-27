
//------------------------------------------------------------------------------
// File Name : MSSqlEngine.h
// Desc      : MSSqlEngine class definition
//------------------------------------------------------------------------------

#ifndef  _DB_ENGINE_MSSQL_H_
#define  _DB_ENGINE_MSSQL_H_

#include "Types.h"
#include "Exception.h"
#include <sybdb.h>

///
#define  MAX_DB_SERVER_NAME    32
#define  MAX_USER_NAME         32
#define  MAX_USER_PASS         32
#define  MAX_DB_NAME           32

#define  MAX_FILE_NAME         256

/// default db value
#define  DEFAULT_MAIN_DB_NAME    "MarbleBank"
#define  DEFAULT_USER_NAME       "dark_nme_service_app"
#define  DEFAULT_USER_PASS       "ekzm@@dpeps~!"
#define  DEFAULT_SERVER          "192.168.201.62:61051"

///
#define  DB_STATUS_NOT_INIT              0
#define  DB_STATUS_INIT_COMPLETE         1
#define  DB_STATUS_CONNECT_COMPLETE      2
#define  DB_STATUS_QUERY_SUCCESS         3
#define  DB_STATUS_FETCH_COMPLETE        4

//
#define  DB_FETCH_RECORD_MAX           1024
#define  DB_FETCH_COLUMN_MAX           32
#define  DB_FETCH_ROW_MAX              32 

///// class MSSqlEngine
//class ancLog;

class MSSqlEngine {
public:
   MSSqlEngine();
   ~MSSqlEngine();

   // public member functions
   bool   InitDBEngine(char *Server, char *User, char *Password, char *DB, char *LogFile); // init
   bool   InitDBEngine(char *LogFile); // init ( default value is used )

   bool   ConnectDB();  // connect to db engine ( must call InitUserDB before this )
   bool   ReConnectDB();
   void   CloseDB();

   int   QuerySQL(char *query_string);
   int   FetchResult();
   void  ReleaseResult();

   int   GetRecordNum() { return m_nNumberOfRecords; };
   int   GetColumnNum() { return m_nNumberOfColumns; };

   char *GetData(int row, int col); // row and col is 0 based

	bool NetmarbleQuery(const char* UserIP);   
protected:
   //// connect string
   char  m_ConnectServer[MAX_DB_SERVER_NAME];
   char  m_ConnectUser[MAX_USER_NAME];
   char  m_ConnectPassword[MAX_USER_PASS];
   char  m_ConnectDBName[MAX_DB_NAME];
   char  m_LogFileName[MAX_FILE_NAME];

   //variables
   int         m_DBStatusFlag;
   //
   LOGINREC    *m_TDSLogin;
   DBPROCESS   *m_TDSDBProc;

   int  m_nNumberOfRecords;
   int  m_nNumberOfColumns;

   int  m_nResultRecordInfo[DB_FETCH_ROW_MAX][DB_FETCH_COLUMN_MAX];
   char m_pResultRecordData[DB_FETCH_ROW_MAX][DB_FETCH_RECORD_MAX];

   //
   void WriteLog(const char *Fmt, ...);
};

extern MSSqlEngine* g_pMSSqlEngine;
#endif

