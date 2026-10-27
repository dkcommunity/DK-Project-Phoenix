//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRepairHandler.cpp
// Written By  : 김성민
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestRepair.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Zone.h"
	#include "ItemUtil.h"
	#include "PriceManager.h"
	#include "ZoneUtil.h"
	#include "Store.h"

	#include "item/Key.h"
	#include "item/SlayerPortalItem.h"
	#include "item/OustersSummonItem.h"

	#include "Gpackets/GCNPCResponse.h"

	#include <stdio.h>
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::execute (CGRequestRepair* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);
	
	ObjectID_t ITEMOID = pPacket->getObjectID();
	
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	PlayerCreature* pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	
	// 개인상점 중인지 체크 
	Store* pStore = pPC->getStore();
	
	if ( pStore != NULL && pStore->isOpen() )
	{
		return;
	}
	
	double dRepairDiscountRate = 0.0;
	
	// GlobalNPC와 대화중인 경우
	if ( pPC->GetTalkingCallNPC() == NULL ) 
	{
		dRepairDiscountRate = 0.0;
	}
	else
	{
		dRepairDiscountRate = pPC->GetParameter<double>("RepairDiscountRate", 0.0);		
	}

	if (ITEMOID == 0)
	{
		// ObjectID가 0이라면 모든 아이템을 수리하고자 하는 것이다.
		executeAll(pPacket, pPlayer, dRepairDiscountRate);
	}
	else
	{
		Item* pItem = NULL;
		
		pItem = pPC->findItemOID(ITEMOID);

		// 플레이어가 아이템을 가지고 있다면
		if (pItem != NULL)
		{
			// 그 아이템이 모터 사이클 키라면...
			if (pItem->getItemClass() == Item::ITEM_CLASS_KEY && pItem->getItemType() == 2)
			{
				executeMotorcycle(pPacket, pPlayer, dRepairDiscountRate);
				
				return;
			}
			else executeNormal(pPacket, pPlayer, dRepairDiscountRate);
		}
		else
		{
			// 아이템이 없으니, 당연히 수리할 수 없다.
			GCNPCResponse response;
			response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST);
			pPlayer->sendPacket(&response);
		}
	}
	
#endif

	__END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 일반 아이템을 처리한다.
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::executeNormal (CGRequestRepair* pPacket , Player* pPlayer, double dRepairDiscountRate)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	ObjectID_t    ITEMOID       = pPacket->getObjectID();
	GamePlayer *pGamePlayer		= dynamic_cast<GamePlayer*>(pPlayer);	
	PlayerCreature* pPC         = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());

	Gold_t        playerMoney   = 0;
	Price_t       repairPrice   = 0;
	Item*         pItem         = NULL;

	int           storage       = 0;
	int           X             = 0;
	int           Y             = 0;
	GCNPCResponse response;

	if ( !(storage == STORAGE_INVENTORY || storage == STORAGE_GEAR) )
	{
		// 수리 가능한 저장소가 아니다
		
		response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST);
		pPlayer->sendPacket(&response);
		
		return;
	}

	playerMoney = pPC->getGold();
	pItem = pPC->findItemOID(ITEMOID, storage, X, Y);
	
	// 플레이어가 수리하려고 하는 아이템을 가지고 있는지 
	// 상위에서 검사를 하기 때문에, pItem이 널일리는 없다.
	// 단, 수리할 수 없는 아이템인지를 검사한다.
	if (isRepairableItem(pItem) == false)
	{
		response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_TYPE);
		pPlayer->sendPacket(&response);
		return;
	}

	// 이전 내구도를 저장한다.
	Durability_t oldDurability = pItem->getDurability();

	repairPrice = g_pPriceManager->getRepairPrice(pItem, pPC);
	repairPrice = PriceManager::GetDiscountPrice(repairPrice, dRepairDiscountRate);

	if (repairPrice > playerMoney)
	{
		response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
		pPlayer->sendPacket(&response);
		
		return;
	}
	
	if ( repairPrice == 0 )
	{
		// 수리할 필요가 없음
		response.setCode(NPC_RESPONSE_REPAIR_FAIL_NOT_NEED);
		pPlayer->sendPacket(&response);
		
		return;
	}

	// 수리한다.
	repairItem(pItem);

	// 수리한 아이템이 기어창의 아이템이고 이전 내구도가 0 이었다면 정보를 새로 보내줘야한다.
	if ( storage == STORAGE_GEAR && oldDurability == 0 )
	{
		pPC->initAllStatAndSend();
		pPC->sendRealWearingInfo();
	}

	pPC->decreaseGoldEx(repairPrice);

	// OK 패킷을 날려준다.
	response.setCode(NPC_RESPONSE_REPAIR_OK);
	response.setParameter(playerMoney-repairPrice);
	pPlayer->sendPacket(&response);

#endif

	__END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// 모터 사이클을 처리한다.
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::executeMotorcycle (CGRequestRepair* pPacket , Player* pPlayer, double dRepairDiscountRate)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	// 패킷 정보를 뽑아낸다.
	ObjectID_t      ITEMOID     = pPacket->getObjectID();
	GamePlayer *pGamePlayer		= dynamic_cast<GamePlayer*>(pPlayer);	
	PlayerCreature* pPC         = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	Zone*           pZone       = pPC->getZone();
	Gold_t          playerMoney = 0;
	ZoneCoord_t     CenterX     = pPC->getX();
	ZoneCoord_t     CenterY     = pPC->getY();
	Item*           pItem       = NULL;
	
	GCNPCResponse   response;
	
	pItem       = pPC->findItemOID(ITEMOID);
	playerMoney = pPC->getGold();

	// 주위 일정 범위를 검색해서, 모터 사이클이 있는지 확인한다.
	for (ZoneCoord_t zx=CenterX-5; zx<=CenterX+5; zx++)
	{
		for (ZoneCoord_t zy=CenterY-5; zy<=CenterY+5; zy++)
		{
			if (!isValidZoneCoord(pZone, zx, zy)) continue;

			Tile & tile = pZone->getTile(zx, zy);

			if (tile.hasItem())
			{
				Item* pItemOnTile = tile.getItem();
				Assert(pItemOnTile != NULL);

				// 만일 아이템이 타일 위에 있을 경우, 모터 사이클인지 확인한다.
				if (pItemOnTile->getItemClass() == Item::ITEM_CLASS_MOTORCYCLE)
				{
					DWORD    targetID     = dynamic_cast<Key*>(pItem)->getTarget();
					ItemID_t motorcycleID = pItemOnTile->getItemID();

					if (targetID == motorcycleID)
					{
						Price_t repairPrice = g_pPriceManager->getRepairPrice(pItemOnTile, pPC);
						repairPrice = PriceManager::GetDiscountPrice(repairPrice, dRepairDiscountRate);

						if (repairPrice > playerMoney)
						{
							response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
							pPlayer->sendPacket(&response);
							
							return;
						}
						
						if ( repairPrice == 0 )
						{
							// 수리할 필요가 없음
							response.setCode(NPC_RESPONSE_REPAIR_FAIL_NOT_NEED);
							pPlayer->sendPacket(&response);
							
							return;
						}

						// 수리한다.
						repairItem(pItemOnTile);

						pPC->decreaseGoldEx(repairPrice);

						response.setCode(NPC_RESPONSE_REPAIR_OK);
						response.setParameter(playerMoney-repairPrice);
						
						pPlayer->sendPacket(&response);

						return;
					} // if (targetID == 
				} // if (itemclas == MOTORCYCLE
			}
		} // end of for (ZoneCoord_t zy=CenterY-5; zy<=CenterY+5; zy++)
	} // end of for (ZoneCoord_t zx=CenterX-5; zx<=CenterX+5; zx++)

	// FOR 루프를 다 돌고, 이까지 왔다는 것은 근처에 오토바이가 없다는 말이당...
	// 그러므로 모터 사이클 팔기가 실패했다는 것을 알린다.
	response.setCode(NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST);
	pPlayer->sendPacket(&response);
	
#endif

	__END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 모든 아이템 수리하기
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairHandler::executeAll(CGRequestRepair* pPacket , Player* pPlayer, double dRepairDiscountRate)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	GamePlayer *pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);	
	PlayerCreature* pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	
	Price_t repairTotalPrice = 0;
	
	GCNPCResponse response;

	bool bSendRealWearingInfo = false;

	// 모든 아이템을 합한 수리비를 계산한다.
	for (int i=0; i < pPC->getMaxWearSlotNum(); i++)
	{
		Item* pItem = pPC->getWearItem(i);
		if (pItem != NULL)
		{
			if (i == pPC->getWearPartOfRightHand() && isTwohandWeapon(pItem))
			{
				// 오른손이고, 현재 들고 있는 무기가 양손 무기라면...
				// 수리 가격에 포함시킬 필요가 없다.
			}
			else if ( isRepairableItem( pItem ) )
			{
				Price_t repairPrice = 0;
				
				repairPrice = g_pPriceManager->getRepairPrice(pItem, pPC);
				repairPrice = PriceManager::GetDiscountPrice(repairPrice, dRepairDiscountRate);
				
				repairTotalPrice += repairPrice;
			}
		}
	}

	// 돈이 모자라다면 리턴한다.
	if (pPC->getGold() < repairTotalPrice)
	{
		response.setCode(NPC_RESPONSE_REPAIR_FAIL_MONEY);
		pPlayer->sendPacket(&response);
		
		return;
	}
	
	if ( repairTotalPrice == 0 )
	{
		// 수리할 필요가 없음
		response.setCode(NPC_RESPONSE_REPAIR_FAIL_NOT_NEED);
		pPlayer->sendPacket(&response);
		
		return;
	}

//	// 각각의 아이템을 수리하고, DB에 저장한다.
//	char pField[80];

	for (int i=0; i < pPC->getMaxWearSlotNum(); i++)
	{
		Item* pItem = pPC->getWearItem(i);
		if (pItem != NULL)
		{
			if (i == pPC->getWearPartOfRightHand() && isTwohandWeapon(pItem))
			{
				// 오른손이고, 현재 들고 있는 무기가 양손 무기라면...
				// 수리할 필요가 없다.
			}
			else if ( isRepairableItem( pItem ) )
			{
				Durability_t oldDurability = pItem->getDurability();
				repairItem(pItem);

				if ( oldDurability == 0 )
				{
					bSendRealWearingInfo = true;
				}
			}
		}
	}

	pPC->decreaseGoldEx(repairTotalPrice);

	// OK 패킷을 날려준다.
	response.setCode(NPC_RESPONSE_REPAIR_OK);
	response.setParameter(pPC->getGold());
	
	pPlayer->sendPacket(&response);

	if ( bSendRealWearingInfo )
	{
		pPC->initAllStatAndSend();
		pPC->sendRealWearingInfo();
	}

#endif

	__END_DEBUG_EX __END_CATCH
}
