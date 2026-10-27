//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireTunningItem.h
// Written By  : bigheart 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __VAMPIRE_TUNNING_ITEM_H__
#define __VAMPIRE_TUNNING_ITEM_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class VampireTunningItem;
//////////////////////////////////////////////////////////////////////////////

class VampireTunningItem : public Item 
{
public:
	VampireTunningItem() throw();
	VampireTunningItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM; }
	virtual string getObjectTableName() const throw() { return "VampireTunningItemObject"; }

	virtual ItemType_t getItemType() const throw() { return m_ItemType; }
	virtual void setItemType(ItemType_t itemType) throw() { m_ItemType = itemType; }

	virtual VolumeWidth_t getVolumeWidth() const throw(Error);
	virtual VolumeHeight_t getVolumeHeight() const throw(Error);
	virtual Weight_t getWeight() const throw(Error);

public:
	virtual ItemNum_t getNum() const throw() { return m_Num; }
	virtual void setNum(ItemNum_t Num) throw() { m_Num = Num; }

	bool    isStackable() const throw() { return true; }

private:
	ItemType_t m_ItemType;
	ItemNum_t  m_Num;
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};

//////////////////////////////////////////////////////////////////////////////
// class VampireTunningItemInfo
//////////////////////////////////////////////////////////////////////////////

class VampireTunningItemInfo : public ItemInfo 
{
public:

	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM; }
	virtual string toString() const throw();

	void	setRace(Race_t race)	{ m_Race = race; }
	Race_t	getRace() const throw() { return m_Race; }

	void	setTunningItemClass(int itemClass) { m_TunningItemClass = itemClass; }
	int getTunningItemClass() const throw() { return m_TunningItemClass; }

	void	setTunningItemType(ItemType_t itemType) { m_TunningItemType = itemType; }
	ItemType_t getTunningItemType() const throw() { return m_TunningItemType; }
	
private:
	Race_t m_Race;							// 종족 타입
	int m_TunningItemClass;
	ItemType_t m_TunningItemType;
};

//////////////////////////////////////////////////////////////////////////////
// class VampireTunningItemInfoManager;
//////////////////////////////////////////////////////////////////////////////

class VampireTunningItemInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM; }
	virtual void load() throw(Error);
};

extern VampireTunningItemInfoManager* g_pVampireTunningItemInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class VampireTunningItemFactory
//////////////////////////////////////////////////////////////////////////////

class VampireTunningItemFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM; }
	virtual string getItemClassName() const throw() { return "VampireTunningItem"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new VampireTunningItem(ItemType,OptionType,1); }
};

//////////////////////////////////////////////////////////////////////////////
// class VampireTunningItemLoader;
//////////////////////////////////////////////////////////////////////////////

class VampireTunningItemLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM; }
	virtual string getItemClassName() const throw() { return "VampireTunningItem"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern VampireTunningItemLoader* g_pVampireTunningItemLoader;

#endif
