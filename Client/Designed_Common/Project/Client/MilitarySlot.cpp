//----------------------------------------------------------------------
// MPlayerGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MilitarySlot.h"
#include "MItem.h"
#include "MHelpManager.h"
#include "debuginfo.h"

#ifdef __GAME_CLIENT__
	#include "ClientConfig.h"
#endif

MilitarySlot* g_pMilitarySlot = NULL;

//----------------------------------------------------------------------
// 
// constructor / destructor
//
//----------------------------------------------------------------------
MilitarySlot::MilitarySlot()
{
	Release();
}

MilitarySlot::~MilitarySlot()
{
	Release();
}
//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init ItemStatus ( n개 )
//----------------------------------------------------------------------
void			
MilitarySlot::Init(BYTE size)
{
	MSlotItemManager::Init( size );
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void			
MilitarySlot::Release()
{
	MSlotItemManager::Release();
}

//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------
bool			
MilitarySlot::AddItem(MItem* pItem, BYTE n)
{
	bool bOK = MSlotItemManager::AddItem(pItem, n);

	return bOK;
}


//----------------------------------------------------------------------
// Get Item
//----------------------------------------------------------------------
// get get get~
//----------------------------------------------------------------------
MItem*
MilitarySlot::GetItem(BYTE n) const
{
	return MSlotItemManager::GetItem( n );	// 음냐 하하..
}

//----------------------------------------------------------------------
// Remove Item ( slot )
//----------------------------------------------------------------------
MItem*			
MilitarySlot::RemoveItem(BYTE n)
{
	MItem* pItem = MSlotItemManager::RemoveItem( n );
	return pItem;
}

//----------------------------------------------------------------------
// Remove Item ( id )
//----------------------------------------------------------------------
MItem*			
MilitarySlot::RemoveItem(TYPE_OBJECTID id)
{
	MItem* pItem = MSlotItemManager::RemoveItem( id );
	return pItem;
}

//----------------------------------------------------------------------
// Replace Item
//----------------------------------------------------------------------
bool			
MilitarySlot::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	bool bAdded = MSlotItemManager::ReplaceItem( pItem, n, pOldItem );
	return bAdded;
}
