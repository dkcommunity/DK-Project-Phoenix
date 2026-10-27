//////////////////////////////////////////////////////////////////////////////
// Filename    : PCItemInfo.h
// Written By  : elca
// Description :
// 아이템 하나에 대한 정보를 가지고 있는 패킷이다.
// 좌표라든지, 이펙트에 관한 정보는 가지고 있지 않다.
// 인벤토리와 관련된 여러 군데에서 상속되어 사용된다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __SLAYER_ITEM_INFO_H__
#define __SLAYER_ITEM_INFO_H__

#include "Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "SubItemInfo.h"
#include "DarkAssert.h"

//////////////////////////////////////////////////////////////////////////////
// class PCItemInfo
//////////////////////////////////////////////////////////////////////////////

class PCItemInfo 
{
public:
	PCItemInfo() ;
	virtual ~PCItemInfo() ;

public:
	void read (SocketInputStream & iStream) ;
	void write (SocketOutputStream & oStream) const ;

#ifdef __DEBUG_OUTPUT__
	std::string toString() const ;
#endif

public:
	void setObjectID(ObjectID_t ObjectID)  { m_ObjectID = ObjectID; }
	ObjectID_t getObjectID() const  { return m_ObjectID; }

	void setItemClass (BYTE IClass)  { m_IClass = IClass; }
	BYTE getItemClass () const  { return m_IClass; }

	void setItemType (ItemType_t ItemType)  { m_ItemType = ItemType; }
	ItemType_t getItemType() const  { return m_ItemType; }

	void addOptionType(OptionType_t OptionType)  { m_OptionType.push_back( OptionType ); }
	void setOptionType(const std::list<OptionType_t>& OptionType)  { m_OptionType = OptionType; }
	int getOptionTypeSize() const  { return m_OptionType.size(); }
	const std::list<OptionType_t>& getOptionType() const  { return m_OptionType; }
	OptionType_t popOptionType()  
	{ 
		if (m_OptionType.empty()) return 0;
		OptionType_t optionType = m_OptionType.front(); 
		m_OptionType.pop_front(); 
		return optionType; 
	}

	///////////////////////////맣芚溝固///////////////////////////////////
	void addGTwoOptionType(OptionType_t OptionType)  { m_GTwoOptionType.push_back( OptionType ); }
	void setGTwoOptionType(const std::list<OptionType_t>& OptionType)  { m_GTwoOptionType = OptionType; }
	int getGTwoOptionTypeSize() const  { return m_GTwoOptionType.size(); }
	const std::list<OptionType_t>& getGTwoOptionType() const  { return m_GTwoOptionType; }
	OptionType_t popGTwoOptionType()  
	{ 
		if (m_GTwoOptionType.empty()) return 0;
		OptionType_t optionType = m_GTwoOptionType.front(); 
		m_GTwoOptionType.pop_front(); 
		return optionType; 
	}

	void setSilver(Silver_t amount)  { m_Silver = amount; }
	Silver_t getSilver() const  { return m_Silver; }

	void setDurability(Durability_t Durability)  { m_Durability = Durability; }
	Durability_t getDurability() const  { return m_Durability; }

	void setGrade(Grade_t grade)  { m_Grade = grade; }
	Grade_t getGrade() const  { return m_Grade; }

	void setEnchantLevel(EnchantLevel_t level)  { m_EnchantLevel = level; }
	EnchantLevel_t getEnchantLevel() const  { return m_EnchantLevel; }

	void setItemNum(ItemNum_t ItemNum)  { m_ItemNum = ItemNum; }
	ItemNum_t getItemNum() const  { return m_ItemNum; }

	void setMainColor(WORD MainColor)  { m_MainColor = MainColor; }
	WORD getMainColor() const  { return m_MainColor; }

	BYTE getListNum() const  { return m_ListNum; }
	void setListNum(BYTE ListNum)  { m_ListNum = ListNum; }

	std::list<SubItemInfo*>&getSubItems()   {return m_SubItemInfoList; }
	
	void addListElement(SubItemInfo* pSubItemInfo)  
	{ 
		m_SubItemInfoList.push_back(pSubItemInfo); 
		m_ListNum++;
	}

	void clearList()  { m_SubItemInfoList.clear(); m_ListNum = 0; }

	SubItemInfo* popFrontListElement()  
	{ 
		SubItemInfo* TempSubItemInfo = m_SubItemInfoList.front(); 
		m_SubItemInfoList.pop_front(); 
		return TempSubItemInfo; 
	}
	void addThirdOptionType(OptionType_t ThirdOptionType)  { m_OptionType.push_back( ThirdOptionType ); }
	void setThirdOptionType(const std::list<OptionType_t>& ThirdOptionType)  { m_ThirdOptionType = ThirdOptionType; }
	int getThirdOptionTypeSize() const  { return m_ThirdOptionType.size(); }
	const std::list<OptionType_t>& getThirdOptionType() const  { return m_ThirdOptionType; }
	OptionType_t popThirdOptionType()  
	{ 
		if (m_ThirdOptionType.empty()) return 0;
		OptionType_t ThirdoptionType = m_ThirdOptionType.front(); 
		m_ThirdOptionType.pop_front(); 
		return ThirdoptionType; 
	}

	BYTE getThirdEnchantType() const 				{ return m_ThirdEnchantType; }
	void setThirdEnchantType(BYTE ThirdEnchantType)	{m_ThirdEnchantType = ThirdEnchantType; }

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	void setCashItem(BYTE cashItem) 					{m_CashItem = cashItem;}
	BYTE getCashItem()										{return m_CashItem;}
#endif

#if __CONTENTS(__PET_MIXINGFORGE)
	void						setMixOptionType(const std::list<OptionType_t>& mixOptionType)  { m_MixOptionType = mixOptionType; }
	int							getMixOptionTypeSize() const  { return m_MixOptionType.size(); }
	const std::list<OptionType_t>&	getMixOptionType() const  { return m_MixOptionType; }
	OptionType_t				getMixFirstOptionType() const  { if (m_MixOptionType.empty()) return 0; return m_MixOptionType.front(); }
	void						removeMixOptionType(OptionType_t mixOptionType)  { std::list<OptionType_t>::iterator itr = std::find(m_MixOptionType.begin(), m_MixOptionType.end(), mixOptionType); if (itr!=m_MixOptionType.end()) m_MixOptionType.erase(itr); }
	void						addMixOptionType(OptionType_t mixOptionType)  { m_MixOptionType.push_back(mixOptionType); }
#endif //__PET_MIXINGFORGE

#if __CONTENTS(__PET_VISION_AMPLE)
	// 1208 wlzzi - 여러가지 용도로 사용할 목적으로 이름에 Pet를 붙이지 않았다.
	// 현재목적은 인벤토리상에 아이템에 펫아이템에만 있는 레드 버드로 인첸트한옵션인지
	// 비젼엠플로 인첸트한 옵션인지를 알수없기때문에 setpetinfo() 를 하지않는대신 아이템 Info 에 직접 이정보를 넣어주었다.
	void setAddedInfo1(BYTE idx) 	{ m_AddedInfo1 = idx; }
	BYTE getAddedInfo1() const 		{ return m_AddedInfo1; }
	void setAddedInfo2(BYTE idx) 	{ m_AddedInfo2 = idx; }
	BYTE getAddedInfo2() const 		{ return m_AddedInfo2; }
	void setAddedInfo3(BYTE idx) 	{ m_AddedInfo3 = idx; }
	BYTE getAddedInfo3() const 		{ return m_AddedInfo3; }
#endif // __PET_VISION_AMPLE

public:
	uint getSize() const 
	{
		return szObjectID +
			szBYTE +
			szItemType +
			szBYTE + m_OptionType.size() * szOptionType +
			szBYTE + m_GTwoOptionType.size() * szOptionType +
			szDurability +
			szSilver +
			szGrade +
			szEnchantLevel +
			szItemNum +
			szWORD +
			szBYTE +
			SubItemInfo::getMaxSize()*m_ListNum
#if __CONTENTS(__PET_VISION_AMPLE)
			//1208 wlzzi
			+ szBYTE * 3 //by rallser 녘膠3橄昑
#endif //__PET_VISION_AMPLE
			+ szBYTE + m_ThirdOptionType.size() * szOptionType +
			szBYTE
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
			+ szBYTE
#endif
#if __CONTENTS(__PET_MIXINGFORGE)
			+ szBYTE + m_MixOptionType.size() * szOptionType //mixOptionType추가
#endif //__PET_MIXINGFORGE
			;
	}

	static uint getMaxSize() 
	{
		return szObjectID +
			szBYTE +
			szItemType +
			szBYTE + 255 * szOptionType +
			szBYTE + 255 * szOptionType +
			szDurability +
			szSilver +
			szGrade +
			szEnchantLevel +
			szItemNum +
			szWORD +
			szBYTE +
			SubItemInfo::getMaxSize()*8
#if __CONTENTS(__PET_VISION_AMPLE)
			//1208 wlzzi
			+ szBYTE * 3//by rallser 녘膠3橄昑
#endif //__PET_VISION_AMPLE
			+ szBYTE + 255 * szOptionType +
			szBYTE
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
			+szBYTE
#endif
#if __CONTENTS(__PET_MIXINGFORGE)
			+ szBYTE + 10 * szOptionType;	//OptionType은 최대 10개까지만.. 들어갈 수 있다. //mixOptionType추가
#endif //__PET_MIXINGFORGE
			;

	}


protected:
	ObjectID_t         m_ObjectID;         // item object id
	BYTE               m_IClass;           // item class
	ItemType_t         m_ItemType;         // item type
	std::list<OptionType_t> m_OptionType;       // item option type

	std::list<OptionType_t> m_GTwoOptionType;       // 맣芚

	Durability_t       m_Durability;       // item durability
	Silver_t           m_Silver;           // silver coating amount
	Grade_t		       m_Grade;            // item grade
	EnchantLevel_t     m_EnchantLevel;     // item enchant level
	ItemNum_t          m_ItemNum;          // number of item
	WORD               m_MainColor;        // item color
	BYTE               m_ListNum;          // number of sub item
	std::list<SubItemInfo*> m_SubItemInfoList;  // actual sub item info
//20071009 by diesirace 서드아이템옵션 추가
	std::list<OptionType_t> m_ThirdOptionType;      // item Third option type
	BYTE				m_ThirdEnchantType;			// Third Enchant type
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	BYTE				m_CashItem;			// this item is cash
#endif

#if __CONTENTS(__PET_VISION_AMPLE)
	// 1208 wlzzi - PetOption의 indexing정보 추가
	//	- PetOption2가 추가되면서 필요해 졌음. -_-;;
	//	- 아이템 클레스 별로 추가적인 정보의 전달이 필요할 때 공용으로 사용하도록 .. 네이밍 한다.
	BYTE			   m_AddedInfo1;
	BYTE			   m_AddedInfo2;
	BYTE			   m_AddedInfo3;
#endif //__PET_VISION_AMPLE
#if __CONTENTS(__PET_MIXINGFORGE)
	std::list<OptionType_t>	m_MixOptionType;
#endif //__PET_MIXINGFORGE
};

#endif
