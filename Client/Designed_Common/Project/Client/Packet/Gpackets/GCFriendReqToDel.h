//----------------------------------------------------------------------
// 
// Filename    : GCFriendReqToDel.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_REQ_TO_DEL_H__
#define __GC_FRIEND_REQ_TO_DEL_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendReqToDel : public Packet {

public:
	virtual ~GCFriendReqToDel() {}
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_REQ_TO_DEL; }
	PacketSize_t getPacketSize() const 
	{ 
		return szBYTE + m_strPCName.size();
	}

	std::string getPacketName() const  { return "GCFriendReqToDel"; }
	std::string toString() const ;

public:
	enum ResultCode
	{
		RESULT_SUCCESS = 0,
		RESULT_UNKNOWN_ERROR,		
		RESULT_NOT_EXIST			// 존재하지 않는 캐릭터임
	};

	std::string& getPCName() { return m_strPCName; }
	void setPCName(const std::string& strPCName) { m_strPCName = strPCName; }
	
	ResultCode getResult() { return (ResultCode)m_Result; }
	void setResult(ResultCode result) { m_Result = (BYTE)result; }

private :
	std::string m_strPCName;		// 요청한 캐릭터 이름
	BYTE m_Result;			// 요청 결과

};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendReqToDelFactory;
//
// Factory for GCFriendReqToDel
//
//////////////////////////////////////////////////////////////////////

class GCFriendReqToDelFactory : public PacketFactory {

public:
	
	Packet* createPacket()  { return new GCFriendReqToDel(); }
	std::string getPacketName() const  { return "GCFriendReqToDel"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_REQ_TO_DEL; }

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

class GCFriendReqToDelHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendReqToDel* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
