//////////////////////////////////////////////////////////////////////////////
// Filename    : ModifyInfo.h
// Written By  : elca@ewestsoft.com
// Description :
// 플레이어의 상태가 변할 때, 클라이언트에게 날아가는 패킷이다.
// 주로 플레이어가 쓰는 기술에 대한 결과 패킷이 이 패킷을 상속받아서
// 쓰는 경우가 많다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __MODIFY_INFO_H__
#define __MODIFY_INFO_H__

#include "Exception.h"
#include "Packet.h"
#include "Types.h"
#include <list>


//////////////////////////////////////////////////////////////////////////////
// 상태 변화 타입
//////////////////////////////////////////////////////////////////////////////
/*
enum ModifyType
{
        MODIFY_BASIC_STR = 0,        // 기본 STR을 변경
        MODIFY_CURRENT_STR,          // 현재 STR을 변경
        MODIFY_MAX_STR,              // 맥스 STR을 변경
        MODIFY_STR_EXP,              // STR 경험치를 변경
        MODIFY_BASIC_DEX,            // 기본 DEX를 변경
        MODIFY_CURRENT_DEX,          // 현재 DEX를 변경
        MODIFY_MAX_DEX,              // 맥스 DEX를 변경
        MODIFY_DEX_EXP,              // DEX 경험치를 변경
        MODIFY_BASIC_INT,            // 기본 INT를 변경
        MODIFY_CURRENT_INT,          // 현재 INT를 변경
        MODIFY_MAX_INT,              // 맥스 INT를 변경
        MODIFY_INT_EXP,              // INT 경험치를 변경
        MODIFY_CURRENT_HP,           // 현재 HP를 변경
        MODIFY_MAX_HP,               // 맥스 HP를 변경
        MODIFY_CURRENT_MP,           // 현재 MP를 변경
        MODIFY_MAX_MP,               // 맥스 MP를 변경
        MODIFY_MIN_DAMAGE,           // 최소 데미지를 변경
        MODIFY_MAX_DAMAGE,           // 맥스 데미지를 변경
        MODIFY_DEFENSE,              // 디펜스를 변경
        MODIFY_PROTECTION,           // 프로텍션을 변경
        MODIFY_TOHIT,                // 명중률을 변경
        MODIFY_VISION,               // 시야를 변경
        MODIFY_FAME,                 // 명성을 변경
        MODIFY_GOLD,                 // 소지금을 변경
        MODIFY_SWORD_DOMAIN_LEVEL,   // 검 계열 레벨을 변경
        MODIFY_SWORD_DOMAIN_EXP,     // 검 계열 레벨을 변경
        MODIFY_SWORD_DOMAIN_GOAL_EXP,// 검 계열 레벨을 변경
        MODIFY_BLADE_DOMAIN_LEVEL,   // 도 계열 레벨을 변경
        MODIFY_BLADE_DOMAIN_EXP,     // 도 계열 레벨을 변경
        MODIFY_BLADE_DOMAIN_GOAL_EXP,// 도 계열 레벨을 변경
        MODIFY_HEAL_DOMAIN_LEVEL,    // 힐 계열 레벨을 변경
        MODIFY_HEAL_DOMAIN_EXP,      // 힐 계열 레벨을 변경
        MODIFY_HEAL_DOMAIN_GOAL_EXP, // 힐 계열 레벨을 변경
        MODIFY_ENCHANT_DOMAIN_LEVEL, // 인챈트 계열 레벨을 변경
        MODIFY_ENCHANT_DOMAIN_EXP,   // 인챈트 계열 레벨을 변경
        MODIFY_ENCHANT_DOMAIN_GOAL_EXP, // 인챈트 계열 레벨을 변경
        MODIFY_GUN_DOMAIN_LEVEL,     // 총 계열 레벨을 변경
        MODIFY_GUN_DOMAIN_EXP,       // 총 계열 레벨을 변경
        MODIFY_GUN_DOMAIN_GOAL_EXP,  // 총 계열 레벨을 변경
        MODIFY_ETC_DOMAIN_LEVEL,     // 기타 계열 레벨을 변경
        MODIFY_ETC_DOMAIN_EXP,       // 기타 계열 레벨을 변경
        MODIFY_ETC_DOMAIN_GOAL_EXP,  // 기타 계열 레벨을 변경
        MODIFY_SKILL_LEVEL,          // 특정 스킬 레벨을 변경
        MODIFY_LEVEL,                // 뱀파이어 레벨을 변경
        MODIFY_EFFECT_STAT,          // 이펙트 상태를 변경
        MODIFY_DURATION,             // 이펙트 지속 시간을 변경
        MODIFY_BULLET,               // 들고 있는 총알의 총알 수를 변경
        MODIFY_BONUS_POINT,          // 뱀파이어의 보너스 포인트를 변경
        MODIFY_DURABILITY,           // 들고 있는 아이템 중 하나의 내구성을 변경
        MODIFY_NOTORIETY,            // 악명치를 변경
        MODIFY_VAMP_EXP,             // 뱀파이어의 경험치를 변경
        MODIFY_SILVER_DAMAGE,        // 은 데미지를 변경
        MODIFY_ATTACK_SPEED,         // 공격 속도를 변경
        MODIFY_ALIGNMENT,			 // 성향
        MODIFY_SILVER_DURABILITY,    // 은 도금양을 변경
        MODIFY_REGEN_RATE,           // 단위 시간당 리젠양을 변경
        MODIFY_GUILDID,				 // 길드 아이디를 변경
        MODIFY_RANK,				 // 계급 단계
        MODIFY_RANK_EXP,			 // 계급 경험치
        MODIFY_OUSTERS_EXP,          // 아우스터스의 경험치를 변경
        MODIFY_SKILL_BONUS_POINT,    // 아우스터스의 스킬 보너스 포인트를 변경

        MODIFY_MAX
};
*/

const char *const ModifyType2String[] = {
    "BASIC_STR",      // 기본 STR을 변경
    "CURRENT_STR",    // 현재 STR을 변경
    "MAX_STR",        // 맥스 STR을 변경
    "STR_EXP_REMAIN", // STR 경험치를 변경

    "BASIC_DEX",      // 기본 DEX를 변경
    "CURRENT_DEX",    // 현재 DEX를 변경
    "MAX_DEX",        // 맥스 DEX를 변경
    "DEX_EXP_REMAIN", // DEX 경험치를 변경

    "BASIC_INT",      // 기본 INT를 변경
    "CURRENT_INT",    // 현재 INT를 변경
    "MAX_INT",        // 맥스 INT를 변경
    "INT_EXP_REMAIN", // INT 경험치를 변경

    "CURRENT_HP", // 현재 HP를 변경
    "MAX_HP",     // 맥스 HP를 변경

    "CURRENT_MP", // 현재 MP를 변경
    "MAX_MP",     // 맥스 MP를 변경

    "MIN_DAMAGE", // 최소 데미지를 변경
    "MAX_DAMAGE", // 맥스 데미지를 변경

    "DEFENSE",    // 디펜스를 변경
    "PROTECTION", // 프로텍션을 변경
    "TOHIT",      // 명중률을 변경

    "VISION", // 시야를 변경
    "FAME",   // 명성을 변경
    "GOLD",   // 소지금을 변경

    "SWORD_DOMAIN_LEVEL",        // 검 계열 레벨을 변경
    "SWORD_DOMAIN_EXP_REMAIN",   // 검 계열 레벨을 변경
    "SWORD_DOMAIN_GOAL_EXP",     // 검 계열 레벨을 변경
    "BLADE_DOMAIN_LEVEL",        // 도 계열 레벨을 변경
    "BLADE_DOMAIN_EXP_REMAIN",   // 도 계열 레벨을 변경
    "BLADE_DOMAIN_GOAL_EXP",     // 도 계열 레벨을 변경
    "HEAL_DOMAIN_LEVEL",         // 힐 계열 레벨을 변경
    "HEAL_DOMAIN_EXP_REMAIN",    // 힐 계열 레벨을 변경
    "HEAL_DOMAIN_GOAL_EXP",      // 힐 계열 레벨을 변경
    "ENCHANT_DOMAIN_LEVEL",      // 인챈트 계열 레벨을 변경
    "ENCHANT_DOMAIN_EXP_REMAIN", // 인챈트 계열 레벨을 변경
    "ENCHANT_DOMAIN_GOAL_EXP",   // 인챈트 계열 레벨을 변경
    "GUN_DOMAIN_LEVEL",          // 총 계열 레벨을 변경
    "GUN_DOMAIN_EXP_REMAIN",     // 총 계열 레벨을 변경
    "GUN_DOMAIN_GOAL_EXP",       // 총 계열 레벨을 변경
    "ETC_DOMAIN_LEVEL",          // 기타 계열 레벨을 변경
    "ETC_DOMAIN_EXP_REMAIN",     // 기타 계열 레벨을 변경
    "ETC_DOMAIN_GOAL_EXP",       // 기타 계열 레벨을 변경

    "SKILL_LEVEL",     // 특정 스킬 레벨을 변경
    "LEVEL",           // 뱀파이어 레벨을 변경
    "EFFECT_STAT",     // 이펙트 상태를 변경
    "DURATION",        // 이펙트 지속 시간을 변경
    "BULLET",          // 들고 있는 총알의 총알 수를 변경
    "BONUS_POINT",     // 뱀파이어의 보너스 포인트를 변경
    "DURABILITY",      // 들고 있는 아이템 중 하나의 내구성을 변경
    "NOTORIETY",       // 악명치를 변경
    "VAMP_EXP_REMAIN", // 뱀파이어의 경험치를 변경

    "SILVER_DAMAGE", // 은 데미지를 변경
    "ATTACK_SPEED",  // 공격 속도를 변경
    "ALIGNMENT",     // 성향

    "SILVER_DURABILITY", // 은도금 남은거..

    "REGEN_RATE", // 단위 시간당 리젠양을 변경 일단쌩-_-
    "GUILDID",    // 길드 아이디 바꼈을때

    "RANK",            // 계급 단계
    "RANK_EXP_REMAIN", // 계급 경험치

    "OUSTERS_EXP_REMAIN", // 아우스터스의 경험치를 변경
    "SKILL_BONUS_POINT",  // 아우스터스의 스킬 보너스 포인트를 변경

    "ELEMENTAL_FIRE",
    "ELEMENTAL_WATER",
    "ELEMENTAL_EARTH",
    "ELEMENTAL_WIND",

    "SKILL_EXP", // 스킬 경험치 변경

    "PET_HP",
    "PET_EXP_REMAIN", // 펫 관련

    "LAST_TARGET",

    "UNIONID",
    "UNIONGRADE",

    "ADVANCEMENT_CLASS_LEVEL",    // 승직 레벨
    "ADVANCEMENT_CLASS_GOAL_EXP", // 승직 경험치

    "ATTACK_BLOOD_BURST_POINT",  // 공격 폭주 포인트
    "DEFENSE_BLOOD_BURST_POINT", // 수비 폭주 포인트
    "PARTY_BLOOD_BURST_POINT",   // 파티 폭주 포인트

    "ADVANCED_SLAYERSTR",
    "ADVANCED_SLAYERDEX",
    "ADVANCED_SLAYERINT",

#if __CONTENTS(__CONTRIBUTE_SYSTEM)
    "CONTRIBUTE_POINT", // MODIFY_CONTRIBUTE_POINT 기여도 점수
#endif
    "DIANJUAN",
    "JIFEN",
    "LUCK",
    "HPSTEAL",
    "MPSTEAL",
    "ALLRES",
    "CRI",
    "MONSTER_COUNTER",
    "MONSTER_COUNTER_GOAL",
    "MAX"};

//////////////////////////////////////////////////////////////////////////////
// 2바이트로 충분한 데이터일 경우, 이 구조체를 사용한다.
//////////////////////////////////////////////////////////////////////////////
typedef struct _SHORTDATA {
  BYTE type;
  ushort value;

} SHORTDATA;

//////////////////////////////////////////////////////////////////////////////
// 4바이트로 충분한 데이터일 경우, 이 구조체를 사용한다.
//////////////////////////////////////////////////////////////////////////////
typedef struct _LONGDATA {
  BYTE type;
  ulong value;

} LONGDATA;

//////////////////////////////////////////////////////////////////////////////
// class ModifyInfo;
// 게임서버에서 클라이언트로 자신의 변형된 데이터를 알려주기 위한 객채
// ModifyInformation, SkillToObjectOK 등에 실려서 날아간다.
//////////////////////////////////////////////////////////////////////////////

class ModifyInfo : public Packet {
public:
  ModifyInfo();
  virtual ~ModifyInfo();

public:
  void read(SocketInputStream &iStream);
  void write(SocketOutputStream &oStream) const;
  PacketSize_t getPacketSize() const {
    return szBYTE * 2 + m_ShortCount * (szBYTE + szshort) +
           m_LongCount * (szBYTE + szlong);
  }
  static PacketSize_t getPacketMaxSize() {
    return szBYTE * 2 + 255 * (szBYTE + szshort + szBYTE + szlong);
  }

#ifdef __DEBUG_OUTPUT__
  std::string toString() const;
#endif

public:
  BYTE getShortCount(void) const { return m_ShortCount; }
  BYTE getLongCount(void) const { return m_LongCount; }

  void addShortData(ModifyType type, ushort value);
  void addLongData(ModifyType type, ulong value);

  void popShortData(SHORTDATA &rData);
  void popLongData(LONGDATA &rData);

  void clearList(void) {
    m_ShortCount = 0;
    m_LongCount = 0;
    m_ShortList.clear();
    m_LongList.clear();
  }

protected:
  BYTE m_ShortCount;
  std::list<SHORTDATA> m_ShortList;

  BYTE m_LongCount;
  std::list<LONGDATA> m_LongList;
};

#endif
