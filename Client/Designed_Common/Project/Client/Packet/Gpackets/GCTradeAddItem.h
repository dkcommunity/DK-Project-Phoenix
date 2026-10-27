////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeAddItem.h
// Written By  : 김성민
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_ADD_ITEM_H__
#define __GC_TRADE_ADD_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "SubItemInfo.h"
#include <list>
#if __CONTENTS(__PCITEMINFO2)
#include "PCItemInfo.h"
#endif //__PCITEMINFO2

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeAddItem;
//
////////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__PCITEMINFO2)
class GCTradeAddItem : public Packet {
public:
  GCTradeAddItem();
  ~GCTradeAddItem();

public:
  void read(SocketInputStream &iStream);
  void write(SocketOutputStream &oStream) const;
  void execute(Player *pPlayer);
  PacketID_t getPacketID() const { return PACKET_GC_TRADE_ADD_ITEM; }
  PacketSize_t getPacketSize() const {
    PacketSize_t size = 0;
    size += szObjectID;   // m_TargetObjectID
    size += szCoordInven; // m_X
    size += szCoordInven; // m_Y

    size += m_PCItemInfo.getSize(); // pciteminfo

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
    size += szBYTE;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
    return size;
  }

#ifdef __DEBUG_OUTPUT__
  std::string getPacketName() const { return "GCTradeAddItem"; }
  std::string toString() const;
#endif

public:
  ObjectID_t getTargetObjectID() const { return m_TargetObjectID; }
  void setTargetObjectID(ObjectID_t id) { m_TargetObjectID = id; }

  CoordInven_t getX() const { return m_X; }
  void setX(CoordInven_t x) { m_X = x; }

  CoordInven_t getY() const { return m_Y; }
  void setY(CoordInven_t y) { m_Y = y; }

  void SetPCItemInfo(PCItemInfo &pcItemInfo) { m_PCItemInfo = pcItemInfo; }
  PCItemInfo *GetPCItemInfo() { return &m_PCItemInfo; }

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  BYTE getCashItem() const { return m_CashItem; }
  void setCashItem(BYTE cashItem) { m_CashItem = cashItem; }

#endif //__INTERNATIONAL_PREMIUM_SYSTEM
private:
  ObjectID_t m_TargetObjectID; // 교환을 하고 있는 상대방의 OID
  CoordInven_t m_X;            // 인벤토리에서의 X 좌표
  CoordInven_t m_Y;            // 인벤토리에서의 Y 좌표

  PCItemInfo m_PCItemInfo;

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  BYTE m_CashItem;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
};
#else //__PCITEMINFO2
class GCTradeAddItem : public Packet {
public:
  GCTradeAddItem();
  ~GCTradeAddItem();

public:
  void read(SocketInputStream &iStream);
  void write(SocketOutputStream &oStream) const;
  void execute(Player *pPlayer);
  PacketID_t getPacketID() const { return PACKET_GC_TRADE_ADD_ITEM; }
  PacketSize_t getPacketSize() const {
    PacketSize_t size = 0;
    size += szObjectID;                   // m_TargetObjectID
    size += szCoordInven;                 // m_X
    size += szCoordInven;                 // m_Y
    size += szObjectID;                   // m_ItemObjectID
    size += szBYTE;                       // m_ItemClass
    size += szItemType;                   // m_ItemType
    size += szBYTE + m_OptionType.size(); // m_OptionType
    size += szDurability;                 // m_Durability
    size += szItemNum;                    // m_ItemNum
    size += szSilver;                     // silver coating amount
    size += szGrade;                      // grade
    size += szEnchantLevel;               // enchant level
    size += szBYTE;                       // m_ListNum
    size += (SubItemInfo::getSize() *
             m_ListNum); // std::list<SubItemInfo*> m_InfoList;
    size = size + szBYTE + m_ThirdOptionType.size(); // item Third option type
    size += szBYTE;                                  // Third Enchant type
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
    size += szBYTE;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
    return size;
  }

#ifdef __DEBUG_OUTPUT__
  std::string getPacketName() const { return "GCTradeAddItem"; }
  std::string toString() const;
#endif

public:
  ObjectID_t getTargetObjectID() const { return m_TargetObjectID; }
  void setTargetObjectID(ObjectID_t id) { m_TargetObjectID = id; }

  CoordInven_t getX() const { return m_X; }
  void setX(CoordInven_t x) { m_X = x; }

  CoordInven_t getY() const { return m_Y; }
  void setY(CoordInven_t y) { m_Y = y; }

  ObjectID_t getItemObjectID() const { return m_ItemObjectID; }
  void setItemObjectID(ObjectID_t id) { m_ItemObjectID = id; }

  BYTE getItemClass() const { return m_ItemClass; }
  void setItemClass(BYTE IClass) { m_ItemClass = IClass; }

  ItemType_t getItemType() const { return m_ItemType; }
  void setItemType(ItemType_t itemType) { m_ItemType = itemType; }

  int getOptionTypeSize() const { return m_OptionType.size(); }
  const std::list<OptionType_t> &getOptionType() const { return m_OptionType; }
  OptionType_t popOptionType() {
    if (m_OptionType.empty())
      return 0;
    OptionType_t optionType = m_OptionType.front();
    m_OptionType.pop_front();
    return optionType;
  }
  void addOptionType(OptionType_t otype) { m_OptionType.push_back(otype); }
  void setOptionType(const std::list<OptionType_t> &OptionTypes) {
    m_OptionType = OptionTypes;
  }

  Durability_t getDurability() const { return m_Durability; }
  void setDurability(Durability_t dur) { m_Durability = dur; }

  ItemNum_t getItemNum() const { return m_ItemNum; }
  void setItemNum(ItemNum_t itemNum) { m_ItemNum = itemNum; }

  Silver_t getSilver() const { return m_Silver; }
  void setSilver(Silver_t amount) { m_Silver = amount; }

  Grade_t getGrade() const { return m_Grade; }
  void setGrade(Grade_t grade) { m_Grade = grade; }

  EnchantLevel_t getEnchantLevel() const { return m_EnchantLevel; }
  void setEnchantLevel(EnchantLevel_t level) { m_EnchantLevel = level; }

  BYTE getListNum() const { return m_ListNum; }
  void setListNum(BYTE num) { m_ListNum = num; }

  void addListElement(SubItemInfo *pInfo) { m_InfoList.push_back(pInfo); }
  void clearList() {
    m_InfoList.clear();
    m_ListNum = 0;
  }

  SubItemInfo *popFrontListElement() {
    SubItemInfo *pInfo = m_InfoList.front();
    m_InfoList.pop_front();
    return pInfo;
  }
  void addThirdOptionType(OptionType_t ThirdOptionType) {
    m_OptionType.push_back(ThirdOptionType);
  }
  void setThirdOptionType(const std::list<OptionType_t> &ThirdOptionType) {
    m_ThirdOptionType = ThirdOptionType;
  }
  int getThirdOptionTypeSize() const { return m_ThirdOptionType.size(); }
  const std::list<OptionType_t> &getThirdOptionType() const {
    return m_ThirdOptionType;
  }
  OptionType_t popThirdOptionType() {
    if (m_ThirdOptionType.empty())
      return 0;
    OptionType_t ThirdoptionType = m_ThirdOptionType.front();
    m_ThirdOptionType.pop_front();
    return ThirdoptionType;
  }

  BYTE getThirdEnchantType() const { return m_ThirdEnchantType; }
  void setThirdEnchantType(BYTE ThirdEnchantType) {
    m_ThirdEnchantType = ThirdEnchantType;
  }

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  BYTE getCashItem() const { return m_CashItem; }
  void setCashItem(BYTE cashItem) { m_CashItem = cashItem; }

#endif //__INTERNATIONAL_PREMIUM_SYSTEM
private:
  ObjectID_t m_TargetObjectID;          // 교환을 하고 있는 상대방의 OID
  ObjectID_t m_ItemObjectID;            // 아이템 OID
  CoordInven_t m_X;                     // 인벤토리에서의 X 좌표
  CoordInven_t m_Y;                     // 인벤토리에서의 Y 좌표
  BYTE m_ItemClass;                     // 아이템 클래스
  ItemType_t m_ItemType;                // 아이템 타입
  std::list<OptionType_t> m_OptionType; // 옵션 타입
  Durability_t m_Durability;            // 내구도
  ItemNum_t m_ItemNum;                  // 아이템 숫자
  Silver_t m_Silver;                    // silver coating amount
  Grade_t m_Grade;                      // grade
  EnchantLevel_t m_EnchantLevel;        // enchant level
  BYTE m_ListNum;                       // 벨트일 경우, 안에 있는 아이템의 숫자
  std::list<SubItemInfo *> m_InfoList;  // 벨트일 경우, 안에 있는 아이템의 정보
  // 20071009 by diesirace 서드아이템옵션 추가
  std::list<OptionType_t> m_ThirdOptionType; // item Third option type
  BYTE m_ThirdEnchantType;                   // Third Enchant type

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  BYTE m_CashItem;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
};
#endif //__PCITEMINFO2

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeAddItemFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeAddItemFactory : public PacketFactory {
public:
  Packet *createPacket() { return new GCTradeAddItem(); }
  std::string getPacketName() const { return "GCTradeAddItem"; }
  PacketID_t getPacketID() const { return Packet::PACKET_GC_TRADE_ADD_ITEM; }
  PacketSize_t getPacketMaxSize() const {
    PacketSize_t size = 0;
#if __CONTENTS(__PCITEMINFO2)
    size = szCoordInven + // 아이템 X 좌표
           szCoordInven + // 아이템 Y 좌표
           szObjectID;    // 인벤토리 오브젝트 아이디
    size += PCItemInfo::getMaxSize();
#else

    size += szObjectID;                   // m_TargetObjectID
    size += szCoordInven;                 // m_X
    size += szCoordInven;                 // m_Y
    size += szObjectID;                   // m_ItemObjectID
    size += szBYTE;                       // m_ItemClass
    size += szItemType;                   // m_ItemType
    size += szBYTE + 255;                 // m_OptionType
    size += szDurability;                 // m_Durability
    size += szItemNum;                    // m_ItemNum
    size += szSilver;                     // silver coating amount
    size += szGrade;                      // Grade coating amount
    size += szEnchantLevel;               // enchant level
    size += szBYTE;                       // m_ListNum
    size += (SubItemInfo::getSize() * 8); // std::list<SubItemInfo*> m_InfoList;
    size = size + szBYTE + 255;           // m_ThirdOptionType
    size += szBYTE;                       // m_ThirdEnchantType
#endif //
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
    size += szBYTE;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
    return size;
  }
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeAddItemHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeAddItemHandler {
public:
  static void execute(GCTradeAddItem *pPacket, Player *pPlayer);
};

#endif
