//////////////////////////////////////////////////////////////////////////////
// Filename    : GCCashChargeURL.cpp
// Description : PayPal Cash yükleme checkout URL'i (server -> client).
//////////////////////////////////////////////////////////////////////////////

// include files
#include "GCCashChargeURL.h"


GCCashChargeURL::GCCashChargeURL ()
	throw()
	: m_Result( CASH_CHARGE_RESULT_ERROR )
{
	// Güvenli default: başarısız — setResult(CASH_CHARGE_RESULT_OK) ancak
	// PENDING kaydı başarıyla atıldığında çağrılır.
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCCashChargeURL::read ( SocketInputStream & iStream )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_Result );

	BYTE szURL;
	iStream.read( szURL );

	// Boş URL legal (hata dönüşü) — GCShowWebMarket'in aksine 0 uzunluk kabul
	m_URL = "";
	if ( szURL > 0 )
	{
		iStream.read( m_URL, szURL );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCCashChargeURL::write ( SocketOutputStream & oStream ) const
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	if ( m_URL.size() > 255 )
		throw InvalidProtocolException( "GCCashChargeURL URL too long" );

	oStream.write( m_Result );

	BYTE szURL = (BYTE)m_URL.size();
	oStream.write( szURL );
	if ( szURL > 0 )
	{
		oStream.write( m_URL );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCCashChargeURL::execute ( Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	GCCashChargeURLHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCCashChargeURL::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "GCCashChargeURL("
		<< "Result:" << (int)m_Result
		<< ", URL:" << m_URL
		<< ")";

	return msg.toString();

	__END_CATCH
}
