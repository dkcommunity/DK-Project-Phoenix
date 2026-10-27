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
#include "FriendGlobalOptionInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
FriendGlobalOptionInfo::FriendGlobalOptionInfo () 
     throw ()
{
	__BEGIN_TRY
	
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
FriendGlobalOptionInfo::~FriendGlobalOptionInfo () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void FriendGlobalOptionInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	iStream.read( m_AllowToRequestAddition );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void FriendGlobalOptionInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_AllowToRequestAddition );

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t FriendGlobalOptionInfo::getSize() const
	throw()
{
	__BEGIN_TRY

	PacketSize_t PacketSize;
	
	PacketSize = szBYTE; 
	
	return PacketSize;

	__END_CATCH
}

uint FriendGlobalOptionInfo::getMaxSize() throw() 
{
	PacketSize_t PacketSize;
	
	PacketSize = szBYTE;
	
	return PacketSize;
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string FriendGlobalOptionInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "FriendGlobalOptionInfo(" 
		<< "AllowToRequestAddition:" << (int)GetAllowToRequestAddition()
		<< ")";
	
	return msg.toString();

	__END_CATCH
}

