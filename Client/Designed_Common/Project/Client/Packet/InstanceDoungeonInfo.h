//////////////////////////////////////////////////////////////////////
// 
// Filename    : InstanceDoungeonInfo.h 
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __INSTANCEDOUNGEON_INFO_H__
#define __INSTANCEDOUNGEON_INFO_H__

// include files
#include "Client_PCH.h"
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class InstanceDoungeonInfo;
//
//
//////////////////////////////////////////////////////////////////////

class InstanceDoungeonInfo {

public :
	
	// constructor
	InstanceDoungeonInfo () ;
	
	// destructor
	~InstanceDoungeonInfo() ;

public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () ;

	static uint getMaxSize()  {
		return szBYTE + szBYTE + 40;
	}

	// get packet's debug std::string
	std::string toString () const ;

	BYTE getDoungeonIndex() const  { return m_DoungeonIndex; }
	void setDoungeonIndex( BYTE DoungeonIndex )  { m_DoungeonIndex = DoungeonIndex; }

	std::string getDoungeonName() const  { return m_DoungeonName; }
	void setDoungeonName( const std::string& DoungeonName )  { m_DoungeonName = DoungeonName; }

private :

	BYTE m_DoungeonIndex;
	std::string m_DoungeonName;

};

#endif
