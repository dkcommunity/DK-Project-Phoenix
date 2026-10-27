//////////////////////////////////////////////////////////////////////////////
// Filename    : CGStoreOpen.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGStoreOpen.h"

void CGStoreOpen::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY

	__END_CATCH
}

void CGStoreOpen::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	__END_CATCH
}

void CGStoreOpen::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGStoreOpenHandler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGStoreOpen::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGStoreOpen("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
