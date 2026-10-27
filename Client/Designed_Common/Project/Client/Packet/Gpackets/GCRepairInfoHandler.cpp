//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRepairInfoHandler.cpp
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRepairInfo.h"
#include "MGameStringTable.h"
#include "CMessageArray.h"
#include "MPriceManager.h"
#include "VS_UI.h"
#include "VS_UI_GameCommon.h"
#include "UIFunction.h"

extern CMessageArray*				g_pSystemMessage;

void GCRepairInfoHandler::execute ( GCRepairInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
	
#ifdef __GAME_CLIENT__
	DWORD objectID = pPacket->GetItemOID();
	DWORD repairPrice = pPacket->GetRepairPrice();
	DWORD repairPriceDC = pPacket->GetRepairDiscountPrice();
	MPriceManager::REPAIRPIRCEPAIR repairPircePair(repairPrice, repairPriceDC);
	

	g_pPriceManager->RepairPriceListInsert(objectID, repairPircePair);

	if(objectID == 0)
	{
		if(repairPriceDC == 0)
		{
			UI_PopupMessage(STRING_MESSAGE_CANNOT_REPAIR);
			UI_UnlockItemTrade();
		}
		else
		{
			gC_vs_ui.RunRepairItemAll(objectID , repairPriceDC);
		}
	}
	

#endif

	 __END_CATCH
}
