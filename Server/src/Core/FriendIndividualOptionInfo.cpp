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
#include "FriendIndividualOptionInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
FriendIndividualOptionInfo::FriendIndividualOptionInfo () 
     throw ()
{
	__BEGIN_TRY
	
	m_Memo.SetMaxLength(100);
	
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
FriendIndividualOptionInfo::~FriendIndividualOptionInfo () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void FriendIndividualOptionInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_Memo.read(iStream);
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void FriendIndividualOptionInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_Memo.write(oStream);

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t FriendIndividualOptionInfo::getSize() const
	throw()
{
	__BEGIN_TRY

	PacketSize_t PacketSize;
	
	PacketSize = m_Memo.getSize();
	
	return PacketSize;

	__END_CATCH
}

uint FriendIndividualOptionInfo::getMaxSize() throw() 
{
	PacketSize_t PacketSize;
	
	PacketSize = StringInfo::getMaxSize();
	
	return PacketSize;
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string FriendIndividualOptionInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "FriendIndividualOptionInfo(" 
		<< "Memo:" << m_Memo.toString()
		<< ")";
	
	return msg.toString();

	__END_CATCH
}

