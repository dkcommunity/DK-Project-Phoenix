//----------------------------------------------------------------------
//
// Filename    : GCFriendResToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GCFriendResToAdd.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "Player.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendResToAdd.h"

#endif

void GCFriendResToAddHandler::execute ( GCFriendResToAdd* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	
#endif
		
	__END_CATCH
}
