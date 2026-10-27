//////////////////////////////////////////////////////////////////////////////
// Filename    : Mace.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __MACE_H__
#define __MACE_H__

#include "Item.h"
#include "ConcreteItem.h"
#include "ItemPolicies.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class Mace;
//////////////////////////////////////////////////////////////////////////////

class Mace : public ConcreteItem<Item::ITEM_CLASS_MACE, NoStack, HasDurability, HasOption, WeaponGrade, SlayerWeapon>
{
public:
	Mace() throw();
	Mace(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:

	MP_t getMPBonus() const throw();
	
private:
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};


//////////////////////////////////////////////////////////////////////////////
// class MaceInfo
//////////////////////////////////////////////////////////////////////////////

class MaceInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_MACE; }

	virtual Durability_t getDurability() const throw() { return m_Durability; }
	virtual void setDurability(Durability_t durability) throw() { m_Durability = durability; }

	virtual Damage_t getMinDamage() const throw() { return m_MinDamage; }
	virtual void setMinDamage(Damage_t minDamage) throw() { m_MinDamage = minDamage; }

	virtual Damage_t getMaxDamage() const throw() { return m_MaxDamage; }
	virtual void setMaxDamage(Damage_t maxDamage) throw() { m_MaxDamage = maxDamage; }

	virtual MP_t getMPBonus() const throw() { return m_MPBonus; }
	void setMPBonus(MP_t mpBonus) throw() { m_MPBonus = mpBonus; }

	virtual Silver_t getMaxSilver() const throw() { return m_MaxSilver; }
	virtual void setMaxSilver(Silver_t amount) throw() { m_MaxSilver = amount; }

	virtual Speed_t getSpeed(void) const throw() { return m_Speed; }
	virtual void setSpeed(Speed_t speed) throw() { m_Speed = speed; }

	virtual uint getItemLevel(void) const throw() { return m_ItemLevel; }
	virtual void setItemLevel(uint level) throw() { m_ItemLevel = level; }

	virtual int getCriticalBonus(void) const throw() { return m_CriticalBonus; }
	virtual void setCriticalBonus(int bonus) throw() { m_CriticalBonus = bonus; }

	virtual string toString() const throw();

private:
	Durability_t   m_Durability;
	Damage_t       m_MinDamage;
	Damage_t       m_MaxDamage;
	MP_t           m_MPBonus;
	Silver_t       m_MaxSilver;
	Speed_t        m_Speed;
	uint           m_ItemLevel;
	int            m_CriticalBonus; // 아이템마다 다른 크리티컬 확률

};


//////////////////////////////////////////////////////////////////////////////
// class MaceInfoManager;
//////////////////////////////////////////////////////////////////////////////

class MaceInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_MACE; }
	virtual void load() throw(Error);

};

extern MaceInfoManager* g_pMaceInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class MaceFactory
//////////////////////////////////////////////////////////////////////////////

class MaceFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_MACE; }
	virtual string getItemClassName() const throw() { return "Mace"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new Mace(ItemType,OptionType); }

};


//////////////////////////////////////////////////////////////////////////////
// class MaceLoader;
//////////////////////////////////////////////////////////////////////////////

class MaceLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_MACE; }
	virtual string getItemClassName() const throw() { return "Mace"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);

};

extern MaceLoader* g_pMaceLoader;

#endif
