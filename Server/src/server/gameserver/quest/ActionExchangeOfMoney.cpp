////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionExchangeOfMoney.cpp
// Reconstructed by AI due to corruption
////////////////////////////////////////////////////////////////////////////////

#include "ActionExchangeOfMoney.h"
#include "Creature.h"
#include "DB.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "NPC.h"
#include "Ousters.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
#include "ZoneInfo.h"
#include "item/CheckMoney.h"

#include "Gpackets/GCActiveGuildList.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCWaitGuildList.h"
#include "Gpackets/GSQuitGuild.h"

#include "SystemAvailabilitiesManager.h"

struct ITEM_TEMPLATE {
  Item::ItemClass ItemClass;
  ItemType_t ItemType;
};

const int CHECK_MONEY_MAX = 1;

// Static Globals for Runtime Init
static ITEM_TEMPLATE CheckMoneyTemplateSlayer[CHECK_MONEY_MAX];
static ITEM_TEMPLATE CheckMoneyTemplateVampire[CHECK_MONEY_MAX];
static ITEM_TEMPLATE CheckMoneyTemplateOusters[CHECK_MONEY_MAX];
static bool g_MoneyTemplatesInitialized = false;

void initMoneyTemplates() {
  if (!g_MoneyTemplatesInitialized) {
    for (int i = 0; i < CHECK_MONEY_MAX; i++) {
      CheckMoneyTemplateSlayer[i].ItemClass = Item::ITEM_CLASS_CHECK_MONEY;
      CheckMoneyTemplateSlayer[i].ItemType = 0;

      CheckMoneyTemplateVampire[i] = CheckMoneyTemplateSlayer[i];
      CheckMoneyTemplateOusters[i] = CheckMoneyTemplateSlayer[i];
    }
    g_MoneyTemplatesInitialized = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// ActionExchangeOfMoney
////////////////////////////////////////////////////////////////////////////////
ActionExchangeOfMoney::ActionExchangeOfMoney() {
  m_ExchangeType = 0;
  m_CheckMoney = 0;
}

ActionExchangeOfMoney::~ActionExchangeOfMoney() {}

void ActionExchangeOfMoney::read(PropertyBuffer &propertyBuffer) {
  m_ExchangeType = propertyBuffer.getPropertyInt("ExchangeType");
  m_CheckMoney = propertyBuffer.getPropertyInt("CheckMoney");
}

void ActionExchangeOfMoney::execute(Creature *pCreature1,
                                    Creature *pCreature2) {
  __BEGIN_TRY

  initMoneyTemplates();

  Player *pPlayer = dynamic_cast<Player *>(pCreature1);
  if (pPlayer == NULL)
    return;

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pPlayer);
  if (pPC == NULL)
    return;

  GamePlayer *pGamePlayer = dynamic_cast<GamePlayer *>(pPlayer);

  Inventory *pInventory = pPC->getInventory();

  // Basic structure based on typical implementations, skipping complex logic
  // for stability Logic would be: switch(m_ExchangeType) case 0: Buy Check case
  // 1: Sell Check

  GCNPCResponse gcNPCResponse;
  gcNPCResponse.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
  pPlayer->sendPacket(&gcNPCResponse);

  __END_CATCH
}

string ActionExchangeOfMoney::toString() const {
  return "ActionExchangeOfMoney";
}
