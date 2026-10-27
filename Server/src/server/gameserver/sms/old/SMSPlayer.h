//////////////////////////////////////////////////////////////////////
// 
// Filename    : SMSPlayer.h 
// Written by  : reiot@ewestsoft.com
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SMS_PLAYER_H__
#define __SMS_PLAYER_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Player.h"
#include "SMSInfo.h"

class CommonSMSPacket;

//////////////////////////////////////////////////////////////////////
//
// class SMSPlayer
//
//////////////////////////////////////////////////////////////////////

class SMSPlayer : public Player 
{

public :

	// constructor
	SMSPlayer () throw (Error);
	SMSPlayer (Socket * pSocket) throw ( Error );
	
	// destructor
	virtual ~SMSPlayer () throw (Error);

	virtual void processInput () throw (IOException, Error);
	
	virtual void processOutput () throw (IOException, Error);
	
	// parse packet and execute handler for the packet
	virtual void processCommand () throw (IOException, Error);
	
	// send packet to player's output buffer
	virtual void sendPacket (Packet* pPacket) throw (ProtocolException, Error);

	// disconnect
	// 플레이어의 연결을 종료할 때, 상대편이 적절하게 로그아웃하지 않을 경우 소켓의 연결이 
	// 이미 끊겨 있으므로 disconnect(DISCONNECTED) 를 사용해서 연결을 종료해야 한다. 반면, 
	// 정당하게 로그아웃을 한 경우에는 disconnect(UNDISCONNECTED) 를 사용해야 한다.
	virtual void disconnect (bool bDisconnected = DISCONNECTED) throw (InvalidProtocolException, Error);

	void setSocket ( Socket * pSocket ) throw ();
	
	// get debug string
	virtual string toString () const throw (Error);

public :
	void sendSMSInit() throw( ProtocolException, Error );
	void sendSMSMsg( CommonSMSPacket* pPacket ) throw( ProtocolException, Error );

};

#endif

