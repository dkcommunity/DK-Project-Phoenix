//----------------------------------------------------------------------
// MNPCTable.cpp
//----------------------------------------------------------------------
#include "MNPCTable.h"
#include "Client_PCH.h"
#include "MCreatureTable.h"


//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MNPCTable *g_pNPCTable = NULL;

//----------------------------------------------------------------------
//
// NPC_INFO
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void NPC_INFO::Release() { ListShopTemplateID.clear(); }

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void NPC_INFO::SaveToFile(std::ofstream &file) {
  //--------------------------------------------------
  // ListShopTemplateID
  //--------------------------------------------------
  Name.SaveToFile(file);

  int size = ListShopTemplateID.size();

  file.write((const char *)&size, 4);

  SHOPTEMPLATEID_LIST::iterator iID = ListShopTemplateID.begin();

  while (iID != ListShopTemplateID.end()) {
    unsigned int id = *iID;

    file.write((const char *)&id, 4);

    iID++;
  }

  //--------------------------------------------------
  // Description
  //--------------------------------------------------
  Description.SaveToFile(file);
  file.write((const char *)&SpriteID, 4);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void NPC_INFO::LoadFromFile(ivfstream &file) {
  Release();

  Name.LoadFromFile(file);
  //--------------------------------------------------
  // ListShopTemplateID
  //--------------------------------------------------
  int size;

  file.read((char *)&size, 4);

  // Sanity Check: Prevent huge loop allocations
  if (size < 0 || size > 10000)
    size = 0;

  unsigned int id;

  for (int i = 0; i < size; i++) {
    file.read((char *)&id, 4);

    ListShopTemplateID.push_back(id);
  }

  //--------------------------------------------------
  // Description
  //--------------------------------------------------
  Description.LoadFromFile(file);
  file.read((char *)&SpriteID, 4);
}

//----------------------------------------------------------------------
//
// MNPCTable
//
//----------------------------------------------------------------------
MNPCTable::MNPCTable() {}

MNPCTable::~MNPCTable() {
  //	Release();
}

//----------------------------------------------------------------------
//
// SERVERNPC_INFO
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void SERVERNPC_INFO::Release() { ListShopTemplateID.clear(); }

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void SERVERNPC_INFO::SaveToFile(std::ofstream &file) {
  //--------------------------------------------------
  // ListShopTemplateID
  //--------------------------------------------------
  Name.SaveToFile(file);

  int size = ListShopTemplateID.size();

  file.write((const char *)&size, 4);

  SHOPTEMPLATEID_LIST::iterator iID = ListShopTemplateID.begin();

  while (iID != ListShopTemplateID.end()) {
    unsigned int id = *iID;

    file.write((const char *)&id, 4);

    iID++;
  }

  //--------------------------------------------------
  // Description
  //--------------------------------------------------
  Description.SaveToFile(file);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void SERVERNPC_INFO::LoadFromFile(ivfstream &file) {
  Release();

  Name.LoadFromFile(file);
  //--------------------------------------------------
  // ListShopTemplateID
  //--------------------------------------------------
  int size;

  file.read((char *)&size, 4);

  // Sanity Check: Prevent huge loop allocations
  if (size < 0 || size > 10000)
    size = 0;

  unsigned int id;

  for (int i = 0; i < size; i++) {
    file.read((char *)&id, 4);

    ListShopTemplateID.push_back(id);
  }

  //--------------------------------------------------
  // Description
  //--------------------------------------------------
  Description.LoadFromFile(file);
}

//----------------------------------------------------------------------
//
// MSERVERNPCTable
//
//----------------------------------------------------------------------
MServerNPCTable::MServerNPCTable() {

  // !!!!!!!!!!!!!    @@@@@@  @@@  @   @     @@   @@  @@@@@@    !!!!!!!!!!!!!
  // !!!!!!!!!!!!!    @        @    @ @      @ @ @ @  @         !!!!!!!!!!!!!
  // !!!!!!!!!!!!!    @@@@@@   @     @       @  @  @  @@@@@@    !!!!!!!!!!!!!
  // !!!!!!!!!!!!!    @        @    @ @      @     @  @         !!!!!!!!!!!!!
  // !!!!!!!!!!!!!    @       @@@  @   @     @     @  @@@@@@    !!!!!!!!!!!!!

  NPCSPRITEID.clear();

  NPCSPRITEID[157] = 1;
  NPCSPRITEID[17] = 2;
  NPCSPRITEID[16] = 3;
  NPCSPRITEID[18] = 4;
  NPCSPRITEID[26] = 5;
  NPCSPRITEID[21] = 6;
  NPCSPRITEID[97] = 7;
  NPCSPRITEID[100] = 8;
  NPCSPRITEID[24] = 9;
  NPCSPRITEID[14] = 10;
  NPCSPRITEID[110] = 11;
  NPCSPRITEID[108] = 12;
  NPCSPRITEID[101] = 13;
  NPCSPRITEID[95] = 14;
  NPCSPRITEID[109] = 15;
  NPCSPRITEID[103] = 16;
  NPCSPRITEID[27] = 17;
  NPCSPRITEID[98] = 18;
  NPCSPRITEID[15] = 19;
  NPCSPRITEID[20] = 20;
  NPCSPRITEID[96] = 21;
  NPCSPRITEID[11] = 22;
  NPCSPRITEID[102] = 23;
  NPCSPRITEID[23] = 24;
  NPCSPRITEID[158] = 25;
  NPCSPRITEID[99] = 26;
  NPCSPRITEID[25] = 27;
  NPCSPRITEID[163] = 28;
  NPCSPRITEID[162] = 29;
  NPCSPRITEID[164] = 30;
  NPCSPRITEID[12] = 31;
  NPCSPRITEID[13] = 32;
  NPCSPRITEID[22] = 33;
  NPCSPRITEID[159] = 34;
  NPCSPRITEID[160] = 35;
  NPCSPRITEID[161] = 36;
  NPCSPRITEID[19] = 37;
  NPCSPRITEID[93] = 38;
  NPCSPRITEID[92] = 39;
  NPCSPRITEID[94] = 40;
  NPCSPRITEID[219] = 41;
  NPCSPRITEID[220] = 42;
  NPCSPRITEID[253] = 48;
  NPCSPRITEID[254] = 47;
  NPCSPRITEID[255] = 49;
  NPCSPRITEID[217] = 43;
  NPCSPRITEID[250] = 44;
  NPCSPRITEID[251] = 44;
  NPCSPRITEID[252] = 44;
  NPCSPRITEID[296] = 45;
  NPCSPRITEID[297] = 46;
  NPCSPRITEID[362] = 50;
  NPCSPRITEID[363] = 51;
  NPCSPRITEID[364] = 52;
  NPCSPRITEID[365] = 53;
  NPCSPRITEID[366] = 54;
  NPCSPRITEID[302] = 45;
  NPCSPRITEID[303] = 46;
  NPCSPRITEID[311] = 45;
  NPCSPRITEID[368] = 39;
  NPCSPRITEID[370] = 35;
  NPCSPRITEID[427] = 55;
  NPCSPRITEID[428] = 56;
  NPCSPRITEID[429] = 57;
  NPCSPRITEID[430] = 58;
  NPCSPRITEID[519] = 59; // 아일린',
  NPCSPRITEID[521] = 60; // 알렌',
  NPCSPRITEID[525] = 66; // 시모네',
  NPCSPRITEID[516] = 62; // 마리',
  NPCSPRITEID[518] = 63; // 베아트릭스',
  NPCSPRITEID[522] = 64; // 주디스',
  NPCSPRITEID[523] = 65; // 헤롤드',
  NPCSPRITEID[524] = 61; // 브렌다',
  NPCSPRITEID[520] = 67; // 아미',
  NPCSPRITEID[517] = 68; // 몰록',

  // 69 - 72 는 각각 불사조, 소, 용, 흑표범
  //
  NPCSPRITEID[551] = 70;
  NPCSPRITEID[553] = 72;
  NPCSPRITEID[555] = 69;
  NPCSPRITEID[557] = 71;

  NPCSPRITEID[634] = 78; // 커피숍 여주인
  NPCSPRITEID[635] = 75; // 건달
  NPCSPRITEID[636] = 76; // 생선가게 주인
  NPCSPRITEID[637] = 77; // 할머니
  NPCSPRITEID[638] = 73; // 아마타
  NPCSPRITEID[639] = 74; // 리파티
  NPCSPRITEID[653] = 79; // 라비니아
  NPCSPRITEID[654] = 74; // 리파티
  NPCSPRITEID[655] = 80; // 마네스코
  NPCSPRITEID[656] = 81; // 시오람
  NPCSPRITEID[657] = 73; // 아마타
  NPCSPRITEID[658] = 82; // 호라

  NPCSPRITEID[676] = 85; // 레프리
  NPCSPRITEID[677] = 86; // 엘루니아
  NPCSPRITEID[678] = 87; // 이그니엄
  NPCSPRITEID[679] = 88; // 로와
  NPCSPRITEID[680] = 89; // 나이샤스
  NPCSPRITEID[681] = 90; // 니트리샤

  NPCSPRITEID[806] = 90; // 디올
  NPCSPRITEID[818] = 88; // 엘루티아

  NPCSPRITEID[820] = 91; // 카를코흐

  NPCSPRITEID[912] = 92; // 헬가든 감시의 수정구
  NPCSPRITEID[913] = 93; // 헬가든 예지의 수정구
  NPCSPRITEID[914] = 94; // 헬가든 심안의 수정구
  NPCSPRITEID[915] = 95; // 헬가든 심안의 수정구
  NPCSPRITEID[916] = 96; // 헬가든 심안의 수정구

#if __CONTENTS(__MENEGROTH)
  NPCSPRITEID[946] = 97; // 메네그로스 입장 슬래
  NPCSPRITEID[947] = 98; // 메네그로스 입장 뱀파
  NPCSPRITEID[948] = 99; // 메네그로스 입장 아우

  NPCSPRITEID[949] = 100; // 메네그로스 소환 슬레 1 - 1
  NPCSPRITEID[950] = 100; // 메네그로스 소환 슬레 1 - 2
  NPCSPRITEID[951] = 100; // 메네그로스 소환 슬레 1 - 3
  NPCSPRITEID[952] = 100; // 메네그로스 소환 슬레 1 - 4

  NPCSPRITEID[953] = 100; // 메네그로스 소환 슬레 2 - 1
  NPCSPRITEID[954] = 100; // 메네그로스 소환 슬레 2 - 2
  NPCSPRITEID[955] = 100; // 메네그로스 소환 슬레 2 - 3
  NPCSPRITEID[956] = 100; // 메네그로스 소환 슬레 2 - 4

  NPCSPRITEID[957] = 100; // 메네그로스 소환 슬레 3 - 1
  NPCSPRITEID[958] = 100; // 메네그로스 소환 슬레 3 - 2
  NPCSPRITEID[959] = 100; // 메네그로스 소환 슬레 3 - 3
  NPCSPRITEID[960] = 100; // 메네그로스 소환 슬레 3 - 4

  NPCSPRITEID[961] = 100; // 메네그로스 소환 슬레 4 - 1
  NPCSPRITEID[962] = 100; // 메네그로스 소환 슬레 4 - 2
  NPCSPRITEID[963] = 100; // 메네그로스 소환 슬레 4 - 3
  NPCSPRITEID[964] = 100; // 메네그로스 소환 슬레 4 - 4

  NPCSPRITEID[965] = 100; // 메네그로스 소환 슬레 5 - 1
  NPCSPRITEID[966] = 100; // 메네그로스 소환 슬레 5 - 2
  NPCSPRITEID[967] = 100; // 메네그로스 소환 슬레 5 - 3
  NPCSPRITEID[968] = 100; // 메네그로스 소환 슬레 5 - 4

  NPCSPRITEID[969] = 100; // 메네그로스 소환 슬레 6 - 1
  NPCSPRITEID[970] = 100; // 메네그로스 소환 슬레 6 - 2
  NPCSPRITEID[971] = 100; // 메네그로스 소환 슬레 6 - 3
  NPCSPRITEID[972] = 100; // 메네그로스 소환 슬레 6 - 4

  NPCSPRITEID[973] = 100; // 메네그로스 소환 뱀파 1 - 1
  NPCSPRITEID[974] = 100; // 메네그로스 소환 뱀파 1 - 2
  NPCSPRITEID[975] = 100; // 메네그로스 소환 뱀파 1 - 3
  NPCSPRITEID[976] = 100; // 메네그로스 소환 뱀파 1 - 4

  NPCSPRITEID[977] = 100; // 메네그로스 소환 뱀파 2 - 1
  NPCSPRITEID[978] = 100; // 메네그로스 소환 뱀파 2 - 2
  NPCSPRITEID[979] = 100; // 메네그로스 소환 뱀파 2 - 3
  NPCSPRITEID[980] = 100; // 메네그로스 소환 뱀파 2 - 4

  NPCSPRITEID[981] = 100; // 메네그로스 소환 뱀파 3 - 1
  NPCSPRITEID[982] = 100; // 메네그로스 소환 뱀파 3 - 2
  NPCSPRITEID[983] = 100; // 메네그로스 소환 뱀파 3 - 3
  NPCSPRITEID[984] = 100; // 메네그로스 소환 뱀파 3 - 4

  NPCSPRITEID[985] = 100; // 메네그로스 소환 뱀파 4 - 1
  NPCSPRITEID[986] = 100; // 메네그로스 소환 뱀파 4 - 2
  NPCSPRITEID[987] = 100; // 메네그로스 소환 뱀파 4 - 3
  NPCSPRITEID[988] = 100; // 메네그로스 소환 뱀파 4 - 4

  NPCSPRITEID[989] = 100; // 메네그로스 소환 뱀파 5 - 1
  NPCSPRITEID[990] = 100; // 메네그로스 소환 뱀파 5 - 2
  NPCSPRITEID[991] = 100; // 메네그로스 소환 뱀파 5 - 3
  NPCSPRITEID[992] = 100; // 메네그로스 소환 뱀파 5 - 4

  NPCSPRITEID[993] = 100; // 메네그로스 소환 뱀파 6 - 1
  NPCSPRITEID[994] = 100; // 메네그로스 소환 뱀파 6 - 2
  NPCSPRITEID[995] = 100; // 메네그로스 소환 뱀파 6 - 3
  NPCSPRITEID[996] = 100; // 메네그로스 소환 뱀파 6 - 4

  NPCSPRITEID[997] = 100;  // 메네그로스 소환 아우 1 - 1
  NPCSPRITEID[998] = 100;  // 메네그로스 소환 아우 1 - 2
  NPCSPRITEID[999] = 100;  // 메네그로스 소환 아우 1 - 3
  NPCSPRITEID[1000] = 100; // 메네그로스 소환 아우 1 - 4

  NPCSPRITEID[1001] = 100; // 메네그로스 소환 아우 2 - 1
  NPCSPRITEID[1002] = 100; // 메네그로스 소환 아우 2 - 2
  NPCSPRITEID[1003] = 100; // 메네그로스 소환 아우 2 - 3
  NPCSPRITEID[1004] = 100; // 메네그로스 소환 아우 2 - 4

  NPCSPRITEID[1005] = 100; // 메네그로스 소환 아우 3 - 1
  NPCSPRITEID[1006] = 100; // 메네그로스 소환 아우 3 - 2
  NPCSPRITEID[1007] = 100; // 메네그로스 소환 아우 3 - 3
  NPCSPRITEID[1008] = 100; // 메네그로스 소환 아우 3 - 4

  NPCSPRITEID[1009] = 100; // 메네그로스 소환 아우 4 - 1
  NPCSPRITEID[1010] = 100; // 메네그로스 소환 아우 4 - 2
  NPCSPRITEID[1011] = 100; // 메네그로스 소환 아우 4 - 3
  NPCSPRITEID[1012] = 100; // 메네그로스 소환 아우 4 - 4

  NPCSPRITEID[1013] = 100; // 메네그로스 소환 아우 5 - 1
  NPCSPRITEID[1014] = 100; // 메네그로스 소환 아우 5 - 2
  NPCSPRITEID[1015] = 100; // 메네그로스 소환 아우 5 - 3
  NPCSPRITEID[1016] = 100; // 메네그로스 소환 아우 5 - 4

  NPCSPRITEID[1017] = 100; // 메네그로스 소환 아우 6 - 1
  NPCSPRITEID[1018] = 100; // 메네그로스 소환 아우 6 - 2
  NPCSPRITEID[1019] = 100; // 메네그로스 소환 아우 6 - 3
  NPCSPRITEID[1020] = 100; // 메네그로스 소환 아우 6 - 4
#endif                     //__MENEGROTH

#if __CONTENTS(__QUEST_RENEWAL)
  NPCSPRITEID[1021] = 101;
  NPCSPRITEID[1022] = 102;
  NPCSPRITEID[1023] = 103;

  NPCSPRITEID[1029] = 101; // 위에 번호 말고 이것으로 패치가 되야 한다. 서버가
                           // 준비되면 위에 사용 되는것은 지워 주자.
  NPCSPRITEID[1030] = 102;
  NPCSPRITEID[1031] = 103;
#endif

#if __CONTENTS(__TIPOJYU_CASTLE) // NPC 추가
  NPCSPRITEID[1041] = 104;
#endif // __TIPOJYU_CASTLE
#if __CONTENTS(__OX_EVENT)
  NPCSPRITEID[1046] = 105;
  NPCSPRITEID[1047] = 105;
  NPCSPRITEID[1048] = 105;
#endif //__OX_EVENT
#if __CONTENTS(__BLITZ_COUPON_EVENT)
  NPCSPRITEID[1049] = 105;
  NPCSPRITEID[1050] = 105;
  NPCSPRITEID[1051] = 105;
#endif //__BLITZ_COUPON_EVENT
#if __CONTENTS(__2008_AUTUMN_LEAVES_EVENT)
  NPCSPRITEID[1052] = 105;
  NPCSPRITEID[1053] = 105;
  NPCSPRITEID[1054] = 105;
#endif //__2008_AUTUMN_LEAVES_EVENT

#if __CONTENTS(__GLOBAL_NPC)
  NPCSPRITEID[1056] = 106;
  NPCSPRITEID[1057] = 107;
  NPCSPRITEID[1058] = 108;
#endif //__GLOBAL_NPC
#if __CONTENTS(__CHRISTMAS_SOCKS_EVENT)
  NPCSPRITEID[1060] = 105;
  NPCSPRITEID[1061] = 105;
  NPCSPRITEID[1062] = 105;
#endif //__CHRISTMAS_SOCKS_EVENT
#if __CONTENTS(__ONIBLA)
  NPCSPRITEID[1079] = 38;
  NPCSPRITEID[1080] = 32;
  NPCSPRITEID[1081] = 79; // 라비니아
#endif                    //__ONIBLA
#if __CONTENTS(__MOONLIGHT_FESTIVAL_EVENT)
  NPCSPRITEID[1086] = 109;
  NPCSPRITEID[1087] = 109;
  NPCSPRITEID[1088] = 109;
#endif //__MOONLIGHT_FESTIVAL_EVENT
  NPCSPRITEID[1472] = 109;
}

MServerNPCTable::~MServerNPCTable() {
  //	Release();
}

int MServerNPCTable::AffectToNPCTable(MNPCTable *npc) {
  // 크기가 같아야 한다.
  int ServerSize = size();

  TYPE_MAP::iterator iData = begin();

  int count = 0;
  while (iData != end()) {
    count++;
    unsigned id = (*iData).first;
    SERVERNPC_INFO *info = (*iData).second;

    NPC_INFO *ni = new NPC_INFO;
    ni->Name = info->Name;
    ni->Description = info->Description;
    ni->ListShopTemplateID.clear();

    (*g_pCreatureTable)[id].Name = info->Name;

    std::list<unsigned int>::iterator iID = info->ListShopTemplateID.begin();
    while (iID != info->ListShopTemplateID.end()) {
      ni->ListShopTemplateID.push_back(*iID);
      iID++;
    }

    if (NPCSPRITEID.find(id) == NPCSPRITEID.end())
      ni->SpriteID = -1;
    else
      ni->SpriteID = NPCSPRITEID[id];

    npc->AddData(id, ni);

    iData++;
  }

  if (ServerSize != count) {
    return false;
  }
  return true;
}
