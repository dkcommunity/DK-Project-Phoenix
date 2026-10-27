//////////////////////////////////////////////////////////////////////////////
// Filename    : CreatureInventory.h
// Written By  : elca@ewestsoft.com
// Revised By  : ±è¼º¹Î
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __PLAYER_INVENTORY_H__
#define __PLAYER_INVENTORY_H__

#include "Types.h"
#include "Inventory.h"
#include <list>
#define CONTRACT_OF_BLOOD_MAX 3
class PlayerCreature;
class Item;

class PlayerInventory : public Inventory
{
public:
	PlayerInventory(PlayerCreature* pPlayerCreature, CoordInven_t X, CoordInven_t Y, bool bDeleteAll=true) throw();
	virtual ~PlayerInventory() throw() ;
	virtual bool	addItem(CoordInven_t X, CoordInven_t Y, Item* pItem) throw();
	virtual Item* 	addItemEx(CoordInven_t X, CoordInven_t Y, Item* pItem) throw(Error);
	virtual void	deleteItem(CoordInven_t X, CoordInven_t Y) throw(Error);
	bool*			getCheckContractOfBlood() { return m_bCheckContractOfBlood; }

public:
	PlayerCreature* m_pPlayerCreature;
	bool            m_bCheckContractOfBlood[CONTRACT_OF_BLOOD_MAX];
};

#endif //__PLAYER_INVENTORY_H__
