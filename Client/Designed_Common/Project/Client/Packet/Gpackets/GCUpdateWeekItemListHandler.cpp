////////////////////////////////////////////////////////////////////////////////
// Filename    : GCUpdateWeekItemListHandler.cpp
// Written By  : rappi76
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCUpdateWeekItemList.h"

#ifdef __GAME_CLIENT__

	#include "VS_UI.h"
	#include "MPremiumGiveItemInfo.h"
	#include "MTimeItemManager.h"
	#include "DebugInfo.h"

#endif

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
void GCUpdateWeekItemListHandler::execute ( GCUpdateWeekItemList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY //__BEGIN_DEBUG_EX
	__BEGIN_DEBUG
	
#ifdef __GAME_CLIENT__
	// std::cout << pPacket->toString() << std::endl;
	
	g_pPremiumGiveItemInfoList->Give_Item_List_ClearAll();
	
	WeekItemInfo* pWeekItemInfo = NULL;
	while(pWeekItemInfo = pPacket->popWeekItemInfo())
	{
		MItem *pItem = MItem::NewItem( (ITEM_CLASS)pWeekItemInfo->itemClass );
		if (pItem == NULL)
		{
			DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL. class=%d", (int)pWeekItemInfo->itemClass);
			DeleteNew(pWeekItemInfo);
			continue;
		}
		pItem->SetItemType(pWeekItemInfo->itemType);
		pItem->SetItemOptionList(pWeekItemInfo->optionType);
		pItem->SetGrade(pWeekItemInfo->grade); // 시간제 아이템 황색처리 하기
		pItem->SetNumber(pWeekItemInfo->num);
		pItem->SetTimeLimitStatic(pWeekItemInfo->timeLimit);
		pItem->SetCurrentDurability(pWeekItemInfo->Durability);

		MPremiumGiveItemInfo *pPremiumGiveItemInfo = new MPremiumGiveItemInfo(pItem,pWeekItemInfo->ID,pWeekItemInfo->timeLimit);
		g_pPremiumGiveItemInfoList->AddItem(pPremiumGiveItemInfo);

		DeleteNew(pWeekItemInfo);
	}
	
	gC_vs_ui.Request_PremiumGiveItem();
#endif

	__END_DEBUG //__END_DEBUG_EX
	__END_CATCH
}
#endif //__PREMIUM_GIVE_ITEM_UI
