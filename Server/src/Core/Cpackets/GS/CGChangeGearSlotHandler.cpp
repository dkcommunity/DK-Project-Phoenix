//////////////////////////////////////////////////////////////////////////////
// Filename    : CGChangeGearSlotHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGChangeGearSlot.h"

#ifdef __GAME_SERVER__
	#include "Assert.h"
	#include "GamePlayer.h"
	#include "Slayer.h"
	#include "StringPool.h"

	#include "Gpackets/GCChangeGearSlot.h"
	#include "Gpackets/GCSystemMessage.h"
#endif

void CGChangeGearSlotHandler::execute (CGChangeGearSlot* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	GearSlotID_t GearSlotID = pPacket->getGearSlotID();

	Creature* pCreature = pGamePlayer->getCreature();
	PlayerCreature *pPlayerCreature = dynamic_cast<PlayerCreature *>(pCreature);

	if ( pPlayerCreature->getMaxGearSlotID() < GearSlotID )
	{
		return;
	}

	if ( pPlayerCreature->getCurrentGearSlotID() == GearSlotID )
	{
		return;
	}

	pPlayerCreature->ChangeGearSlotID(GearSlotID);

	GCChangeGearSlot ChangeGearSlot;

	ChangeGearSlot.setGearSlotID(GearSlotID);
	ChangeGearSlot.setGearInfo(pPlayerCreature->getGearInfo());

	pGamePlayer->sendPacket(&ChangeGearSlot);

	/////////////////////////////////////////////////////////////
	//
	GCSystemMessage gcSystemMessage;


	if ( GearSlotID == 0 )
	{
		gcSystemMessage.setMessage( g_pStringPool->c_str( STRID_GEARSLOT_CHANGED_DEFAULTSLOT ) );
	}
	else
	{
		gcSystemMessage.setMessage( g_pStringPool->c_str( STRID_GEARSLOT_CHANGED_SUBSLOT ) );
	}

	pGamePlayer->sendPacket(&gcSystemMessage);
#endif

	__END_DEBUG_EX __END_CATCH
}

