//----------------------------------------------------------------------
// 
// Filename    : GSLogout.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_LOGOUT_H__
#define __GS_LOGOUT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GSLogout : public Packet {

public:
	GSLogout();
	virtual ~GSLogout() throw() {}
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GS_LOGOUT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		return m_Sender.getSize();
	}

	// get packet name
	string getPacketName() const throw() { return "GSLogout"; }
	
	// get packet's debug string
	string toString() const throw();

public:
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
	string GetSender() const { return m_Sender.GetString(); }

private :
	StringInfo m_Sender;

};


//////////////////////////////////////////////////////////////////////
//
// class GSLogoutFactory;
//
// Factory for GSLogout
//
//////////////////////////////////////////////////////////////////////

class GSLogoutFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new GSLogout(); }

	// get packet name
	string getPacketName() const throw() { return "GSLogout"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_LOGOUT; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSLogoutHandler {
	
public:

	// execute packet's handler
	static void execute(GSLogout* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
