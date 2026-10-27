//////////////////////////////////////////////////////////////////////////////
// Filename    : BalloonHeadbandUtil.h
// Written By  : Bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CONTRACT_OF_BLOOD_UTIL_H__
#define __CONTRACT_OF_BLOOD_UTIL_H__

class Item;
class PlayerCreature;

#include "Types.h"
#include "Inventory.h"
#include <list>

bool checkContractOfBlood(Item* pItem);
list<OptionType_t> getContractOfBloodPositionByItemType(Inventory* pInventory);
bool checkContractOfBloodPosition( bool* checkPoint, CoordInven_t x, CoordInven_t y );
list<OptionType_t> getContractOfBloodKindByItemType(PlayerCreature* pPlayerCreature);

#endif //__CONTRACT_OF_BLOOD_UTIL_H__
