//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSubInventoryInfoHandler.cpp
// Written By  : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSubInventoryInfo.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "MInventory.h"
#include "MItemFinder.h"

extern void SetSubInventoryInfo(MItem* pSubInventory, InventoryInfo* pInventoryInfo);
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void GCSubInventoryInfoHandler::execute ( GCSubInventoryInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY// __BEGIN_DEBUG_EX
	
#ifdef __GAME_CLIENT__

	if(NULL == g_pZone || NULL == g_pPlayer)
		return ;
/*
	MItem* pItem = g_pPlayer->GetItemCheckBuffer();
	if( pItem != NULL )
		UI_RunSubInventory(pItem);

	SetSubInventoryInfo(pPacket->getInventoryInfo());

	g_pPlayer->ClearItemCheckBuffer();*/

//	MItem* pItem = g_pPlayer->GetItemCheckBuffer();
//	if(NULL == pItem)
//		return;

	g_pPlayer->ClearItemCheckBuffer();

	TYPE_OBJECTID subInventoryID = pPacket->getObjectID();

	MItem* pSubInventory = g_pInventory->MItemManager::FindItem(MItemObjectFinder(subInventoryID));

	if(pSubInventory)
	{
		UI_RunSubInventory(pSubInventory);
		SetSubInventoryInfo(pSubInventory, pPacket->getInventoryInfo());
	}
#endif

//	__END_DEBUG_EX 
	__END_CATCH
}
