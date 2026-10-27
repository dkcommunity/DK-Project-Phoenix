//////////////////////////////////////////////////////////////////////////////
// Filename    : PetEnchantItem.h
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __PET_ENCHANT_ITEM_H__
#define __PET_ENCHANT_ITEM_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"
#include "types/PetTypes.h"

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItem;
//////////////////////////////////////////////////////////////////////////////

class PetEnchantItem : public Item 
{
public:
	PetEnchantItem() throw();
	PetEnchantItem(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_PET_ENCHANT_ITEM; }
	virtual string getObjectTableName() const throw() { return "PetEnchantItemObject"; }

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
	ItemNum_t m_Num;
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItemInfo
//////////////////////////////////////////////////////////////////////////////

class PetEnchantItemInfo : public ItemInfo 
{
public:
	enum PetEnchantFunctions
	{
		ENCHANT_FUNCTION = 0,
		SPEC_OPTION_FUNCTION = 1,
		ADDED_OPTION_FUNCTION = 2,	//1204 wlzzi - 추가 옵션 인챈트
		ADDED_OPTION_FUNCTION_PET3 = 3,	//grade 3 new pet.
		REVIVAL_FUNCTION = 13,
		RARE_FUNCTION = 14,
		MUTANT_FUNCTION = 15,
		POLYMORPH_FUNCTION = 21,
		CLEAR_FIRST_ATTRIBUTE_FUNCTION = 22,
		MAGIC_ENCHANT_FUNCTION = 24,
		EXPERT_PET_CHANGER_ENCHANT_FUNCTION = 25,	//081202 wlzzi - 본 서버에 적용.
		ENCHANT_SKILL_FUNCTION = 26,
		
//081001 smallheart - 루펭 이후부터, 추가된(될) 공용펫들에 대한 (공용)인젝션 Function사용
//		- 모두 28번 Function Type을 사용하도록 수정한다.
//		- DB::PetEnchantItemInfo::[Function]컬럼은 아래 28번 으로 ~
//		- DB::PetEnchantItemInfo::[FunctionGrade]컬럼에 인젝션될 PetType을....
		COMMON_PET_INJECTION = 28,
/*		LUPENG_FUNCTION = 28,
		//2007.11.20 kim sung yong
		MILITARY_BEAR_FUNCTION = 29,
		//2007.11.20 kim sung yong
		GARGOYLE_FUNCTION = 30,
		FIRE_ELEMENTAL_FUNCTION = 31,*/
	};

	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_PET_ENCHANT_ITEM; }
	virtual string toString() const throw();

	BYTE getFunction() const { return m_Function; }
	void setFunction( BYTE Function ) { m_Function = Function; }

	BYTE getFunctionGrade() const { return m_FunctionGrade; }
	void setFunctionGrade( BYTE Grade ) { m_FunctionGrade = Grade; }
private :
	BYTE m_Function;
	BYTE m_FunctionGrade;
};

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItemInfoManager;
//////////////////////////////////////////////////////////////////////////////

class PetEnchantItemInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_PET_ENCHANT_ITEM; }
	virtual void load() throw(Error);
};

extern PetEnchantItemInfoManager* g_pPetEnchantItemInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItemFactory
//////////////////////////////////////////////////////////////////////////////

class PetEnchantItemFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_PET_ENCHANT_ITEM; }
	virtual string getItemClassName() const throw() { return "PetEnchantItem"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new PetEnchantItem(ItemType,OptionType,1); }
};

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantItemLoader;
//////////////////////////////////////////////////////////////////////////////

class PetEnchantItemLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_PET_ENCHANT_ITEM; }
	virtual string getItemClassName() const throw() { return "PetEnchantItem"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern PetEnchantItemLoader* g_pPetEnchantItemLoader;

#endif
