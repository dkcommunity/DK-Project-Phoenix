//----------------------------------------------------------------------
// 
// Filename    : GSFindServerUser.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FIND_SERVER_USER_H__
#define __GS_FIND_SERVER_USER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class GSFindServerUser;
//
// 유저를 찾아달라고 요청 SharedSvr 에 요청한다.
//
// *CAUTION*
//
//----------------------------------------------------------------------

class GSFindServerUser : public Packet {

public:
	
    // Stream 객체에서부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // Stream 객체로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GS_FIND_SERVER_USER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		return szServerGroupID +		// ServerGroupID
			   szBYTE +					// Finder name length 
			   m_FinderName.size() +	// Finder name
			   szBYTE +					// Keyword length
			   m_Keyword.size() +		// Keyword 
	}

	// get packet name
	string getPacketName() const throw() { return "GSFindServerUser"; }
	
	// get packet's debug string
	string toString() const throw();

public:

	// get/set Finder Name
	const string& getFinderName() const throw() { return m_FinderName; }
	void setFinderName( const string& name ) throw() { m_FinderName = name; }

	// get/set Keyword 
	const string& getKeyword() const throw() { return m_Keyword; }
	void setKeyword( const string& keyword ) throw() { m_Keyword = keyword; }
	
	// get/set server group ID
	ServerGroupID_t getServerGroupID() const throw() { return m_ServerGroupID; }
	void setServerGroupID( ServerGroupID_t serverGroupID ) throw() { m_ServerGroupID = serverGroupID; }


private :

	// Finder's ServerID
	ServerGroupID_t m_ServerGroupID;
	
	// Finder name
	string	m_FinderName;

	// Keyword
	string	m_Keyword;


};


//////////////////////////////////////////////////////////////////////
//
// class GSFindServerUserFactory;
//
// Factory for GSFindServerUser
//
//////////////////////////////////////////////////////////////////////

class GSFindServerUserFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new GSFindServerUser(); }

	// get packet name
	string getPacketName() const throw() { return "GSFindServerUser"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FIND_SERVER_USER; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static LGIncomingConnectionPacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
        return szServerGroupID +        // ServerGroupID
               szBYTE +                 // Finder name length 
               30 +						// Finder name max length
               szBYTE +                 // Keyword length
               30;						// Keyword max length
		
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GSFindServerUserHandler 
//
//////////////////////////////////////////////////////////////////////

class GSFindServerUserHandler {
	
public:

	// execute packet's handler
	static void execute(GSFindServerUser* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
