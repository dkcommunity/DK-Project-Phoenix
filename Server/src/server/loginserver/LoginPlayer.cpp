//////////////////////////////////////////////////////////////////////
// 
// Filename    : LoginPlyaer.cpp 
// Written By  : Reiot
// Description :
// 
//////////////////////////////////////////////////////////////////////

#include "LoginPlayer.h"
#include "Assert.h"
#include "Packet.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "DB.h"
#include "LogClient.h"
#include "Profile.h"
#include "PacketProfile.h"
#include "GameServerInfoManager.h"
#include "GameServerManager.h"
#include "gameserver/billing/BillingPlayerManager.h"

#include "Lpackets/LCLoginOK.h"
#include "Lpackets/LCLoginError.h"
#include "Lpackets/LGKickCharacter.h"
#include "Lpackets/LCPCList.h"
#include "Shape.h"

// by sigi. 2002.11.12
const int defaultLoginPlayerInputStreamSize = 1024;
const int defaultLoginPlayerOutputStreamSize = 4096;

static int maxIdleSec = 60 * 15;		// 15 �е��� �Է��� ���� ������ �ڵ� ���� ����ȴ�.

// '�̹� ���� ��'������ �ذ��ϱ� ����.. �ð� üũ
static uint maxWaitForKickCharacter = 3;			// GameServer�� ������ 5�ʰ� ��ٸ���.
static uint maxWaitForKickCharacterCount = 3;	// GameServer�� ������ ������ 3ȸ ������ �õ��Ѵ�.


// CLLoginHandler.cpp�� �ִ� �Լ���.
void addLoginPlayerData(const string& ID, const string& ip, const string& SSN, const string& zipcode);
void addLogoutPlayerData(Player* pPlayer);


//////////////////////////////////////////////////////////////////////
//
// constructor
//
//////////////////////////////////////////////////////////////////////
LoginPlayer::LoginPlayer (Socket * pSocket)
	 throw ( Error )
: //Player(pSocket), 
	m_PlayerStatus(LPS_NONE), m_FailureCount(0)
{
	__BEGIN_TRY

	Assert( pSocket != NULL );
	m_pSocket = pSocket;

	// create socket input stream
	m_pInputStream = new SocketInputStream( m_pSocket, defaultLoginPlayerInputStreamSize );

	Assert( m_pInputStream != NULL );
	
	// create socket output stream
	m_pOutputStream = new SocketOutputStream( m_pSocket, defaultLoginPlayerOutputStreamSize );
	Assert( m_pOutputStream != NULL );


	m_Mutex.setName("LoginPlayer");

	m_ID = "NONE";
		
	Assert( m_PacketHistory.empty() );

	// �α��� �÷��̾ ������ ��, ���� �ð��� ���� �Է� �ð����� �����Ѵ�.
	getCurrentTime(m_ExpireTime);
	m_ExpireTime.tv_sec += maxIdleSec;

	m_bSetWorldGroupID = false;
	m_WorldID = 1;
	m_ServerGroupID = 0;
	m_LastSlot = 0;

	m_isAdult = true;

	m_KickCharacterCount = 0;

	m_bFreePass = false;

	m_bAgree = true;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//
//////////////////////////////////////////////////////////////////////
LoginPlayer::~LoginPlayer ()
	 throw ( Error )
{
	__BEGIN_TRY
		
	// �� � �÷��̾� ��ü�� ������ ������, �� ���´� �α׾ƿ��̾�� �Ѵ�.
	// �� � �÷��̾ ���� ���� ��Ű����, �� ���¸� �α׾ƿ����� ������ �Ѵ�.
	Assert( m_PlayerStatus == LPS_END_SESSION );

	// delete all previous packets
	while ( ! m_PacketHistory.empty() ) {
		Packet * pPacket = m_PacketHistory.front();
		delete pPacket;
		m_PacketHistory.pop_front();
	}
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// '�̹� ���� ��'�� ���. ĳ������ ���� ���� ������ ���ؼ�
// ����ϴ� �ð� ����.
//
//////////////////////////////////////////////////////////////////////
void
LoginPlayer::setExpireTimeForKickCharacter()
	throw ()
{
	getCurrentTime(m_ExpireTimeForKickCharacter);

	m_ExpireTimeForKickCharacter.tv_sec += maxWaitForKickCharacter;
}

//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void LoginPlayer::processCommand ( bool Option ) 
     throw ( IOException , Error )
{
	__BEGIN_TRY

//	static Timeval currentTime;

	// '�̹� ���� ��'�� ���.. ���� ���� ������ ��ų���� �� ��.
	if (m_PlayerStatus==LPS_WAITING_FOR_GL_KICK_VERIFY)
	{
		Timeval currentTime;
		getCurrentTime(currentTime);

		// timeout üũ
		if (currentTime >= m_ExpireTimeForKickCharacter)
		{
			// �ٽ� KickCharcter�� ��������.
			sendLGKickCharacter();

			// ������ ���� ��� ������ �õ��� �غ���.
			// �Ѱ迡 �����ϸ�.. GameServer�� �׾��ٰ� �Ǵ��ϰ� LoginOK�� ������.
			if (++ m_KickCharacterCount >= maxWaitForKickCharacterCount)
			{
				sendLCLoginOK();
			}
		}

		return;
	}

	try 
	{
		sPacketHeader header;
		
		PacketID_t packetID;
		PacketSize_t packetSize;
		Packet * pPacket;

		// �Է¹��ۿ� ����ִ� ������ ��Ŷ���� ������ ó���Ѵ�.
		while ( true ) 
		{
			// �Է½�Ʈ������ ��Ŷ���ũ�⸸ŭ �о��.
			// ���� ������ ũ�⸸ŭ ��Ʈ������ ���� �� ���ٸ�,
			// Insufficient ���ܰ� �߻��ϰ�, ������ ����������.
			//if (!m_pInputStream->peek( header , szPacketHeader ))
			if (!m_pInputStream->peek( (char *)&header , sizeof(sPacketHeader) ))
			{
				// �Է��� �ƹ� �͵� �����ٸ�, �Է����� �ð��� �ʰ��ߴ��� üũ�Ѵ�.
				Timeval currentTime;
				getCurrentTime(currentTime);
				if ( currentTime >= m_ExpireTime )
					throw DisconnectException("���� �ð����� �Է����� ������ ������ ����˴ϴ�.");
				break;
			}

			// ��Ŷ���̵� �� ��Ŷũ�⸦ �˾Ƴ���.
			// �̶� ��Ŷũ��� ����� �����Ѵ�.
			packetID = header.packetID;
			packetSize = header.packetSize;

			/*
			LOG4("RECV PACKET from %s, %s(%d) %d/%d\n",
				m_ID.c_str(),
				g_pPacketFactoryManager->getPacketName( packetID ).c_str(),
				packetID,
				szPacketHeader + packetSize,
				m_pInputStream->length()
			);

			StringStream msg;
			msg << "RECV PACKET from " << m_ID << ", " 
				<< g_pPacketFactoryManager->getPacketName( packetID ) << "(" << packetID << ") " 
				<< szPacketHeader + packetSize << "/" << m_pInputStream->length() << eos;
			cout << msg.toString() << endl;
			*/
			
			// ��Ŷ ���̵� �̻��ϸ� �������� ������ �����Ѵ�.
			if ( packetID >= Packet::PACKET_MAX )
			{
				StringStream securityLog;
				securityLog << "event=too_large_packet_id"
					<< " remote=" << m_pSocket->getHost()
					<< " port=" << m_pSocket->getPort()
					<< " player=" << m_ID
					<< " status=" << (int)getPlayerStatus()
					<< " packetID=" << packetID
					<< " packetSize=" << packetSize
					<< " inputLen=" << m_pInputStream->length()
					<< eos;

				filelog("phoenix_packet_abuse.log", "%s", securityLog.toString().c_str());

				// ������� ���ؼ� ������ ��ü������ ǥ���صд�.
				throw InvalidProtocolException("too large packet id");
			}

			try 
			{
				// ��Ŷ�� ������ valid ���� üũ�Ѵ�.
				if ( ! g_pPacketValidator->isValidPacketID( getPlayerStatus() , packetID ) )
				{
					StringStream msg;
					msg << "login invalid packet order"
						<< " player=" << m_ID
						<< " remote=" << m_pSocket->getHost() << ":" << m_pSocket->getPort()
						<< " status=" << (int)getPlayerStatus()
						<< " packetID=" << packetID
						<< " packetName=" << g_pPacketFactoryManager->getPacketName(packetID)
						<< " packetSize=" << packetSize
						<< " maxPacketSize=" << g_pPacketFactoryManager->getPacketMaxSize(packetID)
						<< " inputLen=" << m_pInputStream->length()
						<< eos;

					cout << "[LoginPacketOrder] " << msg.toString() << endl;
					filelog("login_packet_order.txt", "%s", msg.toString().c_str());
					filelog("phoenix_packet_abuse.log",
						"event=login_invalid_packet_order remote=%s port=%d player=%s status=%d packetID=%d packetName=%s packetSize=%u maxPacketSize=%u inputLen=%u",
						m_pSocket->getHost().c_str(),
						m_pSocket->getPort(),
						m_ID.c_str(),
						(int)getPlayerStatus(),
						packetID,
						g_pPacketFactoryManager->getPacketName(packetID).c_str(),
						packetSize,
						g_pPacketFactoryManager->getPacketMaxSize(packetID),
						m_pInputStream->length());
					throw InvalidProtocolException(msg.toString());
				}
				
				// ��Ŷ ũ�Ⱑ �ʹ� ũ�� �������� ������ �����Ѵ�.
				if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize( packetID ) )
                                     {
                                     StringStream msg;

		                       msg << "too large packet size(" << packetID << ")";

		                   throw InvalidProtocolException(msg.toString());}

				   //throw InvalidProtocolException("too large packet size");
				
				// �Է¹��۳��� ��Ŷũ�⸸ŭ�� ����Ÿ�� ����ִ��� Ȯ���Ѵ�.
				// ����ȭ�� break �� ����ϸ� �ȴ�. (���⼭�� �ϴ� exception�� �� ���̴�.)
				//if ( m_pInputStream->length() < szPacketHeader + packetSize ) 
				if ( m_pInputStream->length() < sizeof(sPacketHeader) + packetSize )
				//	throw InsufficientDataException();
					break;
	
				// �����Է½ð��� �����Ѵ�. 
				// �����Է½ð��� ��Ŷ �ϳ��� �����ϰ� ������ �ð��� �ǹ��Ѵ�.
				getCurrentTime(m_ExpireTime);
				m_ExpireTime.tv_sec += maxIdleSec;
	
				// ������� �Դٸ� �Է¹��ۿ��� ������ ��Ŷ �ϳ� �̻��� ����ִٴ� ���̴�.
				// ��Ŷ���丮�Ŵ����κ��� ��Ŷ���̵� ����ؼ� ��Ŷ ��Ʈ��ó�� �����ϸ� �ȴ�.
				// ��Ŷ���̵� �߸��� ���� ��Ŷ���丮�Ŵ������� ó���Ѵ�.
				pPacket = g_pPacketFactoryManager->createPacket( packetID );
	
				m_pInputStream->skip( szPacketHeader );
				pPacket->readTimeStamp(*m_pInputStream);
				
				// ���� �� ��Ŷ��Ʈ��ó�� �ʱ�ȭ�Ѵ�.
				// ��Ŷ����Ŭ������ ���ǵ� read()�� virtual ��Ŀ���� ���ؼ� ȣ��Ǿ�
				// �ڵ������� �ʱ�ȭ�ȴ�.
				m_pInputStream->readPacket( pPacket, false );

				Timeval start, end;
				getCurrentTime(start);
				
				// ���� �� ��Ŷ��Ʈ��ó�� ������ ��Ŷ�ڵ鷯�� �����ϸ� �ȴ�.
				// ��Ŷ���̵� �߸��� ���� ��Ŷ�ڵ鷯�Ŵ������� ó���Ѵ�.
				pPacket->execute( this );

				getCurrentTime(end);
				g_PacketProfileManager.addAccuTime(pPacket->getPacketName(), start, end);
				
				// ���� ��Ŷ�� ��Ŷ �����丮�� �� �ڿ� �ִ´�.
				m_PacketHistory.push_back(pPacket);
	
				// ��Ŷ�� nPacketHistory ����ŭ�� �����Ѵ�.
				while ( m_PacketHistory.size() > nPacketHistory ) {
					Packet * oldPacket = m_PacketHistory.front();
					delete oldPacket;
					m_PacketHistory.pop_front();
				}

			} 
			catch ( IgnorePacketException& ) 
			{
                // PacketValidator ���� ��Ŷ�� �����϶�� ������,
                // �Է½�Ʈ������ ��� ���������� �������� �ʵ��� �Ѵ�.

                // ��Ŷ ũ�Ⱑ �ʹ� ũ�� �������� ������ �����Ѵ�.
                if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID) )
                    throw InvalidProtocolException("too large packet size");

                // �Է¹��۳��� ��Ŷũ�⸸ŭ�� ����Ÿ�� ����ִ��� Ȯ���Ѵ�.
                // ����ȭ�� break �� ����ϸ� �ȴ�. (���⼭�� �ϴ� exception�� ?���̴�.)
                //if ( m_pInputStream->length() < szPacketHeader + packetSize )
                if ( m_pInputStream->length() < sizeof(sPacketHeader) + packetSize )
                    throw InsufficientDataException();

                // ����Ÿ�� ��� ����������, �� ũ�⸸ŭ �����ϰ�,
                // �ٸ� ��Ŷ�� ó���ϵ��� �Ѵ�....
                //m_pInputStream->skip( szPacketHeader + packetSize );
                m_pInputStream->skip( sizeof(sPacketHeader) + packetSize );

                // ���õ� ��Ŷ��, expire �� ������ ���� �ʰ� �ȴ�.
                // �� ��ȿ�� ��Ŷ���� ©���� �ʰ� ���ش�.
                // ���� �����丮���� ���� �ʴ´�.

			}
		}

	} catch ( InsufficientDataException & ide ) {

		// �Է��� �ƹ� �͵� �����ٸ�, �Է����� �ð��� �ʰ��ߴ��� üũ�Ѵ�.
		Timeval currentTime;
		getCurrentTime(currentTime);
		if ( currentTime >= m_ExpireTime )
			throw DisconnectException("���� �ð����� �Է����� ������ ������ ����˴ϴ�.");

	} catch ( InvalidProtocolException & ipe ) {

		// ������ ����������Ѿ� �Ѵ�. ���� �������??
		throw;
		
	} catch ( DisconnectException & de ) {

		// ��Ŷ ó������ �߻��� � ������ ������ �����ؾ� �Ѵ�.
		throw;
	}

	__END_CATCH
}
		    

//--------------------------------------------------------------------------------
// disconnect player
//--------------------------------------------------------------------------------
void LoginPlayer::disconnect ( bool bDisconnected )
	throw ( Error )
{
	__BEGIN_TRY

	if ( bDisconnected == UNDISCONNECTED ) 
	{
		// Ŭ���̾�Ʈ���� GCDisconnect ��Ŷ�� �����Ѵ�.
		//GCDisconnect lcDisconnect;
		//sendPacket( lcDisconnect );

		// ��� ���ۿ� �����ִ� ����Ÿ�� �����Ѵ�.
		m_pOutputStream->flush();
	}

	// ���� ������ �ݴ´�.
	m_pSocket->close();

	// '�̹� ���� ��'�� ���, ĳ���� ���� ���� ������ ��ٸ��� ��Ȳ.
	if (m_PlayerStatus==LPS_WAITING_FOR_GL_KICK_VERIFY)
	{
		m_ID = "NONE";
	}

	// �÷��̾��� ���¸� �α׾ƿ����� �����.
	Assert( m_PlayerStatus != LPS_END_SESSION );
	m_PlayerStatus = LPS_END_SESSION;

	// ���̵� �����Ǿ��ٴ� ����, �α����� �̷�����ٴ� ���̴�.
	// '�̹� ���� ��'�� ��쿡.. 
	// ĳ�� ���� ������ ��ٸ��� ���� ID�� ������ �� �����Ƿ� �ƴϴ�
	if ( m_ID != "NONE" )
	{
		Statement* pStmt   = NULL;
		//Result*    pResult = NULL;

		try
		{
			pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		// query�ؼ� �� ���淡.. ���ȶ�.. by sigi. 2002.5.7
		//	pResult = pStmt->executeQuery( "SELECT LogOn FROM Player WHERE PlayerID='%s'" , m_ID.c_str() );

			// �α׿��̾�� �Ѵ�.
		//	pResult->next();
		//	string logon = pResult->getString(1);
			//cout << "logon = " << logon << endl;
			//Assert( logon == "LOGON" );

			// LogOn�� 'LOGON'�� ��츸 'LOGOFF'�� �����Ѵ�. by sigi. 2002.5.15
			// �α׿����� �����Ѵ�.
			pStmt->executeQuery( "UPDATE Player SET LogOn = 'LOGOFF' WHERE PlayerID='%s' AND LogOn='LOGON'" , m_ID.c_str() );

#if defined(__PAY_SYSTEM_LOGIN__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
			bool bClear 		= false;	// ���� ���� ���� ����
			bool bDecreaseTime 	= false;	// ��� �ð� ���� - loginserver������ ��������.
			logoutPayPlay( m_ID, bClear, bDecreaseTime );
#endif

			//cout << m_ID << " : LOGOFF" << endl;

			SAFE_DELETE(pStmt);
		}
		catch ( SQLQueryException & sqe )
		{
			filelog("DBError.log", "%s", sqe.toString().c_str());
			SAFE_DELETE(pStmt);
			throw Error(sqe.toString());
		}	
	}

	addLogoutPlayerData( this );

	__END_CATCH
}
//--------------------------------------------------------------------------------
// disconnect player no log
// DB �� �α׸� ���� �ʰ� �Ѵ�.
//--------------------------------------------------------------------------------
void LoginPlayer::disconnect_nolog ( bool bDisconnected )
	throw ( Error )
{
	__BEGIN_TRY

	if ( bDisconnected == UNDISCONNECTED ) 
	{
		// Ŭ���̾�Ʈ���� GCDisconnect ��Ŷ�� �����Ѵ�.
		//GCDisconnect lcDisconnect;
		//sendPacket( lcDisconnect );

		// ��� ���ۿ� �����ִ� ����Ÿ�� �����Ѵ�.
		m_pOutputStream->flush();
	}

	// ���� ������ �ݴ´�.
	m_pSocket->close();

	// '�̹� ���� ��'�� ���, ĳ���� ���� ���� ������ ��ٸ��� ��Ȳ.
	if (m_PlayerStatus==LPS_WAITING_FOR_GL_KICK_VERIFY)
	{
		m_ID = "NONE";
	}

	// �÷��̾��� ���¸� �α׾ƿ����� �����.
	Assert( m_PlayerStatus != LPS_END_SESSION );
	m_PlayerStatus = LPS_END_SESSION;

	// ���̵� �����Ǿ��ٴ� ����, �α����� �̷�����ٴ� ���̴�.
	// '�̹� ���� ��'�� ��쿡.. 
	// ĳ�� ���� ������ ��ٸ��� ���� ID�� ������ �� �����Ƿ� �ƴϴ�
	if ( m_ID != "NONE" )
	{
		Statement* pStmt   = NULL;
		//Result*    pResult = NULL;

		try
		{
			pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		// query�ؼ� �� ���淡.. ���ȶ�.. by sigi. 2002.5.7
		//	pResult = pStmt->executeQuery( "SELECT LogOn FROM Player WHERE PlayerID='%s'" , m_ID.c_str() );

			// �α׿��̾�� �Ѵ�.
		//	pResult->next();
		//	string logon = pResult->getString(1);
			//cout << "logon = " << logon << endl;
			//Assert( logon == "LOGON" );

			// LogOn�� 'LOGON'�� ��츸 'LOGOFF'�� �����Ѵ�. by sigi. 2002.5.15
			// �α׿����� �����Ѵ�.
			pStmt->executeQuery( "UPDATE Player SET LogOn = 'LOGOFF' WHERE PlayerID='%s' AND LogOn='LOGON'" , m_ID.c_str() );

#if defined(__PAY_SYSTEM_LOGIN__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
			bool bClear 		= false;	// ���� ���� ���� ����
			bool bDecreaseTime 	= false;	// ��� �ð� ���� - loginserver������ ��������.
			logoutPayPlay( m_ID, bClear, bDecreaseTime );
#endif

			//cout << m_ID << " : LOGOFF" << endl;

			SAFE_DELETE(pStmt);
		}
		catch ( SQLQueryException & sqe )
		{
			filelog("DBError.log", "%s", sqe.toString().c_str());
			SAFE_DELETE(pStmt);
			throw Error(sqe.toString());
		}	
	}


	__END_CATCH
}

//--------------------------------------------------------------------------------
//
// ������ �α����÷��̾�Ŵ��� �ܿ��� �α����÷��̾ ���ÿ� �����ϴ�
// ������� �������� ���� ��ȹ�̾�����, ���Ӽ����Ŵ����� ���ÿ� �������
// ���ư��鼭 �α��� �÷��̾ ������ ���ɼ��� ���ܹ��ȴ�. - -; �׷���,
// �Ʒ��� ���� mutex �� ��ȣ�Ǵ� ������ �����ߴ�.
//
//--------------------------------------------------------------------------------
void LoginPlayer::sendPacket ( Packet * pPacket )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	//LOG4("SEND PACKET to %s : %s(%d) %d /%d\n", m_ID.c_str() , pPacket->getPacketName().c_str() , pPacket->getPacketID(), pPacket->getPacketSize(), m_pOutputStream->length() );
	Player::sendPacket(pPacket);

        filelog("senpacket.log", "SEND PACKET to %s : %s(%d) %d /%d\n", m_ID.c_str() , pPacket->getPacketName().c_str() , pPacket->getPacketID(), pPacket->getPacketSize(), m_pOutputStream->length());
	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// �ֱ� N ��°�� ��Ŷ�� �����Ѵ�.
// 
// N == 0 �� ���, ���� �ֱ��� ��Ŷ�� �����ϰ� �ȴ�.
//
// �ִ� nPacketHistory - 1 ���� ������ �� �ִ�. 
//
//////////////////////////////////////////////////////////////////////
Packet * LoginPlayer::getOldPacket ( uint prev )
	throw ( OutOfBoundException , NoSuchElementException )
{
	__BEGIN_TRY

	if ( prev >= nPacketHistory ) throw OutOfBoundException();
	if ( prev >= m_PacketHistory.size() ) throw NoSuchElementException();
	return m_PacketHistory[ m_PacketHistory.size() - prev - 1 ];

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// Ư�� ��Ŷ���̵� ���� ���� �ֱ��� ��Ŷ�� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////
Packet * LoginPlayer::getOldPacket ( PacketID_t packetID )
	throw ( NoSuchElementException )
{
	__BEGIN_TRY

	Packet * pPacket = NULL;

	deque<Packet*>::reverse_iterator ritr = m_PacketHistory.rbegin();
	for (; ritr != m_PacketHistory.rend(); ritr++ ) 
	{
		if ( (*ritr)->getPacketID() == packetID ) 
		{
			pPacket = (*ritr);
			break;
		}	
	}

	if ( pPacket == NULL ) throw NoSuchElementException();

	return pPacket;

	__END_CATCH
}



//////////////////////////////////////////////////////////////////////////////
//
// send LGKickCharacter
//
// GameServer�� '�̹� ������'�� ĳ���͸� �����ش޶�� �޼����� ������.
//
//////////////////////////////////////////////////////////////////////////////
void
LoginPlayer::sendLGKickCharacter()
	throw ()
{
	cout << "send LGKickCharacter" << endl;

	// Game������ ĳ���͸� �����ش޶�� message�� ������.
	LGKickCharacter lgKickCharacter;

	Statement* pStmt = NULL;
	Statement* pStmt1 = NULL;

	string characterName = getLastCharacterName();
	int serverID, serverGroupID, worldID, lastSlot;

	string gameServerIP;
	uint gameServerPort;

	//----------------------------------------------------------------------
	// DB���� �� player�� �ֱٿ� ������ 
	// WorldID, ServerID, LastSlot�� ����.
	//----------------------------------------------------------------------
	if (!isSetWorldGroupID())
	{
		BEGIN_DB
		{
			pStmt1 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			Result * pResult = pStmt1->executeQuery("SELECT CurrentWorldID, CurrentServerGroupID, LastSlot FROM Player where PlayerID='%s'" , getID().c_str() );

			if( pResult->next() ) {
				serverID 		= 1;	// ����� ������ 1�̴�.
				worldID 		= pResult->getInt(1);
				serverGroupID 	= pResult->getInt(2);
				lastSlot	 	= pResult->getInt(3);

				setWorldID( worldID );
				setGroupID( serverGroupID );
				setLastSlot( lastSlot );

				setWorldGroupID( true );	// ���� �����ƴٴ� �ǹ�.
														// ������ �ٽ� Query �� �ҷ���
			}

			SAFE_DELETE(pStmt1);

		}
		END_DB(pStmt1)
	}
	else
	{
		// ������ ����� ���� �״�� ����.
		serverID 		= 1;	// ����� ������ 1�̴�.
		worldID 		= getWorldID();
		serverGroupID 	= getGroupID();
	}


	//----------------------------------------------------------------------
	// Slot�� �����Ǵ� ĳ���� �̸��� �˾Ƴ���.
	//----------------------------------------------------------------------
	if (characterName.size() == 0)
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection(m_WorldID)->createStatement();
			Result * pResult = pStmt->executeQuery("SELECT Name from Slayer where PlayerID='%s' AND Slot='SLOT%d'", getID().c_str(), lastSlot );

			if( pResult->next() ) {
				characterName = pResult->getString(1);

				setLastCharacterName( characterName );
			}
			else
			{
				cout << "No CharacterName" << endl;
				// LoginError(�̹� ���� ��)
				LCLoginError lcLoginError;
				lcLoginError.setErrorID(ALREADY_CONNECTED);	
				sendPacket(&lcLoginError);
				setPlayerStatus(LPS_BEGIN_SESSION);

				setID("NONE");	// disconnect���� LOGOFF�� �������� �ʰ� �ϱ� ���ؼ�

				SAFE_DELETE(pStmt);
				return;
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}

	//----------------------------------------------------------------------
	// GameServer�� ������ �˾Ƴ���.
	//
	// �ش� World �� ��� Server �� ������
	//----------------------------------------------------------------------
	for ( int i = 0; i < g_pGameServerInfoManager->getMaxServerGroupID(); i++)
	{
		serverGroupID = i;

		try {
			cout << "World=" << worldID << ", "
				 << "Group=" << serverGroupID << ", "
				 << "Server=" << serverID << endl;

			GameServerInfo* pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo(
													serverID,
													serverGroupID,
													worldID);

			if (pGameServerInfo!=NULL)
			{
				gameServerIP = pGameServerInfo->getIP();
				gameServerPort = pGameServerInfo->getUDPPort();

				cout << "IP=" << gameServerIP.c_str() << ", Port=" << gameServerPort << endl;
			}
		} catch (NoSuchElementException&) {
			cout << "No GameServerInfo" << endl;
			// LoginError(�̹� ���� ��)
	//		LCLoginError lcLoginError;
	//		lcLoginError.setErrorID(ALREADY_CONNECTED);	
	//		sendPacket(&lcLoginError);
	//		setPlayerStatus(LPS_BEGIN_SESSION);

			setID("NONE");	// disconnect���� LOGOFF�� �������� �ʰ� �ϱ� ���ؼ�

			return;
		}

		lgKickCharacter.setID( getSocket()->getSOCKET() );	// SocketFD. �˻��� ���ؼ�
		lgKickCharacter.setPCName( characterName );

		cout << "( " << gameServerIP.c_str() << ", " << gameServerPort << " )" << endl;
		g_pGameServerManager->sendPacket( gameServerIP, gameServerPort, &lgKickCharacter );
	}

	setExpireTimeForKickCharacter();
	setPlayerStatus(LPS_WAITING_FOR_GL_KICK_VERIFY);
}


//////////////////////////////////////////////////////////////////////
//
// send LCLoginOK
//
// Player table�� LogOn�� 'LOGON'���� �ٲٰ�
// client���� LCLoginOK�� ������.
// PlayerStatus�� LPS_WAITING_FOR_CL_GET_PC_LIST�� ����.
//
//////////////////////////////////////////////////////////////////////
void
LoginPlayer::sendLCLoginOK()
	throw ()
{
	try {
	//cout << "Send LCLoginOK" << endl;

	Statement* pStmt = NULL;

	string connectIP = getSocket()->getHost();

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pStmt->executeQuery("UPDATE Player SET LogOn = 'LOGON' WHERE PlayerID = '%s'", getID().c_str());
		if ( pStmt->getAffectedRowCount() == 0 )
		{
			filelog("MultiLogin.log", "��Ƽ �α��� ���� �õ��� ����� : [%s:%s]", getID().c_str(), connectIP.c_str());
			LCLoginError lcLoginError;
			//lcLoginError.setMessage("already connected");
			lcLoginError.setErrorID(ALREADY_CONNECTED);
			sendPacket(&lcLoginError);

			SAFE_DELETE(pStmt);
			setPlayerStatus(LPS_BEGIN_SESSION);

			return;
		}

		pStmt->executeQuery("UPDATE Player SET LoginIP = '%s' WHERE PlayerID = '%s'",connectIP.c_str(), getID().c_str());

		SAFE_DELETE(pStmt)
	}
	END_DB(pStmt)


	// Player table�� Login�� LOGON���� �ٲ۴�.
	LCLoginOK lcLoginOK;

	// �� ���� LoginPlayer�� �����صд�.
	lcLoginOK.setAdult( isAdult() );
	lcLoginOK.setUnderFifthteen( isUnderFifthteen() );
	lcLoginOK.setLastDays(0xffff);

	sendPacket(&lcLoginOK);

	setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

	// ����� �����ߴٰ� ���
	addLoginPlayerData( m_ID, connectIP, m_SSN, m_Zipcode );

	} catch (Throwable& t) {
		filelog("loginOKError.txt", "%s", t.toString().c_str());
		throw;
	}
}


bool    LoginPlayer::sendBillingLogin() 
	throw (Error)
{
	__BEGIN_TRY

	if (!m_ID.empty() && m_ID!="NONE")
	{
		Timeval currentTime;
		getCurrentTime(currentTime);

		if (currentTime > m_BillingNextLoginRequestTime)
		{
			g_pBillingPlayerManager->sendPayLogin( this );

			// PayLogin ��û�� ȸ�� ���
			m_BillingLoginRequestCount ++;

			// 10�� �� �ٽ� üũ�Ѵ�.
			m_BillingNextLoginRequestTime.tv_sec = currentTime.tv_sec + 10;
		}

		return true;
	}

	return false;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string LoginPlayer::toString () const
       throw ( Error )
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "LoginPlayer("
		<< "ID:" << m_ID 
		<< ",SocketID:" << m_pSocket->getSOCKET() 
		<< ",Host:" << m_pSocket->getHost() 
		<< ")" ;
	return msg.toString();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//
// add LogoutPlayerdata
//
// ������ ��踦 ���ؼ� 
// UserInfo DB�� LogoutPlayerData�� Logout�� ����ڸ� �߰��Ѵ�.
//
//////////////////////////////////////////////////////////////////////////////
void addLogoutPlayerData(Player* pPlayer)
{

	if(pPlayer->getID() != "NONE")
	{
	
		Statement* pStmt = NULL;

		pStmt = g_pDatabaseManager->getUserInfoConnection()->createStatement();

		// ���� ��� ���� ������ �Է��Ѵ�.
		BEGIN_DB
		{
			string ID = pPlayer->getID();
			string ip = pPlayer->getSocket()->getHost();

			// ���� ���� �ð��� ����.
			int year, month, day, hour, minute, second;
			getCurrentTimeEx(year, month, day, hour, minute, second);
			string currentDT = VSDateTime::currentDateTime().toDateTime();

			StringStream sql;
			sql << "INSERT INTO LogoutPlayerData (PlayerID,IP,Date,Time) VALUES ('"
				<< ID << "','" << ip << "','"
				<< currentDT.substr( 0, 10 ).c_str() << "','"
				<< currentDT.substr( 11 ).c_str() << "')";

			pStmt->executeQuery(sql.toString());

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
}

void LoginPlayer::makePCList( LCPCList& lcPCList )
	throw( DisconnectException )
{
	Statement* pStmt = NULL;
	Statement* pStmt2 = NULL;
	Result* pResult1 = NULL;
	Result* pResult2 = NULL;

	WorldID_t WorldID = getWorldID();

	try {
		pStmt = g_pDatabaseManager->getConnection( WorldID )->createStatement();	
		pStmt2 = g_pDatabaseManager->getConnection( WorldID )->createStatement();	

		//----------------------------------------------------------------------
		// �켱 �����̾� ���̺��� �˻��ؼ�, Active �� �����̾� ������ �ε��Ѵ�.
		// Ŭ���̾�Ʈ�� �����ؾ� �� ������ ������ ����.
		//
		//    - �̸�
		//    - ������ȣ
		//    - ����
		//    - ��Ÿ�� : �����̾� ����
		//    - �Ӹ���
		//    - �Ǻλ�
		//    - �԰��ִ¿����� : �����̾� ����, �Ӹ�/����/������ ��������
		//    - �ɷ�ġ : STR,DEX,CON
		//    - HP/MP �� ���� �� �ִ� : �����̾�� MP ����
		//    - ����� �� ����ġ : �����̾�����
		//    - ���� �ִ� ��
		//    - ���� ���̵�
		//
		//----------------------------------------------------------------------
		pResult1 = pStmt->executeQuery(
			"SELECT Race, Name, Slot, Sex, HairColor, SkinColor, AdvancementClass, STR, STRExp, DEX, DEXExp, INTE, INTExp, HP, CurrentHP, MP, CurrentMP, Fame, BladeLevel, SwordLevel, GunLevel, HealLevel, EnchantLevel, ETCLevel, Alignment, Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor, `Rank` FROM Slayer WHERE PlayerID = '%s' AND Active = 'ACTIVE'",
			getID().c_str()
		);

		// ���� flag. by sigi. 2002.6.18
		DWORD   shape;
		Color_t colors[PCSlayerInfo::SLAYER_COLOR_MAX];
		Color_t colorsVamp[PCVampireInfo::VAMPIRE_COLOR_MAX];

		while (pResult1->next()) 
		{	
			uint i = 0;
			string race = pResult1->getString(++i);
			string name = pResult1->getString(++i);

			if (race == "SLAYER") 
			{
				// �����̾� PCInfo ��ü�� �����Ѵ�.
				PCSlayerInfo* pPCSlayerInfo = new PCSlayerInfo();

				// �� �ʵ尪�� �����Ѵ�.
				pPCSlayerInfo->setName(name);
				pPCSlayerInfo->setSlot(pResult1->getString(++i));
				pPCSlayerInfo->setSex(pResult1->getString(++i));
				pPCSlayerInfo->setHairStyle(HAIR_STYLE1);
				pPCSlayerInfo->setHairColor(pResult1->getInt(++i));
				pPCSlayerInfo->setSkinColor(pResult1->getInt(++i));
				pPCSlayerInfo->setAdvancementLevel(pResult1->getInt(++i));
				pPCSlayerInfo->setSTR(pResult1->getInt(++i));
				pPCSlayerInfo->setSTRExp(pResult1->getInt(++i));
				pPCSlayerInfo->setDEX(pResult1->getInt(++i));
				pPCSlayerInfo->setDEXExp(pResult1->getInt(++i));
				pPCSlayerInfo->setINT(pResult1->getInt(++i));
				pPCSlayerInfo->setINTExp(pResult1->getInt(++i));
				pPCSlayerInfo->setHP(pResult1->getInt(++i) , pResult1->getInt(++i));
				pPCSlayerInfo->setMP(pResult1->getInt(++i) , pResult1->getInt(++i));
				pPCSlayerInfo->setFame(pResult1->getInt(++i));

				for ( int j = 0; j < SKILL_DOMAIN_VAMPIRE; j++ ) {
					pPCSlayerInfo->setSkillDomainLevel( (SkillDomain)j, (SkillLevel_t)pResult1->getInt(++i) );
				}

				pPCSlayerInfo->setAlignment(pResult1->getInt(++i));

				// ���� ������ flag�� ��ü�Ѵ�. by sigi. 2002.6.18
				shape = pResult1->getDWORD(++i);

				colors[PCSlayerInfo::SLAYER_COLOR_HAIR] = pPCSlayerInfo->getHairColor();
				colors[PCSlayerInfo::SLAYER_COLOR_SKIN] = pPCSlayerInfo->getSkinColor();
				colors[PCSlayerInfo::SLAYER_COLOR_HELMET] = pResult1->getInt(++i);
				colors[PCSlayerInfo::SLAYER_COLOR_JACKET] = pResult1->getInt(++i);
				colors[PCSlayerInfo::SLAYER_COLOR_PANTS] = pResult1->getInt(++i);
				colors[PCSlayerInfo::SLAYER_COLOR_WEAPON] = pResult1->getInt(++i);
				colors[PCSlayerInfo::SLAYER_COLOR_SHIELD] = pResult1->getInt(++i);

				pPCSlayerInfo->setShapeInfo( shape, colors );

				pPCSlayerInfo->setRank(pResult1->getInt(++i));

				lcPCList.setPCInfo(pPCSlayerInfo->getSlot() , pPCSlayerInfo);
			} 
			else if ( race == "VAMPIRE" ) 
			{
				//----------------------------------------------------------------------
				// ���� �����̾� ���̺��� �˻��ؼ� LCPCList ��Ŷ�� �������..
				// Ŭ���̾�Ʈ�� �����ؾ� �� ������ ������ ����.
				//
				//    - Name
				//    - Slot
				//    - Sex
				//    - BatColor
				//    - SkinColor
				//    - ����/������ ��������
				//    - �ɷ�ġ : STR,DEX,CON
				//    - CurrentHP/MaxHP
				//    - Gold
				//    - ZoneID
				//
				//----------------------------------------------------------------------

				pResult2 = pStmt2->executeQuery(
					"SELECT Name, Slot, Sex, BatColor, AdvBatColor, SkinColor, AdvancementClass, STR, DEX, INTE, HP, CurrentHP, `Rank`, GoalExp, Level, Bonus, Fame, Alignment, Shape, CoatColor FROM Vampire WHERE PlayerID = '%s' AND Active = 'ACTIVE' AND Name='%s'",
					getID().c_str() , name.c_str()
				);

				if (pResult2->getRowCount() == 0)
				{
					throw DisconnectException("No Vampire");
				}

				pResult2->next();

				// �����̾� PCInfo ��ü�� �����Ѵ�.
				PCVampireInfo* pPCVampireInfo = new PCVampireInfo();

				// �� �ʵ尪�� �����Ѵ�.
				uint i = 0;

				pPCVampireInfo->setName(pResult2->getString(++i));
				pPCVampireInfo->setSlot(pResult2->getString(++i));

				string sex = pResult2->getString(++i);
				pPCVampireInfo->setSex(sex);
				pPCVampireInfo->setBatColor(pResult2->getInt(++i));
				pPCVampireInfo->setAdvanceBatColor(pResult2->getInt(++i)); // 2007.07.24 ���� ���� ����
				pPCVampireInfo->setSkinColor(pResult2->getInt(++i));
				pPCVampireInfo->setAdvancementLevel(pResult2->getInt(++i));
				pPCVampireInfo->setSTR(pResult2->getInt(++i));
				pPCVampireInfo->setDEX(pResult2->getInt(++i));
				pPCVampireInfo->setINT(pResult2->getInt(++i));
				pPCVampireInfo->setHP(pResult2->getInt(++i) , pResult2->getInt(++i));
				pPCVampireInfo->setRank(pResult2->getInt(++i));
				pPCVampireInfo->setExp(pResult2->getInt(++i));
				pPCVampireInfo->setLevel(pResult2->getInt(++i));
				pPCVampireInfo->setBonus(pResult2->getInt(++i));
				pPCVampireInfo->setFame(pResult2->getInt(++i));
				pPCVampireInfo->setAlignment(pResult2->getInt(++i));

				// ���� ���� �о���� ����ȭ. by sigi. 2002.6.19
				shape = pResult2->getDWORD(++i);

				colorsVamp[0] = pResult2->getInt(++i);  // CoatColor
				pPCVampireInfo->setShapeInfo( shape, colorsVamp );

				pPCVampireInfo->setArmType( VAMPIRE_ARM_NONE );
				pPCVampireInfo->setCoatType( getVampireCoatType(shape) );

				lcPCList.setPCInfo(pPCVampireInfo->getSlot() , pPCVampireInfo);
			}
			else
			{
				//----------------------------------------------------------------------
				// ���� �ƿ콺���� ���̺��� �˻��ؼ� LCPCList ��Ŷ�� �������..
				// Ŭ���̾�Ʈ�� �����ؾ� �� ������ ������ ����.
				//
				//    - Name
				//    - Slot
				//    - Sex
				//    - HairColor
				//    - SkinColor
				//    - ����/������ ��������
				//    - �ɷ�ġ : STR,DEX,CON
				//    - CurrentHP/MaxHP
				//    - Gold
				//    - ZoneID
				//
				//----------------------------------------------------------------------

				pResult2 = pStmt2->executeQuery(
					"SELECT Name, Slot, Sex, AdvancementClass, STR, DEX, INTE, HP, CurrentHP, `Rank`, Exp, Level, Bonus, SkillBonus, Fame, Alignment, CoatType, ArmType, CoatColor, HairColor, ArmColor, BootsColor FROM Ousters WHERE PlayerID = '%s' AND Active = 'ACTIVE' AND Name='%s'",
					getID().c_str() , name.c_str()
				);

				if (pResult2->getRowCount() == 0)
				{
					throw DisconnectException("No Ousters");
				}

				pResult2->next();

				// �ƿ콺���� PCInfo ��ü�� �����Ѵ�.
				PCOustersInfo* pPCOustersInfo = new PCOustersInfo();

				// �� �ʵ尪�� �����Ѵ�.
				uint i = 0;

				pPCOustersInfo->setName(pResult2->getString(++i));
				pPCOustersInfo->setSlot(pResult2->getString(++i));

				string sex = pResult2->getString(++i);
				pPCOustersInfo->setSex(sex);
				pPCOustersInfo->setAdvancementLevel(pResult2->getInt(++i));
				pPCOustersInfo->setSTR(pResult2->getInt(++i));
				pPCOustersInfo->setDEX(pResult2->getInt(++i));
				pPCOustersInfo->setINT(pResult2->getInt(++i));
				pPCOustersInfo->setHP(pResult2->getInt(++i) , pResult2->getInt(++i));
				pPCOustersInfo->setRank(pResult2->getInt(++i));
				pPCOustersInfo->setExp(pResult2->getInt(++i));
				pPCOustersInfo->setLevel(pResult2->getInt(++i));
				pPCOustersInfo->setBonus(pResult2->getInt(++i));
				pPCOustersInfo->setSkillBonus(pResult2->getInt(++i));
				pPCOustersInfo->setFame(pResult2->getInt(++i));
				pPCOustersInfo->setAlignment(pResult2->getInt(++i));
				pPCOustersInfo->setCoatType((OustersCoatType)pResult2->getInt(++i));
				pPCOustersInfo->setArmType((OustersArmType)pResult2->getInt(++i));
				pPCOustersInfo->setCoatColor(pResult2->getInt(++i));
				pPCOustersInfo->setHairColor(pResult2->getInt(++i));
				pPCOustersInfo->setArmColor(pResult2->getInt(++i));
				pPCOustersInfo->setBootsColor(pResult2->getInt(++i));

				lcPCList.setPCInfo(pPCOustersInfo->getSlot() , pPCOustersInfo);
			}
		}

		// ���� ��� �� ������ ��ü�� �����Ѵ�.
		SAFE_DELETE(pStmt);
		SAFE_DELETE(pStmt2);
	} 
	catch (SQLQueryException & sce) 
	{
		// ���� ��� �� ������ ��ü�� �����Ѵ�.
		SAFE_DELETE(pStmt);
		SAFE_DELETE(pStmt2);

		throw DisconnectException(sce.toString());
	}
}
