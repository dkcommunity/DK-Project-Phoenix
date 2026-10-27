//////////////////////////////////////////////////////////////////////////////
// Filename    : CueOfAdam.h
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CUE_OF_ADAM_H__
#define __CUE_OF_ADAM_H__

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
// class CueOfAdam;
//////////////////////////////////////////////////////////////////////////////

class CueOfAdam : public ConcreteItem<Item::ITEM_CLASS_CUE_OF_ADAM, NoStack, NoDurability, HasOption, HasGrade, NoAttacking, NoEnchantLevel>
{
public:
	CueOfAdam() throw();
	CueOfAdam(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
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
// class CueOfAdamInfo
//////////////////////////////////////////////////////////////////////////////

class CueOfAdamInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CUE_OF_ADAM; }
	
	OptionClass	getOptionClass() const { return m_OptionClass; }
	void		setOptionClass(OptionClass oClass) { m_OptionClass = oClass; }
	
	OptionType_t	getSetOptionType() const { return m_SetOptionType; }
	void			setSetOptionType( const string OptionString ) throw();

	virtual string toString() const throw();

private:
	OptionClass		m_OptionClass;
	OptionType_t	m_SetOptionType;
};


//////////////////////////////////////////////////////////////////////////////
// class CueOfAdamInfoManager;
//////////////////////////////////////////////////////////////////////////////

class CueOfAdamInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CUE_OF_ADAM; }
	virtual void load() throw(Error);
};

// global variable declaration
extern CueOfAdamInfoManager* g_pCueOfAdamInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class CueOfAdamFactory
//////////////////////////////////////////////////////////////////////////////

class CueOfAdamFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CUE_OF_ADAM; }
	virtual string getItemClassName() const throw() { return "CueOfAdam"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new CueOfAdam(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class CueOfAdamLoader;
//////////////////////////////////////////////////////////////////////////////

class CueOfAdamLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CUE_OF_ADAM; }
	virtual string getItemClassName() const throw() { return "CueOfAdam"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern CueOfAdamLoader* g_pCueOfAdamLoader;
#endif
