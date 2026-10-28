////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeLeather.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionTradeLeather.h"
#include "DB.h"
#include "FlagSet.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "NPC.h"
#include "OptionInfo.h"
#include "Ousters.h"
#include "PacketUtil.h"
#include "Player.h"
#include "Slayer.h"
#include "Vampire.h"
#include "VariableManager.h"
#include "Zone.h"


#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCNPCResponse.h"


#include <list>

struct ITEM_TEMPLATE {
  Item::ItemClass ItemClass;
  ItemType_t ItemType;
  string ItemName;
  int ItemNum;
};

const int NEED_ITEM_MAX = 1;
const ITEM_TEMPLATE NeedItemTemplate[NEED_ITEM_MAX] = {
    {Item::ITEM_CLASS_COMMON_QUEST_ITEM, 5, "�ջ�� ����", 10},
};

// BUGFIX: Changed from const global to runtime initialization to avoid
// double-free crash
ITEM_TEMPLATE ExchangeItemTemplate[1];
bool g_ExchangeItemTemplateInitialized = false;

void initExchangeItemTemplate() {
  if (!g_ExchangeItemTemplateInitialized) {
    ExchangeItemTemplate[0].ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
    ExchangeItemTemplate[0].ItemType = 4;
    ExchangeItemTemplate[0].ItemName =
        "ExchangeItemTemplate"; // Changed from corrupted string
    ExchangeItemTemplate[0].ItemNum = 1;
    g_ExchangeItemTemplateInitialized = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
