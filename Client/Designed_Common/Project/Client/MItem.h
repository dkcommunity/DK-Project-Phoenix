//----------------------------------------------------------------------
// MItem.h
//----------------------------------------------------------------------
// 아이템..
//----------------------------------------------------------------------
/*

 [ class hierarchy ]


  ITEM	- BELT
                        - MOTORCYCLE
                        - CORPSE

                - PILE_ITEM
                        - POTION
                        - MAGAZINE

                - GEAR_ITEM
                        - RING
                        - BRACELET
                        - NECKLACE
                        - SHOES
                        - SWORD
                        - BLADE
                        - SHIELD
                        - CROSS
                        - GLOVE
                        - HELM
                        - GUN_SG
                        - GUN_SMG
                        - GUN_AR
                        - GUN_TR

                - WATER
                - HOLYWATER
                - ETC
                - MOTORCYCLE(-I)
                - KEY(+V)
                - BELT
                - BOMB_MATERIAL
                - LEARNINGITEM
                - MONEY(+V)
                - BOMB
                - MINE

                - VAMPIRE_ITEM(+V)
                        - VAMPIRE_GEAR_ITEM
                                - VAMPIRE_EARRING
                                - VAMPIRE_RING
                                - VAMPIRE_BRACELET
                                - VAMPIRE_NECKLACE
                                - VAMPIRE_COAT
                                - VAMPIRE_SHOES

*/

#ifndef __MITEM_H__
#define __MITEM_H__

#include "ItemClassDef.h"
#include "MGridItemManager.h"
#include "MITemTable.h"
#include "MObject.h"
#include "MSlotItemManager.h"
#include <string>

// #include "SkillDef.h"
// #include "AddonDef.h"
#include "CAnimationFrame.h"
#include "RaceType.h"
class MCreature;

#define MAX_DROP_COUNT 6

class MItem : public MObject, public CAnimationFrame {
public:
  //----------------------------------------------------------------------
  // Item이 장착되는 위치
  //----------------------------------------------------------------------
  enum ITEM_GEAR_SLOT {
    SLOT_HELM = 0,  // 모자
    SLOT_NECKLACE,  // 목걸이
    SLOT_COAT,      // 상의
    SLOT_LEFTHAND,  // 왼손
    SLOT_RIGHTHAND, // 오른손
    SLOT_TWOHAND,   // 양손
    SLOT_BELT,      // 혁대
    SLOT_BRACELET,  // 팔찌
    SLOT_TROUSER,   // 하의
    SLOT_RING,      // 반지
    SLOT_SHOES,     // 신발
    SLOT_GLOVE,     // 장갑
    SLOT_PDA,       // PDA
    SLOT_SHOULDER,  // 어깨

    // vampire
    SLOT_VAMPIRE_RING,
    SLOT_VAMPIRE_BRACELET,
    SLOT_VAMPIRE_NECKLACE,
    SLOT_VAMPIRE_COAT,
    SLOT_VAMPIRE_EARRING,
    SLOT_VAMPIRE_LEFTHAND,
    SLOT_VAMPIRE_RIGHTHAND,
    SLOT_VAMPIRE_TWOHAND, // 양손
    SLOT_VAMPIRE_AMULET,
    SLOT_VAMPIRE_DERMIS,  // 문신
    SLOT_VAMPIRE_PERSONA, // 가면

    // ousters
    SLOT_OUSTERS_CIRCLET,   // 서클릿
    SLOT_OUSTERS_COAT,      // 옷
    SLOT_OUSTERS_LEFTHAND,  // 왼손
    SLOT_OUSTERS_RIGHTHAND, // 오른손
    SLOT_OUSTERS_TWOHAND,   // 양손
    SLOT_OUSTERS_BOOTS,     // 신발
    SLOT_OUSTERS_ARMSBAND,  // 암스밴드
    SLOT_OUSTERS_RING,      // 링
    SLOT_OUSTERS_PENDENT,   // 목걸이
    SLOT_OUSTERS_STONE,     // 정령석
    SLOT_OUSTERS_FASCIA,    // 허리장식
    SLOT_OUSTERS_MITTEN,    // 장갑

    // 공통
    SLOT_BLOOD_BIBLE, // 블러드 바이블

    MAX_SLOT, // 최대값

    SLOT_NULL // 없는 경우
  };

public:
  MItem();
  virtual ~MItem();

  //-------------------------------------------------------
  // Item의 class
  //-------------------------------------------------------
  virtual ITEM_CLASS GetItemClass() const { return ITEM_CLASS_NULL; }

  //-------------------------------------------------------
  // ItemType
  //-------------------------------------------------------
  virtual void SetItemType(TYPE_ITEMTYPE type) { m_ItemType = type; }
  TYPE_ITEMTYPE GetItemType() const { return m_ItemType; }

  //-------------------------------------------------------
  // ItemStyle
  //-------------------------------------------------------
  int GetItemStyle() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle;
  }
  int IsNormalItem() const {
    return ((*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle == 0);
  }
  int IsUniqueItem() const {
    return ((*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle == 1);
  }
  // 2 = isim RGB geçişi (unique değil). drop_item_name_tag = 2..8 durak sayısı.
  // 3 = sol-orta-sağ (kırmızı-yeşil-mavi) yumuşak lerp.
  int IsNameColorCycleItem() const {
    return ((*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle == 2);
  }
  COLORREF GetNameColorCycle() const;
  COLORREF GetNameColorCycleAtGlyph(int glyphIndex, int glyphCount) const;
  static int CountNameGlyphs(const char *str);
  static int ReadNameGlyph(const char *str, int offset, char out[3]);
  int IsQuestItem() const;
  bool IsSpecialColorItem() const;

#if __CONTENTS(__TUNING_ITEM)
  bool IsTuningItem() const;
#endif //__TUNING_ITEM
#if __CONTENTS(__ONIBLA_ITEM)
  bool IsSeniorItem() const;
#endif //__ONIBLA_ITEM

  virtual bool IsRepairableItem() const { return true; }

  int GetSpecialColorItemColorset();
  static int GetSpecialColorItemColorset(unsigned short srcColor);
  static int GetUniqueItemColorset();
  static int GetItemOptionExtColorSet(WORD wColorSet);
  static int GetQuestItemColorset();
  int GetRareItemColorset();
  int GetItemOptionExtColorSet();
#if __CONTENTS(__ONIBLA_ITEM)
  int GetSeniorItemColorSet();
#endif //__ONIBLA_ITEM

#if __CONTENTS(__TUNING_ITEM)
  static int GetTuningItemColorSet();
#endif //__TUNING_ITEM

#if __CONTENTS(__LEVEL_WAR_RENEWAL)
  static int GetLevelWarItemColorset();
#endif //__LEVEL_WAR_RENEWAL

  //-------------------------------------------------------
  // ItemOption
  //-------------------------------------------------------
  int GetItemDefaultOptionCount() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].DefaultOptionList.size();
  }
  bool IsEmptyItemDefaultOption() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType]
        .DefaultOptionList.empty();
  }
  const std::list<TYPE_ITEM_OPTION> &GetItemDefaultOptionList() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].DefaultOptionList;
  }
  void SetItemOptionList(const std::list<TYPE_ITEM_OPTION> &option_list) {
    m_ItemOptionList = option_list;
  }
  const std::list<TYPE_ITEM_OPTION> &GetItemOptionList() const {
    return m_ItemOptionList;
  }
  int GetItemOptionListCount() const { return m_ItemOptionList.size(); }
  bool IsEmptyItemOptionList() const { return m_ItemOptionList.empty(); }
  void RemoveItemOption(TYPE_ITEM_OPTION option);
  void AddItemOption(TYPE_ITEM_OPTION option);
  void ChangeItemOption(TYPE_ITEM_OPTION ori_option,
                        TYPE_ITEM_OPTION new_option);
  void ClearItemOption() { m_ItemOptionList.clear(); }

  //-------------------------------------------------------
  // Slayer / Vampire / Ousters ?
  //-------------------------------------------------------
  int IsSlayerItem() const {
    return ((*g_pItemTable)[GetItemClass()][m_ItemType].Race &
            FLAG_RACE_SLAYER);
  }
  int IsVampireItem() const {
    return ((*g_pItemTable)[GetItemClass()][m_ItemType].Race &
            FLAG_RACE_VAMPIRE);
  }
  int IsOustersItem() const {
    return ((*g_pItemTable)[GetItemClass()][m_ItemType].Race &
            FLAG_RACE_OUSTERS);
  }

  //-------------------------------------------------------
  // 기본 Item Color
  //-------------------------------------------------------
  void SetItemColorSet(WORD cs) { m_ItemColorSet = cs; }
  WORD GetItemColorSet() const { return m_ItemColorSet; }

  //-------------------------------------------------------
  // 속성
  //-------------------------------------------------------
  // 승직 전용 아이템인가?
  virtual bool IsAdvancementClassItem() const { return false; }

  // Container로 사용할 수 있는 Item인가?
  virtual bool IsContainerItem() const { return false; }

  // 같은 Item끼리 쌓이는가?
  virtual bool IsPileItem() const { return false; }

  // 쌓이는 아이템중에 분리가 되는 아이템인가?
  virtual bool IsDetachableItem() const { return false; }

  // 쌓이는거랑은 다르다. 개수만 바뀐다.
  virtual bool IsChargeItem() const { return false; }

  // 장착할 수 있는가?
  virtual bool IsGearItem() const { return false; }

  // 착용해서 그림이 바뀌는 Item인가?
  virtual bool IsAddonItem() const { return false; }

  // 기본 공격 무기인가?
  virtual bool IsBasicWeapon() const { return false; }

  // 총인가?
  virtual bool IsGunItem() const { return false; }

  virtual bool IsMeleeWeapon() const { return false; }
  virtual bool IsMagicWeapon() const { return false; }
  virtual bool IsCanAddThirdOptionItem() const { return false; }

  // Inventory에 들어갈 수 있는 Item인가?
  virtual bool IsInventoryItem() const { return true; }

  // QuickItem에 들어가는가?
  // QuickItem은 기본적으로 사용가능한것들만 넣기 때문에 QuickItem이면 무조건
  // 사용 가능하다.
  virtual bool IsQuickItem() const { return false; }

  // 사용 가능한 아이템인가?
  virtual void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0) {}
  virtual void UseQuickItem() {}
  virtual void UseGear() {}

  // 다른 Item에 추가될 수 있는가?
  virtual bool IsInsertToItem(const MItem *pItem) const;

  // 아이템 이름의 색상이 바뀌는가?
  bool IsItemNameColorChange() const { return GetItemNameColorSet() != -1; }
#if __CONTENTS(__ADD_MAGICPROTECTION)
  // 마법 방어력 속성을 가지는가?
  virtual bool HasMagicProtection() const { return false; }
#endif //__ADD_MAGICPROTECTION
       // 아이템 이름을 바꿀 칼라셋을 지정한다.
  virtual int GetItemNameColorSet() const { return -1; }

  // 장착할 수 있는 곳은?
  virtual ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_NULL; }

  // 특정 부위에 장착할 수 있는가?
  virtual bool IsGearSlotHelm() const { return false; }       // 모자
  virtual bool IsGearSlotNecklace() const { return false; }   // 목걸이
  virtual bool IsGearSlotCoat() const { return false; }       // 상의
  virtual bool IsGearSlotLeftHand() const { return false; }   // 왼손
  virtual bool IsGearSlotRightHand() const { return false; }  // 오른손
  virtual bool IsGearSlotTwoHand() const { return false; }    // 양손
  virtual bool IsGearSlotBelt() const { return false; }       // 혁대
  virtual bool IsGearSlotBracelet() const { return false; }   // 팔찌
  virtual bool IsGearSlotTrouser() const { return false; }    // 하의
  virtual bool IsGearSlotRing() const { return false; }       // 반지
  virtual bool IsGearSlotShoes() const { return false; }      // 신발
  virtual bool IsGearSlotGlove() const { return false; }      // 장갑
  virtual bool IsGearSlotCoreZap() const { return false; }    // 코어잽
  virtual bool IsGearSlotBloodBible() const { return false; } // 블러드 바이블
  virtual bool IsGearSlotPDA() const { return false; }        // PDA 슬롯
  virtual bool IsGearSlotShoulder() const { return false; }   // 어깨 슬롯
  virtual bool IsGearSlotEtherealChain() const {
    return false;
  }                                                    // 목걸이 코어잽
  virtual bool IsGearSlotCue() const { return false; } // 큐

  // vampire
  virtual bool IsGearSlotVampireEarRing() const { return false; }
  virtual bool IsGearSlotVampireRing() const { return false; }
  virtual bool IsGearSlotVampireBracelet() const { return false; }
  virtual bool IsGearSlotVampireNecklace() const { return false; }
  virtual bool IsGearSlotVampireCoat() const { return false; }
  virtual bool IsGearSlotVampireLeftHand() const { return false; }
  virtual bool IsGearSlotVampireRightHand() const { return false; }
  virtual bool IsGearSlotVampireTwoHand() const { return false; } // 양손
  virtual bool IsGearSlotVampireAmulet() const { return false; }
  virtual bool IsGearSlotVampireCoreZap() const { return false; } // 코어잽
  virtual bool IsGearSlotVampireBloodBible() const {
    return false;
  } // 블러드 바이블
  virtual bool IsGearSlotVampireDermis() const { return false; }  // 문신
  virtual bool IsGearSlotVampirePersona() const { return false; } // 페르소나
  virtual bool IsGearSlotVampireEtherealChain() const {
    return false;
  }                                                           // 목걸이 코어잽
  virtual bool IsGearSlotVampireCue() const { return false; } // 큐

  // ousters
  virtual bool IsGearSlotOustersCirclet() const { return false; }
  virtual bool IsGearSlotOustersCoat() const { return false; }
  virtual bool IsGearSlotOustersLeftHand() const { return false; }
  virtual bool IsGearSlotOustersRightHand() const { return false; }
  virtual bool IsGearSlotOustersTwoHand() const { return false; } // 양손
  virtual bool IsGearSlotOustersBoots() const { return false; }
  virtual bool IsGearSlotOustersArmsBand() const { return false; }
  virtual bool IsGearSlotOustersRing() const { return false; }
  virtual bool IsGearSlotOustersPendent() const { return false; }
  virtual bool IsGearSlotOustersStone() const { return false; }
  virtual bool IsGearSlotOustersCoreZap() const { return false; } // 코어잽
  virtual bool IsGearSlotOustersBloodBible() const {
    return false;
  } // 블러드 바이블
  virtual bool IsGearSlotOustersFascia() const { return false; } // 허리장식
  virtual bool IsGearSlotOustersMitten() const { return false; } // 장갑
  virtual bool IsGearSlotOustersEtherealChain() const {
    return false;
  }                                                           // 목걸이 코어잽
  virtual bool IsGearSlotOustersCue() const { return false; } // 큐

  // 2005, 1, 3, sobeit add start
  virtual bool IsDurationAlwaysOkay() const {
    return false;
  } // 내구 체크 안하는 아이템
  // 2005, 1, 3, sobeit add end

  //-------------------------------------------------------
  // 아이템 이름 : 한글 / 영어?
  //-------------------------------------------------------
  static bool IsUseKorean() { return s_bUseKorean; }
  static bool IsUseEnglish() { return !s_bUseKorean; }
  static void UseKorean() { s_bUseKorean = true; }
  static void UseEnglish() { s_bUseKorean = false; }

  //---------------------------------------------------
  // class마다 고정된 정보
  //---------------------------------------------------
  const char *GetName();
  const char *GetEName() const;
  const char *GetDescription() const;
  TYPE_ITEM_WEIGHT GetWeight() const;
  TYPE_ITEM_PRICE GetPrice() const;
  BYTE GetGridWidth() const;
  BYTE GetGridHeight() const;

  // FrameID
  TYPE_FRAMEID GetTileFrameID() const;      // Tile에서의 FrameID
  TYPE_FRAMEID GetInventoryFrameID() const; // Inventory에서의 Frame ID
  TYPE_FRAMEID GetGearFrameID() const;      // Gear에서의 Frame ID
  TYPE_FRAMEID GetAddonMaleFrameID() const;
  TYPE_FRAMEID GetAddonFemaleFrameID() const;
  TYPE_FRAMEID GetDropFrameID() const; // 바닥에 떨어지는 Item
  TYPE_FRAMEID GetUseFrameID() const;  // 사용할 때 Item

  // Sound ID
  TYPE_SOUNDID GetUseSoundID() const;       // Item 사용 SoundID
  TYPE_SOUNDID GetTileSoundID() const;      // Item 줍기 SoundID
  TYPE_SOUNDID GetInventorySoundID() const; // Inventory에서의 Sound
  TYPE_SOUNDID GetGearSoundID() const;      // Gear에서의 Sound

  // addon slot
  virtual ADDON GetAddonSlot() const { return ADDON_NULL; }

  const ITEM_MOVE_CONTROL_ATTR GetItemMoveContorl() const;
  const ITEM_USE_CAN_ADVANCE GetItemCanAdvance() const;

  //---------------------------------------------------
  // 성별
  //---------------------------------------------------
  bool IsGenderForMale() const;
  bool IsGenderForFemale() const;
  bool IsGenderForAll() const;

  //---------------------------------------------------
  // 필요능력
  //---------------------------------------------------
  int GetRequireSTR() const;
  int GetRequireDEX() const;
  int GetRequireINT() const;
  int GetRequireLevel() const;
  int GetRequireSUM() const;

  //---------------------------------------------------
  // 장착했을 때 바뀌는.. BasicActionInfo
  //---------------------------------------------------
  TYPE_ACTIONINFO GetUseActionInfo() const;

  //---------------------------------------------------
  // Option에 따른 정보
  //---------------------------------------------------
  BYTE GetItemOptionPart(int OptionNum = 0) const;
  const char *GetItemOptionName(int OptionNum = 0) const;
  const char *GetItemOptionEName(int OptionNum = 0) const;
  int GetItemOptionPlusPoint(int OptionNum = 0) const;
  //		BYTE GetItemOptionPlusRequireAbility() const;
  DWORD GetItemOptionPriceMultiplier() const;
  WORD GetItemOptionColorSet(int OptionNum = 0);
  int GetItemOptionRequireSTR() const;
  int GetItemOptionRequireDEX() const;
  int GetItemOptionRequireINT() const;
  int GetItemOptionRequireLevel() const;
  int GetItemOptionRequireSUM() const;

  //---------------------------------------------------
  // 확장 Option에 따른 정보
  //---------------------------------------------------
  int IsItemOptionExt(); // const;
  void SetItemOptionExtFlag(bool isOptionExt = true) {
    m_OptionExt = isOptionExt;
  }
  //---------------------------------------------------
  // 세부 정보
  //---------------------------------------------------
  // Inventory에서의 좌표
  void SetGridXY(BYTE x, BYTE y) {
    m_GridX = x;
    m_GridY = y;
  }
  BYTE GetGridX() const { return m_GridX; }
  BYTE GetGridY() const { return m_GridY; }

  // 어떤(?) Slot에서의 좌표 (m_GridX를 이용한다. 관계없다. - -;;)
  void SetItemSlot(BYTE n) { m_GridX = n; }
  BYTE GetItemSlot() const { return m_GridX; }

  // 내구성(현재)
  TYPE_ITEM_DURATION GetCurrentDurability() const {
    return m_CurrentDurability;
  }
  virtual void SetCurrentDurability(TYPE_ITEM_DURATION d) {
    m_CurrentDurability = d;
  }

  // 개수( 돈, 쌓인 개수)
  virtual TYPE_ITEM_NUMBER GetNumber() const { return m_Number; }
  virtual void SetNumber(TYPE_ITEM_NUMBER n) { m_Number = n; }

  // 최대 개수
  virtual TYPE_ITEM_NUMBER GetMaxNumber() const { return 1; }

  //---------------------------------------------------
  // Value들..
  //---------------------------------------------------
  virtual int GetMaxDurability() const { return -1; }    // 내구성
  virtual int GetCarryingCapacity() const { return -1; } // 운반 가능 무게
  virtual int GetProtectionValue() const { return -1; }  // 방어력
  virtual int GetDefenseValue() const { return -1; }     // 방어력
  virtual int GetPocketNumber() const { return -1; }     // 포켓수
  virtual int GetMinDamage() const { return -1; }        // 최소 공격력
  virtual int GetMaxDamage() const { return -1; }        // 최대 공격력
  virtual int GetHealPoint() const { return -1; }        // HP수치
  virtual int GetManaPoint() const { return -1; }        // MP수치
  virtual ITEM_CLASS GetGunClass() const {
    return ITEM_CLASS_NULL;
  }                                                  // 총의 종류
  virtual int GetMagazineSize() const { return -1; } // 탄창 수
  virtual int GetReach() const { return 1; }         // 사정거리
  virtual int GetToHit() const { return -1; }        // ToHit
  virtual int GetCriticalHit() const { return -1; }  // 크리티컬힛 확률
  virtual int GetLucky() const { return -9999; }     // 럭키!
#if __CONTENTS(__TUNING_ITEM)
  virtual int GetTargetClass() const { return -1; }
  virtual int GetTargetType() const { return -1; }
#endif //__TUNING_ITEM
#if __CONTENTS(__ADD_MAGICPROTECTION)
  virtual int GetMagicProtection() const { return -1; } // 마법방어력
#endif                                                  //__ADD_MAGICPROTECTION
  virtual int GetRankGemOptionClass() const { return -1; }

  //---------------------------------------------------
  // Item떨어뜨리기..
  //---------------------------------------------------
  void SetDropping();
  BOOL IsDropping() const { return m_bDropping; }
  int GetDropHeight() const { return s_DropHeight[m_DropCount]; }
  void NextDropFrame();

  //---------------------------------------------------
  // Identified
  //---------------------------------------------------
  BOOL IsIdentified() const { return m_bIdentified; }
  void SetIdentified() { m_bIdentified = TRUE; }
  void UnSetIdentified() { m_bIdentified = FALSE; }

  //-----------------------------------------------
  // AffectStatus - 수치 적용되는가?
  //-----------------------------------------------
  bool IsAffectStatus() const { return m_bAffectStatus; }
  void SetAffectStatus() { m_bAffectStatus = true; }
  void UnSetAffectStatus() { m_bAffectStatus = false; }

  //---------------------------------------------------
  // Trade - 교환할려고 선택한 Item인가?
  //---------------------------------------------------
  BOOL IsTrade() const { return m_bTrade; }
  void SetTrade() { m_bTrade = TRUE; }
  void UnSetTrade() { m_bTrade = FALSE; }

  //---------------------------------------------------
  // Silver
  //---------------------------------------------------
  int GetSilverMax() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].SilverMax;
  } // Silver coating최대값
  int GetSilver() const { return m_Silver; }
  virtual void SetSilver(int s) { m_Silver = s; }

  //---------------------------------------------------
  // Speed
  //---------------------------------------------------
  int GetOriginalSpeed() const;
  int GetSpeed() const;
  void SetSpeed(WORD s) { m_Speed = s; }

  //---------------------------------------------------
  // Grade
  //---------------------------------------------------
  int GetGrade() const { return m_Grade; }
  virtual void SetGrade(int s) { m_Grade = s; }

  //---------------------------------------------------
  // EnchantLevel
  //---------------------------------------------------
  WORD GetEnchantLevel() const { return m_EnchantLevel; }
  virtual void SetEnchantLevel(WORD s) { m_EnchantLevel = s; }

  //-------------------------------------------------------
  // 이름
  //-------------------------------------------------------
  void SetName(const char *pName);

  //-------------------------------------------------------
  // Quest
  //-------------------------------------------------------
  void SetQuestFlag(bool isQuest = true) { m_Quest = isQuest; }

  void SetPersnalPrice(TYPE_ITEM_PRICE price) { m_persnal_price = price; }
  TYPE_ITEM_PRICE GetPersnalPrice() { return m_persnal_price; }

  void SetPersnal(bool persnal) { m_persnal = persnal; }
  bool GetPersnal() { return m_persnal; }

  //-------------------------------------------------------
  // File I/O
  //-------------------------------------------------------
  // void	SaveToFile(std::ofstream& file);
  // void	LoadFromFile(ivfstream& file);

  //-------------------------------------------------------
  // Parent Inventory
  //-------------------------------------------------------
  void SetParentInventory(MGridItemManager *parent) {
    m_ParentInventory = parent;
  }

  MGridItemManager *GetParentInventory() { return m_ParentInventory; }
  void
  SetItemThirdOptionList(const std::list<TYPE_ITEM_OPTION> &thirdOption_list) {
    m_ItemThirdOptionList = thirdOption_list;
  }
  const std::list<TYPE_ITEM_OPTION> &GetItemThirdOptionList() const {
    return m_ItemThirdOptionList;
  }
  int GetItemThirdOptionListCount() const {
    return m_ItemThirdOptionList.size();
  }
  bool IsEmptyItemThirdOptionList() const {
    return m_ItemThirdOptionList.empty();
  }
  void RemoveItemThirdOption(TYPE_ITEM_OPTION thirdOption);
  void AddItemThirdOption(TYPE_ITEM_OPTION thirdOption);
  void ChangeItemThirdOption(std::list<TYPE_ITEM_OPTION> &thirdOption);
  void ClearItemThirdOption() { m_ItemThirdOptionList.clear(); }
  BYTE GetItemThirdOptionType() { return m_ThirdEnchantType; }
  void SetItemThirdOptionType(BYTE thirdEnchantType) {
    m_ThirdEnchantType = thirdEnchantType;
  }

  //////////////////맣芚//////////////
  void
  SetItemGTwoOptionList(const std::list<TYPE_ITEM_OPTION> &GTwooption_list) {
    m_GTwoItemOptionList = GTwooption_list;
  }
  const std::list<TYPE_ITEM_OPTION> &GetItemGTwoOptionList() const {
    return m_GTwoItemOptionList;
  }
  int GetItemGTwoOptionListCount() const { return m_GTwoItemOptionList.size(); }
  bool IsEmptyItemGTwoOptionList() const {
    return m_GTwoItemOptionList.empty();
  }
  void RemoveItemGTwoOption(TYPE_ITEM_OPTION GTwooption);
  void AddItemGTwoOption(TYPE_ITEM_OPTION GTwooption);
  void ChangeItemGTwoOption(TYPE_ITEM_OPTION GTwoori_option,
                            TYPE_ITEM_OPTION GTwonew_option);
  void ClearGTwoItemOption() { m_GTwoItemOptionList.clear(); }
  ///////////////////////////////////////////////////

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  void SetCashItem(BYTE cashItem) { m_CashItem = cashItem; }
  BYTE GetCashItem() const { return m_CashItem; }
#endif //__INTERVATIONAL_PREMIUM_SYSTEM

#if __CONTENTS(__GEAR_SWAP_CHANGE) // MItem 착용 가능한지의 값을 설정 하거나
                                   // 얻는다.
  void SetActive(bool bActive) { m_bActive = bActive; }
  BYTE GetActive() { return m_bActive; }
#endif

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
  void SetTimeLimitStatic(DWORD _nTimeLimit_Static) {
    m_nTimeLimit_Static = _nTimeLimit_Static;
  }
  DWORD GetTimeLimitStatic() { return m_nTimeLimit_Static; }
  bool IsTimeLimitStatic() const { return (m_nTimeLimit_Static > 0); }
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__PET_VISION_AMPLE)
  void SetPet_RedBirdOptionNum(BYTE _RedBirdOption) {
    m_nPet_RedBirdOption = _RedBirdOption;
  }
  BYTE GetPet_RedBirdOptionNum() const { return m_nPet_RedBirdOption; }

  void SetPet_AmpulOptionNum(BYTE _AmpulOption) {
    m_nPet_AmpulOption = _AmpulOption;
  }
  BYTE GetPet_AmpulOptionNum() const { return m_nPet_AmpulOption; }
  void SetPet_op3(BYTE _op3) { m_nPet_op3 = _op3; }
  BYTE GetPet_op3() const { return m_nPet_op3; }
#endif //__PET_VISION_AMPLE

public:
  // NewItem()에 대한 functions pointer
  typedef MItem *(*FUNCTION_NEWITEMCLASS)();

  //-------------------------------------------------------
  // 원하는 class의 Item의 객체를 생성한다.
  //-------------------------------------------------------
  static MItem *NewItem(ITEM_CLASS itemClass);

  // 각 class별로 NewItem()에 대한 function pointer
  static FUNCTION_NEWITEMCLASS s_NewItemClassTable[MAX_ITEM_CLASS];
#if __CONTENTS(__PET_MIXINGFORGE)

  void SetPetMixingOptionList(
      const std::list<TYPE_ITEM_OPTION> &petMixingOption_list) {
    m_PetMixingOptionList = petMixingOption_list;
  }
  const std::list<TYPE_ITEM_OPTION> &GetPetMixingOptionList() const {
    return m_PetMixingOptionList;
  }
  int GetPetMixingOptionListCount() const {
    return m_PetMixingOptionList.size();
  }
  bool IsEmptyPetMixginOptionList() const {
    return m_PetMixingOptionList.empty();
  }
  void ClearPetMixingOption() { m_PetMixingOptionList.clear(); }

  void RemovePetMixingOption(TYPE_ITEM_OPTION petMixingOption);
  void AddPetMixingOption(TYPE_ITEM_OPTION petMixingOption);
  void ChangePetMixingOption(TYPE_ITEM_OPTION ori_petMixingOption,
                             TYPE_ITEM_OPTION new_petMixingOption_list);

#endif //__PET_MIXINGFORGE

  //-------------------------------------------------------
  //
  //			하나의 Item instance에 대한 정보
  //
  //-------------------------------------------------------
protected:
  // Item의 종류에 대한 ID
  TYPE_ITEMTYPE m_ItemType;

  // Item의 option
  std::list<TYPE_ITEM_OPTION> m_ItemOptionList;

  std::list<TYPE_ITEM_OPTION> m_GTwoItemOptionList;

  // 기본 Color
  WORD m_ItemColorSet;

  // Inventory에서의 좌표
  BYTE m_GridX;
  BYTE m_GridY;

  // 세부 정보
  TYPE_ITEM_DURATION m_CurrentDurability;

  // 개수 (돈, 쌓인 개수)
  TYPE_ITEM_NUMBER m_Number;

  //---------------------------------------------------
  // drop
  //---------------------------------------------------
  BOOL m_bDropping;                        // 떨어지고 있는 중
  int m_DropCount;                         // 현재 count
  static int s_DropHeight[MAX_DROP_COUNT]; // Drop 높이

  //---------------------------------------------------
  // identified
  //---------------------------------------------------
  BOOL m_bIdentified;

  //---------------------------------------------------
  // item입은 상태가 모두 제대로 적용됐는가?
  //---------------------------------------------------
  bool m_bAffectStatus;

  //---------------------------------------------------
  // 교환할 item인가?
  //---------------------------------------------------
  BOOL m_bTrade;

  //---------------------------------------------------
  // Silver
  //---------------------------------------------------
  int m_Silver;

  //---------------------------------------------------
  // Speed
  //---------------------------------------------------
  int m_Speed;

  //---------------------------------------------------
  // Grade
  //---------------------------------------------------
  int m_Grade;

  //---------------------------------------------------
  // Enchant Level
  //---------------------------------------------------
  int m_EnchantLevel;

  //---------------------------------------------------
  // item 이름
  //---------------------------------------------------
  char *m_pName;

  // Item이름에 한글을 사용하는가?
  static bool s_bUseKorean;

  //---------------------------------------------------
  // 아이템이 퀘스트 아이템인가? (Time Manager에 등록되지 않았을경우..
  //---------------------------------------------------
  bool m_Quest;
  bool m_persnal;
  bool m_OptionExt;

  int m_OptionExtFrame;
  int m_OptionExtTime;
  int m_OptionExtGrade;
  // by thai00
  TYPE_ITEM_PRICE m_persnal_price;

  //---------------------------------------------------
  // 부모 인벤토리의 포인터를 기억한다.
  //---------------------------------------------------
  // Main Inventory: g_pInventory
  // Sub Inventory: SubInventory Pointer
  // No Inventory: NULL
  //---------------------------------------------------
  MGridItemManager *m_ParentInventory;
  std::list<TYPE_ITEM_OPTION> m_ItemThirdOptionList;
  BYTE m_ThirdEnchantType; // 0 밀리 무기, 1 마법 무기, 2 방어구, 255없음

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
  BYTE m_CashItem;
#endif

#if __CONTENTS(__GEAR_SWAP_CHANGE) // MItem 착용 가능한 아이템인지 확인 할 변수
  bool m_bActive;
#endif

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
  DWORD m_nTimeLimit_Static; // 지급받지 않은 아이템에서 Description으로
                             // 보여주기 위한 최초리미티드 Time
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__PET_VISION_AMPLE)
  BYTE m_nPet_RedBirdOption;
  BYTE m_nPet_AmpulOption;
  BYTE m_nPet_op3;
#endif //__PET_VISION_AMPLE
#if __CONTENTS(__PET_MIXINGFORGE)
  std::list<TYPE_ITEM_OPTION> m_PetMixingOptionList;
#endif //__PET_MIXINGFORGE
};

//----------------------------------------------------------------------
//
//          Gear에 사용하는 Item인 경우.. 착용할 수 있다.
//
//----------------------------------------------------------------------
class MGearItem : public MItem {
public:
  MGearItem() {}
  virtual ~MGearItem() {}

public:
  bool IsGearItem() const { return true; }
  virtual bool IsCanAddThirdOptionItem() const { return true; }
  // 값
  virtual int GetMaxDurability()
      const; //	{ return (*g_pItemTable)[GetItemClass()][m_ItemType].Value1; }
             //// 내구성
  virtual int GetProtectionValue() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].Value2;
  } // 방어력
  virtual int GetDefenseValue() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].Value6;
  } // def
};

//----------------------------------------------------------------------
//
//               Pile(쌓이는) Item인 경우..
//
//----------------------------------------------------------------------
class MPileItem : public MItem {
public:
  MPileItem() { m_Number = 1; }
  virtual ~MPileItem() {}

  TYPE_ITEM_NUMBER GetNumber() const { return m_Number; }
  void SetNumber(TYPE_ITEM_NUMBER n) { m_Number = n; }
  virtual bool IsRepairableItem() const { return false; }
  bool IsPileItem() const { return true; }
};

//-------------------------------------------------------
// MUsePotionItem - 포션 아이템은 기본적으로 같은 루틴이라 검사 조건을
// 제외하고는 상속받아서 으쌰으쌰
//-------------------------------------------------------
class MUsePotionItem : public MPileItem {
public:
  virtual bool IsRepairableItem() const { return false; }
  virtual void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
};

//----------------------------------------------------------------------
// MArmorItem
//----------------------------------------------------------------------
class MArmorItem : public MGearItem {
public:
  MArmorItem() {}
  ~MArmorItem() {}

  int GetProtectionValue() const;
  int GetDefenseValue() const;
};
//----------------------------------------------------------------------
// MArmorItem2
//----------------------------------------------------------------------
class MArmorItem2 : public MGearItem {
public:
  MArmorItem2() {}
  ~MArmorItem2() {}

  int GetProtectionValue() const;
  int GetDefenseValue() const;
  int GetMaxDurability() const;
};
//----------------------------------------------------------------------
// Accessory Item
//----------------------------------------------------------------------
class MAccessoryItem : public MGearItem {
public:
  MAccessoryItem() {}
  ~MAccessoryItem() {}

#if __CONTENTS(__ADD_MAGICPROTECTION)
  int GetMagicProtection() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].Value7;
  }
#endif

  int GetLucky() const;
};
//----------------------------------------------------------------------
// BELT
//----------------------------------------------------------------------
class MBelt : public MArmorItem2, public MSlotItemManager {
public:
  MBelt() {}
  ~MBelt() {}

public:
  //------------------------------------------------
  //
  //					MGearItem
  //
  //------------------------------------------------
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BELT; }

  //------------------------------------------------
  // ItemType설정
  //------------------------------------------------
  void SetItemType(TYPE_ITEMTYPE type);

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_BELT; }
  bool IsGearSlotBelt() const { return true; }

  bool IsContainerItem() const { return true; }

  //------------------------------------------------
  //
  //				SlotItemManager
  //
  //------------------------------------------------
  // AddItem : slot(n)에 pItem을 추가한다.
  //------------------------------------------------
  bool AddItem(MItem *pItem);
  bool AddItem(MItem *pItem, BYTE n);

  //------------------------------------------------
  // ReplaceItem : pItem을 추가하고 딴게 있다면 Item교환
  //------------------------------------------------
  bool ReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem);

  //------------------------------------------------
  // Can ReplaceItem : n slot에 pItem을 추가가능한가?
  // pOldItem에는 이전에 있던 것..
  //------------------------------------------------
  bool CanReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem);

  //------------------------------------------------
  // Find Slot To Add Item
  //------------------------------------------------
  bool FindSlotToAddItem(MItem *pItem, int &slot) const;

  // 값
  virtual int GetPocketNumber() const {
    return (*g_pItemTable)[ITEM_CLASS_BELT][m_ItemType].Value3;
  } // 포켓수

  static MItem *NewItem() { return new MBelt; }
};

//----------------------------------------------------------------------
// CORPSE
//----------------------------------------------------------------------
class MCorpse : public MItem, public MGridItemManager {
public:
  MCorpse();
  ~MCorpse();

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CORPSE; }

  bool IsInventoryItem() const { return false; }

  bool IsContainerItem() const { return true; }

  static MItem *NewItem() { return new MCorpse; }

  // 캐릭터 관련
  //----------------------------------------------------------
  // 탄창
  //----------------------------------------------------------
  MCreature *GetCreature() const { return m_pCreature; }
  MCreature *SetCreature(MCreature *pCreature) {
    MCreature *pTemp = m_pCreature;
    m_pCreature = pCreature;
    return pTemp;
  }

public:
  // 시체의 원래 캐릭터
  MCreature *m_pCreature;
};

//----------------------------------------------------------------------
// POTION
//----------------------------------------------------------------------
class MPotion : public MUsePotionItem {
public:
  MPotion() {}
  ~MPotion() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_POTION; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // quick item
  bool IsQuickItem() const { return true; }

  // heal 되는 수치
  int GetHealPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_POTION][m_ItemType].Value1;
  } // HP수치

  // mana 되는 수치
  int GetManaPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_POTION][m_ItemType].Value2;
  } // MP수치

  // use item
  void UseQuickItem();
  static MItem *NewItem() { return new MPotion; }
};

//----------------------------------------------------------------------
// WATER
//----------------------------------------------------------------------
class MWater : public MPileItem {
public:
  MWater() {}
  ~MWater() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_WATER; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // 사용 가능한 아이템이다.
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MWater; }
};

//----------------------------------------------------------------------
// HOLYWATER
//----------------------------------------------------------------------
class MHolyWater : public MPileItem {
public:
  MHolyWater() {}
  ~MHolyWater() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_HOLYWATER; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // damage
  virtual int GetMinDamage() const {
    return (*g_pItemTable)[ITEM_CLASS_HOLYWATER][m_ItemType].Value1;
  } // 최소 공격력
  virtual int GetMaxDamage() const {
    return (*g_pItemTable)[ITEM_CLASS_HOLYWATER][m_ItemType].Value2;
  } // 최대 공격력

  static MItem *NewItem() { return new MHolyWater; }
};

//----------------------------------------------------------------------
// MAGAZINE
//----------------------------------------------------------------------
class MMagazine : public MPileItem {
public:
  MMagazine() {}
  ~MMagazine() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MAGAZINE; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // quick item
  bool IsQuickItem() const { return true; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
  void UseQuickItem();

  // 다른 Item에 추가될 수 있는가?
  bool IsInsertToItem(const MItem *pItem) const;

  // 탄창 수
  ITEM_CLASS GetGunClass() const {
    return (ITEM_CLASS)(*g_pItemTable)[ITEM_CLASS_MAGAZINE][m_ItemType].Value1;
  } // 총의 종류
  virtual int GetMagazineSize() const {
    return (*g_pItemTable)[ITEM_CLASS_MAGAZINE][m_ItemType].Value2;
  } // 탄창수

  BOOL IsSilverMagazine() const {
    return (*g_pItemTable)[ITEM_CLASS_MAGAZINE][m_ItemType].Value4;
  } // by diesirace 20070227 은총알이여 아녀?

  static MItem *NewItem() { return new MMagazine; }
};

//----------------------------------------------------------------------
// Ring
//----------------------------------------------------------------------
class MRing : public MAccessoryItem {
public:
  MRing() {}
  ~MRing() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_RING; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_RING; }
  bool IsGearSlotRing() const { return true; }
#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MRing; }
};

//----------------------------------------------------------------------
// Bracelet
//----------------------------------------------------------------------
class MBracelet : public MAccessoryItem {
public:
  MBracelet() {}
  ~MBracelet() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BRACELET; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_BRACELET; }
  bool IsGearSlotBracelet() const { return true; }
#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif

  static MItem *NewItem() { return new MBracelet; }
};

//----------------------------------------------------------------------
// NECKLACE
//----------------------------------------------------------------------
class MNecklace : public MAccessoryItem {
public:
  MNecklace() {}
  ~MNecklace() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_NECKLACE; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_NECKLACE; }
  bool IsGearSlotNecklace() const { return true; }
#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif

  static MItem *NewItem() { return new MNecklace; }
};

//----------------------------------------------------------------------
// COAT
//----------------------------------------------------------------------
class MCoat : public MArmorItem {
public:
  MCoat() {}
  ~MCoat() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_COAT; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_COAT; }
  bool IsGearSlotCoat() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_COAT; }

  static MItem *NewItem() { return new MCoat; }
};

//----------------------------------------------------------------------
// Trouser
//----------------------------------------------------------------------
class MTrouser : public MArmorItem {
public:
  MTrouser() {}
  ~MTrouser() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_TROUSER; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TROUSER; }
  bool IsGearSlotTrouser() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_TROUSER; }

  static MItem *NewItem() { return new MTrouser; }
};

//----------------------------------------------------------------------
// SHOES
//----------------------------------------------------------------------
class MShoes : public MArmorItem2 {
public:
  MShoes() {}
  ~MShoes() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SHOES; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_SHOES; }
  bool IsGearSlotShoes() const { return true; }

  static MItem *NewItem() { return new MShoes; }
};

//----------------------------------------------------------------------
// BasicWeapon
//----------------------------------------------------------------------
class MWeaponItem : public MGearItem {
public:
  MWeaponItem() {}
  ~MWeaponItem() {}

  bool IsBasicWeapon() const { return true; }

  int GetMinDamage() const; //		{ return
                            //(*g_pItemTable)[GetItemClass()][m_ItemType].Value3;
                            //}	// 최소 공격력
  int GetMaxDamage() const; //		{ return
                            //(*g_pItemTable)[GetItemClass()][m_ItemType].Value4;
                            //}	// 최대 공격력
  int GetToHit() const; //			{ return
                        //(*g_pItemTable)[GetItemClass()][m_ItemType].ToHit; }
                        //// 최대 공격력
  int GetCriticalHit()
      const; //		{ return
             //(*g_pItemTable)[GetItemClass()][m_ItemType].CriticalHit; }
};

//----------------------------------------------------------------------
// SWORD
//----------------------------------------------------------------------
class MSword : public MWeaponItem {
public:
  MSword() {}
  ~MSword() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SWORD; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_RIGHTHAND; }
  bool IsGearSlotRightHand() const { return true; }

  bool IsGearSlotSword() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }

  bool IsMeleeWeapon() const { return true; }

  static MItem *NewItem() { return new MSword; }
};

//----------------------------------------------------------------------
// BLADE
//----------------------------------------------------------------------
class MBlade : public MWeaponItem {
public:
  MBlade() {}
  ~MBlade() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BLADE; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  bool IsGearSlotBlade() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }

  bool IsMeleeWeapon() const { return true; }
  static MItem *NewItem() { return new MBlade; }
};

//----------------------------------------------------------------------
// Shield
//----------------------------------------------------------------------
class MShield : public MArmorItem2 {
public:
  MShield() {}
  ~MShield() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SHIELD; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_LEFTHAND; }
  bool IsGearSlotLeftHand() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_LEFTHAND; }

  static MItem *NewItem() { return new MShield; }
};

//----------------------------------------------------------------------
//
//----CROSS------------------------------------------------------------------
class MCross : public MWeaponItem {
public:
  MCross() {}
  ~MCross() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CROSS; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  bool IsGearSlotCross() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }

  // 값
  virtual int GetManaPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_CROSS][m_ItemType].Value5;
  } // MP수치
  bool IsMagicWeapon() const { return true; }

  static MItem *NewItem() { return new MCross; }
};

//----------------------------------------------------------------------
// Mace
//----------------------------------------------------------------------
class MMace : public MWeaponItem {
public:
  MMace() {}
  ~MMace() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MACE; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  bool IsGearSlotMace() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }

  // 값
  virtual int GetManaPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_MACE][m_ItemType].Value5;
  } // MP수치
  bool IsMagicWeapon() const { return true; }

  static MItem *NewItem() { return new MMace; }
};

//----------------------------------------------------------------------
// GLOVE
//----------------------------------------------------------------------
class MGlove : public MArmorItem2 {
public:
  MGlove() {}
  ~MGlove() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_GLOVE; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_GLOVE; }
  bool IsGearSlotGlove() const { return true; }

  static MItem *NewItem() { return new MGlove; }
};

//----------------------------------------------------------------------
// HELM
//----------------------------------------------------------------------
class MHelm : public MArmorItem2 {
public:
  MHelm() {}
  ~MHelm() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_HELM; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_HELM; }
  bool IsGearSlotHelm() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_HELM; }

  static MItem *NewItem() { return new MHelm; }
};

//----------------------------------------------------------------------
//
// Gun Item
//
//----------------------------------------------------------------------
class MGunItem : public MWeaponItem {
public:
  MGunItem() { m_pMagazine = NULL; }
  ~MGunItem() {
    if (m_pMagazine != NULL)
      delete m_pMagazine;
  }

public:
  bool IsGunItem() const { return true; } // 총인가?

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }

  // 값
  virtual int GetReach() const {
    return (*g_pItemTable)[GetItemClass()][m_ItemType].Value5;
  } // MP수치
  virtual int GetMagazineSize() const {
    return (m_pMagazine == NULL) ? 0 : m_pMagazine->GetNumber();
  } // 탄창 수

  //----------------------------------------------------------
  // 탄창
  //----------------------------------------------------------
  MMagazine *GetMagazine() const { return m_pMagazine; }
  MMagazine *RemoveMagazine() {
    MMagazine *pTemp = m_pMagazine;
    m_pMagazine = NULL;
    return pTemp;
  }
  MMagazine *SetMagazine(MMagazine *pMagazine) {
    MMagazine *pTemp = m_pMagazine;
    m_pMagazine = pMagazine;
    return pTemp;
  }
  void SetMagazineNULL() { m_pMagazine = NULL; }
  bool IsMeleeWeapon() const { return true; }

protected:
  MMagazine *m_pMagazine;
};

//----------------------------------------------------------------------
// SG
//----------------------------------------------------------------------
class MGunSG : public MGunItem {
public:
  MGunSG() {}
  ~MGunSG() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SG; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  static MItem *NewItem() { return new MGunSG; }
};

//----------------------------------------------------------------------
// SMG
//----------------------------------------------------------------------
class MGunSMG : public MGunItem {
public:
  MGunSMG() {}
  ~MGunSMG() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SMG; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  static MItem *NewItem() { return new MGunSMG; }
};

//----------------------------------------------------------------------
// AR
//----------------------------------------------------------------------
class MGunAR : public MGunItem {
public:
  MGunAR() {}
  ~MGunAR() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_AR; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  static MItem *NewItem() { return new MGunAR; }
};

//----------------------------------------------------------------------
// TR
//----------------------------------------------------------------------
class MGunTR : public MGunItem {
public:
  MGunTR() {}
  ~MGunTR() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SR; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  static MItem *NewItem() { return new MGunTR; }
};

//----------------------------------------------------------------------
// ETC
//----------------------------------------------------------------------
class MItemETC : public MPileItem {
public:
  MItemETC() {}
  ~MItemETC() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_ETC; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
  virtual bool IsRepairableItem() const { return false; }
  static MItem *NewItem() { return new MItemETC; }
};

//----------------------------------------------------------------------
// MOTORCYCLE
//----------------------------------------------------------------------
class MMotorcycle : public MItem, public MGridItemManager {
public:
  MMotorcycle() {}
  ~MMotorcycle() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MOTORCYCLE; }

  bool IsInventoryItem() const { return false; }

  bool IsContainerItem() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_MOTOR; }
  // 값
  virtual int GetMaxDurability() const; //	{ return
                                        //(*g_pItemTable)[ITEM_CLASS_MOTORCYCLE][m_ItemType].Value1;
                                        //}	// 내구성
  virtual int GetCarryingCapacity() const {
    return (*g_pItemTable)[ITEM_CLASS_MOTORCYCLE][m_ItemType].Value2;
  } // 운반가능무게

  static MItem *NewItem() { return new MMotorcycle; }
};

//----------------------------------------------------------------------
// KEY
//----------------------------------------------------------------------
class MKey : public MItem {
public:
  MKey() {}
  ~MKey() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_KEY; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
  void UseQuickItem();
  bool IsQuickItem() const { return true; }

  virtual bool IsRepairableItem() const { return false; }

  static MItem *NewItem() { return new MKey; }
};

//----------------------------------------------------------------------
// Relic
//----------------------------------------------------------------------
class MRelic : public MItem {
public:
  MRelic() {}
  ~MRelic() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_RELIC; }

  static MItem *NewItem() { return new MRelic; }
};

//----------------------------------------------------------------------
// BOMB_MATERIAL
//----------------------------------------------------------------------
class MBombMaterial : public MPileItem {
public:
  MBombMaterial() {}
  ~MBombMaterial() {}

public:
  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BOMB_MATERIAL; }

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MBombMaterial; }
};

//----------------------------------------------------------------------
// LEARNINGITEM
//----------------------------------------------------------------------
class MLearningItem : public MItem {
public:
  MLearningItem() {}
  ~MLearningItem() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_LEARNINGITEM; }

  virtual bool IsRepairableItem() const { return false; }

  static MItem *NewItem() { return new MLearningItem; }
};

//----------------------------------------------------------------------
// MONEY
//----------------------------------------------------------------------
class MMoney : public MPileItem {
public:
  MMoney() { m_Number = 1; }
  ~MMoney() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MONEY; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  static MItem *NewItem() { return new MMoney; }
};

//----------------------------------------------------------------------
// BOMB
//----------------------------------------------------------------------
class MBomb : public MPileItem {
public:
  MBomb() {}
  ~MBomb() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BOMB; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // 값
  int GetMinDamage() const {
    return (*g_pItemTable)[ITEM_CLASS_BOMB][m_ItemType].Value1;
  } // 최소 공격력
  int GetMaxDamage() const {
    return (*g_pItemTable)[ITEM_CLASS_BOMB][m_ItemType].Value2;
  } // 최대 공격력

  static MItem *NewItem() { return new MBomb; }
};

//----------------------------------------------------------------------
// MINE
//----------------------------------------------------------------------
class MMine : public MPileItem {
public:
  MMine() { m_bInstalled = false; }
  ~MMine() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MINE; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // 값
  int GetMinDamage() const {
    return (*g_pItemTable)[ITEM_CLASS_MINE][m_ItemType].Value1;
  } // 최소 공격력
  int GetMaxDamage() const {
    return (*g_pItemTable)[ITEM_CLASS_MINE][m_ItemType].Value2;
  } // 최대 공격력

  // 설치되었나?
  bool IsInstalled() const { return m_bInstalled; }
  void SetInstalled() { m_bInstalled = true; }
  void UnSetInstalled() { m_bInstalled = false; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MMine; }

protected:
  bool m_bInstalled;
};

//----------------------------------------------------------------------
// Skull
//-----------------------------------------------------------------------
/*
class MSkull : public MItem {
        public :
                MSkull() {}
                ~MSkull() {}

        public :
                ITEM_CLASS	GetItemClass() const		{ return
ITEM_CLASS_SKULL; }

                bool		IsVampireItem() const		{ return true; }

                static MItem*	NewItem()	{ return new MSkull; }
};
*/

//----------------------------------------------------------------------
// VAMPIRE_RING
//----------------------------------------------------------------------
class MVampireRing : public MAccessoryItem {
public:
  MVampireRing() {}
  ~MVampireRing() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_RING; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_RING; }
  bool IsGearSlotVampireRing() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MVampireRing; }
};

//----------------------------------------------------------------------
// VAMPIRE_EARRING
//----------------------------------------------------------------------
class MVampireEarRing : public MAccessoryItem {
public:
  MVampireEarRing() {}
  ~MVampireEarRing() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_EARRING; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_EARRING; }
  bool IsGearSlotVampireEarRing() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MVampireEarRing; }
};

//----------------------------------------------------------------------
// VAMPIRE_BRACELET
//----------------------------------------------------------------------
class MVampireBracelet : public MAccessoryItem {
public:
  MVampireBracelet() {}
  ~MVampireBracelet() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_BRACELET; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_BRACELET; }
  bool IsGearSlotVampireBracelet() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MVampireBracelet; }
};

//----------------------------------------------------------------------
// VAMPIRE_NECKLACE
//----------------------------------------------------------------------
class MVampireNecklace : public MAccessoryItem {
public:
  MVampireNecklace() {}
  ~MVampireNecklace() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_NECKLACE; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_NECKLACE; }
  bool IsGearSlotVampireNecklace() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MVampireNecklace; }
};

//----------------------------------------------------------------------
// VAMPIRE_COAT
//----------------------------------------------------------------------
class MVampireCoat : public MArmorItem {
public:
  MVampireCoat() {}
  ~MVampireCoat() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_COAT; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_COAT; }
  bool IsGearSlotVampireCoat() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_COAT; }

  static MItem *NewItem() { return new MVampireCoat; }
};

//----------------------------------------------------------------------
// MSkull
//----------------------------------------------------------------------
class MSkull : public MPileItem {
public:
  MSkull() {}
  ~MSkull() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SKULL; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MSkull; }
};

//----------------------------------------------------------------------
// Serum - 혈청
//----------------------------------------------------------------------
class MSerum : public MUsePotionItem {
public:
  MSerum() {}
  ~MSerum() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SERUM; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // heal 되는 수치
  int GetHealPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_SERUM][m_ItemType].Value1;
  } // HP수치

  static MItem *NewItem() { return new MSerum; }
};

//----------------------------------------------------------------------
// ETC
//----------------------------------------------------------------------
class MVampireETC : public MPileItem {
public:
  MVampireETC() {}
  ~MVampireETC() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_ETC; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MVampireETC; }
};

//----------------------------------------------------------------------
// Slayer Portal
//----------------------------------------------------------------------
class MSlayerPortalItem : public MItem {
public:
  MSlayerPortalItem() { m_Number = 0; }
  virtual ~MSlayerPortalItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SLAYER_PORTAL_ITEM; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  TYPE_ITEM_NUMBER GetNumber() const { return m_Number; }
  void SetNumber(TYPE_ITEM_NUMBER n) { m_Number = n; }

  bool IsChargeItem() const { return true; }

  void SetEnchantLevel(WORD s);

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MSlayerPortalItem; }
};

//----------------------------------------------------------------------
// Vampire Portal
//----------------------------------------------------------------------
class MVampirePortalItem : public MItem {
public:
  MVampirePortalItem();
  virtual ~MVampirePortalItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_PORTAL_ITEM; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  TYPE_ITEM_NUMBER GetNumber() const { return m_Number; }
  void SetNumber(TYPE_ITEM_NUMBER n) { m_Number = n; }

  bool IsChargeItem() const { return true; }

  // 목표 zone에 대한 정보
  bool IsMarked() const;
  void SetZone(int zoneID, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
  int GetZoneID() const { return m_ZoneID; }
  TYPE_SECTORPOSITION GetZoneX() const { return m_ZoneX; }
  TYPE_SECTORPOSITION GetZoneY() const { return m_ZoneY; }

  // zone정보 설정한다. - -;;
  void SetSilver(int s);
  void SetCurrentDurability(TYPE_ITEM_DURATION d);
  void SetEnchantLevel(WORD s);

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MVampirePortalItem; }

  virtual bool IsRepairableItem() const { return false; }

protected:
  int m_ZoneID;
  TYPE_SECTORPOSITION m_ZoneX;
  TYPE_SECTORPOSITION m_ZoneY;
};

//----------------------------------------------------------------------
// Event GiftBox
//----------------------------------------------------------------------
class MEventGiftBoxItem : public MItem {
public:
  MEventGiftBoxItem() {}
  virtual ~MEventGiftBoxItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_EVENT_GIFT_BOX; }

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  virtual bool IsRepairableItem() const { return false; }

  static MItem *NewItem() { return new MEventGiftBoxItem; }
};

//----------------------------------------------------------------------
// Event Star
//----------------------------------------------------------------------
class MEventStarItem : public MPileItem {
public:
  MEventStarItem() {}
  virtual ~MEventStarItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_EVENT_STAR; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MEventStarItem; }

  virtual bool IsRepairableItem() const { return false; }
  virtual bool IsDetachableItem() const { return true; }
};

//----------------------------------------------------------------------
// Vampire Weapon
//----------------------------------------------------------------------
class MVampireWeapon : public MWeaponItem {
public:
  MVampireWeapon() {}
  ~MVampireWeapon() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_WEAPON; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  bool IsGearSlotVampireWeapon() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }

  bool IsMeleeWeapon() const { return true; }
  bool IsMagicWeapon() const { return true; }

  static MItem *NewItem() { return new MVampireWeapon; }
};

//----------------------------------------------------------------------
// VAMPIRE_Amulet
//----------------------------------------------------------------------
class MVampireAmulet : public MAccessoryItem {
public:
  MVampireAmulet() {}
  ~MVampireAmulet() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_AMULET; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_AMULET; }
  bool IsGearSlotVampireAmulet() const { return true; }
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  static MItem *NewItem() { return new MVampireAmulet; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// QuestItem
//----------------------------------------------------------------------
class MQuestItem : public MItem {
public:
  MQuestItem() {}
  virtual ~MQuestItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_QUEST_ITEM; }
  virtual bool IsRepairableItem() const { return false; }
  static MItem *NewItem() { return new MQuestItem; }
};

//----------------------------------------------------------------------
// Event Tree
//----------------------------------------------------------------------
#if __CONTENTS(__EVENTTREE_PILE)
class MEventTreeItem : public MPileItem {
#else
class MEventTreeItem : public MItem {
#endif //__EVENTTREE_PILE
public:
  MEventTreeItem() {}
  virtual ~MEventTreeItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_EVENT_TREE; }
#if __CONTENTS(__EVENTTREE_PILE)
  TYPE_ITEM_NUMBER GetMaxNumber() const;
#endif //__EVENTTREE_PILE
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MEventTreeItem; }

  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// Event ETC
//----------------------------------------------------------------------
class MEventEtcItem : public MPileItem {
public:
  MEventEtcItem() {}
  virtual ~MEventEtcItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_EVENT_ETC; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
  void UseQuickItem();
  bool IsQuickItem() const {
    return (GetItemType() >= 14 && GetItemType() <= 17) ? true : false;
  }

  static MItem *NewItem() { return new MEventEtcItem; }

  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// BloodBible
//----------------------------------------------------------------------
class MBloodBible : public MItem {
public:
  MBloodBible() {}
  ~MBloodBible() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BLOOD_BIBLE; }

  static MItem *NewItem() { return new MBloodBible; }
};

//----------------------------------------------------------------------
// CastleSymbol
//----------------------------------------------------------------------
class MCastleSymbol : public MItem {
public:
  MCastleSymbol() {}
  ~MCastleSymbol() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CASTLE_SYMBOL; }

  static MItem *NewItem() { return new MCastleSymbol; }
};

//----------------------------------------------------------------------
// CoupleRing
//----------------------------------------------------------------------
class MCoupleRing : public MRing {
public:
  MCoupleRing() {}
  ~MCoupleRing() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_COUPLE_RING; }

  // use item
  void UseGear();
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템

  static MItem *NewItem() { return new MCoupleRing; }

  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// VampireCoupleRing
//----------------------------------------------------------------------
class MVampireCoupleRing : public MVampireRing {
public:
  MVampireCoupleRing() {}
  ~MVampireCoupleRing() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIRE_COUPLE_RING; }

  // use item
  void UseGear();
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  static MItem *NewItem() { return new MVampireCoupleRing; }

  virtual bool IsRepairableItem() const { return false; }
};

class MEventItem : public MItem {
public:
  MEventItem() {}
  ~MEventItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_EVENT_ITEM; }
  virtual bool IsRepairableItem() const { return false; }

  static MItem *NewItem() { return new MEventItem; }
};

class MDyePotionItem : public MItem {
public:
  MDyePotionItem() {}
  ~MDyePotionItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_DYE_POTION; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
  //	void		UseQuickItem();
  //	bool		IsQuickItem() const { return true; }

  static MItem *NewItem() { return new MDyePotionItem; }
};

class MResurrectItem : public MPileItem {
public:
  MResurrectItem() {}
  ~MResurrectItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_RESURRECT_ITEM; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  static MItem *NewItem() { return new MResurrectItem; }
};

class MMixingItem : public MPileItem {
public:
  MMixingItem() {}
  ~MMixingItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MIXING_ITEM; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MMixingItem; }

  virtual bool IsDetachableItem() const { return true; }
};

//----------------------------------------------------------------------
// Ousters ArmsBand
//----------------------------------------------------------------------
class MOustersArmsBand : public MArmorItem2, public MSlotItemManager {
public:
  MOustersArmsBand() {}
  ~MOustersArmsBand() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_ARMSBAND; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_ARMSBAND; }

  bool IsGearSlotOustersArmsBand() const { return true; }
  bool IsContainerItem() const { return true; }

  void SetItemType(TYPE_ITEMTYPE type);

  //------------------------------------------------
  //
  //				SlotItemManager
  //
  //------------------------------------------------
  // AddItem : slot(n)에 pItem을 추가한다.
  //------------------------------------------------
  bool AddItem(MItem *pItem);
  bool AddItem(MItem *pItem, BYTE n);

  //------------------------------------------------
  // ReplaceItem : pItem을 추가하고 딴게 있다면 Item교환
  //------------------------------------------------
  bool ReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem);
  //------------------------------------------------
  // Can ReplaceItem : n slot에 pItem을 추가가능한가?
  // pOldItem에는 이전에 있던 것..
  //------------------------------------------------
  bool CanReplaceItem(MItem *pItem, BYTE n, MItem *&pOldItem);

  //------------------------------------------------
  // Find Slot To Add Item
  //------------------------------------------------
  bool FindSlotToAddItem(MItem *pItem, int &slot) const;
  // 값
  virtual int GetPocketNumber() const {
    return (*g_pItemTable)[ITEM_CLASS_OUSTERS_ARMSBAND][m_ItemType].Value3;
  } // 포켓수

  static MItem *NewItem() { return new MOustersArmsBand; }
};

//----------------------------------------------------------------------
// Ousters Boots
//----------------------------------------------------------------------
class MOustersBoots : public MArmorItem {
public:
  MOustersBoots() {}
  ~MOustersBoots() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_BOOTS; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_BOOTS; }
  bool IsGearSlotOustersBoots() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_TROUSER; }

  static MItem *NewItem() { return new MOustersBoots; }
};

//----------------------------------------------------------------------
// Ousters Chakram
//----------------------------------------------------------------------
class MOustersChakram : public MWeaponItem {
public:
  MOustersChakram() {}
  ~MOustersChakram() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_CHAKRAM; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  bool IsGearSlotOustersWeapon() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }
  bool IsMeleeWeapon() const { return true; }

  static MItem *NewItem() { return new MOustersChakram; }
};

//----------------------------------------------------------------------
// Ousters Circlet
//----------------------------------------------------------------------
class MOustersCirclet : public MArmorItem2 {
public:
  MOustersCirclet() {}
  ~MOustersCirclet() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_CIRCLET; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_CIRCLET; }
  bool IsGearSlotOustersCirclet() const { return true; }

  static MItem *NewItem() { return new MOustersCirclet; }
};

//----------------------------------------------------------------------
// Ousters Coat
//----------------------------------------------------------------------
class MOustersCoat : public MArmorItem {
public:
  MOustersCoat() {}
  ~MOustersCoat() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_COAT; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_COAT; }
  bool IsGearSlotOustersCoat() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_COAT; }

  static MItem *NewItem() { return new MOustersCoat; }
};

//----------------------------------------------------------------------
// Ousters Pendent
//----------------------------------------------------------------------
class MOustersPendent : public MAccessoryItem {
public:
  MOustersPendent() {}
  ~MOustersPendent() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_PENDENT; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_PENDENT; }
  bool IsGearSlotOustersPendent() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MOustersPendent; }
};

//----------------------------------------------------------------------
// Ousters Ring
//----------------------------------------------------------------------
class MOustersRing : public MAccessoryItem {
public:
  MOustersRing() {}
  ~MOustersRing() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_RING; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_RING; }
  bool IsGearSlotOustersRing() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MOustersRing; }
};

//----------------------------------------------------------------------
// Ousters Stone
//----------------------------------------------------------------------
class MOustersStone : public MAccessoryItem {
public:
  MOustersStone() {}
  ~MOustersStone() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_STONE; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_STONE; }
  bool IsGearSlotOustersStone() const { return true; }

#if __CONTENTS(__ADD_MAGICPROTECTION)
  bool HasMagicProtection() const { return true; }
#endif
  static MItem *NewItem() { return new MOustersStone; }
};

//----------------------------------------------------------------------
// Ousters Wristlet
//----------------------------------------------------------------------
class MOustersWristlet : public MWeaponItem {
public:
  MOustersWristlet() {}
  ~MOustersWristlet() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_WRISTLET; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_TWOHAND; }
  bool IsGearSlotTwoHand() const { return true; }

  bool IsGearSlotOustersWristlet() const { return true; }

  bool IsAddonItem() const { return true; }
  ADDON GetAddonSlot() const { return ADDON_RIGHTHAND; }
  bool IsMagicWeapon() const { return true; }

  static MItem *NewItem() { return new MOustersWristlet; }
};

//----------------------------------------------------------------------
// Ousters Larva
//----------------------------------------------------------------------
class MOustersLarva : public MPileItem {
public:
  MOustersLarva() {}
  ~MOustersLarva() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_LARVA; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;
  static MItem *NewItem() { return new MOustersLarva; }
};

//----------------------------------------------------------------------
// Ousters Pupa
//----------------------------------------------------------------------
class MOustersPupa : public MUsePotionItem {
public:
  MOustersPupa() {}
  ~MOustersPupa() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_PUPA; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // heal 되는 수치
  int GetHealPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_PUPA][m_ItemType].Value1;
  } // HP수치
  // mana 되는 수치
  int GetManaPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_PUPA][m_ItemType].Value2;
  } // MP수치

  bool IsQuickItem() const { return true; }

  // use item
  void UseQuickItem();

  static MItem *NewItem() { return new MOustersPupa; }
};

//----------------------------------------------------------------------
// Ousters ComposMei
//----------------------------------------------------------------------
class MOustersComposMei : public MUsePotionItem {
public:
  MOustersComposMei() {}
  ~MOustersComposMei() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_COMPOS_MEI; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // heal 되는 수치
  int GetHealPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_COMPOS_MEI][m_ItemType].Value1;
  } // HP수치

  // mana 되는 수치
  int GetManaPoint() const {
    return (*g_pItemTable)[ITEM_CLASS_COMPOS_MEI][m_ItemType].Value2;
  } // MP수치

  bool IsQuickItem() const { return true; }

  // use item
  void UseQuickItem();

  static MItem *NewItem() { return new MOustersComposMei; }

  //	void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
};

//----------------------------------------------------------------------
// Ousters SummonGem
//----------------------------------------------------------------------
class MOustersSummonGem : public MItem {
public:
  MOustersSummonGem() {}
  ~MOustersSummonGem() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERS_SUMMON_ITEM; }

  static MItem *NewItem() { return new MOustersSummonGem; }

  // 쌓이는 최대 개수
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  TYPE_ITEM_NUMBER GetNumber() const { return m_Number; }
  void SetNumber(TYPE_ITEM_NUMBER n) { m_Number = n; }

  void SetEnchantLevel(WORD s);

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  bool IsChargeItem() const { return true; }
};

class MEffectItem : public MPileItem {
public:
  MEffectItem() {}
  ~MEffectItem() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_EFFECT_ITEM; }

  static MItem *NewItem() { return new MEffectItem; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  TYPE_ITEM_NUMBER GetMaxNumber() const;
};

class MCodeSheetItem : public MItem {
public:
  MCodeSheetItem() {}
  ~MCodeSheetItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CODE_SHEET; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
  virtual bool IsRepairableItem() const { return false; }

  static MItem *NewItem() { return new MCodeSheetItem; }
};

class MMoonCardItem : public MPileItem {
public:
  MMoonCardItem() {}
  ~MMoonCardItem() {}

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MOON_CARD; }
  static MItem *NewItem() { return new MMoonCardItem; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  virtual bool IsDetachableItem() const { return true; }
};

class MSweeperItem : public MItem {
public:
  MSweeperItem() {}
  ~MSweeperItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SWEEPER; }
  static MItem *NewItem() { return new MSweeperItem; }
};

class MPetItem : public MItem {
private:
  DWORD m_UpdateTime;
  DWORD m_PetExpRemain;
  // 2004, 5, 11 sobeit add start
  DWORD m_PetKeepedDay; // 보관날짜
  // 2004, 5, 11 sobeit add end
  WORD m_PetFoodType;
  WORD m_PetCreatureType; // GCPetInfo PetCreatureType → creature.inf satırı
  bool m_bCanGamble;
  bool m_bCutHead;
  bool m_bCanAttack;
  WORD m_EnchantSkillType;
#if __CONTENTS(__FUNCTION_PETFOOD)
  BYTE m_Function_Petfood_Type;
#endif //__FUNCTION_PETFOOD

public:
  MPetItem();
  ~MPetItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_PET_ITEM; }

  // pet info
  void SetPetExpRemain(DWORD exp) { m_PetExpRemain = exp; }
  const DWORD GetPetExpRemain() const { return m_PetExpRemain; }
  void SetPetFoodType(WORD type) { m_PetFoodType = type; }
  const DWORD GetPetFoodType() const { return m_PetFoodType; }
  void SetPetCreatureType(WORD creatureType) { m_PetCreatureType = creatureType; }
  WORD GetPetCreatureType() const { return m_PetCreatureType; }
  void SetPetGamble(bool bGamble) { m_bCanGamble = bGamble; }
  const bool IsCanGamble() const { return m_bCanGamble; }
  void SetPetCutHead(bool bCutHead) { m_bCutHead = bCutHead; }
  const bool IsCanCutHead() const { return m_bCutHead; }
  void SetPetAttack(bool bAttack) { m_bCanAttack = bAttack; }
  const bool IsCanAttack() const { return m_bCanAttack; }
  const DWORD GetUpdateTime() const { return m_UpdateTime; }
  void SetUpdateTime(DWORD updateTime) { m_UpdateTime = updateTime; }
  void SetCurrentDurability(TYPE_ITEM_DURATION d) {
    MItem::SetCurrentDurability(d);
    SetUpdateTime(timeGetTime());
  }

  const WORD GetEnchantSkillType() const { return m_EnchantSkillType; }
  void SetEnchantSkillType(WORD wSkill) { m_EnchantSkillType = wSkill; }
  virtual bool IsRepairableItem() const { return false; }
  // 2004, 5, 11 sobeit add start
  void SetPetKeepedDay(DWORD day) { m_PetKeepedDay = day; }
  const DWORD GetPetKeepedDay() const { return m_PetKeepedDay; }
  // 2004, 5, 11 sobeit add end
  std::string GetPetName();
  std::string GetPetOptionName();
  std::string GetPetOptionEName();

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
#if __CONTENTS(__FUNCTION_PETFOOD)
  void setFunctionPetfoodType(BYTE function_Petfood_Type) {
    m_Function_Petfood_Type = function_Petfood_Type;
  }
  BYTE getFunctionPetfoodType() const { return m_Function_Petfood_Type; }
#endif //__FUNCTION_PETFOOD

  static MItem *NewItem() { return new MPetItem; }
};

class MPetFood : public MPileItem {
public:
  MPetFood() {}
  ~MPetFood() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_PET_FOOD; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MPetFood; }

  virtual bool IsDetachableItem() const { return true; }
};

class MPetEnchantItem : public MPileItem {
public:
  MPetEnchantItem() {}
  ~MPetEnchantItem() {}

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_PET_ENCHANT_ITEM; }

  static MItem *NewItem() { return new MPetEnchantItem; }

  virtual bool IsRepairableItem() const { return false; }
  virtual bool IsDetachableItem() const { return true; }
};

class MLuckyBag : public MPileItem {
public:
  MLuckyBag() {}
  ~MLuckyBag() {}

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_LUCKY_BAG; }

  static MItem *NewItem() { return new MLuckyBag; }
};

#if __CONTENTS(__GLOBAL_NPC)
class MRankGem : public MItem {
public:
  MRankGem() {}
  ~MRankGem() {}

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_RANK_GEM; }

  int GetRankGemOptionClass() const {
    return (*g_pItemTable)[ITEM_CLASS_RANK_GEM][m_ItemType].Value1;
  } // 괜柯橄昑잚깎

  static MItem *NewItem() { return new MRankGem; }
};
#endif //__GLOBAL_NPC

class MSms_item : public MItem {
public:
  MSms_item() {}
  ~MSms_item() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SMS_ITEM; }

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MSms_item; }
};

class MCoreZap : public MAccessoryItem {
public:
  MCoreZap() {}
  ~MCoreZap() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CORE_ZAP; }

  static MItem *NewItem() { return new MCoreZap; }
  bool IsGearSlotCoreZap() const { return true; }
  bool IsGearSlotVampireCoreZap() const { return true; }
  bool IsGearSlotOustersCoreZap() const { return true; }
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsCanAddThirdOptionItem() const { return false; }

  int GetItemNameColorSet() const;

  virtual bool IsRepairableItem() const { return false; }
};

class MGQuestItem : public MItem {
public:
  MGQuestItem() {}
  ~MGQuestItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_GQUEST_ITEM; }
  static MItem *NewItem() { return new MGQuestItem; }
};

class MTrapItem : public MItem {
public:
  MTrapItem() {}
  ~MTrapItem() {}

  // use item
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_TRAP_ITEM; }
  static MItem *NewItem() { return new MTrapItem; }
};

class MBloodBibleSign : public MGearItem {
public:
  MBloodBibleSign() {}
  ~MBloodBibleSign() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_BLOOD_BIBLE_SIGN; }

  static MItem *NewItem() { return new MBloodBibleSign; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_BLOOD_BIBLE; }

  bool IsGearSlotBloodBible() const { return true; }
  bool IsGearSlotVampireBloodBible() const { return true; }
  bool IsGearSlotOustersBloodBible() const { return true; }

  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템

  virtual bool IsRepairableItem() const { return false; }
};

class MWarItem : public MItem {
public:
  MWarItem() {}
  ~MWarItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_WAR_ITEM; }
  static MItem *NewItem() { return new MWarItem; }
};

// By csm 2004.12.27 전직 아이템 추가

class MCarryingReceiver : public MAccessoryItem {
public:
  MCarryingReceiver() {}
  ~MCarryingReceiver() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CARRYING_RECEIVER; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_PDA; }
  //
  static MItem *NewItem() { return new MCarryingReceiver; }

  bool IsGearSlotPDA() const { return true; } // PDA 슬롯
  bool IsAdvancementClassItem() const { return true; }
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템

  virtual bool IsRepairableItem() const { return false; }
};

class MShoulderArmor : public MAccessoryItem {
public:
  MShoulderArmor() {}
  ~MShoulderArmor() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SHOULDER_ARMOR; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_SHOULDER; }

  static MItem *NewItem() { return new MShoulderArmor; }

  bool IsGearSlotShoulder() const { return true; } // 어깨 슬롯
  bool IsAdvancementClassItem() const { return true; }
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsAddonItem() const { return true; }
};
class MDermis : public MAccessoryItem {
public:
  MDermis() {}
  ~MDermis() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_DERMIS; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_DERMIS; }
  // bool		IsGearSlotVampireRing() const			{ return
  // true; }

  static MItem *NewItem() { return new MDermis; }
  bool IsGearSlotVampireDermis() const { return true; } // 문신
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsAdvancementClassItem() const { return true; }

  virtual bool IsRepairableItem() const { return false; }
};

class MPersona : public MAccessoryItem {
public:
  MPersona() {}
  ~MPersona() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_PERSONA; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_VAMPIRE_PERSONA; }
  //		bool		IsGearSlotVampireRing() const
  //{ return true; }

  static MItem *NewItem() { return new MPersona; }
  bool IsGearSlotVampirePersona() const { return true; } // 페르소나
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsAdvancementClassItem() const { return true; }
};

class MFascia : public MAccessoryItem {
public:
  MFascia() {}
  ~MFascia() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_FASCIA; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_FASCIA; }
  //		bool		IsGearSlotVampireRing() const
  //{ return true; }

  static MItem *NewItem() { return new MFascia; }
  bool IsGearSlotOustersFascia() const { return true; } // 허리장식
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsAdvancementClassItem() const { return true; }

  virtual bool IsRepairableItem() const { return false; }
};

class MMitten : public MAccessoryItem {
public:
  MMitten() {}
  ~MMitten() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_MITTEN; }

  ITEM_GEAR_SLOT GetGearSlot() const { return SLOT_OUSTERS_MITTEN; }
  //		bool		IsGearSlotCoat() const			{ return
  //true; }

  //		bool		IsAddonItem() const
  //{ return true; } 		ADDON		GetAddonSlot() const
  //{ return ADDON_COAT; }

  static MItem *NewItem() { return new MMitten; }
  bool IsGearSlotOustersMitten() const { return true; } // 장갑
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsAdvancementClassItem() const { return true; }
};

class MSubInventory : public MItem, public MGridItemManager {
public:
  MSubInventory() { /*m_bAlpha = false;*/ }
  ~MSubInventory() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SUB_INVENTORY; }

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MSubInventory; }

  void SetItemType(TYPE_ITEMTYPE type);

  bool IsContainerItem() const { return true; }

  //------------------------------------------------
  //
  //				SlotItemManager
  //
  //------------------------------------------------
  // AddItem : slot(n)에 pItem을 추가한다.
  //------------------------------------------------
  bool AddItem(MItem *pItem);
  bool AddItem(MItem *pItem, BYTE X, BYTE Y);

  //------------------------------------------------
  // ReplaceItem : pItem을 추가하고 딴게 있다면 Item교환
  //------------------------------------------------
  bool ReplaceItem(MItem *pItem, BYTE X, BYTE Y, MItem *&pOldItem);

  //------------------------------------------------
  // Can ReplaceItem : n slot에 pItem을 추가가능한가?
  // pOldItem에는 이전에 있던 것..
  //------------------------------------------------
  bool CanReplaceItem(MItem *pItem, BYTE X, BYTE Y, MItem *&pOldItem);

  void OpenAllSubInventory();

  /*
  //------------------------------------------------
  // 인터페이스 정보
  //------------------------------------------------
  void			SetInterfaceRect(const Rect& rect)	{
m_InterfaceRect = rect;	} const Rect&		GetInterfaceRect()
{ return m_InterfaceRect;	}

  void			SetAlpha(bool alpha)				{
m_bAlpha = alpha;			} bool			GetAlpha()
{ return m_bAlpha;			}

protected:
  Rect			m_InterfaceRect;
  bool			m_bAlpha;
  */
};

//----------------------------------------------------------------------
// CommonQuestItem
//----------------------------------------------------------------------
class MCommonQuestItem : public MPileItem {
public:
  MCommonQuestItem() {}
  ~MCommonQuestItem() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_COMMON_QUEST_ITEM; }

  TYPE_ITEM_NUMBER GetMaxNumber() const;

  // use item
#if __CONTENTS(__LEVEL_WAR_RENEWAL)
  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
#endif //__LEVEL_WAR_RENEWAL
  static MItem *NewItem() { return new MCommonQuestItem; }
};

//----------------------------------------------------------------------
// EtherealChain
//----------------------------------------------------------------------
class MEtherealChain : public MAccessoryItem {
public:
  MEtherealChain() {}
  ~MEtherealChain() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_ETHEREAL_CHAIN; }

  static MItem *NewItem() { return new MEtherealChain; }
  bool IsGearSlotEtherealChain() const { return true; }
  bool IsGearSlotVampireEtherealChain() const { return true; }
  bool IsGearSlotOustersEtherealChain() const { return true; }
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsCanAddThirdOptionItem() const { return false; }

  int GetItemNameColorSet() const;

  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// MOustersHarmonicPendent
//----------------------------------------------------------------------
class MOustersHarmonicPendent : public MOustersPendent {
public:
  MOustersHarmonicPendent() {}
  ~MOustersHarmonicPendent() {}

public:
  ITEM_CLASS GetItemClass() const {
    return ITEM_CLASS_OUSTERS_HARMONIC_PENDENT;
  }

  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  static MItem *NewItem() { return new MOustersHarmonicPendent; }
  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// MCheckMoney
//----------------------------------------------------------------------
class MCheckMoney : public MItem {
public:
  MCheckMoney() {}
  ~MCheckMoney() {}

public:
  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CHECK_MONEY; }
  static MItem *NewItem() { return new MCheckMoney; }
};

//----------------------------------------------------------------------
// MCueOfAdam
//----------------------------------------------------------------------
class MCueOfAdam : public MAccessoryItem {
public:
  MCueOfAdam() {}
  ~MCueOfAdam() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CUE_OF_ADAM; }

  static MItem *NewItem() { return new MCueOfAdam; }

  bool IsGearSlotCue() const { return true; }
  bool IsGearSlotVampireCue() const { return true; }
  bool IsGearSlotOustersCue() const { return true; }
  bool IsDurationAlwaysOkay() const { return true; } // 내구 체크 안하는 아이템
  bool IsCanAddThirdOptionItem() const { return false; }
  int GetItemNameColorSet() const;

  virtual bool IsRepairableItem() const { return false; }
};

//----------------------------------------------------------------------
// MContractOfBlood
//----------------------------------------------------------------------
class MContractOfBlood : public MItem {
public:
  MContractOfBlood() {}
  ~MContractOfBlood() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CONTRACT_OF_BLOOD; }

  static MItem *NewItem() { return new MContractOfBlood; }
};

//----------------------------------------------------------------------
// MSkillBook
//----------------------------------------------------------------------
class MSkillBook : public MItem {
public:
  MSkillBook() {}
  ~MSkillBook() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_SKILL_BOOK; }

  static MItem *NewItem() { return new MSkillBook; }

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
};

#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
class MVampireWingItem : public MItem {
public:
  MVampireWingItem() {}
  ~MVampireWingItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_VAMPIREWING_ITEM; }

  static MItem *NewItem() { return new MVampireWingItem; }

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
};

class MOustersWingItem : public MItem {
public:
  MOustersWingItem() {}
  ~MOustersWingItem() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_OUSTERSWING_ITEM; }

  static MItem *NewItem() { return new MOustersWingItem; }

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
};
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

#if __CONTENTS(__TUNING_ITEM)
class MTuningItemSlayer : public MItem {
public:
  MTuningItemSlayer() {}
  ~MTuningItemSlayer() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_TUNING_SLAYER; }

  static MItem *NewItem() { return new MTuningItemSlayer; }

  virtual int GetTargetClass() const {
    return (*g_pItemTable)[ITEM_CLASS_TUNING_SLAYER][m_ItemType].Value1;
  }
  virtual int GetTargetType() const {
    return (*g_pItemTable)[ITEM_CLASS_TUNING_SLAYER][m_ItemType].Value2;
  }
};

class MTuningItemVampire : public MItem {
public:
  MTuningItemVampire() {}
  ~MTuningItemVampire() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_TUNING_VAMPIRE; }

  static MItem *NewItem() { return new MTuningItemVampire; }

  virtual int GetTargetClass() const {
    return (*g_pItemTable)[ITEM_CLASS_TUNING_VAMPIRE][m_ItemType].Value1;
  }
  virtual int GetTargetType() const {
    return (*g_pItemTable)[ITEM_CLASS_TUNING_VAMPIRE][m_ItemType].Value2;
  }
};

class MTuningItemOusters : public MItem {
public:
  MTuningItemOusters() {}
  ~MTuningItemOusters() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_TUNING_OUSTERS; }

  static MItem *NewItem() { return new MTuningItemOusters; }

  virtual int GetTargetClass() const {
    return (*g_pItemTable)[ITEM_CLASS_TUNING_OUSTERS][m_ItemType].Value1;
  }
  virtual int GetTargetType() const {
    return (*g_pItemTable)[ITEM_CLASS_TUNING_OUSTERS][m_ItemType].Value2;
  }
};
#endif //__TUNING_ITEM

#if __CONTENTS(__GLOBAL_NPC)
class MCallNPCNPCCard : public MPileItem {
public:
  MCallNPCNPCCard() {}
  virtual ~MCallNPCNPCCard() {}

  ITEM_CLASS GetItemClass() const { return ITEM_CLASS_CALLNPC_CARD; }
  TYPE_ITEM_NUMBER GetMaxNumber() const;

  void UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

  static MItem *NewItem() { return new MCallNPCNPCCard; }

  virtual bool IsRepairableItem() const { return false; }
};

#endif //__GLOBAL_NPC
#endif
