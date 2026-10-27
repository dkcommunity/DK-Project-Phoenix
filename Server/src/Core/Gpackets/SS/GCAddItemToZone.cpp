//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddItemToZone.cc 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCAddItemToZone.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"
#include "Assert.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCAddItemToZone::GCAddItemToZone() 
     throw ()
{
	__BEGIN_TRY
//	m_ListNum = 0;
	__END_CATCH
}

	
//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCAddItemToZone::~GCAddItemToZone() 
    throw ()
{
	__BEGIN_TRY

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCAddItemToZone::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	iStream.read( m_X );
	iStream.read( m_Y );
	
	m_PCItemInfo.read(iStream);

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCAddItemToZone::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_X );
	oStream.write( m_Y );
	
	m_PCItemInfo.write(oStream);

	__END_CATCH
}
