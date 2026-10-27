//----------------------------------------------------------------------
// MSkillManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MSkillManager.h"
#include "crash_trace_tool.h"
#include "MTypeDef.h"
#include <new>


#ifdef __GAME_CLIENT__
#include "MHelpManager.h"
#include "MInventory.h"
#include "MPlayer.h"
#include "MSlayerGear.h"
#include "ServerInfo.h"

//	#include "MFileDef.h"
#include "DebugInfo.h"
#include "MItemFinder.h"
#include "MZone.h" // ë ˆì–´ì¡´ì—ì„œ invisibility ëª»ì“°ê²Œ í• ë ¤ê³ 
#include "Properties.h"
#include "UserInformation.h"
#include "VS_UI.h" // [ìƒˆê¸°ìˆ 3]

extern MItem *UI_GetMouseItem();
extern bool IsBombMaterial(const MItem *pItem);

extern DWORD g_CurrentTime;
#endif

#ifdef __GAME_CLIENT__
bool FileOpenBinary(const char *filename, ivfstream &file);

// MItem.cppì— ìžˆë‹¤.
bool IsBombMaterial(const MItem *pItem);

bool g_abHolyLandBonusSkills[HOLYLAND_BONUS_MAX] = {
    false,
};
bool g_abSweeperBonusSkills[SWEEPER_BONUS_MAX] = {
    false,
};

#endif

//----------------------------------------------------------------------
//
// Global
//
//----------------------------------------------------------------------
// SkillInfoTable
MSkillInfoTable *g_pSkillInfoTable = NULL;

// SkillManager
MSkillManager *g_pSkillManager = NULL;

// SkillAvailable
MSkillSet *g_pSkillAvailable = NULL;

//
//----------------------------------------------------------------------
//
//							SKILLINFO_NODE
//
//----------------------------------------------------------------------
SKILLINFO_NODE::SKILLINFO_NODE() {
  m_Level = 1;

  m_SpriteID = 0;

  m_MP = 0;
  m_bPassive = false;
  m_bActive = false;

  m_ExpLevel = 0; // ìŠ¤í‚¬ ë ˆë²¨
  m_SkillExp = 0; // ìŠ¤í‚¬ ê²½í—˜ì¹˜

  m_LearnLevel = 100;
  m_eSkillRace = RACE_SLAYER;

  m_DelayTime = 0;     // ê¸°ìˆ  ì‚¬ìš©í›„ ë‹¤ì‹œ ì‚¬ìš©ê°€ëŠ¥í•œ delay
  m_AvailableTime = 0; // ë‹¤ì‹œ ì‚¬ìš© ê°€ëŠ¥í•œ ì‹œê°„

  m_bEnable = false;

  DomainType = 0;  // ê·¸ ê¸°ìˆ ì´ ì–´ëŠ ë„ë©”ì¸ì— ì†í•˜ëŠ”ê°€.
  minDamage = 0;   // ìµœì†Œ ë°ë¯¸ì§€ ë˜ëŠ” íš¨ê³¼ì¹˜.
  maxDamage = 0;   // ìµœëŒ€ ë°ë¯¸ì§€ ë˜ëŠ” íš¨ê³¼ì¹˜.
  minDelay = 0;    // ìµœì†Œ ì‚¬ìš© ë”œë ˆì´.
  maxDelay = 0;    // ìµœëŒ€ ì‚¬ìš© ë”œë ˆì´.
  minCastTime = 0; // ìµœì†Œ ìºìŠ¤íŒ… íƒ€ìž„.
  maxCastTime = 0; // ìµœëŒ€ ìºìŠ¤íŒ… íƒ€ìž„.
  minDuration = 0; // ìµœì†Œ ì§€ì† ì‹œê°„
  maxDuration = 0; // ìµœëŒ€ ì§€ì† ì‹œê°„
  minRange = 1;    // ìµœì†Œ ì‚¬ì •ê±°ë¦¬
  maxRange = 1;    // ìµœëŒ€ ì‚¬ì •ê±°ë¦¬
  maxExp = 0;      // ê·¸ ê¸°ìˆ ì˜ 100% ê²½í—˜ì¹˜. 1 íšŒë‹¹ + 1 ì”© ì˜¬ë¼ê°
  SkillPoint = 0;
  LevelUpPoint = 0;
  Fire = 0;
  Water = 0;
  Earth = 0;
  Wind = 0;
  Sum = 0;
  Wristlet = 0;
  Stone1 = 0;
  Stone2 = 0;
  Stone3 = 0;
  Stone4 = 0;
  ElementalDomain = 0;
  CanDelete = 0;

  m_bHideInSkillTree = false;
}

//----------------------------------------------------------------------
// Load From File ServerSkillInfo
//----------------------------------------------------------------------
void SKILLINFO_NODE::LoadFromFileServerSkillInfo(ivfstream &file) {
  int ll;
  MString name;
  MString hname;
  int mp;

  file.read((char *)&ll, 4);
  file.read((char *)&DomainType, 4);
  name.LoadFromFile(file);
  hname.LoadFromFile(file);
  file.read((char *)&minDamage, 4);   // ìµœì†Œ ë°ë¯¸ì§€ ë˜ëŠ” íš¨ê³¼ì¹˜.
  file.read((char *)&maxDamage, 4);   // ìµœëŒ€ ë°ë¯¸ì§€ ë˜ëŠ” íš¨ê³¼ì¹˜.
  file.read((char *)&minDelay, 4);    // ìµœì†Œ ì‚¬ìš© ë”œë ˆì´.
  file.read((char *)&maxDelay, 4);    // ìµœëŒ€ ì‚¬ìš© ë”œë ˆì´.
  file.read((char *)&minDuration, 4); // ìµœì†Œ ìºìŠ¤íŒ… íƒ€ìž„.
  file.read((char *)&maxDuration, 4); // ìµœëŒ€ ìºìŠ¤íŒ… íƒ€ìž„.
  file.read((char *)&mp, 4);          // ë§ˆë‚˜ ì†Œëª¨ëŸ‰.(m_MP)
  file.read((char *)&minRange, 4);    // ìµœì†Œ ì‚¬ì •ê±°ë¦¬
  file.read((char *)&maxRange, 4);    // ìµœëŒ€ ì‚¬ì •ê±°ë¦¬
  file.read((char *)&maxExp, 4); // ê·¸ ê¸°ìˆ ì˜ 100% ê²½í—˜ì¹˜. 1 íšŒë‹¹ + 1 ì”© ì˜¬ë¼ê°

  if (DomainType == SKILLDOMAIN_OUSTERS) {
    file.read((char *)&SkillPoint, sizeof(int));
    file.read((char *)&LevelUpPoint, sizeof(int));
    int szSkill;
    file.read((char *)&szSkill, sizeof(int));
    SkillTypeList.clear();
    for (int i = 0; i < szSkill; i++) {
      int skillType;
      file.read((char *)&skillType, sizeof(int));
      SkillTypeList.push_back(skillType);
    }
    file.read((char *)&Fire, sizeof(int));
    file.read((char *)&Water, sizeof(int));
    file.read((char *)&Earth, sizeof(int));
    file.read((char *)&Wind, sizeof(int));
    file.read((char *)&Sum, sizeof(int));
    file.read((char *)&Wristlet, sizeof(int));
    file.read((char *)&Stone1, sizeof(int));
    file.read((char *)&Stone2, sizeof(int));
    file.read((char *)&Stone3, sizeof(int));
    file.read((char *)&Stone4, sizeof(int));
    file.read((char *)&ElementalDomain, sizeof(int));
    file.read((char *)&CanDelete, sizeof(BYTE));
  } else {
    ElementalDomain = -1;
  }
  if (name != "Empty Skill") {
    m_LearnLevel = ll;
    m_Name = name;
    m_HName = hname;
    m_MP = mp;
  }
}

//----------------------------------------------------------------------
// Add NextSkill
//----------------------------------------------------------------------
// ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” Skillë“¤ì„ ì„¤ì •í•œë‹¤.
//----------------------------------------------------------------------
bool SKILLINFO_NODE::AddNextSkill(ACTIONINFO id) {
  SKILLID_LIST::iterator iSkill = m_listNextSkill.begin();

  // sortí•´ì„œ addí•œë‹¤.
  while (iSkill != m_listNextSkill.end()) {
    // ì´ë¯¸ ìžˆìœ¼ë©´ ì¶”ê°€ ë¶ˆê°€
    if (*iSkill == id) {
      return false;
    }
    // í°ê±° ì•žì—..
    else if (*iSkill > id) {
      // ì•žì— ì¶”ê°€í•œë‹¤.
      m_listNextSkill.insert(iSkill, id);

      return true;
    }

    iSkill++;
  }

  m_listNextSkill.push_back(id);

  return true;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void SKILLINFO_NODE::SaveToFile(std::ofstream &file) {

  m_Name.SaveToFile(file); // ê¸°ìˆ  ì´ë¦„
  m_HName.SaveToFile(file);
  file.write((const char *)&m_Level, 4);
  file.write((const char *)&m_X, 4);
  file.write((const char *)&m_Y, 4); // í™”ë©´ì—ì„œì˜ ì¶œë ¥ ì‹œìž‘ ìœ„ì¹˜
  file.write((const char *)&m_SpriteID, SIZE_SPRITEID); // ê¸°ìˆ ì˜ Icon Sprite
  file.write((const char *)&m_MP, 4);                   // MPì†Œë¹„ëŸ‰
  file.write((const char *)&m_bPassive, 1);             // passive skillì¸ê°€?
  file.write((const char *)&m_YMP, 4);                  // MPì†Œë¹„ëŸ‰
  file.write((const char *)&m_ZMP, 4);                  // MPì†Œë¹„ëŸ‰
  file.write((const char *)&m_bActive, 1); // í•­ìƒ ì‚¬ìš© ê°€ëŠ¥í•œ skillì¸ê°€?

  BYTE skillStep = m_SkillStep;
  file.write((const char *)&skillStep, 1);

  // id list ì €ìž¥
  int idNum;
  SKILLID_LIST::const_iterator iSkillID;

  idNum = m_listNextSkill.size();
  file.write((const char *)&idNum, 4);
  iSkillID = m_listNextSkill.begin();

  while (iSkillID != m_listNextSkill.end()) {
    TYPE_ACTIONINFO id = *iSkillID;
    file.write((const char *)&id, SIZE_ACTIONINFO);
    iSkillID++;
  }

  // Delay Group ì €ìž¥
  idNum = m_listDelayGroup.size();
  file.write((const char *)&idNum, 4);
  iSkillID = m_listDelayGroup.begin();

  while (iSkillID != m_listDelayGroup.end()) {
    TYPE_ACTIONINFO id = *iSkillID;
    file.write((const char *)&id, SIZE_ACTIONINFO);
    iSkillID++;
  }

  m_Description.SaveToFile(file);

  file.write((const char *)&m_bHideInSkillTree,
             1); // ìŠ¤í‚¬ íŠ¸ë¦¬ì—ì„œ ìˆ¨ê²¨ì•¼í•˜ëŠ” ìŠ¤í‚¬
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
int g_nskillnodeindex = 0;

void SKILLINFO_NODE::LoadFromFile(ivfstream &file) {
  m_index = g_nskillnodeindex;
  g_nskillnodeindex++;
  m_Name.LoadFromFile(file); // ê¸°ìˆ  ì´ë¦„
  m_HName.LoadFromFile(file);
  file.read((char *)&m_Level, 4);
  file.read((char *)&m_X, 4);
  file.read((char *)&m_Y, 4);                    // í™”ë©´ì—ì„œì˜ ì¶œë ¥ ì‹œìž‘ ìœ„ì¹˜
  file.read((char *)&m_SpriteID, SIZE_SPRITEID); // ê¸°ìˆ ì˜ Icon Sprite
  file.read((char *)&m_MP, 4);                   // MPì†Œë¹„ëŸ‰
  file.read((char *)&m_bPassive, 1);             // passive ìŠ¤í‚¬?
  file.read((char *)&m_YMP, 4);                  // MPì†Œë¹„ëŸ‰
  file.read((char *)&m_ZMP, 4);                  // MPì†Œë¹„ëŸ‰
  file.read((char *)&m_bActive, 1);              // í•­ìƒ ì‚¬ìš© ê°€ëŠ¥í•œ skillì¸ê°€?

  BYTE skillStep;
  file.read((char *)&skillStep, 1);
  m_SkillStep = (SKILL_STEP)skillStep;
  // id list load
  int idNum;
  file.read((char *)&idNum, 4);
  if (idNum < 0 || idNum > 10000) idNum = 0;

  m_listNextSkill.clear();
  for (int i = 0; i < idNum; i++) {
    TYPE_ACTIONINFO id;
    file.read((char *)&id, SIZE_ACTIONINFO);
    m_listNextSkill.push_back((ACTIONINFO)id);
  }

  // ë°°ìš´ level
  m_ExpLevel = 0;

  // Delay Group
  file.read((char *)&idNum, 4);
  if (idNum < 0 || idNum > 10000) idNum = 0;
  m_listDelayGroup.clear();
  for (int i = 0; i < idNum; i++) {
    TYPE_ACTIONINFO id;
    file.read((char *)&id, SIZE_ACTIONINFO);
    m_listDelayGroup.push_back((ACTIONINFO)id);
  }

  m_Description.LoadFromFile(file);

  file.read((char *)&m_bHideInSkillTree, 1); // ìŠ¤í‚¬ íŠ¸ë¦¬ì—ì„œ ìˆ¨ê²¨ì•¼í•˜ëŠ” ìŠ¤í‚¬
}

//----------------------------------------------------------------------
// Set DelayTime ( delay )
//----------------------------------------------------------------------
// ê¸°ìˆ  ì‚¬ìš©í›„ ë‹¤ì‹œ ì‚¬ìš©í•  ìˆ˜ ìžˆëŠ” delayì‹œê°„ ì„¤ì •
//----------------------------------------------------------------------
void SKILLINFO_NODE::SetDelayTime(DWORD delay) {
  // 3ì´ˆ ì´í•˜ ê¸°ìˆ ì€ delayê°€ ì—†ëŠ” ê±¸ë¡œ í‘œì‹œí•œë‹¤.
  if (delay < 1800) {
    delay = 0;
  }

  m_DelayTime = delay;
}

//----------------------------------------------------------------------
// Is AvailableTime ?
//----------------------------------------------------------------------
// ì§€ê¸ˆ ì‚¬ìš© ê°€ëŠ¥í•œê°€?
//----------------------------------------------------------------------
bool SKILLINFO_NODE::IsAvailableTime() const {
#if defined(__GAME_CLIENT__)
  return g_CurrentTime >= m_AvailableTime;
#else
  return true;
#endif
}

//----------------------------------------------------------------------
// Get AvailableTimeLeft
//----------------------------------------------------------------------
// ë‚¨ì€ ì‚¬ìš© ê°€ëŠ¥ ì‹œê°„
//----------------------------------------------------------------------
DWORD
SKILLINFO_NODE::GetAvailableTimeLeft() const {
#if defined(__GAME_CLIENT__)
  int timeGap = (int)m_AvailableTime - (int)g_CurrentTime;

  if (timeGap > 0) {
    return timeGap;
  }
#endif

  return 0;
}

//----------------------------------------------------------------------
// Set AvailableTime
//----------------------------------------------------------------------
// ì§€ê¸ˆ ë°”ë¡œ ì‚¬ìš© ê°€ëŠ¥í•˜ê²Œ ì„¤ì •í•œë‹¤.
//----------------------------------------------------------------------
void SKILLINFO_NODE::SetAvailableTime(int delay) {
// ê¹”ë”í•˜ê²Œ 0ìœ¼ë¡œ.. - -;
#ifdef __GAME_CLIENT__
  if (delay == 0) {
    m_CurrentDelayTime = 0;
    m_AvailableTime = 0; // g_CurrentTime;
  } else {
    m_CurrentDelayTime = delay;
    m_AvailableTime = g_CurrentTime + delay; // g_CurrentTime;
  }
#endif
}

//----------------------------------------------------------------------
// Set Next AvailableTime
//----------------------------------------------------------------------
// ë‹¤ìŒ ì‚¬ìš© ê°€ëŠ¥í•œ ì‹œê°„ì„ ê²°ì •í•œë‹¤.
//----------------------------------------------------------------------
void SKILLINFO_NODE::SetNextAvailableTime() {
#ifdef __GAME_CLIENT__
  // í˜„ìž¬ì‹œê°„ì—ì„œ delayTimeí›„ì—ëŠ” ì‚¬ìš© ê°€ëŠ¥í•˜ë‹¤.
  m_CurrentDelayTime = m_DelayTime;
  m_AvailableTime = g_CurrentTime + m_DelayTime;
  //		m_AvailableTime = g_CurrentTime + m_DelayTime<1800? 0 :
  // m_DelayTime;
#endif
}

//----------------------------------------------------------------------
// Set Enable
//----------------------------------------------------------------------
void SKILLINFO_NODE::SetEnable(bool enable) { m_bEnable = enable; }

//----------------------------------------------------------------------
// Set Disable
//----------------------------------------------------------------------
/*
void
SKILLINFO_NODE::SetDisable()
{
        m_bEnable = false;
}
*/

//----------------------------------------------------------------------
//
//							MSkillSet
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Add Skill
//----------------------------------------------------------------------
bool MSkillSet::AddSkill(ACTIONINFO id, BYTE flag) {
  SKILLID_MAP::iterator iSkill;

  iSkill = find(id);

  //-----------------------------------------------
  // ì•„ì§ ì—†ëŠ” Skillì´ë©´ ì¶”ê°€
  //-----------------------------------------------
  if (iSkill == end()) {
    insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));

    return true;
  }

  //-----------------------------------------------
  // ì´ë¯¸ ìžˆë‹¤ë©´ flagë§Œ ë°”ê¾¼ë‹¤.
  //-----------------------------------------------
  SKILLID_NODE &node = (*iSkill).second;

  node.Flag = flag;

  return false;
}

//----------------------------------------------------------------------
// Remove Skill
//----------------------------------------------------------------------
// SkillID ì œê±°
//----------------------------------------------------------------------
bool MSkillSet::RemoveSkill(ACTIONINFO id) {
  SKILLID_MAP::iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  iSkill = find(id);

  //--------------------------------------------------
  // ê·¸ëŸ° idë¥¼ ê°€ì§„ Skillì´ ì—†ëŠ” ê²½ìš°
  //--------------------------------------------------
  if (iSkill == end()) {
    return false;
  }

  //--------------------------------------------------
  // ì°¾ì€ ê²½ìš° --> ì œê±°
  //--------------------------------------------------
  // mapì—ì„œ ì œê±°
  erase(iSkill);

  return true;
}

//----------------------------------------------------------------------
// Is Enable Skill?
//----------------------------------------------------------------------
// idì˜ skillì´ ì‚¬ìš© ê°€ëŠ¥í•œê°€?
//----------------------------------------------------------------------
bool MSkillSet::IsEnableSkill(ACTIONINFO id) const {
#ifdef __GAME_CLIENT__
  // í”Œë ˆì´ì–´ê°€ SILENCE ìŠ¤í‚¬ì´ ê±¸ë ¤ìžˆëŠ” ê²½ìš° ìŠ¤í‚¬ì„ ì‚¬ìš©í•  ìˆ˜ ì—†ë‹¤.
  if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SILENCE))
    return false;

  // í”¼ì˜ ì €ì£¼ê°€ ê±¸ë ¤ìžˆì§€ ì•Šìœ¼ë©´ í”¼ì˜ í–¥ì—°ì„ ì“¸ìˆ˜ ì—†ë‹¤.
  if (id == SKILL_BLOODS_SYMPOSION_ATTACK &&
      (!g_pPlayer->HasEffectStatus(EFFECTSTATUS_BLOOD_CURSE) ||
       !g_pPlayer->IsVampire()))
    return false;
#endif

  SKILLID_MAP::const_iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.x
  //--------------------------------------------------
  iSkill = find(id);

  //--------------------------------------------------
  // ê·¸ëŸ° idë¥¼ ê°€ì§„ Skillì´ ì—†ëŠ” ê²½ìš°
  //--------------------------------------------------
  if (iSkill == end()) {
    return false;
  }

  //--------------------------------------------------
  // ì°¾ì€ ê²½ìš° --> Is enable?
  //--------------------------------------------------

  return ((*iSkill).second).IsEnable() != 0;
}

//----------------------------------------------------------------------
// Enable Skill
//----------------------------------------------------------------------
bool MSkillSet::EnableSkill(ACTIONINFO id) {
  SKILLID_MAP::iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  iSkill = find(id);

  //--------------------------------------------------
  // ê·¸ëŸ° idë¥¼ ê°€ì§„ Skillì´ ì—†ëŠ” ê²½ìš°
  //--------------------------------------------------
  if (iSkill == end()) {
    return false;
  }

  //--------------------------------------------------
  // ì°¾ì€ ê²½ìš° --> enable
  //--------------------------------------------------
  // mapì—ì„œ ì œê±°
  ((*iSkill).second).SetEnable();

  return true;
}

//----------------------------------------------------------------------
// Disable Skill
//----------------------------------------------------------------------
bool MSkillSet::DisableSkill(ACTIONINFO id) {
  SKILLID_MAP::iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  iSkill = find(id);

  //--------------------------------------------------
  // ê·¸ëŸ° idë¥¼ ê°€ì§„ Skillì´ ì—†ëŠ” ê²½ìš°
  //--------------------------------------------------
  if (iSkill == end()) {
    return false;
  }

  //--------------------------------------------------
  // ì°¾ì€ ê²½ìš° --> enable
  //--------------------------------------------------
  // mapì—ì„œ ì œê±°
  ((*iSkill).second).SetDisable();

  return true;
}

//----------------------------------------------------------------------
// Set Avaliable Skills
//----------------------------------------------------------------------
// í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ ëª¨ë“  skillë“¤ì„ ì°¾ì•„ì„œ ì¶”ê°€í•œë‹¤.
//
// - í˜„ìž¬ ë“¤ê³  ìžˆëŠ” ë¬´ê¸°ë¥¼ ë³´ê³ 
//   SkillTreeì—ì„œ ì ì ˆí•œ domainì„ ëª¨ë‘ enable / ë‚˜ë¨¸ì§€ëŠ” disable
// - inventoryì—ì„œ skillì— ê´€ë ¨ëœ ê¸°ìˆ ì„ ì°¾ëŠ”ë‹¤.
// - ê¸°íƒ€.. skill ?
//----------------------------------------------------------------------
void MSkillSet::SetAvailableSkills() {
#ifdef __GAME_CLIENT__

  if (g_pPlayer == NULL || g_pSkillManager == NULL ||
      g_pSkillManager->GetSize() == 0 || g_pSkillInfoTable == NULL ||
      g_pSkillInfoTable->GetSize() == 0) {
    return;
  }

  //--------------------------------------------------
  // playerì˜ í˜„ìž¬ MP
  //--------------------------------------------------
  int playerMP;
  BYTE flag;

  if (g_pPlayer->GetRace() != RACE_VAMPIRE) {
    playerMP = g_pPlayer->GetMP();

    // EFFECTSTATUS_SACRIFICE ì‚¬ìš©ì¤‘ì´ë©´ HP 1ì´ MP 2ê°€ ëœë‹¤.
    if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SACRIFICE)) {
      playerMP += (g_pPlayer->GetHP() << 1);
    }
  } else {
    // vampireì¸ ê²½ìš°ëŠ” HPë¥¼ MPëŒ€ì‹ ì— ì“´ë‹¤.
    playerMP = g_pPlayer->GetHP();
  }

  // ëª¨ë“  skillë“¤ì„ ì§€ìš´ë‹¤.
  clear();

  if (g_pZone != NULL && g_pZone->GetID() == 3001)
    return;

  //-----------------------------------------------------
  //
  //					slayerì¸ ê²½ìš°
  //
  //-----------------------------------------------------
  switch (g_pPlayer->GetRace()) {
  case RACE_SLAYER: {
    if (g_pSlayerGear == NULL || g_pInventory == NULL) {
      return;
    }
    // 2004, 9, 16, sobeit add start - ì¸ìŠ¤í†¨ í„°ë ›ì¼ë•Œ ìŠ¤í‚¬ ì •ë³´ ê°±ì‹ 
    if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)) {
      // insert(SKILLID_MAP::value_type( SKILL_CLINT_ADV2_HALCYON_MK1,
      // SKILLID_NODE(SKILL_CLINT_ADV2_HALCYON_MK1, FLAG_SKILL_ENABLE) ));
      insert(SKILLID_MAP::value_type(
          MAGIC_UN_TRANSFORM,
          SKILLID_NODE(MAGIC_UN_TRANSFORM, FLAG_SKILL_ENABLE)));
      if ((*g_pSkillInfoTable)[SKILL_TURRET_FIRE].GetMP() > playerMP)
        insert(SKILLID_MAP::value_type(SKILL_TURRET_FIRE,
                                       SKILLID_NODE(SKILL_TURRET_FIRE, 0)));
      else
        insert(SKILLID_MAP::value_type(
            SKILL_TURRET_FIRE,
            SKILLID_NODE(SKILL_TURRET_FIRE, FLAG_SKILL_ENABLE)));
      insert(SKILLID_MAP::value_type(
          SKILL_VIVID_MAGAZINE,
          SKILLID_NODE(SKILL_VIVID_MAGAZINE, FLAG_SKILL_ENABLE)));
      return;
    }

    // Add By Hays 2013  3  10
    if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_INSTALL_MK1)) {
      bool bCheckNAPALNBULLET =
          ((*g_pSkillManager)[SKILLDOMAIN_GUN].GetSkillStatus(
               SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET) ==
           MSkillDomain::SKILLSTATUS_LEARNED);
      bool bCheckNAPALNBULLET2 =
          ((*g_pSkillManager)[SKILLDOMAIN_GUN].GetSkillStatus(
               SKILL_CLIENT_ADV2_HALCYON_MK1) ==
           MSkillDomain::SKILLSTATUS_LEARNED);

      insert(SKILLID_MAP::value_type(
          MAGIC_UN_TRANSFORM,
          SKILLID_NODE(MAGIC_UN_TRANSFORM, FLAG_SKILL_ENABLE)));

      if ((*g_pSkillInfoTable)[SKILL_CLINT_ADV2_HALCYON_MK1].GetMP() > playerMP)
        insert(SKILLID_MAP::value_type(
            SKILL_CLINT_ADV2_HALCYON_MK1,
            SKILLID_NODE(SKILL_CLINT_ADV2_HALCYON_MK1, 0)));

      if ((bCheckNAPALNBULLET == true) &&
          (*g_pSkillInfoTable)[SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET].GetMP() >
              playerMP) {
        insert(SKILLID_MAP::value_type(
            SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET,
            SKILLID_NODE(SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET, 0)));
        if ((bCheckNAPALNBULLET2 == true) &&
            (*g_pSkillInfoTable)[SKILL_CLIENT_ADV2_HALCYON_MK1].GetMP() >
                playerMP) {
          insert(SKILLID_MAP::value_type(
              SKILL_CLIENT_ADV2_HALCYON_MK1,
              SKILLID_NODE(SKILL_CLIENT_ADV2_HALCYON_MK1, 0)));
        }
      }

      else
        insert(SKILLID_MAP::value_type(
            SKILL_CLINT_ADV2_HALCYON_MK1,
            SKILLID_NODE(SKILL_CLINT_ADV2_HALCYON_MK1, FLAG_SKILL_ENABLE)));

      if (bCheckNAPALNBULLET == true) {
        insert(SKILLID_MAP::value_type(
            SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET,
            SKILLID_NODE(SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET,
                         FLAG_SKILL_ENABLE)));
      }
      if (bCheckNAPALNBULLET2 == true) {
        insert(SKILLID_MAP::value_type(
            SKILL_CLIENT_ADV2_HALCYON_MK1,
            SKILLID_NODE(SKILL_CLIENT_ADV2_HALCYON_MK1, FLAG_SKILL_ENABLE)));
      }
      return;
    }
    // 2004, 9, 16, sobeit add end - ì¸ìŠ¤í†¨ í„°ë ›ì¼ë•Œ ìŠ¤í‚¬ ì •ë³´ ê°±ì‹ 
    //-----------------------------------------------------
    //
    // Domainì— ë”°ë¥¸ enable ì²´í¬..
    //
    //-----------------------------------------------------
    BYTE fDomain[MAX_SKILLDOMAIN];

    // í˜„ìž¬ ë“¤ê³  ìžˆëŠ” item
    const MItem *pItem =
        (*g_pSlayerGear)
            .GetItem(
                (MSlayerGear::GEAR_SLAYER)MSlayerGear::GEAR_SLAYER_RIGHTHAND);

    //-----------------------------------------------------
    // gun/sword/blade ë§Œ ì²´í¬í•˜ë©´ ëœë‹¤.
    //-----------------------------------------------------
    fDomain[SKILLDOMAIN_GUN] = 0;
    fDomain[SKILLDOMAIN_BLADE] = 0;
    fDomain[SKILLDOMAIN_SWORD] = 0;

    if (pItem != NULL && pItem->IsAffectStatus()) {
      //-----------------------------------------------------
      // ì´ì´ë©´.. ì´ë§Œ enable
      //-----------------------------------------------------
      if (pItem->IsGunItem()) {
        fDomain[SKILLDOMAIN_GUN] = FLAG_SKILL_ENABLE;
      }
      //-----------------------------------------------------
      // swordì´ë©´ swordë§Œ enable
      //-----------------------------------------------------
      else if (pItem->GetItemClass() == ITEM_CLASS_SWORD) {
        fDomain[SKILLDOMAIN_SWORD] = FLAG_SKILL_ENABLE;
      }
      //-----------------------------------------------------
      // bladeì´ë©´ bladeë§Œ enable
      //-----------------------------------------------------
      else if (pItem->GetItemClass() == ITEM_CLASS_BLADE) {
        fDomain[SKILLDOMAIN_BLADE] = FLAG_SKILL_ENABLE;
      }
    }

    //-----------------------------------------------------
    //
    // SkillTree ê²€ìƒ‰
    //
    //-----------------------------------------------------
    //-----------------------------------------------------
    // Blade
    //-----------------------------------------------------
    MSkillDomain &bladeDomain = (*g_pSkillManager)[SKILLDOMAIN_BLADE];

    bladeDomain.SetBegin();
    while (bladeDomain.IsNotEnd()) {
      MSkillDomain::SKILLSTATUS status = bladeDomain.GetSkillStatus();

      // ë°°ì› ìœ¼ë©´..
      if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
        ACTIONINFO id = bladeDomain.GetSkillID();

        if ((*g_pSkillInfoTable)[id].GetMP() > playerMP) {
          flag = 0;
        } else {
          flag = fDomain[SKILLDOMAIN_BLADE];

          if ((*g_pSkillInfoTable)[id].IsActive()) {
            flag |= FLAG_SKILL_ENABLE;
          }
        }

        insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
      }

      // ë‹¤ìŒ
      bladeDomain.Next();
    }

    //-----------------------------------------------------
    // Sword
    //-----------------------------------------------------
    MSkillDomain &swordDomain = (*g_pSkillManager)[SKILLDOMAIN_SWORD];

    swordDomain.SetBegin();
    while (swordDomain.IsNotEnd()) {
      MSkillDomain::SKILLSTATUS status = swordDomain.GetSkillStatus();

      // ë°°ì› ìœ¼ë©´..
      if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
        ACTIONINFO id = swordDomain.GetSkillID();
        int iSilver = 0;

        if (pItem)
          iSilver = pItem->GetSilver();

        if ((*g_pSkillInfoTable)[id].GetMP() > playerMP) {
          flag = 0;
        }
        //	else if(iSilver == 0 && id == SKILL_SHINE_SWORD)
        //	{
        //		flag = 0;
        //	}
        else {
          flag = fDomain[SKILLDOMAIN_SWORD];

          if ((*g_pSkillInfoTable)[id].IsActive()) {
            flag |= FLAG_SKILL_ENABLE;
          }
        }

        insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
      }

      // ë‹¤ìŒ
      swordDomain.Next();
    }

    //-----------------------------------------------------
    // Gun
    //-----------------------------------------------------
    MSkillDomain &gunDomain = (*g_pSkillManager)[SKILLDOMAIN_GUN];

    gunDomain.SetBegin();
    while (gunDomain.IsNotEnd()) {
      MSkillDomain::SKILLSTATUS status = gunDomain.GetSkillStatus();

      // ë°°ì› ìœ¼ë©´..
      if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
        ACTIONINFO id = gunDomain.GetSkillID();

        if ((*g_pSkillInfoTable)[id].GetMP() > playerMP) {
          flag = 0;
        } else {
          flag = fDomain[SKILLDOMAIN_GUN];

          if ((*g_pSkillInfoTable)[id].IsActive()) {
            flag |= FLAG_SKILL_ENABLE;
          }
        }

        insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
      }

      // ë‹¤ìŒ
      gunDomain.Next();
    }

    //-----------------------------------------------------
    // Enchant - ê·¸ëƒ¥ ëª¨ë‘ ì¶”ê°€í•˜ë©´ ëœë‹¤.
    //-----------------------------------------------------
    MSkillDomain &enchantDomain = (*g_pSkillManager)[SKILLDOMAIN_ENCHANT];

    enchantDomain.SetBegin();
    while (enchantDomain.IsNotEnd()) {
      MSkillDomain::SKILLSTATUS status = enchantDomain.GetSkillStatus();

      // ë°°ì› ìœ¼ë©´..
      if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
        ACTIONINFO id = enchantDomain.GetSkillID();

        int iSilver = 0;
        if (pItem)
          iSilver = pItem->GetSilver();

        if ((*g_pSkillInfoTable)[id].GetMP() > playerMP) {
          flag = 0;
        }
        // 20090306 ì¸ì±ˆíŠ¸ ìŠ¤í‚¬ì¤‘ì—ì„œë„ ì€ë„ê¸ˆì— ì˜í–¥ ë°›ëŠ” ìŠ¤í‚¬ì€ ì²´í¬ë¥¼ í•˜ìž
        //	else if(iSilver == 0 && id == SKILL_HEAVEN_GROUND)
        //	{
        //		flag = 0;
        //	}
        else {
          flag = FLAG_SKILL_ENABLE;
        }

        insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
      }

      // ë‹¤ìŒ
      enchantDomain.Next();
    }

    //-----------------------------------------------------
    // Heal - ê·¸ëƒ¥ ëª¨ë‘ ì¶”ê°€í•˜ë©´ ëœë‹¤.
    //-----------------------------------------------------
    MSkillDomain &healDomain = (*g_pSkillManager)[SKILLDOMAIN_HEAL];

    healDomain.SetBegin();
    while (healDomain.IsNotEnd()) {
      MSkillDomain::SKILLSTATUS status = healDomain.GetSkillStatus();

      // ë°°ì› ìœ¼ë©´..
      if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
        ACTIONINFO id = healDomain.GetSkillID();

        if ((*g_pSkillInfoTable)[id].GetMP() > playerMP) {
          flag = 0;
        } else {
          flag = FLAG_SKILL_ENABLE;
        }

        insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
      }

      // ë‹¤ìŒ
      healDomain.Next();
    }

    //-----------------------------------------------------
    //
    // Inventory ê²€ìƒ‰
    //
    //-----------------------------------------------------
    BOOL bCheckHolyWater = TRUE;
    BOOL bCheckPortal = TRUE;

    BOOL bCheckInstallMine = (gunDomain.GetSkillStatus(SKILL_INSTALL_MINE) ==
                              MSkillDomain::SKILLSTATUS_LEARNED);
    BOOL bCheckCreateMine = (gunDomain.GetSkillStatus(SKILL_MAKE_MINE) ==
                             MSkillDomain::SKILLSTATUS_LEARNED);
    BOOL bCheckCreateBomb = (gunDomain.GetSkillStatus(SKILL_MAKE_BOMB) ==
                             MSkillDomain::SKILLSTATUS_LEARNED);

    BOOL bCheckBomb = (gunDomain.GetSkillStatus(SKILL_THROW_BOMB) ==
                       MSkillDomain::SKILLSTATUS_LEARNED);
    BOOL bCheckBombOrMine = bCheckBomb || bCheckInstallMine;
    BOOL bCheckBombOrMineMaterial = bCheckCreateBomb || bCheckCreateMine;

    BOOL bHasBomb = FALSE;
    BOOL bHasMine = FALSE;
    BOOL bHasMineMaterial = FALSE;
    BOOL bHasBombMaterial = FALSE;

    g_pInventory->SetBegin();

    while (g_pInventory->IsNotEnd()) {
      const MItem *pItem = g_pInventory->Get();

      ITEM_CLASS itemClass = pItem->GetItemClass();

      // 2005, 3, 2, sobeit add start - ì„œë¸Œ ì¸ë²¤ë„ ë’¤ì§€ìž..-_-; ì†ŒìŠ¤ê°€
      // ê¹ê¹í•˜ë„¤..ã…‹
      if (itemClass == ITEM_CLASS_SUB_INVENTORY) {
        MSubInventory *pSubItem = (MSubInventory *)pItem;
        pSubItem->SetBegin();
        while (pSubItem->IsNotEnd()) {

          const MItem *pSubSbuItem = pSubItem->Get();
          if (NULL != pSubSbuItem && bCheckPortal &&
              pSubSbuItem->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM) {
            flag = FLAG_SKILL_ENABLE;
            insert(SKILLID_MAP::value_type(
                SUMMON_HELICOPTER, SKILLID_NODE(SUMMON_HELICOPTER, flag)));
            bCheckPortal = FALSE;
          }
          pSubItem->Next();
        }
      }

      // 2005, 3, 2, sobeit add end

      //-----------------------------------------------------
      // Portal
      //-----------------------------------------------------
      if (bCheckPortal && itemClass == ITEM_CLASS_SLAYER_PORTAL_ITEM) {
        flag = FLAG_SKILL_ENABLE;

        insert(SKILLID_MAP::value_type(SUMMON_HELICOPTER,
                                       SKILLID_NODE(SUMMON_HELICOPTER, flag)));

        bCheckPortal = FALSE;
      }
      //-----------------------------------------------------
      // HolyWater
      //-----------------------------------------------------
      else if (bCheckHolyWater && itemClass == ITEM_CLASS_HOLYWATER) {
        if ((*g_pSkillInfoTable)[MAGIC_THROW_HOLY_WATER].GetMP() > playerMP) {
          flag = 0;
        } else {
          flag = FLAG_SKILL_ENABLE;
        }

        insert(SKILLID_MAP::value_type(
            MAGIC_THROW_HOLY_WATER,
            SKILLID_NODE(MAGIC_THROW_HOLY_WATER, flag)));

        // [ë„ì›€ë§] ë²¨íŠ¸ì˜ ì•„ì´í…œ ì‚¬ìš©
        //					__BEGIN_HELP_EVENT
        //						ExecuteHelpEvent(
        // HE_ITEM_APPEAR_HOLY_WATER );
        //					__END_HELP_EVENT

        bCheckHolyWater = FALSE;
      }
      //-----------------------------------------------------
      // Bomb / Mine - ì¢…ë¥˜ë³„ë¡œ ë”°ë¡œ ì¶”ê°€í•´ì•¼í•œë‹¤.
      //-----------------------------------------------------
      else if (bCheckBombOrMine &&
               (itemClass == ITEM_CLASS_BOMB || itemClass == ITEM_CLASS_MINE)) {
        if (itemClass == ITEM_CLASS_BOMB) {
          bHasBomb = TRUE;

          if (bCheckBomb)
            flag = FLAG_SKILL_ENABLE;
          else
            flag = 0;
        } else {
          bHasMine = TRUE;

          if (bCheckInstallMine)
            flag = FLAG_SKILL_ENABLE;
          else
            flag = 0;
        }

        int skillID = pItem->GetUseActionInfo();

        // í­íƒ„/ì§€ë¢° ë§ˆë‹¤ ì‚¬ìš© ê°€ëŠ¥í•œ ì•„ì´ì½˜ì„ ì¶”ê°€í•œë‹¤.
        if (find((ACTIONINFO)skillID) == end()) {
          insert(SKILLID_MAP::value_type(
              (ACTIONINFO)skillID, SKILLID_NODE((ACTIONINFO)skillID, flag)));
        }
      }
      //-----------------------------------------------------
      // í­íƒ„/ì§€ë¢° ìž¬ë£Œ
      //-----------------------------------------------------
      else if (bCheckBombOrMineMaterial &&
               itemClass == ITEM_CLASS_BOMB_MATERIAL) {
        if (IsBombMaterial(pItem)) {
          bHasBombMaterial = TRUE;
        } else {
          bHasMineMaterial = TRUE;
        }
      }

      // ë‹¤ìŒ
      g_pInventory->Next();
    }

    // mouseì— ìžˆëŠ” ì•„ì´í…œë„ ì²´í¬í•œë‹¤.
    MItem *pMouseItem = UI_GetMouseItem();

    if (pMouseItem != NULL) {
      ITEM_CLASS itemClass = pMouseItem->GetItemClass();
      bool isBombMaterial = IsBombMaterial(pMouseItem);

      bHasBomb = bHasBomb || itemClass == ITEM_CLASS_BOMB;
      bHasMine = bHasMine || itemClass == ITEM_CLASS_MINE;

      bHasMineMaterial =
          bHasMineMaterial ||
          itemClass == ITEM_CLASS_BOMB_MATERIAL && !isBombMaterial;
      bHasBombMaterial =
          bHasBombMaterial ||
          itemClass == ITEM_CLASS_BOMB_MATERIAL && isBombMaterial;
    }

    // ì§€ë¢° ì„¤ì¹˜ ê¸°ìˆ ì„ ë°°ì› ê³  ì§€ë¢°ê°€ ìžˆë‹¤ë©´ iconì„ enableì‹œí‚¨ë‹¤.
    if (bCheckInstallMine) {
      ACTIONINFO skillID = SKILL_INSTALL_MINE;
      flag = (IsEnableSkill(skillID) && bHasMine ? FLAG_SKILL_ENABLE : 0);

      iterator iSkill = find(skillID);
      if (iSkill != end()) {
        iSkill->second.Flag = flag;
      } else {
        insert(SKILLID_MAP::value_type(skillID, SKILLID_NODE(skillID, flag)));
      }
    }

    // ì§€ë¢° ìƒì„± ê¸°ìˆ ì„ ë°°ì› ê³  ì§€ë¢° ìž¬ë£Œê°€ ìžˆë‹¤ë©´ iconì„ enableì‹œí‚¨ë‹¤.
    if (bCheckCreateMine) {
      ACTIONINFO skillID = SKILL_MAKE_MINE;
      flag =
          (IsEnableSkill(skillID) && bHasMineMaterial ? FLAG_SKILL_ENABLE : 0);

      iterator iSkill = find(skillID);
      if (iSkill != end()) {
        iSkill->second.Flag = flag;
      } else {
        insert(SKILLID_MAP::value_type(skillID, SKILLID_NODE(skillID, flag)));
      }
    }

    // í­íƒ„ ìƒì„± ê¸°ìˆ ì„ ë°°ì› ê³  í­íƒ„ ìž¬ë£Œê°€ ìžˆë‹¤ë©´ iconì„ enableì‹œí‚¨ë‹¤.
    if (bCheckCreateBomb) {
      ACTIONINFO skillID = SKILL_MAKE_BOMB;
      flag =
          (IsEnableSkill(skillID) && bHasBombMaterial ? FLAG_SKILL_ENABLE : 0);

      iterator iSkill = find(skillID);
      if (iSkill != end()) {
        iSkill->second.Flag = flag;
      } else {
        insert(SKILLID_MAP::value_type(skillID, SKILLID_NODE(skillID, flag)));
      }
    }

    // í­íƒ„ ë˜ì§€ê¸° ê¸°ìˆ ì„ ë°°ì› ê³  í­íƒ„ì´ ìžˆë‹¤ë©´ iconì„ enableì‹œí‚¨ë‹¤.
    if (bCheckBomb) {
      ACTIONINFO skillID = SKILL_THROW_BOMB;
      flag = (IsEnableSkill(skillID) && bHasBomb ? FLAG_SKILL_ENABLE : 0);

      iterator iSkill = find(skillID);
      if (iSkill != end()) {
        iSkill->second.Flag = flag;
      } else {
        insert(SKILLID_MAP::value_type(skillID, SKILLID_NODE(skillID, flag)));
      }
    }

    //-----------------------------------------------------
    // Restore ìž„ì˜ë¡œ ì¶”ê°€
    //-----------------------------------------------------
    if (g_pUserInformation->HasSkillRestore) {
      if ((*g_pSkillInfoTable)[MAGIC_RESTORE].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }

      insert(SKILLID_MAP::value_type(MAGIC_RESTORE,
                                     SKILLID_NODE(MAGIC_RESTORE, flag)));
    }

    //-----------------------------------------------------
    // ì€ë„ê¸ˆì´ í•„ìš”í•œ ìŠ¤í‚¬ë“¤
    //-----------------------------------------------------
    /*	if(pItem != NULL && !pItem->IsGunItem())
            {
                    if(pItem->GetSilver() < 50)
                    {
                            for(int i = 0; i < 5; ++i) {
                                    ACTIONINFO localActionInfo =
       static_cast<ACTIONINFO>(SKILL_SHINE_SWORD + i);

                                    iterator iSkill = find( localActionInfo );
                                    if (iSkill != end())
                                            iSkill->second.Flag = false;
                            }
                    }
            }*/ // 2013-5-23  by rallser íŒê·ç˜»æ–‚ì„¸ì½˜çŸœï¤«ë—¬ï¥™
  } break;

  //-----------------------------------------------------
  //
  //					vampireì¸ ê²½ìš°
  //
  //-----------------------------------------------------
  case RACE_VAMPIRE: {
    //-----------------------------------------------------
    //
    // SkillTree ê²€ìƒ‰
    //
    //-----------------------------------------------------
    // [ìƒˆê¸°ìˆ 3] ê´€ ì†ì—ì„œëŠ” ê¸°ìˆ  ëª» ì“´ë‹¤.
    if (g_pPlayer->IsInCasket()) {
      flag = FLAG_SKILL_ENABLE;

      insert(SKILLID_MAP::value_type(MAGIC_OPEN_CASKET,
                                     SKILLID_NODE(MAGIC_OPEN_CASKET, flag)));
      gC_vs_ui.SelectSkill(MAGIC_OPEN_CASKET);

      return;
    }

    // ë±€íŒŒì´ì–´ì¸ ê²½ìš°ë§Œ ì“¸ ìˆ˜ ìžˆë‹¤. --> ë°•ì¥ë‚˜ ëŠ‘ëŒ€ì—ì„œëŠ” ì‚¬ìš©ëª»í•œë‹¤.
    if (g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_MALE1 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_FEMALE1 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_MALE2 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_FEMALE2 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_MALE3 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_FEMALE3 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_MALE4 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_FEMALE4 ||
        g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR) {
      MSkillDomain &vampireDomain = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE];

      vampireDomain.SetBegin();

      MItem *pSubInventory = NULL;
      while (vampireDomain.IsNotEnd()) {
        MSkillDomain::SKILLSTATUS status = vampireDomain.GetSkillStatus();

        // ë°°ì› ìœ¼ë©´..
        if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
          ACTIONINFO id = vampireDomain.GetSkillID();

          // ë ˆì–´ì¡´ì—ì„œëŠ” ì¸ë¹„ì €ë¹Œë¦¬í‹° ëª»ì“°ê²Œ í•œë‹¤...í•˜ë“œí•˜ë“œ
          if ((*g_pSkillInfoTable)[id].GetMP() > playerMP ||
              id == MAGIC_INVISIBILITY &&
                  (g_pZone->GetID() == 1104 || g_pZone->GetID() == 1106 ||
                   g_pZone->GetID() == 1114 || g_pZone->GetID() == 1115)) {
            flag = 0;
          } else {
            flag = FLAG_SKILL_ENABLE;
          }

          // Item ì‚¬ìš©í•˜ëŠ”ê±° ì²´í¬
          switch (id) {
          case MAGIC_BLOODY_MARK:
            if (NULL == ((MItemManager *)g_pInventory)
                            ->FindItemAll(MVampirePortalItemFinder(false),
                                          pSubInventory)) {
              flag = 0;
            }
            break;

          case MAGIC_BLOODY_TUNNEL:
            if (NULL == ((MItemManager *)g_pInventory)
                            ->FindItemAll(MVampirePortalItemFinder(true),
                                          pSubInventory)) {
              flag = 0;
            }
            break;

          case MAGIC_TRANSFORM_TO_WOLF:
            if (NULL == g_pInventory->FindItem(ITEM_CLASS_VAMPIRE_ETC, 0)) {
              flag = 0;
            }
            break;

          case MAGIC_TRANSFORM_TO_BAT:
            if (NULL == g_pInventory->FindItemAll(
                            MItemClassTypeFinder(ITEM_CLASS_VAMPIRE_ETC, 1),
                            pSubInventory)
#if __CONTENTS(__FAST_TRANSFORTER)
                && NULL ==
                       g_pInventory->FindItemAll(
                           MItemClassTypeFinder(ITEM_CLASS_VAMPIREWING_ITEM, 0),
                           pSubInventory) &&
                NULL ==
                    g_pInventory->FindItemAll(
                        MItemClassTypeFinder(ITEM_CLASS_VAMPIREWING_ITEM, 2),
                        pSubInventory)
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
                && NULL ==
                       g_pInventory->FindItemAll(
                           MItemClassTypeFinder(ITEM_CLASS_VAMPIREWING_ITEM, 1),
                           pSubInventory)
#endif //__SECOND_TRANSFORTER
            )

            {
              flag = 0;
            }
            break;

          case SKILL_TRANSFORM_TO_WERWOLF:
            if (g_pInventory->FindItem(ITEM_CLASS_SKULL, 39) == NULL) {
              flag = 0;
            }
            break;

          case MAGIC_HOWL:
            flag = 0;
            break;
          }

          insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
        }

        // ë‹¤ìŒ
        vampireDomain.Next();
      }
    }

    //-----------------------------------------------------
    //
    // Inventory ê²€ìƒ‰
    //
    //-----------------------------------------------------
    /*
    BOOL bCheckPortalMark = TRUE;
    BOOL bCheckPortalTunnel = TRUE;

    g_pInventory->SetBegin();

    while ((*g_pInventory).IsNotEnd())
    {
            const MItem* pItem = g_pInventory->Get();

            ITEM_CLASS itemClass = pItem->GetItemClass();

            //-----------------------------------------------------
            // Portal
            //-----------------------------------------------------
            if (itemClass==ITEM_CLASS_VAMPIRE_PORTAL_ITEM)
            {
                    flag = FLAG_SKILL_ENABLE;

                    if (bCheckPortalMark && !pItem->IsMarked())
                    {
                            insert(SKILLID_MAP::value_type( MAGIC_BLOODY_MARK,
    SKILLID_NODE(MAGIC_BLOODY_MARK, flag)) );

                            bCheckPortalMark = false;
                    }

                    if (bCheckPortalTunnel && pItem->IsMarked())
                    {
                            insert(SKILLID_MAP::value_type( MAGIC_BLOODY_TUNNEL,
    SKILLID_NODE(MAGIC_BLOODY_TUNNEL, flag)) );

                            bCheckPortalTunnel = false;
                    }
            }

            // ë‹¤ìŒ
            g_pInventory->Next();
    }
    */

    //-----------------------------------------------------
    //
    // ê¸°ë³¸ Skill
    //
    //-----------------------------------------------------
    // í¡í˜ˆ --> ëŠ‘ëŒ€ë‚˜ ë°•ì¥ëŠ” í¡í˜ˆ ëª»í•œë‹¤.

    //-----------------------------------------------------
    // ë¶ˆê¸°ë‘¥ ìž„ì˜ë¡œ ì¶”ê°€
    //-----------------------------------------------------
    if (g_pUserInformation->HasMagicGroundAttack) {
      if ((*g_pSkillInfoTable)[MAGIC_GROUND_ATTACK].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }

      insert(SKILLID_MAP::value_type(MAGIC_GROUND_ATTACK,
                                     SKILLID_NODE(MAGIC_GROUND_ATTACK, flag)));
    }

    SetAvailableVampireSkills();

    //-----------------------------------------------------
    // ë¸”ëŸ¬ë”” ìŠ¤ë„¤ì´í¬ ìž„ì˜ë¡œ ì¶”ê°€
    //-----------------------------------------------------
    if (g_pUserInformation->HasMagicBloodySnake) {
      if ((*g_pSkillInfoTable)[MAGIC_BLOODY_SNAKE].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }

      insert(SKILLID_MAP::value_type(MAGIC_BLOODY_SNAKE,
                                     SKILLID_NODE(MAGIC_BLOODY_SNAKE, flag)));
    }

    //-----------------------------------------------------
    // ë¸”ëŸ¬ë”” ì›Œí”„ ìž„ì˜ë¡œ ì¶”ê°€
    //-----------------------------------------------------
    if (g_pUserInformation->HasMagicBloodyWarp) {
      if ((*g_pSkillInfoTable)[MAGIC_BLOODY_WARP].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }

      insert(SKILLID_MAP::value_type(MAGIC_BLOODY_WARP,
                                     SKILLID_NODE(MAGIC_BLOODY_WARP, flag)));
    }
  } break;

  case RACE_OUSTERS: {
    //-----------------------------------------------------
    //
    // SkillTree ê²€ìƒ‰
    //
    //-----------------------------------------------------
    {
      MSkillDomain &oustersDomain = (*g_pSkillManager)[SKILLDOMAIN_OUSTERS];

      oustersDomain.SetBegin();
      while (oustersDomain.IsNotEnd()) {
        MSkillDomain::SKILLSTATUS status = oustersDomain.GetSkillStatus();

        // ë°°ì› ìœ¼ë©´..
        if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
          ACTIONINFO id = oustersDomain.GetSkillID();

          SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[id];

          flag = 0;

          if (sInfo.GetMP() <= playerMP) {
            if (sInfo.IsActive()) {
              flag = FLAG_SKILL_ENABLE;
            } else {
              // í˜„ìž¬ ë“¤ê³  ìžˆëŠ” item
              const MItem *pItem =
                  (*g_pOustersGear)
                      .GetItem((MOustersGear::GEAR_OUSTERS)
                                   MOustersGear::GEAR_OUSTERS_RIGHTHAND);

              if (sInfo.ElementalDomain ==
                      SKILLINFO_NODE::ELEMENTAL_DOMAIN_NO_DOMAIN ||
                  sInfo.ElementalDomain ==
                      SKILLINFO_NODE::ELEMENTAL_DOMAIN_WIND ||
                  sInfo.ElementalDomain ==
                      SKILLINFO_NODE::ELEMENTAL_DOMAIN_ETC ||
                  sInfo.GetSkillStep() == SKILL_STEP_ETC) {
                flag = FLAG_SKILL_ENABLE;
              } else {
                if (pItem != NULL && pItem->IsAffectStatus()) {
                  const int itemClass = pItem->GetItemClass();

                  if (itemClass == ITEM_CLASS_OUSTERS_CHAKRAM &&
                          sInfo.ElementalDomain ==
                              SKILLINFO_NODE::ELEMENTAL_DOMAIN_COMBAT ||
                      itemClass == ITEM_CLASS_OUSTERS_WRISTLET &&
                          (sInfo.ElementalDomain ==
                                   SKILLINFO_NODE::ELEMENTAL_DOMAIN_FIRE &&
                               sInfo.Fire <= g_pPlayer->GetElementalFire() ||
                           sInfo.ElementalDomain ==
                                   SKILLINFO_NODE::ELEMENTAL_DOMAIN_WATER &&
                               sInfo.Water <= g_pPlayer->GetElementalWater() ||
                           sInfo.ElementalDomain ==
                                   SKILLINFO_NODE::ELEMENTAL_DOMAIN_EARTH &&
                               sInfo.Earth <= g_pPlayer->GetElementalEarth())) {
                    flag = FLAG_SKILL_ENABLE;
                  }
                }
              }
            }
          }

          if (id == SKILL_SUMMON_SYLPH) {
            MItem *pSubInventory = NULL;
            if (NULL == ((MItemManager *)g_pInventory)
                            ->FindItemAll(MOustersSummonGemItemFinder(),
                                          pSubInventory)
#if __CONTENTS(__FAST_TRANSFORTER)
                && NULL ==
                       g_pInventory->FindItemAll(
                           MItemClassTypeFinder(ITEM_CLASS_OUSTERSWING_ITEM, 0),
                           pSubInventory)
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
                && NULL ==
                       g_pInventory->FindItemAll(
                           MItemClassTypeFinder(ITEM_CLASS_OUSTERSWING_ITEM, 1),
                           pSubInventory) &&
                NULL ==
                    g_pInventory->FindItemAll(
                        MItemClassTypeFinder(ITEM_CLASS_OUSTERSWING_ITEM, 2),
                        pSubInventory)
#endif //__SECOND_TRANSFORTER
            )
              flag = 0;
          }

          insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
        }

        // ë‹¤ìŒ
        oustersDomain.Next();
      }
    }

    //-----------------------------------------------------
    //
    // ê¸°ë³¸ Skill
    //
    //-----------------------------------------------------
    //			// í¡í˜ˆ --> ëŠ‘ëŒ€ë‚˜ ë°•ì¥ëŠ” í¡í˜ˆ ëª»í•œë‹¤.
    //			if (g_pPlayer->GetCreatureType()!=CREATURETYPE_BAT
    //				&&
    // g_pPlayer->GetCreatureType()!=CREATURETYPE_WOLF)
    //			{
    //				if
    //((*g_pSkillInfoTable)[SKILL_BLOOD_DRAIN].GetMP() > playerMP)
    //				{
    //					flag = 0;
    //				}
    //				else
    //				{
    //					flag = FLAG_SKILL_ENABLE;
    //				}
    //				insert(SKILLID_MAP::value_type(
    // SKILL_BLOOD_DRAIN, SKILLID_NODE(SKILL_BLOOD_DRAIN, flag) ));
    //			}

  } break;
  }

  //-----------------------------------------------------
  // Etc - ê·¸ëƒ¥ ëª¨ë‘ ì¶”ê°€í•˜ë©´ ëœë‹¤.
  //-----------------------------------------------------
  MSkillDomain &etcDomain = (*g_pSkillManager)[SKILLDOMAIN_ETC];

  etcDomain.SetBegin();
  while (etcDomain.IsNotEnd()) {
    MSkillDomain::SKILLSTATUS status = etcDomain.GetSkillStatus();

    // ë°°ì› ìœ¼ë©´..
    if (status == MSkillDomain::SKILLSTATUS_LEARNED) {
      ACTIONINFO id = etcDomain.GetSkillID();

      if ((*g_pSkillInfoTable)[id].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }

      erase(id);
      insert(SKILLID_MAP::value_type(id, SKILLID_NODE(id, flag)));
    }

    // ë‹¤ìŒ
    etcDomain.Next();
  }

  //-----------------------------------------------------
  //
  // í”¼ì˜ ì„±ì„œ ë³´ë„ˆìŠ¤ ë§˜ëŒ€ë¡œ ì¶”ê°€-ã……-
  //
  //-----------------------------------------------------
  for (int i = 0; i < HOLYLAND_BONUS_MAX; i++) {
    if (g_abHolyLandBonusSkills[i] == true) {
      insert(SKILLID_MAP::value_type(
          (ACTIONINFO)(SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA + i),
          SKILLID_NODE((ACTIONINFO)(SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA + i),
                       FLAG_SKILL_ENABLE)));
    }
  }

  for (int i = 0; i < SWEEPER_BONUS_MAX; i++) {
    if (g_abSweeperBonusSkills[i] == true) {
      insert(SKILLID_MAP::value_type(
          (ACTIONINFO)(SKILL_SWEEPER_BONUS_1 + i),
          SKILLID_NODE((ACTIONINFO)(SKILL_SWEEPER_BONUS_1 + i),
                       FLAG_SKILL_ENABLE)));
    }
  }

  if (g_pPlayer->GetCreatureType() != CREATURETYPE_BAT &&
      g_pPlayer->GetCreatureType() != CREATURETYPE_VAMPIRE_GHOST &&
      g_pPlayer->GetCreatureType() != CREATURETYPE_WOLF
#if __CONTENTS(__FAST_TRANSFORTER)
      && g_pPlayer->GetCreatureType() != CREATURETYPE_FLITTERMOUSE &&
      g_pPlayer->GetCreatureType() != CREATURETYPE_NEDE &&
      g_pPlayer->GetCreatureType() != CREATURETYPE_KLTL
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
      && g_pPlayer->GetCreatureType() != CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER

  ) {
    // ì›Œì•„ì´í…œì„ ê°€ì§€ê³  ìžˆìœ¼ë©´ ì»¤í”Œì²´ì¸ì€ ì‚¬ìš©í• ìˆ˜ ì—†ë‹¤.
    MItem *pWarItem = g_pInventory->FindItem(ITEM_CLASS_WAR_ITEM);
    if (pWarItem != NULL) {
    } else {
      MPlayerGear *pGear;
      MItemClassFinder itemFinder(ITEM_CLASS_COUPLE_RING);

      switch (g_pPlayer->GetRace()) {
      case RACE_SLAYER:
        pGear = g_pSlayerGear;
        break;

      case RACE_VAMPIRE:
        pGear = g_pVampireGear;
        itemFinder.SetItemClass(ITEM_CLASS_VAMPIRE_COUPLE_RING);
        break;

      case RACE_OUSTERS:
        itemFinder.SetItemClass(ITEM_CLASS_OUSTERS_HARMONIC_PENDENT);
        pGear = g_pOustersGear;
        break;
      }

      MItem *pItem = pGear->FindItem(itemFinder);
      if (pItem != NULL) {
        insert(
            SKILLID_MAP::value_type((ACTIONINFO)SKILL_LOVE_CHAIN,
                                    SKILLID_NODE((ACTIONINFO)(SKILL_LOVE_CHAIN),
                                                 pItem->IsAffectStatus())));
      }
    }
  }

  //	insert(SKILLID_MAP::value_type( (ACTIONINFO)SKILL_MAGIC_ELUSION,
  // SKILLID_NODE((ACTIONINFO)(SKILL_MAGIC_ELUSION),  FLAG_SKILL_ENABLE)) );

  //-----------------------------------------------------
  // ì¼ìžì½”í í”„ë¡¤ë¡œê·¸ - ê»ë‹¥ ë²—ê¸°ê¸°
  // : ë¬´ë‘ì§ˆìš© ì¹¼ì´ ìžˆì„ ê²½ìš°ì—ë§Œ ë¬´ë‘ì§ˆ ìŠ¤í‚¬ ì¶”ê°€
  //-----------------------------------------------------
  MItemClassTypeFinder findTanningKnife(ITEM_CLASS_COMMON_QUEST_ITEM, 3);
  MItem *pSubInventory;
  MItem *pTanningKnife =
      g_pInventory->FindItemAll(findTanningKnife, pSubInventory);

  if (pTanningKnife) {
    ACTIONINFO id = static_cast<ACTIONINFO>(SKILL_TANNING);
    SKILLID_NODE node = SKILLID_NODE(id, FLAG_SKILL_ENABLE);
    insert(SKILLID_MAP::value_type(id, node));
  }

  //-----------------------------------------------------
  // ì‹œì•¼ ê³„ì‚°
  //-----------------------------------------------------
  g_pPlayer->CalculateLightSight();

  // CheckMP();

  // ìŠ¤í‚¬ì´ ë¹„ì—ˆì„ë•Œ í˜¸ì¶œë˜ë©´ -_- ë‚œë¦¬ë‚œë‹¤.
//	if( size() > 5 )
//		gC_vs_ui.ResetHotKey();
#endif
}

void MSkillSet::SetAvailableVampireSkills() {
#ifdef __GAME_CLIENT__
  MSkillDomain &vampireDomain = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE];
  TYPE_CREATURETYPE PlayerCreatureType = g_pPlayer->GetCreatureType();

  int playerMP;
  BYTE flag;

  if (g_pPlayer->GetRace() != RACE_VAMPIRE) {
    playerMP = g_pPlayer->GetMP();

    // EFFECTSTATUS_SACRIFICE ì‚¬ìš©ì¤‘ì´ë©´ HP 1ì´ MP 2ê°€ ëœë‹¤.
    if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_SACRIFICE)) {
      playerMP += (g_pPlayer->GetHP() << 1);
    }
  } else {
    // vampireì¸ ê²½ìš°ëŠ” HPë¥¼ MPëŒ€ì‹ ì— ì“´ë‹¤.
    playerMP = g_pPlayer->GetHP();
  }

  if (PlayerCreatureType != CREATURETYPE_BAT &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_GHOST &&
      PlayerCreatureType != CREATURETYPE_WOLF &&
      PlayerCreatureType != CREATURETYPE_WER_WOLF // ê¸´ì˜ë¹ˆì„¸ì½˜
      && PlayerCreatureType != CREATURETYPE_INSTALL_TURRET &&
      PlayerCreatureType != CREATURETYPE_INSTALL_MK1
#if __CONTENTS(__FAST_TRANSFORTER)
      && PlayerCreatureType != CREATURETYPE_FLITTERMOUSE &&
      PlayerCreatureType != CREATURETYPE_NEDE &&
      PlayerCreatureType != CREATURETYPE_KLTL
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
      && PlayerCreatureType != CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
  ) {
    if ((*g_pSkillInfoTable)[SKILL_BLOOD_DRAIN].GetMP() > playerMP) {
      flag = 0;
    } else {
      flag = FLAG_SKILL_ENABLE;
    }
    insert(SKILLID_MAP::value_type(SKILL_BLOOD_DRAIN,
                                   SKILLID_NODE(SKILL_BLOOD_DRAIN, flag)));
  }

  //-----------------------------------------------------
  //
  // invisibleì¸ê°€?
  //
  //-----------------------------------------------------
  if (g_pPlayer->IsInvisible()) {
    flag = FLAG_SKILL_ENABLE;

    insert(SKILLID_MAP::value_type(MAGIC_UN_INVISIBILITY,
                                   SKILLID_NODE(MAGIC_UN_INVISIBILITY, flag)));
  }

  switch (PlayerCreatureType) {
  case CREATURETYPE_WOLF: {
    if ((*g_pSkillInfoTable)[MAGIC_EAT_CORPSE].GetMP() > playerMP) {
      flag = 0;
    } else {
      flag = FLAG_SKILL_ENABLE;
    }
    insert(SKILLID_MAP::value_type(MAGIC_EAT_CORPSE,
                                   SKILLID_NODE(MAGIC_EAT_CORPSE, flag)));

    // ì§–ê¸° - -;
    if (vampireDomain.GetSkillStatus(MAGIC_HOWL) ==
        MSkillDomain::SKILLSTATUS_LEARNED) {
      if ((*g_pSkillInfoTable)[MAGIC_HOWL].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }
      insert(
          SKILLID_MAP::value_type(MAGIC_HOWL, SKILLID_NODE(MAGIC_HOWL, flag)));
    }
  } break;
  case CREATURETYPE_WER_WOLF: // ê¸´ì˜ë¹ˆè±šå†€è³ˆç—°ë¨ì„¸ì½˜
  {
    if ((*g_pSkillInfoTable)[SKILL_BITE_OF_DEATH].GetMP() > playerMP) {
      flag = 0;
    } else {
      flag = FLAG_SKILL_ENABLE;
    }

    insert(SKILLID_MAP::value_type(SKILL_BITE_OF_DEATH,
                                   SKILLID_NODE(SKILL_BITE_OF_DEATH, flag)));

    if (vampireDomain.GetSkillStatus(MAGIC_RAPID_GLIDING) ==
        MSkillDomain::SKILLSTATUS_LEARNED) {
      if ((*g_pSkillInfoTable)[MAGIC_RAPID_GLIDING].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }
      insert(SKILLID_MAP::value_type(MAGIC_RAPID_GLIDING,
                                     SKILLID_NODE(MAGIC_RAPID_GLIDING, flag)));
    }

    if (vampireDomain.GetSkillStatus(SKILL_VIOLENT_PHANTOM) ==
        MSkillDomain::SKILLSTATUS_LEARNED) {
      if ((*g_pSkillInfoTable)[SKILL_VIOLENT_PHANTOM].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }
      insert(SKILLID_MAP::value_type(
          SKILL_VIOLENT_PHANTOM, SKILLID_NODE(SKILL_VIOLENT_PHANTOM, flag)));
    }

    /*		if( vampireDomain.GetSkillStatus( SKILL_LETHAL_CLAW ) ==
       MSkillDomain::SKILLSTATUS_LEARNED )//è—¤ì†161å±¢ç¶¾çž³ë‚šì˜æ¦´æª„è‹Ÿï¼Œby rallser
                    {
                            if( (*g_pSkillInfoTable)[SKILL_LETHAL_CLAW].GetMP()
       > playerMP )
                            {
                                    flag  = 0;
                            }
                            else
                            {
                                    flag = FLAG_SKILL_ENABLE;
                            }
                            insert(SKILLID_MAP::value_type( SKILL_LETHAL_CLAW,
       SKILLID_NODE(SKILL_LETHAL_CLAW, flag) ));
                    }*/			 //MAGIC_BLOODY_ZENITH
    if (vampireDomain.GetSkillStatus(MAGIC_BLOODY_ZENITH) ==
        MSkillDomain::SKILLSTATUS_LEARNED) // è—¤ì†161å±¢ç¶¾çž³ë‚šì˜æ¦´æª„è‹Ÿï¼Œby
                                           // rallser
    {
      if ((*g_pSkillInfoTable)[SKILL_LETHAL_CLAW].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }
      insert(SKILLID_MAP::value_type(MAGIC_BLOODY_ZENITH,
                                     SKILLID_NODE(MAGIC_BLOODY_ZENITH, flag)));
    }
    if (vampireDomain.GetSkillStatus(MAGIC_DARKNESS) ==
        MSkillDomain::SKILLSTATUS_LEARNED) // è—¤ì†161å±¢ç¶¾çž³ë‚šì˜æ¦´æª„è‹Ÿï¼Œby
                                           // rallser
    {
      if ((*g_pSkillInfoTable)[MAGIC_DARKNESS].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }
      insert(SKILLID_MAP::value_type(MAGIC_DARKNESS,
                                     SKILLID_NODE(MAGIC_DARKNESS, flag)));
    }
    if (vampireDomain.GetSkillStatus(SKILL_WILD_WOLF) ==
        MSkillDomain::SKILLSTATUS_LEARNED) {
      if ((*g_pSkillInfoTable)[SKILL_WILD_WOLF].GetMP() > playerMP) {
        flag = 0;
      } else {
        flag = FLAG_SKILL_ENABLE;
      }
      insert(SKILLID_MAP::value_type(SKILL_WILD_WOLF,
                                     SKILLID_NODE(SKILL_WILD_WOLF, flag)));
    }
  } break;
  }

  //-----------------------------------------------------
  //
  // ë³€ì‹  ì¤‘ì¸ê°€?
  //
  //-----------------------------------------------------
  if (PlayerCreatureType != CREATURETYPE_VAMPIRE_MALE1 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_FEMALE1 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_MALE2 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_FEMALE2 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_MALE3 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_FEMALE3 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_MALE4 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_FEMALE4 &&
      PlayerCreatureType != CREATURETYPE_VAMPIRE_OPERATOR) {
    flag = FLAG_SKILL_ENABLE;

    insert(SKILLID_MAP::value_type(MAGIC_UN_TRANSFORM,
                                   SKILLID_NODE(MAGIC_UN_TRANSFORM, flag)));
  }
#endif
}

//----------------------------------------------------------------------
// Check MP
//----------------------------------------------------------------------
// ì„ íƒëœ skillë“¤ì˜ MPë¥¼ ë³´ê³ 
// ì‚¬ìš©ê°€ëŠ¥í•œì§€ ì•„ë‹Œì§€ë¥¼ ì²´í¬í•œë‹¤.
//----------------------------------------------------------------------
void MSkillSet::CheckMP() {
#ifdef __GAME_CLIENT__

  // mp ì²´í¬í• ë•Œ.. í˜„ìž¬ ìž¥ë¹„ì¤‘ì¸ ë¬´ê¸°ë„ ì²´í¬í•´ì•¼ë˜ëŠ”ë°
  // ì¼ë‹¨ì€.. ì´ì¼€ ê°„ë‹¤. T_T;
  SetAvailableSkills();

  /*
  if (g_pPlayer==NULL)
  {
          return;
  }

  //--------------------------------------------------
  // playerì˜ í˜„ìž¬ MP
  //--------------------------------------------------
  int playerMP;

  if (g_pPlayer->IsSlayer())
  {
          playerMP = g_pPlayer->GetMP();
  }
  else
  {
          // vampireì¸ ê²½ìš°ëŠ” HPë¥¼ MPëŒ€ì‹ ì— ì“´ë‹¤.
          playerMP = g_pPlayer->GetHP();
  }

  SKILLID_MAP::iterator iID = begin();

  //--------------------------------------------------
  // ëª¨ë“  skillë“¤ì— ëŒ€í•´ì„œ mp ì²´í¬
  //--------------------------------------------------
  while (iID != end())
  {
          ACTIONINFO		id = (*iID).first;
          SKILLID_NODE&	node = (*iID).second;

          //--------------------------------------------------
          // MPì‚¬ìš©ëŸ‰ì´ í˜„ìž¬MPë³´ë‹¤ í° ê²½ìš°.. --> ì‚¬ìš© ë¶ˆê°€
          //--------------------------------------------------
          if ((*g_pSkillInfoTable)[id].GetMP() > playerMP)
          {
                  node.SetDisable();
          }
          //--------------------------------------------------
          // ì•„ë‹ˆë©´, ì‚¬ìš© ê°€ëŠ¥í•˜ê²Œ í‘œì‹œ
          //--------------------------------------------------
          else
          {
                  node.SetEnable();
          }

          iID++;
  }
  */
#endif
}

//----------------------------------------------------------------------
//
//							MSkillDomain
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MSkillDomain::MSkillDomain() {
  m_bNewSkill = false;

  m_MaxLevel = 0;
  m_MaxLearnedLevel = -1;
  m_pLearnedSkillID = NULL;
  m_DomainLevel = 0;
  m_DomainExpRemain = 0;

  m_DomainExpTable.Init(151);
}

MSkillDomain::~MSkillDomain() {
  if (m_pLearnedSkillID != NULL) {
    delete[] m_pLearnedSkillID;
  }

  //---------------------------------------------------------
  // Skill Step map ì§€ìš°ê¸°
  //---------------------------------------------------------
  SKILL_STEP_MAP::iterator iList = m_mapSkillStep.begin();

  while (iList != m_mapSkillStep.end()) {
    SKILL_STEP_LIST *pList = iList->second;

    if (pList != NULL) {
      delete pList;
    }

    iList++;
  }

  m_mapSkillStep.clear();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set Max Level
//----------------------------------------------------------------------
// domainì˜ ìµœê³  level ê¸°ìˆ 
//----------------------------------------------------------------------
void MSkillDomain::SetMaxLevel() {
  if (m_pLearnedSkillID != NULL) {
    delete[] m_pLearnedSkillID;
  }

  m_pLearnedSkillID = new ACTIONINFO[m_MaxLevel + 1]; // 0ë¶€í„° ì‹œìž‘í•œë‹¤.
  m_MaxLearnedLevel = -1;

  for (int i = 0; i <= m_MaxLevel; i++) {
    m_pLearnedSkillID[i] = MAX_ACTIONINFO; // ACTIONINFO_NULL;
  }
}

//----------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------
void MSkillDomain::Clear() {
  //	SKILLID_MAP::iterator	iSkill = m_mapSkillID.begin();
  //	SKILLID_MAP::iterator	endItr = m_mapSkillID.end();
  //
  //	while (iSkill!=endItr)
  //	{
  //		//-----------------------------------------------
  //		// í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ Skillì—ì„œ ì œê±°í•œë‹¤.
  //		//-----------------------------------------------
  //		(*g_pSkillAvailable).RemoveSkill( (*iSkill).first );
  //
  //		iSkill++;
  //	}
  //
  //	m_iterator = NULL;
  //	m_mapSkillID.clear();

  ClearSkillList();

  //-----------------------------------------------
  // ë°°ìš´ level ì œê±°
  //-----------------------------------------------
  if (m_pLearnedSkillID != NULL) {
    delete[] m_pLearnedSkillID;

    m_MaxLevel = 0;
    m_MaxLearnedLevel = -1;
    m_pLearnedSkillID = NULL;
  }

  // Zone hop'ta Init reuse: destructor'un yaptigi skill-step free'i burada da.
  // Aksi halde SetRootSkill eski listelerin ustune yenilerini yazar / leak.
  {
    SKILL_STEP_MAP::iterator iList = m_mapSkillStep.begin();
    while (iList != m_mapSkillStep.end()) {
      SKILL_STEP_LIST *pList = iList->second;
      if (pList != NULL)
        delete pList;
      iList++;
    }
    m_mapSkillStep.clear();
  }
}

void MSkillDomain::ClearSkillList() {
  SKILLID_MAP::iterator iSkill = m_mapSkillID.begin();
  SKILLID_MAP::iterator endItr = m_mapSkillID.end();

  while (iSkill != endItr) {
    //-----------------------------------------------
    // í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ Skillì—ì„œ ì œê±°í•œë‹¤.
    //-----------------------------------------------
    (*g_pSkillAvailable).RemoveSkill((*iSkill).first);

    iSkill++;
  }

  m_iterator = m_mapSkillID.end();
  m_mapSkillID.clear();

  if (m_pLearnedSkillID != NULL) {
    delete[] m_pLearnedSkillID;
    m_pLearnedSkillID = NULL;
  }
}

//----------------------------------------------------------------------
// AddSkill
//----------------------------------------------------------------------
// idì™€ ê·¸ì˜ í•˜ìœ„ì— ìžˆëŠ” ê²ƒë“¤ì„ ëª¨ë‘ ì¶”ê°€í•œë‹¤.
// ë‹¨ idì˜ skillì˜ levelì€ 0ì´ì–´ì•¼ í•œë‹¤.
//----------------------------------------------------------------------
bool MSkillDomain::SetRootSkill(ACTIONINFO id, bool reset) {
  //--------------------------------------------------
  // ë‹¤ ì§€ìš´ë‹¤.
  //--------------------------------------------------
  // Clear();
  if (reset)
    m_DomainLevel = 0;

  //--------------------------------------------------
  // levelì´ 0ì¸ skillì´ì–´ì•¼ í•œë‹¤.
  //--------------------------------------------------
  //	if ((*g_pSkillInfoTable)[id].GetLevel()==0)
  {
    int oldMaxLevel = m_MaxLevel;

    if (reset)
      m_MaxLevel = 0;
    bool bOK = AddSkill(id);

    if (oldMaxLevel > m_MaxLevel && reset) {
      m_MaxLevel = oldMaxLevel;
    }

    //--------------------------------------------------
    // ì œëŒ€ë¡œ ì¶”ê°€ëœ ê²½ìš°
    //--------------------------------------------------
    if (bOK) {
      //--------------------------------------------------
      // domainì˜ skill levelì„ ì„¤ì •í•´ì¤€ë‹¤.
      //--------------------------------------------------
      SetMaxLevel();
    }
  }

  return false;
}

//----------------------------------------------------------------------
// AddSkill
//----------------------------------------------------------------------
// idì™€ ê·¸ì˜ í•˜ìœ„ì— ìžˆëŠ” ê²ƒë“¤ì„ ëª¨ë‘ ì¶”ê°€í•œë‹¤.
//----------------------------------------------------------------------
bool MSkillDomain::AddSkill(ACTIONINFO id) {
  SKILLID_MAP::iterator iSkill;

  // FIX 2026-05-12: g_pSkillInfoTable NULL + bounds guard.
  // Login öncesi veya skill table init başarısız durumda AddSkill çağrılırsa
  // (*g_pSkillInfoTable)[id].GetLevel() NULL deref / OOB read → crash.
  // MSkillSet::SetAvailableSkills L569-571'de aynı tablo için zaten guard pattern var.
  if (g_pSkillInfoTable == NULL ||
      (size_t)id >= g_pSkillInfoTable->GetSize()) {
    return false;
  }

  if (SKILL_ABERRATION == id)
    int a = 0;

  iSkill = m_mapSkillID.begin();

  while (iSkill != m_mapSkillID.end()) {
    if (iSkill->first == id) {
      break;
    }
    iSkill++;
  }
  int skillLevel = (*g_pSkillInfoTable)[id].GetLevel();

  //--------------------------------------------------
  // domain ìµœê³  skill levelì„ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  if (m_MaxLevel < skillLevel) {
    m_MaxLevel = skillLevel;
  }

  //-----------------------------------------------
  // ì•„ì§ ì—†ëŠ” Skillì´ë©´ ì¶”ê°€
  //-----------------------------------------------
  if (iSkill == m_mapSkillID.end()) {
    //-----------------------------------------------
    // root levelì´ë©´ ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê±¸ë¡œ ì²´í¬í•œë‹¤.
    //-----------------------------------------------
    if (id == SKILL_ABERRATION)
      int a = 0;
    if (skillLevel == 0) {
      m_mapSkillID.insert(SKILLID_MAP::value_type(id, SKILLSTATUS_NEXT));
    } else {
      m_mapSkillID.insert(SKILLID_MAP::value_type(id, SKILLSTATUS_OTHER));
    }

    //-----------------------------------------------
    // ì´ skillì˜ SkillStepì— ê´€í•œ ì •ë³´ë¥¼ ì„¤ì •í•œë‹¤.
    //-----------------------------------------------

    SKILL_STEP skillStep = (*g_pSkillInfoTable)[id].GetSkillStep();
    if (skillStep == SKILL_STEP_ETC && id == SKILL_SOUL_CHAIN) {
#ifdef __GAME_CLIENT__
      if (this == &(*g_pSkillManager)[SKILL_DOMAIN_VAMPIRE])
        AddSkillStep(SKILL_STEP_VAMPIRE_INNATE, id);
      else if (this == &(*g_pSkillManager)[SKILL_DOMAIN_OUSTERS])
        AddSkillStep(SKILL_STEP_OUSTERS_ETC, id);
      else
        AddSkillStep(SKILL_STEP_APPRENTICE, id);
#endif
    } else {
      AddSkillStep(skillStep, id);
    }

    //--------------------------------------------------
    // ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê²ƒë“¤ì„ ì°¾ì•„ì„œ ì¶”ê°€í•œë‹¤.
    //--------------------------------------------------
    const SKILLINFO_NODE::SKILLID_LIST &listNextSkill =
        (*g_pSkillInfoTable)[id].GetNextSkillList();

    SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill =
        listNextSkill.begin();

    while (iNextSkill != listNextSkill.end()) {
      //--------------------------------------------------
      // IDê°€ *iNextSkilì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
      //--------------------------------------------------
      iSkill = m_mapSkillID.find(*iNextSkill);

      //--------------------------------------------------
      // ì•„ì§ ì—†ëŠ” ê²½ìš°ì— ì¶”ê°€í•œë‹¤.
      //--------------------------------------------------
      if (iSkill == m_mapSkillID.end()) {
        AddSkill(*iNextSkill);
      }

      iNextSkill++;
    }

    return true;
  }

  //-----------------------------------------------
  // ì´ë¯¸ ìžˆëŠ” Skillì´ë©´ false
  //-----------------------------------------------
  return false;
}

//----------------------------------------------------------------------
// Set SkillStatus
//----------------------------------------------------------------------
// skillì˜ ìƒíƒœë¥¼ ë³€ê²½í•œë‹¤.
//----------------------------------------------------------------------
/*
bool
MSkillDomain::SetSkillStatus(ACTIONINFO id, SKILLSTATUS status)
{
        SKILLID_MAP::iterator	iSkill;

        iSkill = m_mapSkillID.find( id );

        //-----------------------------------------------
        // domainì— ìžˆëŠ” Skillì´ë©´..
        //-----------------------------------------------
        if (iSkill != m_mapSkillID.end())
        {
                m_mapSkillID.insert(SKILLID_MAP::value_type( id, status ));

                switch (status)
                {
                        case SKILLSTATUS_LEARNED :		// ë°°ì› ë‹¤.
                                //-----------------------------------------------
                                // í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ Skillì— ì¶”ê°€í•œë‹¤.
                                //-----------------------------------------------
                                //(*g_pSkillAvailable).AddSkill( id );
                                LearnSkill(id);
                        break;

                        case SKILLSTATUS_NEXT :				//
ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆë‹¤. case SKILLSTATUS_OTHER :			// ì•„ì§ì€ ë°°ìš¸
ìˆ˜ ì—†ë‹¤
                                //-----------------------------------------------
                                // í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ Skillì—ì„œ ì œê±°í•œë‹¤.
                                //-----------------------------------------------
                                (*g_pSkillAvailable).RemoveSkill( id );
                        break;
                }

                return true;
        }

        //-----------------------------------------------
        // ì´ë¯¸ ìžˆëŠ” Skillì´ë©´ false
        //-----------------------------------------------
        return false;
}
*/

//----------------------------------------------------------------------
// Get SkillStatus
//----------------------------------------------------------------------
// idì˜ ìƒíƒœëŠ”?
//----------------------------------------------------------------------
MSkillDomain::SKILLSTATUS MSkillDomain::GetSkillStatus(ACTIONINFO id) const {
  SKILLID_MAP::const_iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  iSkill = m_mapSkillID.find(id);

  //--------------------------------------------------
  // ì—†ì„ ê²½ìš° NULLì„ returní•œë‹¤.
  //--------------------------------------------------
  if (iSkill == m_mapSkillID.end()) {
    return SKILLSTATUS_NULL;
  }

  //--------------------------------------------------
  // ìžˆìœ¼ë©´ ê·¸ Skillë¥¼ returní•œë‹¤.
  //--------------------------------------------------
  return (*iSkill).second;
}

//----------------------------------------------------------------------
// Remove Skill
//----------------------------------------------------------------------
// mapSkillì—ì„œ ì œê±°í•˜ê³  Skillì˜ pointerë¥¼ ë„˜ê²¨ì¤€ë‹¤.
//----------------------------------------------------------------------
/*
bool
MSkillDomain::RemoveSkill(ACTIONINFO id)
{
        SKILLID_MAP::iterator	iSkill;

        //--------------------------------------------------
        // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
        //--------------------------------------------------
        iSkill = m_mapSkillID.find(id);

        //--------------------------------------------------
        // ê·¸ëŸ° idë¥¼ ê°€ì§„ Skillì´ ì—†ëŠ” ê²½ìš°
        //--------------------------------------------------
        if (iSkill == m_mapSkillID.end())
        {
                return false;
        }

        //--------------------------------------------------
        // ì°¾ì€ ê²½ìš° --> ì œê±°
        //--------------------------------------------------
        // mapì—ì„œ ì œê±°
        m_mapSkillID.erase( iSkill );

        return true;
}
*/

//----------------------------------------------------------------------
// Add NextSkill
//----------------------------------------------------------------------
// ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê¸°ìˆ ë“¤ì„ ì²´í¬í•œë‹¤.
//----------------------------------------------------------------------
void MSkillDomain::AddNextSkill(ACTIONINFO id) {
  const SKILLINFO_NODE::SKILLID_LIST &listNextSkill =
      (*g_pSkillInfoTable)[id].GetNextSkillList();

  SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill =
      listNextSkill.begin();

  while (iNextSkill != listNextSkill.end()) {
    //--------------------------------------------------
    // IDê°€ *iNextSkilì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
    //--------------------------------------------------
    SKILLID_MAP::iterator iSkill = m_mapSkillID.find(*iNextSkill);

    //--------------------------------------------------
    // ìžˆìœ¼ë©´ ê·¸ Skillì˜ ê°’ì„ ë°”ê¾¼ë‹¤.
    //--------------------------------------------------
    // ë¬¼ë¡ , í˜„ìž¬ domainì— ì†í•´ìžˆëŠ” ê²½ìš°ì—ë§Œ ê°€ëŠ¥í•˜ê³ 
    // ì•„ì§ ë°°ìš°ì§€ ì•Šì€ ê²ƒì¼ ê²½ìš°ì—ë§Œ NEXTë¡œ ì„¤ì •í•œë‹¤.
    //--------------------------------------------------
    if (iSkill != m_mapSkillID.end()) {
      if ((*iSkill).second == SKILLSTATUS_OTHER) {
        (*iSkill).second = SKILLSTATUS_NEXT;
      }
    }

    iNextSkill++;
  }
}
void MSkillDomain::AddNextSkillForce(ACTIONINFO id) {
  if (id == MAX_ACTIONINFO) {
    return;
  }
  bool HasChildSkill = false;

  //	const SKILLINFO_NODE::SKILLID_LIST& listNextSkill =
  //(*g_pSkillInfoTable)[id].GetNextSkillList();
  //
  //	SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill =
  // listNextSkill.begin();
  //
  //	while (iNextSkill != listNextSkill.end())
  //	{
  //		//--------------------------------------------------
  //		// IDê°€ *iNextSkilì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //		//--------------------------------------------------
  //		SKILLID_MAP::iterator iSkill = m_mapSkillID.find( *iNextSkill );
  //
  //		//--------------------------------------------------
  //		// ìžˆìœ¼ë©´ ê·¸ Skillì˜ ê°’ì„ ë°”ê¾¼ë‹¤.
  //		//--------------------------------------------------
  //		// ë¬¼ë¡ , í˜„ìž¬ domainì— ì†í•´ìžˆëŠ” ê²½ìš°ì—ë§Œ ê°€ëŠ¥í•˜ê³ 
  //		// ì•„ì§ ë°°ìš°ì§€ ì•Šì€ ê²ƒì¼ ê²½ìš°ì—ë§Œ NEXTë¡œ ì„¤ì •í•œë‹¤.
  //		//--------------------------------------------------
  //		if (iSkill != m_mapSkillID.end())
  //		{
  //			if ((*iSkill).second==SKILLSTATUS_LEARNED)
  //			{
  //				HasChildSkill = true; // ìžê¸° ë°‘ì— ë”¸ë¦° ìŠ¤í‚¬ì¤‘ì—
  // ë°°ìš´ìŠ¤í‚¬ì´ ì—†ì–´ì•¼ í•œë‹¤.
  //			}
  //		}
  //
  //		iNextSkill++;
  //	}
  if (!HasChildSkill &&
      (*g_pSkillInfoTable)[id]
          .CanDelete) // ìžê¸°ë°‘ì— ë”¸ë¦° ìŠ¤í‚¬ì´ ì—†ê³  ì‚­ì œ ê°€ëŠ¥í•œ ìŠ¤í‚¬ì´ë©´
  {
    SKILLID_MAP::iterator iSkill = m_mapSkillID.find(id);
    if (iSkill != m_mapSkillID.end()) {
      if ((*iSkill).second == SKILLSTATUS_LEARNED) {
        (*iSkill).second = SKILLSTATUS_NEXT;
        //	RemoveNextSkill(id);
      }
    }
  }
}

//----------------------------------------------------------------------
// Remove NextSkill
//----------------------------------------------------------------------
// ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆê²Œ í‘œì‹œëœ ê¸°ìˆ ë“¤ì„ ì œê±°í•œë‹¤.
//----------------------------------------------------------------------
void MSkillDomain::RemoveNextSkill(ACTIONINFO id) {
  if (id == MAX_ACTIONINFO) {
    return;
  }

  SKILLID_MAP::iterator iPreviousSkill = m_mapSkillID.find(id);

  // ë°”ë¡œ ì „ì— ë°°ì› ë˜ ê¸°ìˆ ì˜ ID
  ACTIONINFO previousID = (*iPreviousSkill).first;

  //--------------------------------------------------
  //
  // ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê²ƒë“¤ì„ ì°¾ì•„ì„œ ì„¤ì •í•œë‹¤.
  //
  //--------------------------------------------------
  const SKILLINFO_NODE::SKILLID_LIST &listNextSkill =
      (*g_pSkillInfoTable)[previousID].GetNextSkillList();

  SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill =
      listNextSkill.begin();

  while (iNextSkill != listNextSkill.end()) {
    //--------------------------------------------------
    // IDê°€ *iNextSkilì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
    //--------------------------------------------------
    SKILLID_MAP::iterator iSkill = m_mapSkillID.find(*iNextSkill);

    //--------------------------------------------------
    // ìžˆìœ¼ë©´ ê·¸ Skillì˜ ê°’ì„ ë°”ê¾¼ë‹¤.
    //--------------------------------------------------
    // ë¬¼ë¡ , í˜„ìž¬ domainì— ì†í•´ìžˆëŠ” ê²½ìš°ì—ë§Œ ê°€ëŠ¥í•˜ê³ 
    // NEXTì´ë©´ OTHERë¡œ ë°”ê¾¼ë‹¤.
    //--------------------------------------------------
    if (iSkill != m_mapSkillID.end()) {
      if ((*iSkill).second == SKILLSTATUS_NEXT) {
        (*iSkill).second = SKILLSTATUS_OTHER;
      }
    }

    iNextSkill++;
  }
}

//----------------------------------------------------------------------
// Learn Skill
//----------------------------------------------------------------------
// idì˜ skillì„ Learnìƒíƒœë¡œ ë°”ê¾¼ë‹¤.
// ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” skillë“¤ì„ Nextìƒíƒœë¡œ ë°”ê¾¼ë‹¤.
//
// ê°™ì€ levelì˜ ê¸°ìˆ ì€ í•˜ë‚˜ë°–ì— ë°°ìš¸ ìˆ˜ ì—†ë‹¤.
// ê·¸ë ‡ê²Œ ë˜ë©´, í˜„ìž¬ ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê¸°ìˆ ì€ ìµœê³ ì˜ levelì¸ ê¸°ìˆ ì´ë‹¤. (m_MaxLevel)
//----------------------------------------------------------------------
bool MSkillDomain::LearnSkill(ACTIONINFO id) {
  //--------------------------------------------------
  // ìƒˆë¡œìš´ ê¸°ìˆ ì„ ë°°ìš¸ ìˆ˜ ì—†ìœ¼ë©´..
  //--------------------------------------------------
  if (!m_bNewSkill) {
    return false;
  }

  //--------------------------------------------------
  // ì´ë²ˆì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” levelì˜ ê¸°ìˆ ì´ ì•„ë‹ˆë©´ return
  //--------------------------------------------------
  if ((*g_pSkillInfoTable)[id].GetLevel() != m_MaxLearnedLevel + 1) {
    //	return false;
  }

  SKILLID_MAP::iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  iSkill = m_mapSkillID.find(id);

  //--------------------------------------------------
  // ì—†ì„ ê²½ìš° NULLì„ returní•œë‹¤.
  //--------------------------------------------------
  if (iSkill == m_mapSkillID.end()) {
    return false;
  }

  //--------------------------------------------------
  // ì´ë¯¸ ë°°ìš´ ê²ƒì´ë©´ return false
  //--------------------------------------------------
  if ((*iSkill).second == SKILLSTATUS_LEARNED) {
    return false;
  }

  //-----------------------------------------------
  // í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ Skillì— ì¶”ê°€í•œë‹¤.
  //-----------------------------------------------
  (*g_pSkillAvailable).AddSkill(id);

  //-----------------------------------------------
  // ë°°ìš´ ê¸°ìˆ  level ì²´í¬
  //-----------------------------------------------
  int skillLevel = (*g_pSkillInfoTable)[id].GetLevel();

  // í˜„ìž¬ levelì—ì„œ ë°°ìš´ ê¸°ìˆ  ì„¤ì •
  m_pLearnedSkillID[skillLevel] = id;

  if (skillLevel > m_MaxLearnedLevel) {
    m_MaxLearnedLevel = skillLevel;

    //--------------------------------------------------
    //
    // í˜„ìž¬ì— ë°°ìš¸ ìˆ˜ ìžˆê²Œ í‘œì‹œëœ ê²ƒë“¤ì„ ëª¨ë‘ ì—†ì• ì¤€ë‹¤.
    //
    //--------------------------------------------------
    // ë°”ë¡œ ì „ levelì˜ í•˜ìœ„ level
    //--------------------------------------------------
    // if (m_MaxLearnedLevel > 0)
    //{
    /// RemoveNextSkill( m_pLearnedSkillID[m_MaxLearnedLevel-1] );
    //}

    //--------------------------------------------------
    // ë°°ìš¸ ìˆ˜ ìžˆë‹¤ê³  í‘œì‹œëœê±° ëª¨ë‘ ì œê±°
    //--------------------------------------------------
    SKILLID_MAP::iterator iSkill2 = m_mapSkillID.begin();

    while (iSkill2 != m_mapSkillID.end()) {
      if ((*iSkill2).second == SKILLSTATUS_NEXT) {
        (*iSkill2).second = SKILLSTATUS_OTHER;
      }

      iSkill2++;
    }

    //--------------------------------------------------
    // ë°°ì› ë‹¤ê³  ì²´í¬í•œë‹¤.
    //--------------------------------------------------
    (*iSkill).second = SKILLSTATUS_LEARNED;

    //--------------------------------------------------
    //
    // ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê²ƒë“¤ì„ ì°¾ì•„ì„œ ì„¤ì •í•œë‹¤.
    //
    //--------------------------------------------------
    AddNextSkill(id);
  } else {
    //--------------------------------------------------
    // ë°°ì› ë‹¤ê³  ì²´í¬í•œë‹¤.
    //--------------------------------------------------
    (*iSkill).second = SKILLSTATUS_LEARNED;
  }

  m_bNewSkill = false;

  return true;
}

//----------------------------------------------------------------------
// UnLearn Skill
//----------------------------------------------------------------------
// idì˜ skillì„ ì•ˆ ë°°ìš´ ìƒíƒœë¡œ ë°”ê¾¼ë‹¤.
//
// ì œê±°ëœ ê²ƒë“¤ ì¤‘ì—ì„œ ìµœê³  levelì˜ ê¸°ìˆ ì˜
// ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” skillë“¤ì„ Nextìƒíƒœë¡œ ë°”ê¾¼ë‹¤.
//
//----------------------------------------------------------------------
bool MSkillDomain::UnLearnSkill(ACTIONINFO id) {
  //--------------------------------------------------
  // í˜„ìž¬ ìµœê³  levelì˜ ê¸°ìˆ ë§Œ ì œê±°í•  ìˆ˜ ìžˆë‹¤.
  //--------------------------------------------------
  if ((*g_pSkillInfoTable)[id].GetLevel() != m_MaxLearnedLevel) {
    return false;
  }

  SKILLID_MAP::iterator iSkill;

  //--------------------------------------------------
  // IDê°€ idì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
  //--------------------------------------------------
  iSkill = m_mapSkillID.find(id);

  //--------------------------------------------------
  // ì—†ì„ ê²½ìš° NULLì„ returní•œë‹¤.
  //--------------------------------------------------
  if (iSkill == m_mapSkillID.end()) {
    return false;
  }

  //--------------------------------------------------
  //
  // Skillì˜ ê°’ì„ UnLearnedë¡œ ë°”ê¾¼ë‹¤.
  //
  //--------------------------------------------------
  // ë°°ìš´ ê²ƒì´ë©´ ì•„ë‹ˆë©´ return false
  if ((*iSkill).second != SKILLSTATUS_LEARNED) {
    return false;
  }

  //--------------------------------------------------
  // ì œê±°í• ë ¤ëŠ” ê¸°ìˆ ì˜ ë‹¤ìŒ ê¸°ìˆ ë“¤ì„
  // ëª» ë°°ìš°ëŠ” ê±¸ë¡œ ì²´ í¬í•œë‹¤.
  //--------------------------------------------------
  RemoveNextSkill(m_pLearnedSkillID[m_MaxLearnedLevel]);

  //--------------------------------------------------
  // ê¸°ìˆ  level ì—†ì• ì£¼ê¸°
  //--------------------------------------------------
  m_pLearnedSkillID[m_MaxLearnedLevel] = MAX_ACTIONINFO; // ACTIONINFO_NULL;
  m_MaxLearnedLevel--;

  (*iSkill).second = SKILLSTATUS_OTHER; // ì‹¤ì œë¡œëŠ” NEXTSKILLì´ì§€ë§Œ..ë¨¸..

  //-----------------------------------------------
  // í˜„ìž¬ ì‚¬ìš© ê°€ëŠ¥í•œ Skillì—ì„œ ì œê±°í•œë‹¤.
  //-----------------------------------------------
  (*g_pSkillAvailable).RemoveSkill(id);

  //--------------------------------------------------
  //
  // ë‹¤ìŒì— ë°°ìš¸ ìˆ˜ ìžˆëŠ” ê²ƒë“¤ì„ ì°¾ì•„ì„œ ì„¤ì •í•œë‹¤.
  //
  //--------------------------------------------------
  if (m_MaxLearnedLevel >= 0) {
    AddNextSkill(m_pLearnedSkillID[m_MaxLearnedLevel]);
  }

  return true;
}

//----------------------------------------------------------------------
// Is Exist SkillStep
//----------------------------------------------------------------------
BOOL MSkillDomain::IsExistSkillStep(SKILL_STEP ss) const {
  SKILL_STEP_MAP::const_iterator iList = m_mapSkillStep.find(ss);

  if (iList == m_mapSkillStep.end()) {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------
// Get SkillStep List
//----------------------------------------------------------------------
const MSkillDomain::SKILL_STEP_LIST *
MSkillDomain::GetSkillStepList(SKILL_STEP ss) const {
  SKILL_STEP_MAP::const_iterator iList = m_mapSkillStep.find(ss);

  if (iList == m_mapSkillStep.end()) {
    return NULL;
  }

  return iList->second;
}

//----------------------------------------------------------------------
// Add SkillStep List
//----------------------------------------------------------------------
void MSkillDomain::AddSkillStep(SKILL_STEP ss, ACTIONINFO ai) {
  SKILL_STEP_LIST *pList;
  SKILL_STEP_MAP::const_iterator iList = m_mapSkillStep.find(ss);

  if (iList == m_mapSkillStep.end()) {
    // ì—†ìœ¼ë©´ newí•´ì„œ ì¶”ê°€í•œë‹¤.
    pList = new SKILL_STEP_LIST;
  } else {
    pList = iList->second;
  }

  // listì— aiì¶”ê°€
  SKILL_STEP_LIST list = *pList;
  bool bExist = false;
  for (int i = 0; i < list.size(); i++) {
    if (list[i] == ai) {
      bExist = true;
      break;
    }
  }

  if (bExist == false)
  //		pList->push_back( ai );
  {

    if (pList->empty()) {
      pList->push_back(ai);
    } else {
      bool bAdded = false;
      SKILL_STEP_LIST::iterator itr = pList->begin();
      SKILL_STEP_LIST::iterator endItr = pList->end();
      int LearnLevel = (*g_pSkillInfoTable)[ai].GetLearnLevel();
      while (itr != endItr) {
        int currentPositionLevel = (*g_pSkillInfoTable)[*itr].GetLearnLevel();

        if (currentPositionLevel > LearnLevel) {
          pList->insert(itr, ai);
          bAdded = true;
          break;
        }

        itr++;
      }
      if (bAdded == false) {
        pList->push_back(ai);
      }
    }
  }

  // (ë‹¤ì‹œ) ì„¤ì •í•œë‹¤.
  m_mapSkillStep[ss] = pList;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
// Skill IDë¥¼ Fileì— ì €ìž¥í•œë‹¤.
//----------------------------------------------------------------------
void MSkillDomain::SaveToFile(std::ofstream &file) {
  SKILLID_MAP::iterator iSkill = m_mapSkillID.begin();

  // sizeì €ìž¥
  int size = m_mapSkillID.size();
  file.write((const char *)&size, 4);

  // ê° idì €ìž¥
  while (iSkill != m_mapSkillID.end()) {
    WORD id = (*iSkill).first;
    BYTE status = (*iSkill).second;

    file.write((const char *)&id, 2);
    file.write((const char *)&status, 1);

    iSkill++;
  }
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
// Skill IDë¥¼ Fileì—ì„œ ì½ì–´ì˜¨ë‹¤.
//----------------------------------------------------------------------
void MSkillDomain::LoadFromFile(ivfstream &file) {
  Clear();
  // m_mapSkillID.clear();

  // sizeì½ì–´ì˜¤ê¸°
  int size;
  file.read((char *)&size, 4);

  // ì½ì–´ì™€ì„œ ì €ìž¥
  WORD id;
  BYTE status;
  for (int i = 0; i < size; i++) {
    file.read((char *)&id, 2);
    file.read((char *)&status, 1);

    m_mapSkillID.insert(
        SKILLID_MAP::value_type((enum ACTIONINFO)id, (enum SKILLSTATUS)status));
  }
}

//----------------------------------------------------------------------
// LoadFromFileServerDomainInfo
//----------------------------------------------------------------------
void MSkillDomain::LoadFromFileServerDomainInfo(ivfstream &file) {
  int level;

  file.read((char *)&level, 4);
  // 2026-05-18 CRASH DEBUG: log level value for first/last/OUSTERS-range entries
  static int _debug_call_count = 0;
  _debug_call_count++;
  if (_debug_call_count < 20 || _debug_call_count > 1100 || _debug_call_count % 200 == 0)
    CT_Done("  MSkillDomain::Load call=%d: level=%d (about to call m_DomainExpTable[level].LoadFromFile)", _debug_call_count, level);

  // 2026-05-18: Phoenix [Antigravity] silent-return guard kaldirildi — kural 8
  // referans VS6 davranisina donus, kural 9 silent-return YASAK. Eski guard:
  //   if (level < 0 || level >= 151)
  //     return;   // See MSkillDomain::MSkillDomain() m_DomainExpTable.Init(151)
  // Sebep: m_DomainExpTable boyutu MSkillDomain::MSkillDomain()'da 151 init
  // edildigi icin valid INF (Level 0-150) zaten OOB yapmaz. Corrupt INF OOB
  // crash = referans fail-fast davranisi (intentional). Silent fail Domain UI
  // bozuyordu.

  // levelì— ë§žì¶°ì„œ loading..
  m_DomainExpTable[level].LoadFromFile(file);
}

bool MSkillDomain::IsAvailableDeleteSkill(ACTIONINFO id) {
  const SKILLINFO_NODE::SKILLID_LIST &listNextSkill =
      (*g_pSkillInfoTable)[id].GetNextSkillList();

  SKILLINFO_NODE::SKILLID_LIST::const_iterator iNextSkill =
      listNextSkill.begin();

  while (iNextSkill != listNextSkill.end()) {
    //--------------------------------------------------
    // IDê°€ *iNextSkilì¸ Skillë¥¼ ì°¾ëŠ”ë‹¤.
    //--------------------------------------------------
    int TempSkillID = *iNextSkill;
    if (false == (*g_pSkillInfoTable)[*iNextSkill].CanDelete)
      return false;
    SKILLID_MAP::iterator iSkill = m_mapSkillID.find(*iNextSkill);

    if (iSkill != m_mapSkillID.end()) {
      if ((*iSkill).second == SKILLSTATUS_LEARNED) {
        return false;
      }
    }

    iNextSkill++;
  }
  return true;
}

//----------------------------------------------------------------------
// Get ExpInfo
//----------------------------------------------------------------------
const ExpInfo &MSkillDomain::GetExpInfo(int level) const {
  return m_DomainExpTable[level];
}

//----------------------------------------------------------------------
//
// MSkillManager
//
//----------------------------------------------------------------------
MSkillManager::MSkillManager() {}

MSkillManager::~MSkillManager() {}

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void MSkillManager::Init() {
    CT_Done("MSkillManager::Init - RE-ACTIVATED");
    CT_Done("MSkillManager::Init - Entry");
    CT_Done("MSkillManager::Init - MAX_SKILLDOMAIN = %d", MAX_SKILLDOMAIN);
  // 2026-08-26: Her GCUpdateInfo'da CTypeTable::Init = delete[] + new MSkillDomain[8].
  // Zone hop sonrasi availVirt ~9MB / fragmentation iken new[] bad_alloc → MODE_QUIT
  // (LoadZone basarili olsa bile). Domain tablosu zaten dogru boyuttaysa reuse et.
  try {
    if (GetSize() == MAX_SKILLDOMAIN && m_pTypeInfo != NULL) {
      MEMORYSTATUSEX ms{};
      ms.dwLength = sizeof(ms);
      GlobalMemoryStatusEx(&ms);
      // Progress SPK leak duzeltilse bile fragmentation'da Clear+SetRootSkill
      // map node alloc bad_alloc verebilir. Tree zaten ayakta — skip.
      if (ms.ullAvailVirtual < (48ULL * 1024ULL * 1024ULL)) {
        CT_Done("MSkillManager::Init - skip rebuild availVirt low");
        return;
      }
      for (int i = 0; i < MAX_SKILLDOMAIN; i++)
        m_pTypeInfo[i].Clear();
      CT_Done("MSkillManager::Init - domain reuse Clear PASS");
    } else {
      CTypeTable<MSkillDomain>::Init(MAX_SKILLDOMAIN);
      CT_Done("MSkillManager::Init - CTypeTable Init PASS");
    }
  } catch (...) {
    CT_Done("MSkillManager::Init - FATAL EXCEPTION in domain Init/Clear");
    throw;
  }
  //--------------------------------------------------
  // ê¸°ë³¸ ê¸°ìˆ ë¡œë¶€í„° skill treeë¥¼ ì´ˆê¸°í™”í•œë‹¤.
  //--------------------------------------------------

  m_pTypeInfo[SKILLDOMAIN_BLADE].SetRootSkill(SKILL_SINGLE_BLOW);
  m_pTypeInfo[SKILLDOMAIN_SWORD].SetRootSkill(SKILL_DOUBLE_IMPACT);
  m_pTypeInfo[SKILLDOMAIN_GUN].SetRootSkill(SKILL_FAST_RELOAD);
  m_pTypeInfo[SKILLDOMAIN_ENCHANT].SetRootSkill(MAGIC_CREATE_HOLY_WATER);
  m_pTypeInfo[SKILLDOMAIN_HEAL].SetRootSkill(MAGIC_CURE_LIGHT_WOUNDS);
  m_pTypeInfo[SKILLDOMAIN_VAMPIRE].SetRootSkill(MAGIC_HIDE);
  m_pTypeInfo[SKILLDOMAIN_OUSTERS].SetRootSkill(SKILL_FLOURISH);
  m_pTypeInfo[SKILLDOMAIN_ETC].SetRootSkill(SKILL_SOUL_CHAIN);

#ifdef __GAME_CLIENT__
  //------------------------------------------------
  // Server ì •ë³´ë¥¼ loadingí•œë‹¤.
  //------------------------------------------------
  // 2026-05-18 ROOT CAUSE FIX: Dangling pointer bug.
  // Eski kod (BUGGY):
  //   const char* path = g_pFileDef->getProperty("...").c_str();
  // `getProperty` returns std::string BY VALUE -> temporary destroyed at
  // semicolon -> `path` dangling pointer. CT_Done sometimes shows correct
  // string (luck, memory not yet overwritten), but ivfstream constructor
  // gets reused memory -> reads from WRONG file (e.g., resolution.inf
  // happens to land in same memory slot due to fprintf buffer reuse).
  //
  // Symptom: num=1869833554 = "Reso" ASCII bytes, domain=1769239916 =
  // "luti" ASCII, level=978873967 = "onX:" ASCII -> "ResolutionX:" text
  // content from data/info/resolution.inf was being read instead of
  // binary domainexp.inf. Crash because text bytes interpreted as int
  // produce wild OOB array index.
  //
  // FIX: Store getProperty return as std::string (value), then use
  // .c_str() within same scope.
  std::string path = g_pFileDef->getProperty("FILE_INFO_SKILL_DOMAIN_EXP");
  CT_Done("MSkillManager::Init - Loading: %s", path.c_str());
  try {
    ivfstream serverDomainInfoFile(path.c_str(), std::ios_base::binary);
    if (!serverDomainInfoFile.is_open()) {
      CT_Done("MSkillManager::Init - FAILED to open: %s", path.c_str());
      return;
    }
    try {
      LoadFromFileServerDomainInfo(serverDomainInfoFile);
    } catch (...) {
      CT_Done("DEBUG: EXCEPTION in LoadFromFileServerDomainInfo - SILENTLY RECOVERING");
    }
    serverDomainInfoFile.close();
  } catch (const std::bad_alloc &) {
    CT_Done("MSkillManager::Init - bad_alloc opening domain exp");
    throw;
  }
#endif
}

void MSkillManager::InitSkillList() {
  for (int i = SKILLDOMAIN_BLADE; i < MAX_SKILLDOMAIN; i++)
    m_pTypeInfo[i].ClearSkillList();

  m_pTypeInfo[SKILLDOMAIN_BLADE].SetRootSkill(SKILL_SINGLE_BLOW, false);
  m_pTypeInfo[SKILLDOMAIN_SWORD].SetRootSkill(SKILL_DOUBLE_IMPACT, false);
  m_pTypeInfo[SKILLDOMAIN_GUN].SetRootSkill(SKILL_FAST_RELOAD, false);
  m_pTypeInfo[SKILLDOMAIN_ENCHANT].SetRootSkill(MAGIC_CREATE_HOLY_WATER, false);
  m_pTypeInfo[SKILLDOMAIN_HEAL].SetRootSkill(MAGIC_CURE_LIGHT_WOUNDS, false);
  m_pTypeInfo[SKILLDOMAIN_VAMPIRE].SetRootSkill(MAGIC_HIDE, false);
  m_pTypeInfo[SKILLDOMAIN_OUSTERS].SetRootSkill(SKILL_FLOURISH, false);
  m_pTypeInfo[SKILLDOMAIN_ETC].SetRootSkill(SKILL_SOUL_CHAIN, false);
}

//----------------------------------------------------------------------
// LoadFromFileServerSkillInfo
//----------------------------------------------------------------------
void MSkillManager::LoadFromFileServerDomainInfo(ivfstream &file) {
  int num, domain;

  char fc; file.read(&fc, 1); file.seekg(-1, std::ios_base::cur); if (fc == '[') { CT_Done("DEBUG: SkillInfo TEXT Bypass ACTIVE"); return; } file.read((char *)&num, 4);

  // 2026-05-18: Phoenix [Antigravity] silent-return guard kaldirildi —
  // kural 8 referans VS6 implementasyona donus, kural 9 silent-return YASAK.
  //
  // KOK NEDEN: Bu guard "sanity check" amacli eklenmis ama threshold yanlis
  // kalibre (num > 1000). Gercek INF 1006-1156 satir, guard valid INF'i
  // silent reject ediyor → Domain UI bozuk. Symptom: "EXP bar bozuk"
  // (kullanici 2026-05-17 raporu — ROOT CAUSE bu).
  //
  // Eski guard (yorum'da, kural 4.1 silme yok):
  //   if (num < 0 || num > 1000) return;
  //
  // Referans VS6 davranisi: hicbir guard yok. Corrupt INF → OOB crash
  // intentional fail-fast. Phoenix tool ile yonetilen INF dosyalari
  // corrupt olma riski dusuk.

  // 2026-05-18 CRASH DEBUG: Domain Exp load post-sync crash diagnostic.
  // CT_DONE_ENABLED=1 kullanici build sirasinda. Fix sonrasi 0'a geri (kural 9.4).
  CT_Done("LoadFromFileServerDomainInfo entry: num=%d", num);

  // ê°œìˆ˜ë§Œí¼..
  for (int i = 0; i < num; i++) {
    // 2026-05-18 CRASH DEBUG instrumentation (kural 9.3 edge-of-context).
    // Log first 10, every 100, last 50, plus iter 1000-1020 (OUSTERS start).
    bool log_iter = (i < 10) || (i >= num - 50) || (i % 100 == 0)
                    || (i >= 1000 && i <= 1020);
    if (log_iter) CT_Done("loop iter=%d: about to read domain (filepos~%d)", i, 4 + i*16);
    file.read((char *)&domain, 4);
    if (log_iter) CT_Done("loop iter=%d: domain=%d (read OK)", i, domain);

    // 2026-05-18: Phoenix [Antigravity] silent-return guard kaldirildi —
    // referans VS6 davranisi (kural 8). Eski guard (yorum'da, kural 4.1):
    //   if (domain < 0 || domain >= MAX_SKILLDOMAIN) return;
    // MAX_SKILLDOMAIN=8, valid DomainType 0-7, OOB array access referans
    // davranisi (intentional fail-fast). Tool-yonetilen INF'lerde corrupt
    // domain bekleyemeyiz.

    // domainì— ë§žì¶°ì„œ loadingí•œë‹¤.
    if (log_iter) CT_Done("loop iter=%d: about to call m_pTypeInfo[%d].Load (deref!)", i, domain);
    m_pTypeInfo[domain].LoadFromFileServerDomainInfo(file);
    if (log_iter) CT_Done("loop iter=%d: m_pTypeInfo[%d].Load returned OK", i, domain);
  }
  CT_Done("LoadFromFileServerDomainInfo exit OK (loaded %d entries)", num);
}

