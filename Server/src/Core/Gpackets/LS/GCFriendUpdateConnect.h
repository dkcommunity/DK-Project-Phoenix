//----------------------------------------------------------------------
// 
// Filename    : GCFriendUpdateConnect.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_UPDATE_CONNECT_H__
#define __GC_FRIEND_UPDATE_CONNECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GCFriendUpdateConnect : public Packet {

public:
	GCFriendUpdateConnect();
	virtual ~GCFriendUpdateConnect() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_UPDATE_CONNECT; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendUpdateConnect"; }
	string toString() const throw();
	
	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strPCName) { m_PCName.SetString(strPCName); }
	
	string GetServerName() const { return m_ServerName.GetString(); }
	void SetServerName(const string& strServerName) { m_ServerName.SetString(strServerName); }
	
	bool GetConnect() const { return m_Connect == 0 ? false : true ; }
	void SetConnect(bool bConnect) { m_Connect = bConnect ? 1 : 0; } 

private :
	StringInfo m_PCName;
	StringInfo m_ServerName;
	BYTE m_Connect;
};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendUpdateConnectFactory;
//
// Factory for GCFriendUpdateConnect
//
//////////////////////////////////////////////////////////////////////

class GCFriendUpdateConnectFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GCFriendUpdateConnect(); }
	string getPacketName() const throw() { return "GCFriendUpdateConnect"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_UPDATE_CONNECT; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize() + szBYTE;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendUpdateConnectHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendUpdateConnect* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
