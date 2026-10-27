#include "SMSServiceThread.h"
#include "Properties.h"
#include "VariableManager.h"

#include "Assert.h"
#include "StringStream.h"
#include "Timeval.h"

#include <unistd.h>

#define KEY_SIZE 32

string SMSMessage::toString() const
{
	StringStream msg;

	msg << "SMSMessage("
		<< m_SenderName << ", "
		<< m_ReceiverNumber << ", "
		<< m_CallerNumber << ", "
		<< m_Message << ")";

	return msg.toString();
}

void operator++ (string& id)
{
	cout << "start operator++" << id << endl;

	string::iterator itr = id.end();
	--itr;

	while ( (*itr) == '9' && itr != id.begin() )
	{
		(*itr) = '0';
		--itr;
	}

	Assert( (*itr) <= '8' && (*itr) >= '0' );
	++(*itr);

	cout << "end operator++" << id << endl;
}

void SMSServiceThread::clear()
{
	processMessages();

	SAFE_DELETE( m_pConnection );

	__ENTER_CRITICAL_SECTION( m_QueueMutex )

	while ( !m_MessageQueue.empty() )
	{
		SMSMessage* pMessage = m_MessageQueue.front();
		SAFE_DELETE( pMessage );
		m_MessageQueue.pop_front();
	}

	m_MessageQueue.clear();

	__LEAVE_CRITICAL_SECTION( m_QueueMutex )
}

void SMSServiceThread::run() throw()
{
	__BEGIN_TRY

	cout << "smsservice........................................" << endl;
	m_Host		= g_pConfig->getProperty("SMS_DB_HOST");
	m_DB		= g_pConfig->getProperty("SMS_DB_DB");
	m_User		= g_pConfig->getProperty("SMS_DB_USER");
	m_Password	= g_pConfig->getProperty("SMS_DB_PASSWORD");
	m_Port		= 0;
	if ( g_pConfig->hasKey("SMS_DB_PORT") )
		m_Port = g_pConfig->getPropertyInt("SMS_DB_PORT");

    try
    {
        m_pConnection = new Connection( m_Host, m_DB, m_User, m_Password, m_Port );
    }
	catch(SQLConnectException& e)
	{
		cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
        filelog( "SMSThreadException.log", "SQLQueryException:%s", e.toString().c_str() );

        return;
    }
	catch(Exception& e)
	{
		cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
        filelog( "SMSThreadException.log", "SQLQueryException:%s", e.toString().c_str() );
        
        return;
	}

	uint Dimension	= g_pConfig->getPropertyInt("Dimension");
	uint WorldID	= g_pConfig->getPropertyInt("WorldID");
	uint ServerID	= g_pConfig->getPropertyInt("ServerID");

	Assert( Dimension<10 );
	Assert( WorldID<10 );
	Assert( ServerID<10 );

	{
		char buffer[KEY_SIZE+1];
		buffer[0] = Dimension + '0';
		buffer[1] = WorldID + '0';
		buffer[2] = ServerID + '0';
		for ( int i=3; i<KEY_SIZE; ++i ) buffer[i] = '0';
		buffer[KEY_SIZE] = 0;
		m_MID = buffer;
	}

	Statement* pStmt = NULL;

	try
	{
//		BEGIN_DB
		{
			cout << "SMS Service - Get Last Message ID" << endl;

			pStmt = m_pConnection->createStatement();
			Result* pResult = pStmt->executeQuery( "SELECT MAX(mid) FROM uds_msg WHERE mid LIKE '%c%c%c%%' AND length(mid)=%d", Dimension+'0', WorldID+'0', ServerID+'0', KEY_SIZE );

			cout << "smsservice2222222222222222222222222222222222222222222" << endl;

			string max;
			if ( pResult->next() ) max = pResult->getString(1);
			if ( max.size() == KEY_SIZE ) m_MID = max;
			++m_MID;
			cout << "initial mid : " << m_MID << endl;;

//			SAFE_DELETE( pStmt );
		}
//		END_DB(pStmt);
	}
	catch(SQLQueryException& e)
	{
		cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
		filelog( "SMSThreadException.log", "SQLQueryException:%s", e.toString().c_str() );

		return;
	}
	catch(Exception& e)
	{
		cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
		filelog( "SMSThreadException.log", "SQLQueryException:%s", e.toString().c_str() );
		
		return;
	}
	
	SAFE_DELETE( pStmt );

	Timeval dummyQueryTime;
	getCurrentTime( dummyQueryTime );
	
	while ( g_bRunning )
	{
		try
		{
			processMessages();

			Timeval currentTime;
			getCurrentTime(currentTime);

			if (dummyQueryTime < currentTime)
			{
				g_pDatabaseManager->executeDummyQuery( m_pConnection );

				// 15분 ~ 20분  사이에서 dummy query 시간을 설정한다.
				// timeout이 되지 않게 하기 위해서이다.
				dummyQueryTime.tv_sec += (15+rand()%5) * 60;
			}

			sleep(1); // 1초에 한번씩 큐를 체크한다.
		}
		catch(SQLConnectException& e)
		{
			delete pStmt;

			cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
			filelog( "SMSThreadException.log", "SQLConnectionException:%s", e.toString().c_str() );
		}
		catch(SQLQueryException& e)
		{
			delete pStmt;

			cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
			filelog( "SMSThreadException.log", "SQLQueryException:%s", e.toString().c_str() );
		}		
		catch(Throwable& t)
		{
			filelog( "SMSThreadException.log", "Throwable:%s", t.toString().c_str() );

		 	return;

		}
	}

	__END_CATCH
}

void SMSServiceThread::processMessages()
	throw()
{
	__ENTER_CRITICAL_SECTION( m_QueueMutex )

	if ( !m_MessageQueue.empty() )
	{
		list<SMSMessage*>::const_iterator itr = m_MessageQueue.begin();
		list<SMSMessage*>::const_iterator endItr = m_MessageQueue.end();

		for ( ; itr != endItr ; ++itr )
		{
			SMSMessage* pMsg = *itr;

			if ( pMsg != NULL )
			{
				Statement* pStmt = NULL;

				//BEGIN_DB
				try
				{
					//cout << pMsg->toString() << endl;
					filelog( "SMS.log", "Send message [%s] %s", m_MID.c_str(), pMsg->toString().c_str() );
					pStmt = m_pConnection->createStatement();
					pStmt->executeQuery( "INSERT INTO uds_msg (mid,recvdate,target,toname,callback,body) VALUES "
							"('%s',now(),'%s','%s','%s','%s')",
							m_MID.c_str(),
							pMsg->m_ReceiverNumber.c_str(),
							pMsg->m_SenderName.c_str(),
							pMsg->m_CallerNumber.c_str(),
							getDBString(pMsg->m_Message).c_str() );

					if ( pStmt->getAffectedRowCount() != 0 )
					{
						pStmt->executeQuery( "INSERT INTO msg_queue (mid) VALUES ('%s')", m_MID.c_str() );
						filelog( "SMS.log", "insert queue %s", m_MID.c_str() );

//						cout << mid << " message sent!" << endl;
						++m_MID;
						//mid++;
					}

					SAFE_DELETE( pStmt );
				}
				//END_DB(pStmt)
				catch(SQLConnectException& e)
				{
					delete pStmt;

					cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
					filelog( "SMSThreadException.log", "SQLConnectionException:%s", e.toString().c_str() );
				}
				catch(SQLQueryException& e)
				{
					delete pStmt;

					cout << "SMS Service : " << e.getName() << " : " << e.toString() << endl;
					filelog( "SMSThreadException.log", "SQLQueryException:%s", e.toString().c_str() );
				}
				catch(Throwable& t)
				{
					delete pStmt;

					cout << "SMS Service : " << t.getName() << " : " << t.toString() << endl;
					filelog( "SMSThreadException.log", "Throwable:%s", t.toString().c_str() );
				}
			}

			SAFE_DELETE( pMsg );
		}

		m_MessageQueue.clear();
	}

	__LEAVE_CRITICAL_SECTION( m_QueueMutex )
}

string SMSServiceThread::getDBString( const string& msg ) const
{
	string ret = "";

	string::const_iterator itr = msg.begin();
	string::const_iterator endItr = msg.end();

	for ( ; itr != endItr ; ++itr )
	{
		char ch = *itr;

		if ( ch == '\'' || ch=='\\' )
		{
			ret += '\\';
		}

		ret += ch;
	}

	return ret;
}

bool SMSServiceThread::isValidNumber( const string& num ) const
{
	if ( num.size() > 11 || num.size() < 9 ) return false;

	string::const_iterator itr = num.begin();
	string::const_iterator endItr = num.end();

	if ( (*itr) != '0' ) return false;

	itr++;

	if ( (*itr) != '1' ) return false;

	for ( ; itr != endItr ; ++itr )
	{
		if ( (*itr) < '0' || (*itr) > '9' ) return false;
	}

	return true;
}

void SMSServiceThread::pushMessage( SMSMessage* pMsg )
{
	__ENTER_CRITICAL_SECTION( m_QueueMutex )

	m_MessageQueue.push_back( pMsg );

	__LEAVE_CRITICAL_SECTION( m_QueueMutex )
}
