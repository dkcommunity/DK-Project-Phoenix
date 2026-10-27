//----------------------------------------------------------------------
// 
// Filename    : GCFriendReqToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_REQ_TO_ADD_H__
#define __GC_FRIEND_REQ_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendReqToAdd : public Packet 
{
public:
	GCFriendReqToAdd();
	virtual ~GCFriendReqToAdd() {}
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketSize() const 
	{ 
		return m_PCName.getSize();
	}

	std::string getPacketName() const  { return "GCFriendReqToAdd"; }
	std::string toString() const ;

public:

	std::string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const std::string& strName) { m_PCName.SetString(strName); }
	
	BYTE GetResultCode() { return m_ResultCode; }
	void SetResultCode(BYTE code) { m_ResultCode = code; }

private :
	StringInfo m_PCName;		// 요청한 캐릭터 이름
	BYTE m_ResultCode;		// 실패 코드

};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendReqToAddFactory;
//
// Factory for GCFriendReqToAdd
//
//////////////////////////////////////////////////////////////////////

class GCFriendReqToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket()  { return new GCFriendReqToAdd(); }
	std::string getPacketName() const  { return "GCFriendReqToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_REQ_TO_ADD; }

	PacketSize_t getPacketMaxSize() const  
	{ 
		return szBYTE + 20;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendReqToAddHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendReqToAdd* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
