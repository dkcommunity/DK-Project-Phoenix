//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCDeletePCError.h 
// Written By  : Reiot
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_DELETE_PC_ERROR_H__
#define __LC_DELETE_PC_ERROR_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCError;
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCError : public Packet {

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_LC_DELETE_PC_ERROR; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const  { return szBYTE; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const  { return "LCDeletePCError"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
	// get/set error message
//	std::string getMessage () const  { return m_Message; }
//	void setMessage ( std::string message )  { m_Message = message; }
	// get /set ErrorID
	BYTE getErrorID() const  { return m_ErrorID; }
	void setErrorID( BYTE ErrorID )  { m_ErrorID = ErrorID; }

private : 

	// 에러 메시지
	BYTE m_ErrorID;

};


//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCErrorFactory;
//
// Factory for LCDeletePCError
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCErrorFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new LCDeletePCError(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "LCDeletePCError"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_LC_DELETE_PC_ERROR; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const  { return szBYTE; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCErrorHandler {

public :

	// execute packet's handler
	static void execute ( LCDeletePCError * pPacket , Player * pPlayer ) ;

};

#endif
