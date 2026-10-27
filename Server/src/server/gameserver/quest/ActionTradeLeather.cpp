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
// BUGFIX: Changed from const global to runtime initialization to avoid
// double-free crash on exit
static ITEM_TEMPLATE NeedItemTemplate[NEED_ITEM_MAX];
static ITEM_TEMPLATE ExchangeItemTemplate[1];
static bool g_TemplatesInitialized = false;

void initItemTemplates() {
  if (!g_TemplatesInitialized) {
    NeedItemTemplate[0].ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
    NeedItemTemplate[0].ItemType = 5;
    NeedItemTemplate[0].ItemName = "손상된 가죽";
    NeedItemTemplate[0].ItemNum = 10;

    ExchangeItemTemplate[0].ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
    ExchangeItemTemplate[0].ItemType = 4;
    ExchangeItemTemplate[0].ItemName = "완전한 가죽";
    ExchangeItemTemplate[0].ItemNum = 1;

    g_TemplatesInitialized = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// ActionTradeLeather
////////////////////////////////////////////////////////////////////////////////
ActionTradeLeather::ActionTradeLeather() {}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
ActionTradeLeather::~ActionTradeLeather() {}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionTradeLeather::read(PropertyBuffer &propertyBuffer) {
  __BEGIN_TRY

  try {
    // read script id
  } catch (NoSuchElementException &nsee) {
    throw Error(nsee.toString());
  }

  __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionTradeLeather::execute(Creature *pCreature1, Creature *pCreature2) {
  __BEGIN_TRY

  // BUGFIX: Initialize templates at runtime to avoid double-free crash
  initItemTemplates();

  Assert(pCreature1 != NULL);
  Assert(pCreature2 != NULL);
  Assert(pCreature1->isNPC());
  Assert(pCreature2->isPC());

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature2);
  Assert(pPC != NULL);

  Player *pPlayer = pPC->getPlayer();
  Assert(pPlayer != NULL);

  Inventory *pInventory = pPC->getInventory();
  Assert(pInventory != NULL);

  Zone *pZone = pPC->getZone();
  Assert(pZone != NULL);

  int tradeitemNum[NEED_ITEM_MAX] = {0};
  bool bTradeOK = true;

  for (int i = 0; i < NEED_ITEM_MAX; ++i) {
    tradeitemNum[i] = pPC->getItemClassTypeNum(NeedItemTemplate[i].ItemClass,
                                               NeedItemTemplate[i].ItemType);

    if (tradeitemNum[i] < NeedItemTemplate[i].ItemNum)
      bTradeOK = false;
  }

  GCNPCResponse gcNPCResponse;

  if (!bTradeOK) {
    gcNPCResponse.setCode(NPC_RESPONSE_NOT_ENOUGH_ITEM);
    pPlayer->sendPacket(&gcNPCResponse);

    gcNPCResponse.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
    pPlayer->sendPacket(&gcNPCResponse);

    return;
  }

  string itemOption;

  ItemInfo *pItemInfo = g_pItemInfoManager->getItemInfo(
      ExchangeItemTemplate[0].ItemClass, ExchangeItemTemplate[0].ItemType);
  Assert(pItemInfo != NULL);

  // 인벤토리의 공간을 체크
  _TPOINT pt;
  if (!pInventory->getEmptySlot(pItemInfo->getVolumeWidth(),
                                pItemInfo->getVolumeHeight(), pt)) {
    GCNPCResponse response;
    response.setCode(NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG);
    response.setParameter(NOT_ENOUGH_INVENTORY_SPACE);
    pPlayer->sendPacket(&response);
    return;
  }

  list<OptionType_t> options;

  Item *pItem = g_pItemFactoryManager->createItem(
      pItemInfo->getItemClass(), pItemInfo->getItemType(), options);
  Assert(pItem != NULL);

  pZone->registerObject(pItem);

  if (pInventory->addItem(pItem, pt)) {
    //		pPC->decreaseItemClassTypeNum(
    // Item::ITEM_CLASS_COMMON_QUEST_ITEM, itemType, m_QuestItemNum );
    GCModifyInformation gcMI;

    for (int i = 0; i < NEED_ITEM_MAX; ++i) {
      pPC->decreaseItemClassTypeNum(NeedItemTemplate[i].ItemClass,
                                    NeedItemTemplate[i].ItemType,
                                    NeedItemTemplate[i].ItemNum);
    }

    gcMI.addLongData(MODIFY_GOLD, pPC->getGold());
    pPlayer->sendPacket(&gcMI);

    pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);

    remainTraceLog(pItem, "Mystery Potion", pPC->getName(), ITEM_LOG_CREATE,
                   DETAIL_EVENTNPC);

    GCCreateItem gcCreateItem;
    makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);

    pPlayer->sendPacket(&gcCreateItem);
  } else {
    pInventory->deleteItem(pItem->getObjectID());

    SAFE_DELETE(pItem);

    gcNPCResponse.setCode(NPC_RESPONSE_NOT_ENOUGH_ITEM);
    pPlayer->sendPacket(&gcNPCResponse);

    gcNPCResponse.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
    pPlayer->sendPacket(&gcNPCResponse);
  }

  // 대화창 닫기
  /*	GCNPCResponse response;
          response.setCode( NPC_RESPONSE_GIVE_EVENT_ITEM_OK );
          pPlayer->sendPacket( &response ); */

  GCNPCResponse response;
  response.setCode(NPC_RESPONSE_GIVE_EVENT_ITEM_OK);
  pPlayer->sendPacket(&response);

  GCNPCAsk gcNPCAsk;
  gcNPCAsk.setObjectID(pCreature1->getObjectID());
  gcNPCAsk.setScriptID(14104);
  gcNPCAsk.setNPCID(dynamic_cast<NPC *>(pCreature1)->getNPCID());

  pPlayer->sendPacket(&gcNPCAsk);

  __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionTradeLeather::toString() const {
  __BEGIN_TRY

  StringStream msg;
  msg << "ActionTradeLeather("
      << ")";
  return msg.toString();

  __END_CATCH
}
