//////////////////////////////////////////////////////////////////////////////
// ActionOpenAuctionHouse.cpp
//////////////////////////////////////////////////////////////////////////////

#include "ActionOpenAuctionHouse.h"
#include "Assert.h"
#include "AuctionHouseManager.h"
#include "Creature.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "Gpackets/GCNPCResponse.h"

void ActionOpenAuctionHouse::read(PropertyBuffer& propertyBuffer)
{
	__BEGIN_TRY
	__END_CATCH
}

void ActionOpenAuctionHouse::execute(Creature* pNPC, Creature* pCreature)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert(pGamePlayer != NULL);

	if (g_pAuctionHouseManager != NULL)
		g_pAuctionHouseManager->sendList(pPC);

	GCNPCResponse response;
	response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
	pGamePlayer->sendPacket(&response);

	__END_CATCH
}

string ActionOpenAuctionHouse::toString() const
{
	__BEGIN_TRY
	return "ActionOpenAuctionHouse()";
	__END_CATCH
}
