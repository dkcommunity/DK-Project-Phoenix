//----------------------------------------------------------------------
// MPremiumGiveItemInfo.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"

#ifdef __GAME_CLIENT__

	#include "MItem.h"
	#include "MPremiumGiveItemInfo.h"

#endif

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MPremiumGiveItemInfoList*		g_pPremiumGiveItemInfoList = NULL;

MPremiumGiveItemInfoList::MPremiumGiveItemInfoList()
{
}

MPremiumGiveItemInfoList::~MPremiumGiveItemInfoList()
{
	Give_Item_List_ClearAll();
}

//----------------------------------------------------------------------
// Add item ( pItem )
//----------------------------------------------------------------------
bool
MPremiumGiveItemInfoList::AddItem(MPremiumGiveItemInfo *pItem)
{
	m_Give_Item_List.push_back(pItem);
	return true;
}

void MPremiumGiveItemInfoList::Give_Item_List_ClearAll()
{
	std::vector<MPremiumGiveItemInfo *>::iterator itt_Give_Item_List = m_Give_Item_List.begin();

	while (itt_Give_Item_List != m_Give_Item_List.end())
	{
		MPremiumGiveItemInfo* p_PremiumGiveItemInfo = (*itt_Give_Item_List);

		delete p_PremiumGiveItemInfo;
		p_PremiumGiveItemInfo = NULL;

		itt_Give_Item_List++;
	}
	
	m_Give_Item_List.clear();
	m_Give_Item_List.resize(0);
}
