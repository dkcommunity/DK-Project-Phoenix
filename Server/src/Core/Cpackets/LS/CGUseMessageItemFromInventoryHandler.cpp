//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseMessageItemFromInventoryHandler.cpp
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGUseMessageItemFromInventory.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Zone.h"
	#include "PlayerCreature.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "MonsterCorpse.h"
	#include "Inventory.h"
	#include "Item.h"
	#include "ItemInfo.h"
	#include "ItemInfoManager.h"
	#include "ItemUtil.h"
	#include "CreatureUtil.h"
	#include "ZoneUtil.h"
	#include "VariableManager.h"
	#include "CastleInfoManager.h"
	#include "DB.h"
	#include "Store.h"

	#include "Gpackets/GCCannotUse.h"
	#include "Gpackets/GCUseOK.h"
	#include "Gpackets/GCSystemMessage.h"
	#include "ZoneGroupManager.h"
	#include "ZoneInfoManager.h"
	#include "ZonePlayerManager.h"
	#include "GameWorldInfoManager.h"
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseMessageItemFromInventoryHandler::execute(CGUseMessageItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

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

	// ???? ??? ???? ????? ? ??.
	if (InvenX >= pInventory->getWidth() || InvenY >= pInventory->getHeight())
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ????? ? ???? ??? ???.
	Item* pItem = pInventory->getItem(InvenX, InvenY);
	if (pItem == NULL)
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ????? ?? ???? Object? ???.
	ObjectID_t ItemObjectID = pItem->getObjectID();

	// OID? ???? ???, ??? ? ?? ?????? ???.
	if (ItemObjectID != pPacket->getObjectID() || !isUsableItem(pItem, pCreature))
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	if ( pPC->getStore()->getItemIndex(pItem) != 0xff )
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ???? ??? ??, ?? ??? ???? ??.
	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_EVENT_TREE:
			executeEventTree(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_EFFECT_ITEM:
			executeEventFromMessage(pPacket , pPlayer);
			break;
		default:
		{
			//Assert(false);
			// by sigi. 2002.12.25
			filelog("useItemError.txt", "[CGUseMessageItemFromInventory] No Such ItemClassHandler=%s, owner=%s", ItemClass2ShortString[pItem->getItemClass()].c_str(), pCreature->getName().c_str());

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);

		}
		return;
	}

#endif

    __END_DEBUG_EX __END_CATCH
}


void CGUseMessageItemFromInventoryHandler::executeEventTree(CGUseMessageItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ?? ???? ?? ??? ?? ?? ???,
	// ?? ??? ?? ????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ObjectID_t      ItemObjectID = pItem->getObjectID();
	MonsterType_t	MType		 = 0;

	int				time		 = 0;

	switch ( pItem->getItemType() )
	{
		case 12:
			MType = 482;
			time = g_pVariableManager->getVariable( CHRISTMAS_TREE_DECAY_TIME ) / 10;
			break;
		case 26:
			MType = 650;
			time = 21600;
			break;
		case 27:
			MType = 650;
			time = 43200;
			break;
		case 28:
			MType = 650;
			time = 86400;
			break;
		case 54:
			MType = 819;
			time = g_pVariableManager->getVariable( CHRISTMAS_TREE_DECAY_TIME ) / 10;
			break;
		default:
			{
				filelog("EventTree.log", "??? ??? ??. : %s ? %d", pPC->getName().c_str(), pItem->getItemType() );
				GCCannotUse _GCCannotUse;
				_GCCannotUse.setObjectID(pPacket->getObjectID());
				pGamePlayer->sendPacket(&_GCCannotUse);
			}
			return;
	}

	// ??? ??? ?? ???? ?? ? ? ???? ????.
	// ??????? ??? ? ??.
	// ? ? ?? ??? ???? ????. ItemType ? 12? ?? ??? ? ??.
	// ???(???? ??? 5x5?? ??) ?? ??? ??? ??? ? ??.
	if ( !isAbleToUseTileSkill( pCreature )
		|| pZone->isMasterLair()
		|| ItemObjectID != pPacket->getObjectID()
		|| checkCorpse( pZone, MType, pPC->getX() - 2, pPC->getY() - 2, pPC->getX() + 2, pPC->getY() + 2 )
		)
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ??? ?? ???? ? ? ??.
	if ( !pPC->isGOD() )
	{
		if ( pZone->isCastle() )
		{
			if ( !g_pCastleInfoManager->isCastleMember( pZone->getZoneID(), pPC ) )
			{
				GCCannotUse _GCCannotUse;
				_GCCannotUse.setObjectID(pPacket->getObjectID());
				pGamePlayer->sendPacket(&_GCCannotUse);
				return;
			}
		}
		// ?? ?? ?? ?????? ?? ? ??.
		else if (pZone->getZoneLevel(pCreature->getX(), pCreature->getY()) & SAFE_ZONE)
		{
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
	}

/*	// ??? ?? ????. (??? ??? ??? ????)
    MonsterCorpse* pMonsterCorpse = new MonsterCorpse( 482, pPacket->getMessage(), 2 ); 
    Assert(pMonsterCorpse!=NULL); 
 
    pZone->getObjectRegistry().registerObject( pMonsterCorpse ); 
 
    // ??? ??? ?? ????. 
	int delayTime = g_pVariableManager->getVariable( CHRISTMAS_TREE_DECAY_TIME ); // by sigi. 2002.12.17
	TPOINT pt = pZone->addItem( pMonsterCorpse, pPC->getX(), pPC->getY(), true, delayTime );	// 6?? ?? ??(??)? ????.
	if (pt.x == -1)*/
	if ( !createBulletinBoard( pZone, pPC->getX(), pPC->getY(), MType, pPacket->getMessage(), 
				VSDateTime::currentDateTime().addSecs( time ) ) )
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);

		return;
	}

	// ??? ?????? ????.
	pInventory->deleteItem(InvenX, InvenY);
	pItem->destroy();
	SAFE_DELETE( pItem );

	// ???? ????? ?????? ???.
	GCUseOK gcUseOK;
	pGamePlayer->sendPacket( &gcUseOK );

	//pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffectToTile );

#endif
    __END_DEBUG_EX __END_CATCH
}
void CGUseMessageItemFromInventoryHandler::executeEventFromMessage(CGUseMessageItemFromInventory* pPacket, Player* pPlayer)
throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
#ifdef __GAME_SERVER__
		Assert(pPacket != NULL);
	Assert(pPlayer != NULL);
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ObjectID_t      ItemObjectID = pItem->getObjectID();
	MonsterType_t	MType		 = 0;
	int				time		 = 0;
	string color="0";
	switch ( pItem->getItemType() )
	{
	case 87:
		color = "0";
		break;
//	case 11:
//		color = "1";
//		break;
//	case 12:
//		color = "2";
		break;
	default:
		{
			filelog("EventTree.log", "ÙZÌµ?¿Ì??? ?ª«?: %s  Äz??Ö”%d", pPC->getName().c_str(), pItem->getItemType() );
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
		}
		return;
	}
	decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	GCSystemMessage _GCSystemMessage;
	string message = pPacket->getMessage().c_str();
	message += color.c_str();
	_GCSystemMessage.setMessage( message );
	_GCSystemMessage.setType(SYSTEM_MESSAGE_PLAYER);
	g_pZoneGroupManager->broadcast( &_GCSystemMessage );
	GCUseOK gcUseOK;
	pGamePlayer->sendPacket( &gcUseOK );
#endif
	__END_DEBUG_EX __END_CATCH
}
	