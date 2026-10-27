//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToZone.h 
// Written By  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_ITEM_TO_ZONE_H__
#define __GC_ADD_ITEM_TO_ZONE_H__

#include "Packet.h"
#include "PacketFactory.h"
#include <list>
#if __CONTENTS(__PCITEMINFO)
#include "PCItemInfo.h"
#else
#include "SubItemInfo.h"
#endif //__PCITEMINFO
//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToZone;
//////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__PCITEMINFO)

class GCAddItemToZone : public Packet 
{
public:
	GCAddItemToZone() ;
	~GCAddItemToZone() ;

public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	PacketSize_t getPacketSize() const  
	{ 
		PacketSize_t rValue = 0;

		rValue += szCoord*2;                        // item coord in inventory
		rValue += m_PCItemInfo.getSize();

		return rValue;
	}

public :

	// get/set X
	Coord_t getX() const  { return m_X; }
	void setX(Coord_t x)  { m_X = x; }
	
	// get/set Y
	Coord_t getY() const  { return m_Y; }
	void setY(Coord_t y)  { m_Y = y; }
	
	void SetPCItemInfo(PCItemInfo& pcItemInfo) { m_PCItemInfo = pcItemInfo; }
	PCItemInfo* GetPCItemInfo() { return &m_PCItemInfo; }
	

protected :

	// Zone의 X, Y 좌표
   	Coord_t m_X;
	Coord_t m_Y;
	
	PCItemInfo m_PCItemInfo;	

};

#else
class GCAddItemToZone : public Packet 
{
public:
	GCAddItemToZone() ;
	~GCAddItemToZone() ;

public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	PacketSize_t getPacketSize() const  
	{ 
		PacketSize_t rValue = 0;

		rValue += szObjectID;                       // item object id
		rValue += szCoord*2;                        // item coord in inventory
		rValue += szBYTE;                           // item class
		rValue += szItemType;                       // item type
		rValue += szBYTE + m_OptionType.size();     // item option type
		rValue += szSilver;                         // silver
		rValue += szGrade;                          // grade
		rValue += szDurability;                     // item durability
		rValue += szEnchantLevel;                   // enchant level
		rValue += szItemNum;                        // number of item
		rValue += szBYTE;                           // number of sub item
		rValue += SubItemInfo::getSize()*m_ListNum; // actual sub item info

		return rValue;
	}

public :

	// get/set creature ID 
	ObjectID_t getObjectID() const  { return m_ObjectID; }
	void setObjectID(ObjectID_t creatureID)  { m_ObjectID = creatureID; }

	// get/set X
	Coord_t getX() const  { return m_X; }
	void setX(Coord_t x)  { m_X = x; }
	
	// get/set Y
	Coord_t getY() const  { return m_Y; }
	void setY(Coord_t y)  { m_Y = y; }

	// get / set ItemClass
	BYTE getItemClass() const  { return m_ItemClass; }
	void setItemClass(BYTE Class)  { m_ItemClass = Class; }

	// get / set ItemType
	ItemType_t getItemType() const  { return m_ItemType; }
	void setItemType(ItemType_t ItemType)  { m_ItemType = ItemType; }

	// get / set OptionType
	OptionType_t popOptionType() 
	{
		if (m_OptionType.empty()) return 0;
		OptionType_t optionType = m_OptionType.front();
		m_OptionType.pop_front();
		return optionType;
	}
	int getOptionTypeSize() const  { return m_OptionType.size(); }
	void addOptionType(OptionType_t OptionType)  { m_OptionType.push_back( OptionType ); }
	void setOptionType(const std::list<OptionType_t>& OptionTypes)  { m_OptionType = OptionTypes; }
	const std::list<OptionType_t>& getOptionType() const  { return m_OptionType; }

	// get / set Silver
	Silver_t getSilver() const  { return m_Silver; }
	void setSilver(Silver_t Silver)  { m_Silver = Silver; }

	// get / set Grade
	Grade_t getGrade() const  { return m_Grade; }
	void setGrade(Grade_t Grade)  { m_Grade = Grade; }

	// get / set enchant level
	EnchantLevel_t getEnchantLevel() const  { return m_EnchantLevel; }
	void setEnchantLevel(EnchantLevel_t level)  { m_EnchantLevel = level; }

	// get / set Durability
	Durability_t getDurability() const  { return m_Durability; }
	void setDurability(Durability_t Durability)  { m_Durability = Durability; }

	// get / set ItemNum
	ItemNum_t getItemNum() const  { return m_ItemNum; }
	void setItemNum(ItemNum_t ItemNum)  { m_ItemNum = ItemNum; }

	// get / set ListNumber
	BYTE getListNum() const  { return m_ListNum; }
	void setListNum(BYTE ListNum)  { m_ListNum = ListNum; }

	// add / delete / clear S List
	void addListElement(SubItemInfo* pSubItemInfo)  { m_SubItemInfoList.push_back(pSubItemInfo); }

	// ClearList
	void clearList()  { m_SubItemInfoList.clear(); m_ListNum = 0; }

	// pop front Element in Status List
	SubItemInfo* popFrontListElement()  
	{ 
		SubItemInfo* TempSubItemInfo = m_SubItemInfoList.front(); m_SubItemInfoList.pop_front(); return TempSubItemInfo; 
	}

protected :
	
    // Item의 ObjectID
    ObjectID_t m_ObjectID;

	// Zone의 X, Y 좌표
   	Coord_t m_X;
	Coord_t m_Y;

	// Item Class
	BYTE m_ItemClass;

	// ItemType
	ItemType_t m_ItemType;

	// ItemOptionType
	std::list<OptionType_t> m_OptionType;

	// Silvering
	Silver_t m_Silver;

	// Grade
	Grade_t m_Grade;

	// Durability
	Durability_t m_Durability;

	// Enchant level
	EnchantLevel_t m_EnchantLevel;

	// ItemNum
	ItemNum_t m_ItemNum;

	//	// SubItemInfo List Total Number
	BYTE m_ListNum;
	
	// SubItemInfo List
	std::list<SubItemInfo*> m_SubItemInfoList;
	
	//20071010 혹시라도 훗날 몬스터에서 서드 인첸트 된 아이템이 떨어지게 되면 수정해야 함

};
#endif //__PCITEMINFO
#endif //__GC_ADD_ITEM_TO_ZONE_H__
