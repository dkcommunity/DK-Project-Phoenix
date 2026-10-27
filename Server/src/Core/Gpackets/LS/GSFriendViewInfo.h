//----------------------------------------------------------------------
// 
// Filename    : GSFriendViewInfo.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FRIEND_VIEW_INFO_H__
#define __GS_FRIEND_VIEW_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GSFriendViewInfo : public Packet {

public:
	GSFriendViewInfo();
	virtual ~GSFriendViewInfo() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GS_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GSFriendViewInfo"; }
	string toString() const throw();
	
	string GetSender() const { return m_Sender.GetString(); }
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
		
	string GetFriendName() const { return m_FriendName.GetString(); }
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); } 

private :
	StringInfo m_Sender;
	StringInfo m_FriendName;
};


//////////////////////////////////////////////////////////////////////
//
// class GSFriendViewInfoFactory;
//
// Factory for GSFriendViewInfo
//
//////////////////////////////////////////////////////////////////////

class GSFriendViewInfoFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GSFriendViewInfo(); }
	string getPacketName() const throw() { return "GSFriendViewInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FRIEND_VIEW_INFO; }

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

class GSFriendViewInfoHandler {
	
public:

	// execute packet's handler
	static void execute(GSFriendViewInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
