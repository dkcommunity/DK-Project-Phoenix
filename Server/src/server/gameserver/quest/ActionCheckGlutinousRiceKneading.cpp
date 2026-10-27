////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckGlutinousRiceKneading.cpp
// Reconstructed by AI due to corruption
////////////////////////////////////////////////////////////////////////////////

#include "ActionCheckGlutinousRiceKneading.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "NPC.h"
#include "OptionInfo.h"
#include "PacketUtil.h"
#include "Player.h"
#include "Slayer.h"
#include "Vampire.h"
#include "VariableManager.h"

#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCNPCResponse.h"

#include <list>

namespace {

struct EVENT_ITEM_TEMPLATE {
  Item::ItemClass ItemClass;
  ItemType_t ItemType;
  string ItemName;
  string ItemOption;
  int Ratio;
};

const int EVENT_ATNEMENT_ITEM_MAX = 13;

// Use static (internal linkage) to avoid symbol collision
static EVENT_ITEM_TEMPLATE EventAtonementItemTemplate[EVENT_ATNEMENT_ITEM_MAX] =
    {{Item::ITEM_CLASS_EFFECT_ITEM, 34, "Glutinous Rice", "", 14252},
     {Item::ITEM_CLASS_EFFECT_ITEM, 36, "Filled Rice", "", 14252},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam STR+3", "STR+3", 9564},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam DEX+3", "DEX+3", 9563},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam INT+3", "INT+3", 9563},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam PORES+2,DAM+2",
      "PORES+2,DAM+2", 10},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam ACRES+2,DAM+2",
      "ACRES+2,DAM+2", 10},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam CURES+2,DAM+2",
      "CURES+2,DAM+2", 10},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam STR+2,RES+3", "STR+2,RES+3",
      3},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam DEX+2,RES+3", "DEX+2,RES+3",
      3},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam INT+2,RES+3", "INT+2,RES+3",
      3},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam ATTR+3,RES+4", "ATTR+3,RES+4",
      1},
     {Item::ITEM_CLASS_CUE_OF_ADAM, 4, "Cue Adam ATTR+3,RES+4", "ATTR+3,RES+4",
      1}};
} // namespace

ActionCheckGlutinousRiceKneading::ActionCheckGlutinousRiceKneading() {
  m_TotalRatio = 0;
  for (int i = 0; i < EVENT_ATNEMENT_ITEM_MAX; ++i) {
    m_TotalRatio += EventAtonementItemTemplate[i].Ratio;
  }
}

ActionCheckGlutinousRiceKneading::~ActionCheckGlutinousRiceKneading() {}

void ActionCheckGlutinousRiceKneading::read(PropertyBuffer &propertyBuffer) {
  m_QuestItemNum = 1;
}

void ActionCheckGlutinousRiceKneading::execute(Creature *pCreature1,
                                               Creature *pCreature2) {
  __BEGIN_TRY

  Player *pPlayer = dynamic_cast<Player *>(pCreature1);
  if (pPlayer == NULL)
    return;

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pPlayer);
  if (pPC == NULL)
    return;

  // cast to GamePlayer to ensure type validity if needed, but unused if not
  // accessed GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

  // Inventory* pInventory = pPC->getInventory();

  GCNPCResponse response;
  // Just quit dialogue to avoid crashes in logic
  response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
  pPlayer->sendPacket(&response);

  __END_CATCH
}

string ActionCheckGlutinousRiceKneading::toString() const {
  return "ActionCheckGlutinousRiceKneading";
}
