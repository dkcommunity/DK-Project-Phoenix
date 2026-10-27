//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_INDIVIDUAL_OPTION_INFO_H__
#define __FRIEND_INDIVIDUAL_OPTION_INFO_H__

#include "ContentsFilter.h"

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class FriendIndividualOptionInfo;
//
//
//////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class FriendIndividualOptionInfo {

public :
	
	// constructor
	FriendIndividualOptionInfo () ;
	
	// destructor
	~FriendIndividualOptionInfo() ;

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
	
	void SetMemo(const std::string& strMemo) { m_Memo.SetString(strMemo); }
	std::string GetMemo() const  { return m_Memo.GetString(); }

private :
	StringInfo m_Memo;

};
#endif //__FRIEND_ADDITION

#endif
