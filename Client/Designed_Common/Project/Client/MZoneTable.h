//----------------------------------------------------------------------
// MZoneTable.h
//----------------------------------------------------------------------
// Zone Loading시에 Zone마다 필요한 정보를 저장하는 class
//----------------------------------------------------------------------
//
// [ Zone별로 Loadind에 필요한 정보 ]
//
// - ZoneID
// - Zone크기Type (Large or Small)
// - file이름
//
//----------------------------------------------------------------------

#ifndef __MZONETABLE_H__
#define __MZONETABLE_H__

#include "MString.h"
#include "MTypeDef.h"
#include <Windows.h>
#include <list>
#include <map>
#include <vector>


#define FLAG_ZONESIZE_SMALL 0x01

// 2004, 5, 14 sobeit add start - pk zone 관련
#define PK_TYPE_NULL 0       // 그냥 default map
#define PK_TYPE_DISABLE 0x01 // 다 우리편..-_-; pk 금지
#define PK_TYPE_ALL 0x02     // 나 빼고 다 적
#define PK_TYPE_GUILD 0x03   // 우리 길드 빼고 다 적..
#define PK_TYPE_SIEGE 0x04   // Siege 맵 // effect status에따라 적
#define PK_TYPE_USER_PK 0x10 // 유저간 PK 가능 지역

// 2007.01.17 by chyaya - 월드 맵 관련 정보
class WORLDMAP_INFO {
public:
  WORLDMAP_INFO();

public:
  enum WORLDMAP_TYPE { WORLDMAP_HELEA, WORLDMAP_MAX };
  enum PAYMENT_TYPE { PAYMENT_FREE, PAYMENT_PREMIUM, PAYMENT_MAX };

  typedef std::vector<TYPE_CREATURETYPE> CREATURETYPE_VEC;

public:
  void SaveToFile(std::ofstream &file);
  void LoadFromFile(ivfstream &file);

public:
  WORLDMAP_TYPE WorldMapType; // 월드 맵 종류 (WORLDMAP_MAX로 세팅되어 있으면
                              // 아래 정보까지 저장안됨)
  PAYMENT_TYPE PaymentType; // 결제 종류

  RECT WorldMapPosition; // 월드 맵에서의 위치

  BYTE EncourageSlayerLevelMin; // 권장 레벨 슬레이어
  BYTE EncourageSlayerLevelMax;

  BYTE EncourageVampireLevelMin; // 권장 레벨 뱀파이어
  BYTE EncourageVampireLevelMax;

  BYTE EncourageOustersLevelMin; // 권장 레벨 아우스터즈
  BYTE EncourageOustersLevelMax;

  MString MiscDescription; // 기타 정보
};

// 2004, 5, 14 sobeit add end - pk zone 관련
//----------------------------------------------------------------------
//
// 한 종류의 Zone에 대한 정보
//
//----------------------------------------------------------------------
class ZONETABLE_INFO {
public:
  typedef std::list<TYPE_SOUNDID> SOUNDID_LIST;
  typedef std::vector<MString> MAPSHAPE_VEC;

public:
  TYPE_ZONEID ID;       // Zone ID
  MString Name;         // Zone 이름
  BYTE Property;        // Zone의 속성
  MString Filename;     // Zone File이름
  MString InfoFilename; // ZoneInfo File이름
  MString TeenFilename; // Minimap File이름
                        //		TYPE_MUSICID			MusicIDSlayer; // Music종류
  //		TYPE_MUSICID			MusicIDVampire; // Music종류
  TYPE_MUSICID MusicID;     // Music종류
  SOUNDID_LIST SoundIDList; // random하게 나는 소리들..
  WORD CreatureColorSet;    // Zone 전체의 몹 색깔 바꾸기
  bool Safety;
  unsigned short MinimapSpriteID; // 미니맵의 스프라이트 아이디
  bool HolyLand;                  // 아담의 성지인가
  bool ChatMaskByRace;            // 종족에 따라 채팅내용에 마스크를 씌우는가
  bool CannotAttackInSafe;        // 안전지대에서 공격할 수 없는가
  bool CannotUseSpecialItem;      // 변신이나 소환 금지 여부
  bool CompetenceZone;
  BYTE PKType;                // 자유 PK 지역인가?
  WORLDMAP_INFO WorldMapInfo; // 월드 맵 관련 정보

  MAPSHAPE_VEC MapShapeList; // 맵의 다양한 모양

public:
  ZONETABLE_INFO();

  TYPE_SOUNDID GetRandomSoundID() const;
  bool IsShownInWorldMap() const // 월드맵에서 보이는 맵인가?
  {
    return WorldMapInfo.WorldMapType != WORLDMAP_INFO::WORLDMAP_MAX;
  }

  //-------------------------------------------------------
  // File I/O
  //-------------------------------------------------------
  void SaveToFile(std::ofstream &file);
  void LoadFromFile(ivfstream &file);
};

//----------------------------------------------------------------------
//
// 전체 Zone에 대한 정보
//
//----------------------------------------------------------------------
class CZoneTable {
public:
  typedef std::map<TYPE_ZONEID, ZONETABLE_INFO *> ZONEINFO_MAP;

public:
  CZoneTable();
  ~CZoneTable();

  //-------------------------------------------------------
  // Release
  //-------------------------------------------------------
  void Release();

  //-------------------------------------------------------
  // zoneID가 id인 zone의 정보를 얻는다.
  //-------------------------------------------------------
  bool Add(ZONETABLE_INFO *pZoneInfo);
  ZONETABLE_INFO *Get(TYPE_ZONEID id);

  const ZONEINFO_MAP &GetZoneMap() const { return m_mapZoneInfo; }

  //-------------------------------------------------------
  // File I/O
  //-------------------------------------------------------
  void SaveToFile(std::ofstream &file);
  void LoadFromFile(ivfstream &file);

protected:
  ZONEINFO_MAP m_mapZoneInfo; // zone정보 map
};

extern CZoneTable *g_pZoneTable;

#endif
