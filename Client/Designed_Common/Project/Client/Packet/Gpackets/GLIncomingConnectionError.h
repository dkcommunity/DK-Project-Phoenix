//////////////////////////////////////////////////////////////////////
// 
// Filename    : GLIncomingConnectionError.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GL_INCOMING_CONNECTION_ERROR_H__
#define __GL_INCOMING_CONNECTION_ERROR_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionError;
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionError : public DatagramPacket {

public :
	
    // Datagram 객체에서부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( Datagram & iDatagram ) ;
		    
    // Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
    void write ( Datagram & oDatagram ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_GL_INCOMING_CONNECTION_ERROR; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const  { return szBYTE + m_Message.size(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GLIncomingConnectionError"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif

	// get/set error message
	const std::string& getMessage () const  { return m_Message; }
	void setMessage ( std::string message )  { m_Message = message; }

	// get/set player id
	const std::string& getPlayerID () const  { return m_PlayerID; }
	void setPlayerID ( std::string playerID )  { m_PlayerID = playerID; }

private :

	// error message
	std::string m_Message;

	// player id
	std::string m_PlayerID;

};


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionErrorFactory;
//
// Factory for GLIncomingConnectionError
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionErrorFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new GLIncomingConnectionError(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GLIncomingConnectionError"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_GL_INCOMING_CONNECTION_ERROR; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GLIncomingConnectionErrorPacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize () const  { return szBYTE + 80; }

};


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionErrorHandler {
	
public :

	// execute packet's handler
	static void execute ( GLIncomingConnectionError * pPacket ) ;

};

#endif
