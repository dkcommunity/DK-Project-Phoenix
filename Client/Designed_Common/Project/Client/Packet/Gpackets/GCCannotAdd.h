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
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//GCCannotAdd				m_CannotReason = CANNOT_REASON_FAIL;
		m_CannotReason = CANNOT_REASON_FAIL;
#endif
	}

    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_GC_CANNOT_ADD; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static GCCannotAddPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize () const  
	{ 
		PacketSize_t	szSize	= 0;
		szSize	+=	szObjectID; 
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//  GCCannotAdd		eCannotReason 사이즈 패킷 추가
		szSize	+=	sizeof(int);
#endif // __GEAR_SWAP_CHANGE
		return szSize;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GCCannotAdd"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GCCannotAdd	eCannotReason 상수 선언
	enum eCannotReason
	{
		CANNOT_REASON_FAIL = 0,

		CANNOT_REASON_NOT_EXIST_ITEM = 1,
		CANNOT_REASON_PREMIUM_FEATURE = 2,

		// GearSlot
		CANNOT_REASON_DIFFERENT_GEARSLOTID = 10,

	};
#endif // __GEAR_SWAP_CHANGE

	// get / set ObjectID
	ObjectID_t getObjectID()  { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID )  { m_ObjectID = ObjectID; }

#if __CONTENTS(__GEAR_SWAP_CHANGE)		//GCCannotAdd  eCannotReason 상수값 설정 및 얻는 함수
	int getCannotreason()  { return m_CannotReason; }
	void setCannotreason( int CannotReason )  { m_CannotReason = CannotReason; }
#endif // __GEAR_SWAP_CHANGE

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

#if __CONTENTS(__GEAR_SWAP_CHANGE)	////  GCCannotAdd		eCannotReason 변수 추가
	int m_CannotReason;
#endif // __GEAR_SWAP_CHANGE
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
	Packet * createPacket ()  { return new GCCannotAdd(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GCCannotAdd"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_GC_CANNOT_ADD; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCCannotAddPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize () const  
	{
		PacketSize_t	szSize	= 0;
		szSize	+= szObjectID;
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//  GCCannotAddFactory		eCannotReason 사이즈 패킷 추가
		szSize	+= sizeof(int);
#endif // __GEAR_SWAP_CHANGE
		return szSize; 
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCCannotAddHandler;
//
//////////////////////////////////////////////////////////////////////

class GCCannotAddHandler {
	
public :

	// execute packet's handler
	static void execute ( GCCannotAdd * pPacket , Player * player ) ;
};

#endif
