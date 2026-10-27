//////////////////////////////////////////////////////////////////////////////
// Filename    : Bracelet.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __BRACELET_H__
#define __BRACELET_H__

#include "Item.h"
#include "ConcreteItem.h"
#include "ItemPolicies.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class Bracelet;
//////////////////////////////////////////////////////////////////////////////

class Bracelet : public ConcreteItem<Item::ITEM_CLASS_BRACELET, NoStack, HasDurability, HasOption, AccessoryGrade, NoAttacking>
{
public:
	Bracelet() throw();
	Bracelet(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);
	
	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

private:
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};


//////////////////////////////////////////////////////////////////////////////
// class BraceletInfo
//////////////////////////////////////////////////////////////////////////////

class BraceletInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_BRACELET; }

	virtual Durability_t getDurability() const throw() { return m_Durability; }
	virtual void setDurability(Durability_t durability) throw() { m_Durability = durability; }

	Defense_t getDefenseBonus() const throw() { return m_DefenseBonus; }
	void setDefenseBonus(Defense_t acBonus) throw() { m_DefenseBonus = acBonus; }

	Protection_t getProtectionBonus() const throw() { return m_ProtectionBonus; }
	void setProtectionBonus(Protection_t acBonus) throw() { m_ProtectionBonus = acBonus; }

	// 20080703 
	int getMagicProtectionBonus() const throw() { return m_MagicProtectionBonus; }
	void setMagicProtectionBonus(int protection) throw() {m_MagicProtectionBonus = protection; }

	virtual uint getItemLevel(void) const throw() { return m_ItemLevel; }
	virtual void setItemLevel(uint level) throw() { m_ItemLevel = level; }

	virtual string toString() const throw();

private:
	Durability_t   m_Durability;
	Defense_t      m_DefenseBonus;
	Protection_t   m_ProtectionBonus;
	int			   m_MagicProtectionBonus; // 20080703
	uint           m_ItemLevel;

};


//////////////////////////////////////////////////////////////////////////////
// class BraceletInfoManager;
//////////////////////////////////////////////////////////////////////////////

class BraceletInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_BRACELET; }
	virtual void load() throw(Error);
};

extern BraceletInfoManager* g_pBraceletInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class BraceletFactory
//////////////////////////////////////////////////////////////////////////////

class BraceletFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_BRACELET; }
	virtual string getItemClassName() const throw() { return "Bracelet"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new Bracelet(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class BraceletLoader;
//////////////////////////////////////////////////////////////////////////////

class BraceletLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_BRACELET; }
	virtual string getItemClassName() const throw() { return "Bracelet"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);

};

extern BraceletLoader* g_pBraceletLoader;



#endif
