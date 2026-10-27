//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectAllWeekItem.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSelectAllWeekItem.h"

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
void CGSelectAllWeekItem::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	__END_CATCH
}
		    
void CGSelectAllWeekItem::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGSelectAllWeekItem::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGSelectAllWeekItemHandler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGSelectAllWeekItem::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectAllWeekItem("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif //__PREMIUM_GIVE_ITEM_UI
