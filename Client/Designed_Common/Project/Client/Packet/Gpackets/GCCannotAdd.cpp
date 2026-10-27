//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCCannotAdd.cc 
// Written By  : elca@ewestsoft.com
// Description : CGMove가 날아 왓을때 자기 자신에게 OK 사인을 날리기
//               위한 패킷 클래스 함수 정의
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCannotAdd.h"


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCCannotAdd::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	iStream.read( m_ObjectID );

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GCCannotAdd::read
	iStream.read( m_CannotReason );
#endif // __GEAR_SWAP_CHANGE
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCCannotAdd::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	oStream.write( m_ObjectID );

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GCCannotAdd::write
	oStream.write( m_CannotReason );
#endif // __GEAR_SWAP_CHANGE

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCCannotAdd::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCCannotAddHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string GCCannotAdd::toString () const
		   
	{
		__BEGIN_TRY

		StringStream msg;
		msg << "GCCannotAdd("
			<< "ObjectId:" << (int)m_ObjectID
			<< ")";
		return msg.toString();

		__END_CATCH
	}
#endif
