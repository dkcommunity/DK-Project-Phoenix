//--------------------------------------------------------------------------------
// 
// Filename    : CURequestLoginMode.h 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

#ifndef __CU_REQUEST_LOGIN_MODE_H__
#define __CU_REQUEST_LOGIN_MODE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CURequestLoginMode;
//
// 로그인 모드를 알아내기 위한 패킷이다
// 
//
//--------------------------------------------------------------------------------

class CURequestLoginMode : public Packet {
public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;

	// 소켓으로부터 직접 데이터를 읽어서 패킷을 초기화한다.
	void read ( Socket * pSocket ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_CU_REQUEST_LOGIN_MODE; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const  { return 0; }
	//
	static PacketSize_t getPacketMaxSize ()  { return 0; }

	// get packet name
	std::string getPacketName () const  { return "CURequestLoginMode"; }
	
	// get packet's debug std::string
	std::string toString () const ;
};


//--------------------------------------------------------------------------------
//
// class CURequestLoginModeFactory;
//
// Factory for CURequestLoginMode
//
//--------------------------------------------------------------------------------

class CURequestLoginModeFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new CURequestLoginMode(); }

	// get packet name
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName () const  { return "CURequestLoginMode"; }
#endif	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CU_REQUEST_LOGIN_MODE; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const  { return 0; }

};


//--------------------------------------------------------------------------------
//
// class CURequestLoginModeHandler;
//
//--------------------------------------------------------------------------------

class CURequestLoginModeHandler {

public :

	// execute packet's handler
	static void execute ( CURequestLoginMode * pPacket , Player * pPlayer ) ;
};

#endif
