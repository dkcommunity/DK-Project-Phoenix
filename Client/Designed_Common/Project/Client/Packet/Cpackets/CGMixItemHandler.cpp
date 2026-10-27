#ifndef __GAME_CLIENT__
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMixItemHandler.cpp
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGMixItem.h"

#ifdef __GAME_SERVER__
#include "GamePlayer.h"
#include "Inventory.h"
#include "PlayerCreature.h"
#include "Item.h"
#include "ItemUtil.h"
#include "Gpackets/GCCannotUse.h"
void sendCannotUse(CGMixItem* pPacket, Player* pPlayer);
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGMixItemHandler::execute(CGMixItem* pPacket, Player* pPlayer)
	
{
	__BEGIN_TRY 

#ifdef __GAME_SERVER__

	////std::cout << "CGMixItemHandler " << std::endl;
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Creature* pCreature = pGamePlayer->getCreature();
	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	Inventory* pInventory = pPC->getInventory();
	Zone*      pZone      = pPC->getZone();

	Assert(pInventory != NULL);
	Assert(pZone != NULL);

	CoordInven_t InvenX = pPacket->getX();
	CoordInven_t InvenY = pPacket->getY();

	//std::cout << "íŒ¨í‚·ë‚ ë¼ì˜´ : " << pPacket->toString() << std::endl;

	// ì¸ë²¤í† ë¦¬ ì¢Œí‘œë¥¼ ë„˜ì–´ê°€ëŠ” ì˜ì—­ì´ë¼ë©´ ì•ˆ ëœë‹¤.
	if (InvenX >= pInventory->getWidth() || InvenY >= pInventory->getHeight())
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ì¸ë²¤í† ë¦¬ì— ê·¸ ì•„ì´í…œì´ ì—†ë‹¤ë©´ ì—ëŸ¬ë‹¤.
	Item* pItem = pInventory->getItem(InvenX, InvenY);
	if (pItem == NULL)
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ì¸ë²¤í† ë¦¬ì— ìžˆëŠ” ì•„ì´í…œì˜ Objectë¥¼ ë°›ëŠ”ë‹¤.
	ObjectID_t ItemObjectID = pItem->getObjectID();

	// OIDê°€ ì¼ì¹˜í•˜ì§€ ì•Šê±°ë‚˜, ì‚¬ìš©í•  ìˆ˜ ì—†ëŠ” ì•„ì´í…œì´ë¼ë©´ ì—ëŸ¬ë‹¤.
	if (ItemObjectID != pPacket->getObjectID() || !isUsableItem(pItem, pCreature))
	{
		//std::cout << "ì•„í…œ ì‚¬ìš© ë¶ˆê°€. ì˜µì íŠ¸ ì•„ë””ê°€ ì•ˆ ë§žë˜ê°€..." << std::endl;
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	Item *pTargetItem1 = NULL;
	Item *pTargetItem2 = NULL;

	CoordInven_t dummy;

	pTargetItem1 = pInventory->findItemOID( pPacket->getTargetObjectID(0), dummy, dummy );
	pTargetItem2 = pInventory->findItemOID( pPacket->getTargetObjectID(1), dummy, dummy );

	if ( pTargetItem1 == NULL || pTargetItem2 == NULL )
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

#endif

    __END_CATCH
}


#endif
