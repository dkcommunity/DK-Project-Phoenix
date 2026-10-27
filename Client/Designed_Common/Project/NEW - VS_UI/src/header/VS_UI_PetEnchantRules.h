#ifndef __VS_UI_PET_ENCHANT_RULES_H__
#define __VS_UI_PET_ENCHANT_RULES_H__

#include "MItem.h"

inline bool PhoenixCanUseADeLantePetInjection(const MItem* pMouseItem, const MItem* pTargetItem)
{
	if (pMouseItem == NULL || pTargetItem == NULL)
	{
		return false;
	}

	if (pMouseItem->GetItemClass() != ITEM_CLASS_PET_ENCHANT_ITEM ||
		pMouseItem->GetItemType() != 218 ||
		pTargetItem->GetItemClass() != ITEM_CLASS_PET_ITEM)
	{
		return false;
	}

	const int targetType = pTargetItem->GetItemType();
	return targetType == 0 || targetType == 1;
}

#endif // __VS_UI_PET_ENCHANT_RULES_H__
