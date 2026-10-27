//////////////////////////////////////////////////////////////////////////////
// Filename    : CGVerifyTimeHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGVerifyTime.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Ousters.h"
	#include "DB.h"
	#include "Properties.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGVerifyTimeHandler::execute (CGVerifyTime* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	if (!(pGamePlayer->verifySpeed(pPacket))) 
	{
		pGamePlayer->saveSpeedHackPlayer();
		throw DisconnectException("½ºÇÇµåÇÙ µü! °É·Á½á!!");
	}

#endif
		
	__END_DEBUG_EX __END_CATCH
}


