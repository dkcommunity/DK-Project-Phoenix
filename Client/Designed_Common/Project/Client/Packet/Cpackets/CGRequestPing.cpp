//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPing.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestPing.h"


void CGRequestPing::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPing::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPing::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGRequestPingHandler::execute(this , pPlayer);

	__END_CATCH
}
