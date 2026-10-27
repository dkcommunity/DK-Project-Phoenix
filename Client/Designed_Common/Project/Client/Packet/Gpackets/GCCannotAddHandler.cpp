//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCannotAddHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCannotAdd.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "VS_UI.h"

#if OUTPUT_DEBUG
//#if __CONTENTS(__GEAR_SWAP_CHANGE)
	char	m_strLogCodeCannotAdd[500];
//#endif	//__GEAR_SWAP_CHANGE
#endif	// __DEBUG_OUTPUT__


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCCannotAddHandler::execute ( GCCannotAdd * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


	//-----------------------------------------------------------------
	// 처리할려는 item buffer의 상태에 따라서..
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	// Pickup to Inventory
	//-----------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferPickupToInventory())
	{
		DEBUG_ADD("Can't Drop Item to Inventory");
		
		// inventory에 못 넣는 경우..
		// buffer만 제거시키면 된다.
		g_pPlayer->ClearItemCheckBuffer();
	}
	//-----------------------------------------------------------------
	// Pickup to Mouse
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferPickupToMouse())
	{
		DEBUG_ADD("Can't Pickup Item to Mouse");
		
		// buffer만 제거시키면 된다.
		g_pPlayer->ClearItemCheckBuffer();
	}
	//-----------------------------------------------------------------
	// Drop to Zone
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferDropToZone())
	{
		DEBUG_ADD("Can't Drop Item to Zone");
		
		// 다시 item을 mouse에 붙인다.
		MItem* pItem = g_pPlayer->GetItemCheckBuffer();

		UI_PickUpItem( pItem );

		// buffer 제거		
		g_pPlayer->ClearItemCheckBuffer();
	}
	//-----------------------------------------------------------------
	// inventory에서 아이템을 못 분리한다고 검증.
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferPickupSomeFromInventory())
	{
		DEBUG_ADD("Can't PIckupSome From Inventory");

		g_pPlayer->ClearItemCheckBuffer();		
	}
	//-----------------------------------------------------------------
	// Drop to RelicTable
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferDropToRelicTable())
	{
		DEBUG_ADD("Can't Drop to RelicTable");

		g_pPlayer->ClearItemCheckBuffer();		
	}
	//-----------------------------------------------------------------
	// Drop to Creature
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferDropToCreature())
	{
		DEBUG_ADD("Can't Drop to Creature");

		g_pPlayer->ClearItemCheckBuffer();		
	}
	//-----------------------------------------------------------------
	// 다른 경우..
	//-----------------------------------------------------------------
	else
	{

#if __CONTENTS(__GEAR_SWAP_CHANGE)		//	GCCannotAddHandler
		bool	bItemActive	= false;
		int		iValue	= 0;

		iValue	= pPacket->getCannotreason();

		switch(iValue)
		{
		case 0:			// CANNOT_REASON_FAIL
			bItemActive	= false;
			break;

		case 1:			// CANNOT_REASON_NOT_EXIST_ITEM
			bItemActive	= false;
			break;

		case 2:			// CANNOT_REASON_PREMIUM_FEATURE
			bItemActive	= false;
			break;

		case 10:		// CANNOT_REASON_DIFFERENT_GEARSLOTID
			bItemActive	= false;
			break;
		default:
			bItemActive	= true;
			break;
		}

//		MItem* pItem = g_pPlayer->GetItemCheckBuffer();
//		pItem->SetActive(bItemActive);

		/*
		switch(g_pPlayer->GetRace())
		{
		case RACE_SLAYER:
			g_pSlayerGear->SetActive(bItemActive);
			g_pSlayerGear->CheckAffectStatusAll();
			break;
		case RACE_VAMPIRE:
			g_pVampireGear->SetActive(bItemActive);
			g_pVampireGear->CheckAffectStatusAll();
			break;
		case RACE_OUSTERS:
			g_pOustersGear->SetActive(bItemActive);
			g_pOustersGear->CheckAffectStatusAll();
			break;
		}
		*/
#endif // __GEAR_SWAP_CHANGE

		
		DEBUG_ADD("Else.. what?");
		g_pPlayer->ClearItemCheckBuffer();
		
//		gC_vs_ui.Inventory_Item_Position_Remove();
	}


#endif

	__END_CATCH
}
