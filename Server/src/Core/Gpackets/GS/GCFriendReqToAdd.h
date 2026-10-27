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

class GCFriendReqToAdd : public Packet 
{
public:
	GCFriendReqToAdd();
	virtual ~GCFriendReqToAdd() throw() {}
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketSize() const throw()
	{ 
		return m_PCName.getSize();
	}

	string getPacketName() const throw() { return "GCFriendReqToAdd"; }
	string toString() const throw();

public:

	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strName) { m_PCName.SetString(strName); }
	
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
	
	Packet* createPacket() throw() { return new GCFriendReqToAdd(); }
	string getPacketName() const throw() { return "GCFriendReqToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_REQ_TO_ADD; }

	PacketSize_t getPacketMaxSize() const throw() 
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
	static void execute(GCFriendReqToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
