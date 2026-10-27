//////////////////////////////////////////////////////////////////////////////
// Filename    : CommonQuestItem.h
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __COMMON_QUEST_ITEM_H__
#define __COMMON_QUEST_ITEM_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItem;
//////////////////////////////////////////////////////////////////////////////

class CommonQuestItem : public Item 
{
public:
	CommonQuestItem() throw();
	CommonQuestItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_COMMON_QUEST_ITEM; }
	virtual string getObjectTableName() const throw() { return "CommonQuestItemObject"; }

	virtual ItemType_t getItemType() const throw() { return m_ItemType; }
	virtual void setItemType(ItemType_t itemType) throw() { m_ItemType = itemType; }

	virtual VolumeWidth_t getVolumeWidth() const throw(Error);
	virtual VolumeHeight_t getVolumeHeight() const throw(Error);
	virtual Weight_t getWeight() const throw(Error);

	virtual ItemNum_t getNum() const throw() { return m_Num; }
	virtual void setNum(ItemNum_t Num) throw() { m_Num = Num; }

	bool    isStackable() const throw() { return true; }

private:
	ItemType_t m_ItemType;
	ItemNum_t m_Num;				// 겹쳐있는 아이템 갯수
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItemInfo
//////////////////////////////////////////////////////////////////////////////

class CommonQuestItemInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_COMMON_QUEST_ITEM; }
	virtual string toString() const throw();

	Ratio_t getBonusRatio() const throw() { return m_BonusRatio; }
	void setBonusRatio(Ratio_t BonusRatio) throw() { m_BonusRatio = BonusRatio; }

private :
	Ratio_t    m_BonusRatio;

};

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItemInfoManager;
//////////////////////////////////////////////////////////////////////////////

class CommonQuestItemInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_COMMON_QUEST_ITEM; }
	virtual void load() throw(Error);
};

extern CommonQuestItemInfoManager* g_pCommonQuestItemInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItemFactory
//////////////////////////////////////////////////////////////////////////////

class CommonQuestItemFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_COMMON_QUEST_ITEM; }
	virtual string getItemClassName() const throw() { return "CommonQuestItem"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new CommonQuestItem(ItemType,OptionType,1); }
};

//////////////////////////////////////////////////////////////////////////////
// class CommonQuestItemLoader;
//////////////////////////////////////////////////////////////////////////////

class CommonQuestItemLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_COMMON_QUEST_ITEM; }
	virtual string getItemClassName() const throw() { return "CommonQuestItem"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern CommonQuestItemLoader* g_pCommonQuestItemLoader;

#endif
