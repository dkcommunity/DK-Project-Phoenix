//////////////////////////////////////////////////////////////////////////////
// Filename    : Shield.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SHIELD_H__
#define __SHIELD_H__

#include "Item.h"
#include "ConcreteItem.h"
#include "ItemPolicies.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class Shield;
//////////////////////////////////////////////////////////////////////////////

class Shield : public ConcreteItem<Item::ITEM_CLASS_SHIELD, NoStack, HasDurability, HasOption, GroceryGrade, NoAttacking>
{
public:
	Shield() throw();
	Shield(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:

private:

	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};


//////////////////////////////////////////////////////////////////////////////
// class ShieldInfo
//////////////////////////////////////////////////////////////////////////////

class ShieldInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SHIELD; }

	virtual Durability_t getDurability() const throw() { return m_Durability; }
	virtual void setDurability(Durability_t durability) throw() { m_Durability = durability; }

	Defense_t getDefenseBonus() const throw() { return m_DefenseBonus; }
	void setDefenseBonus(Defense_t acBonus) throw() { m_DefenseBonus = acBonus; }

	Protection_t getProtectionBonus() const throw() { return m_ProtectionBonus; }
	void setProtectionBonus(Protection_t acBonus) throw() { m_ProtectionBonus = acBonus; }

	virtual uint getItemLevel(void) const throw() { return m_ItemLevel; }
	virtual void setItemLevel(uint level) throw() { m_ItemLevel = level; }

	virtual string toString() const throw();

private:
	Durability_t		m_Durability;		// 내구성
	Defense_t			m_DefenseBonus;		// 명중률 보너스
	Protection_t		m_ProtectionBonus;
	uint				m_ItemLevel;
};


//////////////////////////////////////////////////////////////////////////////
// class ShieldInfoManager;
//////////////////////////////////////////////////////////////////////////////

class ShieldInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SHIELD; }
	virtual void load() throw(Error);
};

// global variable declaration
extern ShieldInfoManager* g_pShieldInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class ShieldFactory
//////////////////////////////////////////////////////////////////////////////

class ShieldFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SHIELD; }
	virtual string getItemClassName() const throw() { return "Shield"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new Shield(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class ShieldLoader;
//////////////////////////////////////////////////////////////////////////////

class ShieldLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SHIELD; }
	virtual string getItemClassName() const throw() { return "Shield"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern ShieldLoader* g_pShieldLoader;

#endif
