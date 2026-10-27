//-----------------------------------------------------------------------------
// MPriceManager.h
//-----------------------------------------------------------------------------

#ifndef __MPRICEMANAGER_H__
#define __MPRICEMANAGER_H__

class MItem;

// 가격은 type의 별이 number개.
struct STAR_ITEM_PRICE {
  int type;
  int number;
};

class MPriceManager {
public:
  enum TRADE_TYPE {
    NPC_TO_PC, // player가 물건 살때, m_MarketCondSell
    PC_TO_NPC, // player가 물건 팔때, m_MarketCondBuy
    REPAIR,    // player가 물건 repair할 때,
    SILVERING, // player가 물건에 실버코팅할 때,
  };
  typedef std::pair<DWORD, DWORD> REPAIRPIRCEPAIR;
  typedef std::map<DWORD, REPAIRPIRCEPAIR> REPAIRPRICELIST;

public:
  MPriceManager();
  ~MPriceManager();

  //-------------------------------------------------------
  // Get Item Price
  //-------------------------------------------------------
  int GetItemPrice(MItem *pItem, TRADE_TYPE type, bool bMysterious = false);
  void GetItemPrice(MItem *pItem, STAR_ITEM_PRICE &price);
  int GetMysteriousPrice(MItem *pItem) const;

  //-------------------------------------------------------
  // 시세
  //-------------------------------------------------------
  // NPC 기준으로 buy, sell
  void SetMarketCondBuy(int buy) { m_MarketCondBuy = buy; }
  void SetMarketCondSell(int sell) { m_MarketCondSell = sell; }

  int GetMarketCondBuy() const { return m_MarketCondBuy; }
  int GetMarketCondSell() const { return m_MarketCondSell; }

  void SetEventItemPrice(int Price) { m_EventFixPrice = Price; }
  REPAIRPRICELIST::iterator GetRepairPriceListBegin() {
    return m_RepairPriceList.begin();
  }
  REPAIRPRICELIST::iterator GetRepairPriceListEnd() {
    return m_RepairPriceList.end();
  }
  void RepairPriceListInsert(DWORD itemObjectID,
                             REPAIRPIRCEPAIR repairPircePair);
  bool IsRepairPriceListHave(DWORD itemObjectID);
  REPAIRPIRCEPAIR GetRepairPricePair(DWORD itemObjectID);
  void RepairPriceListeraseAll() {
    m_RepairPriceList.erase(GetRepairPriceListBegin(), GetRepairPriceListEnd());
  }
  void RepairPriceListerase(DWORD itemObjectID);
  void SetOldRepairItemID(DWORD oldReapairItemID) {
    m_OldRepairItemID = oldReapairItemID;
  }
  DWORD GetOldRepairItemID() { return m_OldRepairItemID; }

protected:
  // 시세
  int m_MarketCondBuy;  // NPC가 살때(25)
  int m_MarketCondSell; // NPC가 팔때(100)
  int m_EventFixPrice;  // 서버에서 정해주는 이벤트 아이템 가격
  MPriceManager::REPAIRPRICELIST m_RepairPriceList;
  DWORD m_OldRepairItemID;
};

extern MPriceManager *g_pPriceManager;

#endif
