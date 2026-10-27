//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPing.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestPing.h"


void CGRequestPing::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPing::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPing::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGRequestPingHandler::execute(this , pPlayer);

	__END_CATCH
}
