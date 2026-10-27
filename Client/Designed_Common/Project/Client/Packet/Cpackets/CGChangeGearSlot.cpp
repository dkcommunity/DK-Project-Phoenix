//////////////////////////////////////////////////////////////////////////////
// Filename    : CGChangeGearSlot.cpp 
// Written By  : shootkj
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGChangeGearSlot.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#if __CONTENTS(__GEAR_SWAP_CHANGE) // CGChangeGearSlot.cpp
void CGChangeGearSlot::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY

	iStream.read(m_GearSlotID);

	__END_CATCH
}

void CGChangeGearSlot::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_GearSlotID);

	__END_CATCH
}

void CGChangeGearSlot::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
//	CGChangeGearSlotHandler::execute (this , pPlayer);
		
	__END_CATCH
}

#endif //__GEAR_SWAP_CHANGE
