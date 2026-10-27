//////////////////////////////////////////////////////////////////////////////
// Filename    : Item.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Item.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "Assert.h"

#include "PCItemInfo.h"
#include "PlayerCreature.h"
#include "Store.h"

#include "Cpackets/CGRequestStoreInfo.h"

#include <cstdio>
#ifdef VERSION_THIRD_ENCHANT_1
#include "ThirdEnchantInfo.h"
#endif

#include "ItemUtil.h"
#include "DBExecuteManager.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
// class Item member methods
//////////////////////////////////////////////////////////////////////////////

Item::Item()
	throw()
{
	m_ItemID = 0;
	m_CreateType = CREATE_TYPE_NORMAL;
	m_bUnique = false;
	m_bQuest = false;
	m_bTrace = false;
	m_bTimeLimit = false;
	m_bOnStore = false;
	m_pStore = NULL;
}

Item::~Item()
	throw()
{
}

bool Item::destroy() 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "DELETE FROM " << getObjectTableName().c_str() << " WHERE ItemID=" << m_ItemID;
	
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	return true;

	__END_CATCH
}

void Item::tinysave(const string & field) const throw (Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE " << getObjectTableName().c_str() 
		<< " SET " << field 
		<< " WHERE ItemID=" << m_ItemID; 
	
	if ( getOptionTypeList().size() > 0 || getThirdOptionTypeList().size() < 0 || getGTwoOptionTypeList().size() < 0 )
	{
		os << " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")"
		<< " GTwoOptionType=(" << getOptionTypeToString(getGTwoOptionTypeList()).c_str() << ")";
	}
	
	//cout <<"tinysave::::-->"<<os.str() << endl;
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH

}

// 아이템을 버린다. 기본은 가비지로, 특별한 경우엔 Timeover 나 그외 안 쓰는 storage로..
void Item::waste(Storage storage ) const
	throw(Error)
{
	Assert( (uint)storage >= (uint)STORAGE_GARBAGE );

	char query[50];

	sprintf( query, "Storage = %u", (uint)storage );

	tinysave(query );
}

const list<OptionType_t>& Item::getDefaultOptions(void) const 
	throw()
{
	__BEGIN_TRY

	try {

		return g_pItemInfoManager->getItemInfo(getItemClass(), getItemType())->getDefaultOptions();

	} catch (NoSuchElementException&){
		StringStream msg;
		msg << "그런 아이템 정보가 없습니다.(" << (int)getItemClass() << ", " << (int)getItemType() << ")";

		filelog("itemError.txt", "%s", msg.toString().c_str());
		//throw Error(msg.toString());
	}

	static list<OptionType_t> optionTypes;
	return optionTypes;

	__END_CATCH
}

bool Item::isQuestItem() const
{
	return (getItemClass() == ITEM_CLASS_EVENT_ITEM && getItemType() < 27 ) ||//(!= 27 && getItemType() != 28) || 
		(getItemClass()==ITEM_CLASS_EVENT_TREE && getItemType()>12 && getItemType()<26) ||
		(getItemClass()==ITEM_CLASS_EVENT_TREE && getItemType()>28 && getItemType()<42) ||
		(getItemClass()==ITEM_CLASS_CODE_SHEET);
}

void Item::makePCItemInfo(PCItemInfo& result) const
{
	if ( m_ObjectID == 0 )
	{
		filelog("ItemError.log", "아이템 oid가 0입니다. : %s", toString().c_str());
		result.setObjectID(0);
	}
	else
	{
		result.setObjectID(getObjectID());
	}

	result.setItemClass(getItemClass());
	result.setItemType(getItemType());
	result.setOptionType(getOptionTypeList());
	result.setGTwoOptionType(getGTwoOptionTypeList());
	result.setSilver(getSilver());
	result.setDurability(getDurability());
	result.setEnchantLevel(getEnchantLevel());
	result.setItemNum(getNum());
	result.setMainColor(0);
	result.setGrade(getGrade());
#ifdef VERSION_THIRD_ENCHANT_1
	result.setThirdOptionType(getThirdOptionTypeList());
	result.setThirdEnchantType((BYTE)g_pThirdEnchantInfo->getEnchantClassThirdOption(getThirdOptionTypeList()));
#endif
}

void Item::whenPCTake( PlayerCreature* pPC )
{
//	cout << pPC->getName() << " Take " << toString() << endl; 
}

void Item::whenPCLost( PlayerCreature* pPC )
{
//	cout << pPC->getName() << " Lost " << toString() << endl; 
	Store* pStore = pPC->getStore();
	BYTE index = pStore->getItemIndex(this);

	if ( index != 0xff )
	{
		pStore->removeStoreItem( index );

		CGRequestStoreInfo cgInfo;
		cgInfo.setOwnerObjectID(0);
		cgInfo.execute( pPC->getPlayer() );
	}
}

Item::ItemClass Item::getItemClass(string& strItemClass)
{
	int iIndex;
	
	for( iIndex = 0; iIndex < Item::ITEM_CLASS_MAX ; ++iIndex )
	{
		if ( strItemClass.compare(ItemClass2String[iIndex]) == 0 )
		{
			return (ItemClass)iIndex; 
		}
	}
	
	return Item::ITEM_CLASS_MAX;
}
