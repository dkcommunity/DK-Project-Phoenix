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

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendDel : public Packet {

public:
	virtual ~GCFriendDel() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_DEL; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendDel"; }
	std::string toString() const ;
	
	std::string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const std::string& strPCName) { m_PCName.SetString(strPCName); } 

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
	
	Packet* createPacket()  { return new GCFriendDel(); }
	std::string getPacketName() const  { return "GCFriendDel"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_DEL; }

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

class GCFriendDelHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendDel* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
