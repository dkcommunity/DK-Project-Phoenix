#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectSupplyItem.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSelectSupplyItem.h"

void CGSelectSupplyItem::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_SelectID);

	__END_CATCH
}
		    
void CGSelectSupplyItem::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write(m_SelectID);

	__END_CATCH
}

void CGSelectSupplyItem::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGSelectSupplyItemHandler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGSelectSupplyItem::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectSupplyItem("
		<< "SelectID:" << (int)m_SelectID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}

#endif
