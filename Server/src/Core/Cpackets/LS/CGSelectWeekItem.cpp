//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectWeekItem.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGSelectWeekItem.h"

void CGSelectWeekItem::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	iStream.read(m_SelectID);

	__END_CATCH
}
		    
void CGSelectWeekItem::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	oStream.write(m_SelectID);

	__END_CATCH
}

void CGSelectWeekItem::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGSelectWeekItemHandler::execute(this , pPlayer);

	__END_CATCH
}

string CGSelectWeekItem::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectWeekItem("
		<< "SelectID:" << (int)m_SelectID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
