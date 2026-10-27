//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersWingItem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __OUSTERS_WING_ITEM_H__
#define __OUSTERS_WING_ITEM_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class OustersWingItem;
//////////////////////////////////////////////////////////////////////////////

class OustersWingItem : public Item 
{
public:
	OustersWingItem() throw();
	OustersWingItem(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_OUSTERS_WING_ITEM; }
	virtual string getObjectTableName() const throw() { return "OustersWingItemObject"; }

	virtual ItemType_t getItemType() const throw() { return m_ItemType; }
	virtual void setItemType(ItemType_t itemType) throw() { m_ItemType = itemType; }

	virtual VolumeWidth_t getVolumeWidth() const throw(Error);
	virtual VolumeHeight_t getVolumeHeight() const throw(Error);
	virtual Weight_t getWeight() const throw(Error);

	Color_t getBodyColor() { return m_BodyColor; }
	void setBodyColor(Color_t Color) { m_BodyColor = Color; }

	Color_t getEffectColor() { return m_EffectColor; }
	void setEffectColor(Color_t Color) { m_EffectColor = Color; }

	virtual string toString() const throw();

private:
	ItemType_t m_ItemType; // 아이템 타입

	Color_t m_BodyColor;
	Color_t m_EffectColor;
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};


//////////////////////////////////////////////////////////////////////////////
// class OustersWingItemInfo
//////////////////////////////////////////////////////////////////////////////

class OustersWingItemInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_OUSTERS_WING_ITEM; }
	virtual string toString() const throw();

	Color_t getBodyColor() { return m_BodyColor; }
	void setBodyColor(Color_t Color) { m_BodyColor = Color; }

	Color_t getEffectColor() { return m_EffectColor; }
	void setEffectColor(Color_t Color) { m_EffectColor = Color; }

private:
	ItemType_t m_ItemType; // 아이템 타입

	Color_t m_BodyColor;
	Color_t m_EffectColor;
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};


//////////////////////////////////////////////////////////////////////////////
// class OustersWingItemInfoManager;
//////////////////////////////////////////////////////////////////////////////

class OustersWingItemInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_OUSTERS_WING_ITEM; }
	virtual void load() throw(Error);
};

extern OustersWingItemInfoManager* g_pOustersWingItemInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class OustersWingItemFactory
//////////////////////////////////////////////////////////////////////////////

class OustersWingItemFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_OUSTERS_WING_ITEM; }
	virtual string getItemClassName() const throw() { return "OustersWingItem"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new OustersWingItem(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class OustersWingItemLoader;
//////////////////////////////////////////////////////////////////////////////

class OustersWingItemLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_OUSTERS_WING_ITEM; }
	virtual string getItemClassName() const throw() { return "OustersWingItem"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);

};

extern OustersWingItemLoader* g_pOustersWingItemLoader;

#endif
