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

class FriendWaitInfo {

public :
	
	// constructor
	FriendWaitInfo () throw ();
	
	// destructor
	~FriendWaitInfo () throw ();

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () const throw ();
	static uint getMaxSize() throw();

	// get packet's debug string
	string toString () const throw ();
	
	void SetFriendName(const string& strFriendName) { m_FriendName.SetString(strFriendName); }
	string GetFriendName() const  { return m_FriendName.GetString(); }

private :
	StringInfo m_FriendName;		// 친구 이름
};

#endif
