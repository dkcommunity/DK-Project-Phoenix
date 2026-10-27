//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_GLOBAL_OPTION_INFO_H__
#define __FRIEND_GLOBAL_OPTION_INFO_H__

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

class FriendGlobalOptionInfo {

public :
	
	// constructor
	FriendGlobalOptionInfo () throw ();
	
	// destructor
	~FriendGlobalOptionInfo () throw ();

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
	
	void SetAllowToRequestAddition(bool bOption) { m_AllowToRequestAddition = bOption ? 1 : 0; }
	bool GetAllowToRequestAddition() const  { return m_AllowToRequestAddition ? true : false; }

private :
	BYTE m_AllowToRequestAddition;
};

#endif
