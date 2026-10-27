//////////////////////////////////////////////////////////////////////////////
// Filename    : GCShopList.cpp
// Written By  : 김성민
// Description :
// 플레이어가 가진 상점 버전이 서버에 있는 것과 다를 경우,
// 플레이어는 서버에 상점에 있는 상품 목록을 요구하게 된다.
// 이 패킷은 그 때 플레이어에게 전달하게 되는 패킷이다.
//////////////////////////////////////////////////////////////////////////////

#include "GCShopList.h"
#include "Client_PCH.h"
#include "DarkAssert.h"

// #include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
GCShopList::GCShopList()

{
  __BEGIN_TRY

  for (int i = 0; i < SHOP_RACK_INDEX_MAX; i++)
    m_pBuffer[i].bExist = false;

  m_ShopType = 0;

  __END_CATCH;
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCShopList::~GCShopList() {
  __BEGIN_TRY
  __END_CATCH;
}

//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void GCShopList::read(SocketInputStream &iStream)

{
  __BEGIN_TRY

  BYTE i = 0;
  BYTE nTotal = 0;
  BYTE index = 0;

  // read NPC id & version & rack type & number of item
  iStream.read(m_ObjectID);
  iStream.read(m_Version);
  iStream.read(m_RackType);
  iStream.read(nTotal);

  // read each item info
  for (int i = 0; i < nTotal; i++) {
    iStream.read(index);

    // FIX 2026-05-12: index packet'tan geliyor (server tarafindan), bound check
    // YOK idi. m_pBuffer[index] array size = SHOP_RACK_INDEX_MAX (20). Eger
    // server `index >= 20` gonderirse `_SHOPLISTITEM &item = m_pBuffer[index]`
    // OOB write/read -> stack corruption -> Client crash. NPC Shop tiklandiktan
    // sonra crash root cause'unun olasi sitesi (GCShopList read flow).
    //
    // getShopItem() L237'de bound check Assert var ama read()'de yok — paralel
    // fix. Buggy/hostile server'a karsi defansif: invalid index'ler skip,
    // sonraki item devam etsin (loop break degil continue).
    if (index >= SHOP_RACK_INDEX_MAX) {
      // Stream'i tutarli tutmak icin item bilgilerini yine de oku (sonraki
      // iterasyon dogru noktadan baslasin), ama m_pBuffer'a yazma.
      _SHOPLISTITEM dummy;
      iStream.read(dummy.objectID);
      iStream.read(dummy.itemClass);
      iStream.read(dummy.itemType);
      BYTE optionSize;
      iStream.read(optionSize);
      for (int j = 0; j < optionSize; j++) {
        OptionType_t optionType;
        iStream.read(optionType);
      }
      iStream.read(dummy.durability);
      iStream.read(dummy.silver);
      iStream.read(dummy.grade);
      iStream.read(dummy.enchantLevel);
      continue;
    }

    _SHOPLISTITEM &item = m_pBuffer[index];

    iStream.read(item.objectID);
    iStream.read(item.itemClass);
    iStream.read(item.itemType);

    BYTE optionSize;
    iStream.read(optionSize);
    for (int j = 0; j < optionSize; j++) {
      OptionType_t optionType;
      iStream.read(optionType);
      item.optionType.push_back(optionType);
    }

    iStream.read(item.durability);
    iStream.read(item.silver);
    iStream.read(item.grade);
    iStream.read(item.enchantLevel);

    m_pBuffer[index].bExist = true;
  }

  iStream.read(m_MarketCondBuy);
  iStream.read(m_MarketCondSell);
  iStream.read(m_ShopType);

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void GCShopList::write(SocketOutputStream &oStream) const

{
  __BEGIN_TRY

  BYTE i = 0;
  BYTE nTotal = 0;

  // calculate total number of items
  for (int i = 0; i < SHOP_RACK_INDEX_MAX; i++)
    if (m_pBuffer[i].bExist)
      nTotal++;

  // write NPC id & version & rack type & number of item
  oStream.write(m_ObjectID);
  oStream.write(m_Version);
  oStream.write(m_RackType);
  oStream.write(nTotal);

  // write each item info
  for (int i = 0; i < SHOP_RACK_INDEX_MAX; i++) {
    const _SHOPLISTITEM &item = m_pBuffer[i];

    if (item.bExist) {
      oStream.write(i);
      oStream.write(item.objectID);
      oStream.write(item.itemClass);
      oStream.write(item.itemType);

      BYTE optionSize = item.optionType.size();
      oStream.write(optionSize);

      std::list<OptionType_t>::const_iterator itr;
      for (itr = item.optionType.begin(); itr != item.optionType.end(); itr++) {
        OptionType_t optionType = *itr;
        oStream.write(optionType);
      }

      oStream.write(item.durability);
      oStream.write(item.silver);
      oStream.write(item.grade);
      oStream.write(item.enchantLevel);
    }
  }

  oStream.write(m_MarketCondBuy);
  oStream.write(m_MarketCondSell);
  oStream.write(m_ShopType);

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCShopList::execute(Player *pPlayer)

{
  __BEGIN_TRY

  GCShopListHandler::execute(this, pPlayer);

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

PacketSize_t GCShopList::getPacketSize() const

{
  PacketSize_t size = szObjectID +     // NPC id
                      szShopVersion +  // shop version
                      szShopRackType + // rack type
                      szBYTE;          // total number of item

  for (int i = 0; i < SHOP_RACK_INDEX_MAX; i++) {
    if (m_pBuffer[i].bExist) {
      size += szBYTE;     // Item index in shop rack
      size += szObjectID; // item OID
      size += szBYTE;     // item class
      size += szItemType; // item type
      size += szBYTE + m_pBuffer[i].optionType.size(); // item option type
      size += szDurability;                            // item durability
      size += szSilver;                                // silver coating amount
      size += szGrade;                                 // grade
      size += szEnchantLevel;                          // enchant level
    }
  }

  size += szMarketCond + szMarketCond;

  return size;
}

#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string GCShopList::toString() const

{
  __BEGIN_TRY

  StringStream msg;

  msg << "GCShopList(" << "ObjectID:" << m_ObjectID << ","
      << "ShopVersion:" << (int)m_Version << ","
      << "RackType:" << (int)m_RackType << ",";

  for (int i = 0; i < SHOP_RACK_INDEX_MAX; i++) {
    msg << "(Item" << i << ":";
    if (m_pBuffer[i].bExist) {
      msg << "ObjectID:" << (int)(m_pBuffer[i].objectID)
          << "ItemClass:" << (int)(m_pBuffer[i].itemClass)
          << "ItemType:" << (int)(m_pBuffer[i].itemType)
          << "OptionTypeSize:" << (int)(m_pBuffer[i].optionType.size())
          << "Durability:" << (int)(m_pBuffer[i].durability)
          << "Silver:" << (int)(m_pBuffer[i].silver)
          << "Grade:" << (int)(m_pBuffer[i].grade)
          << "EnchantLevel:" << (int)(m_pBuffer[i].enchantLevel);
      msg << ")";
    }
  }

  msg << "MarketCondBuy:" << (int)m_MarketCondBuy
      << "MarketCondSell:" << (int)m_MarketCondSell
      << "ShopType:" << (int)m_ShopType;

  msg << ")";

  return msg.toString();

  __END_CATCH
}
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SHOPLISTITEM GCShopList::getShopItem(BYTE index) const

{
  // check bound
  if (index >= SHOP_RACK_INDEX_MAX)
    Assert(false && "GCShopList::getShopItem : Out of Bound!");

  // return shop item info
  return m_pBuffer[index];
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
void GCShopList::setShopItem(BYTE index, const Item *pItem)

{
  // check bound
  if (index >= SHOP_RACK_INDEX_MAX)
    Assert(false && "GCShopList::setShopItem : Out of Bound!");

  // check pointer
  Assert(pItem != NULL);

  // set shop item info
  m_pBuffer[index].bExist = true;
  m_pBuffer[index].objectID = pItem->getObjectID();
  m_pBuffer[index].itemClass = pItem->getItemClass();
  m_pBuffer[index].itemType = pItem->getItemType();
  m_pBuffer[index].optionType = pItem->getOptionTypeList();
  m_pBuffer[index].durability = pItem->getDurability();
  m_pBuffer[index].silver = pItem->getSilver();
  m_pBuffer[index].grade = pItem->getGrade();
  m_pBuffer[index].enchantLevel = pItem->getEnchantLevel();
}

#endif
