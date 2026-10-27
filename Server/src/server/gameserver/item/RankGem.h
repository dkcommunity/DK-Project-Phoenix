//////////////////////////////////////////////////////////////////////////////
// Filename    : RankGem.h
// Written By  : rallser
// Description : 2014-10-17
//////////////////////////////////////////////////////////////////////////////

#ifndef __RANK_GEM_H__
#define __RANK_GEM_H__

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
// class RankGem;
//////////////////////////////////////////////////////////////////////////////

class RankGem : public ConcreteItem<Item::ITEM_CLASS_RANK_GEM, NoStack, NoDurability, HasOption, HasGrade, NoAttacking, NoEnchantLevel>
{
public:
	RankGem() throw();
	RankGem(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

private:
	
	static Mutex    m_Mutex;          // 酒捞袍 ID 包访 遏
	static ItemID_t m_ItemIDRegistry; // 努贰胶喊 绊蜡 酒捞袍 酒捞叼 惯鞭扁
};


//////////////////////////////////////////////////////////////////////////////
// class RankGemInfo
//////////////////////////////////////////////////////////////////////////////

class RankGemInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RANK_GEM; }
	
	OptionClass	getOptionClass() const { return m_OptionClass; }
	void		setOptionClass(OptionClass oClass) { m_OptionClass = oClass; }
	
	OptionType_t	getSetOptionType() const { return m_SetOptionType; }
	void		setSetOptionType( const string OptionString ) throw();

	Race_t	getRace() const { return m_Race; }
	void	setRace(Race_t Race) { m_Race = Race; }

	virtual string toString() const throw();

private:
	OptionClass		m_OptionClass;
	OptionType_t	m_SetOptionType;
	Race_t	m_Race;
};


//////////////////////////////////////////////////////////////////////////////
// class RankGemInfoManager;
//////////////////////////////////////////////////////////////////////////////

class RankGemInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RANK_GEM; }
	virtual void load() throw(Error);
};

// global variable declaration
extern RankGemInfoManager* g_pRankGemInfoManager;


//////////////////////////////////////////////////////////////////////////////
// class RankGemFactory
//////////////////////////////////////////////////////////////////////////////

class RankGemFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RANK_GEM; }
	virtual string getItemClassName() const throw() { return "RankGem"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new RankGem(ItemType,OptionType); }
};


//////////////////////////////////////////////////////////////////////////////
// class RankGemLoader;
//////////////////////////////////////////////////////////////////////////////

class RankGemLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_RANK_GEM; }
	virtual string getItemClassName() const throw() { return "RankGem"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern RankGemLoader* g_pRankGemLoader;
#endif
