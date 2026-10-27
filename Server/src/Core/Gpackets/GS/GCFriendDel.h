//----------------------------------------------------------------------
// 
// Filename    : GCFriendDel.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_DEL_H__
#define __GC_FRIEND_DEL_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GCFriendDel : public Packet {

public:
	virtual ~GCFriendDel() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_DEL; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendDel"; }
	string toString() const throw();
	
	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strPCName) { m_PCName.SetString(strPCName); } 

private :
	StringInfo m_PCName;
};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendDelFactory;
//
// Factory for GCFriendDel
//
//////////////////////////////////////////////////////////////////////

class GCFriendDelFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GCFriendDel(); }
	string getPacketName() const throw() { return "GCFriendDel"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_DEL; }

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

class GCFriendDelHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendDel* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
