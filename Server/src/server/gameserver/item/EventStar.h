//////////////////////////////////////////////////////////////////////////////
// Filename    : EventStar.h
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_STAR_H__
#define __EVENT_STAR_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class EventStar;
//////////////////////////////////////////////////////////////////////////////

class EventStar : public Item 
{
public:
	EventStar() throw();
	EventStar(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);

	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_EVENT_STAR; }
	virtual string getObjectTableName() const throw() { return "EventStarObject"; }

	virtual ItemType_t getItemType() const throw() { return m_ItemType; }
	virtual void setItemType(ItemType_t itemType) throw() { m_ItemType = itemType; }

	virtual VolumeWidth_t getVolumeWidth() const throw(Error);
	virtual VolumeHeight_t getVolumeHeight() const throw(Error);
	virtual Weight_t getWeight() const throw(Error);

public:
	virtual ItemNum_t getNum() const throw() { return m_Num; }
	virtual void setNum(ItemNum_t Num) throw() { m_Num = Num; }

	bool    isStackable() const throw() { return true; }

private:
	ItemType_t m_ItemType;
	ItemNum_t  m_Num;
	
	static Mutex    m_Mutex;          // ŸÆÀÌÅÛ ID °ü·Ã ¶ô
	static ItemID_t m_ItemIDRegistry; // Å¬·¡œºº° °íÀ¯ ŸÆÀÌÅÛ ŸÆÀÌµð ¹ß±Þ±â
};

//////////////////////////////////////////////////////////////////////////////
// class EventStarInfo
//////////////////////////////////////////////////////////////////////////////

class EventStarInfo : public ItemInfo 
{
public:
	enum ITEM_FUNCTION
	{
		FUNCTION_NULL = 0,
		FUNCTION_ENCHANT_OPTION = 0x01,	// ¿ÉŒÇÀ» Žõ ÁÁ°Ô ¹Ù²ÛŽÙ.
		FUNCTION_ADD_OPTION = 0x02,		// ¿ÉŒÇÀ» Ãß°¡ÇÑŽÙ.
		FUNCTION_ENCHANT_RARE_OPTION = 0x04,	// ·¹Ÿî ŸÆÀÌÅÛÀÇ ¿ÉŒÇÀ» Ÿ÷~ÇÑŽÙ.
		FUNCTION_TRANS_KIT = 0x08,		// ŸÆÅÛ Œºº°À» ¹Ù²ÛŽÙ.
		FUNCTION_UP_GRADE = 0x10,		// ŸÆÀÌÅÛ ±ÞŒöžŠ ¿Ãž°ŽÙ.
		FUNCTION_ADD_RANDOM_OPTION = 0x20,		// 20070725 ÄÚ¶öÃß°¡ ¹«¿É ŸÆÀÌÅÛ¿¡ ¿ÉŒÇÃß°¡ ŒöÄ¡ŽÂ 1-3±îÁö ·£ŽýÀÌŽÙ.
		FUNCTION_ENCHANT_RARE_OPTION_Two = 0x58,//2属性不爆
#ifdef VERSION_THIRD_ENCHANT_1
		FUNCTION_THIRD_ENCHANT_OPTION = 0x40,	// 20071010 THIRD ¿ÉŒÇÀ» ¹Ù²ÛŽÙ.
                FUNCTION_ADD_THREE_RANDOM_OPTION = 0x50,//添加3属性 BY RALLSER 80
#endif
	};

public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_EVENT_STAR; }
	virtual string toString() const throw();

public :
	virtual BYTE getFunctionFlag() const throw() { return m_fFunction; }
	virtual BYTE isFunctionEnchantOption() const { return m_fFunction & FUNCTION_ENCHANT_OPTION; }	// ¿ÉŒÇÀ» Žõ ÁÁ°Ô ¹Ù²ÛŽÙ.
	virtual BYTE isFunctionAddOption() const { return m_fFunction & FUNCTION_ADD_OPTION; }	// ¿ÉŒÇÀ» Ãß°¡ÇÑŽÙ.
	virtual BYTE isFunctionEnchantRareOption() const { return m_fFunction & FUNCTION_ENCHANT_RARE_OPTION; }	// ¿ÉŒÇÀ» Žõ ÁÁ°Ô ¹Ù²ÛŽÙ.
	virtual BYTE isFunctionEnchantRareOptionTwo() const { return m_fFunction & FUNCTION_ENCHANT_RARE_OPTION_Two; }	
	virtual BYTE isFunctionTransKit() const { return m_fFunction & FUNCTION_TRANS_KIT; }
// 20070725 ÄÚ¶öÃß°¡
	virtual BYTE isFunctionAddRandomOption() const { return m_fFunction & FUNCTION_ADD_RANDOM_OPTION; }
	virtual BYTE isFunctionAddThreeRandomOption() const { return m_fFunction & FUNCTION_ADD_THREE_RANDOM_OPTION; }//添加3属性 BY RALLSER
	virtual void setFunctionFlag(BYTE flag) throw() { m_fFunction = flag; }
#ifdef VERSION_THIRD_ENCHANT_1
	virtual BYTE isFunctionThirdEnchantOption() const { return m_fFunction & FUNCTION_THIRD_ENCHANT_OPTION; }
#endif
	virtual int getFunctionValue() const throw() { return m_FunctionValue; }
	virtual void setFunctionValue(int value) throw() { m_FunctionValue = value; }


	BYTE	   m_fFunction;		// ±âŽÉ
	int        m_FunctionValue;	// ±âŽÉ°ú °ü·ÃµÈ °ª
};

//////////////////////////////////////////////////////////////////////////////
// class EventStarInfoManager;
//////////////////////////////////////////////////////////////////////////////

class EventStarInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_EVENT_STAR; }
	virtual void load() throw(Error);
};

extern EventStarInfoManager* g_pEventStarInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class EventStarFactory
//////////////////////////////////////////////////////////////////////////////

class EventStarFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_EVENT_STAR; }
	virtual string getItemClassName() const throw() { return "EventStar"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new EventStar(ItemType,OptionType,1); }
};

//////////////////////////////////////////////////////////////////////////////
// class EventStarLoader;
//////////////////////////////////////////////////////////////////////////////

class EventStarLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_EVENT_STAR; }
	virtual string getItemClassName() const throw() { return "EventStar"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern EventStarLoader* g_pEventStarLoader;

#endif
