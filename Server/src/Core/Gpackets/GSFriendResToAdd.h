//----------------------------------------------------------------------
// 
// Filename    : GSFriendResToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FRIEND_RES_TO_ADD_H__
#define __GS_FRIEND_RES_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GSFriendResToAdd : public Packet {

public:
	GSFriendResToAdd();
	virtual ~GSFriendResToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GS_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GSFriendResToAdd"; }
	string toString() const throw();
	
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
	string GetSender() const { return m_Sender.GetString(); }
	
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() const { return m_FriendName.GetString(); }
	
	void SetResponse(BYTE Response) { m_Response = Response; }
	BYTE GetResponse() const { return m_Response; }

private :
	StringInfo m_Sender;
	StringInfo m_FriendName;
	BYTE m_Response;
};


//////////////////////////////////////////////////////////////////////
//
// class GSFriendResToAddFactory;
//
// Factory for GSFriendResToAdd
//
//////////////////////////////////////////////////////////////////////

class GSFriendResToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GSFriendResToAdd(); }
	string getPacketName() const throw() { return "GSFriendResToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FRIEND_RES_TO_ADD; }

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

class GSFriendResToAddHandler {
	
public:

	// execute packet's handler
	static void execute(GSFriendResToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
