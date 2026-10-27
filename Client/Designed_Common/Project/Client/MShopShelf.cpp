//----------------------------------------------------------------------
// MShopShelf.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MShopShelf.h"
//#include "MShopFixedItemTable.h"

#ifdef __GAME_CLIENT__
	#include "MPlayer.h"
#endif

//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// ItemClass에 맞게 메모리를 잡아주는 class table을 설정..
//----------------------------------------------------------------------
MShopShelf::FUNCTION_NEWSHELFCLASS
MShopShelf::s_NewShelfClassTable[MAX_SHELF] =
{
	MShopFixedShelf::NewShelf,
	MShopSpecialShelf::NewShelf,
	MShopUnknownShelf::NewShelf
};

//----------------------------------------------------------------------
//
//							 MShop Shelf
//
//----------------------------------------------------------------------
MShopShelf::MShopShelf()
{
	m_bShelfEnable = false;

	m_Version = 0;

	// 모두 NULL로 초기화
	for (int i=0; i<SHOP_SHELF_SLOT; i++)
	{
		m_pItems[i] = NULL;
	}
}

MShopShelf::~MShopShelf()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// New Shelf
//----------------------------------------------------------------------
// ShelfClass에 맞는 class의 객체를 생성해서(new) 넘겨준다.
//
// FIX 2026-05-12: VS Debugger ile reproduce edildi — NPC Shop tiklandiktan
// sonra crash burada (GCShopListHandler.cpp -> NewShelf cagri zinciri).
// Root cause: s_NewShelfClassTable array size = MAX_SHELF (3). Eger
// ShelfClass >= MAX_SHELF (packet'tan geliyor, server hatali/buggy data
// gonderirse) -> OOB read function pointer -> garbage address indirect
// call -> Access Violation.
//
// Memory: project_empty_table_oob_pattern.md ile benzer (global table
// direct index access bounds check yok).
//----------------------------------------------------------------------
MShopShelf*
MShopShelf::NewShelf(MShopShelf::SHELF_TYPE ShelfClass)
{
	if (ShelfClass < 0 || ShelfClass >= MAX_SHELF)
	{
		// Caller (GCShopListHandler etc.) zaten NULL guard'a sahip,
		// graceful skip eder. Aksi takdirde OOB -> crash.
		return NULL;
	}
	return (MShopShelf*)(*s_NewShelfClassTable[ShelfClass])();
};

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MShopShelf::Release()
{
	m_bShelfEnable = false;

	m_Version = 0;

	for (int i=0; i<SHOP_SHELF_SLOT; i++)
	{
		// 뭔가 있으면 지우고.. NULL로..
		if (m_pItems[i]!=NULL)
		{
			delete m_pItems[i];
		}

		m_pItems[i] = NULL;
	}
}

//----------------------------------------------------------------------
// Is Empty
//----------------------------------------------------------------------
bool		
MShopShelf::IsEmptySlot(unsigned int slot)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return true;
	}
	
	return m_pItems[slot]==NULL;
}

//----------------------------------------------------------------------
// Get item
//----------------------------------------------------------------------
MItem*		
MShopShelf::GetItem(unsigned int slot) const
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return NULL;
	}

	return m_pItems[slot];
}
//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------
// 앞에서부터 검색해서 빈 곳에 추가한다.
//----------------------------------------------------------------------
bool		
MShopShelf::AddItem(MItem* pItem)
{
	for (int i=0; i<SHOP_SHELF_SLOT; i++)
	{
		// 빈 곳에 추가한다.
		if (m_pItems[i]==NULL)
		{
			if (SetItem( i, pItem ))
			{
				return true;
			}

			return false;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Set Item
//----------------------------------------------------------------------
bool		
MShopShelf::SetItem(unsigned int  slot, MItem* pItem)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return false;
	}

	// 이전에 뭔가 있었으면..
	if (m_pItems[slot]!=NULL)
	{
		delete m_pItems[slot];
	}

	m_pItems[slot] = pItem;

	//------------------------------------------------------
	// 사용 가능 여부 체크
	//------------------------------------------------------
	#ifdef __GAME_CLIENT__
		g_pPlayer->CheckAffectStatus( pItem );
	#endif

	return true;
}

//----------------------------------------------------------------------
// Remove - Shelf에서 제거해서 return해준다.(외부에서 delete해야한다.)
//----------------------------------------------------------------------
MItem*		
MShopShelf::RemoveItem(unsigned int slot)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return NULL;
	}

	// 임시로 저장하고 
	MItem* pRemoveItem = m_pItems[slot];

	// 내부에 있는 걸 지워준다.
	m_pItems[slot] = NULL;

	return pRemoveItem;
}

//----------------------------------------------------------------------
// Delete - Shelf 내부에 있는걸 지워준다.
//----------------------------------------------------------------------
void		
MShopShelf::DeleteItem(unsigned int slot)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return;
	}

	// 이전에 뭔가 있었으면..
	if (m_pItems[slot]!=NULL)
	{
		delete m_pItems[slot];
	}

	// 없애준다.
	m_pItems[slot] = NULL;
}


//----------------------------------------------------------------------
//
//							MShop FixedShelf 
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init From FixedItemTable
//----------------------------------------------------------------------
/*
bool		
MShopFixedShelf::InitFromFixedItemTable(unsigned int tableID)
{
	FIXED_ITEM_INFO* pInfo = g_ShopFixedItemTable.GetItemInfo( tableID );

	// 그런 id의 정보는 없따~
	if (pInfo==NULL)
	{
		return false;
	}


	//---------------------------------------------------------
	// 그 tableID에 속한 모든 item들을 검색..
	//---------------------------------------------------------
	FIXED_ITEM_INFO::iterator iInfo = pInfo->begin();

	while (iInfo != pInfo->end())
	{
		unsigned int	slotID		= (*iInfo).first;
		ITEM_INFO*		pItemInfo	= (*iInfo).second;

		//---------------------------------------------------------
		// Item을 생성하고 정보를 설정한다.
		//---------------------------------------------------------
		MItem* pItem = MItem::NewItem( pItemInfo->Class );
		
		pItem->SetItemType( pItemInfo->Type );

		pItem->SetItemOption( pItemInfo->Option );

		pItem->SetCurrentDurability( pItemInfo->Durability );

		pItem->SetNumber( pItemInfo->Number );

		//---------------------------------------------------------
		// 생성된 item을 shelf에 추가한다.
		//---------------------------------------------------------
		if (!SetItem( slotID, pItem ))
		{
			// item이 shelf에 추가되지 못한 경우.. -_-;;
			delete pItem;
		}
	
		//---------------------------------------------------------
		// 다음
		//---------------------------------------------------------
		iInfo++;
	}

	return true;
}
*/

//----------------------------------------------------------------------
//
// MShop FixedShelf
//
//----------------------------------------------------------------------
MShopFixedShelf::MShopFixedShelf()
{
	m_Name = "Normal";
}

MShopFixedShelf::~MShopFixedShelf()
{
}

//----------------------------------------------------------------------
//
// MShop SpecialShelf
//
//----------------------------------------------------------------------
MShopSpecialShelf::MShopSpecialShelf()
{
	m_Name = "Special";
}

MShopSpecialShelf::~MShopSpecialShelf()
{
}

//----------------------------------------------------------------------
//
// MShop UnknownShelf
//
//----------------------------------------------------------------------
MShopUnknownShelf::MShopUnknownShelf()
{
	m_Name = "Mysterious";
}

MShopUnknownShelf::~MShopUnknownShelf()
{
}

