//----------------------------------------------------------------------
// 
// Filename    : RCStatusHP.h 
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __RC_STATUS_HP_H__
#define __RC_STATUS_HP_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class RCStatusHP;
//
// 내가 다른 클라이언트에게 말하는 패킷
//
//----------------------------------------------------------------------

class RCStatusHP : public DatagramPacket {

public :
	RCStatusHP();
	
    // Datagram 객체에서부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( Datagram & iDatagram ) ;
		    
    // Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
    void write ( Datagram & oDatagram ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_RC_STATUS_HP; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const  
	{ 
		return szBYTE + m_Name.size() + szHP + szHP;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "RCStatusHP"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif

public :
	// get /set CurrentHP
	HP_t getCurrentHP() const  { return m_CurrentHP; }
	void setCurrentHP( HP_t CurrentHP )  { m_CurrentHP = CurrentHP; }

	// get /set MaxHP
	HP_t getMaxHP() const  { return m_MaxHP; }
	void setMaxHP( HP_t MaxHP )  { m_MaxHP = MaxHP; }


	// get/set chatting Name
	const std::string& getName () const  { return m_Name; }
	void setName ( const std::string & name )  { m_Name = name; }

protected :
	std::string		m_Name;

	// 나타날 좌표의 대강의 위치
	HP_t		m_MaxHP;
	HP_t		m_CurrentHP;	
};


//////////////////////////////////////////////////////////////////////
//
// class RCStatusHPFactory;
//
// Factory for RCStatusHP
//
//////////////////////////////////////////////////////////////////////

class RCStatusHPFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new RCStatusHP(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "RCStatusHP"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_RC_STATUS_HP; }

	PacketSize_t getPacketMaxSize () const  
	{ 
		return szBYTE + 20 + szHP + szHP;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class RCStatusHPHandler;
//
//////////////////////////////////////////////////////////////////////

class RCStatusHPHandler {
	
public :

	// execute packet's handler
	static void execute ( RCStatusHP * pPacket ) ;

};

#endif
