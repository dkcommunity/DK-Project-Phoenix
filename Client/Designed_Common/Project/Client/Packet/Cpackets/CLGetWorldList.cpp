//////////////////////////////////////////////////////////////////////////////
// Filename    : CLGetWorldList.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CLGetWorldList.h"

void CLGetWorldList::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
	__END_CATCH
}

void CLGetWorldList::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
	__END_CATCH
}

void CLGetWorldList::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CLGetWorldListHandler::execute(this, pPlayer);
	#endif

	__END_CATCH
}

