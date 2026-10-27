//////////////////////////////////////////////////////////////////////////////
// ActionQuitMobaQueue.cpp
//////////////////////////////////////////////////////////////////////////////

#include "ActionQuitMobaQueue.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "DynamicZoneMoba.h"
#include "Gpackets/GCNPCResponse.h"

void ActionQuitMobaQueue::read(PropertyBuffer& propertyBuffer)
{
	__BEGIN_TRY
	__END_CATCH
}

void ActionQuitMobaQueue::execute(Creature* pNPC, Creature* pCreature)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert(pGamePlayer != NULL);

	DynamicZoneMoba::CancelQueue(pPC);

	GCNPCResponse response;
	response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
	pGamePlayer->sendPacket(&response);

	__END_CATCH
}

string ActionQuitMobaQueue::toString() const
{
	__BEGIN_TRY
	return "ActionQuitMobaQueue()";
	__END_CATCH
}
