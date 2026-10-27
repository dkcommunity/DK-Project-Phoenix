//----------------------------------------------------------------------
// MCreature.h
//----------------------------------------------------------------------
// ¿òÁ÷ÀÌ´Â »ý¹°¿¡ ´ëÇÑ class
//----------------------------------------------------------------------
// UndergroundCreature
// GroundCreature
// FlyingCreature ·Î ±¸ºÐµÈ´Ù.
//
// ¸ðµÎ ZÁÂÇ¥¸¦ °®°í ÀÖÁö¸¸,
// FlyingCreature¸¸ÀÌ ÀÏÁ¤ÇÑ ³ôÀÌ¿¡ Á¸ÀçÇÏ°Ô µÈ´Ù.
//----------------------------------------------------------------------

#ifndef __MCREATURE_H__
#define __MCREATURE_H__

#include "EffectSpriteTypeDef.h"
#include "MActionInfoTable.h"
#include "MAttachEffect.h"
#include "MCreatureSpriteTable.h"
#include "MCreatureTable.h"
#include "MEffectStatusTable.h"
#include "MObject.h"
#include "MStatus.h"
#include "MTypeDef.h"
#include "MemoryPool.h"
#include "NicknameInfo.h"
#include "RaceType.h"
#include "SkillDef.h"
#include "TickTimer.h"

//----------------------------------------------------------------------
// define
//----------------------------------------------------------------------
#define MAX_FRAME_MOVE 16

// °øÁß Ä³¸¯ÅÍÀÇ ³ôÀÌ(ÀÏÁ¤)
// #define	FLYINGCREATURE_HEIGHT	0	//64

class MItem;
class MZone;
class MActionFrameArray;
class MActionResult;

class MCreature : public MObject, public MStatus {
  //--------------------------------------------------------------
  // enum
  //--------------------------------------------------------------
public:
  enum CLASS_TYPE {
    CLASS_CREATURE,
    CLASS_CREATUREWEAR,
    CLASS_NPC,
    CLASS_FAKE,
    CLASS_PLAYER
  };

  // CreatureÀÇ Á¾·ù : ÁöÇÏ,Áö»ó,°øÁß
  enum MOVE_TYPE {
    CREATURE_UNDERGROUND = 0,
    CREATURE_GROUND,
    CREATURE_FLYING,
    CREATURE_FAKE_NO_BLOCK, // sector¿¡ Á¸ÀçÇÏÁö ¾Ê´Â ¾Ö´ú..
    CREATURE_FAKE_UNDERGROUND,
    CREATURE_FAKE_GROUND,
    CREATURE_FAKE_FLYING,
  };

  // ÀÌµ¿ÇÏ´Â ¹æ¹ý
  enum MOVE_DEVICE {
    MOVE_DEVICE_WALK,         // °É¾î´Ù´Ñ´Ù.
    MOVE_DEVICE_RIDE,         // ¸ðÅÍ½ÎÀÌÅ¬
    MOVE_DEVICE_SUMMON_SYLPH, // Á¤·ÉÅ¸°í ½µ~
    MOVE_DEVICE_WING_SYLPH,
    MOVE_DEVICE_MAX
  };

  // ÀÌµ¿ÇÏ´Â ¹æ¹ý
  enum WEAPON_SPEED {
    WEAPON_SPEED_SLOW,   // ´À¸° ¹«±â
    WEAPON_SPEED_NORMAL, // º¸Åë ¹«±â
    WEAPON_SPEED_FAST    // ºü¸¥ ¹«±â
  };

  enum { PERSNALSHOPCOLOR_MAX = 8 };
  static const COLORREF s_PersnalShopColor[PERSNALSHOPCOLOR_MAX];

public:
  //----------------------------------------------------------------------
  // MoveNode
  //----------------------------------------------------------------------
  // (x,y)¿¡¼­ direction¹æÇâÀ¸·Î ¿òÁ÷ÀÎ´Ù´Â ÀÇ¹Ì
  class MoveNode {
  public:
    int x;
    int y;
    int direction;
  };

  typedef std::list<MoveNode *> MOVENODE_LIST;

  //----------------------------------------------------------------------
  // HPModifyList
  //----------------------------------------------------------------------
  class HPModify {
  public:
    HPModify(int value_modify, DWORD value_TickCount) {
      modify = value_modify;
      TickCount = value_TickCount;
    }
    int modify;
    DWORD TickCount;
  };

  typedef std::list<HPModify> HPMODIFYLIST;

public:
  MCreature(); // MActionFrameArray* pFrames);
  virtual ~MCreature();

  void *operator new(size_t size) { return g_CreatureMemoryPool.Alloc(); }

  void operator delete(void *pmem) { g_CreatureMemoryPool.Free(pmem); }

  virtual CLASS_TYPE GetClassType() const { return CLASS_CREATURE; }

  //------------------------------------------------------
  // Init
  //------------------------------------------------------
  static void InitMoveTable();
  static void ReleaseMoveTable();

  //------------------------------------------------------
  // (x,y)¿¡¼­ directionÀ¸·Î ¿òÁ÷ÀÎ ÁÂÇ¥¸¦ ³Ñ°ÜÁØ´Ù.
  //------------------------------------------------------
  static void GetPositionToDirection(TYPE_SECTORPOSITION &x,
                                     TYPE_SECTORPOSITION &y, BYTE direction);

  int GetHeadSkin() const;
  //-------------------------------------------------------
  // Creature Type
  //-------------------------------------------------------
  virtual void SetCreatureType(TYPE_CREATURETYPE type);
  TYPE_CREATURETYPE GetCreatureType() const { return m_CreatureType; }

  CREATURETRIBE GetCreatureTribe() const {
    return (*g_pCreatureTable)[m_CreatureType].GetCreatureTribe();
  }
  bool IsPlayer() const {
    return (*g_pCreatureSpriteTable)[(*g_pCreatureTable)[m_CreatureType]
                                         .SpriteTypes[m_current_sprite_index]]
               .IsPlayerSprite() != false;
  }
  bool IsPlayerOnly() const {
    return (*g_pCreatureSpriteTable)[(*g_pCreatureTable)[m_CreatureType]
                                         .SpriteTypes[m_current_sprite_index]]
               .IsPlayerOnlySprite() != false;
  }
  bool IsSlayer() const {
    return (*g_pCreatureTable)[m_CreatureType].IsSlayer();
  }
  bool IsVampire() const {
    return (*g_pCreatureTable)[m_CreatureType].IsVampire();
  }
  bool IsOusters() const {
    return (*g_pCreatureTable)[m_CreatureType].IsOusters();
  }
  bool IsNPC() const { return (*g_pCreatureTable)[m_CreatureType].IsNPC(); }
  int GetActionMax() const;
  bool IsSlayerCharacter() const;
  bool IsVampireCharacter() const;
  Race GetRace() const {
    if (IsSlayer())
      return RACE_SLAYER;
    else if (IsVampire())
      return RACE_VAMPIRE;
    return RACE_OUSTERS;
  }

  int GetLevel() const { return (*g_pCreatureTable)[m_CreatureType].Level; }

  // 2004, 12, 7, sobeit add start - ¿î¿µÀÚ ÀÎÁö ¾Æ´ÑÁö
  bool IsSlayerOperator() const {
    return m_CreatureType == CREATURETYPE_SLAYER_OPERATOR;
  }
  bool IsVampireOperator() const {
    return m_CreatureType == CREATURETYPE_VAMPIRE_OPERATOR;
  }
  bool IsOustersOperator() const {
    return m_CreatureType == CREATURETYPE_OUSTERS_OPERATOR;
  }
  bool IsOperator() const {
    return (m_CreatureType == CREATURETYPE_SLAYER_OPERATOR ||
            m_CreatureType == CREATURETYPE_VAMPIRE_OPERATOR ||
            m_CreatureType == CREATURETYPE_OUSTERS_OPERATOR);
  }
  bool IsRide_Fast_Transforter();
  bool IsRide_Air_Transporter();
  TYPE_SOUNDID GetRideMotorcycleSoundID(MOTORCYCLE_SOUND_KIND kind) const;
  // ÀÌ¸§À» Ãâ·ÂÇØ¾ßÇÏ´Â Creature
  bool IsShowNameCreature() const;
  // 2004, 12, 7, sobeit add end
  //------------------------------------------------------
  // IndexSetÀ» Á¤ÇØÁØ´Ù.
  //------------------------------------------------------
  int GetCreatureColorSet() const {
    return (*g_pCreatureTable)[m_CreatureType].ColorSet;
  }
  int GetItemOptionExtColorSet(unsigned short srcColor);
  //------------------------------------------------------
  // Set Same Body
  //------------------------------------------------------
  void SetSameBody(const MCreature *pCreature);

  //------------------------------------------------------
  // »ö±ò °ü·Ã...
  //------------------------------------------------------
  void SetBodyColor1(WORD set);
  void SetBodyColor2(WORD set);
  void SetChangeColorSet(WORD set) { m_ChangeColorSet = set; }
  WORD GetBodyColor1() const { return m_ColorBody1; }
  WORD GetBodyColor2() const { return m_ColorBody2; }
  WORD GetChangeColorSet() const { return m_ChangeColorSet; }

  // Vampire Bat
  void SetBatColor(WORD set) { m_BatColorSet = set; }
  WORD GetBatColor() const { return m_BatColorSet; }

  void SetAdvanceBatColor(WORD set) { m_AdvanceBatColorSet = set; }
  void SetAdvanceBatEffect();
  // 20090325 Hellbiketest
  void SetHellBikeEffect(WORD set);

#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)

  void SetWingType(BYTE set) { m_WingType = set; }
  BYTE GetWingType() const { return m_WingType; }

  void SetWingColor(WORD set) { m_WingColorSet = set; }
  WORD GetWingColor() const { return m_WingColorSet; }

  void SetWingEffectColor(WORD set) { m_WingEffectColorSet = set; }
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
#if __CONTENTS(__FAST_TRANSFORTER)
  void SetWingEffect1(); // ÀÌµ¿¼ö´Ü 1Â÷
#endif                   //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
  void SetWingItemType(BYTE set) { m_WingItemType = set; }
  BYTE GetWingItemType() const { return m_WingItemType; }

  void SetWingEffect2(); // ÀÌµ¿¼ö´Ü 2Â÷
  void SetWingEffect3(WORD type);
#endif //__SECOND_TRANSFORTER
  void SetFastTransforterEffect(WORD transforterEffect);

  // by diesirace 20070312
  static int Wisp_Startcolor_Kind(int status);
  static int Wisp_Endcolor_Kind(int status);
  static int Wisp_Sylph_Loopcolor_Kind(int status);

#if __CONTENTS(__FAST_TRANSFORTER)
  static int Seragon_Color_Kind(int status);
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
  static int Unicorn_Color_Kind(int status);
#endif //__SECOND_TRANSFORTER

  void SetAlpha(BOOL bAlpha) { m_bAlpha = bAlpha; }
  BOOL GetAlpha() { return m_bAlpha; }

  void SetAlphaShadow(BOOL bAlphaShadow) { m_bAlphaShadow = bAlphaShadow; }
  BOOL GetAlphaShdow() { return m_bAlphaShadow; }

  // 2005, 2, 21, sobeit add start
  // ÀÌ ¸¶½ºÅÍ Å¸ÀÔÀº ¸¶ÄÏ Å¸ÀÔÀ» ¸»ÇÑ´Ù.
  void SetMasterEffectType(WORD bType) { m_MasterEffectType = bType; }
  WORD GetMasterEffectType() const { return m_MasterEffectType; }
  // 2005, 2, 21, sobeit add end
  //------------------------------------------------------
  // Move Type
  //------------------------------------------------------
  MOVE_TYPE GetMoveType() const { return m_MoveType; }
  void SetMoveType(MOVE_TYPE moveType) { m_MoveType = moveType; }
  void SetUndergroundCreature();
  void SetGroundCreature();
  void SetFlyingCreature();
  void SetFakeCreature() {
    m_MoveType = CREATURE_FAKE_NO_BLOCK;
    // FAZ 4 (2026-05-03): bounds guard (Empty global table OOB pattern)
    if (g_pCreatureTable != NULL
        && m_CreatureType >= 0
        && m_CreatureType < g_pCreatureTable->GetSize()) {
      m_Z = (*g_pCreatureTable)[m_CreatureType].FlyingHeight;
    } else {
      m_Z = 0;
    }
  }
  bool IsUndergroundCreature() const {
    return m_MoveType == CREATURE_UNDERGROUND ||
           m_MoveType == CREATURE_FAKE_UNDERGROUND;
  }
  bool IsGroundCreature() const {
    return m_MoveType == CREATURE_GROUND || m_MoveType == CREATURE_FAKE_GROUND;
  }
  bool IsFlyingCreature() const {
    return m_MoveType == CREATURE_FLYING || m_MoveType == CREATURE_FAKE_FLYING;
  }
  bool IsFakeCreature() const {
    return m_MoveType == CREATURE_FAKE_NO_BLOCK ||
           m_MoveType == CREATURE_FAKE_UNDERGROUND ||
           m_MoveType == CREATURE_FAKE_GROUND ||
           m_MoveType == CREATURE_FAKE_FLYING;
  }

  //------------------------------------------------------
  // ¼ºº°
  //------------------------------------------------------
  // bool	IsMale() const					{ return
  // (*g_pCreatureTable)[m_CreatureType].bMale; } bool	IsFemale() const
  // { return !(*g_pCreatureTable)[m_CreatureType].bMale; }
  void SetMale(bool bMale = true) { m_bMale = bMale; }
  void SetFemale() { m_bMale = false; }
  bool IsMale() const { return m_bMale; }
  bool IsFemale() const { return !m_bMale; }

  //------------------------------------------------------
  // Å° / ³Êºñ
  //------------------------------------------------------
  int GetHeight() const {
    if (m_bAlive)
      return (*g_pCreatureTable)[m_CreatureType].Height;
    return (*g_pCreatureTable)[m_CreatureType].DeadHeight;
  }
  int GetWidth() const { return (*g_pCreatureTable)[m_CreatureType].Width; }

  //------------------------------------------------------
  // ÀÌµ¿ÇÏ´Â ¹æ¹ý
  //------------------------------------------------------
  void SetMoveDevice(MOVE_DEVICE md);
  MOVE_DEVICE GetMoveDevice() const { return m_MoveDevice; }

  //------------------------------------------------------
  // ¿òÁ÷ÀÌ±â
  //------------------------------------------------------
  virtual void Action(); // ÇöÀç Çàµ¿À» ÃëÇÏµµ·Ï ÇÑ´Ù.
  virtual BOOL IsStop();
  virtual void SetStop();

  //------------------------------------------------------
  // BufferingµÈ ´ÙÀ½ÀÇ ÀÌµ¿ Á¤º¸¸¦ °¡Á®¿Â´Ù.
  //------------------------------------------------------
  virtual bool AffectMoveBuffer();
  virtual void AffectMoveBufferAll();
  void ReleaseMoveBuffer();

  //----------------------------------------------------------
  // »óÅÂ °ª ¹Ù²Ù±â
  //----------------------------------------------------------
  virtual void SetStatus(DWORD n, DWORD value);

  //----------------------------------------------------------
  // º¯½Å
  //----------------------------------------------------------
  virtual bool ChangeToSlayer();
  virtual bool ChangeToVampire();

  //----------------------------------------------------------
  // item»ç¿ë °¡´É ¿©ºÎ Ã¼Å©
  //----------------------------------------------------------
  virtual void CheckAffectStatus(MItem *pItem); // Æ¯Á¤ ¾ÆÀÌÅÛ

  //------------------------------------------------------
  // set
  //-------------------------------------x-----------------
  void SetName(const char *pName);
  void SetOldName(const char *pOldName);

  bool MovePosition(TYPE_SECTORPOSITION x,
                    TYPE_SECTORPOSITION y); // ZoneÀÇ Sector¿¡¼­µµ ÀÌµ¿ÇÑ´Ù.
  void SetZone(MZone *pZone) { m_pZone = pZone; }
  void SetPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
  void SetDirection(BYTE d); //		{ m_Direction = d; }//by viva
  void SetCurrentDirection(BYTE d) { m_CurrentDirection = d; }
  void SetDirectionToPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);

  // Sjheon 2005.07.13 Combo Skill  Add
  void SetCombo(int ComboCnt) { m_ComboCnt = ComboCnt; }
  void SetCurrentCombo(int CurrentCombo) { m_CurrentCombo = CurrentCombo; }
  void UseSkill_Effect();
  // Sjheon 2005.07.13 Combo Skill  End
  virtual void SetAction(BYTE action);

  void SetActionCountMax(BYTE ActionCountMax) {
    m_ActionCountMax = ActionCountMax;
  }

  // void	SetFrame(BYTE f)			{ m_ActionCount = f; }
  // void	SetZ(short z)				{ m_Z = z; }

  //----------------------------------------------------------
  // ¹æÇâ ¼³Á¤
  //----------------------------------------------------------
  bool
  ChangeNearDirection(); // CurrentDirectionÀ» DirectionÀ¸·Î ÀÚ¿¬½º·´°Ô ¹Ù²Û´Ù.

  //----------------------------------------------------------
  // Server
  //----------------------------------------------------------
  virtual void SetServerPosition(TYPE_SECTORPOSITION sX,
                                 TYPE_SECTORPOSITION sY);

  // ´ÙÀ½¿¡ ÇÒ Çàµ¿ ¼³Á¤..
  virtual void SetNextAction(BYTE action);
  virtual void SetNextActionToMove();

  //------------------------------------------------------
  // get
  //------------------------------------------------------
  virtual const char *GetName() const { return m_pName; }
  const char *GetOldName() const { return m_pOldName; }

  int GetCX() const { return m_cX; } // ÇÑ¹ø¿¡ ÀÌµ¿ÇÏ´Â pixel x
  int GetCY() const { return m_cY; } // ÇÑ¹ø¿¡ ÀÌµ¿ÇÏ´Â pixel y
  int GetSX() const { return m_sX; } // sector¿¡¼­ ¶³¾îÁø X°ª
  int GetSY() const { return m_sY; } // sector¿¡¼­ ¶³¾îÁø Y°ª
  TYPE_SECTORPOSITION GetServerX() const { return m_ServerX; }
  TYPE_SECTORPOSITION GetServerY() const { return m_ServerY; }
  BYTE GetAction() const { return m_Action; }
  BYTE GetNextAction() const { return m_NextAction; }
  BYTE GetDirection() const { return m_CurrentDirection; }
  BYTE GetServerDirection() const { return m_Direction; }
  short GetZ() const { return m_Z; }
  BYTE GetActionCount() const { return m_ActionCount; }
  BYTE GetActionCountMax() const; //{ return m_ActionCountMax; }
  BYTE GetMoveCount() const { return m_MoveCount; }
  BYTE GetMoveCountMax() const { return m_MoveCountMax; }

  // 2005.12 26 Add
  int GetOriginServerNum() { return m_Origin_ServerNum; }
  void SetOriginServerNum(int iOriginSeverNum) {
    m_Origin_ServerNum = iOriginSeverNum;
  }
  // 2005.12 26 AEnd

  // Sjheon 2005.07.13 Combo Skill  Add
  int GetCombo() { return m_ComboCnt; }
  int GetCurrentCombo() { return m_CurrentCombo; }
  // Sjheon 2005.07.13 Combo Skill  End

  BYTE GetFrame() const {
    // Æ¯º°ÇÑ actionÀÌ ¾øÀÌ ¿òÁ÷ÀÌ°í ÀÖ´Â °æ¿ì
    if (m_ActionCount >= GetActionCountMax()) {
      // ´Ù ¿òÁ÷¿©¼­ Á¤ÁöÇÏ°í ÀÖ´Â °æ¿ì´Â...actionÀ» º¸¿©Áà¾ß ÇÑ´Ù.
      return (m_MoveCount < m_MoveCountMax) ? m_MoveCount
             : (m_ActionCount == 0)         ? 0
                                            : GetActionCountMax() - 1;
    }
    // actionÀ» Ç¥ÇöÇØÁÖ´Â °æ¿ì
    else {
      return (m_ActionCount < GetActionCountMax()) ? m_ActionCount
                                                   : GetActionCountMax() - 1;
    }
  }

  // nActionInfo¿¡ ÀûÀýÇÑ actionÀ» Ã£´Â´Ù.
  int GetActionInfoAction(TYPE_ACTIONINFO nActionInfo,
                          bool IsSelfAction = false, bool IsSound = false);
  int GetActionInfoCastingStartFrame(TYPE_ACTIONINFO nActionInfo);
  int GetActionInfoCastingFrames(TYPE_ACTIONINFO nActionInfo);
  int GetActionInfoDelay(TYPE_ACTIONINFO nActionInfo);
  int GetActionInfoActionEffectSpriteType(TYPE_ACTIONINFO nActionInfo);
  int GetActionInfoRepeatStartFrame(TYPE_ACTIONINFO nActionInfo);
  int GetActionInfoRepeatEndFrame(TYPE_ACTIONINFO nActionInfo);

  // nActionInfo¿¡ ÀûÀýÇÑ casting sound¸¦ Ã£´Â´Ù.
  TYPE_SOUNDID GetCastingSoundID(TYPE_ACTIONINFO nActionInfo);

  void GetNextPosition(BYTE direction, POINT &next);
  // void		GetNextPosition(POINT &next);
  void GetNextPosition(TYPE_SECTORPOSITION &sX, TYPE_SECTORPOSITION &sY);
  int GetCounterDirection(int d);

  // LightSight³ª MaxEffectLight Áß¿¡¼­ Å« °ªÀ» ³Ñ°ÜÁØ´Ù. (ÇöÀçÀÇ °¡Àå Å« ½Ã¾ß)
  // char		GetMaxLightSight() const			{ return
  // (m_MaxEffectLight > m_LightSight)? m_MaxEffectLight : m_LightSight; }
  BYTE GetDirectionToPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);

  // PixelÁÂÇ¥
  int GetPixelX() const;
  int GetPixelY() const;

  // item count
  int GetItemCount() const { return m_ItemCount; }
  void SetItemCount(int c) { m_ItemCount = c; }

  // ( ±â¼úID, Effect DelayFrame )
  TYPE_ACTIONINFO GetDelayActionInfo() const { return m_DelayActionInfo; }
  DWORD GetEffectDelayFrame() const { return m_EffectDelayFrame; }
  void SetEffectDelayFrame(TYPE_ACTIONINFO ai, DWORD f) {
    m_DelayActionInfo = ai;
    m_EffectDelayFrame = f;
  }

  //------------------------------------------------------
  // Ä³¸¯ÅÍ Á¤º¸
  //------------------------------------------------------
  TYPE_FRAMEID GetCreatureFrameID(int index) const {
    return m_CreatureFrameIDs[index];
  }
  int GetCreatureFrameCount() const { return m_CreatureFrameIDs.GetSize(); }

  BOOL IsExistBody() const { return m_bExistBody; }

  //------------------------------------------------------
  // ±â¼ú
  //------------------------------------------------------
  void SetBasicActionInfo(TYPE_ACTIONINFO n) { m_nBasicActionInfo = n; }
  TYPE_ACTIONINFO GetBasicActionInfo() const;
  virtual void SetSpecialActionInfo(TYPE_ACTIONINFO n) {
    m_nSpecialActionInfo = n;
  }
  void SetSpecialActionInfoNULL() { m_nSpecialActionInfo = ACTIONINFO_NULL; }
  TYPE_ACTIONINFO GetSpecialActionInfo() const { return m_nSpecialActionInfo; }
  TYPE_ACTIONINFO GetUsedActionInfo() const { return m_nUsedActionInfo; }

  //----------------------------------------------------------
  // ±â¼ú ¸ñÇ¥´Â?
  //----------------------------------------------------------
  BOOL IsSpecialActionInfoTargetSelf() const {
    return (m_nSpecialActionInfo == ACTIONINFO_NULL)
               ? FALSE
               : (*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetSelf();
  }
  BOOL IsSpecialActionInfoTargetOther() const {
    return (m_nSpecialActionInfo == ACTIONINFO_NULL)
               ? FALSE
               : (*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetOther();
  }
  BOOL IsSpecialActionInfoTargetZone() const {
    return (m_nSpecialActionInfo == ACTIONINFO_NULL)
               ? FALSE
               : (*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetZone();
  }
  BOOL IsSpecialActionInfoTargetItem() const {
    return (m_nSpecialActionInfo == ACTIONINFO_NULL)
               ? FALSE
               : (*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetItem();
  }

  // º¹Àå
  virtual BOOL IsWear() const { return FALSE; }

  //------------------------------------------------------
  // Chatting String
  //------------------------------------------------------
  void SetChatString(char *str, COLORREF color = RGB(255, 255, 255));
  void SetPersnalString(char *str, COLORREF color = RGB(255, 255, 255));
  const char *GetChatString(BYTE i);
  void ClearChatString();
  void FadeChatString();
  COLORREF GetChatColor() { return m_ChatColor; }
  COLORREF GetOriChatColor() { return m_OriChatColor; }
  DWORD GetChatTime() const { return m_ChatTime; }

  // nanomech 2006.04.24
  COLORREF GetPersnalShopColor() { return m_ColorPersonalShop; }
  void SetPersnalShopColor(COLORREF nColor) { m_ColorPersonalShop = nColor; }

  //------------------------------------------------
  //
  // Effect °ü·Ã...
  //
  // AttachEffect´Â Ä³¸¯ÅÍ ¸ö¿¡ ºÙ¾îÀÖ´Â effect¸¦ ÀÇ¹ÌÇÑ´Ù.
  // GroundAttachEffect´Â Ä³¸¯ÅÍÀÇ ¹ß ¹Ø¿¡ ºÙ¾îÀÖ´Â effect¸¦ ÀÇ¹ÌÇÑ´Ù.
  // ±Ùµ¥, AttachEffect°¡ GroundAttachEffectµµ Æ÷ÇÔÇÏ´Â ÀÇ¹ÌÀÌ±âµµ ÇÑ´Ù.
  // (±âÁ¸¿¡ ±âÈ¹¿¡ ¾ø´ø ³»¿ëÀÌ¶ó¼­ Ãß°¡ÇÏ¸é¼­ °³°¡ µÈ ¼ö¸¹Àº ÄÚµå ÁßÀÇ
  // ÇÏ³ªÀÌ´Ù. 1818!)
  //
  //------------------------------------------------
  bool IsExistAttachEffect() const { return !m_listEffect.empty(); }
  bool IsExistGroundAttachEffect() const { return !m_listGroundEffect.empty(); }
  int GetAttachEffectSize() const { return m_listEffect.size(); }
  int GetGroundAttachEffectSize() const { return m_listGroundEffect.size(); }
  void ClearAttachEffect();

  // ¿©·¯°¡Áö ºÙ¾îÀÖ´Â Effect¸¦ Ç¥ÇöÇÏ´Â »ö±ò Áß¿¡¼­ --> ÇöÀç Ãâ·ÂÇÒ·Á´Â »ö
  WORD GetAttachEffectColor() const { return m_AttachEffectColor; }

  // AttachEffectÀÇ ½Ã¾ß Ãß°¡/»èÁ¦
  // void		SetLightSightAttachEffect();
  // void		UnSetLightSightAttachEffect();

  // ¿ÜºÎ¿¡¼­ iterator·Î ÀÛ¾÷À» ÇÒ ¼ö ÀÖµµ·Ï..
  ATTACHEFFECT_LIST::const_iterator GetAttachEffectIterator() const {
    return m_listEffect.begin();
  }
  ATTACHEFFECT_LIST::const_iterator GetGroundAttachEffectIterator() const {
    return m_listGroundEffect.begin();
  }

  // Add/Update
  MAttachEffect *
  CreateAttachEffect(TYPE_EFFECTSPRITETYPE type, DWORD delayFrame,
                     DWORD linkDelayFrame, BOOL bGroundEffect = FALSE,
                     MEffect::EFFECT_TYPE eType = MEffect::EFFECT_ATTACH,
                     bool bBasisCreature = true);
  // void
  // SetAttachEffectSprite(TYPE_EFFECTSPRITETYPE type)	{
  // m_AttachEffectSprite = type;	} int
  // GetAttachEffectSprite(TYPE_EFFECTSPRITETYPE type)	{	return
  // m_AttachEffectSprite;	}
  void RemoveAttachEffect(TYPE_EFFECTSPRITETYPE type);

  virtual bool AddEffectStatus(EFFECTSTATUS status, DWORD delayFrame);
  virtual bool RemoveEffectStatus(EFFECTSTATUS status);
  virtual bool ClearEffectStatus();

  bool HasEffectStatus(EFFECTSTATUS status) const {
    return m_bEffectStatus[status];
  }
  bool IsAvailableEffectStatus() const { return m_bEffectStatus != NULL; }

  virtual void UpdateAttachEffect();

  void RemoveGlacierEffect();
  void Remove_Curse_Paralsis_Effect();

  // char		GetMaxEffectLight() const		{ return
  // m_MaxEffectLight; }

  //------------------------------------------------------
  // Á×À½..
  //------------------------------------------------------
  virtual void SetDead();   // Á×´Â ¸ð½À º¸¿©ÁØ´Ù.
  virtual void SetAlive();  // »ì¾Æ³­´Ù.
  virtual void SetCorpse(); // Á×¾îÀÖ´Â ½ÃÃ¼ »óÅÂ
  virtual bool IsDead() const { return !m_bAlive; }
  virtual bool IsAlive() const { return m_bAlive; }

  //------------------------------------------------------
  // Invisible
  //------------------------------------------------------
  int IsInvisible() const { return m_InvisibleCount; }
  void SetInvisible();
  void SetInvisibleSoon(); // Áï½Ã invisible·Î ¸¸µç´Ù.
  void SetVisible();
  void SetVisibleSoon();
  int GetInvisibleCount() const { return m_InvisibleCount; }
  void UpdateInvisible();

  //------------------------------------------------------
  // Level Name
  //------------------------------------------------------
  void SetLevelName(int ln);
  BOOL HasLevelName() const { return m_LevelName; }
  const char *GetLevelName() const;

  //------------------------------------------------------
  // Guild
  //------------------------------------------------------
  void SetGuildNumber(int gn) { m_GuildNumber = gn; }
  int GetGuildNumber() const { return m_GuildNumber; }

  void SetUnionGuildID(int gn) { m_nUnionGuildID = gn; }
  int GetUnionGuildID() const { return m_nUnionGuildID; }

  //------------------------------------------------------
  // Weapon Speed
  //------------------------------------------------------
  WEAPON_SPEED GetWeaponSpeed() const { return m_WeaponSpeed; }
  void SetWeaponSpeed(int speed);
  void SetWeaponSpeed(WEAPON_SPEED ws) { m_WeaponSpeed = ws; }

  //------------------------------------------------------
  // Head
  //------------------------------------------------------
  bool CanRemoveHead() const {
    return (*g_pCreatureTable)[m_CreatureType].bHeadCut;
  }
  void RemoveHead() {
    if (CanRemoveHead())
      m_bHasHead = false;
  }
  bool HasHead() const { return m_bHasHead; }

  //------------------------------------------------------
  // HP È¸º¹
  //------------------------------------------------------
  // rate¸¸Å­ times¹ø È¸º¹µÈ´Ù.
  inline BOOL IsRecoveryHP();
  inline BOOL IsRecoveryMP();
  void SetRecoveryHP(int amount, int times, DWORD delay);
  void SetRecoveryMP(int amount, int times, DWORD delay);
  void StopRecoveryHP() { m_RecoveryHPTimes = 0; }
  void StopRecoveryMP() { m_RecoveryMPTimes = 0; }
  DWORD GetRecoveryHPDelayTime() const { return m_RecoveryHPDelayTime; }
  int GetRecoveryHPTimes() const { return m_RecoveryHPTimes; }
  int GetRecoveryHPAmmount() const { return m_RecoveryHPAmount; }
  DWORD GetRecoveryMPDelayTime() const { return m_RecoveryMPDelayTime; }
  int GetRecoveryMPTimes() const { return m_RecoveryMPTimes; }
  int GetRecoveryMPAmmount() const { return m_RecoveryMPAmount; }

  void SetRegen(int amount, DWORD delay);
  void SetRegenBonus(int amount, DWORD delay);
  void CheckRegen();

  void UpdateStatus();

  //------------------------------------------------------
  // ÇÇ Èê¸®±â
  //------------------------------------------------------
  virtual void CheckDropBlood();

  //------------------------------------------------------
  // È­¸é¿¡ Ç¥½ÃÇÏ´Â HP barÀÇ ±æÀÌ
  //------------------------------------------------------
  int GetHPBarWidth() const {
    return (*g_pCreatureTable)[m_CreatureType].HPBarWidth;
  }

  //------------------------------------------------------
  // Fade
  //------------------------------------------------------
  // ¸ö ÀüÃ¼°¡ ¾îµÓ°Ô Ç¥ÇöµÇ´Â °æ¿ì
  bool IsFade() const { return m_bFade; }
  void SetFade() { m_bFade = true; }
  void UnSetFade() { m_bFade = false; }

  bool IsFadeShadow() const { return m_bFadeShadow; }
  void SetFadeShadow() { m_bFadeShadow = true; }
  void UnSetFadeShadow() { m_bFadeShadow = false; }

  // Ä³¸¯ÅÍ µ¿ÀÛÀ» µû¸£´Â ±×¸²ÀÚ ¸ð½À °³¼ö.. - -;
  int GetShadowCount() const { return m_ShadowCount; }
  void SetShadowCount(int n);

  int IsSkipShadow() const { return m_bSkipShadow; }
  void SetSkipShadow(bool b) { m_bSkipShadow = b; }

  //------------------------------------------------------
  // ³È..
  //------------------------------------------------------
  virtual int FindEnemy();

  //------------------------------------------------------
  //
  //              Server Packet Ã³¸®
  //
  //------------------------------------------------------
  virtual void PacketSpecialActionResult(TYPE_ACTIONINFO nResultActionInfo,
                                         TYPE_OBJECTID id,
                                         TYPE_SECTORPOSITION sX,
                                         TYPE_SECTORPOSITION sY, BYTE temp = 0);
  virtual void PacketAttackNormal(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y,
                                  BYTE direction);
  virtual void PacketMove(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y,
                          BYTE direction);
  virtual void PacketSpecialActionToSector(TYPE_ACTIONINFO nActionInfo,
                                           TYPE_SECTORPOSITION sX,
                                           TYPE_SECTORPOSITION sY,
                                           MActionResult *pActionResult);
  virtual void PacketSpecialActionToOther(TYPE_ACTIONINFO nActionInfo,
                                          TYPE_OBJECTID id,
                                          MActionResult *pActionResult);
  virtual void
  PacketSpecialActionToNobody(TYPE_ACTIONINFO nActionInfo,
                              TYPE_SECTORPOSITION x,
                              TYPE_SECTORPOSITION y); // »ó´ë°¡ zone¿¡ ¾ø´Â °æ¿ì
  virtual void PacketSpecialActionToSelf(TYPE_ACTIONINFO nActionInfo,
                                         MActionResult *pActionResult);
  virtual void PacketSpecialActionToInventoryItem(TYPE_ACTIONINFO nActionInfo);

  //------------------------------------------------------
  // Fast Move
  //------------------------------------------------------
  bool IsFastMove() const { return m_bFastMove; }
  virtual bool FastMovePosition(
      TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y,
      bool server = false); // ZoneÀÇ Sector¿¡¼­µµ »¡¸®(-_-;) ÀÌµ¿ÇÑ´Ù.
  void StopFastMove();

  //------------------------------------------------------
  // Knock Back
  //------------------------------------------------------
  int IsKnockBack() const { return m_bKnockBack > 0; }
  virtual bool KnockBackPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY,
                                 BYTE Action = 0);
  void StopKnockBack() { m_bKnockBack = 0; }

  //------------------------------------------------------
  // Casket
  //------------------------------------------------------
  bool IsInCasket() const { return m_bInCasket; }
  int GetCasketCount() const { return m_CasketCount; }
  int GetCasketType() const { return m_CasketType; }
  void AddCasket(int casketType);
  void RemoveCasket();
  void AddCasketSoon(int casketType);
  void RemoveCasketSoon();
  void UpdateCasket();

  //--------------------------------------------------
  // CutHeight
  //--------------------------------------------------
  bool IsCutHeight() const { return m_bCutHeight; }
  int GetCutHeightCount() const { return m_CutHeightCount; }
#if __CONTENTS(__2008_OCTOBER_MAKETITEM)
  void SetCutHeightFrom(int startCount, int lastCount, int inc,
                        int turn = true);
#else
  void SetCutHeightFrom(int startCount, int lastCount, int inc);
#endif //__2008_OCTOBER_MAKETITEM
  void UpdateCutHeight();

  //--------------------------------------------------
  // Turning
  //--------------------------------------------------
  bool IsTurning() const { return m_bTurning; }
  int GetTurningCount() const { return m_TurningCount; }
  void SetTurning(WORD count) {
    m_bTurning = true;
    m_TurningCount = count;
  }
  void StopTurning() { m_bTurning = false; }
  void UpdateTurning();

  //--------------------------------------------------
  // CauseCriticalWounds
  //--------------------------------------------------
  bool IsCauseCriticalWounds() const { return m_bCauseCriticalWounds; }
  void SetCauseCriticalWounds(int delay) {
    m_bCauseCriticalWounds = true;
    m_CauseCriticalWoundsCount = delay;
  }
  void StopCauseCriticalWounds() { m_bCauseCriticalWounds = false; }
  void UpdateCauseCriticalWounds();

  //--------------------------------------------------
  // BloodyZenith
  //--------------------------------------------------
  bool IsBloodyZenith();
  void SetBloodyZenith(int delay) {
    m_bBloodyZenith = true;
    m_BloodyZenithCount = delay;
  }
  void StopBloodyZenith() { m_bBloodyZenith = false; }
  int GetBloodyZenithCount();
  void UpdateBloodyZenith();

  //--------------------------------------------------
  //   Ä§ÁéÓ°×Ó
  //--------------------------------------------------
  bool IsOusterGhost();
  void SetOusterGhost(int delay) {
    m_bOusterGhost = true;
    m_OusterGhostCount = delay;
  }
  void StopOusterGhost() { m_bOusterGhost = false; }
  int GetOusterGhostCount();
  void UpdateOusterGhost();
  //--------------------------------------------------
  // GunShotGuidance
  //--------------------------------------------------
  bool IsGunShotGuidance() const { return m_bGunShotGuidance; }
  void SetGunShotGuidance(int delay) {
    m_bGunShotGuidance = true;
    m_GunShotGuidanceCount = delay;
  }
  void StopGunShotGuidance() { m_bGunShotGuidance = false; }
  void UpdateGunShotGuidance();

  //--------------------------------------------------
  // BurningSolCount
  //--------------------------------------------------
  bool IsBurningSol() const { return m_bBurningSol; }
  void SetBurningSol(int delay) {
    m_bBurningSol = true;
    m_bBurningSolCount = delay;
  }
  void StopBurningSol() { m_bBurningSol = false; }
  bool UpdateBurningSol();

  //--------------------------------------------------
  // InstallTurret
  //--------------------------------------------------
  BYTE GetInstallTurretCount() const { return m_bInstallTurretCount; }
  void SetInstallTurretCount(int Count) { m_bInstallTurretCount = Count; }
  BYTE GetInstallTurretDirect() const { return m_bInstallTurretDirect; }
  void SetInstallTurretDirect(int Count) { m_bInstallTurretDirect = Count; }
  bool UpDateInstallTurret();
  bool UpDateInstallMK1();
  void InstallTurretStopAttack();

  //--------------------------------------------------
  // Armageddon
  //--------------------------------------------------
  bool IsArmageddon() const { return m_bArmageddon; }
  void SetArmageddon(int delay) {
    m_bArmageddon = true;
    m_ArmageddonCount = delay;
  }
  const int GetArmageddon() const {
    if (IsArmageddon())
      return m_ArmageddonCount;
    return -1;
  }
  void StopArmageddon() { m_bArmageddon = false; }
  void UpdateArmageddon();

  //--------------------------------------------------
  // Teleport
  //--------------------------------------------------
  bool IsGhost(BYTE flag) const { return (m_fGhost & flag) ? true : false; }
  void SetGhost(BYTE flag, int delay) {
    m_fGhost = flag;
    m_GhostCount = delay;
  }
  const int GetGhost() const {
    if (IsGhost(0xFF))
      return m_GhostCount;
    return -1;
  }
  void StopGhost() { m_fGhost = 0; }
  void UpdateGhost();

  //--------------------------------------------------
  // FakeDie
  //--------------------------------------------------
  bool IsFakeDie() const { return m_bFakeDie; }
  void SetFakeDie();
  int GetFakeDieCount() { return m_FakeDieCount; }
  void StopFakeDie() { m_bFakeDie = false; }
  void UpdateFakeDie();

  //----------------------------------------------------------
  // player°¡ Darkness¾È¿¡ ÀÖ´Â°¡?
  // DarknessCount´Â ¸îÅ¸ÀÏÀÌ º¸ÀÌ´Â°¡?ÀÌ´Ù.
  //----------------------------------------------------------
  BOOL IsInDarkness() const { return m_DarknessCount >= 0; }
  BOOL ShowInDarkness(int sX, int sY) const;
  int GetDarknessCount() const { return m_DarknessCount; }
  void UpdateDarkness();
  virtual void CheckInDarkness();
  void
  PlaceInDarkness(bool IsBlindness =
                      false); // 2004, 6, 24 sobeit modify - blindness Ã³¸® ¶«¿¡
  void PlaceNotInDarkness();

  BOOL IsInGroundElemental() const;

  BOOL IsInGunFiled() const { return m_GunFiledCount >= 0; } //   ÈËÀàÇ¹211ÒþÉí
  BOOL ShowInGunFiled(int sX, int sY) const;
  int GetGunFiledCount() const { return m_GunFiledCount; }
  void UpdateGunFiled();
  virtual void CheckInGunFiled();
  void
  PlaceInGunFiled(bool IsBlindness =
                      false); // 2004, 6, 24 sobeit modify - blindness Ã³¸® ¶«¿¡
  void PlaceNotInGunFiled();
  //--------------------------------------------------
  // ÈíÇ÷ delay¸¦ ¾ø¾Ö±â À§ÇØ¼­ ÀÓ½Ã·Î..
  //--------------------------------------------------
  void SetStopBloodDrain() { m_bStopBloodDrain++; }
  void UnSetStopBloodDrain() {
    if (m_bStopBloodDrain > 0)
      m_bStopBloodDrain--;
  }
  void ClearStopBloodDrain() { m_bStopBloodDrain = 0; }
  virtual void StopBloodDrain();

  //--------------------------------------------------
  // Èí¿µ delay¸¦ ¾ø¾Ö±â À§ÇØ¼­ ÀÓ½Ã·Î..
  //--------------------------------------------------
  void SetStopAbsorbSoul() { m_bStopAbsorbSoul++; }
  void UnSetStopAbsorbSoul() {
    if (m_bStopAbsorbSoul > 0)
      m_bStopAbsorbSoul--;
  }
  void ClearStopAbsorbSoul() { m_bStopAbsorbSoul = 0; }
  virtual void StopAbsorbSoul();

  //--------------------------------------------------
  // ÆÄÆ¼ Ã¼Å©
  //--------------------------------------------------
  bool IsPlayerParty() const {
    return m_bPlayerParty && !m_bHallu;
  } // ÇÒ·ç°¡ °É·ÁÀÖÀ¸¸é ÆÄÆ¼ ¾Æ´Ô
  void SetPlayerParty() { m_bPlayerParty = true; }
  void UnSetPlayerParty() { m_bPlayerParty = false; }

  //--------------------------------------------------
  // Hallucination°É·ÈÀ»¶§ ¸÷À¸·Î º¸ÀÌ±â - -;
  //--------------------------------------------------
  bool IsHallu() const { return m_bHallu; }
  void DetermineHalluActionFrame();
  void SetHalluCreature(TYPE_CREATURETYPE type);
  void UnSetHalluCreature() { m_bHallu = false; }
  TYPE_FRAMEID GetHalluCreatureFrameID() const {
    return m_HalluCreatureFrameID;
  }
  TYPE_CREATURETYPE GetHalluCreatureType() const { return m_HalluCreatureType; }
  WORD GetHalluColor() const { return m_HalluColorBody; }
  BYTE GetHalluAction() const { return m_HalluAction; }
  BYTE GetHalluFrame() const { return m_HalluFrame; }
  const char *GetHalluName() const;

  // ¿î¿µÀÚ
  void SetCompetence(BYTE c) { m_Competence = c; }
  BYTE GetCompetence() const { return m_Competence; }

  // ÈíÇ÷
  void SetDrainCreatureID(TYPE_OBJECTID id) { m_DrainCreatureID = id; }
  const TYPE_OBJECTID GetDrainCreatureID() const { return m_DrainCreatureID; }
  void StopDrain(); // ³ª¸¦ ÈíÇ÷ÇÏ°í ÀÖ´ø CreatureÀÇ ÈíÇ÷µ¿ÀÛÀ» ¸ØÃá´Ù

  // Èí¿µ
  void SetAbsorbCreatureID(TYPE_OBJECTID id) { m_DrainCreatureID = id; }
  const TYPE_OBJECTID GetAbsorbCreatureID() const { return m_DrainCreatureID; }
  void StopAbsorb(); // ³ª¸¦ ÈíÇ÷ÇÏ°í ÀÖ´ø CreatureÀÇ Èí¿µµ¿ÀÛÀ» ¸ØÃá´Ù

  // PET
  void SetPetID(TYPE_OBJECTID id) { m_PetID = id; }
  const TYPE_OBJECTID GetPetID() const { return m_PetID; }

  // Elemental
  void SetElementalID(TYPE_OBJECTID id) { m_ElementalID = id; }
  const TYPE_OBJECTID GetElementalID() const { return m_ElementalID; }

  //--------------------------------------------------
  // HP Modify°ü·Ã
  //--------------------------------------------------
  const HPMODIFYLIST *GetHPModifyList() const { return &m_HPModifyList; }
  void AddHPModify(const int modify);
  const bool IsEmptyHPModifyList() const { return m_HPModifyList.empty(); }
  const int GetHPModifyListSize() const { return m_HPModifyList.size(); }

  void SetActionGrade(BYTE a) {
    if (a != 0)
      m_GradeActionInfo = a;
  }
  void ClearActionGrade() { m_GradeActionInfo = 0; }
  BYTE GetActionGrade() { return m_GradeActionInfo; }

  virtual void SetTraceID(TYPE_OBJECTID id);
  TYPE_OBJECTID GetTraceID() const { return m_TraceID; }

  void SetActionDelay(short time) { m_DelayLastAction = time; }
  short GetActionDelay() const { return m_DelayLastAction; }

  void SetNickName(BYTE bType, char *szNickName) {
    m_NickNameType = bType;
    m_NickName = szNickName;
  }
  const std::string &GetNickName() { return m_NickName; }
  BYTE GetNickNameType() { return m_NickNameType; }

  bool GetIsInSiegeWar() { return (m_bSiegeWar > 0) ? true : false; }
  void SetSiegeWar(BYTE bSiege) { m_bSiegeWar = bSiege; }
  bool GetIsAttackerInSiegeWar() {
    return (m_bSiegeWar > 2 && m_bSiegeWar < 8) ? true : false;
  }
  bool GetIsDefenderInSiegeWar() {
    return (m_bSiegeWar > 0 && m_bSiegeWar < 3) ? true : false;
  }

  BOOL IsInFuryOfGnome() const;

  const std::string &GetPersnalShopMessage() { return m_PersnalMessage; }
  void SetPersnalShopMessage(const char *str) { m_PersnalMessage = str; }

  void SetPersnalShop(int curshop) { m_Persnal_flag = curshop; }
  int CurPernalShop() { return m_Persnal_flag; }

  void SetInputChatting(bool b_input) { m_Input_Chat_Flag = b_input; }
  bool GetInputChatting() { return m_Input_Chat_Flag; }

  BYTE GetPersonalShopMsgTime() const { return m_PersonalShopOpenTime; }
  void SetPersonalShopMsgTime(BYTE time) { m_PersonalShopOpenTime = time; }

  bool IsAdvancementClass() const {
    return GetAdvancementClassLevel() > 0 &&
           GetAdvancementClassLevel() != MODIFY_NULL;
  }

  int GetSpriteIndex() const { return m_current_sprite_index; }

  // 2005, 1, 5, sobeit add start
  void UpdateBikeCrash();
  // 2005, 1, 5, sobeit add end
  void RemoveCauseCriticalWoundsEffect();

  // 2005, 2, 22, sobeit add start

  WORD GetMasterEffectType(DWORD Status);
  void ChangeMasterEffectType(int MarketEffect);
  // 2005, 2, 22, sobeit add end

  int GetTeenDrainFrame() { return m_teenDrainFrame; }
  int GetOldTeenDrainFrame() { return m_oldTeenDrainFrame; }
  void SetTeenDrainFrame(int teenDrainFrame) {
    m_teenDrainFrame = teenDrainFrame;
  }
  void SetOldTeenDrainFrame(int oldTeenDrainFrame) {
    m_oldTeenDrainFrame = oldTeenDrainFrame;
  }

  BYTE GetActionStep() { return m_ActionStep; }
  void SetActionStep(BYTE actionStep) { m_ActionStep = actionStep; }

  static int GetVampireActionStep(int action, BYTE step) {
    return s_VampireActionStep[action][step];
  }

  bool IsBattleStand() const { return m_bBattleStand; }

// Å×½ºÆ® ÄÚµå
#if __CONTENTS(__AUTO_ITEM_ROOTING)
  void SetUseActionInfo(unsigned short nUsedActionInfo) {
    m_nUsedActionInfo = nUsedActionInfo;
  }

#endif //__AUTO_ITEM_ROOTING

  void SetTraceX(unsigned short TraceX) { m_TraceX = TraceX; }
  void SetTraceY(unsigned short TraceY) { m_TraceY = TraceY; }

protected:
  virtual void ActionMove();
  virtual void ActionEffect();

  virtual void AffectUsedActionInfo(TYPE_ACTIONINFO nUsedActionInfo);
  virtual void AttachCastingEffect(TYPE_ACTIONINFO nUsedActionInfo,
                                   BOOL bForceAttach = FALSE);

  virtual void AffectBufferedActionInfo();

  void UpdateBattleStand(int action);

protected:
  // CreatureÀÇ Á¾·ù
  TYPE_CREATURETYPE m_CreatureType;
  // TYPE_FRAMEID			m_CreatureFrameID;
  CTypeTable<TYPE_FRAMEID> m_CreatureFrameIDs;
  BOOL m_bExistBody;
  DWORD m_TraceTimer; // µû¶ó°¡±â Á¦ÇÑ½Ã°£

  //--------------------------------------------------
  // »ö±ò
  //--------------------------------------------------
  // PC Vampire : 1=ÇÇºÎ, 2=¿Ê
  // Monster : 1=¸öÀÇ ¾îµò°¡¿¡ º¯ÇÏ´Â »ö.. - -; 2=Nothing
  WORD m_ColorBody1;
  WORD m_ColorBody2;
  WORD m_ChangeColorSet; // ¸÷ ÀüÃ¼ »ö±òÀÌ ¹Ù²î´Â °æ¿ì

  WORD m_BatColorSet;
  WORD m_AdvanceBatColorSet;
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
  BYTE m_WingType;
  WORD m_WingColorSet;
  WORD m_WingEffectColorSet;
#if __CONTENTS(__SECOND_TRANSFORTER)
  WORD m_WingItemType;
#endif //__SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

  BOOL m_bAlpha;
  BOOL m_bAlphaShadow;
  // 2005, 2, 21, sobeit add start
  WORD m_MasterEffectType;
  // 2005, 2, 21, sobeit add end

  int m_OptionExtFrame; // 2006.3.22 chyaya
  int m_OptionExtTime;
  int m_OptionExtGrade;

  // Move Á¾·ù : ÁöÇÏ,Áö»ó,°øÁß
  MOVE_TYPE m_MoveType;
  short m_Z; // ³ôÀÌ Z

  // ÀÌµ¿ÇÏ´Â ¹æ¹ý
  MOVE_DEVICE m_MoveDevice;
  BYTE m_MoveAction;

  MZone *m_pZone; // ÀÌ Creature°¡ ¼ÓÇÏ´Â Zone

  // ´ÙÀ½ ¿òÁ÷ÀÏ ¹æÇâ
  TYPE_SECTORPOSITION m_NextX;
  TYPE_SECTORPOSITION m_NextY;
  BYTE m_NextDirection;

  //--------------------------------------------------
  // Move Buffering
  //--------------------------------------------------
  MOVENODE_LIST m_listMoveBuffer;

  // sector»çÀÌÀÇ smooth scrollÀ» À§ÇØ¼­
  int m_sX, m_sY; // ÀÌµ¿ÇØ¾ßÇÒ ³ª¸ÓÁö pixels
  int m_cX, m_cY; // ÇÑ¹ø¿¡ ÀÌµ¿ÇÏ´Â °¡·Î pixel

  // ÀÌµ¿ÇÏ´Âµ¥ ÇÊ¿äÇÑ Á¤º¸
  static int m_sXTable[MAX_DIRECTION];
  static int m_sYTable[MAX_DIRECTION];
  static int *m_cXTable[MAX_FRAME_MOVE][MAX_DIRECTION];
  static int *m_cYTable[MAX_FRAME_MOVE][MAX_DIRECTION];

  // ÇöÀç frame¿¡ ´ëÇÑ Á¤º¸
  BYTE m_Action;           // Action¿¡ ´ëÇÑ ID
  BYTE m_CurrentDirection; // ÇöÀçÀÇ Direction
  BYTE m_Direction;        // ¹Ù¶óºÁ¾ß ÇÏ´Â Direction
  BYTE m_DirectionMove;    // ÀÌµ¿ÇÒ ¹æÇâ
  BYTE m_DirectionMoved;   // ÀÌµ¿ÇÑ ¹æÇâ
  BYTE m_ActionCount;      // ActionCount == Frame
  BYTE m_ActionCountMax;   // ActionCountMax
  BYTE m_MoveCount;        // ÀÌµ¿ÇÑ È¸¼ö
  BYTE m_MoveCountMax;     // ÀÌµ¿ÇÏ´Â ¸ñÇ¥ È¸¼ö
  BYTE m_MoveTableCount;   // cX,YTableÀÇ count
  BYTE m_NextMoveCount;    // ´ÙÀ½¿¡ ÀÌµ¿ÇØ¾ßÇÏ´Â count¼ö

  // Sjheon 2005.07.13 Combo Skill Add
  int m_ComboCnt;     // µ¿ÀÛÇÒ ÄÞº¸ °³¼ö
  int m_CurrentCombo; // ÇöÀç ÄÞº¸ °³¼ö
  // Sjheon 2005.07.13 Combo Skill  End

  BYTE m_NextAction;
  BYTE m_ActionStep;

  // ±â¼ú Á¾·ù
  TYPE_ACTIONINFO m_nBasicActionInfo;   // ¼±ÅÃµÇ¾î ÀÖ´Â ±âº» ±â¼ú Á¾·ù
  TYPE_ACTIONINFO m_nSpecialActionInfo; // ¼±ÅÃµÇ¾î ÀÖ´Â Æ¯¼ö ±â¼ú Á¾·ù
  TYPE_ACTIONINFO m_nUsedActionInfo;    // »ç¿ëÇÑ °Í

  // ´ÙÀ½¿¡ »ç¿ëÇÏ°ÔµÇ´Â ±â¼ú..
  TYPE_ACTIONINFO m_nNextUsedActionInfo;

  // °ø°ÝÇÏ´Â ¸ñÇ¥¿¡ ´ëÇÑ Á¤º¸
  TYPE_OBJECTID m_TraceID;      // µû¶ó°¡´Â ObjectÀÇ ID
  TYPE_SECTORPOSITION m_TraceX; // ÃßÀûÇÏ´Â ¸ñÇ¥ À§Ä¡
  TYPE_SECTORPOSITION m_TraceY;
  short m_TraceZ;

  // Server¿¡¼­ ÃÖ±Ù¿¡ °ËÁõµÈ À§Ä¡
  TYPE_SECTORPOSITION m_ServerX;
  TYPE_SECTORPOSITION m_ServerY;

  // Ä³¸¯ÅÍ¿¡ ºÙ¾î ÀÖ´Â Effect Ç¥Çö
  bool *m_bAttachEffect;
  bool *m_bEffectStatus;
  ATTACHEFFECT_LIST m_listEffect;
  ATTACHEFFECT_LIST m_listGroundEffect;
  WORD m_AttachEffectColor; // ÇöÀç Ãâ·ÂÇÒ·Á´Â Ä³¸¯ÅÍ »ö±ò ¹Ù²Ù±â »ö -_-;(¸»ÀÌ
                            // ÀÌ»óÇØ..À½³Ä)
  // BYTE					m_nAlphaEffect;
  // char					m_MaxEffectLight;
  // // EffectÀÇ ÃÖ°í ¹à±â(½Ã¾ß)
  //		int m_AttachEffectStatus; 		int
  // m_AttachEffectSprite;

  // chat
  COLORREF m_ChatColor;    // ÇöÀçÀÇ Ã¤ÆÃ »ö
  COLORREF m_OriChatColor; // fadeµÇ±â ÀüÀÇ Ã¤ÆÃ »ö

  COLORREF m_ColorPersonalShop; // »óÁ¡ Ã¤ÆÃ »ö

  DWORD m_NextChatFadeTime; // ÃÖ±ÙÀÇ Chat stringÀÌ ÀÔ·ÂµÈ ½Ã°£
  BYTE m_ChatStringCurrent;
  char **m_ChatString;
  DWORD m_ChatTime;
  BYTE m_PersonalShopOpenTime;

  // ActionInfo¿¡ ´ëÇÑ °á°ú(Server·ÎºÎÅÍ °ËÁõµÈ °Í)
  MActionResult *m_pActionResult;

  // Ä³¸¯ÅÍ ÀÌ¸§
  char *m_pName;
  char *m_pOldName;

  // »ì¾ÆÀÖ³ª?
  bool m_bAlive;

  // ´ÙÀ½ Çàµ¿ÀÌ ÀÖ³ª?
  bool m_bNextAction;

  // °¡Áö°í ÀÖ´Â item °³¼ö [ TEST CODE ] - -;;
  int m_ItemCount;

  // ±â¼úÀÇ Áö¼Ó ½Ã°£ ¼³Á¤
  TYPE_ACTIONINFO m_DelayActionInfo;
  DWORD m_EffectDelayFrame;

  // LevelName
  int m_LevelName;

  // ±æµå
  int m_GuildNumber;
  int m_nUnionGuildID;

  // fast move
  bool m_bFastMove;

  // repeat count
  int m_RepeatCount;

  // Origin_ServerIdx ;
  int m_Origin_ServerNum;

  // ¹«±â °ø°Ý ¼Óµµ
  WEAPON_SPEED m_WeaponSpeed;

  static int s_SlayerActionSpeed[ACTION_MAX_SLAYER][3];
  static int s_VampireActionSpeed[ACTION_MAX_VAMPIRE]
                                 [3]; // ACTION_MAX_VAMPIRE][3];
  static int s_OustersActionSpeed[ACTION_MAX_SLAYER]
                                 [3]; // ACTION_MAX_VAMPIRE][3];

  // Action Step
  static int s_VampireActionStep[ACTION_MAX_VAMPIRE][3];

  //--------------------------------------------------
  // ½Ã°£¿¡ µû¸¥ HP È¸º¹
  //--------------------------------------------------
  DWORD m_RecoveryHPNextTime;
  DWORD m_RecoveryHPDelayTime;
  int m_RecoveryHPTimes;
  int m_RecoveryHPAmount;
  DWORD m_RecoveryMPNextTime;
  DWORD m_RecoveryMPDelayTime;
  int m_RecoveryMPTimes;
  int m_RecoveryMPAmount;
  // DWORD					m_RecoveryPart;		// hp,
  // mp?

  // ÀÚµ¿ regen - -;
  DWORD m_RegenDelayTime;
  DWORD m_RegenNextTime;
  int m_RegenAmount;
  DWORD m_RegenBonusDelayTime;
  DWORD m_RegenBonusNextTime;
  int m_RegenBonusAmount;

  //--------------------------------------------------
  // ¸Ó¸®°¡ ÀÖ³ª?
  //--------------------------------------------------
  bool m_bHasHead;

  //--------------------------------------------------
  // ¾îµÓ°Ô Âï±â
  //--------------------------------------------------
  bool m_bFade;
  bool m_bFadeShadow;
  int m_ShadowCount;

  //--------------------------------------------------
  // ¼ºº°
  //--------------------------------------------------
  bool m_bMale;

  //--------------------------------------------------
  // ÇÇ Èê¸®±â
  //--------------------------------------------------
  DWORD m_NextBloodingTime;

  //--------------------------------------------------
  // KnockBack Ã³¸®
  //--------------------------------------------------
  int m_bKnockBack; // >0ÀÌ¸é KnockBackÁß.

  //--------------------------------------------------
  // ÀÌ°Íµé ´Ù ¹­¾î¾ß µÇ´Âµ¥...  ¾ðÁ¦³ª~ --;;
  //--------------------------------------------------
  // Åõ¸í µÇ±â
  //--------------------------------------------------
  int m_InvisibleCount; // 0ÀÌ¸é ¿ÏÀüÈ÷ º¸ÀÎ´Ù. 64¸é ¿ÏÀü Åõ¸í
  int m_InvisibleStep;

  //--------------------------------------------------
  // °ü
  //--------------------------------------------------
  bool m_bInCasket;
  int m_CasketCount;
  int m_CasketCountInc;
  int m_CasketType;

  //--------------------------------------------------
  // ´Ù ±×¸±±î ¸»±î..
  //--------------------------------------------------
  bool m_bCutHeight;
  int m_CutHeightCount;
  int m_CutHeightCountLast;
  int m_CutHeightCountInc;
#if __CONTENTS(__2008_OCTOBER_MAKETITEM)
  int m_Turn;
#endif //__2008_OCTOBER_MAKETITEM
       //--------------------------------------------------
       // ºùºù µ¹±â
       //--------------------------------------------------
  bool m_bTurning;
  int m_TurningCount;

  //--------------------------------------------------
  // ÈíÇ÷ delay¸¦ ¾ø¾Ö±â À§ÇÑ ÀÓ½Ã ¹æÆí
  //--------------------------------------------------
  BYTE m_bStopBloodDrain;
  BYTE m_bStopAbsorbSoul;

  //----------------------------------------------------
  // CauseCriticalWounds¿¡ ¹­¿© ÀÖÀ»¶§ ÈíÇ÷ ´çÇÏ´Â µ¿ÀÛ
  //----------------------------------------------------
  bool m_bCauseCriticalWounds;
  int m_CauseCriticalWoundsCount;

  //----------------------------------------------------
  // Bloody Zenith¿¡ ¹­¿© ÀÖÀ»¶§ ÈíÇ÷ ´çÇÏ´Â µ¿ÀÛ
  //----------------------------------------------------
  bool m_bBloodyZenith;
  int m_BloodyZenithCount;

  bool m_bOusterGhost;
  int m_OusterGhostCount;

  //----------------------------------------------------
  // GunShotGuidance¿¡ ¹­¿© ÀÖÀ»¶§ ÃÑ½î´Â µ¿ÀÛ
  //----------------------------------------------------
  bool m_bGunShotGuidance;
  int m_GunShotGuidanceCount;

  //----------------------------------------------------
  // Armageddon¿¡ ¹­¿© ÀÖÀ» ¶§
  //----------------------------------------------------
  bool m_bArmageddon;
  int m_ArmageddonCount;

  //----------------------------------------------------
  // Teleport¿¡ ¹­¿© ÀÖÀ» ¶§
  //----------------------------------------------------
  BYTE m_fGhost;
  int m_GhostCount;

  //----------------------------------------------------
  // Ä³¸¯ÅÍ »ç¶óÁö´Â µ¿ÀÛ
  //----------------------------------------------------
  bool m_bFakeDie;
  int m_FakeDieCount;
  //----------------------------------------------------
  // Æ¾¹öÁ¯ ½ÉÀÇ¸¦ À§ÇÑ ÈíÇ÷ µ¿ÀÛ ÇÁ·¹ÀÓ Á¶Á¤
  //----------------------------------------------------
  int m_teenDrainFrame;
  int m_oldTeenDrainFrame;

  //----------------------------------------------------
  // darkness
  //----------------------------------------------------
  int m_DarknessCount;
  int m_DarknessCountInc;

  int m_GunFiledCount;
  int m_GunFiledCountInc;
  //----------------------------------------------------
  // party - playerÀÇ Ã¼Å©¸¦ ºü¸£°Ô ÇÒ·Á°í..
  //----------------------------------------------------
  bool m_bPlayerParty;

  //--------------------------------------------------
  // Hallucination°É·ÈÀ»¶§ ¸÷À¸·Î º¸ÀÌ±â
  //--------------------------------------------------
  bool m_bHallu;
  TYPE_CREATURETYPE m_HalluCreatureType;
  TYPE_FRAMEID m_HalluCreatureFrameID;
  WORD m_HalluColorBody;
  BYTE m_HalluAction;
  BYTE m_HalluFrame;
  WORD m_HalluName;        // ÀÏ´Ü ÀÓ½Ã·Î...
  short m_DelayLastAction; // ÇØ´ç ¾×¼ÇÀ» ÇÑ ÈÄ¿¡ ÀÏ¹Ý ¾×¼ÇÀ¸·Î µ¹¾Æ¿Ã¶§ µô·¹ÀÌ

  //--------------------------------------------------
  // ¿î¿µÀÚ
  //--------------------------------------------------
  BYTE m_Competence;

  //--------------------------------------------------
  // ÈíÇ÷Áß¿¡ ÈíÇ÷»ó´ë°¡ Á×À¸¸é ÈíÇ÷ µ¿ÀÛ ÁßÁöÇÏ±â À§ÇØ.
  // ÈíÇ÷´çÇÏ´Â »ç¶÷ÀÌ ÀÚ½ÅÀ» ÈíÇ÷ ÇÏ´Â »ç¶÷ÀÇ ID¸¦ ±â¾ï
  //--------------------------------------------------
  TYPE_OBJECTID m_DrainCreatureID;

  // PET ID
  TYPE_OBJECTID m_PetID;
  TYPE_OBJECTID m_ElementalID;

  //--------------------------------------------------
  // HPÀÇ º¯È­·®À» ±â¾ï
  //--------------------------------------------------
  HPMODIFYLIST m_HPModifyList;
  BYTE m_GradeActionInfo; // µ¿ÀÛÇÏ·Á´Â Action ÀÇ ´Ü°è.

  int m_HeadSkin;

  BYTE m_NickNameType;
  std::string m_NickName;

  //--------------------------------------------------
  // °ø¼ºÀü Âü°¡ ÁßÀÎÁö..
  // 1,2 : ¼öºñÃø
  // 3~7 : °ø°ÝÃø
  //--------------------------------------------------
  BYTE m_bSiegeWar;

  // ¹ö´× ¼Ö
  BYTE m_bBurningSolCount;
  bool m_bBurningSol;

  BYTE m_bInstallTurretCount;
  BYTE m_bInstallTurretDirect;
  int m_current_sprite_index;

  std::string m_PersnalMessage;
  int m_Persnal_flag;
  bool m_Input_Chat_Flag;

  bool m_IsSkipMotorStand;

  CTickTimer m_timerBattleStand;
  bool m_bBattleStand;

  bool m_bSkipShadow;
};

#endif
