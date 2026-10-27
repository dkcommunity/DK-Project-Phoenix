//////////////////////////////////////////////////////////////////////
// 
// Filename    : CRRequest.h 
// Written By  : crazydog
// Description : Effect 제거.
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CR_REQUEST_H__
#define __CR_REQUEST_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

enum CR_REQUEST_CODE
{
	CR_REQUEST_NULL,

	// 조만간 구현 예정.. 과연.. - -;
	CR_REQUEST_FILE_GUILD_MARK,
	CR_REQUEST_FILE_PROFILE,
};

//////////////////////////////////////////////////////////////////////
//
// class CRRequest;
//
// 게임서버에서 클라이언트로 자신의 변형된 데이터를 알려주기 위한 객채
// RemoveEffectrmation, SkillToObjectOK 등에 실려서 날아간다.
//
//////////////////////////////////////////////////////////////////////
class CRRequest : public Packet
{

public :
	
	// constructor
	CRRequest () ;
	
	// destructor
	~CRRequest() ;
	
public :
    PacketID_t getPacketID () const  { return PACKET_CR_REQUEST; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CRRequest"; }
		
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
	PacketSize_t getPacketSize () const  { return szBYTE + szBYTE + m_RequestName.size(); }
	static PacketSize_t getPacketMaxSize()  { return szBYTE + szBYTE + 20;}

	// get / set ListNumber
	CR_REQUEST_CODE getCode() const  { return m_Code; }
	void setCode( CR_REQUEST_CODE code)  { m_Code = code; }

	const std::string& getRequestName() const  { return m_RequestName; }
	void setRequestName(const char* pName)  { m_RequestName = pName; }

protected :
	
	CR_REQUEST_CODE		m_Code;

	std::string				m_RequestName;
};

//////////////////////////////////////////////////////////////////////
//
// class CRRequestFactory;
//
// Factory for CRRequest
//
//////////////////////////////////////////////////////////////////////

class CRRequestFactory : public PacketFactory {

public :
	
	// constructor
	CRRequestFactory ()  {}
	
	// destructor
	virtual ~CRRequestFactory() {}

	
public :
	
	// create packet
	Packet * createPacket ()  { return new CRRequest(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CRRequest"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CR_REQUEST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const  { return szBYTE + szBYTE + 20;}

};


//////////////////////////////////////////////////////////////////////
//
// class CRRequestHandler;
//
//////////////////////////////////////////////////////////////////////

class CRRequestHandler {

public :

	// execute packet's handler
	static void execute ( CRRequest * pCRRequest , Player * pPlayer ) ;

};


#endif
