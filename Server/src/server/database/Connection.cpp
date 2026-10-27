//////////////////////////////////////////////////////////////////////

//

// File Name	: Connection.cpp

// Written by	: Gday29@ewestsoft.com

// Description	: Ŀ�ؼ� Ŭ���� ����

//

//////////////////////////////////////////////////////////////////////



// include files

#include "Connection.h"

#include "Statement.h"



//////////////////////////////////////////////////////////////////////

//

// constructor

//

// ������ ���̽��� �����ϰ�, �����ͺ��̽��� �����Ѵ�.

// ���ܸ� ������ �κ��� �����ؾ� �Ѵ�.

//

//////////////////////////////////////////////////////////////////////



Connection::Connection ()

	throw ( Error )

: m_bConnected(false), m_bBusy(false)

{

	__BEGIN_TRY



	m_Mutex.setName("Connection");



	// MYSQL ��ü�� �ʱ�ȭ�Ѵ�.

	if ( mysql_init(&m_Mysql) == NULL ) {

		throw Error( mysql_error(&m_Mysql) );

	}



	__END_CATCH

}



//////////////////////////////////////////////////////////////////////

//

// *CAUTION*

//

// connect()�� ȣ���� �� port �� �������� ���� ���, ����Ʈ���� 0 �� ���Ǹ�

// �̶� MYSQL�� ����Ʈ ��Ʈ�� ���ȴ�. �׷�����, ������ ���ؼ� MYSQL�� �ٸ�

// ��Ʈ���ٰ� ������� �� ���̴�. ����, ��Ʈ�� ������ �ʿ䰡 �ִ�.

//

//////////////////////////////////////////////////////////////////////



Connection::Connection ( string host , string db , string user , string password , uint port )

	throw ( SQLConnectException , Error )

: m_bConnected(false), m_Host(host), m_Port(port), m_Database(db), m_User(user), m_Password(password)

{

	__BEGIN_TRY



	m_Mutex.setName("Connection");

		

	// MYSQL ��ü�� �ʱ�ȭ�Ѵ�.

	if ( mysql_init(&m_Mysql) == NULL )

		throw Error( mysql_error(&m_Mysql) );



	// �ٷ� ������ �õ��Ѵ�.

	connect( m_Host , m_Database , m_User , m_Password , m_Port );

	

	__END_CATCH

}



//////////////////////////////////////////////////////////////////////

//

//	destructor

//

//////////////////////////////////////////////////////////////////////



Connection::~Connection() 

	throw ( Error )

{

	__BEGIN_TRY

		

	// ����Ǿ� ���� ���, ������ ���´�.

    if ( m_bConnected ) {

		close();

    }

	

	__END_CATCH

}



//////////////////////////////////////////////////////////////////////

//

// connect()	

//

// Connection ��ü�� ������ ��, ��� ������ �� ���� ���� ���

// �ϴ� default constructor �� ����ؼ� ��ü�� ������ ����, 

// ������ �� ���� �޾Ƽ� ���� �����͸������ �ʱ�ȭ�� �� �����Ѵ�. 

//

// *CAUTION*

//

// connect()�� ȣ���� �� port �� �������� ���� ���, ����Ʈ���� 0 �� ���Ǹ�

// �̶� MYSQL �� ����Ʈ ��Ʈ�� ���ȴ�. �׷�����, ������ ���ؼ� MYSQL�� �ٸ�

// ��Ʈ���ٰ� ������� �� ���̴�.

//

// ����Ʈ �Ķ���͸� ����ؾ� �ϱ� ������, ��Ʈ �Ķ���Ͱ� �� �ڷ� ����.. *_*;

//

// ex> Connection conn;

//     conn.connect( "vampire.ewestsoft.com" , "bbs" , "bbsuser" , "bbspassword" );

//

//////////////////////////////////////////////////////////////////////



void Connection::connect ( string host , string db , string user , string password , uint port )

	throw ( SQLConnectException )

{

	__BEGIN_TRY



	m_Host = host;

	m_Port = port;

	m_Database = db;

	m_User = user;

	m_Password = password;



	connect();



	__END_CATCH

}

	

//////////////////////////////////////////////////////////////////////

//

// connect()	

//

// �����ڿ��� ����  ������ �Է¹��� ���, �ٷ� �� �޽�带 ȣ���ؼ�

// ������ �õ��Ѵ�.

//

//////////////////////////////////////////////////////////////////////



void Connection::connect ()

	throw ( SQLConnectException )

{

	__BEGIN_TRY

		

	// ������ �Ϸ��� �ϴµ� �̹� ������ �Ǿ��ִٸ� ������.

	if ( m_bConnected ) 

	{

		// �̷� ���, ������ �ݾ� �ְ� ���ܸ� ������ �� ���̴�.

		close();



		throw SQLConnectException("Already Connected"); 

	}



	//--------------------------------------------------

	// ������ ������� üũ�ؾ� ��...

	//--------------------------------------------------

	// FAZ 4 (2026-04-29): MYSQL_OPT_RECONNECT.
	// NOT (2026-05-06): MySQL 8.0+ bu seceneği deprecate etti, default 0 ve
	// set edilse bile etkisiz. Production'da gameserver wait_timeout sonrasi
	// "Stmt [SELECT 1]" + "wait_timeout" + UNHANDLED EXCEPTION ile coktu.
	// Gercek reconnect Connection::reconnect() + Statement::executeQuery()
	// CR_SERVER_GONE_ERROR/CR_SERVER_LOST handler'inda yapiliyor.
	// 2026-05-11: MySQL 8.0.34+ icin deprecation warning bastirildi. Eski
	// MySQL build'lerinde (5.x/8.0.33-) hala kullanilir; modern MySQL'de
	// derleme zamani guard ile atlanir (etkisiz oldugu icin kayip yok).
#if !defined(MYSQL_VERSION_ID) || MYSQL_VERSION_ID < 80034
	{
		char reconnect = 1;
		mysql_options( &m_Mysql, MYSQL_OPT_RECONNECT, &reconnect );
	}
#endif

	m_bConnected = ( mysql_real_connect( &m_Mysql, m_Host.c_str(), m_User.c_str(), m_Password.c_str(), m_Database.c_str(), m_Port , 0, 0 ) != NULL );

	//cout << "Connection Calls~~~" << endl;

	

	// ������°� �ƴ϶�� ������.

	if( ! m_bConnected ) 

	{

		throw SQLConnectException( mysql_error(&m_Mysql) );

	}

	

	__END_CATCH

}



//////////////////////////////////////////////////////////////////////
//
// reconnect()
//
// FAZ 4 (2026-05-06): MySQL 8'de wait_timeout kopuk connection'i
// kurtarmak icin. mysql_close + mysql_init + mysql_real_connect
// chain'i. Statement::executeQuery() icinden CR_SERVER_GONE_ERROR (2006)
// veya CR_SERVER_LOST (2013) yakalandiginda cagrilir.
//
//////////////////////////////////////////////////////////////////////

void Connection::reconnect ()
	throw ( SQLConnectException )
{
	__BEGIN_TRY

	if ( m_bConnected )
	{
		mysql_close( &m_Mysql );
		m_bConnected = false;
	}

	if ( mysql_init( &m_Mysql ) == NULL )
	{
		throw SQLConnectException( "mysql_init failed during reconnect" );
	}

	// 2026-05-11: MYSQL_OPT_RECONNECT MySQL 8.0.34+'da deprecated + etkisiz.
	// Manuel reconnect zaten burada yapiliyor; opt cagrisi eski MySQL ile
	// build edilirse korunur (versiyon guard).
#if !defined(MYSQL_VERSION_ID) || MYSQL_VERSION_ID < 80034
	{
		char reconnect_opt = 1;
		mysql_options( &m_Mysql, MYSQL_OPT_RECONNECT, &reconnect_opt );
	}
#endif

	m_bConnected = ( mysql_real_connect( &m_Mysql, m_Host.c_str(), m_User.c_str(), m_Password.c_str(), m_Database.c_str(), m_Port, 0, 0 ) != NULL );

	if ( ! m_bConnected )
	{
		throw SQLConnectException( mysql_error( &m_Mysql ) );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//

//	close()

//

//	������ ���´�.

//

//////////////////////////////////////////////////////////////////////



void Connection::close () 

	throw ( SQLConnectException , Error )

{

	__BEGIN_TRY

		

    if( ! m_bConnected ) 

	{

		throw SQLConnectException("Not Connected");

    }

	

	// void �̹Ƿ� ����.. - -;

    mysql_close( &m_Mysql );



	m_bConnected = false;

	

	__END_CATCH

}



//////////////////////////////////////////////////////////////////////

//	

//	createStatement

//

//	���ο� Statement ��ü�� ����� �� ��ü�� �����͸� �����Ѵ�.

//

//////////////////////////////////////////////////////////////////////



Statement * Connection::createStatement ()

	throw ()

{

	__BEGIN_TRY

		

	// ���ο� Statement ��ü�� �����.

	Statement * pStatement = new Statement();

	

	// ���� ��ü�� Ŀ�ؼ� ���� �� Ŀ�ؼ� ��ü�� �ϰ�..

	pStatement->setConnection(this);

	

	// �����Ѵ�.

	return pStatement;

	

	__END_CATCH

}

