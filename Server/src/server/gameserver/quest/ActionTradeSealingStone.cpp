////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeSealingStone.cpp
// Reconstructed by AI due to corruption
////////////////////////////////////////////////////////////////////////////////

#include "ActionTradeSealingStone.h"
#include "DB.h"
#include "FlagSet.h"
#include "GamePlayer.h"
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

// Custom Item Template Struct
struct ITEM_TEMPLATE {
  Item::ItemClass ItemClass;
  ItemType_t ItemType;
  string ItemName;
  int ItemNum;
};

// Global Constants
const int NEED_ITEM_MAX = 3;
const unsigned int NEED_MONEY = 600000;

// Static Globals for Runtime Init
static ITEM_TEMPLATE NeedItemTemplate[NEED_ITEM_MAX];
static ITEM_TEMPLATE ExchangeItemTemplate[1];
static bool g_SealingStoneTemplatesInitialized = false;

// Initialization Function
void initSealingStoneTemplates() {
  if (!g_SealingStoneTemplatesInitialized) {
    NeedItemTemplate[0].ItemClass = Item::ITEM_CLASS_EVENT_STAR;
    NeedItemTemplate[0].ItemType = 7;
    NeedItemTemplate[0].ItemName = "Event Star";
    NeedItemTemplate[0].ItemNum = 10;

    NeedItemTemplate[1].ItemClass = Item::ITEM_CLASS_QUEST_ITEM;
    NeedItemTemplate[1].ItemType = 0;
    NeedItemTemplate[1].ItemName = "Quest Item 1";
    NeedItemTemplate[1].ItemNum = 7;

    NeedItemTemplate[2].ItemClass = Item::ITEM_CLASS_QUEST_ITEM;
    NeedItemTemplate[2].ItemType = 1;
    NeedItemTemplate[2].ItemName = "Quest Item 2";
    NeedItemTemplate[2].ItemNum = 2;

    ExchangeItemTemplate[0].ItemClass = Item::ITEM_CLASS_QUEST_ITEM;
    ExchangeItemTemplate[0].ItemType = 10;
    ExchangeItemTemplate[0].ItemName = "Sealing Stone";
    ExchangeItemTemplate[0].ItemNum = 1;

    g_SealingStoneTemplatesInitialized = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// ActionTradeSealingStone
////////////////////////////////////////////////////////////////////////////////
ActionTradeSealingStone::ActionTradeSealingStone() {}

ActionTradeSealingStone::~ActionTradeSealingStone() {}

void ActionTradeSealingStone::read(PropertyBuffer &propertyBuffer) {}

void ActionTradeSealingStone::execute(Creature *pCreature1,
                                      Creature *pCreature2) {
  __BEGIN_TRY

  // Runtime Init Call
  initSealingStoneTemplates();

  Player *pPlayer = dynamic_cast<Player *>(pCreature1);
  NPC *pNPC = dynamic_cast<NPC *>(pCreature2);

  if (pPlayer == NULL || pNPC == NULL)
    return;

  GamePlayer *pGamePlayer = dynamic_cast<GamePlayer *>(pPlayer);
  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pPlayer);

  if (pPC == NULL)
    return;

  Inventory *pInventory = pPC->getInventory();
  if (pInventory == NULL)
    return;

  // Check Money
  if (pPC->getGold() < NEED_MONEY) {
    GCNPCResponse gcNPCResponse;
    gcNPCResponse.setCode(NPC_RESPONSE_NOT_ENOUGH_MONEY);
    pPlayer->sendPacket(&gcNPCResponse);
    return;
  }

  // Placeholder logic to allow compilation:
  // Just close the dialogue for now, as full reconstruction of inventory
  // checking logic without source reference is prone to errors. The primary
  // goal is to fix the server crash on exit.

  GCNPCResponse response;
  response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
  pPlayer->sendPacket(&response);

  __END_CATCH
}

string ActionTradeSealingStone::toString() const {
  return "ActionTradeSealingStone";
}
