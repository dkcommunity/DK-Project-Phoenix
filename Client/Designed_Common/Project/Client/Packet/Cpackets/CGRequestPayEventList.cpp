//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPayEventList.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestPayEventList.h"


void CGRequestPayEventList::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPayEventList::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestPayEventList::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGRequestPayEventListHandler::execute(this , pPlayer);

	__END_CATCH
}
