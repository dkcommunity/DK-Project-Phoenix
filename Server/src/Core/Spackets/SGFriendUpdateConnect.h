//----------------------------------------------------------------------
// 
// Filename    : SGFriendUpdateConnect.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_UPDATE_CONNECT_H__
#define __SG_FRIEND_UPDATE_CONNECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class SGFriendUpdateConnect : public Packet {

public:
	SGFriendUpdateConnect();
	virtual ~SGFriendUpdateConnect() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_UPDATE_CONNECT; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendUpdateConnect"; }
	string toString() const throw();
	
	string GetReceiver() const { return m_Receiver.GetString(); }
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
		
	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strPCName) { m_PCName.SetString(strPCName); }
	
	string GetServerName() const { return m_ServerName.GetString(); }
	void SetServerName(const string& strServerName) { m_ServerName.SetString(strServerName); }
	
	bool GetConnect() const { return m_Connect == 0 ? false : true ; }
	void SetConnect(bool bConnect) { m_Connect = bConnect ? 1 : 0; } 

private :
	StringInfo m_Receiver;
	StringInfo m_PCName;
	StringInfo m_ServerName;
	BYTE m_Connect;
};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendUpdateConnectFactory;
//
// Factory for SGFriendUpdateConnect
//
//////////////////////////////////////////////////////////////////////

class SGFriendUpdateConnectFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendUpdateConnect(); }
	string getPacketName() const throw() { return "SGFriendUpdateConnect"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_UPDATE_CONNECT; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize() + StringInfo::getMaxSize() + szBYTE;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendUpdateConnectHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendUpdateConnect* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
