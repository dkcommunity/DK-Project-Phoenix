//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRepairInfoHandler.cpp
// Description :
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestRepairInfo.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "ItemUtil.h"
//	#include "ItemInfo.h"
//	#include "ItemInfoManager.h"
	#include "PriceManager.h"
	#include "ZoneUtil.h"
	#include "Zone.h"

	#include "item/Key.h"

	#include "Gpackets/GCRepairInfo.h"

#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairInfoHandler::execute (CGRequestRepairInfo* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);
	
	ObjectID_t ITEMOID = pPacket->getObjectID();

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	PlayerCreature* pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	
	// 교환중인지 체크 필요
	// 개인상점 중인지 체크  필요

	float fRepairDiscountRate = 0.0f;
	
	if ( pPC->GetTalkingCallNPC() == NULL ) 
	{
		fRepairDiscountRate = 0.0f;
	}
	else
	{
		// GlobalNPC와 대화중인 경우
		
		fRepairDiscountRate = 0.5f;
		
		// 수리 NPC 카드 사용시
		//pPC->SetParameter<float>("RepairDiscountRate", 0.2f);
		//pPC->SetParameter<float>("RepairDiscountRate", 0.5f);
		std::string strParamName = "RepairDiscountRate";
		pPC->GetParameter<float>(strParamName, fRepairDiscountRate, 0.0f);		
	}

	
	if (ITEMOID == 0)
	{
		// ObjectID가 0이라면 모든 아이템을 수리하고자 하는 것이다.
		executeAll(pPacket, pPlayer, fRepairDiscountRate);
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
				executeMotorcycle(pPacket, pPlayer, fRepairDiscountRate);
				
				return;
			}
			else 
			{
				executeNormal(pPacket, pPlayer, fRepairDiscountRate);
			}
		}
	}
	
#endif

__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 일반 아이템을 처리한다.
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairInfoHandler::executeNormal (CGRequestRepairInfo* pPacket , Player* pPlayer, float fRepairDiscountRate)
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	ObjectID_t ItemOID = pPacket->getObjectID();
	GamePlayer *pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);	
	PlayerCreature* pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());

	Item* pItem = NULL;

	int storage = 0;
	int X = 0;
	int Y = 0;
	
	pItem = pPC->findItemOID(ItemOID, storage, X, Y);
	
	if ( pItem == NULL )
	{
		return;
	}

	if (isRepairableItem(pItem) == false)
	{
		return;
	}

	Price_t repairPrice = 0;
	Price_t repairDiscountPrice = 0;
	
	repairPrice = g_pPriceManager->getRepairPrice(pItem, pPC);
	repairDiscountPrice = repairPrice - (Price_t)(repairPrice * fRepairDiscountRate);

	GCRepairInfo gcRepairInfo;
	gcRepairInfo.SetItemOID(ItemOID);
	gcRepairInfo.SetRepairPrice(repairPrice);
	gcRepairInfo.SetRepairDiscountPrice(repairDiscountPrice);
	
	pPlayer->sendPacket(&gcRepairInfo);

#endif

__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// 모터 사이클을 처리한다.
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairInfoHandler::executeMotorcycle (CGRequestRepairInfo* pPacket , Player* pPlayer, float fRepairDiscountRate)
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	// 패킷 정보를 뽑아낸다.
	ObjectID_t      ItemOID     = pPacket->getObjectID();
	GamePlayer *pGamePlayer		= dynamic_cast<GamePlayer*>(pPlayer);	
	PlayerCreature* pPC         = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	
	Zone*           pZone       = pPC->getZone();
	ZoneCoord_t     CenterX     = pPC->getX();
	ZoneCoord_t     CenterY     = pPC->getY();
	Item*           pItem       = NULL;
	
	pItem = pPC->findItemOID(ItemOID);

	// 주위 일정 범위를 검색해서, 모터 사이클이 있는지 확인한다.
	for (ZoneCoord_t zx = CenterX-5; zx <= CenterX+5; ++zx)
	{
		for (ZoneCoord_t zy = CenterY-5; zy <= CenterY+5; ++zy)
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
						Price_t repairPrice = 0;
						Price_t repairDiscountPrice = 0;
						
						repairPrice = g_pPriceManager->getRepairPrice(pItemOnTile, pPC);
						repairDiscountPrice = repairPrice - (Price_t)(repairPrice * fRepairDiscountRate);

						GCRepairInfo gcRepairInfo;
						gcRepairInfo.SetItemOID(ItemOID);
						gcRepairInfo.SetRepairPrice(repairPrice);
						gcRepairInfo.SetRepairDiscountPrice(repairDiscountPrice);
						
						pPlayer->sendPacket(&gcRepairInfo);

						return;
					} 
				}
			}
		}
	}
	
#endif
 __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 모든 아이템 수리하기
//////////////////////////////////////////////////////////////////////////////
void CGRequestRepairInfoHandler::executeAll(CGRequestRepairInfo* pPacket , Player* pPlayer, float fRepairDiscountRate)
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer *pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);	
	PlayerCreature* pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	
	Price_t repairTotalPrice = 0;
	Price_t repairTotalDiscountPrice = 0;

	// 모든 아이템을 합한 수리비를 계산한다.
	for (int i=0; i< pPC->getMaxWearSlotNum(); i++)
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
				Price_t repairDiscountPrice = 0;
				
				repairPrice = g_pPriceManager->getRepairPrice(pItem, pPC);
				repairDiscountPrice = repairPrice - (Price_t)(repairPrice * fRepairDiscountRate);
				
				repairTotalPrice += repairPrice;
				repairTotalDiscountPrice += repairDiscountPrice;
			}
		}
	}

	GCRepairInfo gcRepairInfo;
	gcRepairInfo.SetItemOID(0);
	gcRepairInfo.SetRepairPrice(repairTotalPrice);
	gcRepairInfo.SetRepairDiscountPrice(repairTotalDiscountPrice);
	
	pPlayer->sendPacket(&gcRepairInfo);


#endif

__END_CATCH
}
