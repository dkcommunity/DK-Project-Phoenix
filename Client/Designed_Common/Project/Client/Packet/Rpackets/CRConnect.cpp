//////////////////////////////////////////////////////////////////////
// 
// Filename    : CRConnect.cpp 
// Written By  : elca@ewestsoft.com
// Description : 다른 client에 접속 요청을 한다.
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CRConnect.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
CRConnect::CRConnect () 
     
{
	__BEGIN_TRY
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
CRConnect::~CRConnect() {
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void CRConnect::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	BYTE num;
	iStream.read( num );

	if (num == 0)
		throw InvalidProtocolException("szRequestServerName==0");
	
	iStream.read( m_RequestServerName, num );
	

	iStream.read( num );

	if (num == 0)
		throw InvalidProtocolException("szRequestClientName==0");
	
	iStream.read( m_RequestClientName, num );
	

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void CRConnect::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	// servername
	BYTE num = m_RequestServerName.size();
	
	if (num == 0)
		throw InvalidProtocolException("szRequestServerName==0");

	oStream.write( num );
	oStream.write( m_RequestServerName );	


	// clientname
	num = m_RequestClientName.size();
	
	if (num == 0)
		throw InvalidProtocolException("szRequestClientName==0");

	oStream.write( num );
	oStream.write( m_RequestClientName );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void CRConnect::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CRConnectHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string CRConnect::toString () 
		const 
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "CRConnect( "
			<< "RequestServerName: " << m_RequestServerName 
			<< ",RequestClientName: " << m_RequestClientName
			<< ")";

		return msg.toString();

		__END_CATCH
	}

#endif
