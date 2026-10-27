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

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendResToAdd : public Packet {

public:
	GCFriendResToAdd();
	virtual ~GCFriendResToAdd() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendResToAdd"; }
	std::string toString() const ;
	
	void SetFriendName(const std::string& strName) { m_FriendName.SetString(strName); }
	std::string GetFriendName() const { return m_FriendName.GetString(); }
	
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
	
	Packet* createPacket()  { return new GCFriendResToAdd(); }
	std::string getPacketName() const  { return "GCFriendResToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_RES_TO_ADD; }

	PacketSize_t getPacketMaxSize() const  
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
	static void execute(GCFriendResToAdd* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
