#ifndef __GAME_CLIENT__
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPowerPointHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestPowerPoint.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "DarkAssert.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestPowerPointHandler::execute (CGRequestPowerPoint* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

#endif
		
	__END_DEBUG_EX __END_CATCH
}

#endif
