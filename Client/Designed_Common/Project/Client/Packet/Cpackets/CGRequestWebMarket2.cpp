//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestWebMarket.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestWebMarket2.h"

void CGRequestWebMarket2::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY

	iStream.read( m_Code );

	__END_CATCH
}

void CGRequestWebMarket2::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write( m_Code );

	__END_CATCH
}

void CGRequestWebMarket2::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGRequestWebMarket2Handler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGRequestWebMarket2::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestWebMarket2("
		<< "Code:" << (int)m_Code
		<< ")" ;
	return msg.toString();

	__END_CATCH
}

