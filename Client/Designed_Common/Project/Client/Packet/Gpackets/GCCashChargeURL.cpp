//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCashChargeURL.cpp
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge — checkout URL cevabi.
//               String read/write GCShowWebMarket.cpp oruntusunun aynasi.
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCashChargeURL.h"


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCCashChargeURL::read ( SocketInputStream & iStream )

{
	__BEGIN_TRY

	iStream.read( m_Result );

	BYTE szURL;
	iStream.read( szURL );

	// SocketInputStream::read(string, 0) InvalidProtocolException firlatir —
	// hata cevabinda URL bos gelebilir, bu durumda string read atlanmali.
	if ( szURL > 0 )
	{
		iStream.read( m_URL, szURL );
	}
	else
	{
		m_URL.clear();
	}

	// OK cevabi URL'siz gelirse protokol ihlali
	if ( m_Result == 0 && m_URL.empty() )
		throw InvalidProtocolException( "GCCashChargeURL result==0 but empty URL" );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCCashChargeURL::write ( SocketOutputStream & oStream ) const

{
	__BEGIN_TRY

	BYTE szURL = m_URL.size();

	if ( m_Result == 0 && szURL == 0 )
		throw InvalidProtocolException( "GCCashChargeURL result==0 but empty URL" );

	oStream.write( m_Result );
	oStream.write( szURL );

	if ( szURL > 0 )
		oStream.write( m_URL );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCCashChargeURL::execute ( Player * pPlayer )

{
	__BEGIN_TRY

	GCCashChargeURLHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////
std::string GCCashChargeURL::toString () const

{
	__BEGIN_TRY

	StringStream msg;

	msg << "GCCashChargeURL("
		<< "Result:" << (int)m_Result
		<< "URL:" << m_URL
		<< ")";

	return msg.toString();

	__END_CATCH
}
