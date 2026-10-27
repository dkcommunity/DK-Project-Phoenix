//////////////////////////////////////////////////////////////////////////////
// Filename    : CallNPCCard.h
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CALLNPC_CARD_H__
#define __CALLNPC_CARD_H__

#include "Item.h"
#include "ItemInfo.h"
#include "InfoClassManager.h"
#include "ItemFactory.h"
#include "ItemLoader.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCard;
//////////////////////////////////////////////////////////////////////////////

class CallNPCCard : public Item 
{
public:
	CallNPCCard() throw();
	CallNPCCard(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num) throw();
	
public:
	virtual void create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID=0) throw(Error);
	virtual void save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) throw(Error);
	
	virtual string toString() const throw();

	static void initItemIDRegistry(void) throw();

public:
	virtual ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CALLNPC_CARD; }
	virtual string getObjectTableName() const throw() { return "CallNPCCardObject"; }

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
	
	static Mutex    m_Mutex;          // 아이템 ID 관련 락
	static ItemID_t m_ItemIDRegistry; // 클래스별 고유 아이템 아이디 발급기
};

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCardInfo
//////////////////////////////////////////////////////////////////////////////

class CallNPCCardInfo : public ItemInfo 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CALLNPC_CARD; }
	virtual string toString() const throw();

	virtual NPCID_t getCallNPCID() const throw() { return m_CallNPCID; }
	virtual void setCallNPCID(NPCID_t CallNPCID) throw() { m_CallNPCID = CallNPCID; }
	
	virtual int getScriptType() const throw() { return m_ScriptType; }
	virtual void setScriptType(int ScriptType) throw() { m_ScriptType = ScriptType; }
	
	NPCID_t m_CallNPCID;
	int m_ScriptType;
};

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCardInfoManager;
//////////////////////////////////////////////////////////////////////////////

class CallNPCCardInfoManager : public InfoClassManager 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CALLNPC_CARD; }
	virtual void load() throw(Error);
};

extern CallNPCCardInfoManager* g_pCallNPCCardInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCardFactory
//////////////////////////////////////////////////////////////////////////////

class CallNPCCardFactory : public ItemFactory 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CALLNPC_CARD; }
	virtual string getItemClassName() const throw() { return "CallNPCCard"; }
	
public:
	virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) throw() { return new CallNPCCard(ItemType,OptionType,1); }
};

//////////////////////////////////////////////////////////////////////////////
// class CallNPCCardLoader;
//////////////////////////////////////////////////////////////////////////////

class CallNPCCardLoader : public ItemLoader 
{
public:
	virtual Item::ItemClass getItemClass() const throw() { return Item::ITEM_CLASS_CALLNPC_CARD; }
	virtual string getItemClassName() const throw() { return "CallNPCCard"; }

public:
	virtual void load(Creature* pCreature) throw(Error);
	virtual void load(Zone* pZone) throw(Error);
	virtual void load(StorageID_t storageID, Inventory* pInventory) throw(Error);
};

extern CallNPCCardLoader* g_pCallNPCCardLoader;

#endif
