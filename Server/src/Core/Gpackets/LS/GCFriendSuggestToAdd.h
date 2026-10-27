//----------------------------------------------------------------------
// 
// Filename    : GCFriendSuggestToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_SUGGEST_TO_ADD_H__
#define __GC_FRIEND_SUGGEST_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GCFriendSuggestToAdd : public Packet {

public:
	GCFriendSuggestToAdd();
	virtual ~GCFriendSuggestToAdd() throw() {}
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_SUGGEST_TO_ADD; }
	PacketSize_t getPacketSize() const throw()
	{ 
		return m_PCName.getSize();
	}

	string getPacketName() const throw() { return "GCFriendSuggestToAdd"; }
	string toString() const throw();

public:

	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strName) { m_PCName.SetString(strName); }

private :
	StringInfo m_PCName;		// 요청한 캐릭터 이름

};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendSuggestToAddFactory;
//
// Factory for GCFriendSuggestToAdd
//
//////////////////////////////////////////////////////////////////////

class GCFriendSuggestToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GCFriendSuggestToAdd(); }
	string getPacketName() const throw() { return "GCFriendSuggestToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_SUGGEST_TO_ADD; }

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

class GCFriendSuggestToAddHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendSuggestToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
