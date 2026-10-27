////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionHasEnoughFamilyCoin.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionHasEnoughFamilyCoin.h"
#include "PlayerCreature.h"
#include "Inventory.h"
#include "FlagSet.h"

#include "StringPool.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCNPCResponse.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"

////////////////////////////////////////////////////////////////////////////////
// is satisfied?
////////////////////////////////////////////////////////////////////////////////
bool ConditionHasEnoughFamilyCoin::isSatisfied (Creature * pCreature1 , Creature * pCreature2, void* pParam) const 
{ 
	Assert(pCreature2 != NULL);
	Assert(pCreature2->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	
        Inventory* pInventory = pPC->getInventory();
	Player *pPlayer = pCreature2->getPlayer();
        bool bhaveit = pInventory->hasEnoughNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 32, 1 );

        if ( pInventory == NULL || !bhaveit)
	{
         GCSystemMessage gcSystemMessage;

        gcSystemMessage.setMessage( g_pStringPool->getString( STRID_NOT_ENOUGH_KAMILA_REQUIREDITEM ) );
        pPlayer->sendPacket(&gcSystemMessage);

        GCNPCResponse response;

        response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
        pPlayer->sendPacket( &response );
	return false;
        }

//	return pInventory->hasEnoughNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 32, 1 );


else
        return true;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionHasEnoughFamilyCoin::read (PropertyBuffer & propertyBuffer)
{
}

////////////////////////////////////////////////////////////////////////////////
	// get debug string
////////////////////////////////////////////////////////////////////////////////
string ConditionHasEnoughFamilyCoin::toString () const 
{ 
	__BEGIN_TRY

	StringStream msg;
	msg << "ConditionHasEnoughFamilyCoin("
		<< ")"; 
	return msg.toString();

	__END_CATCH
}
