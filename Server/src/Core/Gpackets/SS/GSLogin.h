//----------------------------------------------------------------------
// 
// Filename    : GSLogin.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_LOGIN_H__
#define __GS_LOGIN_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GSLogin : public Packet {

public:
	GSLogin();
	virtual ~GSLogin() throw();	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GS_LOGIN; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		return m_Sender.getSize() + m_ServerName.getSize();
	}

	// get packet name
	string getPacketName() const throw() { return "GSLogin"; }
	
	// get packet's debug string
	string toString() const throw();

public:

	string GetSender() const { return m_Sender.GetString(); }
	void SetSender(const string& strName) { m_Sender.SetString(strName); }

	string GetServerName() const { return m_ServerName.GetString(); }
	void SetServerName(const string& strServerName) { m_ServerName.SetString(strServerName); }
	
private :
	StringInfo m_Sender;
	StringInfo m_ServerName;

};


//////////////////////////////////////////////////////////////////////
//
// class GSLoginFactory;
//
// Factory for GSLogin
//
//////////////////////////////////////////////////////////////////////

class GSLoginFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new GSLogin(); }

	// get packet name
	string getPacketName() const throw() { return "GSLogin"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_LOGIN; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static LGIncomingConnectionPacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSLoginHandler {
	
public:

	// execute packet's handler
	static void execute(GSLogin* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
