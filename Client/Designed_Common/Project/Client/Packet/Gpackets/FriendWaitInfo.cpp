//////////////////////////////////////////////////////////////////////
// 
// Filename    : 
// Written By  :
// Description :
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "FriendWaitInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#if __CONTENTS(__FRIEND_ADDITION)
const int FriendWaitInfo::m_iMaxFriendName = 20;

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
FriendWaitInfo::FriendWaitInfo () 
     
{
	__BEGIN_TRY
	
	m_FriendName.SetMaxLength(20);
	
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
FriendWaitInfo::~FriendWaitInfo() {
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void FriendWaitInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	m_FriendName.read(iStream);
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void FriendWaitInfo::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	m_FriendName.write(oStream);

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t FriendWaitInfo::getSize() const
	
{
	__BEGIN_TRY

	PacketSize_t PacketSize;
	
	PacketSize = m_FriendName.getSize();
	
	return PacketSize;

	__END_CATCH
}

uint FriendWaitInfo::getMaxSize()  
{
	PacketSize_t PacketSize;
	
	PacketSize =  StringInfo::getMaxSize();
	
	return PacketSize;
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string FriendWaitInfo::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;

	msg << "FriendWaitInfo(" 
		<< "FriendName:" << GetFriendName() 
		<< ")";
	
	return msg.toString();

	__END_CATCH
}
#endif //__FRIEND_ADDITION
