//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGRequestIP.h 
// Written By  : crazydog
// Description : Effect 제거.
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_IP_H__
#define __CG_REQUEST_IP_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRequestIP;
//
// client에서 server로 누군가의 IP를 요청하는데
// 누군가가 근처에 있다면 objectID로 요청하고
// 아니면.. 캐릭터 이름으로 요청한다.
//
//////////////////////////////////////////////////////////////////////

class CGRequestIP : public Packet
{

public :
	
	// constructor
	CGRequestIP () ;
	
	// destructor
	~CGRequestIP() ;
	
public :
    PacketID_t getPacketID () const  { return PACKET_CG_REQUEST_IP; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGRequestIP"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif


    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;
	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getPacketSize () const  { return szBYTE + m_Name.size(); }
	static PacketSize_t getPacketMaxSize()  { return szBYTE + 10;}

	// get&set ObjectID
	const std::string& getName() const  { return m_Name; }
	void setName( const char* pName)  { m_Name = pName;}


protected :
	

	std::string			m_Name;

};

//////////////////////////////////////////////////////////////////////
//
// class CGRequestIPFactory;
//
// Factory for CGRequestIP
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGRequestIPFactory : public PacketFactory {

public :
	
	// constructor
	CGRequestIPFactory ()  {}
	
	// destructor
	virtual ~CGRequestIPFactory() {}

	
public :
	
	// create packet
	Packet * createPacket ()  { return new CGRequestIP(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGRequestIP"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_REQUEST_IP; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const  { return szBYTE + 10;}

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGRequestIPHandler;
//
//////////////////////////////////////////////////////////////////////

class CGRequestIPHandler {

public :

	// execute packet's handler
	static void execute ( CGRequestIP * pCGRequestIP , Player * pPlayer ) ;

};


#endif
