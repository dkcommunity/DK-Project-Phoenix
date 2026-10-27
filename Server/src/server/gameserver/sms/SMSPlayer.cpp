//////////////////////////////////////////////////////////////////////
// 
// Filename    : SMSPlayer.cpp 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SMSPlayer.h"
#include "Socket.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Assert.h"
#include "Packet.h"
#include "Properties.h"
#include "PacketFactoryManager.h"
#include "CommonSMSPacket.h"

#ifdef __GAME_SERVER__
	#include "PCFinder.h"
	#include "GamePlayer.h"
#endif

const int defaultSMSPlayerInputStreamSize = 10240;
const int defaultSMSPlayerOutputStreamSize = 10240;


//////////////////////////////////////////////////////////////////////
//
// constructor
//
//////////////////////////////////////////////////////////////////////
SMSPlayer::SMSPlayer ()
	 throw ( Error )
//: m_pSocket(NULL), m_pInputStream(NULL), m_pOutputStream(NULL)
{
}


SMSPlayer::SMSPlayer (Socket * pSocket)
	 throw ( Error )
//: Player( pSocket )//m_pSocket(pSocket), m_pInputStream(NULL), m_pOutputStream(NULL)
{
	__BEGIN_TRY
		
	Assert( pSocket != NULL );
	m_pSocket = pSocket;

	// create socket input stream
	m_pInputStream = new SocketInputStream( m_pSocket, defaultSMSPlayerInputStreamSize );

	Assert( m_pInputStream != NULL );
	
	// create socket output stream
	m_pOutputStream = new SocketOutputStream( m_pSocket, defaultSMSPlayerOutputStreamSize );

	Assert( m_pOutputStream != NULL );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//
//////////////////////////////////////////////////////////////////////
SMSPlayer::~SMSPlayer ()
	 throw ( Error )
{
	__BEGIN_TRY
		
	// delete socket input stream
	SAFE_DELETE(m_pInputStream);

	// delete socket output stream
	SAFE_DELETE(m_pOutputStream);

	// delete socket
	if ( m_pSocket != NULL ) 
	{
		m_pSocket->close();
		delete m_pSocket;
		m_pSocket = NULL;

		filelog(LOGFILE_SMS_PLAYER, "Close Socket" );
	}

	__END_CATCH
}


void SMSPlayer::processInput()
	throw ( IOException, Error )
{
	__BEGIN_TRY

	try
	{
		m_pInputStream->fill();
	}
	catch ( NonBlockingIOException& nbie )
	{
	}

	__END_CATCH
}


void SMSPlayer::processOutput()
	throw ( IOException, Error )
{
	__BEGIN_TRY

	try
	{
		m_pOutputStream->flush();
	}
	catch ( InvalidProtocolException& )
	{
		throw DisconnectException( "이상한 패킷임" );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void SMSPlayer::processCommand () 
     throw ( IOException , Error )
{
	__BEGIN_TRY

	try {

		// 헤더를 임시저장할 버퍼 생성
		//char header[szPacketHeader];
		//PacketID_t packetID;
		//PacketSize_t packetSize;
		CommonSMSPacket	cbPacket;

		// 입력버퍼에 들어있는 완전한 패킷들을 모조리 처리한다.
		while ( true ) {
		
			/*
			// 입력스트림에서 패킷헤더크기만큼 읽어본다.
			// 만약 지정한 크기만큼 스트림에서 읽을 수 없다면,
			// Insufficient 예외가 발생하고, 루프를 빠져나간다.
			if ( !m_pInputStream->peek( header , szPacketHeader ) )
				break;

			// 패킷아이디 및 패킷크기를 알아낸다.
			// 이때 패킷크기는 헤더를 포함한다.
			memcpy( &packetID   , &header[0] , szPacketID );	
			memcpy( &packetSize , &header[szPacketID] , szPacketSize );

			// 패킷 아이디가 이상하면 프로토콜 에러로 간주한다.
			if ( packetID >= Packet::PACKET_MAX )
				throw InvalidProtocolException("invalid packet id");
			
			// 패킷 크기가 너무 크면 프로토콜 에러로 간주한다.
			if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID) )
				throw InvalidProtocolException("too large packet size");
			*/
			
			// 입력버퍼내에 패킷크기만큼의 데이타가 들어있는지 확인한다.
			// 최적화시 break 를 사용하면 된다. (여기서는 일단 exception을 쓸 것이다.)
			//if ( m_pInputStream->length() < szPacketHeader + packetSize )
			if ( m_pInputStream->length() < cbPacket.getPacketSize())
			{
				//throw InsufficientDataException();
				return;
			}
			
			// 여기까지 왔다면 입력버퍼에는 완전한 패킷 하나 이상이 들어있다는 뜻이다.
			// 패킷팩토리매니저로부터 패킷아이디를 사용해서 패킷 스트럭처를 생성하면 된다.
			// 패킷아이디가 잘못될 경우는 패킷팩토리매니저에서 처리한다.
			//pPacket = g_pPacketFactoryManager->createPacket( packetID );

			// 이제 이 패킷스트럭처를 초기화한다.
			// 패킷하위클래스에 정의된 read()가 virtual 메커니즘에 의해서 호출되어
			// 자동적으로 초기화된다.
			//m_pInputStream->read( pPacket );
			// packetHeader부분이 필요없다.
			cbPacket.read( *m_pInputStream );
			
			// 이제 이 패킷스트럭처를 가지고 패킷핸들러를 수행하면 된다.
			// 패킷아이디가 잘못될 경우는 패킷핸들러매니저에서 처리한다.
			//pPacket->execute( this );
			cbPacket.execute( this );

			// 패킷을 삭제한다
			//delete pPacket;

		}

	} catch ( NoSuchElementException & nsee ) {

		// PacketFactoryManager::createPacket(PacketID_t)
		// PacketFactoryManager::getPacketMaxSize(PacketID_t)
		// 에서 던질 가능성이 있다.
		throw Error( nsee.toString() );

	} catch ( InsufficientDataException ) {

		// do nothing

	} 
	__END_CATCH
}
		    

//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void SMSPlayer::sendPacket ( Packet * pPacket )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	//m_pOutputStream->write( pPacket );	// packetHeader는 필요없다.
	pPacket->write( *m_pOutputStream );

	/*
	cout << endl;
	cout << "=== SMSPlayer::sendPacket() ===" << endl;
	cout << pPacket->toString() << endl;
	cout << "============================" << endl;
	*/

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// disconnect ( close socket )
//
//////////////////////////////////////////////////////////////////////
void SMSPlayer::disconnect ( bool bDisconnected )
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	try 
	{
		// 정당하게 로그아웃한 경우에는 출력 버퍼를 플러시할 수 있다.
		// 그러나, 불법적인 디스를 걸었다면 소켓이 닫겼으므로
		// 플러시할 경우 SIG_PIPE 을 받게 된다.
		if ( bDisconnected == UNDISCONNECTED ) 
		{
			m_pOutputStream->flush();
		}

		m_pSocket->close();
	} 
	catch ( InvalidProtocolException & t ) 
	{
		cerr << "SMSPlayer::disconnect Exception Check!!" << endl;
		cerr << t.toString() << endl;
		m_pSocket->close();
		//throw Error("씨바...");
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// set socket
//////////////////////////////////////////////////////////////////////
void SMSPlayer::setSocket ( Socket * pSocket )
	throw ()
{
	__BEGIN_TRY

	m_pSocket = pSocket;

	if ( m_pInputStream != NULL ) 
	{
		delete m_pInputStream;
		m_pInputStream = new SocketInputStream( m_pSocket );
	}

	if ( m_pOutputStream != NULL ) {
		delete m_pOutputStream;
		m_pOutputStream = new SocketOutputStream( m_pSocket );
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 게임 서버가 처음 뜰 때 보낸다.
//////////////////////////////////////////////////////////////////////
void SMSPlayer::sendSMSInit()
	throw( ProtocolException, Error )
{
	__BEGIN_TRY
		
	CommonSMSPacket cbPacket;

	SMSInfo* pSMSInfo = &cbPacket;
	memset((char*)pSMSInfo, 0, sizeof(SMSInfo));

	cbPacket.setPacket_Type( SMS_PACKET_INIT );
	cbPacket.setUser_CC();

	sendPacket( &cbPacket );

	filelog(LOGFILE_SMS_PLAYER, "-------------------- send SMSInit --------------------");
	filelog(LOGFILE_SMS_PLAYER, "%s", cbPacket.toString().c_str());

#ifdef __COUT_SMS_SYSTEM__
	cout << "[send] SMSPlayer::sendSMSInit" << endl;
#endif

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// SMS 메세지를 보낸다
//////////////////////////////////////////////////////////////////////
void SMSPlayer::sendSMSMsg( CommonSMSPacket* pPacket )
	throw( ProtocolException, Error )
{
	__BEGIN_TRY
		
	string PlayerID = pPacket->User_ID;

	CommonSMSPacket cbPacket;


	SMSInfo* pSMSInfo = &cbPacket;
	memset((char*)pSMSInfo, 0, sizeof(SMSInfo));

	cbPacket.setPacket_Type( SMS_PACKET_CHECK );
	cbPacket.setSession( pPacket->Session );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string SMSPlayer::toString () const
       throw ( Error )
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "SMSPlayer("
		<< "SocketID:" << m_pSocket->getSOCKET() 
		<< ",Host:" << m_pSocket->getHost() 
		<< ",ID:" << m_ID
		<< ")" ;

	return msg.toString();

	__END_CATCH
}
