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

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendSuggestToAdd : public Packet {

public:
	GCFriendSuggestToAdd();
	virtual ~GCFriendSuggestToAdd() {}
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_SUGGEST_TO_ADD; }
	PacketSize_t getPacketSize() const 
	{ 
		return m_PCName.getSize();
	}

	std::string getPacketName() const  { return "GCFriendSuggestToAdd"; }
	std::string toString() const ;

public:

	std::string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const std::string& strName) { m_PCName.SetString(strName); }

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
	
	Packet* createPacket()  { return new GCFriendSuggestToAdd(); }
	std::string getPacketName() const  { return "GCFriendSuggestToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_SUGGEST_TO_ADD; }

	PacketSize_t getPacketMaxSize() const  
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
	static void execute(GCFriendSuggestToAdd* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
