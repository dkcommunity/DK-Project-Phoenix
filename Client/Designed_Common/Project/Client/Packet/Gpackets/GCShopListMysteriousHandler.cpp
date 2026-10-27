//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopListMysteriousHandler.cpp
// Written By  : 김성민
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopListMysterious.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShopShelf.h"
#include "MPriceManager.h"
#include "UIFunction.h"

void GCShopListMysteriousHandler::execute ( GCShopListMysterious * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	// 정상.. 
	//------------------------------------------------------
	else
	{
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		//------------------------------------------------------
		// 그런 creature가 없는 경우
		//------------------------------------------------------
		if (pCreature==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] There is no such Creature id=%d", pPacket->getObjectID());
		}
		//------------------------------------------------------
		// NPC인 경우
		//------------------------------------------------------
		else if (pCreature->GetClassType()==MCreature::CLASS_NPC)
		{
			// release build mismatch korumasi
			MNPC* pNPC = dynamic_cast<MNPC*>(pCreature);
			if (pNPC == NULL)
			{
				DEBUG_ADD_FORMAT("[Error] dynamic_cast<MNPC*> failed id=%d", pPacket->getObjectID());
				return;
			}

			//------------------------------------------------------
			// 새로운 Shelf를 생성한다.
			//------------------------------------------------------
			int rawShelfType = (int)pPacket->getShopType();
			if (rawShelfType < 0 || rawShelfType >= MShopShelf::MAX_SHELF)
			{
				DEBUG_ADD_FORMAT("[Error] invalid SHELF_TYPE=%d, max=%d", rawShelfType, (int)MShopShelf::MAX_SHELF);
				return;
			}

			MShopShelf* pShelf = MShopShelf::NewShelf( (MShopShelf::SHELF_TYPE)rawShelfType );

			if (pShelf == NULL)
			{
				DEBUG_ADD_FORMAT("[Error] MShopShelf::NewShelf() NULL. type=%d", rawShelfType);
				return;
			}

			pShelf->SetVersion( pPacket->getShopVersion() );

			//------------------------------------------------------
			// 아이템들 추가
			//------------------------------------------------------
			for (int i=0; i<SHOP_RACK_INDEX_MAX; i++)
			{
				const SHOPLISTITEM_MYSTERIOUS& item = pPacket->getShopItem( i );

				if (item.bExist)
				{
					// item 생성
					MItem* pItem = MItem::NewItem( (ITEM_CLASS)item.itemClass );

					if (pItem == NULL)
					{
						DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL. class=%d index=%d", (int)item.itemClass, i);
						continue;
					}

					//pItem->SetID( item.objectID );
					//pItem->SetItemType( item.itemType );
					//pItem->SetItemOption( item.optionType );
					//pItem->SetCurrentDurability( item.durability );

					pItem->UnSetIdentified();

					// Shelf에 item추가
					pShelf->SetItem( i, pItem );
				}
			}

			//------------------------------------------------------
			//
			// NPC의 상점에 shelf를 추가한다.
			//
			//------------------------------------------------------
			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				// 상점이 없었으면 생성한다.
				pShop = new MShop;
				pShop->Init( MShopShelf::MAX_SHELF );

				// NPC에 상점 설정
				pNPC->SetShop( pShop );

				// normal item 선반을 생성한다.
				pNPC->CreateFixedShelf();
				pNPC->CreateFixedShelf(true);	// mystrious
			}

			//------------------------------------------------------
			// default로 normal 상점에 접근하게 한다.
			//------------------------------------------------------
			pShop->SetCurrent( 0 );

			//------------------------------------------------------
			// 선반의 가격 비율 
			//------------------------------------------------------
			g_pPriceManager->SetMarketCondBuy( pPacket->getMarketCondBuy() );
			g_pPriceManager->SetMarketCondSell( pPacket->getMarketCondSell() );			
			
			//------------------------------------------------------
			// 상점에 선반 설정
			//------------------------------------------------------
			pShop->SetShelf( pShelf->GetShelfType(), pShelf );

			//------------------------------------------------------
			// 정상적으로 된 경우
			// --> 상점을 실행한다.
			//------------------------------------------------------
			UI_SetShop( pShop );		// shop 설정
			UI_RunShop();
			UI_SetShop( pShop );		// shop 설정
		}
		//------------------------------------------------------
		// NPC가 아닌 경우
		//------------------------------------------------------
		else
		{
			DEBUG_ADD_FORMAT("[Error] The Creature is Not NPC. id=%d", pPacket->getObjectID());
		}
	}


#endif

	__END_CATCH
}
