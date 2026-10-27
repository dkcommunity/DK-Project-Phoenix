//--------------------------------------------------------------------------------
//
// Filename    : GCCreateItemHandler.cpp
// Written By  : elca
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCCreateItem.h"
#include "ClientDef.h"
#include "MInventory.h"
#include "UIFunction.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI.h"
extern int g_C2G;

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCCreateItemHandler::execute ( GCCreateItem * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
#if __CONTENTS(__PCITEMINFO)
	PCItemInfo* pPCItemInfo = pPacket->GetPCItemInfo();
#else //__PCITEMINFO
	GCCreateItem* pPCItemInfo =  pPacket;
#endif //__PCITEMINFO

	MItem* pItem;
	MSubInventory* pSubInventory = NULL;
	bool bSubInventoryUse = false;

	
	//---------------------------------------------------------------------
	// inventory에서 item몇개를 드는 경우..
	//---------------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferPickupSomeFromInventory())
	{
		MItem* pItem = g_pPlayer->GetItemCheckBuffer();
		g_pPlayer->ClearItemCheckBuffer();		

		if (pItem!=NULL)
		{
			// 들어야 하는 item의 개수는
			// ( 원래 개수 - 바닥에 남은 개수 ) 이다.
			int numPickup = pItem->GetNumber() - pPCItemInfo->getItemNum();
			
			// inventory에서 제거한다.
			//pSubInventory = (MSubInventory*)gC_vs_ui.GetSubInventoryItem();

			MItem* pRemovedItem;
			if(pPacket->getInventoryItemObjectID())
			{
				pSubInventory = dynamic_cast<MSubInventory*>(pItem->GetParentInventory());
				pRemovedItem = pSubInventory->RemoveItem( pItem->GetGridX(), pItem->GetGridY() );
				bSubInventoryUse = true;
			}
			else
			{
				pRemovedItem = g_pInventory->RemoveItem( pItem->GetGridX(), pItem->GetGridY() );
				bSubInventoryUse = false;
			}
//			MItem* pRemovedItem = g_pInventory->RemoveItem( pItem->GetGridX(), pItem->GetGridY() );

			if (pRemovedItem==pItem)
			{
				// 개수를 다시 설정해준다.
				pItem->SetNumber( numPickup );

				// mouse에 든다.
				UI_PickUpItem( pItem );
			}
			else
			{
				DEBUG_ADD("[Error] PickupPart item Wrong");
			}
		}
		else
		{
			DEBUG_ADD("[Error] PickupPart item NULL");
		}
	}

	//---------------------------------------------------------------------
	// Item을 생성한다.
	//---------------------------------------------------------------------
	// FAZ 4 (2026-05-12): packet handler sweep — Factory NULL guard.
	// Gecersiz ITEM_CLASS gelirse NewItem NULL doner, SetItemType crash.
	pItem = MItem::NewItem( (enum ITEM_CLASS)pPCItemInfo->getItemClass() );
	if (pItem == NULL)
	{
		DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL for class=%d, type=%d", (int)pPCItemInfo->getItemClass(), (int)pPCItemInfo->getItemType());
		return;
	}
	pItem->SetItemType( pPCItemInfo->getItemType() );
	pItem->SetItemOptionList( pPCItemInfo->getOptionType() );
#if __CONTENTS(__PET_MIXINGFORGE)
	pItem->SetPetMixingOptionList(pPCItemInfo->getMixOptionType());
#endif //__PET_MIXINGFORGE

	pItem->SetID( pPCItemInfo->getObjectID() );		

	pItem->SetCurrentDurability( pPCItemInfo->getDurability() );

	//pItem->SetNumber( pPacket->getItemNum() );

	pItem->SetGrade( pPCItemInfo->getGrade() );
	pItem->SetEnchantLevel( pPCItemInfo->getEnchantLevel() );
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	pItem->SetCashItem(pPCItemInfo->getCashItem());
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
//20071009 by diesirace 서드아이템옵션 추가 (서드 옵션 달린 아이템은 만들어질리가 절대 없단다...)
#if __CONTENTS(__PCITEMINFO)
	pItem->SetItemThirdOptionList(pPCItemInfo->getThirdOptionType() );
	pItem->SetItemThirdOptionType(pPCItemInfo->getThirdEnchantType());
#endif //__PCITEMINFO
	//------------------------------------------
	// 개수
	//------------------------------------------
	// 총인 경우
	//------------------------------------------
	if (pItem->IsGunItem())
	{
		// FAZ 4 (2026-05-12): packet handler sweep — Factory NULL guard.
		// MMagazine olusturulamazsa pItem leak + sonraki SetID/SetMagazine crash.
		MMagazine* pMagazine = (MMagazine*)MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );
		if (pMagazine == NULL)
		{
			DEBUG_ADD("[Error] MItem::NewItem(ITEM_CLASS_MAGAZINE) NULL");
			delete pItem;
			return;
		}

		// 의미 없음 - -;
		pMagazine->SetID( 0 );

		// 이거는 총에 맞춰서 해줘야된다.
		for (int j=0; j<(*g_pItemTable)[ITEM_CLASS_MAGAZINE].GetSize(); j++)			
		{
			pMagazine->SetItemType(	j );

			if (pMagazine->IsInsertToItem( pItem ))
			{
				break;
			}
		}

		// 의미 없음
		pMagazine->ClearItemOption();
	
		// 탄창 개수
		pMagazine->SetNumber( pPCItemInfo->getItemNum() );

		//------------------------------------
		// 탄창 설정
		//------------------------------------
		MGunItem* pGunItem = (MGunItem*)pItem;
		pGunItem->SetMagazine( pMagazine );
	}		
	//------------------------------------------
	// 총이 아닌 경우
	//------------------------------------------
	//20080220 충전 아이템은 충전량을 인첸트 레벨로 받는다.
	else if(pItem->IsChargeItem())
		pItem->SetNumber( pPCItemInfo->getEnchantLevel() );
	else
	{
		pItem->SetNumber( pPCItemInfo->getItemNum() );
		//pItem->SetSilver( item.silver );
	}
	//20081219 트리류의 사이즈를 변경 할수 있게 한다. 
	int TreeSizeX = 3,TreeSizeY = 4;
#if __CONTENTS(__CHRISTMAS_SOCKS_EVENT)	
	if(pItem->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX
		&& pItem->GetNumber() == 1	&& (pItem->GetItemType() == 39))
	{
		TreeSizeY = 3;
	}
#endif //__CHRISTMAS_SOCKS_EVENT


	// 크리스마스 트리 합체
	if(pItem->GetItemClass() == ITEM_CLASS_EVENT_TREE 
#if __CONTENTS(__EVENTTREE_PILE)
		&& pItem->GetNumber() == 1
#endif //__EVENTTREE_PILE
		&&
	   (pItem->GetItemType() == 12 ||
		pItem->GetItemType() == 25 ||
		pItem->GetItemType() == 41 ||
		pItem->GetItemType() == 54		// 터프한 눈사람 2006.11.30 by chyaya
#if __CONTENTS(__BLITZ_COUPON_EVENT)
		|| pItem->GetItemType() == 67	// 블리츠 문양 2008.8.29 by ij-ch
#endif //__BLITZ_COUPON_EVENT
#if __CONTENTS(__2008_AUTUMN_LEAVES_EVENT)
		|| pItem->GetItemType() == 80	// 낙엽 문양 2008.8.29 by ij-ch
#endif //__2008_AUTUMN_LEAVES_EVENT
		)
#if __CONTENTS(__CHRISTMAS_SOCKS_EVENT)
		|| pItem->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX
		&& pItem->GetNumber() == 1	&&
		(
		pItem->GetItemType() == 39
		)
#endif //__CHRISTMAS_SOCKS_EVENT
		)
	{
		if(g_pPlayer->GetItemCheckBufferStatus() == MPlayer::ITEM_CHECK_BUFFER_TREE_MERGE)
		{
			for(int y = 0; y < TreeSizeY; y++)
			{
				for(int x = 0; x < TreeSizeX; x++)
				{
					MItem *pPartOfTree = g_pInventory->GetItem(x+pPacket->getInvenX(), y+pPacket->getInvenY());
					if(pPartOfTree != NULL && 
						pPartOfTree->GetItemClass() == ITEM_CLASS_EVENT_TREE 
#if __CONTENTS(__EVENTTREE_PILE)
						&& pPartOfTree->GetNumber() == 1
#endif //__EVENTTREE_PILE
						&&
						(pPartOfTree->GetItemType() == y*TreeSizeX+x		||
						 pPartOfTree->GetItemType() == y*TreeSizeX+x + 13	||
						 pPartOfTree->GetItemType() == y*TreeSizeX+x + 29	||
						 pPartOfTree->GetItemType() == y*TreeSizeX+x + 42
#if __CONTENTS(__BLITZ_COUPON_EVENT)
						|| pPartOfTree->GetItemType() == y*TreeSizeX+x + 55
#endif //__BLITZ_COUPON_EVENT
#if __CONTENTS(__2008_AUTUMN_LEAVES_EVENT)
						|| pPartOfTree->GetItemType() == y*TreeSizeX+x + 68
#endif //__2008_AUTUMN_LEAVES_EVENT
#if __CONTENTS(__CHRISTMAS_SOCKS_EVENT)
						|| pPartOfTree->GetItemType() == y*TreeSizeX+x + 81
#endif //__CHRISTMAS_SOCKS_EVENT
						))
					{
						g_pInventory->RemoveItem(x+pPacket->getInvenX(), y+pPacket->getInvenY());
						delete pPartOfTree;
					}
				}
			}
			g_pPlayer->ClearItemCheckBuffer();
		}
		else
		{
			DEBUG_ADD("[GCCreateItemHandler] ItemCheckBuffer != ITEM_CHECK_BUFFER_TREE_MERGE");
		}
	}

	//---------------------------------------------------------------------
	// inventory에 추가한다.
	//---------------------------------------------------------------------

	if(bSubInventoryUse)
	{
		pSubInventory->AddItem( pItem, pPacket->getInvenX(), pPacket->getInvenY());
	}
	else
	{
		
		if (g_pInventory->AddItem( pItem, pPacket->getInvenX(), pPacket->getInvenY() ))
		{
			if( g_C2G > 0 )
			{
				C_VS_UI_INVENTORY::AutoMove( pPacket->getInvenX(), pPacket->getInvenY() );
				gpC_base->DispatchMessage();
				gpC_base->DispatchMessage();
				gpC_base->DispatchMessage();
				gpC_base->DispatchMessage();
				gpC_base->DispatchMessage();
				gpC_base->DispatchMessage();
				g_C2G--;
			}
			// 제대로 추가된 경우..
		}
		
		else
		{
			DEBUG_ADD_FORMAT("[Error] Cannot Create Item to inventory: id=%d, tp=%d, xy=(%d,%d)", (int)pItem->GetID(), (int)pItem->GetItemType(), (int)pPacket->getInvenX(), (int)pPacket->getInvenY());
			
			delete pItem;
		}
	}


#endif

	__END_CATCH
}
