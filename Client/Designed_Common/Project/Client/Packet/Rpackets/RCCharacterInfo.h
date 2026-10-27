//----------------------------------------------------------------------
// 
// Filename    : RCCharacterInfo.h 
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __RC_CHARACTER_INFO_H__
#define __RC_CHARACTER_INFO_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"
#include "types\GuildTypes.h"


//----------------------------------------------------------------------
//
// class RCCharacterInfo;
//
// 내가 다른 클라이언트에게 말하는 패킷
//
//----------------------------------------------------------------------

class RCCharacterInfo : public DatagramPacket {

public :
	RCCharacterInfo();
	
    // Datagram 객체에서부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( Datagram & iDatagram ) ;
		    
    // Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
    void write ( Datagram & oDatagram ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_RC_CHARACTER_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const  
	{ 
		return szBYTE + m_Name.size() + szGuildID;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "RCCharacterInfo"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif

public :

	// get /set GuildID
	GuildID_t getGuildID() const  { return m_GuildID; }
	void setGuildID( GuildID_t guildID )  { m_GuildID = guildID; }


	// get/set chatting Name
	const std::string& getName () const  { return m_Name; }
	void setName ( const std::string & name )  { m_Name = name; }

protected :
	std::string		m_Name;

	GuildID_t	m_GuildID;
};


//////////////////////////////////////////////////////////////////////
//
// class RCCharacterInfoFactory;
//
// Factory for RCCharacterInfo
//
//////////////////////////////////////////////////////////////////////

class RCCharacterInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new RCCharacterInfo(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "RCCharacterInfo"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_RC_CHARACTER_INFO; }

	PacketSize_t getPacketMaxSize () const  
	{ 
		return szBYTE + 20 + szGuildID;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class RCCharacterInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class RCCharacterInfoHandler {
	
public :

	// execute packet's handler
	static void execute ( RCCharacterInfo * pPacket ) ;

};

#endif
