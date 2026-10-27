//////////////////////////////////////////////////////////////////////////////
// Filename    : CheckMoney.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CHECK_MONEY_H__
#define __CHECK_MONEY_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class CheckMoney;
//////////////////////////////////////////////////////////////////////////////

class CheckMoney : public Item 
{
public:
	CheckMoney() throw();
	CheckMoney(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	virtual ~CheckMoney() throw() {}
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	bool destroy() throw(Error);


	// get debug string
	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CHECK_MONEY; }
	virtual string getObjectTableName() const throw() { return "CheckMoneyObject"; }

	virtual ItemType_t getItemType() const throw() { return m_ItemType; }
	virtual void setItemType(ItemType_t itemType) throw() { m_ItemType = itemType; }

	virtual VolumeWidth_t getVolumeWidth() const throw(Error);
	virtual VolumeHeight_t getVolumeHeight() const throw(Error);
	virtual Weight_t getWeight() const throw(Error);

//	virtual ItemNum_t getNum() const throw() { return m_Num; }
//	virtual void setNum(ItemNum_t Num) throw() { m_Num = Num; }

	bool    isStackable() const throw() { return false; }

private:

	ItemType_t m_ItemType;			// 아이템 타입
	list<OptionType_t>	m_OptionType;
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};


//--------------------------------------------------------------------------------
// 
// class CheckMoneyInfo
// 
//--------------------------------------------------------------------------------

class CheckMoneyInfo : public ItemInfo 
{
public:

	// get item class
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CHECK_MONEY; }
	// toString
	virtual string toString() const throw();

private:
};


//--------------------------------------------------------------------------------
//
// class CheckMoneyInfoManager;
//
//--------------------------------------------------------------------------------
class CheckMoneyInfoManager : public InfoClassManager {

public:

	// get item class
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CHECK_MONEY; }
	// load from DB
	virtual void load() throw(Error);

};

// global variable declaration
extern CheckMoneyInfoManager* g_pCheckMoneyInfoManager;


//--------------------------------------------------------------------------------
//
// class CheckMoneyFactory
//
//--------------------------------------------------------------------------------
class CheckMoneyFactory : public ItemFactory {

public:
	
	// get item class
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CHECK_MONEY; }

	// get item classname
	virtual string getItemClassName() const throw() { return "CheckMoney"; }
	
public:

	// create item
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new CheckMoney(ItemType,OptionType); }

};


//--------------------------------------------------------------------------------
//
// class CheckMoneyLoader;
//
//--------------------------------------------------------------------------------

class CheckMoneyLoader : public ItemLoader {

public:

	// get item class
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CHECK_MONEY; }

	// get item class name
	virtual string getItemClassName() const throw() { return "CheckMoney"; }

public:

	// load to creature
	virtual void load(Creature* pCreature) throw(Error);

	// load to zone
	virtual void load(Zone* pZone) throw(Error);

	// load to inventory
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);

};

extern CheckMoneyLoader* g_pCheckMoneyLoader;
#endif
