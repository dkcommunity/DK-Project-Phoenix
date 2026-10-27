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
#include "FriendGlobalOptionInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#if __CONTENTS(__FRIEND_ADDITION)
//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
FriendGlobalOptionInfo::FriendGlobalOptionInfo () 
     
{
	__BEGIN_TRY
	
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
FriendGlobalOptionInfo::~FriendGlobalOptionInfo() {
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void FriendGlobalOptionInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	iStream.read( m_OptionToDenyAddRequest );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void FriendGlobalOptionInfo::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	oStream.write( m_OptionToDenyAddRequest );

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t FriendGlobalOptionInfo::getSize() const
	
{
	__BEGIN_TRY

	PacketSize_t PacketSize;
	
	PacketSize = szBYTE; 
	
	return PacketSize;

	__END_CATCH
}

uint FriendGlobalOptionInfo::getMaxSize()  
{
	PacketSize_t PacketSize;
	
	PacketSize = szBYTE;
	
	return PacketSize;
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string FriendGlobalOptionInfo::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;

	msg << "FriendGlobalOptionInfo(" 
		<< "OptionToDenyAddRequest:" << GetOptionToDenyAddRequest()
		<< ")";
	
	return msg.toString();

	__END_CATCH
}
#endif //__FRIEND_ADDITION
