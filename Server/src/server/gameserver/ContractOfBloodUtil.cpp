#include <cassert>
//////////////////////////////////////////////////////////////////////////////
// Filename    : ContractOfBloodUtil.cpp
// Written By  : Bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ContractOfBloodUtil.h"
#include "PlayerCreature.h"
#include "PlayerInventory.h"
#include "Item.h"
#include "VariableManager.h"
#include "ItemUtil.h"
#include "OptionInfo.h"
#include <cassert>

struct CONTACT_OF_BLOOD_POSITION
{
	CoordInven_t	X;
	CoordInven_t	Y;
};

const int MAX_CONTRACT_OF_BLOOD = 3;

const CONTACT_OF_BLOOD_POSITION ContractOfBloodPosition[MAX_CONTRACT_OF_BLOOD] = 
{
	{9, 0},
	{9, 2},
	{9, 4}
};

bool checkContractOfBlood( Item* pItem )
{
	bool bContractOfBlood = false;
	if( pItem == NULL )
		return bContractOfBlood;
	bContractOfBlood = (pItem->getItemClass() == Item::ITEM_CLASS_CONTRACT_OF_BLOOD );//&& pItem->getItemType() == 0 );
	return bContractOfBlood;
}

list<OptionType_t> getContractOfBloodPositionByItemType( Inventory* pInventory )
{
	list<OptionType_t> optionList;

	for( int i = 0; i < MAX_CONTRACT_OF_BLOOD; ++i )
	{
		Item* pItem = pInventory->getItem( ContractOfBloodPosition[i].X, ContractOfBloodPosition[i].Y);
		Item* pItem2 = pInventory->getItem( ContractOfBloodPosition[i].X, ContractOfBloodPosition[i].Y+1);
		if( pItem == NULL || pItem != pItem2  || !checkContractOfBlood(pItem) )
			continue;

		const list<OptionType_t>& itemOptionList = pItem->getOptionTypeList();
		optionList.insert(optionList.end(), itemOptionList.begin(), itemOptionList.end() );
	}

	return optionList;
}

bool checkContractOfBloodPosition( bool* checkPoint, CoordInven_t x, CoordInven_t y )
{
	if ( x != 8 || (y&1 != 0) )
		return false;

	int position = y>>1;

	checkPoint[position] = !checkPoint[position];
	return true;
}

list<OptionType_t> getContractOfBloodKindByItemType( PlayerCreature* pPlayerCreature )
{
	list<OptionType_t> optionList ;
	PlayerInventory* pPlayerInventory = dynamic_cast<PlayerInventory*>(pPlayerCreature->getInventory());

	if( pPlayerInventory == NULL )
	{
		return optionList;
	}

	for( int i = 0; i < 3; ++i )
	{
		if( pPlayerInventory->m_bCheckContractOfBlood[i] == true )
		{
			Item* pItem = pPlayerInventory->getItem( 8, i*2);
			Assert( pItem != NULL );

			const list<OptionType_t>& itemOptionList = pItem->getOptionTypeList(); 
			optionList.insert( optionList.end(), itemOptionList.begin(), itemOptionList.end() );
		}
	}
	return optionList;
}

