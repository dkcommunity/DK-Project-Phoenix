//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGInstanceDoungeonEnter 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_INSTANCEDOUNGEON_ENTER_H__
#define __CG_INSTANCEDOUNGEON_ENTER_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGInstanceDoungeonEnter;
//
//////////////////////////////////////////////////////////////////////

class CGInstanceDoungeonEnter : public Packet {

public:
	
	// constructor
	CGInstanceDoungeonEnter() throw();
	
	// destructor
	~CGInstanceDoungeonEnter() throw();

	
public:
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_INSTANCEDOUNGEON_ENTER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szObjectID + szBYTE; }

	// get packet name
	string getPacketName() const throw() { return "CGInstanceDoungeonEnter"; }
	
	// get packet's debug string
	string toString() const throw();

	// get/set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }
	
	BYTE getSelectDoungeonIndex() const throw() { return m_SelectDoungeonIndex; }
	void setSelectDoungeonIndex(BYTE SelectDoungeonIndex) throw() { m_SelectDoungeonIndex = SelectDoungeonIndex; }

private :
	
	ObjectID_t m_ObjectID;  // NPC ObjectID
	BYTE m_SelectDoungeonIndex;

};


//////////////////////////////////////////////////////////////////////
//
// class CGCGInstanceDoungeonEnterFactory;
//
// Factory for CGInstanceDoungeonEnter
//
//////////////////////////////////////////////////////////////////////

class CGInstanceDoungeonEnterFactory : public PacketFactory {

public:
	
	// constructor
	CGInstanceDoungeonEnterFactory() throw() {}
	
	// destructor
	virtual ~CGInstanceDoungeonEnterFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGInstanceDoungeonEnter(); }

	// get packet name
	string getPacketName() const throw() { return "CGInstanceDoungeonEnter"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_INSTANCEDOUNGEON_ENTER; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szBYTE; }

};

//////////////////////////////////////////////////////////////////////
//
// class CGInstanceDoungeonEnterHandler;
//
//////////////////////////////////////////////////////////////////////

class CGInstanceDoungeonEnterHandler {
	
public:

	// execute packet's handler
	static void execute(CGInstanceDoungeonEnter* pCGInstanceDoungeonEnter, Player* player) throw(Error);
};

#endif
