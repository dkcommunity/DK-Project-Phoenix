//---------------------------------------------------------------------------





// GameInitInfo.cpp





//---------------------------------------------------------------------------





// Information Table�� �� 초기��� ����.





//---------------------------------------------------------------------------











//-----------------------------------------------------------------------------





// Include files





//-----------------------------------------------------------------------------





#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "PhoenixFramePacer.h"	// 2026-09-02 O2 — OptionRenewal.set startup apply





// #include <MMSystem.h>





// #include "MFileDef.h"





#include "Client.h"





#include "Properties.h"











// #include "AddonDef.h"





#include "AcceleratorDef.h"





#include "AcceleratorManager.h"





#include "DebugInfo.h"





#include "ExperienceTable.h"





#include "FameInfo.h"





#include "GameObject.h"





#include "KeyAccelerator.h"





#include "MChatManager.h"





#include "MCompareManager.h"





#include "MEventManager.h"





#include "MGpsTable.h"





#include "MGuildInfoMapper.h"





#include "MGuildMarkManager.h"





#include "MHelpDisplayer.h"





#include "MHelpManager.h"





#include "MHelpStringTable.h"





#include "MJusticeAttackManager.h"





#include "MLevelNameTable.h"





#include "MMonsterKillQuestInfo.h"





#include "MNPCScriptTable.h"





#include "MTimeItemManager.h"





#include "MWarManager.h"





#include "ModifyStatusManager.h"





#include "MonsterNameTable.h"





#include "OperatorOption.h"





#include "ProfileManager.h"





#include "RankBonusTable.h"





#include "ServerInfo.h"





#include "ShrineInfoManager.h"





#include "UIDialog.h"





#include "VS_UI_filepath.h"











#ifdef OUTPUT_DEBUG





// #define	SAVE_TO_FILE_CONFIG





// #define SAVE_TO_FILE_INFO





#endif











// #define	new		DEBUG_NEW











#if __CONTENTS(__GPS_ADD) //	MGPS_INFO	g_pGpsInfoTable;











MGPS_INFO *g_pGpsInfoTable = NULL;











#endif // MGPS_INFO











//-----------------------------------------------------------------------------





// Init Config





//-----------------------------------------------------------------------------





BOOL InitClientConfig() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  ClientConfig");











  //---------------------------------------------------------------------





  // 메모� ���





  //---------------------------------------------------------------------





  if (g_pClientConfig == NULL) {





    g_pClientConfig = new ClientConfig;





  }











  //---------------------------------------------------------------------





  //





  //			Client Config ����





  //





  //---------------------------------------------------------------------





  ///*





  //---------------------------------------------------------------------





  // Load





  //---------------------------------------------------------------------





  if (!g_pClientConfig->LoadFromFile(





          g_pFileDef->getProperty("FILE_INFO_CLIENTCONFIG").c_str())) {





    DEBUG_ADD_FORMAT("[Error] File Open Error : [%s]",





                     g_pFileDef->getProperty("FILE_INFO_CLIENTCONFIG").c_str());





    // InitFail("[Error] File Open Error : [%s]", FILE_INFO_CLIENTCONFIG);











    // return FALSE;





  }











  if (g_pClientConfig->NEW_USER_REGISTERATION_MODE != ClientConfig::NUR_CLIENT) {
    CT_Done("InitClientConfig: forcing NEW_USER_REGISTERATION_MODE from %d to NUR_CLIENT",
            (int)g_pClientConfig->NEW_USER_REGISTERATION_MODE);
    g_pClientConfig->NEW_USER_REGISTERATION_MODE = ClientConfig::NUR_CLIENT;
  }

  // ���� �� � ��������.





  g_pClientConfig->SaveToFile(





      g_pFileDef->getProperty("FILE_INFO_CLIENTCONFIG").c_str());











  return TRUE;





}











//-----------------------------------------------------------------------------





// Init Config





//-----------------------------------------------------------------------------





BOOL InitUserOption() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  UserOption");











  //---------------------------------------------------------------------





  // 메모� ���





  //---------------------------------------------------------------------





  if (!g_pUserOption)





    g_pUserOption = new UserOption;











  if (!g_pOperatorOption)





    g_pOperatorOption = new OperatorOption;











  //---------------------------------------------------------------------





  //





  //			User Option ����





  //





  //---------------------------------------------------------------------





  ///*





  //*/





  //---------------------------------------------------------------------





  // Load





  //---------------------------------------------------------------------





  if (!g_pUserOption->LoadFromFile(





          g_pFileDef->getProperty("FILE_INFO_USEROPTION").c_str())) {





    DEBUG_ADD_FORMAT("[Error] File Open Error : [%s]",





                     g_pFileDef->getProperty("FILE_INFO_USEROPTION").c_str());





    // InitFail("[Error] File Open Error : [%s]", FILE_INFO_USEROPTION);











    // return FALSE;





  }











  // 2026-09-02 O2 — Reborn Option runtime ayarlari (OptionRenewal.set):
  // VSync, brightness, FPS limit, smooth pacing, linear filter, texture cache.
  // Dosya yoksa default'lar mevcut davranisi korur.
  Phoenix_ApplyRenewalRuntimeOptions();

  g_pUserOption->persnalShopupdatetime = GetPersnalShopUpdateTime();





  g_pUserOption->PlayWaveMusic = TRUE;











  g_pUserOption->Resolution1024 = g_pUserInformation->IsResolution1024;











  return TRUE;





}











//-----------------------------------------------------------------------------





// Init Infomation





//-----------------------------------------------------------------------------





BOOL InitInfomation() {
  CT_Done("InitInfo: ENTRY (Start)");





  // Debug Message





  DEBUG_ADD("[ InitGame ]  Information");











  //---------------------------------------------------------------------





  // 메모� ���





  //---------------------------------------------------------------------





  SAFE_DELETE(g_pRegenTowerInfoManager);





  SAFE_DELETE(g_pQuestInfoManager);





  SAFE_DELETE(g_pTimeItemManager);





  SAFE_DELETE(g_pFameInfoTable);





  SAFE_DELETE(g_pWarManager);





  SAFE_DELETE(g_pKeyAccelerator);





  SAFE_DELETE(g_pAcceleratorManager);





  SAFE_DELETE(g_pModifyStatusManager);





  SAFE_DELETE(g_pEffectGeneratorTable);





  SAFE_DELETE(g_pUIDialog);





  SAFE_DELETE(g_pProfileManager);





  SAFE_DELETE(g_pGuildMarkManager);





  SAFE_DELETE(g_pGuildInfoMapper);





  SAFE_DELETE(g_pEventManager);





  SAFE_DELETE(g_pEffectSpriteTypeTable);





  SAFE_DELETE(g_pActionEffectSpriteTypeTable);





  SAFE_DELETE(g_pCreatureTable);





  SAFE_DELETE(g_pCreatureSpriteTypeMapper);





  SAFE_DELETE(g_pItemTable);





  SAFE_DELETE(g_pCreatureSpriteTable);





  //	SAFE_DELETE( g_pCreatureActionSpriteTable );





  //	SAFE_DELETE( g_pAddonSpriteTable );





  SAFE_DELETE(g_pEffectStatusTable);





  SAFE_DELETE(g_pInteractionObjectTable);





  SAFE_DELETE(g_pItemOptionTable);





  SAFE_DELETE(g_pNPCTable);





  SAFE_DELETE(g_pJusticeAttackManager);





  SAFE_DELETE(g_pNPCScriptTable);





  SAFE_DELETE(g_pShopTemplateTable);





  SAFE_DELETE(g_pSkillInfoTable);





#if __CONTENTS(__GPS_ADD) // MGPS_INFO





  SAFE_DELETE(g_pGpsInfoTable);





#endif // MGPS_INFO





  SAFE_DELETE(g_pRankBonusTable);





  SAFE_DELETE(g_pExperienceTable);





  SAFE_DELETE(g_pSoundTable);





  SAFE_DELETE(g_pMusicTable);





  SAFE_DELETE(g_pZoneTable);





  SAFE_DELETE(g_pSoundManager);





  SAFE_DELETE(g_pUsingColorSetTable);





  SAFE_DELETE(g_pActionInfoTable);





  SAFE_DELETE(g_pChatManager);





  SAFE_DELETE(g_pHelpManager);





  SAFE_DELETE(g_pHelpStringTable);





  SAFE_DELETE(g_pCompareManager);





  SAFE_DELETE(g_pHelpDisplayer);





  SAFE_DELETE(g_pLevelNameTable);





  SAFE_DELETE(g_pMonsterNameTable);











  if (g_pKeyAccelerator == NULL) {





    g_pKeyAccelerator = new KeyAccelerator;





    g_pKeyAccelerator->Init(MAX_ACCELERATOR);





    SetDefaultAccelerator(); // ����..





    // g_pKeyAccelerator->LoadFromFile( ) �� ���� ��..





  }











  if (g_pWarManager == NULL) {





    g_pWarManager = new MWarManager;





  }











  if (g_pTimeItemManager == NULL) {





    g_pTimeItemManager = new MTimeItemManager;





  }











  if (g_pAcceleratorManager == NULL) {





    g_pAcceleratorManager = new AcceleratorManager;





  }











  if (g_pModifyStatusManager == NULL) {





    g_pModifyStatusManager = new ModifyStatusManager;





  }











  if (g_pEffectGeneratorTable == NULL) {





    g_pEffectGeneratorTable = new MEffectGeneratorTable;





  }











  if (g_pUIDialog == NULL) {





    g_pUIDialog = new UIDialog;





  }











  if (g_pProfileManager == NULL) {





    g_pProfileManager = new ProfileManager;





  }











  if (g_pGuildInfoMapper == NULL) {





    g_pGuildInfoMapper = new MGuildInfoMapper;











    // �� ���� ���� ���� ����.











    std::string filenameString =





        g_pFileDef->getProperty("FILE_INFO_GUILD_INFO_MAPPER");





    const char *filename = filenameString.c_str();











    ivfstream guildInfoFile(filename, std::ios::binary);





    std::ofstream guildInfoWritefile("Data\\Info\\guildmapper.inf", std::ios::binary);





    if (guildInfoFile.is_open()) {





      g_pGuildInfoMapper->LoadFromFile(guildInfoFile);





      guildInfoFile.close();





    }





  }











  CT_Done("InitInfo: MGuildMarkManager PRE (NULL=%d)", (g_pGuildMarkManager == NULL) ? 1 : 0);
  if (g_pGuildMarkManager == NULL) {





    g_pGuildMarkManager = new MGuildMarkManager(





        g_pFileDef->getProperty("FILE_SPRITE_GUILD_MARK").c_str(),





        g_pFileDef->getProperty("FILE_INFO_GUILD_INFO_MAPPER").c_str(),





        g_pFileDef->getProperty("FILE_SPRITE_GRADE_MARK_SLAYER").c_str(),





        g_pFileDef->getProperty("FILE_SPRITE_GRADE_MARK_VAMPIRE").c_str(),





        g_pFileDef->getProperty("FILE_SPRITE_GRADE_MARK_OUSTERS").c_str(),





        g_pFileDef->getProperty("FILE_SPRITE_LEVEL_MARK_PET").c_str(),





        g_pFileDef->getProperty("FILE_SPRITE_MONSTER_LEVEL_MARK_PET").c_str(),





        g_pFileDef->getProperty("FILE_SPRITE_ORIGIN_MARK").c_str());





  }
  CT_Done("InitInfo: MGuildMarkManager POST (ptr=%p)", (void*)g_pGuildMarkManager);











  CT_Done("InitInfo: MEventManager PRE (NULL=%d)", (g_pEventManager == NULL) ? 1 : 0);
  if (g_pEventManager == NULL) {





    g_pEventManager = new MEventManager;





  }
  CT_Done("InitInfo: MEventManager POST (ptr=%p)", (void*)g_pEventManager);











  if (g_pEffectSpriteTypeTable == NULL) {





    g_pEffectSpriteTypeTable = new EFFECTSPRITETYPE_TABLE;





  }
  CT_Done("InitInfo: EffectSpriteTypeTable#1 POST");











  if (g_pActionEffectSpriteTypeTable == NULL) {





    g_pActionEffectSpriteTypeTable = new MActionEffectSpriteTypeTable;





  }
  CT_Done("InitInfo: ActionEffectSpriteTypeTable POST");











  if (g_pCreatureTable == NULL) {





    g_pCreatureTable = new CREATURE_TABLE;





  }
  CT_Done("InitInfo: CreatureTable#1-globalInit POST");











  if (g_pCreatureSpriteTypeMapper == NULL) {





    g_pCreatureSpriteTypeMapper = new CreatureSpriteTypeMapper;





  }
  CT_Done("InitInfo: CreatureSpriteTypeMapper POST");











  if (g_pItemTable == NULL) {





    g_pItemTable = new ITEMCLASS_TABLE;





  }
  CT_Done("InitInfo: ItemTable POST");











  if (g_pEffectSpriteTypeTable == NULL) {





    g_pEffectSpriteTypeTable = new EFFECTSPRITETYPE_TABLE;





  }











  if (g_pCreatureSpriteTable == NULL) {





    g_pCreatureSpriteTable = new CREATURESPRITE_TABLE;





  }
  CT_Done("InitInfo: CreatureSpriteTable POST");











  //	if (g_pCreatureActionSpriteTable==NULL)





  //	{





  //		g_pCreatureActionSpriteTable = new CREATURESPRITE_TABLE;





  //	}





  //





  //	if (g_pAddonSpriteTable==NULL)





  //	{





  //		g_pAddonSpriteTable = new CREATURESPRITE_TABLE;





  //	}











  if (g_pEffectStatusTable == NULL) {





    g_pEffectStatusTable = new EFFECTSTATUS_TABLE;





  }
  CT_Done("InitInfo: EffectStatusTable#pre POST");











  if (g_pInteractionObjectTable == NULL) {





    g_pInteractionObjectTable = new INTERACTIONOBJECT_TABLE;





  }
  CT_Done("InitInfo: InteractionObjectTable POST");











  if (g_pItemOptionTable == NULL) {
    g_pItemOptionTable = new ITEMOPTION_TABLE;

    // FAZ 4 (2026-05-07): ItemOption table load. VS_UI_Item.cpp constructor'da
    // #ifndef _LIB ile sarılı LoadFromFile, vs_ui_lib (_LIB tanımlı) build
    // target'ında skip ediliyordu — table boş kalıyordu, tüm wearable item
    // option'ları "+0" görünüyordu. InitInfo'ya proper load eklendi.
    ivfstream itemOptionTableFile2;
    if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_ITEMOPTION").c_str(), itemOptionTableFile2)) {
      CrashTrace("InitInfo FAIL: ITEMOPTION load failed!");
    } else {
      g_pItemOptionTable->LoadFromFile(itemOptionTableFile2);
      itemOptionTableFile2.close();
    }
  }
  CrashTrace("InitInfo: ItemOptionTable POST size=%d", g_pItemOptionTable ? g_pItemOptionTable->GetSize() : -1);











  if (g_pNPCTable == NULL) {





    g_pNPCTable = new MNPCTable;





  }
  CT_Done("InitInfo: NPCTable POST");











  if (g_pJusticeAttackManager == NULL) {





    g_pJusticeAttackManager = new MJusticeAttackManager;





  }
  CT_Done("InitInfo: JusticeAttackManager POST");











  if (g_pNPCScriptTable == NULL) {





    g_pNPCScriptTable = new MNPCScriptTable;





  }
  CT_Done("InitInfo: NPCScriptTable POST");











  if (g_pShopTemplateTable == NULL) {





    g_pShopTemplateTable = new MShopTemplateTable;





  }
  CT_Done("InitInfo: ShopTemplateTable POST");











  if (g_pSkillInfoTable == NULL) {





    g_pSkillInfoTable = new MSkillInfoTable;





  }
  CT_Done("InitInfo: SkillInfoTable#pre POST");











#if __CONTENTS(__GPS_ADD) // MGPS_INFO





  if (g_pGpsInfoTable == NULL) {





    g_pGpsInfoTable = new MGPS_INFO;





  }
  CT_Done("InitInfo: GpsInfoTable POST");





#endif // MGPS_INFO











  if (g_pRankBonusTable == NULL) {





    g_pRankBonusTable = new RankBonusTable;





  }
  CT_Done("InitInfo: RankBonusTable POST");











  if (g_pExperienceTable == NULL) {





    g_pExperienceTable = new ExperienceTable;





  }
  CT_Done("InitInfo: ExperienceTable POST");











  if (g_pSoundTable == NULL) {





    g_pSoundTable = new SOUND_TABLE;





  }
  CT_Done("InitInfo: SoundTable#pre POST");











  if (g_pMusicTable == NULL) {





    g_pMusicTable = new MUSIC_TABLE;





  }
  CT_Done("InitInfo: MusicTable#pre POST");











  if (g_pZoneTable == NULL) {





    g_pZoneTable = new CZoneTable;





  }
  CT_Done("InitInfo: ZoneTable POST");











  if (g_pSoundManager == NULL) {





    g_pSoundManager = new CSoundPartManager;





  }
  CT_Done("InitInfo: SoundManager POST");











  if (g_pUsingColorSetTable == NULL) {





    g_pUsingColorSetTable = new USINGCOLORSET_TABLE;





  }
  CT_Done("InitInfo: UsingColorSetTable#pre POST");











  if (g_pActionInfoTable == NULL) {





    g_pActionInfoTable = new MActionInfoTable;





  }
  CT_Done("InitInfo: ActionInfoTable#pre POST");











  if (g_pChatManager == NULL) {





    g_pChatManager = new MChatManager;





  }
  CT_Done("InitInfo: ChatManager#pre POST");











  if (g_pHelpManager == NULL) {





    g_pHelpManager = new MHelpManager;





  }
  CT_Done("InitInfo: HelpManager POST");











  if (g_pHelpStringTable == NULL) {





    g_pHelpStringTable = new MHelpStringTable;





  }
  CT_Done("InitInfo: HelpStringTable POST");











  if (g_pCompareManager == NULL) {





    g_pCompareManager = new MCompareManager;





  }
  CT_Done("InitInfo: CompareManager POST");











  if (g_pHelpDisplayer == NULL) {





    g_pHelpDisplayer = new MHelpDisplayer;





  }
  CT_Done("InitInfo: HelpDisplayer POST");











  if (g_pLevelNameTable == NULL) {





    g_pLevelNameTable = new MLevelNameTable;





  }
  CT_Done("InitInfo: LevelNameTable POST");











  if (g_pMonsterNameTable == NULL) {





    g_pMonsterNameTable = new MonsterNameTable;





  }
  CT_Done("InitInfo: MonsterNameTable POST");











  if (g_pFameInfoTable == NULL) {





    g_pFameInfoTable = new FameInfoTable;





  }
  CT_Done("InitInfo: FameInfoTable POST");











  if (g_pQuestInfoManager == NULL) {





    g_pQuestInfoManager = new MQuestInfoManager;





  }
  CT_Done("InitInfo: QuestInfoManager POST");











  if (g_pRegenTowerInfoManager == NULL) {





    g_pRegenTowerInfoManager = new RegenTowerInfoManager();





  CT_Done("InitInfo: LoadRegenTowerInfo PRE");
    if (g_pRegenTowerInfoManager->LoadRegenTowerInfo() == false) {





      // CT_Done("STABILITY BYPASS: MessageBox blocked.");
      // 2026-05-22 RESTORE: Phoenix-only silent disable kaldirildi.
      // Referans davranis (GameInitInfo.cpp:470-471): MessageBox modal + return FALSE.
      // Phoenix protected INF modeli: RegenTowerPosition.inf artik Data\Info
      // altindan okunur. infodata.rpk password/format zinciri runtime init
      // icin gerekli degil.
      CT_Done("InitInfo: LoadRegenTowerInfo FAILED, return FALSE (RESTORED 2026-05-22 — Phoenix-only bypass kaldirildi)");
      return FALSE;





      //  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





    }
  CT_Done("InitInfo: LoadRegenTowerInfo POST (passed if check)");
  CT_Done("InitInfo: RegenTowerInfoManager POST");





  }











  // int i;





  //---------------------------------------------------------------------





  //





  //			Global ��� ����





  //





  //---------------------------------------------------------------------





  CT_Done("InitInfo: PacketItemTable (PRE)");
  DEBUG_ADD("[ InitGame ]  Information - PacketItemTable");











  InitPacketItemTable();





  //---------------------------------------------------------------------





  //





  //          class�� ���� ��� ����..





  //





  //---------------------------------------------------------------------





  CT_Done("InitInfo: InitMoveTable (PRE)");
  DEBUG_ADD("[ InitGame ]  Information - InitMoveTable");











  MCreature::InitMoveTable();











  DEBUG_ADD("[ InitGame ]  Information - UIDialog-Init");











  g_pUIDialog->Init();











  //---------------------------------------------------------------------





  //





  //      EffectGenerator ����





  //





  //---------------------------------------------------------------------





  CT_Done("InitInfo: EffectGenerator (PRE)");
  DEBUG_ADD("[ InitGame ]  Information - EffectGenerator");











  (*g_pEffectGeneratorTable).Init();











  //---------------------------------------------------------------------





  //





  //		Chat Manager Loading





  //





  //---------------------------------------------------------------------





  //------------------------------------------------





  // Load





  //------------------------------------------------





  CT_Done("InitInfo: ChatManager (PRE)");
  DEBUG_ADD("[ InitGame ]  Information - ChatManager");





  ivfstream chatInfoFile2; //(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





                           //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_CHAT").c_str(),





                      chatInfoFile2)) {





    CT_Done("STABILITY BYPASS: MessageBox blocked.");


     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }


  }





  chatInfoFile2.close();





  g_pChatManager->LoadFromFile(





      g_pFileDef->getProperty("FILE_INFO_CHAT").c_str());











  //	g_pChatManager->LoadFromFileCurse("yok.txt") ;





  //	g_pChatManager->SaveToFile(g_pFileDef->getProperty("FILE_INFO_CHAT").c_str());





  //---------------------------------------------------------------------





  //





  //		Help Manager , Help String Table





  //





  //---------------------------------------------------------------------





  //---------------------------------------------------------------------





  // Help Manager





  //---------------------------------------------------------------------





  //	DEBUG_ADD("[ InitGame ]  Information - HelpManager");





  //	std::ifstream





  //helpManagerFile;//(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





  //std::ios::binary); 	if





  //(!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_SMART_HELP").c_str(),





  //helpManagerFile)) 		 { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } 	g_pHelpManager->LoadFromFile(





  //helpManagerFile ); 	helpManagerFile.close();











  //---------------------------------------------------------------------





  // Help String Table





  //---------------------------------------------------------------------





  CT_Done("InitInfo: HelpString (PRE)");
  DEBUG_ADD("[ InitGame ]  Information - HelpString");





  ivfstream helpStringTableFile; //(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





                                 //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_SMART_HELP_STRING").c_str(), helpStringTableFile)) { CT_Done("InitInfo FAIL: SMART_HELP_STRING load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  g_pHelpStringTable->LoadFromFile(helpStringTableFile);





  helpStringTableFile.close();











  //---------------------------------------------------------------------





  //





  //						LevelName Table





  //





  //---------------------------------------------------------------------





  CT_Done("InitInfo: LevelName");
  DEBUG_ADD("[ InitGame ]  Information - LevelName");











  ivfstream levelNameTableFile; //(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





                                //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_LEVEL_NAME").c_str(), levelNameTableFile)) { CT_Done("InitInfo FAIL: LEVEL_NAME load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  g_pLevelNameTable->LoadFromFile(levelNameTableFile);





  levelNameTableFile.close();











  //---------------------------------------------------------------------





  //





  //						MonsterName Table





  //





  //---------------------------------------------------------------------





  CT_Done("InitInfo: MonsterName (Attempting load from MONSTER_NAME)");
  DEBUG_ADD("[ InitGame ]  Information - MonsterName");











  ivfstream MonsterNameTableFile; //(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





                                  //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_MONSTER_NAME").c_str(), MonsterNameTableFile)) { CT_Done("InitInfo FAIL: MONSTER_NAME load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  g_pMonsterNameTable->LoadFromFile(MonsterNameTableFile);





  MonsterNameTableFile.close();











  //---------------------------------------------------------------------





  //





  //          UsingColor ��� ����





  //





  //---------------------------------------------------------------------





  /*





  #ifdef SAVE_TO_FILE_INFO





          (*g_pUsingColorSetTable).Init( MAX_USINGCOLORSET );











          (*g_pUsingColorSetTable)[USINGCOLORSET_NONE].Disable(0,





  MAX_USINGCOLORSET);





          (*g_pUsingColorSetTable)[USINGCOLORSET_ALL].Enable(0,





  MAX_USINGCOLORSET);











          for (i=0; i<MAX_COLORSET; i++)





          {





                  (*g_pUsingColorSetTable)[USINGCOLORSET_SKIN].Enable(i*MAX_COLORSET_SEED_MODIFY);





                  (*g_pUsingColorSetTable)[USINGCOLORSET_SKIN].Enable(i*MAX_COLORSET_SEED_MODIFY





  + 1);





                  (*g_pUsingColorSetTable)[USINGCOLORSET_SKIN].Enable(i*MAX_COLORSET_SEED_MODIFY





  + 2);





          }











          for (i=0; i<MAX_COLORSET; i++)





          {





                  (*g_pUsingColorSetTable)[USINGCOLORSET_ITEM].Enable(i*MAX_COLORSET_SEED_MODIFY);





                  (*g_pUsingColorSetTable)[USINGCOLORSET_ITEM].Enable(i*MAX_COLORSET_SEED_MODIFY





  + 7);





                  (*g_pUsingColorSetTable)[USINGCOLORSET_ITEM].Enable((i+1)*MAX_COLORSET_SEED_MODIFY





  - 1);





          }











          //------------------------------------------------





          // Save





          //------------------------------------------------





          std::ofstream





  usingColorSetTable(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





  std::ios::binary);





          (*g_pUsingColorSetTable).SaveToFile(usingColorSetTable);





          usingColorSetTable.close();





  #endif





  */











  //------------------------------------------------





  // Load





  //------------------------------------------------





  CT_Done("InitInfo: UsingColorSet (Attempting load from USINGCOLORSET)");
  DEBUG_ADD("[ InitGame ]  Information - UsingColorSet");











  ivfstream usingColorSetTable2; //(g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





                                 //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_USINGCOLORSET").c_str(),





          usingColorSetTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  (*g_pUsingColorSetTable).LoadFromFile(usingColorSetTable2);
  CT_Done("InitInfo: UsingColorSet LOAD SUCCESS");





  usingColorSetTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  CT_Done("InitInfo: SoundTable (Attempting load from SOUND)");
  DEBUG_ADD("[ InitGame ]  Information - SoundTable");











  ivfstream soundTable2; //(g_pFileDef->getProperty("FILE_INFO_SOUND").c_str(),





                         //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_SOUND").c_str(), soundTable2)) { CT_Done("InitInfo FAIL: SOUND load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  (*g_pSoundTable).LoadFromFile(soundTable2);





  soundTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  CT_Done("InitInfo: MusicTable (Attempting load from MUSIC)");
  DEBUG_ADD("[ InitGame ]  Information - MusicTable");











  ivfstream musicTable2; //(g_pFileDef->getProperty("FILE_INFO_MUSIC").c_str(),





                         //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_MUSIC").c_str(), musicTable2)) { CT_Done("InitInfo FAIL: MUSIC load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  (*g_pMusicTable).LoadFromFile(musicTable2);





  musicTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - ZoneTable");











  ivfstream zoneTable2; //(g_pFileDef->getProperty("FILE_INFO_ZONE").c_str(),





                        //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_ZONE").c_str(), zoneTable2)) { CT_Done("InitInfo FAIL: ZONE load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  (*g_pZoneTable).LoadFromFile(zoneTable2);





  zoneTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD_FORMAT("[ InitGame ]  Information - ItemTable(%s)",





                   g_pFileDef->getProperty("FILE_INFO_ITEM").c_str());





  ivfstream itemTable2; //(g_pFileDef->getProperty("FILE_INFO_ITEM").c_str(),





                        //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_ITEM").c_str(), itemTable2)) { CT_Done("InitInfo FAIL: ITEM load failed!");  { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; } }





  (*g_pItemTable).LoadFromFile(itemTable2);


  itemTable2.close();


  DEBUG_ADD("[ InitGame ]  Information - ItemOptionTable");


  // 2026-05-18: Dangling pointer fix (ayni MSkillManager.cpp pattern).
  // getProperty() std::string by-value returns -> .c_str() temporary pointer
  // SEMICOLON sonrasi dangling. std::string ile scope korunur.
  std::string optPath = g_pFileDef->getProperty("FILE_INFO_ITEMOPTION");


  // MessageBox removed


  ivfstream itemOptionTable2(optPath.c_str(), std::ios::binary);


  (*g_pItemOptionTable).LoadFromFile(itemOptionTable2);





  itemOptionTable2.close();











  // 0�� 추ꕜ��.





  /*





  ITEMOPTION_TABLE ItemOptionTable;





  ItemOptionTable.Init( (*g_pItemOptionTable).GetSize()+1 );











  ItemOptionTable[0].Part = 0;





  ItemOptionTable[0].Name = "NONE";





  ItemOptionTable[0].PlusPoint = 0;





  ItemOptionTable[0].PlusRequireAbility = 0;





  ItemOptionTable[0].Worth = 0;





  ItemOptionTable[0].ColorSet = 0;











  for (int o=0; o<(*g_pItemOptionTable).GetSize(); o++)





  {





          ItemOptionTable[o+1] = (*g_pItemOptionTable)[o];





  }











  std::ofstream





  itemOptionTable(g_pFileDef->getProperty("FILE_INFO_ITEMOPTION").c_str(),





  std::ios::binary); ItemOptionTable.SaveToFile( itemOptionTable );





  itemOptionTable.close();





  */











  //---------------------------------------------------------------------





  //





  //    InteractionObjectTable�� �� ��� ����





  //





  //---------------------------------------------------------------------





  /*





#ifdef SAVE_TO_FILE_INFO





  (*g_pInteractionObjectTable).Init( 2 );











  (*g_pInteractionObjectTable)[0].Type	= MInteractionObject::TYPE_DOOR;





  (*g_pInteractionObjectTable)[0].FrameID = 0;





  (*g_pInteractionObjectTable)[0].SoundID = 10;











  (*g_pInteractionObjectTable)[1].Type	= MInteractionObject::TYPE_TRAP;





  (*g_pInteractionObjectTable)[1].FrameID = 1;





  (*g_pInteractionObjectTable)[1].SoundID = 11;











  //------------------------------------------------





  // Save





  //------------------------------------------------





  std::ofstream





interactionObjectTable(g_pFileDef->getProperty("FILE_INFO_INTERACTIONOBJECT"),





std::ios::binary);





  (*g_pInteractionObjectTable).SaveToFile(interactionObjectTable);





  interactionObjectTable.close();





#endif





  */











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - InteractionObjectTable");











  ivfstream interactionObjectTable2; //(g_pFileDef->getProperty("FILE_INFO_INTERACTIONOBJECT"),





                                     //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_INTERACTIONOBJECT").c_str(),





          interactionObjectTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  (*g_pInteractionObjectTable).LoadFromFile(interactionObjectTable2);





  interactionObjectTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  ivfstream creatureSpriteTable2; //(g_pFileDef->getProperty("FILE_INFO_CREATURE"),





                                  //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_CREATURESPRITE").c_str(),





          creatureSpriteTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pCreatureSpriteTable->LoadFromFile(creatureSpriteTable2);





  creatureSpriteTable2.close();











  //------------------------------------------------





  // Addon SpriteTable





  //------------------------------------------------





  //	std::ifstream





  //addonSpriteTable2;//(g_pFileDef->getProperty("FILE_INFO_CREATURE"),





  //std::ios::binary); 	if





  //(!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_ADDONSPRITE").c_str(),





  //addonSpriteTable2)) 		 { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  //	g_pAddonSpriteTable->LoadFromFile(addonSpriteTable2);





  //	addonSpriteTable2.close();











  //------------------------------------------------





  // CreatureAction SpriteTable





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - CreatureActionSpriteTable");











  //	std::ifstream





  //creatureActionSpriteTable2;//(g_pFileDef->getProperty("FILE_INFO_CREATURE"),





  //std::ios::binary); 	if





  //(!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_CREATUREACTIONSPRITE"),





  //creatureActionSpriteTable2)) 		 { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  //	g_pCreatureActionSpriteTable->LoadFromFile(creatureActionSpriteTable2);





  //	creatureActionSpriteTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - CreatureTable");











  ivfstream creatureTable2; //(g_pFileDef->getProperty("FILE_INFO_CREATURE"),





                            //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_CREATURE").c_str(),





                      creatureTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  CT_Done("InitInfo: Loading CreatureTable...");
  CT_Done("DEBUG: Checking creatureTable2 status...");
    if (creatureTable2.is_open()) {
        creatureTable2.seekg(0, std::ios_base::end);
        CT_Done("DEBUG: creatureTable2 is OPEN. Physical Size: %ld", creatureTable2.tellg());
        unsigned char hexBuf[16];
        creatureTable2.seekg(0, std::ios_base::beg);
        creatureTable2.read((char*)hexBuf, 16);
        CT_Done("DEBUG: HEX Header: %02X %02X %02X %02X | %02X %02X %02X %02X | %02X %02X %02X %02X | %02X %02X %02X %02X", 
            hexBuf[0], hexBuf[1], hexBuf[2], hexBuf[3], hexBuf[4], hexBuf[5], hexBuf[6], hexBuf[7], 
            hexBuf[8], hexBuf[9], hexBuf[10], hexBuf[11], hexBuf[12], hexBuf[13], hexBuf[14], hexBuf[15]);
        creatureTable2.seekg(0, std::ios_base::beg);
        creatureTable2.seekg(0, std::ios_base::beg);
    } else {
        CT_Done("DEBUG: creatureTable2 is NOT OPEN!");
    }
    if (!creatureTable2.is_open()) CT_Done("InitInfo: ERROR! creatureTable2 is NOT open!");
    // Ensure creatureTable is NOT zero size if open
    if (creatureTable2.is_open() && (*g_pCreatureTable).GetSize() == 0) {
        CT_Done("InitInfo: CreatureTable is empty but file is open. Initializing with capacity safety.");
        (*g_pCreatureTable).Init(1114);
    }
  else CT_Done("InitInfo: creatureTable2 is open OK.");
  (*g_pCreatureTable).LoadFromFile(creatureTable2);
  CT_Done("InitInfo: CreatureTable Size = %d", (*g_pCreatureTable).GetSize());
  creatureTable2.close();





  // #ifdef _DEBUG





  //	std::ofstream CreatureTable("CreatureTable.csv");





  //	int CreatureMaxSize = (*g_pCreatureTable).GetSize();





  //	for(int CreaturID = 0; CreaturID < CreatureMaxSize; ++CreaturID)





  //	{





  //		(*g_pCreatureTable)[CreaturID].SaveToCSVFile(CreatureTable,





  //CreaturID);





  //	}





  //	CreatureTable.close();





  // #endif











  //---------------------------------------------------------------------





  //





  //		CreatureSpriteTypeMapper





  //





  //---------------------------------------------------------------------





  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - CreatureSpriteTypeMapper");











  ivfstream CreatureSpriteTypeMapper2; //(g_pFileDef->getProperty("FILE_INFO_CREATURE"),





                                       //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_CREATURESPRITE_MAPPER").c_str(),





          CreatureSpriteTypeMapper2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pCreatureSpriteTypeMapper->LoadFromFile(CreatureSpriteTypeMapper2);





  CreatureSpriteTypeMapper2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - ShopTemplate");











  ivfstream ShopTemplateTable2; //(g_pFileDef->getProperty("FILE_INFO_SHOPTEMPLATE"),





                                //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_SHOPTEMPLATE").c_str(),





                      ShopTemplateTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  (*g_pShopTemplateTable).LoadFromFile(ShopTemplateTable2);





  ShopTemplateTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - NPCTable");











  ivfstream





      NPCTable2; //(g_pFileDef->getProperty("FILE_INFO_CREATURE"), std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_NPC").c_str(),





                      NPCTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  (*g_pNPCTable).LoadFromFile(NPCTable2);





  NPCTable2.close();











  //---------------------------------------------------------------------





  //





  //    NPC Script�� �� ��� ����





  //





  //---------------------------------------------------------------------











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - NPCScript");











  ivfstream NPCScriptTable; //(g_pFileDef->getProperty("FILE_INFO_CREATURE"),





                            //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_NPC_SCRIPT_INFO").c_str(),





          NPCScriptTable))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pNPCScriptTable->LoadFromFile(NPCScriptTable);











#ifdef _DEBUG





  std::ofstream NPCScriptTableCSV("NPCScript.csv");





  g_pNPCScriptTable->SaveToCSVFile(NPCScriptTableCSV);





  NPCScriptTableCSV.close();





#endif











  NPCScriptTable.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - EffectSpriteTypeTable");











  ivfstream effectSpriteTypeTable2; //(g_pFileDef->getProperty("FILE_INFO_EFFECTSPRITETYPE"),





                                    //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_EFFECTSPRITETYPE").c_str(),





          effectSpriteTypeTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  (*g_pEffectSpriteTypeTable).LoadFromFile(effectSpriteTypeTable2);





  effectSpriteTypeTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  ivfstream actionEffectSpriteTypeTable2; //(g_pFileDef->getProperty("FILE_INFO_EFFECTSPRITETYPE"),





                                          //std::ios::binary);





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_ACTIONEFFECTSPRITETYPE").c_str(),





          actionEffectSpriteTypeTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pActionEffectSpriteTypeTable->LoadFromFile(actionEffectSpriteTypeTable2);





  actionEffectSpriteTypeTable2.close();











  //------------------------------------------------





  // Load





  //------------------------------------------------





  CT_Done("InitInfo: EffectStatusTable");
  DEBUG_ADD("[ InitGame ]  Information - EffectStatusTable");











  ivfstream EffectStatusTable2; //(g_pFileDef->getProperty("FILE_INFO_EffectStatus"),





                                //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_EFFECTSTATUS").c_str(),





                      EffectStatusTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  (*g_pEffectStatusTable).LoadFromFile(EffectStatusTable2);





  EffectStatusTable2.close();











  //---------------------------------------------------------------------





  //





  //    SkillInfoTable�� �� ��� ����





  //





  //---------------------------------------------------------------------





  //------------------------------------------------





  // Load





  //------------------------------------------------





  CT_Done("InitInfo: SkillInfoTable");
  DEBUG_ADD("[ InitGame ]  Information - SkillInfoTable");











  ivfstream





      skillTable2; //(g_pFileDef->getProperty("FILE_INFO_skill"), std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_SKILL").c_str(),





                      skillTable2))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pSkillInfoTable->LoadFromFile(skillTable2);





  skillTable2.close();











  //------------------------------------------------





  // Server ��보� loading����.





  //------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - ServerSkillInfo");











  ivfstream serverSkillInfoFile; //(g_pFileDef->getProperty("FILE_INFO_skill"),





                                 //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_SKILL_INFO").c_str(),





                      serverSkillInfoFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  g_pSkillInfoTable->LoadFromFileServerSkillInfo(serverSkillInfoFile);





  serverSkillInfoFile.close();





#if __CONTENTS(__GPS_ADD)





  //---------------------------------------------------------------------





  //





  //    GPS�� �� ���





  //





  //---------------------------------------------------------------------





  ivfstream





      gpsInfoFile; //(g_pFileDef->getProperty("FILE_INFO_skill"), std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_GPS").c_str(),





                      gpsInfoFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pGpsInfoTable->LoadFromFile(gpsInfoFile);





  gpsInfoFile.close();





#endif //__GPS_ADD





       //---------------------------------------------------------------------





       //





       //    RankBonusTable�� �� ��� ����





       //





       //---------------------------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Information - rankBonusInfoFile");











  ivfstream rankBonusInfoFile; //(g_pFileDef->getProperty("FILE_INFO_skill"),





                               //std::ios::binary);





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_RANK_BONUS").c_str(),





                      rankBonusInfoFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  g_pRankBonusTable->LoadFromFile(rankBonusInfoFile);





  rankBonusInfoFile.close();











  ivfstream fameInfoFile;











  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_FAME_LIMIT").c_str(),





                      fameInfoFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  g_pFameInfoTable->LoadFromFile(fameInfoFile);





  fameInfoFile.close();











  //---------------------------------------------------------------------





  //





  //    ExperienceTable�� �� ��� ����





  //





  //---------------------------------------------------------------------





  //------------------------------------------------





  // Load





  //------------------------------------------------





  ivfstream strExpFile;





  ivfstream dexExpFile;





  ivfstream intExpFile;





  ivfstream vampireExpFile;





  ivfstream oustersExpFile;





  ivfstream slayerRankExpFile;





  ivfstream vampireRankExpFile;





  ivfstream oustersRankExpFile;





  ivfstream advancementExp;





  ivfstream petExpFile;











  //------------------------------------------------





  // STR





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_STR_EXP").c_str(),





                      strExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - STR");





  g_pExperienceTable->LoadFromFileSTR(strExpFile);











  //------------------------------------------------





  // DEX





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_DEX_EXP").c_str(),





                      dexExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - DEX");





  g_pExperienceTable->LoadFromFileDEX(dexExpFile);











  //------------------------------------------------





  // INT





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_INT_EXP").c_str(),





                      intExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - INT");





  g_pExperienceTable->LoadFromFileINT(intExpFile);











  //------------------------------------------------





  // Vampire





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_VAMPIRE_EXP").c_str(),





                      vampireExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - Vampire");





  g_pExperienceTable->LoadFromFileVampire(vampireExpFile);











  //------------------------------------------------





  // Ousters





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_OUSTERS_EXP").c_str(),





                      oustersExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - Ousters");





  g_pExperienceTable->LoadFromFileOusters(oustersExpFile);











  //------------------------------------------------





  // Slayer Rank





  //------------------------------------------------





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_SLAYER_RANK_EXP").c_str(),





          slayerRankExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - Slayer Rank");





  g_pExperienceTable->LoadFromFileSlayerRank(slayerRankExpFile);











  //------------------------------------------------





  // Vampire Rank





  //------------------------------------------------





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_VAMPIRE_RANK_EXP").c_str(),





          vampireRankExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - Vampire Rank");





  g_pExperienceTable->LoadFromFileVampireRank(vampireRankExpFile);











  //------------------------------------------------





  // Ousters Rank





  //------------------------------------------------





  if (!FileOpenBinary(





          g_pFileDef->getProperty("FILE_INFO_OUSTERS_RANK_EXP").c_str(),





          oustersRankExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - Ousters Rank");





  g_pExperienceTable->LoadFromFileOustersRank(oustersRankExpFile);











  //------------------------------------------------





  // PetExp





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_PET_EXP_INFO").c_str(),





                      petExpFile))





     { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }











  DEBUG_ADD("[ InitGame ]  Information - Ousters Rank");





  g_pExperienceTable->LoadFromFilePetExp(petExpFile);











  //------------------------------------------------





  // Advancement





  //------------------------------------------------





  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_ADVANCE_EXP_INFO").c_str(),





                      advancementExp))





     {
       // 2026-05-18 KALICI FIX (Katman B — silent disable restore):
       // Onceki agent return FALSE -> return TRUE yapmis "STABILITY BYPASS"
       // yorumu ile. Kural 4.2 (Disable yasak) + 4.3 (Yuzeysel fix yasak) ikisini cigniyor.
       // Dosya acilamadiginda init silent basari -> LoadFromFileAdvanceMent CALL
       // EDILMIYOR -> m_advanceSkillExp bos -> GetAdvanceMent(0) NULL deref AV.
       // Restore: orijinal davranis return FALSE.
       //
       // 2026-05-22 UPDATE: Bu dosyadaki TUM 33 STABILITY BYPASS site'i toplu
       // restore edildi (referans davranisa parity, return FALSE). Eski yorumdaki
       // "27 site" sayisi yanlisti — gercek sayi 33 idi. Detay:
       // memory/project_initinformation_stability_bypass_audit_2026_05_22.md
       CT_Done("InitInfomation: FILE_ADVANCE_EXP_INFO open FAILED, return FALSE (disable kaldirildi)");
       return FALSE;
     }











  DEBUG_ADD("[ InitGame ]  Information - Ousters Rank");





  g_pExperienceTable->LoadFromFileAdvanceMent(advancementExp);











  petExpFile.close();





  strExpFile.close();





  dexExpFile.close();





  intExpFile.close();





  vampireExpFile.close();





  oustersExpFile.close();





  slayerRankExpFile.close();





  vampireRankExpFile.close();





  oustersRankExpFile.close();





  advancementExp.close();











  //-----------------------------------





  // Load From File





  //-----------------------------------





  CT_Done("InitInfo: ActionInfoTable");
  DEBUG_ADD("[ InitGame ]  Information - ActionInfoTable");











  // Phoenix protected INF path: ActionInfo diger INF dosyalari gibi
  // Data\Info\Action.inf uzerinden yuklenir. ivfstream once .phx dosyasini
  // dener, yoksa duz INF'e duser.
  ivfstream actionInfoFile2;
  if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_ACTION").c_str(), actionInfoFile2))
  { CT_Done("InitInfo FAIL: ACTION load failed!"); return FALSE; }





  // if (!FileOpenBinary(g_pFileDef->getProperty("FILE_INFO_ACTION").c_str(),





  // actionInfoFile2)) 	 { CT_Done("InitInfomation: file-open FAILED, return FALSE (RESTORED 2026-05-22 — referans davranis: data dosyasi yoksa init basarisiz)"); return FALSE; }





  g_pActionInfoTable->LoadFromFile(actionInfoFile2);





  actionInfoFile2.close();











  CT_Done("InitInfo: MonsterKillQuest");
  DEBUG_ADD("[ InitGame ]  Information - MonsterKillQuest");





  ivfstream MonsterKillQuestInfoFile;





  if (!FileOpenBinary(


          g_pFileDef->getProperty("FILE_INFO_MONSTER_KILL_QUEST").c_str(),


          MonsterKillQuestInfoFile)) {


    // Missing file in 670 client, ignore gracefully instead of Abort


  } else {


    DWORD QuestVer = 0;





    MonsterKillQuestInfoFile.read((char*)&QuestVer, sizeof(DWORD));

    g_pQuestInfoManager->LoadFromFile(MonsterKillQuestInfoFile);
    MonsterKillQuestInfoFile.close();
  }

  CT_Done("InitInfo: LoadWorldMapInfo PRE");
  DEBUG_ADD("[ InitGame ]  Information - LoadWorldMapInfo");

  if (!LoadWorldMapInfo())
    return FALSE;

  CT_Done("InitInfo: All Clear");
  DEBUG_ADD("[ InitGame ]  Information - All Clear");

  return TRUE;
}
