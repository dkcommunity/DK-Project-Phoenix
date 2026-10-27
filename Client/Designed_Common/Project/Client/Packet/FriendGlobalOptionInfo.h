//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_GLOBAL_OPTION_INFO_H__
#define __FRIEND_GLOBAL_OPTION_INFO_H__

#include "ContentsFilter.h"

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class FriendGlobalOptionInfo;
//
//
//////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class FriendGlobalOptionInfo {

public :
	
	// constructor
	FriendGlobalOptionInfo () ;
	
	// destructor
	~FriendGlobalOptionInfo() ;

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () const ;
	static uint getMaxSize() ;

	// get packet's debug std::string
	std::string toString () const ;
	
	void SetOptionToDenyAddRequest(bool bOption) { m_OptionToDenyAddRequest = bOption ? 1 : 0; }
	bool GetOptionToDenyAddRequest() const  { return m_OptionToDenyAddRequest ? true : false; }

private :
	BYTE m_OptionToDenyAddRequest;
};
#endif //__FRIEND_ADDITION

#endif
