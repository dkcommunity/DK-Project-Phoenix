//----------------------------------------------------------------------
// MZoneTable.cpp
//----------------------------------------------------------------------
#include "MZoneTable.h"
#include "Client_PCH.h"


//----------------------------------------------------------------------
// SOUND_SOURCE
//----------------------------------------------------------------------
/*
class SOUND_SOURCE
{
        public :
                SOUND_SOURCE();
                ~SOUND_SOURCE();

        //-------------------------------------------------------
        // zone의 (x, y)에서 TimeGap
        //-------------------------------------------------------
        public :
                int				x, y;
                int				MinTimeGap;
// 최소한 MinTimeGap후에 play해야한다. int
MaxTimeGap;			// MaxTime안에는 play해야한다. int
SoundID; int				NextPlayTime;		// 다음에
play해도 되는 시간

        public :
                //-------------------------------------------------------
                // File I/O
                //-------------------------------------------------------
                void			SaveToFile(std::ofstream& file);
                void			LoadFromFile(ivfstream& file);
};
*/

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
CZoneTable *g_pZoneTable = NULL;

//----------------------------------------------------------------------
//
//						WORLDMAP_INFO
//
//----------------------------------------------------------------------
WORLDMAP_INFO::WORLDMAP_INFO() {
  WorldMapType = WORLDMAP_MAX;
  PaymentType = PAYMENT_FREE;

  EncourageSlayerLevelMin = 0;
  EncourageSlayerLevelMax = 0;

  EncourageVampireLevelMin = 0;
  EncourageVampireLevelMax = 0;

  EncourageOustersLevelMin = 0;
  EncourageOustersLevelMax = 0;
}

void WORLDMAP_INFO::SaveToFile(std::ofstream &file) {
  file.write((const char *)&WorldMapType, sizeof(WORLDMAP_TYPE));

  if (WorldMapType != WORLDMAP_MAX) {
    file.write((const char *)&PaymentType, sizeof(PAYMENT_TYPE));
    file.write((const char *)&WorldMapPosition, sizeof(RECT));

    /*
    BYTE AppearCreatureSize = static_cast<BYTE>(AppearCreatureVec.size());
    file.write((const char*)&AppearCreatureSize, sizeof(BYTE));

    CREATURETYPE_VEC::iterator pos		= AppearCreatureVec.begin();
    CREATURETYPE_VEC::iterator endpos	= AppearCreatureVec.end();

    for(; pos != endpos; ++pos)
            file.write((const char*)&*pos, SIZE_CREATURETYPE);
    */

    file.write((const char *)&EncourageSlayerLevelMin, sizeof(BYTE));
    file.write((const char *)&EncourageSlayerLevelMax, sizeof(BYTE));

    file.write((const char *)&EncourageVampireLevelMin, sizeof(BYTE));
    file.write((const char *)&EncourageVampireLevelMax, sizeof(BYTE));

    file.write((const char *)&EncourageOustersLevelMin, sizeof(BYTE));
    file.write((const char *)&EncourageOustersLevelMax, sizeof(BYTE));

    MiscDescription.SaveToFile(file);
  }
}

void WORLDMAP_INFO::LoadFromFile(ivfstream &file) {
  file.read((char *)&WorldMapType, sizeof(WORLDMAP_TYPE));

  if (WorldMapType != WORLDMAP_MAX) {
    file.read((char *)&PaymentType, sizeof(PAYMENT_TYPE));
    file.read((char *)&WorldMapPosition, sizeof(RECT));

    /*
    BYTE AppearCreatureSize;
    file.read((char*)&AppearCreatureSize, sizeof(BYTE));

    for(int i = 0; i < AppearCreatureSize; ++i) {
            TYPE_CREATURETYPE creatureType;
            file.read((char*)&creatureType, SIZE_CREATURETYPE);
            AppearCreatureVec.push_back(creatureType);
    }
    */

    file.read((char *)&EncourageSlayerLevelMin, sizeof(BYTE));
    file.read((char *)&EncourageSlayerLevelMax, sizeof(BYTE));

    file.read((char *)&EncourageVampireLevelMin, sizeof(BYTE));
    file.read((char *)&EncourageVampireLevelMax, sizeof(BYTE));

    file.read((char *)&EncourageOustersLevelMin, sizeof(BYTE));
    file.read((char *)&EncourageOustersLevelMax, sizeof(BYTE));

    MiscDescription.LoadFromFile(file);
  }
}

//----------------------------------------------------------------------
//
//						ZONETABLE_INFO
//
//----------------------------------------------------------------------
ZONETABLE_INFO::ZONETABLE_INFO() {
  Property = 0;
  CreatureColorSet = 0xFFFF;
  Safety = false;
  HolyLand = false;
  ChatMaskByRace = true;
  CannotAttackInSafe = false;
  CannotUseSpecialItem = false;
  CompetenceZone = false;
  PKType = PK_TYPE_NULL;
  MusicID = MUSICID_NULL;

  //	MusicIDSlayer	= MUSICID_NULL;
  //	MusicIDVampire	= MUSICID_NULL;
}

//----------------------------------------------------------------------
// Get Random SoundID
//----------------------------------------------------------------------
TYPE_SOUNDID
ZONETABLE_INFO::GetRandomSoundID() const {
  int numSound = SoundIDList.size();

  if (numSound == 0) {
    return SOUNDID_NULL;
  }

  int select = rand() % numSound;

  SOUNDID_LIST::const_iterator iID = SoundIDList.begin();

  // select번째 id를 선택한다.
  for (int i = 0; i < select; i++) {
    iID++;
  }

  return *iID;
}

//----------------------------------------------------------------------
// Save
//----------------------------------------------------------------------
void ZONETABLE_INFO::SaveToFile(std::ofstream &file) {
  file.write((const char *)&ID, SIZE_ZONEID);
  Name.SaveToFile(file);
  file.write((const char *)&Property, 1);
  //	file.write((const char*)&MusicIDSlayer, SIZE_MUSICID);
  //	file.write((const char*)&MusicIDVampire, SIZE_MUSICID);
  file.write((const char *)&MusicID, SIZE_MUSICID);
  Filename.SaveToFile(file);
  InfoFilename.SaveToFile(file);
  TeenFilename.SaveToFile(file);

  // sound ID들 저장
  int numSound = SoundIDList.size();

  file.write((const char *)&numSound, 4);

  SOUNDID_LIST::iterator iID = SoundIDList.begin();

  while (iID != SoundIDList.end()) {
    TYPE_SOUNDID soundID = *iID;

    file.write((const char *)&soundID, SIZE_SOUNDID);

    iID++;
  }

  file.write((const char *)&Safety, 1);
  file.write((const char *)&CreatureColorSet, 2);

  file.write((char *)&MinimapSpriteID, 2);
  file.write((const char *)&HolyLand, 1);
  file.write((const char *)&ChatMaskByRace, 1);
  file.write((const char *)&CannotAttackInSafe, 1);
  file.write((const char *)&CannotUseSpecialItem, 1);
  file.write((const char *)&CompetenceZone, 1);
  file.write((const char *)&PKType, 1);

  WorldMapInfo.SaveToFile(file);

  // Map Shape List
  BYTE mapShapeListSize = (BYTE)MapShapeList.size();
  file.write((const char *)&mapShapeListSize, sizeof(BYTE));

  MAPSHAPE_VEC::iterator pos = MapShapeList.begin();
  for (; pos != MapShapeList.end(); ++pos)
    pos->SaveToFile(file);
}

//----------------------------------------------------------------------
// Load
//----------------------------------------------------------------------
void ZONETABLE_INFO::LoadFromFile(ivfstream &file) {
  file.read((char *)&ID, SIZE_ZONEID);
  Name.LoadFromFile(file);
  file.read((char *)&Property, 1);
  //	file.read((char*)&MusicIDSlayer, SIZE_MUSICID);
  //	file.read((char*)&MusicIDVampire, SIZE_MUSICID);
  file.read((char *)&MusicID, SIZE_MUSICID);
  Filename.LoadFromFile(file);
  InfoFilename.LoadFromFile(file);
  TeenFilename.LoadFromFile(file);

  // sound id들 loading
  SoundIDList.clear();

  int numSound;
  file.read((char *)&numSound, 4);

  // Sanity Check: Prevent huge or negative values looping endlessly
  if (numSound < 0 || numSound > 1000)
    numSound = 0;

  TYPE_SOUNDID soundID;
  for (int i = 0; i < numSound; i++) {
    file.read((char *)&soundID, SIZE_SOUNDID);

    SoundIDList.push_back(soundID);
  }

  file.read((char *)&Safety, 1);
  file.read((char *)&CreatureColorSet, 2);

  file.read((char *)&MinimapSpriteID, 2);
  file.read((char *)&HolyLand, 1);
  file.read((char *)&ChatMaskByRace, 1);
  file.read((char *)&CannotAttackInSafe, 1);
  file.read((char *)&CannotUseSpecialItem, 1);
  file.read((char *)&CompetenceZone, 1);
  file.read((char *)&PKType, 1);

  WorldMapInfo.LoadFromFile(file);

  // Map Shape List
  BYTE mapShapeListSize = 0;
  file.read((char *)&mapShapeListSize, sizeof(BYTE));

  MapShapeList.clear();

  for (int j = 0; j < mapShapeListSize; ++j) {
    MString str;
    str.LoadFromFile(file);
    MapShapeList.push_back(str);
  }
}

//----------------------------------------------------------------------
//
//							CZoneTable
//
//----------------------------------------------------------------------
CZoneTable::CZoneTable() {}

CZoneTable::~CZoneTable() { Release(); }

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void CZoneTable::Release() {
  ZONEINFO_MAP::iterator iInfo = m_mapZoneInfo.begin();

  // 모든 ZoneInfo를 메모리에서 지운다.
  while (iInfo != m_mapZoneInfo.end()) {
    ZONETABLE_INFO *pInfo = (*iInfo).second;

    if (pInfo != NULL) {
      delete pInfo;
    }

    iInfo++;
  }

  m_mapZoneInfo.clear();
}

//----------------------------------------------------------------------
// Add ZoneInfo
//----------------------------------------------------------------------
// ZoneInfo를 추가한다.
//----------------------------------------------------------------------
bool CZoneTable::Add(ZONETABLE_INFO *pZoneInfo) {
  if (pZoneInfo == NULL) {
    return false;
  }

  ZONEINFO_MAP::iterator iInfo = m_mapZoneInfo.find(pZoneInfo->ID);

  // 이미 있으면.. 지운다.
  if (iInfo != m_mapZoneInfo.end()) {
    delete pZoneInfo;

    return false;
  }

  // 추가한다.
  m_mapZoneInfo.insert(ZONEINFO_MAP::value_type(pZoneInfo->ID, pZoneInfo));

  return true;
}

//----------------------------------------------------------------------
// Get ZoneInfo
//----------------------------------------------------------------------
// zoneID가 id인 zoneInfo를 얻는다.
//----------------------------------------------------------------------
ZONETABLE_INFO *CZoneTable::Get(TYPE_ZONEID id) {
  ZONEINFO_MAP::iterator iInfo = m_mapZoneInfo.find(id);

  // 없으면..
  if (iInfo == m_mapZoneInfo.end()) {
    return NULL;
  }

  return (*iInfo).second;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
// size , 각각의 ZoneInfo
//----------------------------------------------------------------------
void CZoneTable::SaveToFile(std::ofstream &file) {
  //---------------------------------------------
  // 전체 개수 저장
  //---------------------------------------------
  int size = m_mapZoneInfo.size();

  file.write((const char *)&size, 4);

  //---------------------------------------------
  // 각각의 zoneInfo를 저장한다.
  //---------------------------------------------
  ZONEINFO_MAP::iterator iInfo = m_mapZoneInfo.begin();

  while (iInfo != m_mapZoneInfo.end()) {
    ZONETABLE_INFO *pInfo = (*iInfo).second;

    if (pInfo != NULL) {
      pInfo->SaveToFile(file);
    }

    iInfo++;
  }
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
// size , 각각의 ZoneInfo
//----------------------------------------------------------------------
void CZoneTable::LoadFromFile(ivfstream &file) {
  //---------------------------------------------
  // 기존에 있던 메모리 삭제
  //---------------------------------------------
  Release();

  //---------------------------------------------
  // size 읽어오기
  //---------------------------------------------
  int size;

  file.read((char *)&size, 4);

  // Sanity Check: Prevent huge allocations from corrupted files causing
  // bad_alloc
  if (size < 0 || size > 10000)
    return;

  //---------------------------------------------
  // 각각의 ZoneInfo를 Load한다.
  //---------------------------------------------
  for (int i = 0; i < size; i++) {
    ZONETABLE_INFO *pInfo = new ZONETABLE_INFO;

    pInfo->LoadFromFile(file);

    Add(pInfo);
  }
}
