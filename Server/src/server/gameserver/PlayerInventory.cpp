////////////////////////////////////////////////////////////////////////////////
// Filename    : CreatureInventory.cpp
// Written By  : elca@ewestsoft.com
// Revised By  : ±è¼º¹Î
// Description : 
////////////////////////////////////////////////////////////////////////////////

#include "PlayerCreature.h"
#include "Item.h"
#include "PlayerInventory.h"
#include "ContractOfBloodUtil.h"

PlayerInventory::PlayerInventory(PlayerCreature* pPlayerCreture, CoordInven_t X, CoordInven_t Y, bool bDeleteAll)
	throw()
	: Inventory( X, Y ) 
{
	m_pPlayerCreature = pPlayerCreture;
	memset(m_bCheckContractOfBlood, 0, sizeof(bool)*CONTRACT_OF_BLOOD_MAX);
}

PlayerInventory::~PlayerInventory()
	throw()
{
}

Item* PlayerInventory::addItemEx(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw(Error)
{
	Item* pInventoryItem;
	if( !(pInventoryItem = Inventory::addItemEx(X, Y, pItem)) )
	{
		return NULL;
	}
	bool bContractOfBlood = (pItem->getItemClass() == Item::ITEM_CLASS_CONTRACT_OF_BLOOD && pItem->getItemType() == 0);
	
	if( bContractOfBlood )
	{
		if( checkContractOfBloodPosition( m_bCheckContractOfBlood, X, Y ) )
		{
//			m_pPlayerCreature->initAllStatAndSend();
		}
	}
	return pInventoryItem;
}

bool PlayerInventory::addItem(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw()
{
	if( !Inventory::addItem( X, Y, pItem) )
	{
		return false;
	}

	bool bContractOfBlood = (pItem->getItemClass() == Item::ITEM_CLASS_CONTRACT_OF_BLOOD && pItem->getItemType() == 0);
	
	if( bContractOfBlood )
	{
		if( checkContractOfBloodPosition( m_bCheckContractOfBlood, X, Y ) )
		{
			m_pPlayerCreature->initAllStatAndSend();
		}
	}

	return true;
}

void PlayerInventory::deleteItem(CoordInven_t X, CoordInven_t Y)
	throw(Error)
{
	Item* pItem = getItem(X, Y);
	ItemClass_t	itemClass = 0;

	if( pItem != NULL )
	{
		itemClass = pItem->getItemClass();
	}

	Inventory::deleteItem(X, Y);

	if( (itemClass == Item::ITEM_CLASS_CONTRACT_OF_BLOOD) )
	{
		if( checkContractOfBloodPosition( m_bCheckContractOfBlood, X, Y) )
		{
			m_pPlayerCreature->initAllStatAndSend();
		}
	}
}
