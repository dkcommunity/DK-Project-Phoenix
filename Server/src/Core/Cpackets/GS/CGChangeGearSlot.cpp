//////////////////////////////////////////////////////////////////////////////
// Filename    : CGChangeGearSlot.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGChangeGearSlot.h"

void CGChangeGearSlot::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	iStream.read(m_GearSlotID);

	__END_CATCH
}

void CGChangeGearSlot::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
	
	oStream.write(m_GearSlotID);

	__END_CATCH
}

void CGChangeGearSlot::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGChangeGearSlotHandler::execute (this , pPlayer);
		
	__END_CATCH
}

