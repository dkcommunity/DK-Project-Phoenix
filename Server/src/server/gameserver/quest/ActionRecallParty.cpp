#include "ActionRecallParty.h"
#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "GamePlayer.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "DB.h"
#include "LogClient.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCStatusCurrentHP.h"

#include "EffectAftermath.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRecallParty::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRecallParty::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());
	
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	if ( pCreature2->isDead() || pCreature2->isFlag( Effect::EFFECT_CLASS_COMA ) )
	{
		return;
	}

	if ( !pInventory->hasEnoughNumItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM,67, 1) )
	{
		GCNPCResponse response;
		response.setCode(NPC_RESPONSE_NOT_ENOUGH_ITEM);
		pPlayer->sendPacket(&response);
		
		return ;
	}
	else
	{
		pInventory->decreaseNumItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM, 67, 1, pPlayer);


		GlobalPartyManager* pLPM = NULL;

		int PartyID = 0;
		int X = pCreature2->getX();
		int Y = pCreature2->getY();

		PartyID = pCreature2->getPartyID();
	
		if ( PartyID != 0 )
		{
			Party* pParty = g_pGlobalPartyManager->getParty(PartyID);
		
			map<string, Creature*> MemberMap = pParty->getMemberMap();
			map<string, Creature*>::iterator MemberMapIter;
		
			Creature *pMemberCreature;

			MemberMapIter = MemberMap.begin();
			for( ; MemberMapIter != MemberMap.end() ; ++MemberMapIter)
			{
				pMemberCreature = MemberMapIter->second;
				if ( pMemberCreature != NULL && pMemberCreature != pCreature2)
				{
					transportCreature(pMemberCreature, pCreature2->getZoneID(), X, Y, true);
				}
			}
		}	
		

	}
	
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
string ActionRecallParty::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionRecallParty("
	    << ")";
	return msg.toString();

	__END_CATCH
}
