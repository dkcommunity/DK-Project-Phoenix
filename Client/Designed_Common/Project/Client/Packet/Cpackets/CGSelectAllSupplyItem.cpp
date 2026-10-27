#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectAllSupplyItem.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSelectAllSupplyItem.h"

void CGSelectAllSupplyItem::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	__END_CATCH
}
		    
void CGSelectAllSupplyItem::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGSelectAllSupplyItem::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGSelectAllSupplyItemHandler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGSelectAllSupplyItem::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectAllSupplyItem("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}

#endif
