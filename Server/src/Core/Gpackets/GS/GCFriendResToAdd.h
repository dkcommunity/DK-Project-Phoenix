//----------------------------------------------------------------------
// 
// Filename    : GCFriendResToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_RES_TO_ADD_H__
#define __GC_FRIEND_RES_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GCFriendResToAdd : public Packet {

public:
	GCFriendResToAdd();
	virtual ~GCFriendResToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendResToAdd"; }
	string toString() const throw();
	
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() const { return m_FriendName.GetString(); }
	
	void SetResultCode(BYTE ResultCode) { m_ResultCode = ResultCode; }
	BYTE GetResultCode() const { return m_ResultCode; }

private :
	StringInfo m_FriendName;
	BYTE m_ResultCode;
};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendResToAddFactory;
//
// Factory for GCFriendResToAdd
//
//////////////////////////////////////////////////////////////////////

class GCFriendResToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GCFriendResToAdd(); }
	string getPacketName() const throw() { return "GCFriendResToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_RES_TO_ADD; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + szBYTE;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendResToAddHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendResToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
