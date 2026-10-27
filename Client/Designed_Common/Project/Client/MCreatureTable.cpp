#include "MCreatureTable.h"
#include "crash_trace_tool.h"
#include "Client_PCH.h"
#include "DebugInfo.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectStatusDef.h"

// DarkEden XOR Key: "zhegeshixiexuede" (15 chars)
const char* G_XOR_KEY = "zhegeshixiexuede";
const int G_KEY_LEN = 15;

// Global instance (Needed by Linker)
CREATURE_TABLE* g_pCreatureTable = NULL;
CreatureSpriteTypeMapper* g_pCreatureSpriteTypeMapper = NULL;

//----------------------------------------------------------------------
// XOR Decryption - SMART BYPASS for BINARY FILES
//----------------------------------------------------------------------
void DecryptCreatureTable(CREATURE_TABLE* pTable) {
    if (pTable == NULL) return;
    int count = pTable->GetSize();
    if (count <= 0) return;

    void* pRawData = (void*)&((*pTable)[0]); 
    if (pRawData == NULL) return;

    unsigned char* pData = (unsigned char*)pRawData;
    
    // BINARY DETECTION: 
    // In DarkEden's unencrypted binary tables, the first few bytes are usually small values (IDs).
    // If [0], [1], [4] are all small (e.g. < 128), it's already binary. XORing would corrupt it.
    if (pData[0] < 128 && pData[1] < 128 && pData[4] < 64) {
        CT_Done("DEBUG: CreatureTable is BINARY. Bypassing XOR to prevent 0x1 vtable crash.");
        return;
    }

    CT_Done("DEBUG: CreatureTable looks XOR-Encrypted. Applying Decryption Armor...");
    int totalBytes = count * sizeof(CREATURETABLE_INFO);
    for (int i = 0; i < totalBytes; i++) {
        pData[i] ^= (unsigned char)G_XOR_KEY[i % G_KEY_LEN];
    }
    CT_Done("DEBUG: XOR Decryption Success.");
}

//----------------------------------------------------------------------
// ITEM_WEARINFO Methods — FAZ 4 (2026-05-03) referans port
// Modern projede yok, vampir NPC için CREATURETABLE_INFO::LoadFromFile
// içinde new ITEM_WEARINFO çağrısı linker fail eder. Trivial constructor
// + assignment + binary I/O.
//----------------------------------------------------------------------
ITEM_WEARINFO::ITEM_WEARINFO()
{
    skinColor = 0; hairColor = 0; jacketColor = 0; pantsColor = 0;
    helmetColor = 0; weaponColor = 0; shieldColor = 0; motorcycleColor = 0;
    hair = 0; jacket = 0; pants = 0; helmet = 0;
    weapon = 0; shield = 0; motorcycle = 0;
}

void ITEM_WEARINFO::operator=(const ITEM_WEARINFO& info)
{
    skinColor = info.skinColor; hairColor = info.hairColor;
    jacketColor = info.jacketColor; pantsColor = info.pantsColor;
    helmetColor = info.helmetColor; weaponColor = info.weaponColor;
    shieldColor = info.shieldColor; motorcycleColor = info.motorcycleColor;
    hair = info.hair; jacket = info.jacket; pants = info.pants;
    helmet = info.helmet; weapon = info.weapon; shield = info.shield;
    motorcycle = info.motorcycle;
}

void ITEM_WEARINFO::SaveToFile(std::ofstream& file)
{
    file.write((const char*)&skinColor, 2);
    file.write((const char*)&hairColor, 2);
    file.write((const char*)&jacketColor, 2);
    file.write((const char*)&pantsColor, 2);
    file.write((const char*)&helmetColor, 2);
    file.write((const char*)&weaponColor, 2);
    file.write((const char*)&shieldColor, 2);
    file.write((const char*)&motorcycleColor, 2);
    file.write((const char*)&hair, 1);
    file.write((const char*)&jacket, 1);
    file.write((const char*)&pants, 1);
    file.write((const char*)&helmet, 1);
    file.write((const char*)&weapon, 1);
    file.write((const char*)&shield, 1);
    file.write((const char*)&motorcycle, 1);
}

void ITEM_WEARINFO::LoadFromFile(ivfstream& file)
{
    file.read((char*)&skinColor, 2);
    file.read((char*)&hairColor, 2);
    file.read((char*)&jacketColor, 2);
    file.read((char*)&pantsColor, 2);
    file.read((char*)&helmetColor, 2);
    file.read((char*)&weaponColor, 2);
    file.read((char*)&shieldColor, 2);
    file.read((char*)&motorcycleColor, 2);
    file.read((char*)&hair, 1);
    file.read((char*)&jacket, 1);
    file.read((char*)&pants, 1);
    file.read((char*)&helmet, 1);
    file.read((char*)&weapon, 1);
    file.read((char*)&shield, 1);
    file.read((char*)&motorcycle, 1);
}

//----------------------------------------------------------------------
// CREATURETABLE_INFO Methods
//----------------------------------------------------------------------
CREATURETABLE_INFO::CREATURETABLE_INFO() {
    MoveTimesMotor = 1; bMale = true; ColorSet = 0; m_pActionSound = NULL; m_pActionCount = NULL;
    bFlyingCreature = false; FlyingHeight = 0; bHeadCut = false; HPBarWidth = 120;
    ChangeColorSet = 0xFFFF; pItemWearInfo = NULL; ShadowCount = 0; Width = -1;
    EffectStatus = EFFECTSTATUS_NULL; EffectSpriteType = MAX_EFFECTSPRITETYPE;
    bFade = false; bFadeShadow = false;
}

CREATURETABLE_INFO::~CREATURETABLE_INFO() {
    if (m_pActionSound != NULL) { delete[] m_pActionSound; m_pActionSound = NULL; }
    if (m_pActionCount != NULL) { delete[] m_pActionCount; m_pActionCount = NULL; }
}

void CREATURETABLE_INFO::SetCreatureTribe(enum CREATURETRIBE ct, int nMaxAction) {
    m_CreatureTribe = ct;
    if (m_pActionSound != NULL) delete[] m_pActionSound;
    if (m_pActionCount != NULL) delete[] m_pActionCount;
    int max = (nMaxAction == 0) ? GetActionMax() : nMaxAction;
    if (max <= 0) max = 80;
    m_pActionSound = new TYPE_SOUNDID[max];
    m_pActionCount = new int[max];
    for(int i=0; i<max; i++) { m_pActionSound[i] = SOUNDID_NULL; m_pActionCount[i] = 0; }
}

// FAZ 4 (2026-05-03): GERÇEK IMPLEMENTATION — eski boş stub yüzünden
// CreatureTable 2017 entry default-constructed boş kalıyordu. SpriteTypes
// vektörü hep size=0 → MCreature::SetCreatureType numFrames=0 →
// m_CreatureFrameIDs boş → DrawCreature GetCreatureFrameCount()=0 →
// karakter render fail (görünmez + animasyon yok = hareketsiz görünür).
//
// Referans: Referans Proje/darkeden/.../MCreatureTable.cpp:343
// Sequential binary read; SpriteTypes count + array, sonra fields, sonra
// per-action SoundID/Count, ItemWearInfo (vampir NPC için), bFade flags.
// `tmp` variable referansta dangling (özel edition +5 byte tail) —
// local DWORD discard ile koru.
void CREATURETABLE_INFO::LoadFromFile(ivfstream& file)
{
    static int s_entryIdx = 0;
    int myIdx = s_entryIdx++;
    long long entryStart = (long long)file.tellg();

    Name.LoadFromFile(file);

    int stcount = 0;
    file.read((char*)&stcount, 4);
    if (stcount < 0 || stcount > 1000) {
        CT_Done("CTLF: entry[%d] BAD stcount=%d at pos=%lld (entry started at %lld)",
            myIdx, stcount, (long long)file.tellg(), entryStart);
        return;
    }
#ifndef __INIT_INFO_
    SpriteTypes.Init(stcount);
#endif
    for (int st = 0; st < stcount; st++) {
        file.read((char*)&SpriteTypes[st], 4);
    }

    file.read((char*)&bMale, 1);

    BYTE ct;
    file.read((char*)&ct, 1);
    SetCreatureTribe((enum CREATURETRIBE)ct);  // soundID array init

    file.read((char*)&MoveTimes, 1);
    file.read((char*)&MoveRatio, 1);
    file.read((char*)&MoveTimesMotor, 1);
    file.read((char*)&Height, 4);
    file.read((char*)&Width, 4);
    file.read((char*)&DeadHeight, 4);
    file.read((char*)&DeadActionInfo, SIZE_ACTIONINFO);
    file.read((char*)&ColorSet, 4);
    file.read((char*)&bFlyingCreature, 1);
    file.read((char*)&FlyingHeight, 4);
    file.read((char*)&bHeadCut, 4);
    file.read((char*)&HPBarWidth, 4);
    file.read((char*)&ChangeColorSet, 2);
    file.read((char*)&ShadowCount, 2);
    file.read((char*)&EffectStatus, 4);
    file.read((char*)&Level, 4);

    int max = GetActionMax();
    for (int i = 0; i < max; i++) {
        file.read((char*)&m_pActionSound[i], SIZE_SOUNDID);
    }
    for (int i = 0; i < max; i++) {
        file.read((char*)&m_pActionCount[i], 4);
    }

    if (pItemWearInfo != NULL) {
        delete pItemWearInfo;
        pItemWearInfo = NULL;
    }

    bool bExistItemWearInfo = false;
    file.read((char*)&bExistItemWearInfo, 1);

    if (bExistItemWearInfo) {
        pItemWearInfo = new ITEM_WEARINFO;
        pItemWearInfo->LoadFromFile(file);
    }

    file.read((char*)&bFade, 1);
    file.read((char*)&bFadeShadow, 1);

    file.read((char*)&EffectSpriteType, 4);

    DWORD tmp = 0;  // referans custom edition tail (orig 238 + 5 byte)
    file.read((char*)&tmp, 4);

    long long entryEnd = (long long)file.tellg();
    long long entrySize = entryEnd - entryStart;
    // Her entry için tek satır özet — tribe + size + name length tribe max'ları
    // kalibre etmek için yeterli veri.
    if (myIdx < 50 || myIdx % 100 == 0) {
        CT_Done("CTLF: entry[%d] tribe=%d nameLen=%d size=%lld pos=%lld->%lld max=%d",
            myIdx, (int)m_CreatureTribe, (int)Name.GetLength(), entrySize, entryStart, entryEnd, GetActionMax());
    }
}

int CREATURETABLE_INFO::GetActionMax() const {
    // 2026-05-07: Tam kalibrasyon — orijinal Korean DarkEden source ile birebir.
    // Kaynak: Referans Proje/.../Client/MCreatureTable.cpp:230 + MTypeDef.h:122-218
    //   ACTION_MAX_SLAYER  = 40 (enum sonu)
    //   ACTION_MAX_VAMPIRE = 17 (enum sonu)
    //   ACTION_MAX_OUSTERS = 22 (enum sonu) — Phoenix yorumunda "20" yanlıştı
    //   ACTION_MAX_NPC     = 2  (#define, kullanılmıyor — NPC→VAMPIRE rerouted)
    // Doğrulama: Umbra creature.inf 2275/2275 entry parse + byte-perfect roundtrip
    //   Python parser: tools/inf_editors/creature_inf_editor.py
    switch (m_CreatureTribe) {
        case CREATURETRIBE_SLAYER:
        case CREATURETRIBE_SLAYER_NPC:    return 40;  // ACTION_MAX_SLAYER
        case CREATURETRIBE_VAMPIRE:
            // Drake/Gilles de Rais (SpriteTypes[0]==204) Ousters frame seti kullanır
            if (SpriteTypes.GetSize() > 0 && SpriteTypes[0] == 204)
                return 22;                            // ACTION_MAX_OUSTERS
            return 17;                                // ACTION_MAX_VAMPIRE
        case CREATURETRIBE_OUSTERS:
        case CREATURETRIBE_OUSTERS_NPC:   return 22;  // ACTION_MAX_OUSTERS
        // CREATURETRIBE_NPC (2) + UNKNOW1..5 (6,7,8,9,10) hepsi VAMPIRE (=17)
        default:                          return 17;
    }
}

//----------------------------------------------------------------------
// Mapper Methods
//----------------------------------------------------------------------
CreatureSpriteTypeMapper::CreatureSpriteTypeMapper() {}
CreatureSpriteTypeMapper::~CreatureSpriteTypeMapper() { Release(); }
void CreatureSpriteTypeMapper::Init(int numSpriteTypes) { Release(); if (numSpriteTypes < 0 || numSpriteTypes > 10000) numSpriteTypes = 0; m_CreatureSpriteTypes.resize(numSpriteTypes, NULL); }
void CreatureSpriteTypeMapper::Release() { for (size_t i = 0; i < m_CreatureSpriteTypes.size(); i++) { if (m_CreatureSpriteTypes[i] != NULL) { delete m_CreatureSpriteTypes[i]; m_CreatureSpriteTypes[i] = NULL; } } m_CreatureSpriteTypes.clear(); }
void CreatureSpriteTypeMapper::AddCreatureType(TYPE_SPRITEID spriteID, WORD creatureType) { if (spriteID >= m_CreatureSpriteTypes.size()) m_CreatureSpriteTypes.resize(spriteID + 1, NULL); if (m_CreatureSpriteTypes[spriteID] == NULL) m_CreatureSpriteTypes[spriteID] = new CREATURE_TYPES; m_CreatureSpriteTypes[spriteID]->push_back(creatureType); }
int CreatureSpriteTypeMapper::GetRandomCreatureType(TYPE_SPRITEID spriteID) const { if (spriteID >= m_CreatureSpriteTypes.size() || m_CreatureSpriteTypes[spriteID] == NULL || m_CreatureSpriteTypes[spriteID]->empty()) return 28; return (*m_CreatureSpriteTypes[spriteID])[rand() % m_CreatureSpriteTypes[spriteID]->size()]; }
void CreatureSpriteTypeMapper::SaveToFile(std::ofstream &file)
{
    int numSpriteTypes = (int)m_CreatureSpriteTypes.size();
    file.write((const char*)&numSpriteTypes, 4);

    for (int i = 0; i < numSpriteTypes; ++i)
    {
        CREATURE_TYPES* pCreatureTypes = m_CreatureSpriteTypes[i];
        int numCreatureTypes = (pCreatureTypes == NULL) ? 0 : (int)pCreatureTypes->size();
        file.write((const char*)&numCreatureTypes, 4);

        for (int j = 0; j < numCreatureTypes; ++j)
        {
            WORD creatureType = (*pCreatureTypes)[j];
            file.write((const char*)&creatureType, 2);
        }
    }
}

void CreatureSpriteTypeMapper::LoadFromFile(ivfstream &file)
{
    int numSpriteTypes = 0;
    file.read((char*)&numSpriteTypes, 4);

    if (numSpriteTypes < 0 || numSpriteTypes > 10000)
    {
        Init(0);
        return;
    }

    Init(numSpriteTypes);

    for (int i = 0; i < numSpriteTypes; ++i)
    {
        int numCreatureTypes = 0;
        file.read((char*)&numCreatureTypes, 4);

        if (file.eof() || numCreatureTypes < 0 || numCreatureTypes > 10000)
        {
            Release();
            return;
        }

        if (numCreatureTypes == 0)
        {
            continue;
        }

        CREATURE_TYPES* pCreatureTypes = new CREATURE_TYPES;
        pCreatureTypes->reserve(numCreatureTypes);
        m_CreatureSpriteTypes[i] = pCreatureTypes;

        for (int j = 0; j < numCreatureTypes; ++j)
        {
            WORD creatureType = 0;
            file.read((char*)&creatureType, 2);

            if (file.eof())
            {
                Release();
                return;
            }

            pCreatureTypes->push_back(creatureType);
        }
    }
}
