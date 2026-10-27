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

#include "Assert.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "SubItemInfo.h"
#include "Types.h"
#include <algorithm>
#include <list>

//////////////////////////////////////////////////////////////////////////////
// class PCItemInfo
//////////////////////////////////////////////////////////////////////////////

class PCItemInfo {
public:
  PCItemInfo() throw();
  virtual ~PCItemInfo() throw();

public:
  void read(SocketInputStream &iStream) throw(ProtocolException, Error);
  void write(SocketOutputStream &oStream) const throw(ProtocolException, Error);

  string toString() const throw();

public:
  void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }
  ObjectID_t getObjectID() const throw() { return m_ObjectID; }

  void setItemClass(BYTE IClass) throw() { m_IClass = IClass; }
  BYTE getItemClass() const throw() { return m_IClass; }

  void setItemType(ItemType_t ItemType) throw() { m_ItemType = ItemType; }
  ItemType_t getItemType() const throw() { return m_ItemType; }

  void addOptionType(OptionType_t OptionType) throw() {
    m_OptionType.push_back(OptionType);
  }
  void setOptionType(const list<OptionType_t> &OptionType) throw() {
    m_OptionType = OptionType;
  }
  int getOptionTypeSize() const throw() { return m_OptionType.size(); }
  const list<OptionType_t> &getOptionType() const throw() {
    return m_OptionType;
  }
  OptionType_t popOptionType() throw() {
    if (m_OptionType.empty())
      return 0;
    OptionType_t optionType = m_OptionType.front();
    m_OptionType.pop_front();
    return optionType;
  }

  ///////////////////////////gai zao////////////////////////
  void addGTwoOptionType(OptionType_t GTwoOptionType) throw() {
    m_GTwoOptionType.push_back(GTwoOptionType);
  }
  void setGTwoOptionType(const list<OptionType_t> &GTwoOptionType) throw() {
    m_GTwoOptionType = GTwoOptionType;
  }
  int getGTwoOptionTypeSize() const throw() { return m_GTwoOptionType.size(); }
  const list<OptionType_t> &getGTwoOptionType() const throw() {
    return m_GTwoOptionType;
  }
  OptionType_t popGTwoOptionType() throw() {
    if (m_GTwoOptionType.empty())
      return 0;
    OptionType_t GTwooptionType = m_GTwoOptionType.front();
    m_GTwoOptionType.pop_front();
    return GTwooptionType;
  }
  /////////////////////////////////////////////////////////

  void setSilver(Silver_t amount) throw() { m_Silver = amount; }
  Silver_t getSilver() const throw() { return m_Silver; }

  void setGrade(Grade_t grade) throw() { m_Grade = grade; }
  Grade_t getGrade() const throw() { return m_Grade; }

  void setDurability(Durability_t Durability) throw() {
    m_Durability = Durability;
  }
  Durability_t getDurability() const throw() { return m_Durability; }

  void setEnchantLevel(EnchantLevel_t level) throw() { m_EnchantLevel = level; }
  EnchantLevel_t getEnchantLevel() throw() { return m_EnchantLevel; }

  void setItemNum(ItemNum_t ItemNum) throw() { m_ItemNum = ItemNum; }
  ItemNum_t getItemNum() const throw() { return m_ItemNum; }

  void setMainColor(WORD MainColor) throw() { m_MainColor = MainColor; }
  WORD getMainColor() const throw() { return m_MainColor; }

  BYTE getListNum() const throw() { return m_ListNum; }
  void setListNum(BYTE ListNum) throw() { m_ListNum = ListNum; }

  void addListElement(SubItemInfo *pSubItemInfo) throw() {
    m_SubItemInfoList.push_back(pSubItemInfo);
    m_ListNum++;
  }

  void clearList() throw() {
    m_SubItemInfoList.clear();
    m_ListNum = 0;
  }

  SubItemInfo *popFrontListElement() throw() {
    SubItemInfo *TempSubItemInfo = m_SubItemInfoList.front();
    m_SubItemInfoList.pop_front();
    return TempSubItemInfo;
  }
  // 1208 wlzzi
  void setAddedInfo1(BYTE idx) throw() { m_AddedInfo1 = idx; }
  BYTE getAddedInfo1() const throw() { return m_AddedInfo1; }
  void setAddedInfo2(BYTE idx) throw() { m_AddedInfo2 = idx; }
  BYTE getAddedInfo2() const throw() { return m_AddedInfo2; }
  void setAddedInfo3(BYTE idx) throw() { m_AddedInfo3 = idx; }
  BYTE getAddedInfo3() const throw() { return m_AddedInfo3; }

public:
  uint getSize() const throw() {
    return szObjectID + szBYTE + szItemType +
           szBYTE + m_OptionType.size() * szOptionType +
           szBYTE + m_GTwoOptionType.size() * szOptionType +
           szDurability + szSilver + szGrade +
           szEnchantLevel + szItemNum + szWORD + szBYTE +
           SubItemInfo::getMaxSize() * m_ListNum
           // 1208 wlzzi
           + szBYTE * 3 // by rallser pet 3 option
#ifdef VERSION_THIRD_ENCHANT_1
           + szBYTE + m_ThirdOptionType.size() * szOptionType +
           szBYTE
#endif
           // 20090601 ksym555
           + szBYTE + m_MixOptionType.size() * szOptionType;
  }

  static uint getMaxSize() throw() {
    return szObjectID + szBYTE + szItemType +
           szBYTE + 255 * szOptionType +
           szBYTE + 255 * szOptionType +
           szDurability + szSilver + szGrade + szEnchantLevel + szItemNum +
           szWORD + szBYTE +
           SubItemInfo::getMaxSize() * 8
           // 1208 wlzzi
           + szBYTE * 3 // by rallser pet 3 option
#ifdef VERSION_THIRD_ENCHANT_1
           + szBYTE + 255 * szOptionType +
           szBYTE
#endif
           // 20090601 ksym555
           + szBYTE + 10 * szOptionType;
  }

#ifdef VERSION_THIRD_ENCHANT_1
  void addThirdOptionType(OptionType_t OptionType) throw() {
    m_ThirdOptionType.push_back(OptionType);
  }
  int getThirdOptionTypeSize() const throw() {
    return m_ThirdOptionType.size();
  }
  const list<OptionType_t> &getThirdOptionType() const throw() {
    return m_ThirdOptionType;
  }
  OptionType_t popThirdOptionType() throw() {
    if (m_ThirdOptionType.empty())
      return 0;
    OptionType_t optionType = m_ThirdOptionType.front();
    m_ThirdOptionType.pop_front();
    return optionType;
  }
  void setThirdOptionType(const list<OptionType_t> &optionType) throw() {
    m_ThirdOptionType = optionType;
  }
  BYTE getThirdEnchantType() const throw() { return m_ThirdEnchantType; }
  void setThirdEnchantType(BYTE ThirdEnchantType) throw() {
    m_ThirdEnchantType = ThirdEnchantType;
  }

  // 20090601 ksym555
  void setMixOptionType(const list<OptionType_t> &mixOptionType) throw() {
    m_MixOptionType = mixOptionType;
  }
  int getMixOptionTypeSize() const throw() { return m_MixOptionType.size(); }
  const list<OptionType_t> &getMixOptionType() const throw() {
    return m_MixOptionType;
  }
  OptionType_t getMixFirstOptionType() const throw() {
    if (m_MixOptionType.empty())
      return 0;
    return m_MixOptionType.front();
  }
  void removeMixOptionType(OptionType_t mixOptionType) throw() {
    std::list<OptionType_t>::iterator itr = std::find(
        m_MixOptionType.begin(), m_MixOptionType.end(), mixOptionType);
    if (itr != m_MixOptionType.end())
      m_MixOptionType.erase(itr);
  }
  void addMixOptionType(OptionType_t mixOptionType) throw() {
    m_MixOptionType.push_back(mixOptionType);
  }

#endif
protected:
  ObjectID_t m_ObjectID;               // item object id
  BYTE m_IClass;                       // item class
  ItemType_t m_ItemType;               // item type
  list<OptionType_t> m_OptionType;     // item option type
  list<OptionType_t> m_GTwoOptionType; // item GTwo option type gai zao
#ifdef VERSION_THIRD_ENCHANT_1
  list<OptionType_t> m_ThirdOptionType; // item option type
  BYTE m_ThirdEnchantType;              // Third Enchant type
#endif
  Durability_t m_Durability;             // item durability
  Silver_t m_Silver;                     // silver coating amount
  Grade_t m_Grade;                       // 아이템 등급
  EnchantLevel_t m_EnchantLevel;         // item enchant level
  ItemNum_t m_ItemNum;                   // number of item
  WORD m_MainColor;                      // item color
  BYTE m_ListNum;                        // number of sub item
  list<SubItemInfo *> m_SubItemInfoList; // actual sub item info

  // 1208 wlzzi - PetOption의 indexing정보 추가
  //	- PetOption2가 추가되면서 필요해 졌음. -_-;;
  //	- 아이템 클레스 별로 추가적인 정보의 전달이 필요할 때 공용으로
  // 사용하도록 .. 네이밍 한다.
  BYTE m_AddedInfo1;
  BYTE m_AddedInfo2;
  BYTE m_AddedInfo3;

  // 20090601 ksym555
  list<OptionType_t> m_MixOptionType;
};

#endif
