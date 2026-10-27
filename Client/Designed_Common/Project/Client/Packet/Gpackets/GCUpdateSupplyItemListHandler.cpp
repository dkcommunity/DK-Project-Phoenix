#if 0
////////////////////////////////////////////////////////////////////////////////
// Filename    : GCUpdateSupplyItemListHandler.cpp
// Written By  : rappi76
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCUpdateSupplyItemList.h"

#ifdef __GAME_CLIENT__

	#include "VS_UI.h"
	#include "MPremiumGiveItemInfo.h"
	#include "MTimeItemManager.h"

#endif

void GCUpdateSupplyItemListHandler::execute ( GCUpdateSupplyItemList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY //__BEGIN_DEBUG_EX
	__BEGIN_DEBUG
	
#ifdef __GAME_CLIENT__
	// std::cout << pPacket->toString() << std::endl;
	
	g_pPremiumGiveItemInfoList->Give_Item_List_ClearAll();
	
	SupplyItemInfo* pSupplyItemInfo = NULL;
	while(pSupplyItemInfo = pPacket->popSupplyItemInfo())
	{
		MItem *pItem = MItem::NewItem( (ITEM_CLASS)pSupplyItemInfo->itemClass );
		pItem->SetItemType(pSupplyItemInfo->itemType);
		pItem->SetItemOptionList(pSupplyItemInfo->optionType);
		pItem->SetGrade(pSupplyItemInfo->grade); // ì‹œê°„ì œ ì•„ì´í…œ í™©ìƒ‰ì²˜ë¦¬ í•˜ê¸°
		pItem->SetNumber(pSupplyItemInfo->num);
		pItem->SetTimeLimitStatic(pSupplyItemInfo->timeLimit);
		pItem->SetCurrentDurability(pItem->GetMaxDurability());

		MPremiumGiveItemInfo *pPremiumGiveItemInfo = new MPremiumGiveItemInfo(pItem,pSupplyItemInfo->ID,pSupplyItemInfo->timeLimit);
		g_pPremiumGiveItemInfoList->AddItem(pPremiumGiveItemInfo);

		DeleteNew(pSupplyItemInfo);
	}
	
	gC_vs_ui.Request_PremiumGiveItem();
#endif
	
	__END_DEBUG
//	__END_DEBUG_EX
	__END_CATCH
}

#endif
