#ifndef __GAME_CLIENT__
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFailQuestHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGFailQuest.h"

#ifdef __GAME_SERVER__

#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "NPC.h"
#include "mission/EventQuestAdvance.h"
#include "mission/QuestManager.h"
#include "mission/QuestInfoManager.h"

#include "Gpackets/GCNPCResponse.h"

#include <cstdio>


void CGFailQuestHandler::execute (CGFailQuest* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	GamePlayer* pGP = dynamic_cast<GamePlayer*>( pPlayer );
	Assert( pGP != NULL );
		
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGP->getCreature());

	pPC->getQuestManager()->adjustQuestStatus();

	QuestMessage code;
	if ( pPacket->isFail() )
		code = pPC->getQuestManager()->failQuest();
	else
		code = pPC->getQuestManager()->cancelQuest();

	if ( code == CANCEL_SUCCESS )
	{
		pPC->sendCurrentQuestInfo();

		//std::cout << "Quest ì‹œìž‘ " << pPC->getName() << " " << pPacket->getQuestID() << std::endl;
	}
	else
	{
		//std::cout << "Quest ì‹œìž‘ ì‹¤íŒ¨ " << pPC->getName() << " " << (int)code << std::endl;
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}


#endif	// __GAME_SERVER__

#endif
