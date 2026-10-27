//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillBook.h
// Written By  : Shinobi
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BOOK_H__
#define __SKILL_BOOK_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class SkillBook;
//////////////////////////////////////////////////////////////////////////////

class SkillBook : public Item 
{
public:
	SkillBook() throw();
	SkillBook(ItemType_t itemType, const list<OptionType_t>& optionType) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SKILL_BOOK; }
	virtual string getObjectTableName() const throw() { return "SkillBookObject"; }

	virtual ItemType_t getItemType() const throw() { return m_ItemType; }
	virtual void setItemType(ItemType_t itemType) throw() { m_ItemType = itemType; }

	virtual VolumeWidth_t getVolumeWidth() const throw(Error);
	virtual VolumeHeight_t getVolumeHeight() const throw(Error);
	virtual Weight_t getWeight() const throw(Error);

	virtual string toString() const throw();

public:
	bool    isStackable() const throw() { return false; }

private:
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기

	ItemType_t m_ItemType;					// 아이템 타입
};


//--------------------------------------------------------------------------------
// 
// class SkillBookInfo
// 
//--------------------------------------------------------------------------------

class SkillBookInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SKILL_BOOK; }

	virtual string toString() const throw();

	void	setSkillType(SkillType_t skillType) { m_SkillType = skillType; }
	SkillType_t	getSkillType() const throw() { return m_SkillType; }
	void	setSkillDomainType(SkillDomainType_t skillDomainType) { m_SkillDomainType = skillDomainType; }
	SkillDomainType_t	getSkillDomainType() const throw() { return m_SkillDomainType; }
	void	setRace(Race_t race)	{ m_Race = race; }
	Race_t	getRace() const throw() { return m_Race; }

	Attr_t	getNeedINT() { return m_INT; }
	void	setNeedINT(Attr_t attr) { m_INT = attr; }

	Attr_t	getNeedDEX() { return m_DEX; }
	void	setNeedDEX(Attr_t attr) { m_DEX = attr; }

	void	parseNeedAttribute(const string& needattribute);
	
private:
	SkillType_t m_SkillType;				// 스킬 타입
	SkillDomainType_t m_SkillDomainType;	// 스킬 도메인 타입
	Race_t m_Race;							// 종족 타입
	Attr_t	m_INT;							// 배우기 위한 조건
	Attr_t	m_DEX;							// 20080429 DEX 추가
};


//--------------------------------------------------------------------------------
//
// class SkillBookInfoManager;
//
//--------------------------------------------------------------------------------
class SkillBookInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SKILL_BOOK; }
	virtual void load() throw(Error);

};

// global variable declaration
extern SkillBookInfoManager* g_pSkillBookInfoManager;


//--------------------------------------------------------------------------------
//
// class SkillBookFactory
//
//--------------------------------------------------------------------------------
class SkillBookFactory : public ItemFactory {

public:
	
	// get item class
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SKILL_BOOK; }

	// get item classname
	virtual string getItemClassName() const throw() { return "SkillBook"; }
	
public:

	// create item
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new SkillBook(ItemType,OptionType); }

};


//--------------------------------------------------------------------------------
//
// class SkillBookLoader;
//
//--------------------------------------------------------------------------------

class SkillBookLoader : public ItemLoader {

public:

	// get item class
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_SKILL_BOOK; }

	// get item class name
	virtual string getItemClassName() const throw() { return "SkillBook"; }

public:

	// load to creature
	virtual void load(Creature* pCreature) throw(Error);

	// load to zone
	virtual void load(Zone* pZone) throw(Error);

	// load to inventory
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);

};

extern SkillBookLoader* g_pSkillBookLoader;
#endif
