//////////////////////////////////////////////////////////////////////////////
// Filename    : EtherealChain.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ETHEREAL_CHAIN_H__
#define __ETHEREAL_CHAIN_H__

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
// class EtherealChain;
//////////////////////////////////////////////////////////////////////////////

class EtherealChain : public ConcreteItem<Item::ITEM_CLASS_ETHEREAL_CHAIN, NoStack, NoDurability, HasOption, HasGrade, NoAttacking, NoEnchantLevel>
{
public:
	EtherealChain() throw();
	EtherealChain(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
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
// class EtherealChainInfo
//////////////////////////////////////////////////////////////////////////////

class EtherealChainInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_ETHEREAL_CHAIN; }
	
	virtual string toString() const throw();

private:
};


//////////////////////////////////////////////////////////////////////////////
// class EtherealChainInfoManager;
//////////////////////////////////////////////////////////////////////////////

class EtherealChainInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_ETHEREAL_CHAIN; }
	virtual void load() throw(Error);
};

// global variable declaration
extern EtherealChainInfoManager* g_pEtherealChainInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class EtherealChainFactory
//////////////////////////////////////////////////////////////////////////////

class EtherealChainFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_ETHEREAL_CHAIN; }
	virtual string getItemClassName() const throw() { return "EtherealChain"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new EtherealChain(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class EtherealChainLoader;
//////////////////////////////////////////////////////////////////////////////

class EtherealChainLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_ETHEREAL_CHAIN; }
	virtual string getItemClassName() const throw() { return "EtherealChain"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern EtherealChainLoader* g_pEtherealChainLoader;
#endif
