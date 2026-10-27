//////////////////////////////////////////////////////////////////////
//
// File Name 	: Connection.h
// Written by	: Gday29@ewestsoft.com
// Description	: ����Ÿ���̽����� ������ ����ϴ� Ŭ����
//
//////////////////////////////////////////////////////////////////////

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

// include files
#include "Types.h"
#include "Exception.h"
#include <mysql/mysql.h>
#include "Mutex.h"

// forward declaration
class Statement;

//////////////////////////////////////////////////////////////////////
//
// class Connection;
//
// ������ ���̽��� �����ϰ�, �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////

class Connection {

public:
	
	// constructor
    Connection() throw(Error);

	// constructor(1-time connection)
    Connection(string host, string db, string user, string password, uint port = 0) throw(SQLConnectException, Error);
    
	// destructor
	~Connection() throw(Error);

    // close the connection to database
	void close() throw(SQLConnectException, Error);
	
	// ����Ÿ���̽��� ������ �õ��Ѵ�.
	void connect(string host, string db, string user, string password, uint port = 0) throw(SQLConnectException);

	// ����Ÿ���̽��� ������ �õ��Ѵ�.
	void connect() throw(SQLConnectException);

	// FAZ 4 (2026-05-06): MySQL 8'de MYSQL_OPT_RECONNECT etkisiz — Statement
	// tarafindan CR_SERVER_GONE_ERROR/CR_SERVER_LOST yakalandiginda manuel
	// reconnect icin kullanilir. mysql_close + mysql_init + mysql_real_connect.
	void reconnect() throw(SQLConnectException);

	// check the connection
	bool isConnected() const throw() { return m_bConnected; }
	bool operator !() const throw() { return m_bConnected == false; }
	
	// Statement ��ü�� �����ؼ� �����Ѵ�.
	Statement * createStatement() throw();	
	
	// get the MYSQL object
	MYSQL * getMYSQL() throw() { return &m_Mysql; }
	
	// get MS's host name(ip)
	string getHost() const throw() { return m_Host; }

	// get MS's service port
	uint getPort() const throw() { return m_Port; }

	// get database name
	string getDatabase() const throw() { return m_Database; }

	// get user id 
	string getUser() const throw() { return m_User; }

	// get user password
	string getPassword() const throw() { return m_Password; }

	// get connection's name
	string getName() const throw() { return m_Name; }

	// set connection's name
	void setName(string name) throw() { m_Name = name; }

	// get/set busy status
	bool isBusy(void) const throw() { return m_bBusy;}
	void setBusy(bool busy=true) throw() { m_bBusy = busy;}

	// get error
	string getError() throw() { return mysql_error(&m_Mysql); }

	// lock/unlock
	void lock() throw(Error) { m_Mutex.lock(); }
	void unlock() throw(Error) { m_Mutex.unlock(); }
	
private:
	
	// ??
    MYSQL m_Mysql;

	// is connected?
	bool m_bConnected;

	// DBMS �� ��Ǵ� ȣ��Ʈ ��
	string m_Host;

	// DBMS �� ���� ��Ʈ
	uint m_Port;

	// ����Ÿ���̽� �̸�
	string m_Database;

	// ����� ���̵�
	string m_User;

	// ����� �н�����
	string m_Password;

	// connection name(DatabaseManager ���� ����� Ű��)
	string m_Name;

	// ���� ������� Ŀ�ؼ��ΰ�?
	bool m_bBusy;

	// ���� ������ ���� ���� lock
	Mutex m_Mutex;

};

#endif // __CONNECTION_H__
