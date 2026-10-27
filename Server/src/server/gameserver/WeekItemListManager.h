#ifndef _WEEKITEMLIST_MANAGER_H__
#define _WEEKITEMLIST_MANAGER_H__

#include "Types.h"

#include "Item.h"
#include <map>

class PlayerCreature;

class WeekItemElement
{
public:
	WeekItemElement(Item::ItemClass IClass, ItemType_t ItemType)
	{
		m_ItemClass = IClass;
		m_ItemType = ItemType;
		
		m_Grade = 1;
		m_Num = 1;
		m_LimitTime = 0;
	}
	
	int GetID() { return m_ID; }
	void SetID(int ID) { m_ID = ID; }
	
	Item::ItemClass GetItemClass() { return m_ItemClass; }
	ItemType_t GetItemType() { return m_ItemType; }
	
	Grade_t GetGrade() { return m_Grade; }
	void SetGrade(Grade_t Grade) { m_Grade = Grade; }
	
	list<OptionType_t>& GetOptionType() { return m_optionType; }
	void SetOptionType(list<OptionType_t>& optionType) { m_optionType = optionType; }
	
	ItemNum_t GetNum() { return m_Num; }
	void SetNum(ItemNum_t Num) { m_Num = Num; }
	
	DWORD GetLimitTime() { return m_LimitTime; }
	void SetLimitTime(DWORD LimitTime) { m_LimitTime = LimitTime; }

	Durability_t GetDurability() const { return m_Durability; }
	void SetDurability(Durability_t Durability) { m_Durability = Durability; }
	
	friend class WeekItemListManager;

private:
	int m_ID;
	Item::ItemClass m_ItemClass;
	ItemType_t m_ItemType;
	Grade_t				m_Grade;
	list<OptionType_t>  m_optionType;
	ItemNum_t      		m_Num;
	DWORD				m_LimitTime;
	Durability_t		m_Durability;
};

typedef std::map<ObjectID_t, WeekItemElement*> TWeekItemElement;

class WeekItemListManager
{
public:
	WeekItemListManager( PlayerCreature* pOwner ) : m_pOwner(pOwner) {  }
	~WeekItemListManager();

	void load() throw(Error);
	
	TWeekItemElement::iterator beginIterator() { return m_WeekItemList.begin(); }
	TWeekItemElement::iterator endIterator() { return m_WeekItemList.end(); }
	
	WeekItemElement* GetWeekItemElement(int ID);
	void RemoveWeekItemElement(int ID);
	
	Item* CreateWeekItemByID(int ID);
	
	int GetFrontWeekItemID();
	int GetWeekItemCount();
	
	void GenerateWeekItem();

private:
	PlayerCreature* m_pOwner;
	TWeekItemElement m_WeekItemList;
};

#endif
