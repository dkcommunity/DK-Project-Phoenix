//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestWebMarket.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestWebMarket.h"

void CGRequestWebMarket::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	iStream.read( m_Code );

	__END_CATCH
}

void CGRequestWebMarket::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write( m_Code );

	__END_CATCH
}

void CGRequestWebMarket::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGRequestWebMarketHandler::execute(this , pPlayer);

	__END_CATCH
}

string CGRequestWebMarket::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestWebMarket("
		<< "Code:" << (int)m_Code
		<< ")" ;
	return msg.toString();

	__END_CATCH
}

