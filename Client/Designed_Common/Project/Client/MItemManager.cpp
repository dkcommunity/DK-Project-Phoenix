//----------------------------------------------------------------------
// MItemManager.h
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItemManager.h"
#include "MItem.h"
#include "MItemFinder.h"
#include "MQuickSlot.h"
#include "MPlayer.h"

#ifdef __GAME_CLIENT__
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//initinfo.cpp

	#include "MSlayerGear.h"
	#include "MVampireGear.h"
	#include "MOustersGear.h"
	#include "VS_UI_MOUSE_POINTER.h"

#endif //__GEAR_SWAP_CHANGE
#endif //__GAME_CLIENT__
//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MItemManager::MItemManager()
{
}

MItemManager::~MItemManager()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MItemManager::Release()
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	// 아직 map에 Item이 남아 있는 경우..
	// memory에서 제거시킨다.
	while (iItem != m_mapItem.end())
	{
		MItem* pItem = (*iItem).second;

		delete pItem;
#ifdef __GAME_CLIENT__
		if(pItem == g_pArmsBand1)
		{
			g_pArmsBand1 = NULL;
		}
		else if(pItem == g_pArmsBand2)
		{
			g_pArmsBand2 = NULL;
		}
		else if(pItem == g_pQuickSlot)
		{
			g_pQuickSlot = NULL;
		}
#endif
		iItem++;
	}

	m_mapItem.clear();
}

//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------
// mapItem에 pointer만 저장시켜 둔다.
//----------------------------------------------------------------------
bool		
MItemManager::AddItem(MItem* pItem)
{
	ITEM_MAP::iterator	iItem;

	iItem = m_mapItem.find(pItem->GetID());
	
	//-----------------------------------------------
	// 아직 없는 Item이면 추가	
	//-----------------------------------------------
	if (iItem == m_mapItem.end())
	{
		m_mapItem.insert(ITEM_MAP::value_type(pItem->GetID(), pItem));

		// 착용 여부 체크
		CheckAffectStatus( pItem );	
		return true;
	}

	//-----------------------------------------------
	// 이미 있는 Item이면 false
	//-----------------------------------------------
	return false;
}

//----------------------------------------------------------------------
// Get Item
//----------------------------------------------------------------------
// Item의 pointer만 넘겨준다.
//----------------------------------------------------------------------
MItem*	
MItemManager::GetItemToModify(TYPE_OBJECTID id)
{
	ITEM_MAP::iterator	iItem;

	//--------------------------------------------------
	// ID가 id인 Item를 찾는다.
	//--------------------------------------------------
	iItem = m_mapItem.find(id);

	//--------------------------------------------------
	// 없을 경우 NULL을 return한다.
	//--------------------------------------------------
	if (iItem == m_mapItem.end()) 
	{
		return NULL;
	}

	//--------------------------------------------------
	// 있으면 그 Item를 return한다.
	//--------------------------------------------------
	return (*iItem).second;
}

//----------------------------------------------------------------------
// Get Item (const)
//----------------------------------------------------------------------
// Item의 pointer만 넘겨준다.
//----------------------------------------------------------------------
MItem*	
MItemManager::GetItem(TYPE_OBJECTID id) const
{
	ITEM_MAP::const_iterator	iItem;

	//--------------------------------------------------
	// ID가 id인 Item를 찾는다.
	//--------------------------------------------------
	iItem = m_mapItem.find(id);

	//--------------------------------------------------
	// 없을 경우 NULL을 return한다.
	//--------------------------------------------------
	if (iItem == m_mapItem.end()) 
	{
		return NULL;
	}

	//--------------------------------------------------
	// 있으면 그 Item를 return한다.
	//--------------------------------------------------
	return (*iItem).second;
}

	

//----------------------------------------------------------------------
// Remove Item
//----------------------------------------------------------------------
// mapItem에서 제거하고 Item의 pointer를 넘겨준다.
//----------------------------------------------------------------------
MItem*	
MItemManager::RemoveItem(TYPE_OBJECTID id)
{
	ITEM_MAP::iterator	iItem;

	//--------------------------------------------------
	// ID가 id인 Item를 찾는다.
	//--------------------------------------------------
	iItem = m_mapItem.find(id);
    
	//--------------------------------------------------
	// 그런 id를 가진 Item이 없는 경우
	//--------------------------------------------------
	if (iItem == m_mapItem.end())
	{
		return NULL;
	}

	//--------------------------------------------------
	// 찾은 경우 --> 제거	
	//--------------------------------------------------
	MItem* pItem = (*iItem).second;
	
	// map에서 제거
	m_mapItem.erase( iItem );

	//--------------------------------------------------
	// 모든 다른 item들의 수치 체크
	//--------------------------------------------------
	CheckAffectStatusAll();

	return pItem;
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------
// 사용가능한지 체크
//----------------------------------------------------------------------
#ifdef __GAME_CLIENT__
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//MItemManager::CheckAffectStatusAllGear
void			
MItemManager::CheckAffectStatusAllGear()				// 모든 아이템
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	//------------------------------------------------------
	// 모든 item마다 사용가능한지 아닌지를 체크한다.
	//------------------------------------------------------

	bool	bGetActive;

	switch(g_pPlayer->GetRace())
	{
	case RACE_SLAYER:
		bGetActive	= g_pSlayerGear->GetActive();
		break;

	case RACE_VAMPIRE:
		bGetActive	= g_pVampireGear->GetActive();
		break;
				
	case RACE_OUSTERS:
		bGetActive	= g_pOustersGear->GetActive();
		break;
	}

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = (*iItem).second;
		
//		if(bGetActive == false || !pItem->GetActive())
		if(bGetActive == false)
		{		
			pItem->UnSetAffectStatus();
		}
		else
		{
			CheckAffectStatus( pItem );			// MCreature::CheckAffectStatus의 함수를 호출 하게 된다.
		}

		iItem++;
	}
}	
#endif	// __GEAR_SWAP_CHANGE
#endif	// __INIT_INFO__

#ifdef __GAME_CLIENT__
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//MItemManager::CheckAffectStatusPickupItem()
void			
MItemManager::CheckAffectStatusPickupItem()			// 들고 있는 아이템
{
	MItem* pItem = gpC_mouse_pointer->GetPickUpItem();

	if(pItem == NULL)
		return;

	pItem->SetAffectStatus();
	switch(g_pPlayer->GetRace())
	{
		case RACE_SLAYER:
			if(!g_pSlayerGear->GetActive())
				pItem->UnSetAffectStatus();
			break;
		case RACE_VAMPIRE:
			if(!g_pVampireGear->GetActive())
				pItem->UnSetAffectStatus();
			break;
		case RACE_OUSTERS:
			if(!g_pOustersGear->GetActive())
				pItem->UnSetAffectStatus();
			break;
	}
}
#endif	// __GEAR_SWAP_CHANGE
#endif	// __GAME_CLIENT__

void			
MItemManager::CheckAffectStatusAll()				// 모든 아이템
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();
	
	//------------------------------------------------------
	// 모든 item마다 사용가능한지 아닌지를 체크한다.
	//------------------------------------------------------

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = (*iItem).second;
		CheckAffectStatus( pItem );			// MCreature::CheckAffectStatus의 함수를 호출 하게 된다.
		iItem++;
	}
}


//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------
void			
MItemManager::CheckAffectStatus(MItem* pItem)
{
	// pure virtual로 할려다가.. 그냥 쓸 경우도 있어서..
}

//----------------------------------------------------------------------
// Transfer Item To (id, pItemManager)
//----------------------------------------------------------------------
// this의 ID가 id인 MItem을 제거해서
// pItemManager에 추가시킨다.
//----------------------------------------------------------------------
/*
bool		
MItemManager::TransferItemTo(TYPE_OBJECTID id, MItemManager& targetItemManager)
{
	MItem* pItem = RemoveItem(id);

	// NULL이 아닌 경우..
	if (pItem == NULL)
	{
		return false;
	}
	
	return targetItemManager.AddItem( pItem );
}
*/

//----------------------------------------------------------------------
// 아이템 찾기
//----------------------------------------------------------------------
MItem*
MItemManager::FindItem( MItemFinder& itemFinder ) const
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = iItem->second;
		
		if (itemFinder( pItem ))
		{
			return pItem;
		}
//		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
//		{
//			MItem* pTempItem = ((MSubInventory*)pItem)->FindItemGridOrder( itemFinder );
//			if(NULL != pTempItem)
//				return pTempItem;
//		}
		

		iItem++;
	}

	return NULL;
}

//----------------------------------------------------------------------
// FindItemOrderByIndex
//----------------------------------------------------------------------
// Inventory내의 배치 순서를 고려(가장 앞 - Left-->Right/Top-->Bottom - 기준)
//	하여, 아이템을 찾아 준다. 즉, 두개 이상이 있을 경우 가장 앞의 아이템을 반환한다.
//----------------------------------------------------------------------
MItem*
MItemManager::FindItemOrderByIndex( MItemFinder& itemFinder ) const
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	MItem* pOrderItem = NULL;
	MItem* pItem = NULL;

	while (iItem != m_mapItem.end())
	{
		pItem = iItem->second;
		
		if (itemFinder( pItem ))
		{
			if(!pOrderItem)
				pOrderItem = pItem;
			else if (pItem->GetGridY() < pOrderItem->GetGridY())
				pOrderItem = pItem;
			else if (pItem->GetGridY() == pOrderItem->GetGridY())
				if (pItem->GetGridX() < pOrderItem->GetGridX())
					pOrderItem = pItem;
		}
		iItem++;
	}

	return pOrderItem;
}

//----------------------------------------------------------------------
// FindItemAllOrderByIndex 
//----------------------------------------------------------------------
// 서브 인벤 까지 샅샅이 뒤진다..ㅋㅋ
//----------------------------------------------------------------------
MItem*			
MItemManager::FindItemAllOrderByIndex( MItemFinder& itemFinder, MItem*& pSubInventoryItem) const
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	MItem* pOrderItem = NULL;
	MItem* pItem = NULL;

	while (iItem != m_mapItem.end())
	{
		pItem = iItem->second;
		
		if (itemFinder( pItem ))
		{
			if(!pOrderItem)
				pOrderItem = pItem;
			else if (pItem->GetGridY() < pOrderItem->GetGridY())
				pOrderItem = pItem;
			else if (pItem->GetGridY() == pOrderItem->GetGridY())
				if (pItem->GetGridX() < pOrderItem->GetGridX())
					pOrderItem = pItem;
		}
		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
		{
			MItem* pTempItem = ((MSubInventory*)pItem)->FindItemOrderByIndex( itemFinder );
			if(NULL != pTempItem)
			{
//				pSubInventoryItem = pItem;
				if(!pOrderItem)
					pOrderItem = pItem;
				else if (pItem->GetGridY() < pOrderItem->GetGridY())
					pOrderItem = pTempItem;
				else if (pItem->GetGridY() == pOrderItem->GetGridY())
					if (pItem->GetGridX() < pOrderItem->GetGridX())
						pOrderItem = pItem;
			}
		}
		iItem++;
	}
	
	if (pOrderItem && pOrderItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
	{
		//찾은 아이템이 서브인벤토리 라면, 그 안에 있는 가장 우선순위의 아이템을 찾아서 반환해야 한다.
		MItem *pResItem = ((MSubInventory*)pOrderItem)->FindItemOrderByIndex( itemFinder );
		// 만일.. 서브인벤토리에서 아이템을 못 찾았다면, 이것은 Orderitem를 잘못 찾은 것이다. 이때는 NULL이 반환될 것이다.
		return pResItem;
	}
	return pOrderItem;
}

//----------------------------------------------------------------------
// Find Item 
//----------------------------------------------------------------------
// 서브 인벤 까지 샅샅이 뒤진다..ㅋㅋ
//----------------------------------------------------------------------
MItem*			
MItemManager::FindItemAll( MItemFinder& itemFinder, MItem*& pSubInventoryItem) const
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = iItem->second;
		
		if (itemFinder( pItem ))
		{
			return pItem;
		}
		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
		{
			MItem* pTempItem = ((MSubInventory*)pItem)->FindItem( itemFinder );
			if(NULL != pTempItem)
			{
				pSubInventoryItem = pItem;
				return pTempItem;
			}
		}
		iItem++;
	}

	return NULL;

}



//----------------------------------------------------------------------
// GetItemToModifyAll
//----------------------------------------------------------------------
// 서브 인벤 까지 샅샅이 뒤진다..ㅋㅋ
//----------------------------------------------------------------------
MItem*			
MItemManager::GetItemToModifyAll( TYPE_OBJECTID id, MItem*& pSubInventoryItem)
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = iItem->second;
		
		if (pItem != NULL && pItem->GetID() == id)
		{
			return pItem;
		}
		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
		{
			MItem* pTempItem = ((MSubInventory*)pItem)->GetItemToModify( id );
			if(NULL != pTempItem)
			{
				pSubInventoryItem = pItem;
				return pTempItem;
			}
		}
		iItem++;
	}

	return NULL;

}

// 아이템을 인벤토리 내에서 여러개 검색하여 정보를 전해줄때 이용. 혹 findall 이 필요하면 만들것!
// 20070910
int
MItemManager::FindItemList(MItemFinder& itemFinder, ITEM_LIST& findList) 
{
	findList.clear();

	ITEM_MAP::const_iterator iItem = m_mapItem.begin();
	ITEM_LIST	ItemList;

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = iItem->second;
		
		if (itemFinder(pItem))
		{
			findList.push_back(pItem);
		}
//		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
//		{
//			MItem* pTempItem = ((MSubInventory*)pItem)->FindItemGridOrder( itemFinder );
//			if(NULL != pTempItem)
//				return pTempItem;
//		}
		iItem++;
	}

	return findList.size();
}
