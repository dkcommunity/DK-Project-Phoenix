//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestWebMarket.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestWebMarket.h"

void CGRequestWebMarket::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY

	iStream.read( m_Code );

	__END_CATCH
}

void CGRequestWebMarket::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write( m_Code );

	__END_CATCH
}

void CGRequestWebMarket::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGRequestWebMarketHandler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGRequestWebMarket::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestWebMarket("
		<< "Code:" << (int)m_Code
		<< ")" ;
	return msg.toString();

	__END_CATCH
}

