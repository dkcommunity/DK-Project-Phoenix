//----------------------------------------------------------------------
// MPremiumGiveItemInfo.h
//----------------------------------------------------------------------

#pragma once

class MPremiumGiveItemInfo {
public:
  MPremiumGiveItemInfo(MItem *_Item = NULL, int _Id = 0,
                       DWORD _TimeLimit = -1) {
    m_pItem = _Item;
    m_Id = _Id;
    m_TimeLimit = _TimeLimit;
  }
  ~MPremiumGiveItemInfo() {
    if (m_pItem != NULL) {
      delete m_pItem;
      m_pItem = NULL;
    }
    m_Id = 0;
    m_TimeLimit = -1;
  }

  MItem *GetItem() { return m_pItem; }
  int GetId() { return m_Id; }
  DWORD GetTimeLimit() { return m_TimeLimit; }

private:
  MItem *m_pItem;
  int m_Id;
  DWORD m_TimeLimit;
};

class MPremiumGiveItemInfoList {
protected:
  std::vector<MPremiumGiveItemInfo *>::const_iterator m_iterator;
  std::vector<MPremiumGiveItemInfo *> m_Give_Item_List;

  int m_Select_Item;

public:
  MPremiumGiveItemInfoList();
  ~MPremiumGiveItemInfoList();

  //------------------------------------------------------
  // Add
  //------------------------------------------------------
  bool AddItem(MPremiumGiveItemInfo *pItem);
  void Give_Item_List_ClearAll();

  void SetSelectItem(int _Select_Item) { m_Select_Item = _Select_Item; }
  int GetSelectItem() { return m_Select_Item; }

  //------------------------------------------------------
  // Iterator
  //------------------------------------------------------
  int GetItemNum() { return m_Give_Item_List.size(); }
  void SetBegin() { m_iterator = m_Give_Item_List.begin(); }
  MPremiumGiveItemInfo *Get() { return *m_iterator; }
  void Next() { m_iterator++; }
  bool IsEnd() const { return m_iterator == m_Give_Item_List.end(); }
  bool IsNotEnd() const { return m_iterator != m_Give_Item_List.end(); }
};

extern MPremiumGiveItemInfoList *g_pPremiumGiveItemInfoList;
