//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_WAIT_INFO_H__
#define __FRIEND_WAIT_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class FriendWaitInfo;
//
//
//////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class FriendWaitInfo {

public :
	
	// constructor
	FriendWaitInfo () ;
	
	// destructor
	~FriendWaitInfo() ;

public :
	
	static const int m_iMaxFriendName;
	
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
	
	void SetFriendName(const std::string& strFriendName) { m_FriendName.SetString(strFriendName); }
	std::string GetFriendName() const  { return m_FriendName.GetString(); }

private :
	StringInfo m_FriendName;		// 친구 이름
};
#endif //__FRIEND_ADDITION

#endif
