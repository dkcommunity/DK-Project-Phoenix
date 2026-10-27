//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemClassInfo.h
// Written by  : bigheart
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __ITEM_CLASS_INFO_H__
#define __ITEM_CLASS_INFO_H__

#include "Types.h"
#include "Item.h"

struct ItemClassInfoStruct
{
	char	Name[30];
	int		EnchantClass1;
	int		EnchantClass2;
};

class ItemClassInfo
{
public:
	void load() throw(Error);
	bool getEnchantClass1(int iItemClass) { return m_ItemClassInfo[iItemClass].EnchantClass1; }
	bool getEnchantClass2(int iItemClass) { return m_ItemClassInfo[iItemClass].EnchantClass2; }
	bool hasEnchantClass(int iItemClass, int iEnchantClass);
protected:
	ItemClassInfoStruct m_ItemClassInfo[Item::ITEM_CLASS_MAX];	
};

extern ItemClassInfo* g_pItemClassInfo;

#endif