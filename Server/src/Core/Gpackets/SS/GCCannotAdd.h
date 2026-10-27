//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCCannotAdd.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CANNOT_ADD_H__
#define __GC_CANNOT_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCCannotAdd;
//
//////////////////////////////////////////////////////////////////////

class GCCannotAdd : public Packet {

public :
	GCCannotAdd()
	{
		m_CannotReason = CANNOT_REASON_FAIL;
	}

    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_CANNOT_ADD; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static GCCannotAddPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize() const throw() { return szObjectID + sizeof(eCannotReason); }

	// get packet name
	string getPacketName() const throw() { return "GCCannotAdd"; }
	
	// get packet's debug string
	string toString() const throw();
	
public :
	enum eCannotReason
	{
		CANNOT_REASON_FAIL = 0,

		CANNOT_REASON_NOT_EXIST_ITEM = 1,
		CANNOT_REASON_PREMIUM_FEATURE = 2,

		// GearSlot
		CANNOT_REASON_DIFFERENT_GEARSLOTID = 10

	};

	// get / set ObjectID
	ObjectID_t getObjectID() throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }

	eCannotReason getCannotReason() throw() { return m_CannotReason; }
	void setCannotReason(eCannotReason CannotReason) throw() { m_CannotReason = CannotReason; }


private :
	
	// ObjectID
	ObjectID_t m_ObjectID;
	eCannotReason m_CannotReason;

};


//////////////////////////////////////////////////////////////////////
//
// class GCCannotAddFactory;
//
// Factory for GCCannotAdd
//
//////////////////////////////////////////////////////////////////////

class GCCannotAddFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCCannotAdd(); }

	// get packet name
	string getPacketName() const throw() { return "GCCannotAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_CANNOT_ADD; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCCannotAddPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + sizeof(GCCannotAdd::eCannotReason); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCCannotAddHandler;
//
//////////////////////////////////////////////////////////////////////

class GCCannotAddHandler {
	
public :

	// execute packet's handler
	static void execute(GCCannotAdd* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
