//----------------------------------------------------------------------
// MItem.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable : 4786)

#include "ClientConfig.h"
#include "crash_trace_tool.h"
#include "MCreatureTable.h"
#include "MFakeCreature.h"
#include "MGameStringTable.h"
#include "MItem.h"
#include "MItemOptionTable.h"
#include "MItemTable.h"
#include "MTimeItemManager.h"
#include "UserInformation.h"

// 2026-05-12: g_pItemTable[c][t] direct access OOB guard. Empty table /
// out-of-bounds class veya type'da crash yerine sane fallback dondurur.
// Memory: feedback_data_source_suspect (ItemTable load timing) + Multi-Agent
// audit (MItem 7 getter sweep, Description/Weight/Price/GridW/GridH/
// TileFrameID/InventoryFrameID). VS_UI_GameCommon.cpp:96 ITEM_TABLE_VALID
// makrosu ile ayni mantik, MItem.cpp TU icin local scope.
#define MITEM_TABLE_VALID(classN, typeN)                                      \
    (g_pItemTable != NULL &&                                                  \
     g_pItemTable->GetSize() > (uint)(classN) &&                              \
     (*g_pItemTable)[(classN)].GetSize() > (uint)(typeN))


#ifdef __GAME_CLIENT__
#include "Client.h"
#include "ClientFunction.h"
#include "MGameStringTable.h"
#include "MInventory.h"
#include "MPlayer.h"
#include "MSkillManager.h"
#include "MSlayerGear.h"
#include "MZone.h"
#include "MZoneTable.h"
#include "ServerInfo.h"
#include "SkillDef.h"
#include "UIFunction.h"
#include "packet/CPackets/CGUsePotionFromInventory.h"
#include "packet/CPackets/CGUsePotionFromQuickSlot.h"
#include "packet\CPackets\CGUseItemFromGear.h"

#include "FL2.h"
#include "UIDialog.h"
#include "VS_UI_GameCommon.h"
#include "vs_ui.h"

#endif

#include <algorithm>
#include <fstream>


#ifdef __GAME_CLIENT__
#include "DebugInfo.h"
#include "MCreature.h"
#include "MItemFinder.h"
#include "MTopView.h"
#include "RankBonusTable.h"
#endif

extern DWORD g_CurrentFrame;
extern bool g_bHolyLand;
extern bool g_bZoneSafe;

class MItemManager;

//----------------------------------------------------------------------
// 최대 수
//----------------------------------------------------------------------
// #define MAX_FIRE_CRACKER_PILE_NUMBER	9
// #define	MAX_POTION_PILE_NUMBER			9
// #define	MAX_MAGAZINE_PILE_NUMBER		20
// #define	MAX_EVENT_STAR_PILE_NUMBER		50
// #define	MAX_MONEY_NUMBER				100000000
// #define	MAX_SKULL_NUMBER				9
// #define	MAX_WATER_NUMBER				20
// #define	MAX_HOLY_WATER_NUMBER			20
// #define	MAX_BOMB_MATERIAL_NUMBER		20
// #define	MAX_BOMB_NUMBER					20
// #define	MAX_MINE_NUMBER					20
// #define MAX_EFFECT_ITEM_NUMBER			20
// #define	MAX_VAMPIRE_ETC_NUMBER			9
// #define	MAX_SERUM_NUMBER				9
// #define MAX_RESURRECT_SCROLL_NUMBER		9
// #define MAX_MIXING_ITEM_NUMBER			9
// #define MAX_OUSTERS_GEM_NUMBER			50
// #define MAX_OUSTERS_LARVA_NUMBER		9
// #define MAX_MOON_CARD_NUMBER			99
// #define MAX_LUCKY_BAG_NUMBER			50
//
// #define MAX_SLAYER_ATTR_OLD				200
// #define MAX_SLAYER_ATTR_SUM_OLD			300
// #define MAX_VAMPIRE_LEVEL_OLD			100
//
// #define	MAX_SLAYER_ATTR					295
// #define	MAX_SLAYER_ATTR_SUM				435
// #define MAX_VAMPIRE_LEVEL				150
// #define MAX_OUSTERS_LEVEL				150
// #define MAX_PET_ENCHANT_PILE_NUMBER		50
// #define MAX_PET_FOOD_PILE_NUMBER		9

#define MAX_FIRE_CRACKER_PILE_NUMBER 9
#define MAX_POTION_PILE_NUMBER 30
#define MAX_MAGAZINE_PILE_NUMBER 99
#define MAX_EVENT_STAR_PILE_NUMBER 50
#define MAX_MONEY_NUMBER 100000000
#define MAX_SKULL_NUMBER 9
#define MAX_WATER_NUMBER 50
#define MAX_HOLY_WATER_NUMBER 50
#define MAX_BOMB_MATERIAL_NUMBER 20
#define MAX_BOMB_NUMBER 20
#define MAX_MINE_NUMBER 20
#define MAX_EFFECT_ITEM_NUMBER 20
#define MAX_VAMPIRE_ETC_NUMBER 20
#define MAX_SERUM_NUMBER 30
#define MAX_RESURRECT_SCROLL_NUMBER 9
#define MAX_MIXING_ITEM_NUMBER 9
#define MAX_OUSTERS_GEM_NUMBER 50
#define MAX_OUSTERS_LARVA_NUMBER 30
#define MAX_MOON_CARD_NUMBER 99
#define MAX_LUCKY_BAG_NUMBER 50

#define MAX_SLAYER_ATTR_OLD 200
#define MAX_SLAYER_ATTR_SUM_OLD 300
#define MAX_VAMPIRE_LEVEL_OLD 100

#define MAX_SLAYER_ATTR 295
#define MAX_SLAYER_ATTR_SUM 435
#define MAX_VAMPIRE_LEVEL 150
#define MAX_OUSTERS_LEVEL 150
#define MAX_PET_ENCHANT_PILE_NUMBER 50
#define MAX_PET_FOOD_PILE_NUMBER 9
// nanomech 2006.03.08 Item 수정 작업 start
#define MAX_COMMON_QUEST_ITEM_NUMBER 99
// nanomech 2006.03.08 Item 수정 작업 end

#if __CONTENTS(__EVENTTREE_PILE)
#define MAX_EVENTTREE_NUMBER 99 // 2008.09.17 Eventtree Number..
#endif                          //__EVENTTREE_PILE
#if __CONTENTS(__GLOBAL_NPC)
#define MAX_CALLNPC_CARD 9
#endif //__GLOBAL_NPC

// 아이템 이름 색상 정의
#define COVER_ITEM_NAME_COLOR                                                  \
  16 // [초록색] 코어잽, 에테리얼체인, 큐와 같은 아이템
#define EVENT_ITEM_NAME_COLOR 60 // [보라색] 이벤트 아이템

//----------------------------------------------------------------------
//
// static members
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// 한글을 사용하는가?
//----------------------------------------------------------------------
bool MItem::s_bUseKorean = true;

//----------------------------------------------------------------------
// Item 떨어뜨릴때의 높이들..
//----------------------------------------------------------------------
int MItem::s_DropHeight[MAX_DROP_COUNT] = {36, 49, 64, 36, 16, 4};

//----------------------------------------------------------------------
// ItemClass에 맞게 메모리를 잡아주는 class table을 설정..
//----------------------------------------------------------------------
MItem::FUNCTION_NEWITEMCLASS MItem::s_NewItemClassTable[MAX_ITEM_CLASS] = {
    MMotorcycle::NewItem,
    MPotion::NewItem,
    MWater::NewItem,
    MHolyWater::NewItem,
    MMagazine::NewItem,
    MBombMaterial::NewItem,
    MItemETC::NewItem,
    MKey::NewItem,
    MRing::NewItem,
    MBracelet::NewItem,
    MNecklace::NewItem,
    MCoat::NewItem,
    MTrouser::NewItem,
    MShoes::NewItem,
    MSword::NewItem,
    MBlade::NewItem,
    MShield::NewItem,
    MCross::NewItem,
    MGlove::NewItem,
    MHelm::NewItem,
    MGunSG::NewItem,
    MGunSMG::NewItem,
    MGunAR::NewItem,
    MGunTR::NewItem,
    MBomb::NewItem,
    MMine::NewItem,
    MBelt::NewItem,
    MLearningItem::NewItem,
    MMoney::NewItem,
    MCorpse::NewItem,
    // MSkull::NewItem,

    // vampire용 item
    MVampireRing::NewItem,
    MVampireBracelet::NewItem,
    MVampireNecklace::NewItem,
    MVampireCoat::NewItem,

    // skull
    MSkull::NewItem,

    // mace추가 - 2001/3/26
    MMace::NewItem,

    MSerum::NewItem,

    // 2001.8.21
    MVampireETC::NewItem,

    // 2001.10.22
    MSlayerPortalItem::NewItem,
    MVampirePortalItem::NewItem,

    // 2001.12.10
    MEventGiftBoxItem::NewItem,
    MEventStarItem::NewItem,

    MVampireEarRing::NewItem,

    // 2002.6.7
    MRelic::NewItem,

    // 2002.8.18
    MVampireWeapon::NewItem,
    MVampireAmulet::NewItem,

    // 2002.9.7
    MQuestItem::NewItem,

    // 2002.12.9
    MEventTreeItem::NewItem,
    MEventEtcItem::NewItem,

    // 2003.1.29
    MBloodBible::NewItem,

    // 2003.2.12
    MCastleSymbol::NewItem,

    MCoupleRing::NewItem,
    MVampireCoupleRing::NewItem,

    MEventItem::NewItem,
    MDyePotionItem::NewItem,
    MResurrectItem::NewItem,
    MMixingItem::NewItem,

    MOustersArmsBand::NewItem,
    MOustersBoots::NewItem,
    MOustersChakram::NewItem,
    MOustersCirclet::NewItem,
    MOustersCoat::NewItem,
    MOustersPendent::NewItem,
    MOustersRing::NewItem,
    MOustersStone::NewItem,
    MOustersWristlet::NewItem,

    MOustersLarva::NewItem,     // 66
    MOustersPupa::NewItem,      // 67
    MOustersComposMei::NewItem, // 68
    MOustersSummonGem::NewItem, // 69
    MEffectItem::NewItem,       // 70
    MCodeSheetItem::NewItem,    // 71
    MMoonCardItem::NewItem,

    MSweeperItem::NewItem,
    MPetItem::NewItem,
    MPetFood::NewItem,
    MPetEnchantItem::NewItem,
    MLuckyBag::NewItem,
    MSms_item::NewItem,
    MCoreZap::NewItem,
    MGQuestItem::NewItem,
    MTrapItem::NewItem,
    MBloodBibleSign::NewItem,
    MWarItem::NewItem,
    // by csm 2차 전직 아이템 클래스 추가
    MCarryingReceiver::NewItem,
    MShoulderArmor::NewItem,
    MDermis::NewItem,
    MPersona::NewItem,
    MFascia::NewItem,
    MMitten::NewItem,

    MSubInventory::NewItem,

    MCommonQuestItem::NewItem,
    MEtherealChain::NewItem,
    MOustersHarmonicPendent::NewItem,
    MCheckMoney::NewItem,
    MCueOfAdam::NewItem,
    MContractOfBlood::NewItem,

    MSkillBook::NewItem,
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
    MVampireWingItem::NewItem,
    MOustersWingItem::NewItem,
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

#if __CONTENTS(__TUNING_ITEM)
    MTuningItemSlayer::NewItem,
    MTuningItemVampire::NewItem,
    MTuningItemOusters::NewItem,
#endif //__TUNING_ITEM
#if __CONTENTS(__GLOBAL_NPC)
    MCallNPCNPCCard::NewItem,
    MRankGem::NewItem,
#endif //__GLOBAL_NPC
};

//----------------------------------------------------------------------
// new functions
//----------------------------------------------------------------------
/*
MItem*	MMotorcycle::NewItem()		{ return new MMotorcycle; }
MItem*	MPotion::NewItem()			{ return new MPotion; }
MItem*	MWater::NewItem()			{ return new MWater; }
MItem*	MHolyWater::NewItem()		{ return new MHolyWater; }
MItem*	MMagazine::NewItem()		{ return new MMagazine; }
MItem*	MBombMaterial::NewItem()	{ return new MBombMaterial; }
MItem*	MItemETC::NewItem()			{ return new MItemETC; }
MItem*	MKey::NewItem()				{ return new MKey; }
MItem*	MRing::NewItem()			{ return new MRing; }
MItem*	MBracelet::NewItem()		{ return new MBracelet; }
MItem*	MNecklace::NewItem()		{ return new MNecklace; }
MItem*	MCoat::NewItem()			{ return new MCoat; }
MItem*	MTrouser::NewItem()			{ return new MTrouser; }
MItem*	MShoes::NewItem()			{ return new MShoes; }
MItem*	MSword::NewItem()			{ return new MSword; }
MItem*	MBlade::NewItem()			{ return new MBlade; }
MItem*	MShield::NewItem()			{ return new MShield; }
MItem*	MCross::NewItem()			{ return new MCross; }
MItem*	MGlove::NewItem()			{ return new MGlove; }
MItem*	MHelm::NewItem()			{ return new MHelm; }
MItem*	MGunSG::NewItem()			{ return new MGunSG; }
MItem*	MGunSMG::NewItem()			{ return new MGunSMG; }
MItem*	MGunAR::NewItem()			{ return new MGunAR; }
MItem*	MGunTR::NewItem()			{ return new MGunTR; }
MItem*	MBomb::NewItem()			{ return new MBomb; }
MItem*	MMine::NewItem()			{ return new MMine; }
MItem*	MBelt::NewItem()			{ return new MBelt; }
MItem*	MLearningItem::NewItem()	{ return new MLearningItem; }
MItem*	MMoney::NewItem()			{ return new MMoney; }
MItem*	MCorpse::NewItem()			{ return new MCorpse; }

// vampire용 item
MItem*	MVampireRing::NewItem()		{ return new MVampireRing; }
MItem*	MVampireBracelet::NewItem()	{ return new MVampireBracelet; }
MItem*	MVampireNecklace::NewItem()	{ return new MVampireNecklace; }
MItem*	MVampireCoat::NewItem()		{ return new MVampireCoat; }
MItem*	MVampireShoes::NewItem()	{ return new MVampireShoes; }
*/

//----------------------------------------------------------------------
// New Item
//----------------------------------------------------------------------
// itemClass에 맞는 class의 객체를 생성해서(new) 넘겨준다.
//----------------------------------------------------------------------
MItem *MItem::NewItem(ITEM_CLASS itemClass) {
  return (MItem *)(*s_NewItemClassTable[itemClass])();
};

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

MItem::MItem() {
  m_ObjectType = TYPE_ITEM;

  //
  m_Number = 1;

  // m_ItemClass		= ITEMCLASS_NULL;
  m_ItemType = 0; // ITEMTYPE_NULL;
  //	m_ItemOption	= 0;

  m_bDropping = FALSE; // 떨어지고 있는 중
  m_DropCount = 0;     // 현재 count

  // identify
  m_bIdentified = TRUE;

  m_pName = NULL;

  m_bAffectStatus = true;

  m_Silver = 0;

  m_Grade = -1;
  m_EnchantLevel = 0;
  m_Speed = 0;
  m_Quest = 0;

  m_OptionExt = 0;
  m_OptionExtFrame = 0;
  m_OptionExtTime = 0;
  m_OptionExtGrade = 0;
  m_ItemColorSet = 0xFFFF;
  m_persnal_price = -1;
  m_persnal = false;

  m_ParentInventory = NULL;
  m_ThirdEnchantType = 99;

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  m_CashItem = 0;
#endif

#if __CONTENTS(__GEAR_SWAP_CHANGE) // MItem::MItem() 	m_bActive	= false;
  m_bActive = false; // 아이템을 착용을 허가 하지 않는다.
#endif

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
  m_nTimeLimit_Static = 0;
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__PET_VISION_AMPLE)
  m_nPet_RedBirdOption = 255;
  m_nPet_AmpulOption = 255;
  m_nPet_op3 = 255;
#endif //__PET_VISION_AMPLE
}

MItem::~MItem() {
  if (m_pName != NULL) {
    delete[] m_pName;
  }
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Is Insert To Item ( pItem )
//----------------------------------------------------------------------
// pItem에 추가될 수 있는가?
//----------------------------------------------------------------------
bool MItem::IsInsertToItem(const MItem *pItem) const {
  if (pItem == NULL) {
    return false;
  }

  //-------------------------------------------------
  // 쌓이는 Item이고
  // class와 type이 같아야 쌓인다.
  // 현재 개수가 한계를 넘지 않은 경우
  // --> 추가 가능
  //-------------------------------------------------

  // (!)아직은 쌓이게 하면 안된다.

  ///*
  if (pItem->IsPileItem() && GetItemClass() == pItem->GetItemClass() &&
      GetItemType() == pItem->GetItemType())
  //&& pItem->GetNumber() < pItem->GetMaxNumber())
  {
    return true;
  }
  //*/

  return false;
}

//----------------------------------------------------------------------
// Get Name
// FAZ 4 (2026-05-05): Bounds-check + NULL guard. g_pItemTable[ic][it].HName
// table boş veya item type table size'ından büyükse strcat NULL crash.
// Hover-on-equipped-item crash root cause. Slayer/Vampire/Ousters etkilenir.
//----------------------------------------------------------------------
const char *MItem::GetName() {
  static const char *kEmpty = "";
  if (m_pName != NULL) {
    return m_pName;
  }

  int ic = GetItemClass();
  int it = m_ItemType;
  if (g_pItemTable == NULL || ic < 0 || ic >= g_pItemTable->GetSize()) {
    CT_Done("MItem::GetName OOB: ic=%d (table size=%d)",
               ic, (g_pItemTable != NULL) ? g_pItemTable->GetSize() : -1);
    return kEmpty;
  }
  if (it < 0 || it >= (*g_pItemTable)[ic].GetSize()) {
    CT_Done("MItem::GetName OOB: ic=%d it=%d (sub size=%d)",
               ic, it, (*g_pItemTable)[ic].GetSize());
    return kEmpty;
  }
  const char *pHName = (*g_pItemTable)[ic][it].HName;
  if (pHName == NULL) {
    CT_Done("MItem::GetName NULL HName: ic=%d it=%d", ic, it);
    return kEmpty;
  }

  // Skull → Soul Stone substitution (gore filter)
  if (g_pUserInformation != NULL && !g_pUserInformation->GoreLevel &&
      ic == ITEM_CLASS_SKULL) {
    size_t hlen = strlen(pHName);
    m_pName = new char[hlen + 1 + 5];
    strcpy(m_pName, pHName);
    if (hlen >= 4) {
      char *psz_temp = &m_pName[hlen - 4];
      const char *pSoul =
          (g_pGameStringTable != NULL)
              ? (*g_pGameStringTable)[STRING_MESSAGE_SOUL_STONE].GetString()
              : NULL;
      if (pSoul != NULL) {
        strcpy(psz_temp, pSoul);
        return m_pName;
      }
    }
  }

  return pHName;
}

//----------------------------------------------------------------------
// Get EnglishName
// FAZ 4 (2026-05-05): Bounds-check + NULL guard (GetName ile aynı pattern).
//----------------------------------------------------------------------
const char *MItem::GetEName() const {
  static const char *kEmpty = "";

  int ic = GetItemClass();
  int it = m_ItemType;
  if (g_pItemTable == NULL || ic < 0 || ic >= g_pItemTable->GetSize()) {
    CT_Done("MItem::GetEName OOB: ic=%d (table size=%d)",
               ic, (g_pItemTable != NULL) ? g_pItemTable->GetSize() : -1);
    return kEmpty;
  }
  if (it < 0 || it >= (*g_pItemTable)[ic].GetSize()) {
    CT_Done("MItem::GetEName OOB: ic=%d it=%d (sub size=%d)",
               ic, it, (*g_pItemTable)[ic].GetSize());
    return kEmpty;
  }
  const char *pEName = (*g_pItemTable)[ic][it].EName;
  if (pEName == NULL) {
    CT_Done("MItem::GetEName NULL EName: ic=%d it=%d", ic, it);
    return kEmpty;
  }

  // Skull → Soul Stone substitution (gore filter)
  if (g_pUserInformation != NULL && !g_pUserInformation->GoreLevel &&
      ic == ITEM_CLASS_SKULL) {
    static char sz_temp[256];
    strncpy(sz_temp, pEName, sizeof(sz_temp) - 1);
    sz_temp[sizeof(sz_temp) - 1] = '\0';

    char *psz_temp = strstr(sz_temp, "Head");
    if (psz_temp == NULL) psz_temp = strstr(sz_temp, "Skull");

    if (psz_temp != NULL && g_pGameStringTable != NULL) {
      const char *pSoul =
          (*g_pGameStringTable)[STRING_MESSAGE_SOUL_STONE].GetString();
      if (pSoul != NULL) {
        strcpy(psz_temp, pSoul);
        return sz_temp;
      }
    }
  }

  return pEName;
}

//----------------------------------------------------------------------
// Set Name
//----------------------------------------------------------------------
// m_pName에 이름을 정해넣는다.
//----------------------------------------------------------------------
void MItem::SetName(const char *pName) {
  if (m_pName != NULL) {
    delete[] m_pName;
    m_pName = NULL;
  }

  if (pName == NULL) {
    return;
  }

  //---------------------------------------------
  // 설정..
  //---------------------------------------------
  m_pName = new char[strlen(pName) + 1];

  strcpy(m_pName, pName);
}

//----------------------------------------------------------------------
// Get Description
//----------------------------------------------------------------------
const char *MItem::GetDescription() const {
  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return "";
  return (*g_pItemTable)[GetItemClass()][m_ItemType].Description;
}

//----------------------------------------------------------------------
// Get Weight
//----------------------------------------------------------------------
TYPE_ITEM_WEIGHT
MItem::GetWeight() const {
  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return 0;
  return (*g_pItemTable)[GetItemClass()][m_ItemType].Weight;
}

//----------------------------------------------------------------------
// Get Weight
//----------------------------------------------------------------------
TYPE_ITEM_PRICE
MItem::GetPrice() const {
  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return 0;
  TYPE_ITEM_PRICE price;
  if (GetGrade() > 0 && GetGrade() <= 10 && IsGearItem())
    price = (*g_pItemTable)[GetItemClass()][m_ItemType].Price *
            (100 + (GetGrade() - 4) * 5) / 100;
  else
    price = (*g_pItemTable)[GetItemClass()][m_ItemType].Price;
  return price;
}

//----------------------------------------------------------------------
// Get GridWidth
//----------------------------------------------------------------------
BYTE MItem::GetGridWidth() const {
  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return 1;
  return (*g_pItemTable)[GetItemClass()][m_ItemType].GridWidth;
}

//----------------------------------------------------------------------
// Get GridHeight
//----------------------------------------------------------------------
BYTE MItem::GetGridHeight() const {
  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return 1;
  return (*g_pItemTable)[GetItemClass()][m_ItemType].GridHeight;
}

//----------------------------------------------------------------------
// Get TileFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetTileFrameID() const {
  // 틴버전인 경우 머리는 보석으로 표시한다.
  if (g_pUserInformation->GoreLevel == false &&
      GetItemClass() == ITEM_CLASS_SKULL)
    return 271;

  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return 0;
  return (*g_pItemTable)[GetItemClass()][m_ItemType].TileFrameID;
}

//----------------------------------------------------------------------
// Get InventoryFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetInventoryFrameID() const {
  // 틴버전인 경우 머리는 보석으로 표시한다.
  if (g_pUserInformation->GoreLevel == false &&
      GetItemClass() == ITEM_CLASS_SKULL)
    return 285;

  if (!MITEM_TABLE_VALID(GetItemClass(), m_ItemType)) return 0;

#if __CONTENTS(__GORE_BLOOD_BIBLE)
  if (g_pUserInformation->GoreLevel == false &&
      GetItemClass() == ITEM_CLASS_CONTRACT_OF_BLOOD) {
    if ((*g_pItemTable)[GetItemClass()][m_ItemType].InventoryFrameID == 1118)
      return 1260;
    if ((*g_pItemTable)[GetItemClass()][m_ItemType].InventoryFrameID == 1176)
      return 1258;
    if ((*g_pItemTable)[GetItemClass()][m_ItemType].InventoryFrameID == 1177)
      return 1259;
    if ((*g_pItemTable)[GetItemClass()][m_ItemType].InventoryFrameID == 1178)
      return 1260;
  }
#endif //__GORE_BLOOD_BIBLE

  return (*g_pItemTable)[GetItemClass()][m_ItemType].InventoryFrameID;
}

//----------------------------------------------------------------------
// Get GearFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetGearFrameID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].GearFrameID;
}

//----------------------------------------------------------------------
// Get DropFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetDropFrameID() const {
  if (GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX && GetItemType() == 2) {
    int frameID = (*g_pItemTable)[GetItemClass()][m_ItemType].DropFrameID;
  }
  // 틴버전인 경우 머리는 보석으로 표시한다.
  if (g_pUserInformation->GoreLevel == false &&
      GetItemClass() == ITEM_CLASS_SKULL)
    return 271;

  return (*g_pItemTable)[GetItemClass()][m_ItemType].DropFrameID;
}

//----------------------------------------------------------------------
// Get AddonMaleFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetAddonMaleFrameID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].AddonMaleFrameID;
}

//----------------------------------------------------------------------
// Get AddonFemaleFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetAddonFemaleFrameID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].AddonFemaleFrameID;
}

//----------------------------------------------------------------------
// Get UseFrameID
//----------------------------------------------------------------------
TYPE_FRAMEID
MItem::GetUseFrameID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].UseFrameID;
}

//----------------------------------------------------------------------
// Get UseSoundID
//----------------------------------------------------------------------
TYPE_SOUNDID
MItem::GetUseSoundID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].UseSoundID;
}

//----------------------------------------------------------------------
// Get TileSoundID
//----------------------------------------------------------------------
TYPE_SOUNDID
MItem::GetTileSoundID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].TileSoundID;
}

//----------------------------------------------------------------------
// Get InventorySoundID
//----------------------------------------------------------------------
TYPE_SOUNDID
MItem::GetInventorySoundID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].InventorySoundID;
}

//----------------------------------------------------------------------
// Get GearSoundID
//----------------------------------------------------------------------
TYPE_SOUNDID
MItem::GetGearSoundID() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].GearSoundID;
}

//----------------------------------------------------------------------
// Get ItemMoveContorl
//----------------------------------------------------------------------
const ITEM_MOVE_CONTROL_ATTR MItem::GetItemMoveContorl() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].ItemMoveControl;
}

const ITEM_USE_CAN_ADVANCE MItem::GetItemCanAdvance() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].ItemCanAdvance;
}

//----------------------------------------------------------------------
// Is Gender For Male
//----------------------------------------------------------------------
bool MItem::IsGenderForMale() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].IsGenderForMale();
}

//----------------------------------------------------------------------
// Is Gender For Female
//----------------------------------------------------------------------
bool MItem::IsGenderForFemale() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].IsGenderForFemale();
}

//----------------------------------------------------------------------
// Is Gender For All
//----------------------------------------------------------------------
bool MItem::IsGenderForAll() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].IsGenderForAll();
}

int MItem::IsQuestItem() const {
  if (g_pTimeItemManager != NULL &&
      (g_pTimeItemManager->IsExist(GetID()) || m_Quest))
    return 1;

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
  if (IsTimeLimitStatic())
    return 1;
#endif //__PREMIUM_GIVE_ITEM_UI

  return 0;
}

#if __CONTENTS(__TUNING_ITEM)
bool MItem::IsTuningItem() const {
  if (GetItemClass() == ITEM_CLASS_TUNING_SLAYER ||
      GetItemClass() == ITEM_CLASS_TUNING_VAMPIRE ||
      GetItemClass() == ITEM_CLASS_TUNING_OUSTERS)
    return true;

  return false;
}
#endif //__TUNING_ITEM

#if __CONTENTS(__ONIBLA_ITEM)
bool MItem::IsSeniorItem() const {
  if (g_pTimeItemManager != NULL && g_pTimeItemManager->IsExist(GetID()))
    return false;
  if ((*g_pItemTable)[GetItemClass()][m_ItemType].NormalItemGrade > 0)
    return true;
  return false;
}
#endif //__ONIBLA_ITEM

bool MItem::IsSpecialColorItem() const {
  if (IsUniqueItem() || IsQuestItem()
#if __CONTENTS(__TUNING_ITEM)
      || IsTuningItem()
#endif      //__TUNING_ITEM
          ) // || IsItemOptionExt() )
    return true;

  return false;
}

//----------------------------------------------------------------------
// Potion :: Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MPotion::GetMaxNumber() const { return MAX_POTION_PILE_NUMBER; }

//----------------------------------------------------------------------
// Magazine :: Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MMagazine::GetMaxNumber() const { return MAX_MAGAZINE_PILE_NUMBER; }

//----------------------------------------------------------------------
// Money :: Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MMoney::GetMaxNumber() const { return MAX_MONEY_NUMBER; }

//----------------------------------------------------------------------
// MWater - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MWater::GetMaxNumber() const { return MAX_WATER_NUMBER; }

//----------------------------------------------------------------------
// MHolyWater - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MHolyWater::GetMaxNumber() const { return MAX_HOLY_WATER_NUMBER; }

//----------------------------------------------------------------------
// MBombMaterial - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MBombMaterial::GetMaxNumber() const { return MAX_BOMB_MATERIAL_NUMBER; }

//----------------------------------------------------------------------
// MBomb - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MBomb::GetMaxNumber() const { return MAX_BOMB_NUMBER; }

//----------------------------------------------------------------------
// MMine - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MMine::GetMaxNumber() const { return MAX_MINE_NUMBER; }

//----------------------------------------------------------------------
// MVampireETC - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MVampireETC::GetMaxNumber() const { return MAX_VAMPIRE_ETC_NUMBER; }

//----------------------------------------------------------------------
// MSerum - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MSerum::GetMaxNumber() const { return MAX_SERUM_NUMBER; }

//----------------------------------------------------------------------
// MSkull - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MSkull::GetMaxNumber() const { return MAX_SKULL_NUMBER; }

TYPE_ITEM_NUMBER
MEventEtcItem::GetMaxNumber() const { return MAX_COMMON_QUEST_ITEM_NUMBER; }

// nanomech 2006.03.08 Item 수정 작업 start
TYPE_ITEM_NUMBER
MCommonQuestItem::GetMaxNumber() const { return MAX_COMMON_QUEST_ITEM_NUMBER; }
// nanomech 2006.03.08 Item 수정 작업 end

//----------------------------------------------------------------------
// MGearItem::Get MaxDurability
//----------------------------------------------------------------------
int MGearItem::GetMaxDurability() const {
  int maxDur = (*g_pItemTable)[GetItemClass()][m_ItemType].Value1;

  if (!maxDur)
    return 0;

  maxDur += (GetGrade() - 4) * 1000;

  int plus_point = 100;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    ITEMOPTION_INFO &optionInfo = (*g_pItemOptionTable)[*itr];

    if (optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY) {
      plus_point += optionInfo.PlusPoint - 100;
    }

    itr++;
  }

  if (plus_point != 0)
    maxDur = maxDur * plus_point / 100;

  return max(min(65000, maxDur), 1000);
}

int MWeaponItem::GetMinDamage() const {
  return max(1, (*g_pItemTable)[GetItemClass()][m_ItemType].Value3 +
                    (GetGrade() - 4));
} // 최소 공격력

int MWeaponItem::GetMaxDamage() const {
  return max(1, (*g_pItemTable)[GetItemClass()][m_ItemType].Value4 +
                    (GetGrade() - 4));
} // 최대 공격력

int MWeaponItem::GetToHit() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].ToHit;
} // 최대 공격력

int MWeaponItem::GetCriticalHit() const {
  return max(0, (*g_pItemTable)[GetItemClass()][m_ItemType].CriticalHit +
                    (GetGrade() - 4) * 2);
}

int MArmorItem::GetProtectionValue() const {
  return max(0, MGearItem::GetProtectionValue() + (GetGrade() - 4));
}

int MArmorItem::GetDefenseValue() const {
  return max(0, MGearItem::GetDefenseValue() + (GetGrade() - 4) * 2);
}
int MArmorItem2::GetProtectionValue() const {
  return max(0, MGearItem::GetProtectionValue() + ((GetGrade() - 4) / 2));
}

int MArmorItem2::GetDefenseValue() const {
  return max(0, MGearItem::GetDefenseValue() + (GetGrade() - 4));
}
int MArmorItem2::GetMaxDurability() const {
  int maxDur = (*g_pItemTable)[GetItemClass()][m_ItemType].Value1;

  maxDur += (GetGrade() - 4) * 500;

  int plus_point = 100;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    ITEMOPTION_INFO &optionInfo = (*g_pItemOptionTable)[*itr];

    if (optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY) {
      plus_point += optionInfo.PlusPoint - 100;
    }

    itr++;
  }

  if (plus_point != 0)
    maxDur = maxDur * plus_point / 100;

  return max(min(65000, maxDur), 1000);
}

int MAccessoryItem::GetLucky() const { return (GetGrade() - 4); }

//----------------------------------------------------------------------
// MMotorcycle::Get MaxDurability
//----------------------------------------------------------------------
int MMotorcycle::GetMaxDurability() const {
  int maxDur = (*g_pItemTable)[ITEM_CLASS_MOTORCYCLE][m_ItemType].Value1;

  int plus_point = 100;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    ITEMOPTION_INFO &optionInfo = (*g_pItemOptionTable)[*itr];

    if (optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY) {
      plus_point += optionInfo.PlusPoint - 100;
    }

    itr++;
  }

  if (plus_point != 0)
    maxDur = maxDur * plus_point / 100;

  return min(65000, maxDur);
}

//----------------------------------------------------------------------
// Get ItemOption Part
//----------------------------------------------------------------------
BYTE MItem::GetItemOptionPart(int OptionNum) const {
  if (m_ItemOptionList.empty())
    return (*g_pItemOptionTable)[0].Part;

  int count = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end() && count < OptionNum) {
    count++;
    itr++;
  }

  if (itr != m_ItemOptionList.end())
    return (g_pItemOptionTable != NULL && *itr >= 0 && *itr < g_pItemOptionTable->GetSize()) ? (*g_pItemOptionTable)[*itr].Part : (*g_pItemOptionTable)[0].Part;
  else
    return (*g_pItemOptionTable)[0].Part;
}

//----------------------------------------------------------------------
// Get ItemOption Name
//----------------------------------------------------------------------
const char *MItem::GetItemOptionName(int OptionNum) const {
  if (m_ItemOptionList.empty())
    return (*g_pItemOptionTable)[0].Name;

  int count = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end() && count < OptionNum) {
    count++;
    itr++;
  }

  if (itr != m_ItemOptionList.end() && *itr < g_pItemOptionTable->GetSize())
    return (g_pItemOptionTable != NULL && *itr >= 0 && *itr < g_pItemOptionTable->GetSize()) ? (*g_pItemOptionTable)[*itr].Name : (*g_pItemOptionTable)[0].Name;
  else
    return (*g_pItemOptionTable)[0].Name;
}

//----------------------------------------------------------------------
// Get ItemOption Name
//----------------------------------------------------------------------
const char *MItem::GetItemOptionEName(int OptionNum) const {
  if (m_ItemOptionList.empty())
    return (*g_pItemOptionTable)[0].EName;

  int count = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end() && count < OptionNum) {
    count++;
    itr++;
  }

  if (itr != m_ItemOptionList.end() && *itr < g_pItemOptionTable->GetSize())
    return (g_pItemOptionTable != NULL && *itr >= 0 && *itr < g_pItemOptionTable->GetSize()) ? (*g_pItemOptionTable)[*itr].EName : (*g_pItemOptionTable)[0].EName;
  else
    return (*g_pItemOptionTable)[0].EName;
}

//----------------------------------------------------------------------
// Get ItemOption PlusPoint
//----------------------------------------------------------------------
int MItem::GetItemOptionPlusPoint(int OptionNum) const {
  if (m_ItemOptionList.empty())
    return (*g_pItemOptionTable)[0].PlusPoint;

  int count = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end() && count < OptionNum) {
    count++;
    itr++;
  }

  if (itr != m_ItemOptionList.end())
    return (g_pItemOptionTable != NULL && *itr >= 0 && *itr < g_pItemOptionTable->GetSize()) ? (*g_pItemOptionTable)[*itr].PlusPoint : (*g_pItemOptionTable)[0].PlusPoint;
  else
    return (*g_pItemOptionTable)[0].PlusPoint;
}

//----------------------------------------------------------------------
// Get ItemOption PlusRequireAbility
//----------------------------------------------------------------------
/*
BYTE
MItem::GetItemOptionPlusRequireAbility() const
{
        return (*g_pItemOptionTable)[m_ItemOption].PlusRequireAbility;
}
*/

//----------------------------------------------------------------------
// Get ItemOption PriceMultiplier
//----------------------------------------------------------------------
DWORD
MItem::GetItemOptionPriceMultiplier() const {
  int re = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    re += (*g_pItemOptionTable)[*itr].PriceMultiplier;
    itr++;
  }

  return re;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
WORD MItem::GetItemOptionColorSet(int OptionNum) {
  // FAZ 4 (2026-05-05): g_pItemOptionTable boş ise tüm `[0]` access OOB.
  // Inventory open crash root cause — itemId=12302 ITEM_CLASS_AR için.
  // Memory pattern: "g_pItemOptionTable size=0 OOB". Default fallback color = 0.
  const WORD kDefaultColorSet = 0;
  bool bTableValid = (g_pItemOptionTable != NULL && g_pItemOptionTable->GetSize() > 0);

  if (GetItemClass() == ITEM_CLASS_PET_ITEM) {
#ifdef __GAME_CLIENT__
    g_pPlayer->CheckAffectStatus(this);
#endif
    if (GetItemColorSet() == 0xFFFF) {
      if (GetSilver() > 0) // 속성이 있는 경우는 색 표시
      {
        ITEMOPTION_TABLE::ITEMOPTION_PART optionPart =
            static_cast<ITEMOPTION_TABLE::ITEMOPTION_PART>(GetEnchantLevel());

        if (bTableValid) {
          int size = g_pItemOptionTable->GetSize();
          for (int i = 1; i < size; i++) {
            ITEMOPTION_INFO &optionInfo = g_pItemOptionTable->Get(i);
            if (optionInfo.Part == optionPart &&
                optionInfo.UpgradeOptionType == 0) {
              SetItemColorSet(optionInfo.ColorSet);
              break;
            }
          }
        }
      } else {
        SetItemColorSet(bTableValid ? (*g_pItemOptionTable)[0].ColorSet : kDefaultColorSet);
      }
    }

    return GetItemColorSet();
  }

  if (IsQuestItem())
    return QUEST_ITEM_COLOR;

  if (IsUniqueItem())
    return UNIQUE_ITEM_COLOR;

  if (m_ItemOptionList.empty())
    return bTableValid ? (*g_pItemOptionTable)[0].ColorSet : kDefaultColorSet;

  int count = 0;
  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end() && count < OptionNum) {
    count++;
    itr++;
  }

  if (itr != m_ItemOptionList.end()) {
    return (bTableValid && *itr >= 0 && *itr < g_pItemOptionTable->GetSize())
               ? (*g_pItemOptionTable)[*itr].ColorSet
               : (bTableValid ? (*g_pItemOptionTable)[0].ColorSet : kDefaultColorSet);
  }
  return bTableValid ? (*g_pItemOptionTable)[0].ColorSet : kDefaultColorSet;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
int MItem::GetItemOptionRequireSTR() const {
  int re = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    re += (*g_pItemOptionTable)[*itr].RequireSTR;
    itr++;
  }

  return re;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
int MItem::GetItemOptionRequireDEX() const {
  int re = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    re += (*g_pItemOptionTable)[*itr].RequireDEX;
    itr++;
  }

  return re;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
int MItem::GetItemOptionRequireINT() const {
  int re = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    re += (*g_pItemOptionTable)[*itr].RequireINT;
    itr++;
  }

  return re;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
int MItem::GetItemOptionRequireLevel() const {
  int re = 0;

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    if (g_pItemOptionTable == NULL || *itr < 0 || *itr >= g_pItemOptionTable->GetSize()) { itr++; continue; }
    re += (*g_pItemOptionTable)[*itr].RequireLevel;
    itr++;
  }

  return re;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
int MItem::GetItemOptionRequireSUM() const {
  int re = 0;

  // FAZ 4 (2026-05-04): bounds-check (Empty global table OOB pattern).
  // Item'ın option list'inde g_pItemOptionTable size'ından büyük ID
  // varsa OOB → NULL+offset read → crash. Crash dump (00:30):
  // MItem.cpp:1243 → MCreature.cpp:5687 zinciri ile reproduce edildi.
  if (g_pItemOptionTable == NULL) return 0;
  const int tableSize = g_pItemOptionTable->GetSize();

  std::list<TYPE_ITEM_OPTION>::const_iterator itr = m_ItemOptionList.begin();

  while (itr != m_ItemOptionList.end()) {
    const TYPE_ITEM_OPTION optId = *itr;
    if (optId >= 0 && optId < tableSize) {
      re += (*g_pItemOptionTable)[optId].RequireSUM;
    }
    itr++;
  }

  return re;
}

//----------------------------------------------------------------------
// Get ItemOption ColorSet
//----------------------------------------------------------------------
int MItem::IsItemOptionExt() // const
{
  if (g_pTimeItemManager != NULL &&
      (g_pTimeItemManager->IsExist(GetID()) ||
       GetItemOptionColorSet() >=
           OPTIONEXT_ITEM_COLOR)) //   GetItemOptionColorSet() )) //   >= 495 ))
                                  //   //   m_OptionExt ) )
    return 1;
  return 0;
}

//----------------------------------------------------------------------
// Get Require STR
//----------------------------------------------------------------------
int MItem::GetRequireSTR() const {
  int original = (*g_pItemTable)[GetItemClass()][m_ItemType].GetRequireSTR();
  int maxValue = 0;

  if (original == 0 || IsQuestItem()) {
    return 0;
  }

  if (original <= MAX_SLAYER_ATTR_OLD)
    maxValue = MAX_SLAYER_ATTR_OLD;
  else
    maxValue = MAX_SLAYER_ATTR;

  original += (GetItemOptionRequireSUM() << 1);

  if (IsOustersItem())
    return original;

  return min(original, maxValue);

  // option에 따른 증가치
  // return max(original, GetItemOptionRequireSTR());
  //(*g_pItemOptionTable)[m_ItemOption].PlusRequireAbility;
}

//----------------------------------------------------------------------
// Get Require DEX
//----------------------------------------------------------------------
int MItem::GetRequireDEX() const {
  int original = (*g_pItemTable)[GetItemClass()][m_ItemType].GetRequireDEX();
  int maxValue = 0;

  if (original == 0 || IsQuestItem()) {
    return 0;
  }

  if (original <= MAX_SLAYER_ATTR_OLD)
    maxValue = MAX_SLAYER_ATTR_OLD;
  else
    maxValue = MAX_SLAYER_ATTR;

  original += (GetItemOptionRequireSUM() << 1);

  if (IsOustersItem())
    return original;

  return min(original, maxValue);

  // option에 따른 증가치
  // return max(original, GetItemOptionRequireDEX());
  //+ (*g_pItemOptionTable)[m_ItemOption].PlusRequireAbility;
}

//----------------------------------------------------------------------
// Get Require INT
//----------------------------------------------------------------------
int MItem::GetRequireINT() const {
  int original = (*g_pItemTable)[GetItemClass()][m_ItemType].GetRequireINT();
  int maxValue = 0;

  if (original == 0 || IsQuestItem()) {
    return 0;
  }

  if (original <= MAX_SLAYER_ATTR_OLD)
    maxValue = MAX_SLAYER_ATTR_OLD;
  else
    maxValue = MAX_SLAYER_ATTR;

  original += (GetItemOptionRequireSUM() << 1);

  if (IsOustersItem())
    return original;

  return min(original, maxValue);

  // option에 따른 증가치
  // return max(original, GetItemOptionRequireINT());
  //+ (*g_pItemOptionTable)[m_ItemOption].PlusRequireAbility;
}

//----------------------------------------------------------------------
// Get Require SUM
//----------------------------------------------------------------------
int MItem::GetRequireSUM() const {
  int original = (*g_pItemTable)[GetItemClass()][m_ItemType].GetRequireSUM();
  int maxValue = 0;

  if (original == 0 || IsQuestItem()) {
    return 0;
  }
  if (original <= MAX_SLAYER_ATTR_SUM_OLD)
    maxValue = MAX_SLAYER_ATTR_SUM_OLD;
  else
    maxValue = MAX_SLAYER_ATTR_SUM;
  original += GetItemOptionRequireSUM();

  if (IsOustersItem())
    return original;

  return min(original, maxValue);

  // option에 따른 증가치
  // return max(original, GetItemOptionRequireSUM());
  //+ (*g_pItemOptionTable)[m_ItemOption].PlusRequireAbility;
}

//----------------------------------------------------------------------
// Get Require Level
//----------------------------------------------------------------------
int MItem::GetRequireLevel() const {
  int original = (*g_pItemTable)[GetItemClass()][m_ItemType].GetRequireLevel();
  int maxValue = 0;

  if (original == 0 || IsQuestItem())
    return 0;

  if (original <= 100)
    maxValue = MAX_VAMPIRE_LEVEL_OLD;
  else
    maxValue = MAX_VAMPIRE_LEVEL;

  // option에 따른 증가치
  original += GetItemOptionRequireLevel();

  if (IsOustersItem())
    return original;

  return min(original, maxValue);
  // return max(original, GetItemOptionRequireLevel());
  //+ (*g_pItemOptionTable)[m_ItemOption].PlusRequireAbility;
}

//----------------------------------------------------------------------
// Get UseActionInfo
//----------------------------------------------------------------------
TYPE_ACTIONINFO
MItem::GetUseActionInfo() const {
  if (GetItemClass() == ITEM_CLASS_SWORD)
    return SKILL_ATTACK_SWORD;
  else if (GetItemClass() == ITEM_CLASS_BLADE ||
           GetItemClass() == ITEM_CLASS_CROSS ||
           GetItemClass() == ITEM_CLASS_MACE)
    return SKILL_ATTACK_BLADE;
  else if (GetItemClass() == ITEM_CLASS_AR)
    return SKILL_ATTACK_GUN_AR;
  else if (GetItemClass() == ITEM_CLASS_SR)
    return SKILL_ATTACK_GUN_SR;
  else if (GetItemClass() == ITEM_CLASS_SG)
    return SKILL_ATTACK_GUN_SG;
  else if (GetItemClass() == ITEM_CLASS_SMG)
    return SKILL_ATTACK_GUN_SMG;
  else
    return (*g_pItemTable)[GetItemClass()][m_ItemType].UseActionInfo;
}

//----------------------------------------------------------------------
// Get Original Speed
//----------------------------------------------------------------------
int MItem::GetOriginalSpeed() const {
  return (*g_pItemTable)[GetItemClass()][m_ItemType].Value7;
}

//----------------------------------------------------------------------
// Get Speed
//----------------------------------------------------------------------
int MItem::GetSpeed() const { return m_Speed; }

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
/*
void
MItem::SaveToFile(std::ofstream& file)
{
        MObject::SaveToFile(file);
        file.write((const char*)&m_SpriteID, SIZE_SPRITEID);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void
MItem::LoadFromFile(std::ifstream& file)
{
        MObject::LoadFromFile(file);
        file.read((char*)&m_SpriteID, SIZE_SPRITEID);
}
*/

//----------------------------------------------------------------------
// Set Dropping
//----------------------------------------------------------------------
void MItem::SetDropping() {
  TYPE_FRAMEID dropID = GetDropFrameID();
  // 관련된 frame이 없는 경우
  if (dropID == FRAMEID_NULL
#ifdef __GAME_CLIENT__
      || dropID >= g_pTopView->m_ItemDropFPK.GetSize()
#endif
  ) {
    return;
  }

#ifdef __GAME_CLIENT__
  CAnimationFrame::SetFrameID(dropID,
                              g_pTopView->m_ItemDropFPK[dropID].GetSize());
#endif

  m_DropCount = 0;
  m_bDropping = TRUE;
}

//----------------------------------------------------------------------
// Next Drop Frame
//----------------------------------------------------------------------
void MItem::NextDropFrame() {
  CAnimationFrame::NextFrame();

  if (++m_DropCount >= MAX_DROP_COUNT) {
    m_CurrentFrame = m_MaxFrame - 1; // 마지막 frame으로..
    m_DropCount--;                   // 하나 빼줘야 하는 듯 - -;
    m_bDropping = FALSE;
  }
}

void MItem::RemoveItemOption(TYPE_ITEM_OPTION option) {
  std::list<TYPE_ITEM_OPTION>::iterator itr =
      std::find(m_ItemOptionList.begin(), m_ItemOptionList.end(), option);

  if (itr != m_ItemOptionList.end())
    m_ItemOptionList.erase(itr);
}

void MItem::AddItemOption(TYPE_ITEM_OPTION option) {
  if (option == 0)
    return;

  m_ItemOptionList.push_back(option);
}

void MItem::ChangeItemOption(TYPE_ITEM_OPTION ori_option,
                             TYPE_ITEM_OPTION new_option) {
  if (new_option == 0)
    RemoveItemOption(ori_option);

  std::list<TYPE_ITEM_OPTION>::iterator itr =
      std::find(m_ItemOptionList.begin(), m_ItemOptionList.end(), ori_option);

  if (itr != m_ItemOptionList.end()) {
    *itr = new_option;
  }
}

// 20077009 by diesirace 일단 선언 나중에 사용하면 채우도록.
void MItem::RemoveItemThirdOption(TYPE_ITEM_OPTION thirdOption) {}
void MItem::AddItemThirdOption(TYPE_ITEM_OPTION thirdOption) {}
void MItem::ChangeItemThirdOption(std::list<TYPE_ITEM_OPTION> &thirdOption) {
  m_ItemThirdOptionList = thirdOption;
}

void MItem::RemoveItemGTwoOption(TYPE_ITEM_OPTION GTwooption) {
  std::list<TYPE_ITEM_OPTION>::iterator itr = std::find(
      m_GTwoItemOptionList.begin(), m_GTwoItemOptionList.end(), GTwooption);

  if (itr != m_GTwoItemOptionList.end())
    m_GTwoItemOptionList.erase(itr);
}
void MItem::AddItemGTwoOption(TYPE_ITEM_OPTION GTwooption) {
  if (GTwooption == 0)
    return;

  m_GTwoItemOptionList.push_back(GTwooption);
}

void MItem::ChangeItemGTwoOption(TYPE_ITEM_OPTION GTwoori_option,
                                 TYPE_ITEM_OPTION GTwonew_option) {
  if (GTwonew_option == 0)
    RemoveItemGTwoOption(GTwoori_option);

  std::list<TYPE_ITEM_OPTION>::iterator itr = std::find(
      m_GTwoItemOptionList.begin(), m_GTwoItemOptionList.end(), GTwoori_option);

  if (itr != m_GTwoItemOptionList.end()) {
    *itr = GTwonew_option;
  }
}
// 유니크 아이템 색변하는거
int MItem::GetUniqueItemColorset() {
  int grade = g_CurrentFrame % 28; // 15*2-2
  if (grade > 14)
    grade = 28 - grade;
  return g_pClientConfig->UniqueItemColorSet + grade;
}

// item_style=2: isim soldan sağa Sol-Orta-Sağ yumuşak RGB geçişi.
// drop_item_name_tag 2..8 = durak sayısı (3 = kırmızı-yeşil-mavi); 0/1 veya diğer = 8.
static COLORREF NameCycleLerp(COLORREF a, COLORREF b, int frac256) {
  if (frac256 <= 0)
    return a;
  if (frac256 >= 255)
    return b;
  const int ra = GetRValue(a);
  const int ga = GetGValue(a);
  const int ba = GetBValue(a);
  const int rb = GetRValue(b);
  const int gb = GetGValue(b);
  const int bb = GetBValue(b);
  return RGB(ra + ((rb - ra) * frac256) / 255,
             ga + ((gb - ga) * frac256) / 255,
             ba + ((bb - ba) * frac256) / 255);
}

static int NameCycleStopCount(BYTE itemClass, WORD itemType) {
  DWORD tag = 0;
  if (MITEM_TABLE_VALID(itemClass, itemType))
    tag = (*g_pItemTable)[itemClass][itemType].DropItemNameTag;
  int count = (int)tag;
  if (count < 2 || count > 8)
    count = 8;
  return count;
}

static COLORREF NameCycleStop(int stopIndex, int count) {
  static const COLORREF kRGB3[3] = {
      RGB(255, 48, 48),  // sol
      RGB(48, 220, 80),  // orta
      RGB(56, 96, 255),  // sağ
  };
  static const COLORREF kRainbow[8] = {
      RGB(255, 48, 48),  RGB(255, 140, 32), RGB(255, 220, 48), RGB(48, 220, 72),
      RGB(32, 200, 255), RGB(64, 96, 255),  RGB(188, 72, 255), RGB(255, 72, 180),
  };
  if (count == 3)
    return kRGB3[((stopIndex % 3) + 3) % 3];
  int idx = stopIndex % 8;
  if (idx < 0)
    idx += 8;
  if (count == 8)
    return kRainbow[idx];
  idx = (stopIndex * 8) / count;
  if (idx < 0)
    idx = 0;
  if (idx > 7)
    idx = 7;
  return kRainbow[idx];
}

int MItem::ReadNameGlyph(const char *str, int offset, char out[3]) {
  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  if (str == NULL || offset < 0 || str[offset] == '\0')
    return 0;
  const unsigned char lead = (unsigned char)str[offset];
  if (IsDBCSLeadByte(lead) && str[offset + 1] != '\0') {
    out[0] = str[offset];
    out[1] = str[offset + 1];
    return 2;
  }
  out[0] = str[offset];
  return 1;
}

int MItem::CountNameGlyphs(const char *str) {
  if (str == NULL || str[0] == '\0')
    return 0;
  int count = 0;
  int offset = 0;
  char glyph[3];
  while (str[offset] != '\0') {
    const int bytes = ReadNameGlyph(str, offset, glyph);
    if (bytes <= 0)
      break;
    offset += bytes;
    count++;
  }
  return count;
}

COLORREF MItem::GetNameColorCycleAtGlyph(int glyphIndex, int glyphCount) const {
  const int count = NameCycleStopCount(GetItemClass(), m_ItemType);
  const int framesPerSeg = 16;
  const int cycleFrames = count * framesPerSeg;
  const int frameIn = (int)(g_CurrentFrame % (DWORD)cycleFrames);
  const int phase256 = (frameIn * 256) / framesPerSeg;

  int spatialAdd = 0;
  if (glyphCount > 1) {
    if (glyphIndex < 0)
      glyphIndex = 0;
    if (glyphIndex >= glyphCount)
      glyphIndex = glyphCount - 1;
    spatialAdd = (glyphIndex * (count - 1) * 256) / (glyphCount - 1);
  }

  const int modulus = count * 256;
  int u = (phase256 + spatialAdd) % modulus;
  if (u < 0)
    u += modulus;
  const int seg = u / 256;
  const int frac = u % 256;
  const COLORREF a = NameCycleStop(seg, count);
  const COLORREF b = NameCycleStop((seg + 1) % count, count);
  return NameCycleLerp(a, b, frac);
}

COLORREF MItem::GetNameColorCycle() const {
  return GetNameColorCycleAtGlyph(0, 1);
}
#if __CONTENTS(__LEVEL_WAR_RENEWAL)
int MItem::GetLevelWarItemColorset() {
  int grade = g_CurrentFrame % 28; // 15*2-2
  if (grade > 14)
    grade = 28 - grade;
  return g_pClientConfig->UniqueItemColorSet + grade;
}
#endif //__LEVEL_WAR_RENEWAL
int MItem::GetQuestItemColorset() {
  int grade = g_CurrentFrame % 28;

  if (grade > 14)
    grade = 28 - grade;

  return g_pClientConfig->QuestItemColorSet + grade;
}

#if __CONTENTS(__TUNING_ITEM)
int MItem::GetTuningItemColorSet() {
  int grade = g_CurrentFrame % 28;

  if (grade > 14)
    grade = 28 - grade;

  return (210) + grade;
}
#endif //__TUNING_ITEM

int MItem::GetItemOptionExtColorSet(WORD wColorSet) {
  /*
  int OptionExtGrade  = g_CurrentFrame%28;	// 15*2-2
  if(OptionExtGrade > 14)
          OptionExtGrade = 28-OptionExtGrade;
  return wColorSet+OptionExtGrade; //   m_ItemColorSet
  */

  int OptionExtGrade = g_CurrentFrame % 28; // 13*2-2
  if (OptionExtGrade > 14)
    OptionExtGrade = 28 - OptionExtGrade;
  return wColorSet + OptionExtGrade; //   m_ItemColorSet
}

#if __CONTENTS(__ONIBLA_ITEM)
int MItem::GetSeniorItemColorSet() {
  int grade = g_CurrentFrame % 28;
  if (grade > 14)
    grade = 28 - grade;
  return (*g_pItemTable)[GetItemClass()][m_ItemType].NormalItemGrade + grade;
}
#endif //__ONIBLA_ITEM

int MItem::GetSpecialColorItemColorset() {
#if __CONTENTS(__TUNING_ITEM)
  if (IsTuningItem()) {
    return GetTuningItemColorSet();
  } else
#endif //__TUNING_ITEM
    if (IsQuestItem()) {
      return GetQuestItemColorset();
    } else if (IsUniqueItem()) {
      return GetUniqueItemColorset();
    } else if (IsItemOptionExt()) {
      return GetItemOptionExtColorSet();
    }
  return 0xffff;
}

int MItem::GetSpecialColorItemColorset(unsigned short srcColor) {
  if (srcColor == QUEST_ITEM_COLOR)
    return MItem::GetQuestItemColorset();
  else if (srcColor == UNIQUE_ITEM_COLOR)
    return MItem::GetUniqueItemColorset();
#if __CONTENTS(__LEVEL_WAR_RENEWAL)
  else if (srcColor == LEVEL_WAR_ITEM_COLOR)
    return MItem::GetLevelWarItemColorset();
#endif //__LEVEL_WAR_RENEWAL
  else if (srcColor >= OPTIONEXT_ITEM_COLOR)
    return MItem::GetItemOptionExtColorSet(srcColor);

  return srcColor;
}

// 레어 아이템 색변하는거
int MItem::GetRareItemColorset() {
  /*
          int ColorSet = g_CurrentFrame % (28*GetItemOptionListCount());
          int percent = g_CurrentFrame%28;

          ColorSet = GetItemOptionColorSet(ColorSet/28);
          int grade = ColorSet%15;
          ColorSet = ColorSet - grade;

          if(percent > 14)
                  percent = 28 - percent;
  */

  return GetItemOptionColorSet(); // ColorSet+grade;//*percent/14;
}

int MItem::GetItemOptionExtColorSet() {
  /*
          if( (GetTickCount() - m_OptionExtTime)  >=  150) //    OptionExtCnt >=
     20)
          {

                  m_OptionExtGrade  = g_CurrentFrame%28;	// 15*2-2
                  if(m_OptionExtGrade > 14)
                          m_OptionExtGrade = 28-m_OptionExtGrade;
                  m_OptionExtTime = GetTickCount() ;
          }
          return GetItemOptionColorSet()+m_OptionExtGrade; //   m_ItemColorSet
  */

  const int OPTIONEXT_BRIGHT_MAX = 15;

  if (GetTickCount() >= m_OptionExtTime) //    OptionExtCnt >= 20)
  {
    m_OptionExtGrade = m_OptionExtFrame; // 15*2-2

    if (m_OptionExtGrade > OPTIONEXT_BRIGHT_MAX - 1)
      m_OptionExtGrade = (OPTIONEXT_BRIGHT_MAX * 2 - 2) - m_OptionExtGrade;

    if (m_OptionExtGrade == OPTIONEXT_BRIGHT_MAX - 1)
      m_OptionExtTime = GetTickCount() + 2000;
    else
      m_OptionExtTime = GetTickCount() + 50;

    if (++m_OptionExtFrame >= (OPTIONEXT_BRIGHT_MAX * 2 - 2))
      m_OptionExtFrame = 0;
  }
  return GetItemOptionColorSet() + m_OptionExtGrade; //   m_ItemColorSet
}

/*
int MItem::GetItemNameColorSet()
{
        if( IsQuestItem())
                return GetQuestItemColorset();
        else if( IsUniqueItem())
                return GetUniqueItemColorset();
        else
                return GetItemOptionExtColorSet();
}


COLORREF MItem::GetItemNameColor()
{
        WORD wColor = CIndexSprite::ColorSet[GetItemNameColorSet][15];
        return RGB(CDirectDraw::Red(wColor)<<3, CDirectDraw::Green(wColor)<<3,
CDirectDraw::Blue(wColor)<<3);
}

COLORREF MItem::GetItemNameShadowColor()
{
        WORD wColor = CIndexSprite::ColorSet[GetItemNameColorSet][29];
        return RGB(CDirectDraw::Red(wColor)<<3, CDirectDraw::Green(wColor)<<3,
CDirectDraw::Blue(wColor)<<3);
}
*/

//----------------------------------------------------------------------
//
//						MCorpse
//
//----------------------------------------------------------------------
MCorpse::MCorpse() { m_pCreature = NULL; }

MCorpse::~MCorpse() {
#ifdef __GAME_CLIENT__
  if (m_pCreature != NULL) {
    delete m_pCreature;
  }
#endif
}

//----------------------------------------------------------------------
//
//						MBelt
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set ItemType
//----------------------------------------------------------------------
// ItemType을 설정할때 SlotItemManager도 초기화해야한다.
//----------------------------------------------------------------------
void MBelt::SetItemType(TYPE_ITEMTYPE type) {
  m_ItemType = type;

  MSlotItemManager::Init(GetPocketNumber());
}

//----------------------------------------------------------------------
// AddItem ( pItem )
//----------------------------------------------------------------------
// 적절한 slot에 pItem을 추가한다.
//----------------------------------------------------------------------
bool MBelt::AddItem(MItem *pItem) {
  //---------------------------------------------------------------
  // 비어있는 slot을 찾아서 item을 추가한다.
  //---------------------------------------------------------------
  for (int n = 0; n < m_Size; n++) {
    if (m_ItemSlot[n] == NULL) {
      return AddItem(pItem, n);
    }
  }

  return false;
}

//----------------------------------------------------------------------
// AddItem ( pItem, n )
//----------------------------------------------------------------------
// slot(n)에 pItem을 추가한다.
//----------------------------------------------------------------------
bool MBelt::AddItem(MItem *pItem, BYTE n) {
  if (n >= m_Size) {
    return false;
  }

  //---------------------------------------------------------------
  // Quick item이어야지 belt에 추가할 수 있다.
  //---------------------------------------------------------------
  if (pItem->IsQuickItem()) {
    return MSlotItemManager::AddItem(pItem, n);
  }

  return false;
}

//----------------------------------------------------------------------
// ReplaceItem
//----------------------------------------------------------------------
// pItem을 추가하고 딴게 있다면 Item교환
//----------------------------------------------------------------------
bool MBelt::ReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem) {
  if (n >= m_Size || pItem == NULL) {
    return false;
  }

  //---------------------------------------------------------------
  // 비어 있는 slot이면 그냥 추가
  //---------------------------------------------------------------
  if (m_ItemSlot[n] == NULL) {
    return AddItem(pItem, n);
  }

  //---------------------------------------------------------------
  // 뭔가 있다면 replace해야 한다.
  // 바꿀려는 Item이 quickItem인 경우만 교체..
  //---------------------------------------------------------------
  if (pItem->IsQuickItem()) {
    return MSlotItemManager::ReplaceItem(pItem, n, pOldItem);
  }

  return false;
}

//----------------------------------------------------------------------
// Can ReplaceItem : (n) slot에 pItem을 추가하거나
//						원래 있던 Item과 교체가
//가능한가?
//----------------------------------------------------------------------
bool MBelt::CanReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem) {
  //---------------------------------------------------------
  // ItemSlot 범위를 넘어가는 경우..
  //---------------------------------------------------------
  if (n >= m_Size) {
    // NULL로 설정한다.
    pOldItem = NULL;

    return false;
  }

  //---------------------------------------------------------
  // QuickItem인가?
  //---------------------------------------------------------
  if (pItem->IsQuickItem()) {
    pOldItem = m_ItemSlot[n];

    return true;
  }

  pOldItem = NULL;

  return false;
}

//----------------------------------------------------------------------
// Find Slot To Add Item
//----------------------------------------------------------------------
// return값은 자리가 있느냐(true) / 없느냐(false)
// true인 경우에.. pItem이 들어갈 수 있는 자리가 slot이다.
//----------------------------------------------------------------------
bool MBelt::FindSlotToAddItem(MItem *pItem, int &slot) const {
  if (!pItem->IsQuickItem()) {
    return false;
  }

  //---------------------------------------------------------
  // 어디에 들어갈 수 있을까?
  //---------------------------------------------------------
  for (int i = 0; i < m_Size; i++) {
    const MItem *pQuickItem = m_ItemSlot[i];

    //---------------------------------------------------------
    // 아무것도 없는 곳이면 그냥 넣으면 된다.
    //---------------------------------------------------------
    if (pQuickItem == NULL) {
      slot = i;
      return true;
    }
    //---------------------------------------------------------
    // 뭔가 있으면.. 그곳에 쌓일 수 있는지 알아본다.
    //---------------------------------------------------------
    if (pQuickItem->GetItemClass() == pItem->GetItemClass() &&
        pQuickItem->GetItemType() == pItem->GetItemType()) {
      //----------------------------------------------------
      // 더한 개수가 max를 넘지 않아야 한다.
      //----------------------------------------------------
      int addTotal = pQuickItem->GetNumber() + pItem->GetNumber();

      if (addTotal <= pQuickItem->GetMaxNumber()) {
        slot = i;
        return true;
      }
    }
  }

  // 들어갈 곳이 없다.
  return false;
}

//----------------------------------------------------------------------
//
//						MMagazine
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Is Insert To Item
//----------------------------------------------------------------------
// 다른 Item에 추가될 수 있는가?
//----------------------------------------------------------------------
bool MMagazine::IsInsertToItem(const MItem *pItem) const {
  if (pItem == NULL) {
    return false;
  }

  //------------------------------------------------
  // 총인 경우
  //------------------------------------------------
  if (pItem->IsGunItem()) {
    // 이 탄창을 장착할 수 있는 총인 경우..
    if (GetGunClass() == pItem->GetItemClass()) {
      return true;
    }

    return false;
  }

  // 같은 탄창인 경우 체크..
  return MItem::IsInsertToItem(pItem);
}

//----------------------------------------------------------------------
//
//						MSlayer PortalItem
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Get MaxNumber - Charge개수
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MSlayerPortalItem::GetMaxNumber() const {
  return (*g_pItemTable)[ITEM_CLASS_SLAYER_PORTAL_ITEM][m_ItemType].MaxNumber;
}

//----------------------------------------------------------------------
// Set EnchantLevel
// 실제로는 Charge 개수를 저장한다.
//----------------------------------------------------------------------
void MSlayerPortalItem::SetEnchantLevel(WORD s) { m_Number = s; }

//----------------------------------------------------------------------
//
//						MVampire PortalItem
//
//----------------------------------------------------------------------
MVampirePortalItem::MVampirePortalItem() {
  m_Number = 0;

  m_ZoneID = 0; // OBJECTID_NULL;
  m_ZoneX = SECTORPOSITION_NULL;
  m_ZoneY = SECTORPOSITION_NULL;
}

//----------------------------------------------------------------------
// Is Marked
//----------------------------------------------------------------------
bool MVampirePortalItem::IsMarked() const {
  return m_ZoneID != 0 && m_ZoneID != OBJECTID_NULL;
}

//----------------------------------------------------------------------
// Get MaxNumber - Charge개수
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MVampirePortalItem::GetMaxNumber() const {
  return (*g_pItemTable)[ITEM_CLASS_VAMPIRE_PORTAL_ITEM][m_ItemType].MaxNumber;
}

//----------------------------------------------------------------------
// Set Zone
//----------------------------------------------------------------------
void MVampirePortalItem::SetZone(int zoneID, TYPE_SECTORPOSITION x,
                                 TYPE_SECTORPOSITION y) {
  m_ZoneID = zoneID;
  m_ZoneX = x;
  m_ZoneY = y;
}

//----------------------------------------------------------------------
// Set EnchantLevel
// 실제로는 Charge 개수를 저장한다.
//----------------------------------------------------------------------
void MVampirePortalItem::SetEnchantLevel(WORD s) { m_Number = s; }

//----------------------------------------------------------------------
// Set Silver
//----------------------------------------------------------------------
void MVampirePortalItem::SetSilver(int s) { m_ZoneID = s; }

//----------------------------------------------------------------------
// Set Current Durability
//----------------------------------------------------------------------
void MVampirePortalItem::SetCurrentDurability(TYPE_ITEM_DURATION d) {
  m_ZoneX = (d >> 8) & 0xFF; // 0xFF는 없어도 될 듯..
  m_ZoneY = (d & 0xFF);
}

//----------------------------------------------------------------------
// Get Max Number
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MEventStarItem::GetMaxNumber() const { return MAX_EVENT_STAR_PILE_NUMBER; }

//----------------------------------------------------------------------
// Get Max Number
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MPetEnchantItem::GetMaxNumber() const { return MAX_PET_ENCHANT_PILE_NUMBER; }

//----------------------------------------------------------------------
// Get Max Number
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MPetFood::GetMaxNumber() const { return MAX_PET_FOOD_PILE_NUMBER; }
TYPE_ITEM_NUMBER
MItemETC::GetMaxNumber() const { return MAX_RESURRECT_SCROLL_NUMBER; }

TYPE_ITEM_NUMBER
MResurrectItem::GetMaxNumber() const { return MAX_RESURRECT_SCROLL_NUMBER; }

TYPE_ITEM_NUMBER
MMixingItem::GetMaxNumber() const { return MAX_MIXING_ITEM_NUMBER; }

//----------------------------------------------------------------------
// Is BombMaterial
//----------------------------------------------------------------------
// 이거는 나중에 필살~수정이 필요하다.
// BombMaterial에 member로 넣든지..
// Bomb / Mine Material을 분리하든지....
//----------------------------------------------------------------------
bool IsBombMaterial(const MItem *pItem) {
  return pItem->GetItemClass() == ITEM_CLASS_BOMB_MATERIAL &&
         pItem->GetItemType() <= 4;
}

//----------------------------------------------------------------------
// Is MineMaterial
//----------------------------------------------------------------------
// 이거는 나중에 필살~수정이 필요하다.
// BombMaterial에 member로 넣든지..
// Bomb / Mine Material을 분리하든지....
//----------------------------------------------------------------------
bool IsMineMaterial(const MItem *pItem) {
  return pItem->GetItemClass() == ITEM_CLASS_BOMB_MATERIAL &&
         pItem->GetItemType() >= 5;
}

TYPE_ITEM_NUMBER
MOustersSummonGem::GetMaxNumber() const {
  return (*g_pItemTable)[ITEM_CLASS_OUSTERS_SUMMON_ITEM][m_ItemType].MaxNumber;
}

TYPE_ITEM_NUMBER
MOustersLarva::GetMaxNumber() const { return MAX_OUSTERS_LARVA_NUMBER; }

TYPE_ITEM_NUMBER
MOustersPupa::GetMaxNumber() const { return MAX_OUSTERS_LARVA_NUMBER; }

TYPE_ITEM_NUMBER
MOustersComposMei::GetMaxNumber() const { return MAX_OUSTERS_LARVA_NUMBER; }

//----------------------------------------------------------------------
// Set EnchantLevel
// 실제로는 Charge 개수를 저장한다.
//----------------------------------------------------------------------
void MOustersSummonGem::SetEnchantLevel(WORD s) { m_Number = s; }
//----------------------------------------------------------------------
//
//						MOustersArmsBand
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set ItemType
//----------------------------------------------------------------------
// ItemType을 설정할때 SlotItemManager도 초기화해야한다.
//----------------------------------------------------------------------
void MOustersArmsBand::SetItemType(TYPE_ITEMTYPE type) {
  m_ItemType = type;

  MSlotItemManager::Init(GetPocketNumber());
}

//----------------------------------------------------------------------
// AddItem ( pItem )
//----------------------------------------------------------------------
// 적절한 slot에 pItem을 추가한다.
//----------------------------------------------------------------------
bool MOustersArmsBand::AddItem(MItem *pItem) {
  //---------------------------------------------------------------
  // 비어있는 slot을 찾아서 item을 추가한다.
  //---------------------------------------------------------------
  for (int n = 0; n < m_Size; n++) {
    if (m_ItemSlot[n] == NULL) {
      return AddItem(pItem, n);
    }
  }

  return false;
}

//----------------------------------------------------------------------
// AddItem ( pItem, n )
//----------------------------------------------------------------------
// slot(n)에 pItem을 추가한다.
//----------------------------------------------------------------------
bool MOustersArmsBand::AddItem(MItem *pItem, BYTE n) {
  if (n >= m_Size) {
    return false;
  }

  //---------------------------------------------------------------
  // Quick item이어야지 belt에 추가할 수 있다.
  //---------------------------------------------------------------
  if (pItem->IsQuickItem()) {
    return MSlotItemManager::AddItem(pItem, n);
  }

  return false;
}

//----------------------------------------------------------------------
// ReplaceItem
//----------------------------------------------------------------------
// pItem을 추가하고 딴게 있다면 Item교환
//----------------------------------------------------------------------
bool MOustersArmsBand::ReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem) {
  if (n >= m_Size || pItem == NULL) {
    return false;
  }

  //---------------------------------------------------------------
  // 비어 있는 slot이면 그냥 추가
  //---------------------------------------------------------------
  if (m_ItemSlot[n] == NULL) {
    return AddItem(pItem, n);
  }

  //---------------------------------------------------------------
  // 뭔가 있다면 replace해야 한다.
  // 바꿀려는 Item이 quickItem인 경우만 교체..
  //---------------------------------------------------------------
  if (pItem->IsQuickItem()) {
    return MSlotItemManager::ReplaceItem(pItem, n, pOldItem);
  }

  return false;
}

//----------------------------------------------------------------------
// Can ReplaceItem : (n) slot에 pItem을 추가하거나
//						원래 있던 Item과 교체가
//가능한가?
//----------------------------------------------------------------------
bool MOustersArmsBand::CanReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem) {
  //---------------------------------------------------------
  // ItemSlot 범위를 넘어가는 경우..
  //---------------------------------------------------------
  if (n >= m_Size) {
    // NULL로 설정한다.
    pOldItem = NULL;

    return false;
  }

  //---------------------------------------------------------
  // QuickItem인가?
  //---------------------------------------------------------
  if (pItem->IsQuickItem()) {
    pOldItem = m_ItemSlot[n];

    return true;
  }

  pOldItem = NULL;

  return false;
}

//----------------------------------------------------------------------
// Find Slot To Add Item
//----------------------------------------------------------------------
// return값은 자리가 있느냐(true) / 없느냐(false)
// true인 경우에.. pItem이 들어갈 수 있는 자리가 slot이다.
//----------------------------------------------------------------------
bool MOustersArmsBand::FindSlotToAddItem(MItem *pItem, int &slot) const {
  if (!pItem->IsQuickItem()) {
    return false;
  }

  //---------------------------------------------------------
  // 어디에 들어갈 수 있을까?
  //---------------------------------------------------------
  for (int i = 0; i < m_Size; i++) {
    const MItem *pQuickItem = m_ItemSlot[i];

    //---------------------------------------------------------
    // 아무것도 없는 곳이면 그냥 넣으면 된다.
    //---------------------------------------------------------
    if (pQuickItem == NULL) {
      slot = i;
      return true;
    }
    //---------------------------------------------------------
    // 뭔가 있으면.. 그곳에 쌓일 수 있는지 알아본다.
    //---------------------------------------------------------
    if (pQuickItem->GetItemClass() == pItem->GetItemClass() &&
        pQuickItem->GetItemType() == pItem->GetItemType()) {
      //----------------------------------------------------
      // 더한 개수가 max를 넘지 않아야 한다.
      //----------------------------------------------------
      int addTotal = pQuickItem->GetNumber() + pItem->GetNumber();

      if (addTotal <= pQuickItem->GetMaxNumber()) {
        slot = i;
        return true;
      }
    }
  }

  // 들어갈 곳이 없다.
  return false;
}

TYPE_ITEM_NUMBER
MEffectItem::GetMaxNumber() const { return MAX_EFFECT_ITEM_NUMBER; }

TYPE_ITEM_NUMBER
MMoonCardItem::GetMaxNumber() const { return MAX_MOON_CARD_NUMBER; }

TYPE_ITEM_NUMBER
MLuckyBag::GetMaxNumber() const { return MAX_LUCKY_BAG_NUMBER; }
#if __CONTENTS(__GLOBAL_NPC)
TYPE_ITEM_NUMBER
MRankGem::GetMaxNumber() const { return MAX_LUCKY_BAG_NUMBER; }
#endif //__GLOBAL_NPC

///////////////////////////////////////////////////////////////////////////
//
//	Item Use
//
///////////////////////////////////////////////////////////////////////////
void MUsePotionItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  CGUsePotionFromInventory _CGUsePotionFromInventory;
  _CGUsePotionFromInventory.setObjectID(GetID());
  _CGUsePotionFromInventory.setX(GetGridX());
  _CGUsePotionFromInventory.setY(GetGridY());

  g_pSocket->sendPacket(&_CGUsePotionFromInventory);

  // 일단(!) 그냥 없애고 본다.
  //(*g_pInventory).RemoveItem( pItem->GetID() );

  //----------------------------------------------------
  // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
  //----------------------------------------------------
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
#endif
}

void MWater::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  bool bUseOK = false;
  bool bCreateHolyWater = GetItemType() >= 0 && GetItemType() <= 2;
  bool bCreateHolyPotion = GetItemType() >= 3 && GetItemType() <= 6;

  TYPE_ACTIONINFO useSkill = ACTIONINFO_NULL;

  if (bCreateHolyWater)
    useSkill = MAGIC_CREATE_HOLY_WATER;
  else if (bCreateHolyPotion)
    useSkill = SKILL_CREATE_HOLY_POTION;

  if (useSkill != ACTIONINFO_NULL &&
      g_pSkillAvailable->IsEnableSkill((ACTIONINFO)useSkill) &&
      (*g_pSkillInfoTable)[useSkill].IsAvailableTime()) {
    bUseOK = true;
  }

  //----------------------------------------------------
  // skill이 제대로 설정된 경우
  //----------------------------------------------------
  if (bUseOK) // playerSkill < (*g_pActionInfoTable).GetMinResultActionInfo())
  {
    // if ((*g_pActionInfoTable)[playerSkill].IsTargetItem())
    {
      //----------------------------------------------------
      // 검증 받을게 없는 경우..
      //----------------------------------------------------
      if (!g_pPlayer->IsWaitVerify() && g_pPlayer->IsItemCheckBufferNULL()) {
        POINT fitPoint; // 성수가 들어갈 자리

        if (bCreateHolyWater) {
          /*
          if (GetMakeItemFitPosition(this,
                                                                  ITEM_CLASS_HOLYWATER,
                                                                  GetItemType(),
                                                                  fitPoint))
          */
          {
            CGSkillToInventory _CGSkillToInventory;
            _CGSkillToInventory.setObjectID(GetID());
            _CGSkillToInventory.setX(GetGridX());
            _CGSkillToInventory.setY(GetGridY());
            //						_CGSkillToInventory.setTargetX(
            //fitPoint.x ); 						_CGSkillToInventory.setTargetY( fitPoint.y );
            _CGSkillToInventory.setTargetX(GetGridX());
            _CGSkillToInventory.setTargetY(GetGridY());
            _CGSkillToInventory.setSkillType(useSkill);

            g_pSocket->sendPacket(&_CGSkillToInventory);

            // 일단(!) 그냥 없애고 본다.
            //(*g_pInventory).RemoveItem( pItem->GetID() );

            //----------------------------------------------------
            // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
            //----------------------------------------------------
            g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
            g_pPlayer->SetItemCheckBuffer(
                this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);

            g_pSkillInfoTable->SetNextAvailableTime(useSkill);

            //----------------------------------------------------
            // 기술 사용 시도 동작
            //----------------------------------------------------
            AddNewInventoryEffect(
                GetID(),
                useSkill, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
                g_pClientConfig->FPS * 3 // 3초
            );
          }
          //								else
          //								{
          //									//
          //g_pGameMessage( "자리가 없따야~" );
          //								}
        } else if (bCreateHolyPotion) {
          if (GetMakeItemFitPosition(
                  this, ITEM_CLASS_POTION,
                  (*g_pItemTable)[GetItemClass()][GetItemType()].Value3,
                  fitPoint)) {
            CGSkillToInventory _CGSkillToInventory;

            _CGSkillToInventory.setObjectID(GetID());
            _CGSkillToInventory.setObjectID(GetID());
            _CGSkillToInventory.setX(GetGridX());
            _CGSkillToInventory.setY(GetGridY());
            _CGSkillToInventory.setTargetX(fitPoint.x);
            _CGSkillToInventory.setTargetY(fitPoint.y);
            _CGSkillToInventory.setSkillType(useSkill);

            g_pSocket->sendPacket(&_CGSkillToInventory);

            g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
            g_pPlayer->SetItemCheckBuffer(
                this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);

            g_pSkillInfoTable->SetNextAvailableTime(useSkill);

            AddNewInventoryEffect(GetID(), useSkill, g_pClientConfig->FPS * 3);
          } else {
          }
        }
      }
    }
  }
#endif
}

void MMagazine::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  bool bPossible = false;

  int value = (*g_pItemTable)[GetItemClass()][GetItemType()].Value3;

  if (value <= 0)
    bPossible = true;
  else {
    if (value > 0 && value < g_pActionInfoTable->GetSize())
      bPossible = g_pSkillAvailable->IsEnableSkill((ACTIONINFO)value);
  }

  if (bPossible == false) {
    g_pSystemMessage->Add(
        (*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_RELOAD_BY_VIVID_MAGAZINE]
            .GetString());
    return;
  }

  const MItem *pHandItem =
      (*g_pSlayerGear).GetItem(MSlayerGear::GEAR_SLAYER_RIGHTHAND);

  if (pHandItem != NULL && pHandItem->IsGunItem()) {
    const MGunItem *pGunItem = (const MGunItem *)pHandItem;

    // 현재 총에 장착할 수 있는 탄창인가?
    if (IsInsertToItem(pGunItem)) {
      CGReloadFromInventory _CGReloadFromInventory;
      _CGReloadFromInventory.setObjectID(GetID());
      _CGReloadFromInventory.setX(GetGridX());
      _CGReloadFromInventory.setY(GetGridY());

      g_pSocket->sendPacket(&_CGReloadFromInventory);

      // 일단(!) 그냥 없애고 본다.
      //(*g_pInventory).RemoveItem( GetID() );

      //----------------------------------------------------
      // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
    } else {
      // [도움말] 총알이 안 맞는 경우
      //			__BEGIN_HELP_EVENT
      //				ExecuteHelpEvent(
      //HE_ITEM_DIFFERENT_BULLET_TYPE );
      //			__END_HELP_EVENT

      DEBUG_ADD("Magazine is not fit to Gun");
    }
  } else {
    DEBUG_ADD("Weapon is not Gun");
  }
#endif
}

void MMagazine::UseQuickItem() {
#ifdef __GAME_CLIENT__
  const MItem *pHandItem =
      (*g_pSlayerGear).GetItem(MSlayerGear::GEAR_SLAYER_RIGHTHAND);

  if (pHandItem != NULL && pHandItem->IsGunItem()) {
    const MGunItem *pGunItem = (const MGunItem *)pHandItem;

    //----------------------------------------------------
    // 현재 총에 장착할 수 있는 탄창인가?
    //----------------------------------------------------
    if (IsInsertToItem(pGunItem)) {
      CGReloadFromQuickSlot _CGReloadFromQuickSlot;
      _CGReloadFromQuickSlot.setObjectID(GetID());
      _CGReloadFromQuickSlot.setSlotID(GetItemSlot());

      g_pSocket->sendPacket(&_CGReloadFromQuickSlot);

      //----------------------------------------------------
      // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);

      //----------------------------------------------------
      // 벨트 못 없애도록..
      //----------------------------------------------------
      UI_LockGear();

      //			__BEGIN_HELP_EVENT
      //				// [도움말] 탄창 사용
      //				ExecuteHelpEvent( HE_ITEM_USE_MAGAZINE
      //);
      //				// [도움말] 벨트의 아이템 사용
      //				ExecuteHelpEvent( HE_ITEM_USE_BELT_ITEM
      //);
      //			__END_HELP_EVENT
    } else {
      // [도움말] 총알이 안 맞는 경우
      //			__BEGIN_HELP_EVENT
      //				ExecuteHelpEvent(
      //HE_ITEM_DIFFERENT_BULLET_TYPE );
      //			__END_HELP_EVENT

      PlaySound(SOUND_ITEM_NO_MAGAZINE);

      // 탄창이 안 맞는 경우
      DEBUG_ADD("[Can't use Magazine] Wrong Magazine");
    }
  } else {
    // PlaySound( SOUND_ITEM_NO_MAGAZINE );

    // 총이 없는 경우
    DEBUG_ADD("[Can't use Magazine] No Gun");
  }
#endif
}

void MBombMaterial::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // int playerSkill = g_pPlayer->GetSpecialActionInfo();
  bool bUseOK = false;

  int currentItemType = GetItemType();

  ACTIONINFO useSkill;
  ITEM_CLASS itemClass;
  int itemType;

  //-------------------------------------------
  // 폭탄이냐 지뢰냐...
  //-------------------------------------------
  // 하드코딩 -_-;
  //-------------------------------------------
  if (currentItemType < 5) {
    useSkill = SKILL_MAKE_BOMB;
    itemClass = ITEM_CLASS_BOMB;
    itemType = currentItemType;
  } else {
    useSkill = SKILL_MAKE_MINE;
    itemClass = ITEM_CLASS_MINE;
    itemType = currentItemType - 5;
  }

  if (g_pSkillAvailable->IsEnableSkill(useSkill) &&
      (*g_pSkillInfoTable)[useSkill].IsAvailableTime()) {
    bUseOK = true;
  }

  //----------------------------------------------------
  // skill이 제대로 설정된 경우
  //----------------------------------------------------
  if (bUseOK) // playerSkill < (*g_pActionInfoTable).GetMinResultActionInfo())
  {
    //----------------------------------------------------
    // 검증 받을게 없는 경우..
    //----------------------------------------------------
    if (!g_pPlayer->IsWaitVerify() && g_pPlayer->IsItemCheckBufferNULL()) {
      POINT fitPoint; // 폭탄이 들어갈 자리

      if (GetMakeItemFitPosition(this, itemClass, itemType, fitPoint)) {
        CGSkillToInventory _CGSkillToInventory;
        _CGSkillToInventory.setObjectID(GetID());
        _CGSkillToInventory.setX(GetGridX());
        _CGSkillToInventory.setY(GetGridY());
        _CGSkillToInventory.setTargetX(fitPoint.x);
        _CGSkillToInventory.setTargetY(fitPoint.y);
        _CGSkillToInventory.setSkillType(useSkill);

        g_pSocket->sendPacket(&_CGSkillToInventory);

        // 일단(!) 그냥 없애고 본다.
        //(*g_pInventory).RemoveItem( GetID() );

        //----------------------------------------------------
        // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
        //----------------------------------------------------
        g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
        g_pPlayer->SetItemCheckBuffer(
            this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);

        // 만드는 시도 중에 이미 delay만큼 시간을 보낸다.
        //(*g_pSkillInfoTable)[useSkill].SetNextAvailableTime();

        //----------------------------------------------------
        // 기술 사용 시도 동작
        //----------------------------------------------------
        AddNewInventoryEffect(
            GetID(),
            useSkill, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
            g_pClientConfig->FPS * 3 // 3초
        );
      } else {
        // g_pGameMessage( "자리가 없따야~" );
      }
    }
  }
#endif
}

void MMine::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  bool bUseOK = true;
  ACTIONINFO useSkill;

  //----------------------------------------------------
  // 사용가능한가?
  //----------------------------------------------------
  useSkill = SKILL_INSTALL_MINE;

  if ( // g_pPlayer->GetSpecialActionInfo()==useSkill ||
      g_pSkillAvailable->IsEnableSkill(useSkill) &&
      (*g_pSkillInfoTable)[useSkill].IsAvailableTime() &&
      g_pPlayer->GetMoveDevice() != MCreature::MOVE_DEVICE_RIDE) {
    bUseOK = true;
  } else {
    bUseOK = false;
  }

  if (bUseOK) {
    CGSkillToInventory _CGSkillToInventory;
    _CGSkillToInventory.setObjectID(GetID());
    _CGSkillToInventory.setX(GetGridX());
    _CGSkillToInventory.setY(GetGridY());
    // 지뢰 심는 방향
    _CGSkillToInventory.setTargetX(g_pPlayer->GetDirection());
    _CGSkillToInventory.setSkillType(useSkill);

    g_pSocket->sendPacket(&_CGSkillToInventory);

    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

    g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);

    g_pSkillInfoTable->SetNextAvailableTime(useSkill);

    //----------------------------------------------------
    // 기술 사용 시도 동작
    //----------------------------------------------------
    /*
    if (useSkill == MAGIC_BLOODY_MARK)
    {
            AddNewInventoryEffect( GetID(),
                                                    useSkill,
                                                    14
                                            );
    }
    */
  }
#endif
}

void MSlayerPortalItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__

  if (g_bHolyLand == false // 성지가 아니어야 하고
      && g_pZoneTable->Get(g_pZone->GetID())->CannotUseSpecialItem ==
             false        // 현재 존이 스페셜 아이템을 사용할 수 있어야 하고
      && IsAffectStatus() // 적용 가능해야 하고
      && IsChargeItem() &&
      GetNumber() > 0 // 충전아이템이며 숫자가 0보다 커야 한다.
      && g_pZone->GetHelicopter(g_pPlayer->GetID()) ==
             NULL // 헬기를 불렀으면 안된다.
      && !g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)) {
    // slayer는 item사용하는거당
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());

    if (0 != SubInventoryItemID)
      _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

    g_pSocket->sendPacket(&_CGUseItemFromInventory);

    g_pPlayer->SetItemCheckBuffer(this,
                                  MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY,
                                  SubInventoryItemID);
  }
#endif
}

void MVampirePortalItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__

  int itemPrice = this->GetPrice();
  int money = g_pMoneyManager->GetMoney();
  if (money < itemPrice) {
    UI_PopupMessage(UI_STRING_MESSAGE_PORTAL_ITEM_NOUSE);
    return;
  }

  if (g_bHolyLand == false // 성지가 아니어야 하고
      && g_pZoneTable->Get(g_pZone->GetID())->CannotUseSpecialItem ==
             false        // 현재 존이 스페셜 아이템을 사용할 수 있어야 하고
      && IsAffectStatus() // 적용 가능해야 하고
      //&& IsChargeItem() && GetNumber() > 0	// 충전아이템이며 숫자가 0보다
      //커야 한다.
  ) {
    ACTIONINFO useSkill;

    if (IsMarked()) {
      useSkill = MAGIC_BLOODY_TUNNEL;
    } else {
      useSkill = MAGIC_BLOODY_MARK;
    }

    if (g_pSkillAvailable->IsEnableSkill(useSkill) &&
        (*g_pSkillInfoTable)[useSkill].IsAvailableTime()) {
      // vampire는 기술 사용하는거다
      CGSkillToInventory _CGSkillToInventory;
      _CGSkillToInventory.setObjectID(GetID());
      _CGSkillToInventory.setX(GetGridX());
      _CGSkillToInventory.setY(GetGridY());
      _CGSkillToInventory.setSkillType(useSkill);
      //_CGSkillToInventory.setCEffectID( 0 );	// -_-;;

      if (0 != SubInventoryItemID)
        _CGSkillToInventory.setInventoryItemObjectID(SubInventoryItemID);

      g_pSocket->sendPacket(&_CGSkillToInventory);

      g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY,
          SubInventoryItemID);

      g_pSkillInfoTable->SetNextAvailableTime(useSkill);

      //----------------------------------------------------
      // 기술 사용 시도 동작
      //----------------------------------------------------
      if (useSkill == MAGIC_BLOODY_MARK) {
        AddNewInventoryEffect(GetID(), useSkill, 14);
      }
    }
  }
#endif
}

void MEventStarItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 2개 이상 있어야 분리 된다.
  if (GetNumber() > 1) {
    // 아이템 분리
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);

    //----------------------------------------------------
    // Inventory에서 item을 몇개 드는 걸 검증받는다.
    // 일단은 1개만 들리도록 한다.
    //----------------------------------------------------
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
  }
#endif
}
void MMixingItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
  int itemType = GetItemType();
#ifdef __GAME_CLIENT__
  if (itemType >= 0 && itemType <= 8      // A ~ C형
      || itemType >= 19 && itemType <= 21 // D형
      || itemType >= 26 && itemType <= 27 // E형
      || itemType == 34                   // E core zap by rallser
      || itemType == 38                   // E core zap by rallser
      || itemType >= 35 && itemType <= 37) {
    if (GetNumber() > 1) // 아이템 분리
    {
      CGAddInventoryToMouse _CGAddInventoryToMouse;
      _CGAddInventoryToMouse.setObjectID(0);
      _CGAddInventoryToMouse.setX(GetGridX());
      _CGAddInventoryToMouse.setY(GetGridY());
      _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
      g_pSocket->sendPacket(&_CGAddInventoryToMouse);
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
    } else {
      C_VS_UI_MIXING_FORGE::FORGE_CLASS fc;
      C_VS_UI_MIXING_FORGE::FORGE_TYPE ft;

      if (itemType >= 0 && itemType <= 2) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_WEAPON;
        ft = C_VS_UI_MIXING_FORGE::FORGE_TYPE(C_VS_UI_MIXING_FORGE::TYPE_A +
                                              itemType);
      } else if (itemType >= 3 && itemType <= 5) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ARMOR;
        ft = C_VS_UI_MIXING_FORGE::FORGE_TYPE(C_VS_UI_MIXING_FORGE::TYPE_A +
                                              (itemType - 3));
      } else if (itemType >= 6 && itemType <= 8) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ACCESSORY;
        ft = C_VS_UI_MIXING_FORGE::FORGE_TYPE(C_VS_UI_MIXING_FORGE::TYPE_A +
                                              (itemType - 6));
      } else if (itemType == 19) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_WEAPON;
        ft = C_VS_UI_MIXING_FORGE::TYPE_D;
      } else if (itemType == 20) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ARMOR;
        ft = C_VS_UI_MIXING_FORGE::TYPE_D;
      } else if (itemType == 21) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ACCESSORY;
        ft = C_VS_UI_MIXING_FORGE::TYPE_D;
      } else if (itemType == 26) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_WEAPON;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      } else if (itemType == 27) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ARMOR;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      } else if (itemType == 34) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_COREZAP;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      } else if (itemType == 38) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_CUEOFADAM;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      } else if (itemType == 36) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ARMOR;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      } else if (itemType == 37) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_ACCESSORY;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      } else if (itemType == 35) {
        fc = C_VS_UI_MIXING_FORGE::CLASS_WEAPON;
        ft = C_VS_UI_MIXING_FORGE::TYPE_E;
      }
      gC_vs_ui.RunMixingForge(fc, ft);
      g_pPlayer->SetItemCheckBuffer(this,
                                    MPlayer::ITEM_CHECK_BUFFER_USE_MIXING_ITEM);
    }
  } else if (itemType >= 9 && itemType <= 18 ||
             itemType >= 22 && itemType <= 25 || itemType == 28
#if __CONTENTS(__PET_MIXINGFORGE)
             || itemType == 30
#endif //__PET_MIXINGFORGE
  ) {
    // Puritas 는 우클릭하면 분리한다.
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0);
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
  }
#if __CONTENTS(__PET_MIXINGFORGE)
  else if (itemType == 29) {
    if (GetNumber() > 1) // 아이템 분리
    {
      CGAddInventoryToMouse _CGAddInventoryToMouse;
      _CGAddInventoryToMouse.setObjectID(0);
      _CGAddInventoryToMouse.setX(GetGridX());
      _CGAddInventoryToMouse.setY(GetGridY());
      _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
      g_pSocket->sendPacket(&_CGAddInventoryToMouse);
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
    } else {
      gC_vs_ui.RunPetMixingForge();
      g_pPlayer->SetItemCheckBuffer(this,
                                    MPlayer::ITEM_CHECK_BUFFER_USE_MIXING_ITEM);
    }
  }
#endif //__PET_MIXINGFORGE

#endif
}

void MEffectItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 확성기같은것..
  int itemType = GetItemType();
  bool useItem = false;

#if __CONTENTS(!__MODIFY_FORCE_SCROLL)
  // sjheon 2005.05.16 Add
  if (itemType >= 7 && itemType <= 9 || itemType >= 32 && itemType <= 33) {
    bool bEsBethmothForce =
        UI_GetEffectStatus(EFFECTSTATUS_BEHEMOTH_FORCE_SCROLL);
    bool bEsSafeForce = UI_GetEffectStatus(EFFECTSTATUS_SAFE_FORCE_SCROLL);
    bool bEsCarnelianForce =
        UI_GetEffectStatus(EFFECTSTATUS_CARNELIAN_FORCE_SCROLL);
    bool bEsOdinForce = UI_GetEffectStatus(EFFECTSTATUS_ODIN_FORCE_SCROLL);
    bool bEsTituosForce = UI_GetEffectStatus(EFFECTSTATUS_TITUOS_FORCE_SCROLL);

    if (bEsBethmothForce || bEsSafeForce || bEsCarnelianForce || bEsOdinForce ||
        bEsTituosForce) {
      UI_PopupMessage(UI_STRING_MESSAGE_SCROLL_EFFECT_ITEMDEL);
      return;
    }

    if (!UI_IsRunAskUseItemDialog()) {
      UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION);
      return;
    } else {
      UI_SetUseAskIitemRunning(false);
      useItem = true;
    }

  }
#else // 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
  if (itemType >= 7 && itemType <= 9) {
    bool bEsBethmothForce =
        UI_GetEffectStatus(EFFECTSTATUS_BEHEMOTH_FORCE_SCROLL);
    bool bEsSafeForce = UI_GetEffectStatus(EFFECTSTATUS_SAFE_FORCE_SCROLL);
    bool bEsCarnelianForce =
        UI_GetEffectStatus(EFFECTSTATUS_CARNELIAN_FORCE_SCROLL);
    bool bEsOdinForce = UI_GetEffectStatus(EFFECTSTATUS_ODIN_FORCE_SCROLL);
    bool bEsTituosForce = UI_GetEffectStatus(EFFECTSTATUS_TITUOS_FORCE_SCROLL);

    if (bEsBethmothForce || bEsSafeForce || bEsCarnelianForce || bEsOdinForce ||
        bEsTituosForce) {
      UI_PopupMessage(UI_STRING_MESSAGE_SCROLL_EFFECT_ITEMDEL);
      return;
    }

    if (!UI_IsRunAskUseItemDialog()) {
      UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION);
      return;
    } else {
      UI_SetUseAskIitemRunning(false);
      useItem = true;
    }

  } else if (itemType >= 32 && itemType <= 33) {
    bool bEsBethmothForce =
        UI_GetEffectStatus(EFFECTSTATUS_BEHEMOTH_FORCE_SCROLL);
    bool bEsSafeForce = UI_GetEffectStatus(EFFECTSTATUS_SAFE_FORCE_SCROLL);
    bool bEsCarnelianForce =
        UI_GetEffectStatus(EFFECTSTATUS_CARNELIAN_FORCE_SCROLL);
    bool bEsOdinForce = UI_GetEffectStatus(EFFECTSTATUS_ODIN_FORCE_SCROLL);
    bool bEsTituosForce = UI_GetEffectStatus(EFFECTSTATUS_TITUOS_FORCE_SCROLL);

    if (bEsBethmothForce || bEsSafeForce || bEsCarnelianForce) {
      UI_PopupMessage(UI_STRING_MESSAGE_SCROLL_EFFECT_ITEMDEL);
      return;
    }

    if ((itemType == 32 && bEsOdinForce) ||
        (itemType == 33 && bEsTituosForce)) {
      UI_PopupMessage(UI_STRING_MESSAGE_SCROLL_EFFECT_ITEMDEL);
      return;
    }

    if (!UI_IsRunAskUseItemDialog()) {
      UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION);
      return;
    } else {
      UI_SetUseAskIitemRunning(false);
      useItem = true;
    }
  }
#endif //__MODIFY_FORCE_SCROLL
  else if (itemType >= 24 && itemType <= 26 || itemType >= 34 && itemType <= 36
#if __CONTENTS(__EXTREME_FORCE_SCROLL)
           || itemType == 54
#endif //__EXTREME_FORCE_SCROLL
  ) {

#if __CONTENTS(!__EXTREME_FORCE_SCROLL)
    bool bIsUseForce[3] = {
        UI_GetEffectStatus(EFFECTSTATUS_GREYFELL_FORCE_SCROLL),
        UI_GetEffectStatus(EFFECTSTATUS_SOVIUS_FORCE_SCROLL),
        UI_GetEffectStatus(EFFECTSTATUS_MIR_FORCE_SCROLL),
    };
#else
    bool bIsUseForce[4] = {
        UI_GetEffectStatus(EFFECTSTATUS_GREYFELL_FORCE_SCROLL),
        UI_GetEffectStatus(EFFECTSTATUS_SOVIUS_FORCE_SCROLL),
        UI_GetEffectStatus(EFFECTSTATUS_MIR_FORCE_SCROLL),
        UI_GetEffectStatus(EFFECTSTATUS_EXTREME_FORCE_SCROLL),
    };
#endif //__EXTREME_FORCE_SCROLL

    int typeSelect = 0;

    if ((itemType - 26) < 3) {
      typeSelect = 24;
    } else if ((itemType - 36) < 3) {
      typeSelect = 34;
    }
#if __CONTENTS(__EXTREME_FORCE_SCROLL)
    else if ((itemType - 51) == 3) {
      typeSelect = 51;
    }

    // 익스트림 포스스크롤 효과중이면 다른 효과X || 익스트림 포스스크롤 을
    // 사용하려할때, 다른효과 켜져있으면 X
    if (bIsUseForce[3] == true ||
        (itemType - typeSelect == 3 &&
         (bIsUseForce[0] == true || bIsUseForce[1] == true ||
          bIsUseForce[2] == true))) {
      bIsUseForce[itemType - typeSelect] = true;
    }
#endif //__EXTREME_FORCE_SCROLL

    if (bIsUseForce[itemType - typeSelect] == true) {
      UI_PopupMessage(UI_STRING_MESSAGE_SCROLL_EFFECT_ITEMDEL);
      return;
    }

    if (!UI_IsRunAskUseItemDialog()) {
      // UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION) ;
      UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_EFFECT_ITEM);
      return;
    } else {
      UI_SetUseAskIitemRunning(false);
      useItem = true;
    }

  }
  // sjheon 2005.05.16 End
#if __CONTENTS(__PREMIUM_CARD)
  else if (itemType == 39 || itemType == 40
#if __CONTENTS(__PREMIUM_CARD_D)
           || itemType == 41 || itemType == 55
#endif //__PREMIUM_CARD_D
#if __CONTENTS(__PREMIUM_CARD_ADAY)
           || itemType >= 56 && itemType <= 58
#endif
  ) {
    if (GetNumber() > 1) {
      // 아이템 분리
      CGAddInventoryToMouse _CGAddInventoryToMouse;
      _CGAddInventoryToMouse.setObjectID(0);
      _CGAddInventoryToMouse.setX(GetGridX());
      _CGAddInventoryToMouse.setY(GetGridY());
      _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
      g_pSocket->sendPacket(&_CGAddInventoryToMouse);

      //----------------------------------------------------
      // Inventory에서 item을 몇개 드는 걸 검증받는다.
      // 일단은 1개만 들리도록 한다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);

      return;
    } else {
      if (!UI_IsRunAskUseItemDialog()) {
        // UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION) ;
        UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_EFFECT_ITEM);
        return;
      } else {
        UI_SetUseAskIitemRunning(false);
        useItem = true;
      }
    }
  }
#endif //__PREMIUM_CARD
       // 20080812 일본 분노의 기원 사용 금지 임시 조치
  /*	else if(itemType == 19)
          {
                  UI_PopupMessage(UI_STRING_CANNOT_USE);
                  return ;
          }*/

  // 2009.08.25..neclipse..모래 시계, 경험의 돌, 바이퍼 블루가 분리 될수 있도록
  // 예외처리다... 이런 코드가 자꾸 늘어난다...
  if ((itemType >= 4 && itemType <= 6) || (itemType >= 10 && itemType <= 15) ||
      (itemType >= 28 && itemType <= 30) || itemType == 38 ||
      (itemType >= 48 && itemType <= 53) ||
      (itemType >= 59 && itemType <= 64) ||
      (itemType >= 42 && itemType <= 47) // 46은 바이퍼 블루...47은 경험의 돌..
      || (itemType >= 85 && itemType <= 88)) {
    if (GetNumber() > 1) {
      // 아이템 분리
      CGAddInventoryToMouse _CGAddInventoryToMouse;
      _CGAddInventoryToMouse.setObjectID(0);
      _CGAddInventoryToMouse.setX(GetGridX());
      _CGAddInventoryToMouse.setY(GetGridY());
      _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
      g_pSocket->sendPacket(&_CGAddInventoryToMouse);

      //----------------------------------------------------
      // Inventory에서 item을 몇개 드는 걸 검증받는다.
      // 일단은 1개만 들리도록 한다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
      return;
    }
  }

  // by svi
  if (useItem == false) {
    if (!UI_IsRunAskUseItemDialog()) {
      // UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION) ;
      UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_EFFECT_ITEM);
      return;
    } else {
      UI_SetUseAskIitemRunning(false);
    }
  }
  if (itemType == 87) // laba
  {
    gC_vs_ui.RunBulletinBoardWindow(this);
    return;
  }

  CGUseItemFromInventory _CGUseItemFromInventory;

  _CGUseItemFromInventory.setObjectID(GetID());
  _CGUseItemFromInventory.setX(GetGridX());
  _CGUseItemFromInventory.setY(GetGridY());
  g_pSocket->sendPacket(&_CGUseItemFromInventory);
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
#endif
}

void MItemETC::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  if (GetItemType() == 1) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());
    g_pSocket->sendPacket(&_CGUseItemFromInventory);
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
  }
#endif
}

void MVampireETC::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  //----------------------------------------------------
  //
  //					변신용 아이템 - vampire인 경우
  //
  //----------------------------------------------------
  if (g_pZoneTable->Get(g_pZone->GetID())->CannotUseSpecialItem == false) {
    ACTIONINFO useSkill; // = g_pPlayer->GetSpecialActionInfo();

    MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetID());

    if (pCreature) // Sjheon 2005.09.15 Add
    {
      if (pCreature->GetAction() == ACTION_VAMPIRE_DRAIN) {
        if (GetItemClass() == ITEM_CLASS_SKULL || GetItemType() == 0 ||
            GetItemType() == 1) // 늑대 머리
        {
          return;
        }
      }
    }
    // Sjheon 2005.09.15 End

    if (GetItemClass() == ITEM_CLASS_SKULL) // 늑대 머리
    {
      useSkill = SKILL_TRANSFORM_TO_WERWOLF;
    } else if (GetItemType() == 0) {
      useSkill = MAGIC_TRANSFORM_TO_WOLF;
    } else if (GetItemType() == 1) {
      useSkill = MAGIC_TRANSFORM_TO_BAT;
    } else {
      // - -;
      if (GetItemType() == 2) {

        CGUseItemFromInventory _CGUseItemFromInventory;
        _CGUseItemFromInventory.setObjectID(GetID());
        _CGUseItemFromInventory.setX(GetGridX());
        _CGUseItemFromInventory.setY(GetGridY());

        g_pSocket->sendPacket(&_CGUseItemFromInventory);

        g_pPlayer->SetItemCheckBuffer(
            this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
      }
      return;
    }

    //----------------------------------------------------
    // skill이 제대로 설정된 경우
    //----------------------------------------------------
    if (useSkill != ACTIONINFO_NULL) {
      if (g_pPlayer->IsVampire() &&
          g_pPlayer->GetAction() == ACTION_VAMPIRE_DRAIN)
        return;

      if ( //(*g_pActionInfoTable)[playerSkill].IsTargetItem()
          g_pSkillAvailable->IsEnableSkill(useSkill) &&
          (*g_pSkillInfoTable)[useSkill].IsAvailableTime()) {
        //----------------------------------------------------
        // 검증 받을게 없는 경우..
        //----------------------------------------------------
        if (!g_pPlayer->IsWaitVerify() && g_pPlayer->IsItemCheckBufferNULL()) {
          CGSkillToInventory _CGSkillToInventory;
          _CGSkillToInventory.setObjectID(GetID());
          _CGSkillToInventory.setX(GetGridX());
          _CGSkillToInventory.setY(GetGridY());
          _CGSkillToInventory.setSkillType(useSkill);
          //_CGSkillToInventory.setCEffectID( 0 );	// -_-;;

          if (0 != SubInventoryItemID)
            _CGSkillToInventory.setInventoryItemObjectID(SubInventoryItemID);
          g_pSocket->sendPacket(&_CGSkillToInventory);

          // 일단(!) 그냥 없애고 본다.
          //(*g_pInventory).RemoveItem( GetID() );

          //----------------------------------------------------
          // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
          //----------------------------------------------------
          g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
          g_pPlayer->SetItemCheckBuffer(
              this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY,
              SubInventoryItemID);

          g_pSkillInfoTable->SetNextAvailableTime(useSkill);

          //----------------------------------------------------
          // 기술 사용 시도 동작
          //----------------------------------------------------
          AddNewInventoryEffect(
              GetID(),
              useSkill, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
              g_pClientConfig->FPS * 3 // 3초
          );
        }
      }
    }
  }
#endif
}

void MSkull::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  //----------------------------------------------------
  //
  //					변신용 아이템 - vampire인 경우
  //
  //----------------------------------------------------
  if (g_pZoneTable->Get(g_pZone->GetID())->CannotUseSpecialItem == false) {
    ACTIONINFO useSkill; // = g_pPlayer->GetSpecialActionInfo();

    MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetID());
    // Sjheon 2005.09.15 Add
    if (pCreature) // Sjheon 2005.09.15 Add
    {
      if (pCreature->GetAction() == ACTION_VAMPIRE_DRAIN) {
        if ((GetItemClass() == ITEM_CLASS_SKULL && GetItemType() == 39) ||
            GetItemType() == 0 || GetItemType() == 1) // 늑대 머리
        {
          return;
        }
      }
    }
    // Sjheon 2005.09.15 End

    if (GetItemClass() == ITEM_CLASS_SKULL && GetItemType() == 39) // 늑대 머리
    {
      useSkill = SKILL_TRANSFORM_TO_WERWOLF;
    } else if (GetItemType() == 0) {
      useSkill = MAGIC_TRANSFORM_TO_WOLF;
    } else if (GetItemType() == 1) {
      useSkill = MAGIC_TRANSFORM_TO_BAT;
    } else {
      // - -;
      if (GetItemType() == 2) {

        CGUseItemFromInventory _CGUseItemFromInventory;
        _CGUseItemFromInventory.setObjectID(GetID());
        _CGUseItemFromInventory.setX(GetGridX());
        _CGUseItemFromInventory.setY(GetGridY());

        g_pSocket->sendPacket(&_CGUseItemFromInventory);

        g_pPlayer->SetItemCheckBuffer(
            this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
      }
      return;
    }

    //----------------------------------------------------
    // skill이 제대로 설정된 경우
    //----------------------------------------------------
    if (useSkill != ACTIONINFO_NULL) {
      if (g_pPlayer->IsVampire() &&
          g_pPlayer->GetAction() == ACTION_VAMPIRE_DRAIN)
        return;

      if ( //(*g_pActionInfoTable)[playerSkill].IsTargetItem()
          g_pSkillAvailable->IsEnableSkill(useSkill) &&
          (*g_pSkillInfoTable)[useSkill].IsAvailableTime()) {
        //----------------------------------------------------
        // 검증 받을게 없는 경우..
        //----------------------------------------------------
        if (!g_pPlayer->IsWaitVerify() && g_pPlayer->IsItemCheckBufferNULL()) {
          CGSkillToInventory _CGSkillToInventory;
          _CGSkillToInventory.setObjectID(GetID());
          _CGSkillToInventory.setX(GetGridX());
          _CGSkillToInventory.setY(GetGridY());
          _CGSkillToInventory.setSkillType(useSkill);
          //_CGSkillToInventory.setCEffectID( 0 );	// -_-;;

          g_pSocket->sendPacket(&_CGSkillToInventory);

          // 일단(!) 그냥 없애고 본다.
          //(*g_pInventory).RemoveItem( GetID() );

          //----------------------------------------------------
          // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
          //----------------------------------------------------
          g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
          g_pPlayer->SetItemCheckBuffer(
              this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);

          g_pSkillInfoTable->SetNextAvailableTime(useSkill);

          //----------------------------------------------------
          // 기술 사용 시도 동작
          //----------------------------------------------------
          AddNewInventoryEffect(
              GetID(),
              useSkill, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
              g_pClientConfig->FPS * 3 // 3초
          );
        }
      }
    }
  }
#endif
}

// 해당 크리쳐의 주변에 원하는 시체가 있는지 검사한다.
extern bool IsExistCorpseFromPlayer(MCreature *OriginCreature,
                                    int creature_type);

void MKey::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 2004, 12, 16, sobeit add start - 인스톨 터렛 상태에서 오토바이를 타면 문제
  // 생김..ㅎㅎ 그래서 막음
  if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)) {
    return;
  }
  // 2004, 12, 16, sobeit add end
  // 2009/11/10  bike crash獵契櫓꼇콘賈痰bike ride
  if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_BIKE_CRASH)) {
    return;
  }

  // 주변에 깃발이 없어야 한다.
  if (!IsExistCorpseFromPlayer(dynamic_cast<MCreature *>(g_pPlayer), 670)) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());
    if (0 != SubInventoryItemID)
      _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

    g_pSocket->sendPacket(&_CGUseItemFromInventory);

    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
  } else {
    g_pSystemMessage->Add(
        (*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_ACTION_MOTORCYCLE_FLAG]
            .GetString());
  }

#endif
}
void MKey::UseQuickItem() {
#ifdef __GAME_CLIENT__
  // 2004, 12, 16, sobeit add start - 인스톨 터렛 상태에서 오토바이를 타면 문제
  // 생김..ㅎㅎ 그래서 막음
  if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)) {
    return;
  }
  if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_MK1)) {
    return;
  } // mk1榴檄苟꼇콘梁뻥칡辜
  // 2004, 12, 16, sobeit add end
  // 2009/11/10  bike crash獵契櫓꼇콘賈痰bike ride
  if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_BIKE_CRASH)) {
    return;
  }

  CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
  _CGUsePotionFromQuickSlot.setObjectID(GetID());
  _CGUsePotionFromQuickSlot.setSlotID(GetItemSlot());

  g_pSocket->sendPacket(&_CGUsePotionFromQuickSlot);

  UI_LockGear();
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
  // 일단(!) 그냥 없애고 본다.
  //(*g_pInventory).RemoveItem( GetID() );

  //----------------------------------------------------
  // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
  //----------------------------------------------------
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
  //----------------------------------------------------
  // 벨트 못 없애도록..
  //----------------------------------------------------
  UI_LockGear();

#endif
}

#if __CONTENTS(__EVENTTREE_PILE)
//----------------------------------------------------------------------
// MEventTreeItem - Get MaxNumber
//----------------------------------------------------------------------
TYPE_ITEM_NUMBER
MEventTreeItem::GetMaxNumber() const { return MAX_EVENTTREE_NUMBER; }
#endif //__EVENTTREE_PILE

void MEventTreeItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__

  bool bXmasCardWindow = GetItemType() == 12 || GetItemType() == 54;

#if __CONTENTS(__EVENTTREE_PILE)
  if (GetNumber() > 1) {
    // 아이템 분리
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);

    //----------------------------------------------------
    // Inventory에서 item을 몇개 드는 걸 검증받는다.
    // 일단은 1개만 들리도록 한다.
    //----------------------------------------------------
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
  } else
#endif //__EVENTTREE_PILE
    if (bXmasCardWindow || GetItemType() >= 26 && GetItemType() <= 28) {
      bool bUseOK = true;

      bool bMasterWords =
          g_pPlayer->GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR ||
          g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR ||
          g_pPlayer->GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR;

      if (IsPlayerInSafePosition() ||
          g_pZone != NULL &&
              (g_pZone->GetID() == 1104 || g_pZone->GetID() == 1106 ||
               g_pZone->GetID() == 1114 || g_pZone->GetID() == 1115))
      // 안전지대와 마스터 레어에서는 못쓴다
      {
        // 각 성이 아니면서 운영자가 아니면
        if (!(g_pZone->GetID() >= 1201 && g_pZone->GetID() <= 1206) &&
            !bMasterWords) {
          bUseOK = false;
          g_pUIDialog->PopupFreeMessageDlg(
              (*g_pGameStringTable)[STRING_MESSAGE_NOT_USE_SAFETY_POSITION]
                  .GetString());
          return;
        }
      }

      if (g_pPlayer->GetMoveDevice() == MCreature::MOVE_DEVICE_RIDE) {
        if (g_pPlayer->IsSlayer())
          g_pUIDialog->PopupFreeMessageDlg(
              (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_USE_RIDE_MOTORCYCLE]
                  .GetString());
        else if (g_pPlayer->IsOusters())
          g_pUIDialog->PopupFreeMessageDlg(
              (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_USE_SUMMON_SYLPH]
                  .GetString());

        return;
      }

      int playerX = g_pPlayer->GetX(), playerY = g_pPlayer->GetY();

      for (int y = playerY - 3; y <= playerY + 3; y++) {
        for (int x = playerX - 2; x <= playerX + 2; x++) {
          if (x < 0 || y < 0 || x >= g_pZone->GetWidth() ||
              y >= g_pZone->GetHeight())
            continue;

          MItem *pItem = g_pZone->GetSector(x, y).GetItem();
          MCreature *pCreature = NULL;

          if (pItem != NULL) {
            if (GetItemClass() == ITEM_CLASS_CORPSE) {
              MCorpse *pCorpse = (MCorpse *)pItem;
              pCreature = pCorpse->GetCreature();

              if (pCreature != NULL && pCreature->GetCreatureType() == 482 ||
                  pCreature->GetCreatureType() == 650 ||
                  pCreature->GetCreatureType() == 819) {
                bUseOK = false;
                if (bXmasCardWindow) // 크리스마스 트리 || 터프한 눈사람
                {
                  g_pUIDialog->PopupFreeMessageDlg(
                      (*g_pGameStringTable)[STRING_MESSAGE_XMAS_TREE_CANNOT_USE]
                          .GetString());
                } else // 게시판
                {
                  g_pUIDialog->PopupFreeMessageDlg(
                      (*g_pGameStringTable)
                          [STRING_MESSAGE_BULLETIN_BOARD_CANNOT_USE]
                              .GetString());
                }
                return;
              }
            }
          }
        }
      }
      if (bUseOK) {
        if (bXmasCardWindow) // 크리스마스 트리 || 터프한 눈사람
        {
          gC_vs_ui.RunXmasCardWindow(this);
        } else // 게시판
        {
          gC_vs_ui.RunBulletinBoardWindow(this);
        }
      }
    }
#endif //__GAME_CLIENT__
}

void MEventEtcItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  bool bUseOK = true;
  /* // by svi *
          if(GetItemType() == 18 ) return ;
  */
  // 2개 이상 있어야 분리 된다.
  if (GetItemType() == 18) {
    if (GetNumber() > 1) {
      // 아이템 분리
      CGAddInventoryToMouse _CGAddInventoryToMouse;
      _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
      _CGAddInventoryToMouse.setX(GetGridX());
      _CGAddInventoryToMouse.setY(GetGridY());
      _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
      g_pSocket->sendPacket(&_CGAddInventoryToMouse);

      //----------------------------------------------------
      // Inventory에서 item을 몇개 드는 걸 검증받는다.
      // 일단은 1개만 들리도록 한다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
    }
    return;
  }
  // end

  if (IsPlayerInSafePosition() == 2 && GetItemType() != 14 &&
      GetItemType() != 15 && GetItemType() != 16 &&
      GetItemType() != 17) // 14는 머시냐..사탕..과 송편들..
  {
    bUseOK = false;
    g_pUIDialog->PopupFreeMessageDlg(
        (*g_pGameStringTable)[STRING_MESSAGE_NOT_USE_SAFETY_ZONE].GetString());
    return;
  }

  if (bUseOK) {
    if (GetItemType() == 14 || GetItemType() == 15 || GetItemType() == 16 ||
        GetItemType() == 17) // 사탕만, + 송편
    {
      CGUsePotionFromInventory _CGUsePotionFromInventory;
      _CGUsePotionFromInventory.setObjectID(GetID());
      _CGUsePotionFromInventory.setX(GetGridX());
      _CGUsePotionFromInventory.setY(GetGridY());
      g_pSocket->sendPacket(&_CGUsePotionFromInventory);
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

    } else {
      CGUseItemFromInventory _CGUseItemFromInventory;
      _CGUseItemFromInventory.setObjectID(GetID());
      _CGUseItemFromInventory.setX(GetGridX());
      _CGUseItemFromInventory.setY(GetGridY());
      g_pSocket->sendPacket(&_CGUseItemFromInventory);
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
    }
  }
#endif
}
void MEventEtcItem::UseQuickItem() {
#ifdef __GAME_CLIENT__

  if (GetItemType() == 18)
    return;

  if (GetItemType() == 14 || GetItemType() == 15 || GetItemType() == 16 ||
      GetItemType() == 17) // 사탕만, + 송편
  {
    if (g_pPlayer->IsOusters()) {
      CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
      _CGUsePotionFromQuickSlot.setObjectID(GetID());
      int slotID = GetItemSlot();

      if (g_pArmsBand1 != NULL) // 1번 암스밴드가 있으면 그냥
      {
        if (g_pArmsBand1->GetItem(slotID) == this)
          _CGUsePotionFromQuickSlot.setSlotID(slotID);
        else
          _CGUsePotionFromQuickSlot.setSlotID(slotID + 3);
      } else // 1번 암스밴드가 없으면 2번 +3 을 해준다
      {
        _CGUsePotionFromQuickSlot.setSlotID(slotID + 3);
      }

      //					_CGUsePotionFromQuickSlot.setSlotID(
      //slotID );
      g_pSocket->sendPacket(&_CGUsePotionFromQuickSlot);
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
      //	DEBUG_ADD_FORMAT("[ Ousters Item Use From QuickSlot ] %d
      //%d",slotID, GetID() );

      //----------------------------------------------------
      // 벨트 못 없애도록..
      //----------------------------------------------------
      UI_LockGear();
    } else {
      CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
      _CGUsePotionFromQuickSlot.setObjectID(GetID());
      _CGUsePotionFromQuickSlot.setSlotID(GetItemSlot());

      g_pSocket->sendPacket(&_CGUsePotionFromQuickSlot);

      // 일단(!) 그냥 없애고 본다.
      //(*g_pInventory).RemoveItem( GetID() );

      //----------------------------------------------------
      // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);

      //----------------------------------------------------
      // 벨트 못 없애도록..
      //----------------------------------------------------
      UI_LockGear();
    }
  }
#endif
}

void MDyePotionItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 아우스터즈는 성이 없기 때문에 성전환 아이템을 사용할 수 없다

  const int domain_level_Sword =
      (*g_pSkillManager)[SKILLDOMAIN_SWORD].GetDomainLevel();
  const int domain_level_Blade =
      (*g_pSkillManager)[SKILLDOMAIN_BLADE].GetDomainLevel();
  const int domain_level_Gun =
      (*g_pSkillManager)[SKILLDOMAIN_GUN].GetDomainLevel();
  const int domain_level_Heal =
      (*g_pSkillManager)[SKILLDOMAIN_HEAL].GetDomainLevel();
  const int domain_level_Enchant =
      (*g_pSkillManager)[SKILLDOMAIN_ENCHANT].GetDomainLevel();

  if (g_pPlayer->IsOusters() && GetItemType() == 48) {
    UI_PopupMessage(STRING_MESSAGE_CANNOT_USE_OUSTERS);
  } else {
    // 2005, 2, 22, sobeit add start
    // 		if(g_pPlayer->IsAdvancementClass() && GetItemType() >= 49 &&
    // GetItemType() <= 57) 		{ // 승직 유저는 마스터 이펙트 색상 바꾸는 아이템을
    // 못쓰게 한다네..왤까...?
    // 			UI_PopupMessage(UI_STRING_MESSAGE_CANNOT_USE_ADVANCEMENTCLASS);
    // 			return;
    // 		}

    // by svi
    /*
    if(g_pPlayer->IsAdvancementClass() && GetItemType() >= 58 && GetItemType()
    <= 61) { // 승직 유저는 마스터 이펙트 색상 바꾸는 아이템을 못쓰게
    한다네..왤까...? UI_PopupMessage(UI_STRING_MESSAGE_USE_NOTADVANCEMENTCLASS);
            return;
    } */

    if (!g_pPlayer->IsAdvancementClass() && GetItemType() >= 76 &&
        GetItemType() <= 79) { // 승직 유저는 마스터 이펙트 색상 바꾸는 아이템을
                               // 못쓰게 한다네..왤까...?
      UI_PopupMessage(UI_STRING_MESSAGE_USE_ADVANCEMENTCLASS);
      return;
    }

    if ((g_pPlayer->GetLEVEL() < 99 && domain_level_Sword < 99 &&
         domain_level_Blade < 99 && domain_level_Gun < 99 &&
         domain_level_Heal < 99 && domain_level_Enchant < 99) &&
        GetItemType() >= 58 &&
        GetItemType() <= 61) { // 승직 유저는 마스터 이펙트 색상 바꾸는 아이템을
                               // 못쓰게 한다네..왤까...?
      UI_PopupMessage(UI_STRING_MESSAGE_USE_NOTADVANCEMENTCLASS);
      return;
    }

    else if (!g_pPlayer->IsAdvancementClass() && GetItemType() >= 62 &&
             GetItemType() <= 63) {
      UI_PopupMessage(UI_STRING_MESSAGE_USE_ADVANCEMENTCLASS);
      return;
    }
    // else if((g_pPlayer->IsAdvancementClass() && g_pPlayer->GetLEVEL() < 99)
    // && GetItemType() >= 58 && GetItemType() <= 61)
    //{
    //	UI_PopupMessage(UI_STRING_MESSAGE_USE_NOTADVANCEMENTCLASS);
    //	return;
    // }

    else if (GetItemType() >= 58 && GetItemType() <= 61) {
      if (!UI_IsRunAskUseItemDialog()) {
        UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION);
        return;
      } else {
        UI_SetUseAskIitemRunning(false);
      }
    }

    else if (g_pPlayer->IsAdvancementClass() && GetItemType() >= 62 &&
             GetItemType() <= 63 && GetItemType() == 83) {
      if (!UI_IsRunAskUseItemDialog()) {
        UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION);
        return;
      } else {
        UI_SetUseAskIitemRunning(false);
      }
    }

    else if (GetItemType() == 64) {
      BOOL bType = FALSE;
      for (int type = 0; type < g_pRankBonusTable->GetSize(); type++) {
        if ((*g_pRankBonusTable)[type].GetStatus() ==
            RankBonusInfo::STATUS_LEARNED)
          bType = TRUE;
      }

      if (bType == FALSE)
        return;

      if (!UI_IsRunAskUseItemDialog()) {
        UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION, 1);
        return;
      } else {
        UI_SetUseAskIitemRunning(false);
      }
    }
    // 2006.05.24 sjheon Add
    else if (GetItemType() >= 68 && GetItemType() <= 75) {
      if (g_pPlayer->GetPersnalShopColor() ==
          MCreature::s_PersnalShopColor[GetItemType() - 68]) {
        UI_PopupMessage(UI_STRING_MESSAGE_IS_USED_ITEM);
        return;
      }

      if (!UI_IsRunAskUseItemDialog()) {
        UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION, 2);
        return;
      } else {
        UI_SetUseAskIitemRunning(false);
      }
    }
    // 2006.05.24 sjheon End
    // by diesirace 20070319
    if (GetItemType() >= 65 && GetItemType() <= 67 ||
        GetItemType() >= 80 && GetItemType() <= 82) {
      bool statusInitlow = false;
      bool initImpossible = false;
      int vampireLowestStatus = 20;
      int vampireStartInitstaus = 30;
      int oustersLowestStatus = 10;
      int oustersStartInitstaus = 20;
      int slayerStartInitstaus = 10;
      int slayerLoweststaus = 0;

      int lowInitStatus = 0;
      int statusStringType = UI_STRING_MESSAGE_STR;
      if (GetItemType() >= 80 && GetItemType() <= 82) {
        oustersLowestStatus = 15;
        oustersStartInitstaus = 25;
      }

      if (g_pPlayer->IsSlayer() && !g_pPlayer->IsAdvancementClass()) {
        UI_PopupMessage(UI_STRING_MESSAGE_USE_ADVANCEMENTCLASS);
        return;
      }

      else if (GetItemType() == 65) {
        statusStringType = UI_STRING_MESSAGE_STR;

        if (g_pPlayer->IsVampire()) {
          if (g_char_slot_ingame.STR_PURE < vampireStartInitstaus &&
              g_char_slot_ingame.STR_PURE > vampireLowestStatus) {
            statusInitlow = true;
            lowInitStatus = g_char_slot_ingame.STR_PURE - vampireLowestStatus;
          }
          if (g_char_slot_ingame.STR_PURE == vampireLowestStatus) {
            initImpossible = true;
          }
        } else if (g_pPlayer->IsSlayer()) {
          if (g_char_slot_ingame.SlayerAdvancedStr < slayerStartInitstaus &&
              g_char_slot_ingame.SlayerAdvancedStr > slayerLoweststaus) {
            statusInitlow = true;
            lowInitStatus =
                g_char_slot_ingame.SlayerAdvancedStr - slayerLoweststaus;
          }
          if (g_char_slot_ingame.SlayerAdvancedStr == slayerLoweststaus) {
            initImpossible = true;
          }
        } else {
          if (g_char_slot_ingame.STR_PURE < oustersStartInitstaus &&
              g_char_slot_ingame.STR_PURE > oustersLowestStatus) {
            statusInitlow = true;
            lowInitStatus = g_char_slot_ingame.STR_PURE - oustersLowestStatus;
          }
          if (g_char_slot_ingame.STR_PURE == oustersLowestStatus) {
            initImpossible = true;
          }
        }
      } else if (GetItemType() == 66) {
        statusStringType = UI_STRING_MESSAGE_DEX;

        if (g_pPlayer->IsVampire()) {
          if (g_char_slot_ingame.DEX_PURE < vampireStartInitstaus &&
              g_char_slot_ingame.DEX_PURE > vampireLowestStatus) {
            statusInitlow = true;
            lowInitStatus = g_char_slot_ingame.DEX_PURE - vampireLowestStatus;
          }
          if (g_char_slot_ingame.DEX_PURE == vampireLowestStatus) {
            initImpossible = true;
          }

        } else if (g_pPlayer->IsSlayer()) {
          if (g_char_slot_ingame.SlayerAdvancedDex < slayerStartInitstaus &&
              g_char_slot_ingame.SlayerAdvancedDex > slayerLoweststaus) {
            statusInitlow = true;
            lowInitStatus =
                g_char_slot_ingame.SlayerAdvancedDex - slayerLoweststaus;
          }
          if (g_char_slot_ingame.SlayerAdvancedDex == slayerLoweststaus) {
            initImpossible = true;
          }
        } else {
          if (g_char_slot_ingame.DEX_PURE < oustersStartInitstaus &&
              g_char_slot_ingame.DEX_PURE > oustersLowestStatus) {
            statusInitlow = true;
            lowInitStatus = g_char_slot_ingame.DEX_PURE - oustersLowestStatus;
          }
          if (g_char_slot_ingame.DEX_PURE == oustersLowestStatus) {
            initImpossible = true;
          }
        }
      } else if (GetItemType() == 67) {
        statusStringType = UI_STRING_MESSAGE_INT;

        if (g_pPlayer->IsVampire()) {
          if (g_char_slot_ingame.INT_PURE < vampireStartInitstaus &&
              g_char_slot_ingame.INT_PURE > vampireLowestStatus) {
            statusInitlow = true;
            lowInitStatus = g_char_slot_ingame.INT_PURE - vampireLowestStatus;
          }
          if (g_char_slot_ingame.INT_PURE == vampireLowestStatus) {
            initImpossible = true;
          }
        } else if (g_pPlayer->IsSlayer()) {
          if (g_char_slot_ingame.SlayerAdvancedInt < slayerStartInitstaus &&
              g_char_slot_ingame.SlayerAdvancedInt > slayerLoweststaus) {
            statusInitlow = true;
            lowInitStatus =
                g_char_slot_ingame.SlayerAdvancedInt - slayerLoweststaus;
          }
          if (g_char_slot_ingame.SlayerAdvancedInt == slayerLoweststaus) {
            initImpossible = true;
          }
        } else {
          if (g_char_slot_ingame.INT_PURE < oustersStartInitstaus &&
              g_char_slot_ingame.INT_PURE > oustersLowestStatus) {
            statusInitlow = true;
            lowInitStatus = g_char_slot_ingame.INT_PURE - oustersLowestStatus;
          }
          if (g_char_slot_ingame.INT_PURE == oustersLowestStatus) {
            initImpossible = true;
          }
        }
      } else if (GetItemType() == 80) {
        statusStringType = UI_STRING_MESSAGE_STR;
        if (g_pPlayer->IsVampire()) {
          statusInitlow = true;
          lowInitStatus = g_char_slot_ingame.STR_PURE - vampireLowestStatus;
          if (g_char_slot_ingame.STR_PURE == vampireLowestStatus ||
              lowInitStatus) {
            initImpossible = true;
          }
        } else if (g_pPlayer->IsSlayer()) {
          statusInitlow = true;
          lowInitStatus =
              g_char_slot_ingame.SlayerAdvancedStr - slayerLoweststaus;
          if (g_char_slot_ingame.SlayerAdvancedStr == slayerLoweststaus ||
              lowInitStatus) {
            initImpossible = true;
          }
        } else {
          statusInitlow = true;
          lowInitStatus = g_char_slot_ingame.STR_PURE - oustersLowestStatus;
          if (g_char_slot_ingame.STR_PURE == oustersLowestStatus ||
              lowInitStatus) {
            initImpossible = true;
          }
        }
      } else if (GetItemType() == 81) {
        statusStringType = UI_STRING_MESSAGE_DEX;
        if (g_pPlayer->IsVampire()) {
          statusInitlow = true;
          lowInitStatus = g_char_slot_ingame.DEX_PURE - vampireLowestStatus;
          if (g_char_slot_ingame.DEX_PURE == vampireLowestStatus ||
              lowInitStatus) {
            initImpossible = true;
          }
        } else if (g_pPlayer->IsSlayer()) {
          statusInitlow = true;
          lowInitStatus =
              g_char_slot_ingame.SlayerAdvancedDex - slayerLoweststaus;
          if (g_char_slot_ingame.SlayerAdvancedDex == slayerLoweststaus ||
              lowInitStatus) {
            initImpossible = true;
          }
        } else {
          statusInitlow = true;
          lowInitStatus = g_char_slot_ingame.DEX_PURE - oustersLowestStatus;
          if (g_char_slot_ingame.DEX_PURE == oustersLowestStatus ||
              lowInitStatus) {
            initImpossible = true;
          }
        }
      } else if (GetItemType() == 82) {
        statusStringType = UI_STRING_MESSAGE_INT;
        if (g_pPlayer->IsVampire()) {
          statusInitlow = true;
          lowInitStatus = g_char_slot_ingame.INT_PURE - vampireLowestStatus;
          if (g_char_slot_ingame.INT_PURE == vampireLowestStatus ||
              lowInitStatus) {
            initImpossible = true;
          }
        } else if (g_pPlayer->IsSlayer()) {
          statusInitlow = true;
          lowInitStatus =
              g_char_slot_ingame.SlayerAdvancedInt - slayerLoweststaus;
          if (g_char_slot_ingame.SlayerAdvancedInt == slayerLoweststaus ||
              lowInitStatus) {
            initImpossible = true;
          }
        } else {
          statusInitlow = true;
          lowInitStatus = g_char_slot_ingame.INT_PURE - oustersLowestStatus;
          if (g_char_slot_ingame.INT_PURE == oustersLowestStatus ||
              lowInitStatus) {
            initImpossible = true;
          }
        }
      }

      if (!UI_IsRunAskUseItemDialog()) {
        if (statusInitlow) {
          UI_RunAskUseItemDialog(
              C_VS_UI_ASK_DIALOG::ASK_DYEPOTION_INITPOTION, lowInitStatus,
              (*g_pGameStringTable)[statusStringType].GetString());
          return;
        }
        if (initImpossible) {
          UI_PopupMessage(UI_STRING_MESSAGE_CANNOTUSE_INITSTATUS);
          return;
        } else {
          UI_RunAskUseItemDialog(C_VS_UI_ASK_DIALOG::ASK_USE_DYE_POTION);
          return;
        }
      } else {
        UI_SetUseAskIitemRunning(false);
      }
    }
    // end

    // 2005, 2, 22, sobeit add end
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());

    g_pSocket->sendPacket(&_CGUseItemFromInventory);
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
  }
#endif
}
//
// void	MDyePotionItem::UseQuickItem()
//{
// #ifdef __GAME_CLIENT__
//
//	if( g_pPlayer->IsOusters() )
//	{
//		// 아우스터즈는 성이 없기 때문에 성전환 아이템을 사용할 수 없다
//		if(GetItemType() == 48)
//			UI_PopupMessage(STRING_MESSAGE_CANNOT_USE_OUSTERS);
//		else
//		{
//			CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
//			_CGUsePotionFromQuickSlot.setObjectID( GetID() );
//			int slotID = GetItemSlot();
//
//			if( g_pArmsBand1 != NULL )	// 1번 암스밴드가 있으면
//그냥
//			{
//				if(g_pArmsBand1->GetItem( slotID ) == this)
//					_CGUsePotionFromQuickSlot.setSlotID(
//slotID ); 				else 					_CGUsePotionFromQuickSlot.setSlotID( slotID + 3);
//			}
//			else						// 1번
//암스밴드가 없으면 2번 +3 을 해준다
//			{
//				_CGUsePotionFromQuickSlot.setSlotID( slotID +
//3);
//			}
//
//		//
//_CGUsePotionFromQuickSlot.setSlotID( slotID ); 			g_pSocket->sendPacket(
//&_CGUsePotionFromQuickSlot ); 			g_pPlayer->SetItemCheckBuffer( this,
//MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
//		//	DEBUG_ADD_FORMAT("[ Ousters Item Use From QuickSlot ] %d
//%d",slotID, GetID() );
//
//			//----------------------------------------------------
//			// 벨트 못 없애도록..
//			//----------------------------------------------------
//			UI_LockGear();
//		}
//	}
//	else
//	{
//		CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
//		_CGUsePotionFromQuickSlot.setObjectID( GetID() );
//		_CGUsePotionFromQuickSlot.setSlotID( GetItemSlot() );
//
//		g_pSocket->sendPacket( &_CGUsePotionFromQuickSlot );
//
//
//		// 일단(!) 그냥 없애고 본다.
//		//(*g_pInventory).RemoveItem( GetID() );
//
//		//----------------------------------------------------
//		// Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
//		//----------------------------------------------------
//		g_pPlayer->SetItemCheckBuffer( this,
//MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
//
//		//----------------------------------------------------
//		// 벨트 못 없애도록..
//		//----------------------------------------------------
//		UI_LockGear();
//	}
// #endif
//}

void MOustersSummonGem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  if (IsChargeItem() && GetNumber() > 0 && IsAffectStatus() && !g_bZoneSafe &&
      !g_pPlayer->IsInSafeSector() && g_pPlayer->IsStop() &&
      !g_pPlayer->HasEffectStatus(EFFECTSTATUS_HAS_FLAG)) {
    if (!g_pPlayer->IsAdvancementClass() &&
        GetItemCanAdvance().GetAttr(NONADVANCE_CANNOT_USE_ITEM)) {
      UI_PopupMessage(UI_STRING_MESSAGE_USE_ADVANCEMENTCLASS);
      return;
    }
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());

    if (0 != SubInventoryItemID)
      _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

    g_pSocket->sendPacket(&_CGUseItemFromInventory);
    g_pPlayer->SetItemCheckBuffer(this,
                                  MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY,
                                  SubInventoryItemID);
    g_pSkillInfoTable->SetAvailableTime(SKILL_SUMMON_SYLPH, 4000);
  }
#endif
}

void MCodeSheetItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  if (GetItemType() == 0 && GetItemOptionListCount() == 30) {
    gC_vs_ui.RunQuestInventory(this);
  }
#endif
}

void MPotion::UseQuickItem() {
#ifdef __GAME_CLIENT__
  CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
  _CGUsePotionFromQuickSlot.setObjectID(GetID());
  _CGUsePotionFromQuickSlot.setSlotID(GetItemSlot());

  g_pSocket->sendPacket(&_CGUsePotionFromQuickSlot);

  // 일단(!) 그냥 없애고 본다.
  //(*g_pInventory).RemoveItem( GetID() );

  //----------------------------------------------------
  // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
  //----------------------------------------------------
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);

  //----------------------------------------------------
  // 벨트 못 없애도록..
  //----------------------------------------------------
  UI_LockGear();

  // [도움말] 벨트의 아이템 사용
//	__BEGIN_HELP_EVENT
///		ExecuteHelpEvent( HE_ITEM_USE_BELT_ITEM );
//	__END_HELP_EVENT
#endif
}

void MOustersPupa::UseQuickItem() {
#ifdef __GAME_CLIENT__
  CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
  _CGUsePotionFromQuickSlot.setObjectID(GetID());
  int slotID = GetItemSlot();

  if (g_pArmsBand1 != NULL) // 1번 암스밴드가 있으면 그냥
  {
    if (g_pArmsBand1->GetItem(slotID) == this)
      _CGUsePotionFromQuickSlot.setSlotID(slotID);
    else
      _CGUsePotionFromQuickSlot.setSlotID(slotID + 3);
  } else // 1번 암스밴드가 없으면 2번 +3 을 해준다
  {
    _CGUsePotionFromQuickSlot.setSlotID(slotID + 3);
  }

  //					_CGUsePotionFromQuickSlot.setSlotID(
  //slotID );
  g_pSocket->sendPacket(&_CGUsePotionFromQuickSlot);
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
  //	DEBUG_ADD_FORMAT("[ Ousters Item Use From QuickSlot ] %d %d",slotID,
  //GetID() );

  //----------------------------------------------------
  // 벨트 못 없애도록..
  //----------------------------------------------------
  UI_LockGear();
#endif
}

void MOustersComposMei::UseQuickItem() {
#ifdef __GAME_CLIENT__
  CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
  _CGUsePotionFromQuickSlot.setObjectID(GetID());
  int slotID = GetItemSlot();

  if (g_pArmsBand1 != NULL) // 1번 암스밴드가 있으면 그냥
  {
    if (g_pArmsBand1->GetItem(slotID) == this)
      _CGUsePotionFromQuickSlot.setSlotID(slotID);
    else
      _CGUsePotionFromQuickSlot.setSlotID(slotID + 3);
  } else // 1번 암스밴드가 없으면 2번 +3 을 해준다
  {
    _CGUsePotionFromQuickSlot.setSlotID(slotID + 3);
  }

  //					_CGUsePotionFromQuickSlot.setSlotID(
  //slotID );
  g_pSocket->sendPacket(&_CGUsePotionFromQuickSlot);
  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);
  //	DEBUG_ADD_FORMAT("[ Ousters Item Use From QuickSlot ] %d %d",slotID,
  //GetID() );

  //----------------------------------------------------
  // 벨트 못 없애도록..
  //----------------------------------------------------
  UI_LockGear();
#endif
}

void MCoupleRing::UseGear() {
#ifdef __GAME_CLIENT__
  CGUseItemFromGear _CGUseItemFromGear;

  _CGUseItemFromGear.setObjectID(GetID());
  _CGUseItemFromGear.setPart(GetItemSlot());

  g_pSocket->sendPacket(&_CGUseItemFromGear);

  g_pPlayer->SetItemCheckBuffer(this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_GEAR);
#endif
}

void MVampireCoupleRing::UseGear() {
#ifdef __GAME_CLIENT__
  CGUseItemFromGear _CGUseItemFromGear;

  _CGUseItemFromGear.setObjectID(GetID());
  _CGUseItemFromGear.setPart(GetItemSlot());

  g_pSocket->sendPacket(&_CGUseItemFromGear);

  g_pPlayer->SetItemCheckBuffer(this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_GEAR);
#endif
}

void MPetItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 현재 펫이 있으면 펫을 호출하지 못하도록 했으나 펫을 없앨때도 같이
  // 사용하므로 주석처리했음 어떤 아이템으로 펫을 호출했는지 아이템에
  // 표시해주는것도....오토바이도 안하니까 쌩

  //	if(g_pPlayer->GetPetID() == OBJECTID_NULL)
  //	{
  if (GetCurrentDurability() > 0) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());
    if (0 != SubInventoryItemID)
      _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

    g_pSocket->sendPacket(&_CGUseItemFromInventory);

    g_pPlayer->SetItemCheckBuffer(this,
                                  MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY,
                                  SubInventoryItemID);
  }
//	}
#endif
}

#if __CONTENTS(__PET_MIXINGFORGE)
void RemovePetMixingOption(TYPE_ITEM_OPTION petMixingOption) {}
void AddPetMixingOption(TYPE_ITEM_OPTION petMixingOption) {}
void ChangePetMixingOption(TYPE_ITEM_OPTION ori_petMixingOption,
                           TYPE_ITEM_OPTION new_petMixingOption) {}
#endif //__PET_MIXINGFORGE

void MPetFood::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  const TYPE_OBJECTID petID = g_pPlayer->GetPetID();

  if (g_pPlayer != NULL && petID != OBJECTID_NULL) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());
    if (0 != SubInventoryItemID)
      _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

    g_pSocket->sendPacket(&_CGUseItemFromInventory);

    g_pPlayer->SetItemCheckBuffer(this,
                                  MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY,
                                  SubInventoryItemID);
  } else {
    // 2개 이상 있어야 분리 된다.
    if (GetNumber() > 1) {
      // 아이템 분리
      CGAddInventoryToMouse _CGAddInventoryToMouse;
      _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
      _CGAddInventoryToMouse.setX(GetGridX());
      _CGAddInventoryToMouse.setY(GetGridY());
      _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
      g_pSocket->sendPacket(&_CGAddInventoryToMouse);

      //----------------------------------------------------
      // Inventory에서 item을 몇개 드는 걸 검증받는다.
      // 일단은 1개만 들리도록 한다.
      //----------------------------------------------------
      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
    }
  }
#endif
}

void MSms_item::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  if (g_pPlayer != NULL) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());

    g_pSocket->sendPacket(&_CGUseItemFromInventory);

    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
  }
#endif
}

void MPetEnchantItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__

  // 2개 이상 있어야 분리 된다.
  if (GetNumber() > 1) {
    // 아이템 분리
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);

    //----------------------------------------------------
    // Inventory에서 item을 몇개 드는 걸 검증받는다.
    // 일단은 1개만 들리도록 한다.
    //----------------------------------------------------
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
  }
#endif
}

MPetItem::MPetItem() {
  m_UpdateTime = timeGetTime();
  m_PetKeepedDay = 0;
  m_PetFoodType = 0;
  m_PetCreatureType = 0;
  m_bCanGamble = false;
  m_bCutHead = false;
  m_bCanAttack = false;
  m_EnchantSkillType = 0;
}

std::string MPetItem::GetPetOptionName() {
  std::string petOptionName;
  if (GetSilver() > 0) {
    ITEMOPTION_TABLE::ITEMOPTION_PART optionPart =
        static_cast<ITEMOPTION_TABLE::ITEMOPTION_PART>(GetEnchantLevel());
    int optionLevel = (GetNumber() - 10) / 10;

    int size = g_pItemOptionTable->GetSize();
    bool bFound = false;

    for (int i = 1; i < size; i++) {
      ITEMOPTION_INFO &optionInfo = g_pItemOptionTable->Get(i);
      if (optionInfo.Part == optionPart) {
        bFound = true;
        if (optionLevel == 0) {
          petOptionName = optionInfo.Name;
          petOptionName += " ";
          break;
        } else {
          optionLevel--;
        }
      }
    }
    if (optionLevel != 0 && bFound == true) // 다른 옵션 파트가 나왔을때
    {
      switch (optionPart) {
      case ITEMOPTION_TABLE::PART_LUCK:
        petOptionName =
            (*g_pGameStringTable)[STRING_MESSAGE_OPTION_NAME_LUCK_3 +
                                  optionLevel]
                .GetString();
        petOptionName += " ";
        break;

      case ITEMOPTION_TABLE::PART_ALL_ATTR:
        petOptionName =
            (*g_pGameStringTable)[STRING_MESSAGE_OPTION_NAME_ATTR_3 +
                                  optionLevel]
                .GetString();
        petOptionName += " ";
        break;
      }
    }
  }

  return petOptionName;
}

std::string MPetItem::GetPetOptionEName() {
  std::string petOptionName;
  if (GetSilver() > 0) {
    ITEMOPTION_TABLE::ITEMOPTION_PART optionPart =
        static_cast<ITEMOPTION_TABLE::ITEMOPTION_PART>(GetEnchantLevel());
    int optionLevel = (GetNumber() - 10) / 10;

    int size = g_pItemOptionTable->GetSize();
    bool bFound = false;

    for (int i = 1; i < size; i++) {
      ITEMOPTION_INFO &optionInfo = g_pItemOptionTable->Get(i);
      if (optionInfo.Part == optionPart) {
        bFound = true;
        if (optionLevel == 0) {
          petOptionName = optionInfo.EName;
          petOptionName += " ";
          break;
        } else {
          optionLevel--;
        }
      }
    }
    if (optionLevel != 0 && bFound == true) // 다른 옵션 파트가 나왔을때
    {
      switch (optionPart) {
      case ITEMOPTION_TABLE::PART_LUCK:
        petOptionName =
            (*g_pGameStringTable)[STRING_MESSAGE_OPTION_ENAME_LUCK_3 +
                                  optionLevel]
                .GetString();
        petOptionName += " ";
        break;

      case ITEMOPTION_TABLE::PART_ALL_ATTR:
        petOptionName =
            (*g_pGameStringTable)[STRING_MESSAGE_OPTION_ENAME_ATTR_3 +
                                  optionLevel]
                .GetString();
        petOptionName += " ";
        break;
      }
    }
  }

  return petOptionName;
}

std::string MPetItem::GetPetName() {
  // Tür adı yalnızca creature.inf. ItemType switch 1–33'te bitiyordu;
  // 113/117 (Pixie 711) ve 34+ boş kalıyordu. Kaynak: GCPetInfo PetCreatureType
  // (sunucu PetTypeInfo, sprite ile aynı satır).
  std::string petName = GetPetOptionName();

  int creatureType = (int)m_PetCreatureType;

#ifdef __GAME_CLIENT__
  if (creatureType <= 0 && g_pPlayer != NULL && g_pZone != NULL) {
    const TYPE_OBJECTID petID = g_pPlayer->GetPetID();
    if (petID != OBJECTID_NULL) {
      MFakeCreature *pFake = dynamic_cast<MFakeCreature *>(
          g_pZone->GetFakeCreature(petID));
      if (pFake != NULL && pFake->GetPetItem() == this)
        creatureType = (int)pFake->GetCreatureType();
    }
  }
#endif

  // 0 = Slayer Male; pet CreatureType1=0 satırı isim olarak kullanılmaz.
  if (g_pCreatureTable != NULL && creatureType > 0 &&
      creatureType < (int)g_pCreatureTable->GetSize()) {
    const char *infName = (*g_pCreatureTable)[creatureType].Name.GetString();
    if (infName != NULL && infName[0] != '\0')
      petName += infName;
  }

  return petName;
}

void MEventGiftBoxItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
  int itemType = GetItemType();

#ifdef __GAME_CLIENT__
  if (itemType >= 6) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());

    g_pSocket->sendPacket(&_CGUseItemFromInventory);

    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
  }
#endif
}

void MMoonCardItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 2개 이상 있어야 분리 된다.

  int itemNum = GetNumber();
  if (GetItemType() == 2 && itemNum > 1 && itemNum <= GetMaxNumber()) {
    // 아이템 분리
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);

    //----------------------------------------------------
    // Inventory에서 item을 몇개 드는 걸 검증받는다.
    // 일단은 1개만 들리도록 한다.
    //----------------------------------------------------
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
  }
#endif
}

void MTrapItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  CGUseItemFromInventory _CGUseItemFromInventory;
  _CGUseItemFromInventory.setObjectID(GetID());
  _CGUseItemFromInventory.setX(GetGridX());
  _CGUseItemFromInventory.setY(GetGridY());

  g_pSocket->sendPacket(&_CGUseItemFromInventory);

  g_pPlayer->SetItemCheckBuffer(this,
                                MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
#endif
}

void MSubInventory::OpenAllSubInventory() {
#ifdef __GAME_CLIENT__
  MItemClassFinder itemFinder(ITEM_CLASS_SUB_INVENTORY);
  ITEM_LIST SubInventoryList;

  g_pInventory->FindItemList(itemFinder, SubInventoryList);

  ITEM_LIST::iterator pos = SubInventoryList.begin();

  for (; pos != SubInventoryList.end(); ++pos) {
    MItem *tempSubinventory = *pos;

    if (gC_vs_ui.ExistSubInventory(tempSubinventory->GetID())) {
      gC_vs_ui.RunSubInventory(tempSubinventory);
    } else {
      CGUseItemFromInventory _CGUseItemFromInventory;
      _CGUseItemFromInventory.setObjectID(tempSubinventory->GetID());
      _CGUseItemFromInventory.setX(tempSubinventory->GetGridX());
      _CGUseItemFromInventory.setY(tempSubinventory->GetGridY());

      g_pSocket->sendPacket(&_CGUseItemFromInventory);

      g_pPlayer->SetItemCheckBuffer(
          tempSubinventory, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
    }
  }
#endif
}

void MSubInventory::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  TYPE_OBJECTID itemID = GetID();

  if (g_pDXInput->KeyDown(DIK_LSHIFT)) {
    OpenAllSubInventory();
  } else {
    if (gC_vs_ui.ExistSubInventory(itemID)) {
      if (gC_vs_ui.IsRunningSubInventory(itemID)) {
        gC_vs_ui.CloseSubInventory(itemID);
      } else {
        gC_vs_ui.RunSubInventory(this);
      }
    } else {
      CGUseItemFromInventory _CGUseItemFromInventory;
      _CGUseItemFromInventory.setObjectID(GetID());
      _CGUseItemFromInventory.setX(GetGridX());
      _CGUseItemFromInventory.setY(GetGridY());

      g_pSocket->sendPacket(&_CGUseItemFromInventory);

      g_pPlayer->SetItemCheckBuffer(
          this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
    }
  }
#endif
}

void MSubInventory::SetItemType(TYPE_ITEMTYPE type) {
  switch (type) {
  case 0: // 2*4 pack
    MGridItemManager::Init(2, 4);
    break;

  //  2005.08.15 Sjheon 4x6 팩 Add
  case 1: // 4*6 pack
    MGridItemManager::Init(4, 6);
    break;
    //  2005.08.15 Sjheon 4x6 팩 End

  case 2:
    MGridItemManager::Init(10, 6);
    break;

  default:
    MGridItemManager::Init(2, 4);
    break;
  }
  m_ItemType = type;
}
//----------------------------------------------------------------------
// AddItem ( pItem )
//----------------------------------------------------------------------
// 적절한 slot에 pItem을 추가한다.
//----------------------------------------------------------------------
bool MSubInventory::AddItem(MItem *pItem) {
  pItem->SetParentInventory(this);

  bool re = MGridItemManager::AddItem(pItem);
  SetEnchantLevel(MGridItemManager::GetItemNum()); // Enchant level을 아이템
                                                   // 갯수로 쓰기로 했음.
  return re;
}

//----------------------------------------------------------------------
// AddItem ( pItem, n )
//----------------------------------------------------------------------
// slot(n)에 pItem을 추가한다.
//----------------------------------------------------------------------
bool MSubInventory::AddItem(MItem *pItem, BYTE X, BYTE Y) {
  pItem->SetParentInventory(this);

  bool re = MGridItemManager::AddItem(pItem, X, Y);
  SetEnchantLevel(MGridItemManager::GetItemNum()); // Enchant level을 아이템
                                                   // 갯수로 쓰기로 했음.
  return re;
}

//----------------------------------------------------------------------
// ReplaceItem
//----------------------------------------------------------------------
// pItem을 추가하고 딴게 있다면 Item교환
//----------------------------------------------------------------------
bool MSubInventory::ReplaceItem(MItem *pItem, BYTE X, BYTE Y,
                                MItem *&pOldItem) {
  pItem->SetParentInventory(this);

  bool re = MGridItemManager::ReplaceItem(pItem, X, Y, pOldItem);
  SetEnchantLevel(MGridItemManager::GetItemNum()); // Enchant level을 아이템
                                                   // 갯수로 쓰기로 했음.
  return re;
}

//----------------------------------------------------------------------
// Can ReplaceItem : (n) slot에 pItem을 추가하거나
//						원래 있던 Item과 교체가
//가능한가?
//----------------------------------------------------------------------
bool MSubInventory::CanReplaceItem(MItem *pItem, BYTE X, BYTE Y,
                                   MItem *&pOldItem) {
  return MGridItemManager::CanReplaceItem(pItem, X, Y, pOldItem);
}

//----------------------------------------------------------------------
// Get Item Name Color Set
// : 아이템의 이름 색상을 바꾼다.
//	 자꾸 바껴서 소스파일로 뺐다. 컴파일 시간이 후덜덜이라 -chyaya
//----------------------------------------------------------------------
int MCoreZap::GetItemNameColorSet() const { return COVER_ITEM_NAME_COLOR; }

int MEtherealChain::GetItemNameColorSet() const {
  return COVER_ITEM_NAME_COLOR;
}

int MCueOfAdam::GetItemNameColorSet() const { return COVER_ITEM_NAME_COLOR; }

//----------------------------------------------------------------------
// MSkillBook
//----------------------------------------------------------------------
void MSkillBook::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  CGUseItemFromInventory _CGUseItemFromInventory;
  _CGUseItemFromInventory.setObjectID(GetID());
  _CGUseItemFromInventory.setX(GetGridX());
  _CGUseItemFromInventory.setY(GetGridY());

  if (0 != SubInventoryItemID)
    _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

  g_pSocket->sendPacket(&_CGUseItemFromInventory);

  g_pPlayer->SetItemCheckBuffer(
      this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID);
  // g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_NPC_ASK);
#endif //__GAME_CLIENT__
}

#if __CONTENTS(__LEVEL_WAR_RENEWAL)
void MCommonQuestItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  // 2개 이상 있어야 분리 된다.
  if (GetNumber() > 1) {
    //		if(GetItemType() >= 6 && GetItemType() <= 11)
    //		{
    //		}
    //		else
    //		{
    // 아이템 분리
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);

    //----------------------------------------------------
    // Inventory에서 item을 몇개 드는 걸 검증받는다.
    // 일단은 1개만 들리도록 한다.
    //----------------------------------------------------
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
    // 		}
  }
#endif
}
#endif //__LEVEL_WAR_RENEWAL

#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
void MVampireWingItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
  /**  by svi
#ifdef __GAME_CLIENT__
  CGUseItemFromInventory _CGUseItemFromInventory;
  _CGUseItemFromInventory.setObjectID( GetID() );
  _CGUseItemFromInventory.setX( GetGridX() );
  _CGUseItemFromInventory.setY( GetGridY() );

  if(0 != SubInventoryItemID)
          _CGUseItemFromInventory.setInventoryItemObjectID( SubInventoryItemID
);

  g_pSocket->sendPacket( &_CGUseItemFromInventory );

  g_pPlayer->SetItemCheckBuffer( this,
MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID); #endif
//__GAME_CLIENT__
  */

#ifdef __GAME_CLIENT__
  //----------------------------------------------------
  //
  //					변신용 아이템 - vampire인 경우
  //
  //----------------------------------------------------
  if (g_pZoneTable->Get(g_pZone->GetID())->CannotUseSpecialItem == false) {
    ACTIONINFO useSkill; // = g_pPlayer->GetSpecialActionInfo();

    MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetID());

    if (pCreature) // Sjheon 2005.09.15 Add
    {
      if (pCreature->GetAction() == ACTION_VAMPIRE_DRAIN) {
        if (GetItemClass() == ITEM_CLASS_SKULL || GetItemType() == 0 ||
            GetItemType() == 1 || GetItemType() == 2) // 늑대 머리
        {
          return;
        }
      }
    }
    // Sjheon 2005.09.15 End

    if (GetItemType() == 0) {
      useSkill = MAGIC_TRANSFORM_TO_BAT; // 膠틔돕세콘
    } else if (GetItemType() == 1) {
      useSkill = MAGIC_TRANSFORM_TO_BAT;
    } else if (GetItemType() == 2) {
      useSkill = MAGIC_TRANSFORM_TO_BAT;
    } else if (GetItemType() == 3) {
      useSkill = MAGIC_TRANSFORM_TO_BAT;
    }
    //----------------------------------------------------
    // skill이 제대로 설정된 경우
    //----------------------------------------------------
    if (useSkill != ACTIONINFO_NULL) {
      if (g_pPlayer->IsVampire() &&
          g_pPlayer->GetAction() == ACTION_VAMPIRE_DRAIN)
        return;

      if ( //(*g_pActionInfoTable)[playerSkill].IsTargetItem()
          g_pSkillAvailable->IsEnableSkill(useSkill) &&
          (*g_pSkillInfoTable)[useSkill].IsAvailableTime()) {
        //----------------------------------------------------
        // 검증 받을게 없는 경우..
        //----------------------------------------------------
        if (!g_pPlayer->IsWaitVerify() && g_pPlayer->IsItemCheckBufferNULL()) {
          CGSkillToInventory _CGSkillToInventory;
          _CGSkillToInventory.setObjectID(GetID());
          _CGSkillToInventory.setX(GetGridX());
          _CGSkillToInventory.setY(GetGridY());
          _CGSkillToInventory.setSkillType(useSkill);
          //_CGSkillToInventory.setCEffectID( 0 );	// -_-;;

          if (0 != SubInventoryItemID)
            _CGSkillToInventory.setInventoryItemObjectID(SubInventoryItemID);
          g_pSocket->sendPacket(&_CGSkillToInventory);

          // 일단(!) 그냥 없애고 본다.
          //(*g_pInventory).RemoveItem( GetID() );

          //----------------------------------------------------
          // Inventory에서 item을 사용하는 걸 검증받기를 기다린다.
          //----------------------------------------------------
          g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);
          g_pPlayer->SetItemCheckBuffer(
              this, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY,
              SubInventoryItemID);

          g_pSkillInfoTable->SetNextAvailableTime(useSkill);

          //----------------------------------------------------
          // 기술 사용 시도 동작
          //----------------------------------------------------
          AddNewInventoryEffect(
              GetID(),
              useSkill, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
              g_pClientConfig->FPS * 3 // 3초
          );
        }
      }
    }
  }
#endif
}

void MOustersWingItem::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  CGUseItemFromInventory _CGUseItemFromInventory;
  _CGUseItemFromInventory.setObjectID(GetID());
  _CGUseItemFromInventory.setX(GetGridX());
  _CGUseItemFromInventory.setY(GetGridY());

  if (0 != SubInventoryItemID)
    _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

  g_pSocket->sendPacket(&_CGUseItemFromInventory);

  g_pPlayer->SetItemCheckBuffer(
      this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID);
#endif //__GAME_CLIENT__
}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

#if __CONTENTS(__GLOBAL_NPC)
TYPE_ITEM_NUMBER
MCallNPCNPCCard::GetMaxNumber() const { return MAX_CALLNPC_CARD; }
void MCallNPCNPCCard::UseInventory(TYPE_OBJECTID SubInventoryItemID) {
#ifdef __GAME_CLIENT__
  if (GetNumber() > 1) {
    // 아이템 분리
    CGAddInventoryToMouse _CGAddInventoryToMouse;
    _CGAddInventoryToMouse.setObjectID(0); // 분리한다는 의미
    _CGAddInventoryToMouse.setX(GetGridX());
    _CGAddInventoryToMouse.setY(GetGridY());
    _CGAddInventoryToMouse.setInventoryItemObjectID(SubInventoryItemID);
    g_pSocket->sendPacket(&_CGAddInventoryToMouse);

    //----------------------------------------------------
    // Inventory에서 item을 몇개 드는 걸 검증받는다.
    // 일단은 1개만 들리도록 한다.
    //----------------------------------------------------
    g_pPlayer->SetItemCheckBuffer(
        this, MPlayer::ITEM_CHECK_BUFFER_PICKUP_SOME_FROM_INVENTORY);
  }
  // 20081030 이아이템 클래스는 npc와 연동 되어 있는 inventory UI 이용중에는
  // 사용이 불가능 하다. 또한 이동중에는 사용 불가능하다.
  else if (!gbl_sell_running && !gbl_buy_running && !gbl_repair_running &&
           !gbl_silvering_running && !gbl_persnalshop_running &&
           !gbl_swap_advancement_item_running &&
           g_pPlayer->GetAction() == ACTION_STAND) {
    CGUseItemFromInventory _CGUseItemFromInventory;
    _CGUseItemFromInventory.setObjectID(GetID());
    _CGUseItemFromInventory.setX(GetGridX());
    _CGUseItemFromInventory.setY(GetGridY());

    if (0 != SubInventoryItemID)
      _CGUseItemFromInventory.setInventoryItemObjectID(SubInventoryItemID);

    g_pSocket->sendPacket(&_CGUseItemFromInventory);
    g_pPlayer->SetItemCheckBuffer(this,
                                  MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY,
                                  SubInventoryItemID);
    g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_NPC_ASK);
  }
#endif //__GAME_CLIENT__
}
#endif //__GLOBAL_NPC
