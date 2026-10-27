//////////////////////////////////////////////////////////////////////////////
// Filename    : Ring.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __RING_H__
#define __RING_H__

#include "Item.h"
#include "ConcreteItem.h"
#include "ItemPolicies.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class Ring;
//////////////////////////////////////////////////////////////////////////////

class Ring : public ConcreteItem<Item::ITEM_CLASS_RING, NoStack, HasDurability, HasOption, AccessoryGrade, NoAttacking>
{
public:
	Ring() throw();
	Ring(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
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
// class RingInfo
//////////////////////////////////////////////////////////////////////////////

class RingInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RING; }

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
	Durability_t		m_Durability;		// 내구성
	Defense_t			m_DefenseBonus;		// 명중률 보너스
	Protection_t		m_ProtectionBonus;
	int					m_MagicProtectionBonus; // 마법 방어 20080703
	uint				m_ItemLevel;

};


//////////////////////////////////////////////////////////////////////////////
// class RingInfoManager;
//////////////////////////////////////////////////////////////////////////////

class RingInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RING; }
	virtual void load() throw(Error);
};

// global variable declaration
extern RingInfoManager* g_pRingInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class RingFactory
//////////////////////////////////////////////////////////////////////////////

class RingFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RING; }
	virtual string getItemClassName() const throw() { return "Ring"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new Ring(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class RingLoader;
//////////////////////////////////////////////////////////////////////////////

class RingLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RING; }
	virtual string getItemClassName() const throw() { return "Ring"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern RingLoader* g_pRingLoader;
#endif
