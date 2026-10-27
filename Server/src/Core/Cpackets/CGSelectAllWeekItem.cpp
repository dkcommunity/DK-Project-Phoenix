//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectAllSupplyItem.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGSelectAllWeekItem.h"

void CGSelectAllWeekItem::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	__END_CATCH
}
		    
void CGSelectAllWeekItem::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGSelectAllWeekItem::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGSelectAllWeekItemHandler::execute(this , pPlayer);

	__END_CATCH
}

string CGSelectAllWeekItem::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectAllWeekItem("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
