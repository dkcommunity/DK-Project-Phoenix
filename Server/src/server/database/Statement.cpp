//////////////////////////////////////////////////////////////////////////////
// File Name 	: Statement.cpp
// Written by	: Gday29@ewestsoft.com
// Description	: SQL ���� �����..
//////////////////////////////////////////////////////////////////////////////

#include "Statement.h"
#include <stdio.h>
#include <sys/time.h>
#include <mysql/mysql.h>
#include "Result.h"
#include "Assert.h"
#include "Mutex.h"
#include "Profile.h"
#include <stdarg.h>

#define __FULL_PROFILE__

#ifndef __FULL_PROFILE__
#undef beginProfileEx
#define beginProfileEx(name) ((void)0)
#undef endProfileEx
#define endProfileEx(name) ((void)0)
#endif

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
Statement::Statement() 
	throw ()
{
	__BEGIN_TRY

	m_pConnection   = NULL;
	m_pResult       = NULL;
	m_nAffectedRows = 0;

	__END_CATCH
}

Statement::Statement (char * fmt , ... ) 
	throw (Error )
{
	__BEGIN_TRY

	// variable argument list
	va_list valist;
	
	va_start(valist , fmt );
	
	char buffer[2048];
	
	int nchars = vsnprintf(buffer, 2048, fmt, valist );

	// ���� ���� ũ�Ⱑ �����ϰ� �Ǹ�, ������ ������ �������� ���� �������Ѿ� �Ѵ�.
	if (nchars == -1 || nchars > 2048 )
		throw Error("more buffer size needed for SQL statement buffer...");
	
	va_end(valist);
	
	// string �̹Ƿ� �����ص� �����ϴ�. 
	// ���� char * ���ٸ�, local variable�� �������ϴ� ���� �����ϴ�.
    m_Statement = buffer;
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// destructor
//
// ���������� ���� ����� �޸𸮿��� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////

Statement::~Statement() 
	throw ()
{
	__BEGIN_TRY

	if (m_pResult != NULL ) 
	{
		delete m_pResult;
		m_pResult = NULL;
	}
	
	__END_CATCH
}

    
//////////////////////////////////////////////////////////////////////
//
// executeQuery()
//
// Connection, SQL���� ��Ʈ������ �޾Ƽ� ������� Result *�� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////

Result * Statement::executeQuery () 
	throw (SQLQueryException , Error )
{
	__BEGIN_TRY
		
	Assert(m_pConnection != NULL );

	if (m_pResult != NULL ) 
	{
		// ���ø����̼ǿ��� Result �� �������� ���� ���,
		delete m_pResult;
		m_pResult = NULL;
	}

	beginProfileEx("ZPM_QUERY");

	if (mysql_real_query(m_pConnection->getMYSQL(), m_Statement.c_str() , m_Statement.size() ) != 0 )
	{
		// FAZ 4 (2026-05-06, expanded 2026-05-10): MySQL wait_timeout / inactivity
		// timeout sonrasi connection koparsa reconnect + retry dene.
		//   2006 = CR_SERVER_GONE_ERROR    ("server has gone away")
		//   2013 = CR_SERVER_LOST          ("lost connection during query")
		//   4031 = ER_CLIENT_INTERACTION_TIMEOUT (MySQL 8.0.24+ idle disconnect)
		// MYSQL_OPT_RECONNECT MySQL 8'de etkisiz oldugu icin manuel retry sart.
		// UNHANDLED EXCEPTION ile process'in olmesini engeller.
		unsigned int errcode = mysql_errno( m_pConnection->getMYSQL() );
		bool retry_ok = false;

		if ( errcode == 2006 || errcode == 2013 || errcode == 4031 )
		{
			cout << "Stmt::EQ connection lost (errno=" << errcode << "), reconnecting..." << endl;
			try
			{
				m_pConnection->reconnect();
				cout << "Stmt::EQ reconnect OK, retrying query" << endl;

				if ( mysql_real_query( m_pConnection->getMYSQL(), m_Statement.c_str(), m_Statement.size() ) == 0 )
				{
					retry_ok = true;
				}
			}
			catch ( SQLConnectException& sce )
			{
				cout << "Stmt::EQ reconnect FAILED: " << sce.toString() << endl;
			}
		}

		if ( ! retry_ok )
		{
			cout << "Stmt::EQ real Query Error" << endl;
			cout << "Stmt [" << m_Statement << "]" << endl;
			cout << getError() << endl;

			throw SQLQueryException(getError() );
		}
	}

	MYSQL_RES * pResult = mysql_store_result(m_pConnection->getMYSQL());
	
	// ���� ������� NULL�� ���� update���̰ų� �����̴�.
	if (pResult != NULL)
	{
		m_pResult = new Result(pResult, m_Statement);
	}
	else 
	{
		//if (mysql_num_fields(m_pConnection->getMYSQL() ) != 0 )
		if (mysql_field_count(m_pConnection->getMYSQL() ) != 0 )
		{
			cerr << "Stmt::EQ Unknown Error > " << getError() << endl;

			throw SQLQueryException(getError() );
		}
		else
		{
			m_nAffectedRows = mysql_affected_rows(m_pConnection->getMYSQL());
		}
	}

	endProfileEx("ZPM_QUERY");

	return m_pResult;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// executeQuery ()
//
// Connection, SQL���� ��Ʈ������ �޾Ƽ� ������� Result *�� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////

Result * Statement::executeQuery (const string& sqlStatement )
	throw (SQLQueryException, Error )
{
	__BEGIN_TRY
		
	m_Statement = sqlStatement;

	return executeQuery();

	__END_CATCH
}	
	
//////////////////////////////////////////////////////////////////////
//
// executeQuery ()
//
// Connection, SQL���� ��Ʈ������ �޾Ƽ� ������� Result *�� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////

Result * Statement::executeQuery (char * fmt , ... ) 
	throw (SQLQueryException, Error )
{
	__BEGIN_TRY
		
	// SQL Statement �� ������.
	va_list valist;

	va_start(valist, fmt);

	char buffer[2048+1];

	int nchars = vsnprintf(buffer, 2048, fmt, valist );

	// ���� ���� ũ�Ⱑ �����ϰ� �Ǹ�, ���ܸ� ������ ���� �������Ѿ� �Ѵ�.
	if (nchars == -1 || nchars > 2048 )
		throw Error("more buffer size needed for SQL statement buffer...");

	va_end(valist);

	m_Statement = buffer;

	return executeQuery();

	__END_CATCH
}	
	
//////////////////////////////////////////////////////////////////////
//
//	setStatement()
//
//	�������� �ٽ� �����.
//
//////////////////////////////////////////////////////////////////////

void Statement::setStatement (char * fmt , ... ) 
	throw (Error ) 
{
	__BEGIN_TRY

	// variable argument list
	va_list valist;
	
	va_start(valist, fmt );
	
	// buffer size = 1kb..Too big??
	char buffer[2048+1];
	
	int nchars = vsnprintf(buffer, 2048 , fmt, valist );
	
	// ���� ���� ũ�Ⱑ �����ϰ� �Ǹ�, ���ܸ� ������ ���� �������Ѿ� �Ѵ�.
	if (nchars == -1 || nchars > 2048 )
		throw Error("more buffer size needed for SQL statement buffer...");

	va_end(valist);
	
	m_Statement = buffer;
	
	__END_CATCH	
}

uint Statement::getInsertID() const throw()
{
	__BEGIN_TRY
	return mysql_insert_id( m_pConnection->getMYSQL() );
	__END_CATCH
}
