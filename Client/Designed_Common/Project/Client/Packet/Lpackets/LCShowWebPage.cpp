//----------------------------------------------------------------------
// 
// Filename    : LCShowWebPage.cpp
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCShowWebPage.h"

//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void LCShowWebPage::read ( SocketInputStream & iStream )
     
{
    __BEGIN_TRY

	BYTE szMessage, szURL;

	iStream.read( szMessage );

	if ( szMessage == 0 )
	{
		throw InvalidProtocolException( "szMessage == 0" );
	}
	
	if ( szMessage > 128 )
	{
		throw InvalidProtocolException( "too large message length" );
	}

	iStream.read( m_Message, szMessage );
	
	iStream.read( szURL );

	if ( szURL == 0 )
	{
		throw InvalidProtocolException( "szURL == 0" );
	}
	
	if ( szURL > 128 )
	{
		throw InvalidProtocolException( "too large url length" );
	}

	iStream.read( m_URL, szURL );

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void LCShowWebPage::write ( SocketOutputStream & oStream ) const
     
{
    __BEGIN_TRY

	BYTE szMessage = m_Message.size();
	BYTE szURL = m_URL.size();

	if ( szMessage == 0 )
	{
		throw InvalidProtocolException( "szMessage == 0" );
	}
	
	if ( szMessage > 128 )
	{
		throw InvalidProtocolException( "too large message length" );
	}

	if ( szURL == 0 )
	{
		throw InvalidProtocolException( "szURL == 0" );
	}
	
	if ( szURL > 128 )
	{
		throw InvalidProtocolException( "too large url length" );
	}

	oStream.write( szMessage );
	oStream.write( m_Message );
	oStream.write( szURL );
	oStream.write( m_URL );

    __END_CATCH
}

//----------------------------------------------------------------------
// execute packet handler
//----------------------------------------------------------------------
void LCShowWebPage::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	LCShowWebPageHandler::execute( this , pPlayer );
		
	__END_CATCH
}

std::string LCShowWebPage::toString () const
       
{
    __BEGIN_TRY

    StringStream msg;
    msg << "LCShowWebPage(Message:" << m_Message << ",URL:" << m_URL << ")";
    return msg.toString();

    __END_CATCH
}
