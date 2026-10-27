//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPayEventList.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestPayEventList.h"


void CGRequestPayEventList::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPayEventList::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPayEventList::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGRequestPayEventListHandler::execute(this , pPlayer);

	__END_CATCH
}
