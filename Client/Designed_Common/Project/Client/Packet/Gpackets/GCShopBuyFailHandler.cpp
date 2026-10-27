//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopBuyFailHandler.cpp
// Written By  : 김성민
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopBuyFail.h"
#include "ClientDef.h"
#include "UIDialog.h"
#include "TempInformation.h"
#include "MGameStringTable.h"
#include "UIFunction.h"

void GCShopBuyFailHandler::execute ( GCShopBuyFail * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	// mode를 없앤다.
	g_pTempInformation->SetMode(TempInformation::MODE_NULL);

	// 거래를 다시 활성화한다.
	UI_UnlockItemTrade();

	switch (pPacket->getCode())		//버그 수정시 소스 주석을 풀어주자.	//2008.08.26 shootkj
	{
/*		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_MONEY:	//돈이 모자란다.
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY].GetString() );
			break;

		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_SPACE:	// 자리가 모자란다.
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_SPACE].GetString() );
			break;

		case GC_SHOP_BUY_FAIL_NPC_NOT_EXIST:	// NPC가 존재하지 않는다.
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_NPC_NOT_EXIST].GetString() );
			break;

		case GC_SHOP_BUY_FAIL_NOT_NPC:	// NPC라고 보내온 놈이 NPC가 아니다.
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_NOT_NPC].GetString() );
			break;

		case GC_SHOP_BUY_FAIL_ITEM_NOT_EXIST:	// 지정한 위치에 아이템이 존재하지 않는다.
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_ITEM_NOT_EXIST].GetString() );
			break;
*/
			//pPacket->getAmount()	// 부족한 개수
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_BLACK_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_RED_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_BLUE_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_GREEN_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_CYAN_STAR :
		default :
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_ITEM].GetString() );
	}

#endif

	__END_CATCH
}
