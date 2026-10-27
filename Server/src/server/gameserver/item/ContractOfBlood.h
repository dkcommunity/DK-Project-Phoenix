//////////////////////////////////////////////////////////////////////////////
// Filename    : ContractOfBlood.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CONTRACT_OF_BLOOD_H__
#define __CONTRACT_OF_BLOOD_H__

#include "Item.h"
#include "ConcreteItem.h"
#include "ItemPolicies.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"
#include "OptionInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class ContractOfBlood;
//////////////////////////////////////////////////////////////////////////////

class ContractOfBlood : public ConcreteItem<Item::ITEM_CLASS_CONTRACT_OF_BLOOD, NoStack, NoDurability, HasOption, HasGrade, NoAttacking, NoEnchantLevel>
{
public:
	ContractOfBlood() throw();
	ContractOfBlood(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
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
// class ContractOfBloodInfo
//////////////////////////////////////////////////////////////////////////////

class ContractOfBloodInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CONTRACT_OF_BLOOD; }
	
	OptionClass	getOptionClass() const { return m_OptionClass; }
	void		setOptionClass(OptionClass oClass) { m_OptionClass = oClass; }
	
	virtual string toString() const throw();

private:
	OptionClass		m_OptionClass;
};


//////////////////////////////////////////////////////////////////////////////
// class ContractOfBloodInfoManager;
//////////////////////////////////////////////////////////////////////////////

class ContractOfBloodInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CONTRACT_OF_BLOOD; }
	virtual void load() throw(Error);
};

// global variable declaration
extern ContractOfBloodInfoManager* g_pContractOfBloodInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class ContractOfBloodFactory
//////////////////////////////////////////////////////////////////////////////

class ContractOfBloodFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CONTRACT_OF_BLOOD; }
	virtual string getItemClassName() const throw() { return "ContractOfBlood"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new ContractOfBlood(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class ContractOfBloodLoader;
//////////////////////////////////////////////////////////////////////////////

class ContractOfBloodLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CONTRACT_OF_BLOOD; }
	virtual string getItemClassName() const throw() { return "ContractOfBlood"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern ContractOfBloodLoader* g_pContractOfBloodLoader;
#endif
