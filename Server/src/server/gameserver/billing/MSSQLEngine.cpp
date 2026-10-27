//------------------------------------------------------------------------------
// File Name : MSSqlEngine.cpp
// Desc      : MSSqlEngine class implementation
//------------------------------------------------------------------------------
#include "MSSQLEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sybfront.h>
#include "common.h"
#include <iostream>


//----------------------------------------------------------------------------
// Function Name: MSSqlEngine
// Description  : MSSqlEngine class constructor
// Parameters   : none
// Return Values: none
//----------------------------------------------------------------------------
MSSqlEngine::MSSqlEngine()
{
   //// connect string
   memset(m_ConnectServer, 0, MAX_DB_SERVER_NAME);
   memset(m_ConnectUser, 0, MAX_USER_NAME);
   memset(m_ConnectPassword, 0, MAX_USER_PASS);
   memset(m_ConnectDBName, 0, MAX_DB_NAME);
   memset(m_LogFileName, 0, MAX_FILE_NAME);

   //// member init
   strcpy(m_ConnectServer, DEFAULT_SERVER);
   strcpy(m_ConnectUser, DEFAULT_USER_NAME);
   strcpy(m_ConnectPassword, DEFAULT_USER_PASS);
   strcpy(m_ConnectDBName, DEFAULT_MAIN_DB_NAME);

   m_TDSLogin = NULL;
   m_TDSDBProc = NULL;

   putenv("SYBASE=./");
	//putenv( "SYBASE=./" );
putenv( "TDSVER=42" ); 
   
   ReleaseResult();  // reset info

   m_DBStatusFlag=DB_STATUS_NOT_INIT;
}

//----------------------------------------------------------------------------
// Function Name: ~MSSqlEngine
// Description  : MSSqlEngine class destructor
// Parameters   : none
// Return Values: none
//----------------------------------------------------------------------------
MSSqlEngine::~MSSqlEngine()
{
   ReleaseResult();
   dbexit();

   if (m_DBStatusFlag==DB_STATUS_CONNECT_COMPLETE) CloseDB();
}

//----------------------------------------------------------------------------
// Function Name: InitDBEngine
// Description  : Init member variables and mysql
// Parameters   :
//                char *Server - server ip or name
//                char *User - db user name
//                char *Password - user password
//                char *DB - db name to connect
// Return Values: 1 - success, 0 - fail
//----------------------------------------------------------------------------
bool MSSqlEngine::InitDBEngine(char *Server, char *User, char *Password, char *DB, char *LogFile)
{
   /// check parameter
   if (!Server[0] || !DB[0] || !User[0] || !Password[0]) {
      WriteLog("invalid parameter (MSSqlEngine::InitDBEngine)");
      return false;
   }
   //// member init
   strcpy(m_ConnectServer, Server);
   strcpy(m_ConnectUser, User);
   strcpy(m_ConnectPassword, Password);
   strcpy(m_ConnectDBName, DB);

   return InitDBEngine(LogFile);
}

//----------------------------------------------------------------------------
// Function Name: InitDBEngine
// Description  : Init member variables and mysql (default value is used)
// Parameters   : none
// Return Values: 1 - success, 0 - fail
//----------------------------------------------------------------------------
bool MSSqlEngine::InitDBEngine(char *LogFile)
{
   dbinit(); /* lib 초기화 */

   dberrhandle( syb_err_handler );
   dbmsghandle( syb_msg_handler );

   m_TDSLogin = dblogin();  // 레코드 가져오기
   if (!m_TDSLogin) return false;

   DBSETLPWD(m_TDSLogin, m_ConnectPassword);
   DBSETLUSER(m_TDSLogin, m_ConnectUser);

   strcpy(m_LogFileName, LogFile);
   m_DBStatusFlag=DB_STATUS_INIT_COMPLETE;
   return true;
}

//----------------------------------------------------------------------------
// Function Name: ConnectDB (tcp)
// Description  : real connect to DB Engine (MYSQL)
// Parameters   : none
// Return Values: 1 - success, 0 - fail
//----------------------------------------------------------------------------
bool MSSqlEngine::ConnectDB()
{
   // check init
   if (m_DBStatusFlag!=DB_STATUS_INIT_COMPLETE) {
      switch (m_DBStatusFlag) {
         case DB_STATUS_NOT_INIT:
            WriteLog("you must call InitUserDB before ConnectDB (MSSqlEngine::ConnectDB)");
            return false;
         case DB_STATUS_CONNECT_COMPLETE:
            WriteLog("database connection is already made (MSSqlEngine::ConnectDB)");
            return false;
         default: return false;
      }
   }

   m_TDSDBProc = dbopen(m_TDSLogin, m_ConnectServer);
   if (!m_TDSDBProc) {
      WriteLog("ConnectDB : dbopen failed");
      return false;
   }

   // select db
   if (dbuse(m_TDSDBProc, m_ConnectDBName) == FAIL) {
      WriteLog("ConnectDB : dbopen failed");
      return false;
   }

   m_DBStatusFlag=DB_STATUS_CONNECT_COMPLETE;
   return true;
}

//----------------------------------------------------------------------------
// Function Name: CloseDB
// Description  : close connection
// Parameters   : none
// Return Values: none
//----------------------------------------------------------------------------
void MSSqlEngine::CloseDB()
{
   dbexit();
   
   m_DBStatusFlag=DB_STATUS_INIT_COMPLETE;
   return;
}

//----------------------------------------------------------------------------
// Function Name: QuerySQL
// Description  : send query to db engine
// Parameters   : char * query_string (SQL statement)
// Return Values: 1 - success, 0 - fail
//----------------------------------------------------------------------------
int MSSqlEngine::QuerySQL(char *query_string)
{
   //// check parameter
   if (!query_string[0]) {
      WriteLog("query string is empty (MSSqlEngine::QuerySQL)");
      return 0;
   }
   //// check status
   switch (m_DBStatusFlag) {
      case DB_STATUS_CONNECT_COMPLETE:
         break;
      case DB_STATUS_INIT_COMPLETE:
         ConnectDB();
         break;
      case DB_STATUS_FETCH_COMPLETE:
      case DB_STATUS_QUERY_SUCCESS:
         ReleaseResult();
         break;
      default:
         WriteLog("db status flag mismatch[%d] (MSSqlEngine::QuerySQL)", m_DBStatusFlag);
         return 0;
   }
   //// send query to db engine
   if (dbcmd(m_TDSDBProc, query_string) == FAIL) {
      WriteLog("dbcmd failed : %s (MSSqlEngine::QuerySQL)", query_string);
      return 0;
   }
   if (dbsqlexec(m_TDSDBProc) == FAIL) {
      WriteLog("dbsqlexec failed: %s (MSSqlEngine::QuerySQL)", query_string);
      return 0;
   }

   m_DBStatusFlag=DB_STATUS_QUERY_SUCCESS;
   return 1;
}

//----------------------------------------------------------------------------
// Function Name: FetchResult
// Description  : fetch query result(only used SELECT, SHOW, DESCRIBE, EXPLAIN)
// Parameters   : none
// Return Values: if return value < 0 then error else number of records
//----------------------------------------------------------------------------
int MSSqlEngine::FetchResult()
{
   // reset counter
   m_nNumberOfRecords = 0;
   m_nNumberOfColumns = 0;

   //// check status
   if (m_DBStatusFlag!=DB_STATUS_QUERY_SUCCESS) {
      WriteLog("invalid call - was not called QuerySQL (MSSqlEngine::FetchResult)");
      return -1;
   }

   char buffer[32][256];
   int i, fieldlength;

   while (dbresults(m_TDSDBProc) != NO_MORE_RESULTS) {
      m_nNumberOfColumns = dbnumcols(m_TDSDBProc);
      for (i = 1; i <= dbnumcols(m_TDSDBProc) && i <= 32; i++) {
         dbbind(m_TDSDBProc, i, STRINGBIND, -1, (BYTE *)buffer[i-1]);
      }

      while (dbnextrow(m_TDSDBProc) != NO_MORE_ROWS) {
         fieldlength = 0;
         for (i = 0; i < dbnumcols(m_TDSDBProc); i++) {
            m_nResultRecordInfo[m_nNumberOfRecords][i]=strlen(buffer[i])+1;
            buffer[i][strlen(buffer[i])]=0;
            memcpy((m_pResultRecordData[m_nNumberOfRecords])+fieldlength,buffer[i],m_nResultRecordInfo[m_nNumberOfRecords][i]);
            fieldlength+=m_nResultRecordInfo[m_nNumberOfRecords][i];
         }
         m_nNumberOfRecords++;
      }
   }

   m_DBStatusFlag=DB_STATUS_FETCH_COMPLETE;
   return m_nNumberOfRecords;
}

//----------------------------------------------------------------------------
// Function Name: ReleaseResult
// Description  : release query result set (must be called before next query)
// Parameters   : none
// Return Values: none
//----------------------------------------------------------------------------
void MSSqlEngine::ReleaseResult()
{
   // free record info
   for (int j = 0; j < DB_FETCH_ROW_MAX; j++) {
      for (int i = 0; i < DB_FETCH_COLUMN_MAX; i++) m_nResultRecordInfo[j][i]=0;
      memset(m_pResultRecordData[j], 0, DB_FETCH_RECORD_MAX);
   }

   m_nNumberOfRecords=0;
   m_nNumberOfColumns=0;
}

//----------------------------------------------------------------------------
// Function Name: GetData
// Description  : get data in result set (by row and col)
// Parameters   : int row, int col
// Return Values: char * (if error return null)
//                return data is null terminated string
//----------------------------------------------------------------------------
char *MSSqlEngine::GetData(int row, int col)
{
   int i, pos=0;

   if (row >= m_nNumberOfRecords || col >= m_nNumberOfColumns) {
      WriteLog("invalid args. row[%d], col[%d] (MSSqlEngine::GetData)", row, col);
      return NULL;
   }

   for (i = 0; i < col; i++) pos += m_nResultRecordInfo[row][i];
   return (m_pResultRecordData[row])+pos;
}

//----------------------------------------------------------------------------
// Function Name: WriteLog
// Description  : write log to file
// Parameters   : log string 
// Return Values: none
//----------------------------------------------------------------------------
void MSSqlEngine::WriteLog(const char *Format, ...)
{
  FILE *f=NULL;
  va_list args;
  time_t ct;
  char tmstr[32];

  if (!m_LogFileName[0] || ((f=fopen(m_LogFileName, "a+t"))==NULL)) return;

  // get current time
  ct=time(0);
  strcpy(tmstr,asctime(localtime(&ct))+11);
  tmstr[8]=0;

  if (f) fprintf(f, "%s :: ", tmstr);
  fprintf(stderr, "%s :: ", tmstr);

  va_start(args, Format);
  if (f) vfprintf(f, Format, args);
  vfprintf(stderr, Format, args);
  va_end(args);

  if (f) fprintf(f, "\n");
  fprintf(stderr, "\n");

  if (f) fclose(f);
}

bool MSSqlEngine::NetmarbleQuery(const char* UserIP)
{
	char sql_buf[200];
	int db_ret = 0;
	int ret = 0;
	int contents_no = 0;
	int is_ret = 0;
	snprintf( sql_buf, sizeof(sql_buf), "declare @ORETVAL INT "    "declare @OCONTENTS_NO INT "
													    "exec ADBILLGAME_PLUS_IP_CHECK '%s', @ORETVAL OUTPUT, @OCONTENTS_NO OUTPUT "
																	    "SELECT @ORETVAL, @OCONTENTS_NO", UserIP);
			    
    dbcmd( m_TDSDBProc, sql_buf );
//	QuerySQL(sql_buf);
		
	db_ret = dbsqlexec( m_TDSDBProc );
	
	db_ret = dbresults( m_TDSDBProc );
	db_ret = dbbind( m_TDSDBProc, 1, INTBIND, 0, (BYTE *)&ret );
	db_ret = dbbind( m_TDSDBProc, 2, INTBIND, 0, (BYTE *)&contents_no );
    if( dbnextrow(m_TDSDBProc) == NO_MORE_ROWS )
	{
		is_ret = false;
	}
	else
	{
		if( ret == 1 )
		{
			if( contents_no == 19 )             // 베이직 PC 방
			{
				is_ret = true;
			}
			else
			{
				is_ret = false;
			}
		}
		else
		{
			is_ret = false;
		}
	}

		
	cout << "ret : " << ret << "; contents_no : " << contents_no << endl;

	char log[80];
	sprintf(log, "UserIP : %s, ret : %d, contents_no : %d", UserIP, ret, contents_no);
	WriteLog(log);
	
	return is_ret;
					
}

MSSqlEngine* g_pMSSqlEngine = NULL;
