#include "DB.h"
#include "Directive.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterManager.h"
#include "PlayerCreature.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"


#include "Properties.h"

#include "DEScriptZone.h"
#include "EffectScript.h"
#include "EventTransport.h"
#include "InstanceDoungeonManager.h"

#include "../EffectKillTimer.h"
#include "../skill/EffectStoneSkin.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToScreen.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCRemoveEffect.h"


#include "Gpackets/GCGlobalChat.h"
#include "Gpackets/GCSay.h"


#include "EventSystemMessage.h"
#include "StringPool.h"

#include "Gpackets/GCAddNPC.h"
#include "NPC.h"
#include "NPCInfo.h"
#include "PCFinder.h"
#include "PacketUtil.h"


TScriptZoneCount DEScriptZone::m_ScriptZoneCountMap;

DEScriptZone::DEScriptZone(lua_State *L, Zone *pZone) : DEScript() {
  m_pLuaState = L;
  m_pZone = pZone;
  m_bStaticZone = true;
  m_EmptyTimeOut = 65535;
  m_bIgnoreEmptyTimeOut = false;

  m_EntranceType = ENTRANCE_TYPE_INDIVISUAL;

  lua_tinker::class_add<DEScriptZone>(m_pLuaState, "DEScriptZone");

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "SendSystemMessage",
                                      &DEScriptZone::SendSystemMessage);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "BroadcastSystemMessage",
                                      &DEScriptZone::BroadcastSystemMessage);
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "BroadcastSystemMessageInZone",
      &DEScriptZone::BroadcastSystemMessageInZone);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "TransportCreature",
                                      &DEScriptZone::TransportCreature);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "TraverseCreature",
                                      &DEScriptZone::TraverseCreature);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "TraverseMonster",
                                      &DEScriptZone::TraverseMonster);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "BroadcastGCSay",
                                      &DEScriptZone::BroadcastGCSay);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "BroadcastGCGlobalChat",
                                      &DEScriptZone::BroadcastGCGlobalChat);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "GetZoneID",
                                      &DEScriptZone::GetZoneID);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "GetScriptZoneCount",
                                      &DEScriptZone::GetScriptZoneCount);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "SetIgnoreEmptyTimeOut",
                                      &DEScriptZone::SetIgnoreEmptyTimeOut);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "GetIgnoreEmptyTimeOut",
                                      &DEScriptZone::GetIgnoreEmptyTimeOut);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "HaveItem",
                                      &DEScriptZone::HaveItem);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "TakeItem",
                                      &DEScriptZone::TakeItem);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "GiveItem",
                                      &DEScriptZone::GiveItem);
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveGoodsItem",
      &DEScriptZone::GiveGoodsItem); // add by rallser
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveGoodsItemForLogOut",
      &DEScriptZone::GiveGoodsItemForLogOut); // add by rallser
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveWarPoint",
      &DEScriptZone::GiveWarPoint); // add by rallser

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "SetTimer",
                                      &DEScriptZone::SetTimer);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "KillTimer",
                                      &DEScriptZone::KillTimer);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "AddMonster",
                                      &DEScriptZone::AddMonster);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "AddEffectToTile",
                                      &DEScriptZone::AddEffectToTile);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "AddEffectToCreature",
                                      &DEScriptZone::AddEffectToCreature);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "AddEffectToScreen",
                                      &DEScriptZone::AddEffectToScreen);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "RemoveEffect",
                                      &DEScriptZone::RemoveEffect);
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "AddTimeEffect",
      &DEScriptZone::AddTimeEffect); // add by rallser
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "AddSpecificEffect",
      &DEScriptZone::AddSpecificEffect); // add by rallser
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "setFlag",
                                      &DEScriptZone::setFlag);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "removeFlag",
                                      &DEScriptZone::removeFlag);
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "RemoveEffectFromCreature",
      &DEScriptZone::RemoveEffectFromCreature); // add by rallser
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveExpAdv", &DEScriptZone::GiveExpAdv); // add by rallser
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveRankExp", &DEScriptZone::GiveRankExp); // add by rallser

  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveDianJuan",
      &DEScriptZone::GiveDianJuan); // add by rallser
  lua_tinker::class_def<DEScriptZone>(
      m_pLuaState, "GiveJiFen", &DEScriptZone::GiveJiFen); // add by rallser

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "SetBlocked",
                                      &DEScriptZone::SetBlocked);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "ClearBlocked",
                                      &DEScriptZone::ClearBlocked);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "IsBlocked",
                                      &DEScriptZone::IsBlocked);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "FindPosition",
                                      &DEScriptZone::FindPosition);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "CreateInstanceDoungeon",
                                      &DEScriptZone::CreateInstanceDoungeon);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "KnockbackCreature",
                                      &DEScriptZone::KnockbackCreature);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "MoveFastMonster",
                                      &DEScriptZone::MoveFastMonster);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "GetCreature",
                                      &DEScriptZone::GetCreature);
  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "GetMonster",
                                      &DEScriptZone::GetMonster);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "SetPKZone",
                                      &DEScriptZone::SetPKZone);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "addNPC",
                                      &DEScriptZone::addNPC);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "recallparty",
                                      &DEScriptZone::recallparty);

  lua_tinker::class_def<DEScriptZone>(m_pLuaState, "MonsterUseSkill",
                                      &DEScriptZone::MonsterUseSkill);

  lua_tinker::set(m_pLuaState, "DEScriptZone", this);
}

DEScriptZone::~DEScriptZone() {
  m_TimerList.clear();

  if (m_ScriptZoneCountMap.find(m_Name) != m_ScriptZoneCountMap.end()) {
    --m_ScriptZoneCountMap[m_Name];
    if (m_ScriptZoneCountMap[m_Name] < 0) {
      m_ScriptZoneCountMap[m_Name] = 0;
    }
  }
}

void DEScriptZone::Init(Zone *pZone) { m_pZone = pZone; }

void DEScriptZone::UpdateTime() {
  // KillTimer()占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占 TimerID占쏙옙 占쏙옙占쏙옙
  {
    vector<int>::iterator iter;
    TDEScriptTimer::iterator iterTimer;

    int iTimerID;

    iter = m_KillTimerPlanList.begin();
    for (; iter != m_KillTimerPlanList.end(); ++iter) {
      iTimerID = (*iter);

      cout << "DEScriptZone(TemplateZoneID=" << GetTemplateZoneID()
           << ", ZoneID=" << GetZoneID()
           << "), find KillTimer, TimerID=" << iTimerID << endl;

      iterTimer = m_TimerList.find(iTimerID);

      if (iterTimer != m_TimerList.end()) {
        cout << "DEScriptZone(TemplateZoneID=" << GetTemplateZoneID()
             << ", ZoneID=" << GetZoneID()
             << "), erase KillTimer, TimerID=" << iTimerID << endl;

        m_TimerList.erase(iterTimer);
      }
    }
  }
  m_KillTimerPlanList.clear();

  {
    Timeval CurrentTime;
    Timeval PeriodTime;

    getCurrentTime(CurrentTime);

    TDEScriptTimer::iterator iter;
    iter = m_TimerList.begin();
    for (; iter != m_TimerList.end(); ++iter) {
      DEScriptTimer *pTimer = &(iter->second);

      if (pTimer->m_NextTime < CurrentTime) {
        /* 10 sec */
        PeriodTime.tv_sec = pTimer->m_lPeriodTime / 1000;
        PeriodTime.tv_usec = pTimer->m_lPeriodTime - (PeriodTime.tv_sec * 1000);

        pTimer->m_NextTime = CurrentTime + PeriodTime;

        NotifyTimer(iter->first);
      }
    }
  }
}

lua_tinker::table DEScriptZone::AddMonster(lua_tinker::table MonsterInfo) {
  MonsterType_t MType;
  string strAI;
  string strDeadAI;
  string strEnhance;
  string strName;
  ZoneCoord_t PosX;
  ZoneCoord_t PosY;
  Dir_t Dir;

  MType = MonsterInfo.get<MonsterType_t>("MType");
  strName = MonsterInfo.get<const char *>("Name");
  PosX = MonsterInfo.get<ZoneCoord_t>("X");
  PosY = MonsterInfo.get<ZoneCoord_t>("Y");
  Dir = MonsterInfo.get<Dir_t>("Dir");
  strAI = MonsterInfo.get<const char *>("AI");
  strDeadAI = MonsterInfo.get<const char *>("DeadAI");
  strEnhance = MonsterInfo.get<const char *>("Enhance");

  /*
  lua_tinker::table RegistSkillSet =
  MonsterInfo.get<lua_tinker::table>("RegistSkillSet");

  if ( !lua_isnil(m_pLuaState, -1) )
  {
          // 占쏙옙占쏙옙 占쏙옙킬 占싻깍옙
          lua_pushnil(m_pLuaState);
          while( lua_next(m_pLuaState, -2) != 0 )
          {
                  if ( lua_type(m_pLuaState, -2) != LUA_TNUMBER )
                  {
                          continue;
                  }

                  int iIndex = (int)lua_tonumber(m_pLuaState, -2);

                  switch(lua_type(m_pLuaState, -1))
                  {
                          case LUA_TSTRING :
                                  printf("%s Regist Skill - %d %s\n",
  strName.c_str(), iIndex, lua_tostring(m_pLuaState, -1)); break; default :
                                  break;
                  }

                  lua_pop(m_pLuaState, 1);
          }
  }
  */

  Monster *pMonster;

  if (!strEnhance.empty()) {
    pMonster = new Monster(MType, strEnhance);
  } else {
    pMonster = new Monster(MType);
  }

  if (!strAI.empty()) {
    MonsterAI *pMonsterAI = new MonsterAI(pMonster, strAI, strDeadAI);

    pMonster->setBrain(pMonsterAI);
  }

  if (!strName.empty()) {
    pMonster->setName(strName.c_str());
  }

  lua_tinker::table CreatureInfo(m_pLuaState);

  try {
    m_pZone->addCreature(pMonster, PosX, PosY, Dir);

    this->MakeCreatureInfo(CreatureInfo, pMonster);
  } catch (EmptyTileNotExistException &e) {
    cout << e.toString();

    this->MakeCreatureInfo(CreatureInfo, NULL);
  } catch (Error &e) {
    cout << e.toString();

    this->MakeCreatureInfo(CreatureInfo, NULL);
  }

  return CreatureInfo;
}

void DEScriptZone::SetTimer(int iTimerID, long lPeriodTime, TimerType Type) {
  DEScriptTimer Timer;

  Timer.m_iTimerID = iTimerID;
  Timer.m_lPeriodTime = lPeriodTime;
  getCurrentTime(Timer.m_NextTime);

  switch (Type) {
  case TIMER_TYPE_ONESHOT:
    break;
  case TIMER_TYPE_PERIODIC:
  default:
    Timer.m_NextTime.tv_sec += lPeriodTime / 1000;
    Timer.m_NextTime.tv_usec += lPeriodTime - (Timer.m_NextTime.tv_sec * 1000);
    break;
  }

  m_TimerList[iTimerID] = Timer;
}

void DEScriptZone::KillTimer(int iTimerID) {
  m_KillTimerPlanList.push_back(iTimerID);
}

bool DEScriptZone::HaveItem(lua_tinker::table CreatureInfo,
                            lua_tinker::table HaveItemInfo) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");
  if (!pCreature->isPC()) {
    return false;
  }

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
  Inventory *pInventory = pPC->getInventory();
  if (pInventory == NULL) {
    return false;
  }

  int iItemClass;
  int iItemType;
  int iItemCount;

  iItemClass = HaveItemInfo.get<int>("ItemClass");
  iItemType = HaveItemInfo.get<int>("ItemType");
  iItemCount = HaveItemInfo.get<int>("ItemCount");

  if (pInventory->hasEnoughNumItem((Item::ItemClass)iItemClass, iItemType,
                                   iItemCount)) {
    return true;
  }

  return false;
}

bool DEScriptZone::TakeItem(lua_tinker::table CreatureInfo,
                            lua_tinker::table TakeItemInfo) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");
  if (!pCreature->isPC()) {
    return false;
  }

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
  Inventory *pInventory = pPC->getInventory();
  if (pInventory == NULL) {
    return false;
  }

  int iItemClass;
  int iItemType;
  int iItemCount;

  iItemClass = TakeItemInfo.get<int>("ItemClass");
  iItemType = TakeItemInfo.get<int>("ItemType");
  iItemCount = TakeItemInfo.get<int>("ItemCount");

  pInventory->decreaseNumItem((Item::ItemClass)iItemClass, iItemType,
                              iItemCount, pCreature->getPlayer());

  return true;
}

bool DEScriptZone::GiveItem(lua_tinker::table CreatureInfo,
                            lua_tinker::table GiveItemInfo) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");
  if (!pCreature->isPC()) {
    return false;
  }

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

  int iItemClass;
  int iItemType;
  int iItemCount;
  string strOptions;
  DWORD LimitedTime;

  iItemClass = GiveItemInfo.get<int>("ItemClass");
  iItemType = GiveItemInfo.get<int>("ItemType");
  iItemCount = GiveItemInfo.get<int>("ItemCount");
  strOptions = GiveItemInfo.get<const char *>("Options");
  LimitedTime = GiveItemInfo.get<DWORD>(
      "LimitedTime"); // if LimitedTime is 0, time is unlimited.

  list<OptionType_t> Options;
  if (strOptions.length() > 0) {
    makeOptionList(strOptions, Options);
  }

  Item *pItem;

  pItem = GiveItemToInventory("DEScriptZone", pPC, (Item::ItemClass)iItemClass,
                              iItemType, iItemCount, Options, LimitedTime);
  if (pItem != NULL) {
    return true;
  }

  return false;
}

void DEScriptZone::BroadcastSystemMessage(SystemMessageType iMesgType,
                                          const char *strMesg) {
  if (strMesg == NULL) {
    return;
  }

  GCSystemMessage gcSystemMessage;

  gcSystemMessage.setType(iMesgType);
  gcSystemMessage.setMessage(strMesg);

  g_pZoneGroupManager->pushBroadcastPacket(&gcSystemMessage);
}

void DEScriptZone::BroadcastSystemMessageInZone(SystemMessageType iMesgType,
                                                const char *strMesg) {
  if (strMesg == NULL) {
    return;
  }

  GCSystemMessage gcSystemMessage;

  gcSystemMessage.setType(iMesgType);
  gcSystemMessage.setMessage(strMesg);

  m_pZone->broadcastPacket(&gcSystemMessage);
}

void DEScriptZone::SendSystemMessage(lua_tinker::table CreatureInfo,
                                     SystemMessageType iMesgType,
                                     const char *strMesg) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");

  if (pCreature == NULL) {
    return;
  }

  if (pCreature != NULL && !pCreature->isPC()) {
    return;
  }

  if (strMesg == NULL) {
    return;
  }

  GCSystemMessage gcSystemMessage;

  gcSystemMessage.setType(iMesgType);
  gcSystemMessage.setMessage(strMesg);

  Player *pPlayer;

  pPlayer = pCreature->getPlayer();
  if (pPlayer != NULL) {
    pPlayer->sendPacket(&gcSystemMessage);
  }
}

void DEScriptZone::TransportCreature(lua_tinker::table CreatureInfo,
                                     ZoneID_t TargetZoneID, ZoneCoord_t TargetX,
                                     ZoneCoord_t TargetY) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");
  if (pCreature != NULL && !pCreature->isPC()) {
    return;
  }

  GamePlayer *pGamePlayer = dynamic_cast<GamePlayer *>(pCreature->getPlayer());
  EventTransport *pEventTransport = createEventTransport(pGamePlayer);

  ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(TargetZoneID);

  if (TargetZoneID != 0 && pZoneInfo != NULL) {
    pEventTransport->setDeadline(0);
    pEventTransport->setZoneName(pZoneInfo->getFullName());
    pEventTransport->setTargetZone(TargetZoneID, TargetX, TargetY);

    pGamePlayer->addEvent(pEventTransport);
  }
}

void DEScriptZone::TraverseCreature() {
  try {
    PCManager *pPM = (PCManager *)m_pZone->getPCManager();

    map<ObjectID_t, Creature *> &pcs = pPM->getCreatures();
    map<ObjectID_t, Creature *>::iterator itr = pcs.begin();

    lua_tinker::table CreatureInfo(m_pLuaState);

    for (; itr != pcs.end(); itr++) {
      Creature *pCreature = itr->second;

      this->MakeCreatureInfo(CreatureInfo, pCreature);

      lua_tinker::call<int>(m_pLuaState, "OnTraverseCreature", CreatureInfo);
    }
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

void DEScriptZone::TraverseMonster() {
  try {
    MonsterManager *pMM = (MonsterManager *)m_pZone->getMonsterManager();

    map<ObjectID_t, Creature *> &monsters = pMM->getCreatures();
    map<ObjectID_t, Creature *>::iterator itr = monsters.begin();

    lua_tinker::table MonsterInfo(m_pLuaState);

    for (; itr != monsters.end(); itr++) {
      Creature *pCreature = itr->second;
      Monster *pMonster = dynamic_cast<Monster *>(pCreature);

      this->MakeMonsterInfo(MonsterInfo, pMonster);

      lua_tinker::call<int>(m_pLuaState, "OnTraverseMonster", MonsterInfo);
    }
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

lua_tinker::table
DEScriptZone::AddEffectToTile(lua_tinker::table EffectCreateInfo) {
  /*
  lua_tinker::table CastCreatureInfo =
  EffectCreatureInfo.get<lua_tinker::table>("EffectCastCreature"); Creature
  *pCastCreature = CastCreatureInfo.get<Creature *>("_pCreature");
  */
  Creature *pCastCreature = NULL;

  Effect::EffectClass effectClass =
      EffectCreateInfo.get<Effect::EffectClass>("EffectClass");

  ZoneCoord_t PosX = EffectCreateInfo.get<ZoneCoord_t>("PosX");
  ZoneCoord_t PosY = EffectCreateInfo.get<ZoneCoord_t>("PosY");

  Turn_t NextTime = EffectCreateInfo.get<Turn_t>("NextTime");
  Turn_t Deadline = EffectCreateInfo.get<Turn_t>("Deadline"); // unit - 0.1 sec

  EffectScript *pEffect =
      new EffectScript(effectClass, pCastCreature, m_pZone, PosX, PosY);
  pEffect->setNextTime(NextTime);
  pEffect->setDeadline(Deadline);

  ObjectRegistry &objectregister = m_pZone->getObjectRegistry();
  objectregister.registerObject(pEffect);

  m_pZone->addEffect(pEffect);

  /*
  Tile& tile = m_pZone->getTile(PosX, PosY);
  if ( tile.canAddEffect() )
  {
          tile.addEffect(pEffect);
  }
  */

  lua_tinker::table EffectInfo(m_pLuaState);
  MakeEffectInfo(EffectInfo, pEffect);

  return EffectInfo;
}

lua_tinker::table
DEScriptZone::AddEffectToCreature(lua_tinker::table EffectCreateInfo,
                                  lua_tinker::table CreatureInfo) {
  Creature *pCastCreature = NULL;
  Creature *pTargetCreature = NULL;

  Effect::EffectClass effectClass =
      EffectCreateInfo.get<Effect::EffectClass>("EffectClass");
  pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");

  lua_tinker::table EffectInfo(m_pLuaState);

  if (pTargetCreature == NULL) {
    MakeEffectInfo(EffectInfo, NULL);

    return EffectInfo;
  }

  Turn_t NextTime = EffectCreateInfo.get<Turn_t>("NextTime");
  Turn_t Deadline = EffectCreateInfo.get<Turn_t>("Deadline"); // unit - 0.1 sec

  EffectScript *pEffect =
      new EffectScript(effectClass, pCastCreature, pTargetCreature);
  pEffect->setNextTime(NextTime);
  pEffect->setDeadline(Deadline);

  ObjectRegistry &objectregister = m_pZone->getObjectRegistry();
  objectregister.registerObject(pEffect);

  pTargetCreature->addEffect(pEffect);
  pTargetCreature->setFlag(effectClass);

  GCAddEffect gcAddEffect;
  gcAddEffect.setObjectID(pTargetCreature->getObjectID());
  gcAddEffect.setEffectID(effectClass);
  gcAddEffect.setDuration(Deadline);

  m_pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(),
                           &gcAddEffect);

  MakeEffectInfo(EffectInfo, pEffect);

  return EffectInfo;
}
void DEScriptZone::RemoveEffectFromCreature(lua_tinker::table EffectCreateInfo,
                                            lua_tinker::table CreatureInfo) {

  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");

  Effect::EffectClass effectClass =
      EffectCreateInfo.get<Effect::EffectClass>("EffectClass");

  pTargetCreature->deleteEffect(effectClass);
  pTargetCreature->removeFlag(effectClass);

  GCRemoveEffect gcRemoveEffect;
  gcRemoveEffect.setObjectID(pTargetCreature->getObjectID());
  gcRemoveEffect.addEffectList(effectClass);
  pTargetCreature->getPlayer()->sendPacket(&gcRemoveEffect);

} // remove by rallser

void DEScriptZone::AddTimeEffect(lua_tinker::table EffectCreateInfo,
                                 lua_tinker::table CreatureInfo) {

  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");

  Effect::EffectClass effectClass =
      EffectCreateInfo.get<Effect::EffectClass>("EffectClass");
  if (pTargetCreature == NULL) {
    return;
  }
  Duration_t Duration = EffectCreateInfo.get<Turn_t>("Duration");

  // pTargetCreature->addEffect(effectClass);
  // pTargetCreature->setFlag(effectClass);

  GCAddEffect gcAddEffect;
  gcAddEffect.setObjectID(pTargetCreature->getObjectID());
  gcAddEffect.setEffectID(effectClass);
  gcAddEffect.setDuration(Duration);

  pTargetCreature->getPlayer()->sendPacket(&gcAddEffect);
}
void DEScriptZone::AddSpecificEffect(lua_tinker::table CreatureInfo) {

  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");

  EffectStoneSkin *pEffect = new EffectStoneSkin(pTargetCreature);
  // pEffect->setEffectClass( Effect::EFFECT_CLASS_STONE_SKIN );
  pEffect->setDeadline(150);
  pTargetCreature->setFlag(Effect::EFFECT_CLASS_STONE_SKIN);
  pTargetCreature->addEffect(pEffect);

  GCAddEffect gcAddEffect;
  gcAddEffect.setObjectID(pTargetCreature->getObjectID());
  gcAddEffect.setEffectID(pEffect->getEffectClass());
  gcAddEffect.setDuration(150);
  pTargetCreature->getPlayer()->sendPacket(&gcAddEffect);
}

void DEScriptZone::setFlag(lua_tinker::table CreatureInfo) {

  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");
  pTargetCreature->setFlag(Effect::EFFECT_CLASS_BOSS_ANGER);
}

void DEScriptZone::removeFlag(lua_tinker::table CreatureInfo) {

  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");
  pTargetCreature->removeFlag(Effect::EFFECT_CLASS_BOSS_ANGER);
}

void DEScriptZone::GiveExpAdv(lua_tinker::table CreatureInfo,
                              unsigned int amount) {

  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
  pPC->increaseAdvancementClassExp(amount, false);
  GCModifyInformation gcMI;
  gcMI.addShortData(MODIFY_ADVANCEMENT_CLASS_LEVEL,
                    pPC->getAdvancementClassLevel());
  pPC->getPlayer()->sendPacket(&gcMI);
}

void DEScriptZone::GiveRankExp(lua_tinker::table CreatureInfo,
                               unsigned int amount) {

  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
  pPC->increaseRankExp(amount);
  // GCModifyInformation gcMI;
  // gcMI.addShortData(MODIFY_ADVANCEMENT_CLASS_LEVEL,
  // pPC->getAdvancementClassLevel()); pPC->getPlayer()->sendPacket( &gcMI );
}

void DEScriptZone::GiveWarPoint(lua_tinker::table CreatureInfo,
                                unsigned int Amount) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");

  char *race;
  if (pCreature->isSlayer()) {
    race = "SLAYER";
  } else if (pCreature->isVampire()) {
    race = "VAMPIRE";
  } else if (pCreature->isOusters()) {
    race = "OUSTERS";
  }
  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

  Statement *pStmt = NULL;

  BEGIN_DB {
    Result *pResult = NULL;
    pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
    pResult = pStmt->executeQuery("SELECT Point FROM WarPoint WHERE Name='%s'",
                                  pPC->getName().c_str());

    if (pResult->next()) {
      int Point = pResult->getInt(1);
      pStmt->executeQuery("UPDATE WarPoint SET Point = %d WHERE Name = '%s'",
                          Point + Amount, pPC->getName().c_str());
    } else {
      pStmt->executeQuery("INSERT INTO WarPoint  VALUES ('%s','%s', '%d')",
                          pPC->getName().c_str(), race, Amount);
    }

    SAFE_DELETE(pStmt);
  }
  END_DB(pStmt)
}

bool DEScriptZone::GiveGoodsItemForLogOut(char *Type, char *Account, char *Name,
                                          unsigned int GiveGoodsItemNumber,
                                          unsigned int GiveGoodsItemCount) {

  Statement *pStmt = NULL;

  BEGIN_DB {
    pStmt =
        g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

    pStmt->executeQuery("INSERT IGNORE INTO GoodsListObject VALUES ('%s', '', "
                        "%d, '%s', '%s', %d, %d, 'NOT', now())",
                        Type, g_pConfig->getPropertyInt("WorldID"), Account,
                        Name, GiveGoodsItemNumber, GiveGoodsItemCount);
    SAFE_DELETE(pStmt);
  }

  END_DB(pStmt)

  return true;
}

bool DEScriptZone::GiveGoodsItem(lua_tinker::table CreatureInfo,
                                 unsigned int GiveGoodsItemNumber,
                                 unsigned int GiveGoodsItemCount) {
  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");
  if (!pCreature->isPC()) {
    return false;
  }

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

  Statement *pStmt = NULL;

  BEGIN_DB {
    pStmt =
        g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

    pStmt->executeQuery("INSERT IGNORE INTO GoodsListObject VALUES ('%s', '', "
                        "%d, '%s', '%s', %d, %d, 'NOT', now())",
                        "LUA", g_pConfig->getPropertyInt("WorldID"),
                        pPC->getPlayer()->getID().c_str(),
                        pPC->getName().c_str(), GiveGoodsItemNumber,
                        GiveGoodsItemCount);
    SAFE_DELETE(pStmt);
  }

  END_DB(pStmt)

  pPC->loadGoods();
  pPC->registerGoodsInventory(pPC->getZone()->getObjectRegistry());

  // GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
  // Assert(pGamePlayer!=NULL);

  // EventSystemMessage* pESM = new EventSystemMessage( pGamePlayer );
  // pESM->setDeadline( 0 );
  // pESM->addMessage(g_pStringPool->getString(STRID_GET_EVENT_PRESENT_IN_SHOPPINGCART).c_str());
  // pGamePlayer->addEvent( pESM );

  return true;
}

void DEScriptZone::AddEffectToScreen(lua_tinker::table EffectCreateInfo,
                                     lua_tinker::table CreatureInfo) {
  EffectID_t effectClass = EffectCreateInfo.get<EffectID_t>("EffectClass");
  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");

  if (pTargetCreature == NULL) {
    return;
  }

  Duration_t Duration = EffectCreateInfo.get<Turn_t>("Duration");

  GCAddEffectToScreen gcAddEffectToScreen;

  gcAddEffectToScreen.setEffectID(effectClass);
  gcAddEffectToScreen.setDuration(Duration);

  pTargetCreature->getPlayer()->sendPacket(&gcAddEffectToScreen);
}

void DEScriptZone::recallparty(lua_tinker::table CreatureInfo, ZoneCoord_t X,
                               ZoneCoord_t Y) {
  Creature *pTargetCreature = CreatureInfo.get<Creature *>("_pCreature");

  if (pTargetCreature == NULL) {
    return;
  }

  // PlayerCreature *pPC = dynamic_cast<PlayerCreature*>(pTargetCreature);

  GlobalPartyManager *pLPM = NULL;
  int PartyID = 0;
  PartyID = pTargetCreature->getPartyID();

  if (PartyID != 0) {
    Party *pParty = g_pGlobalPartyManager->getParty(PartyID);

    map<string, Creature *> MemberMap = pParty->getMemberMap();
    map<string, Creature *>::iterator MemberMapIter;

    Creature *pMemberCreature;
    int partysize = pParty->getSize();
    cout << "party size -->" << partysize << endl;
    MemberMapIter = MemberMap.begin();
    for (; MemberMapIter != MemberMap.end(); ++MemberMapIter) {
      pMemberCreature = MemberMapIter->second;
      if (pMemberCreature != NULL && pMemberCreature != pTargetCreature) {
        transportCreature(pMemberCreature, pTargetCreature->getZoneID(), X, Y,
                          false);
      }
    }
  }
}
void DEScriptZone::addNPC(lua_tinker::table NpcInfo) {
  NPCID_t NpcID;
  string NpcName;
  ZoneCoord_t PosX;
  ZoneCoord_t PosY;
  Dir_t Dir;

  NpcID = NpcInfo.get<NPCID_t>("NpcID");
  cout << "NpcID=" << NpcID;

  NpcName = NpcInfo.get<const char *>("Name");

  // SpriteType = NpcInfo.get<int>("SpriteType");

  // cout<<"SpriteType="<<SpriteType;

  // Race = NpcInfo.get<int>("Race");
  // cout<<"Race="<<Race;

  PosX = NpcInfo.get<ZoneCoord_t>("PosX");
  cout << "PosX=" << PosX;

  PosY = NpcInfo.get<ZoneCoord_t>("PosY");
  cout << "PosY=" << PosY;

  Dir = NpcInfo.get<Dir_t>("Dir");
  cout << "Dir=" << Dir;

  NPC *pNPC = new NPC();

  pNPC->setName(NpcName);

  pNPC->setNPCID(NpcID);

  pNPC->setSpriteType(303);

  pNPC->setRace(3);

  pNPC->setMainColor(1);

  pNPC->setSubColor(1);

  pNPC->setClanType(0);
  pNPC->setShowInMinimap(true);
  pNPC->setTaxingCastleZoneID(0);

  m_pZone->addCreature(pNPC, PosX, PosY, Dir);

  GCAddNPC gcAddNPC;
  makeGCAddNPC(&gcAddNPC, pNPC);
  m_pZone->broadcastPacket(PosX, PosY, &gcAddNPC);

  pNPC->init();
}

void DEScriptZone::RemoveEffect(lua_tinker::table EffectInfo) {
  Effect *pEffect;

  pEffect = EffectInfo.get<Effect *>("_pEffect");

  if (pEffect == NULL) {
    return;
  }

  /*
  Zone *pZone = pEffect->getZone();
  if ( pZone == NULL )
  {
          return;
  }

Tile& tile = pZone->getTile(pEffect->getX(), pEffect->getY());
if ( tile.hasEffect() )
{
  tile.deleteEffect(pEffect->getObjectID());
}
*/

  pEffect->setDeadline(0);
}

void DEScriptZone::SetBlocked(ZoneCoord_t PosX, ZoneCoord_t PosY) {
  if (isValidZoneCoord(m_pZone, PosX, PosY)) {
    m_pZone->getTile(PosX, PosY).setBlocked(Creature::MOVE_MODE_WALKING);
    m_pZone->getTile(PosX, PosY).setBlocked(Creature::MOVE_MODE_FLYING);
    m_pZone->getTile(PosX, PosY).setBlocked(Creature::MOVE_MODE_BURROWING);
  }
}

void DEScriptZone::ClearBlocked(ZoneCoord_t PosX, ZoneCoord_t PosY) {
  if (isValidZoneCoord(m_pZone, PosX, PosY)) {
    m_pZone->getTile(PosX, PosY).clearBlocked(Creature::MOVE_MODE_WALKING);
    m_pZone->getTile(PosX, PosY).clearBlocked(Creature::MOVE_MODE_FLYING);
    m_pZone->getTile(PosX, PosY).clearBlocked(Creature::MOVE_MODE_BURROWING);
  }
}

bool DEScriptZone::IsBlocked(ZoneCoord_t PosX, ZoneCoord_t PosY) {
  if (isValidZoneCoord(m_pZone, PosX, PosY)) {
    // 占싹놂옙占쏙옙 Block占실억옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쌍댐옙
    // 占쏙옙占쏙옙占쏙옙  占쏙옙占쏙옙占싼댐옙.

    return m_pZone->getTile(PosX, PosY)
               .isBlocked(Creature::MOVE_MODE_WALKING) ||
           m_pZone->getTile(PosX, PosY).isBlocked(Creature::MOVE_MODE_FLYING) ||
           m_pZone->getTile(PosX, PosY)
               .isBlocked(Creature::MOVE_MODE_BURROWING);
  }

  return false;
}

bool DEScriptZone::FindPosition(MonsterType_t monsterType,
                                lua_tinker::table PositionInfo) {
  MonsterManager *pMonsterManager = m_pZone->getMonsterManager();

  if (pMonsterManager != NULL) {
    ZoneCoord_t x, y;
    pMonsterManager->findPosition(monsterType, x, y);

    PositionInfo.set<ZoneCoord_t>("X", x);
    PositionInfo.set<ZoneCoord_t>("Y", y);

    return true;
  }

  return false;
}

ZoneID_t DEScriptZone::CreateInstanceDoungeon(const char *strScriptName) {
  string scriptName;

  scriptName = strScriptName;

  DynamicZoneScript *pDynamicZoneScript =
      InstanceDoungeonManager::CreateInstanceDoungeon(scriptName);

  if (pDynamicZoneScript != NULL) {
    return pDynamicZoneScript->getZoneID();
  }

  return 0;
}

void DEScriptZone::KnockbackCreature(lua_tinker::table TargetCreatureInfo,
                                     ZoneCoord_t iOriginX,
                                     ZoneCoord_t iOriginY) {
  Creature *pTargetCreature = TargetCreatureInfo.get<Creature *>("_pCreature");

  if (pTargetCreature == NULL) {
    return;
  }

  knockbackCreature(m_pZone, pTargetCreature, iOriginX, iOriginY);
}

bool DEScriptZone::MoveFastMonster(lua_tinker::table CreatureInfo,
                                   lua_tinker::table MovePosInfo) {
  Creature *pCreature = CreatureInfo.get<Creature *>("_pCreature");

  if (pCreature == NULL) {
    return false;
  }

  if (!pCreature->isMonster()) {
    return false;
  }

  ZoneCoord_t X1 = MovePosInfo.get<ZoneCoord_t>("X1");
  ZoneCoord_t Y1 = MovePosInfo.get<ZoneCoord_t>("Y1");

  ZoneCoord_t X2 = MovePosInfo.get<ZoneCoord_t>("X2");
  ZoneCoord_t Y2 = MovePosInfo.get<ZoneCoord_t>("Y2");

  SkillType_t skillType = MovePosInfo.get<SkillType_t>("SkillType");

  if (X1 == X2 && Y1 == Y2) {
    return false;
  }

  Monster *pMonster = dynamic_cast<Monster *>(pCreature);

  return m_pZone->moveFastMonster(pMonster, X1, Y1, X2, Y2, skillType);
}

void DEScriptZone::BroadcastGCSay(lua_tinker::table CreatureInfo,
                                  const char *strMsg, long lColor) {
  Creature *pCreature = CreatureInfo.get<Creature *>("_pCreature");

  if (pCreature == NULL) {
    return;
  }

  if (strMsg == NULL) {
    return;
  }

  Zone *pZone;

  pZone = pCreature->getZone();
  if (pZone == NULL) {
    return;
  }

  GCSay gcSay;
  gcSay.setObjectID(pCreature->getObjectID());
  gcSay.setMessage(strMsg);
  gcSay.setColor(lColor);

  pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSay);
}

void DEScriptZone::BroadcastGCGlobalChat(lua_tinker::table CreatureInfo,
                                         const char *strMsg, long lColor) {
  Creature *pCreature = CreatureInfo.get<Creature *>("_pCreature");

  if (pCreature == NULL) {
    return;
  }

  if (strMsg == NULL) {
    return;
  }

  Zone *pZone;

  pZone = pCreature->getZone();
  if (pZone == NULL) {
    return;
  }

  GCGlobalChat gcGC;
  gcGC.setColor(lColor);
  gcGC.setMessage((pCreature->getName() + " " + strMsg).c_str());
  gcGC.setRace(RACE_VAMPIRE);

  pZone->broadcastPacket(&gcGC);
}

ZoneID_t DEScriptZone::GetZoneID() {
  if (m_pZone != NULL) {
    return m_pZone->getZoneID();
  }

  return 0;
}

int DEScriptZone::GetScriptZoneCount() {
  if (m_ScriptZoneCountMap.find(m_Name) != m_ScriptZoneCountMap.end()) {
    return m_ScriptZoneCountMap[m_Name];
  }

  return 0;
}

void DEScriptZone::SetIgnoreEmptyTimeOut(bool bIgnoreEmptyTimeOut) {
  m_bIgnoreEmptyTimeOut = bIgnoreEmptyTimeOut;
}

bool DEScriptZone::GetIgnoreEmptyTimeOut() { return m_bIgnoreEmptyTimeOut; }

void DEScriptZone::NotifyInitializeVariables() {
  try {
    lua_tinker::table ZoneCreateInfo(m_pLuaState);

    lua_tinker::call<void>(m_pLuaState, "OnInitializeVariables",
                           ZoneCreateInfo);

    m_Name = ZoneCreateInfo.get<const char *>("Name");
    m_TemplateZoneID = ZoneCreateInfo.get<ZoneID_t>("TemplateZoneID");
    m_StartPosX = ZoneCreateInfo.get<ZoneCoord_t>("StartPosX");
    m_StartPosY = ZoneCreateInfo.get<ZoneCoord_t>("StartPosY");

    string strEntranceType;

    strEntranceType = ZoneCreateInfo.get<const char *>("EntranceType");

    if (strEntranceType.compare("indivisual") == 0) {
      m_EntranceType = ENTRANCE_TYPE_INDIVISUAL;
    } else if (strEntranceType.compare("party") == 0) {
      m_EntranceType = ENTRANCE_TYPE_PARTY;

    } else if (strEntranceType.compare("lobby") == 0) {
      m_EntranceType = m_EntranceType = ENTRANCE_TYPE_LOBBY;
    } else {
      m_EntranceType = ENTRANCE_TYPE_INDIVISUAL;
    }

    m_bStaticZone = ZoneCreateInfo.get<bool>("StaticZone");
    m_EmptyTimeOut = ZoneCreateInfo.get<Duration_t>("EmptyTimeOut");

    if (m_ScriptZoneCountMap.find(m_Name) == m_ScriptZoneCountMap.end()) {
      m_ScriptZoneCountMap[m_Name] = 1;
    } else {
      ++m_ScriptZoneCountMap[m_Name];
    }
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

void DEScriptZone::NotifyInitialize() {
  try {
    lua_tinker::call<void>(m_pLuaState, "OnInitialize");
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

void DEScriptZone::NotifyAttackCreature(Creature *pAttackerCreature,
                                        Creature *pDefenderCreature) {
  try {
    lua_tinker::table AttackerCreatureInfo(m_pLuaState);
    lua_tinker::table DefenderCreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(AttackerCreatureInfo, pAttackerCreature);
    this->MakeCreatureInfo(DefenderCreatureInfo, pDefenderCreature);

    lua_tinker::call<void>(m_pLuaState, "OnAttackCreature",
                           AttackerCreatureInfo, DefenderCreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

void DEScriptZone::NotifyKillCreature(Creature *pAttackerCreature,
                                      Creature *pDeadCreature) {
  try {
    lua_tinker::table AttackerCreatureInfo(m_pLuaState);
    lua_tinker::table DeadCreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(AttackerCreatureInfo, pAttackerCreature);
    this->MakeCreatureInfo(DeadCreatureInfo, pDeadCreature);

    lua_tinker::call<void>(m_pLuaState, "OnKillCreature", AttackerCreatureInfo,
                           DeadCreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

void DEScriptZone::NotifyPK(Creature *pAttackerCreature,
                            Creature *pDeadCreature) {
  try {
    lua_tinker::table AttackerCreatureInfo(m_pLuaState);
    lua_tinker::table DeadCreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(AttackerCreatureInfo, pAttackerCreature);
    this->MakeCreatureInfo(DeadCreatureInfo, pDeadCreature);

    lua_tinker::call<void>(m_pLuaState, "OnPK", AttackerCreatureInfo,
                           DeadCreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

void DEScriptZone::NotifyTimer(int iTimerID) {
  try {
    lua_tinker::call<void>(m_pLuaState, "OnTimer", iTimerID);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占
    // do nothing
  }
}

bool DEScriptZone::NotifyBeforeEnterZone(Creature *pCreature) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnBeforeEnterZone",
                                  CreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return false;
  }
}

void DEScriptZone::NotifyAfterEnterZone(Creature *pCreature) {
  try {
    if (!pCreature->isPC())
      return;

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    lua_tinker::call<void>(m_pLuaState, "OnAfterEnterZone", CreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    // do nothing
  }
}

void DEScriptZone::NotifyBeforeLeaveZone(Creature *pCreature) {
  try {
    if (!pCreature->isPC()) {
      return;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    lua_tinker::call<void>(m_pLuaState, "OnBeforeLeaveZone", CreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    // do nothing
  }
}

void DEScriptZone::NotifyAfterLeaveZone(Creature *pCreature) {
  try {
    if (!pCreature->isPC()) {
      return;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    lua_tinker::call<void>(m_pLuaState, "OnAfterLeaveZone", CreatureInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    // do nothing
  }
}

void DEScriptZone::NotifyAddMonster(Monster *pMonster) {
  try {
    lua_tinker::table MonsterInfo(m_pLuaState);

    this->MakeMonsterInfo(MonsterInfo, pMonster);

    lua_tinker::call<void>(m_pLuaState, "OnAddMonster", MonsterInfo);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    // do nothing
  }
}

bool DEScriptZone::NotifyResurrectCreature(Creature *pCreature,
                                           ZONE_COORD &zoneCoord) {
  try {
    lua_tinker::table CreatureInfo(m_pLuaState);
    lua_tinker::table ZoneCoordInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    bool bResult = false;

    bResult = lua_tinker::call<bool>(m_pLuaState, "OnResurrectCreature",
                                     CreatureInfo, ZoneCoordInfo);

    zoneCoord.id = ZoneCoordInfo.get<ZoneID_t>("ZoneID");
    zoneCoord.x = ZoneCoordInfo.get<ZoneID_t>("X");
    zoneCoord.y = ZoneCoordInfo.get<ZoneID_t>("Y");

    return bResult;
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return false;
  }
}

bool DEScriptZone::NotifySay(Creature *pCreature, string &strMesg) {
  try {
    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnSay", CreatureInfo,
                                  strMesg.c_str());
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true; // 占쌨쇽옙占쏙옙占쏙옙 占쏙옙처占쏙옙
                 // 클占쏙옙占싱억옙트占쏙옙占쏙옙 Broadcast 占싹듸옙占쏙옙
                 // 占쏙옙
  }
}

bool DEScriptZone::NotifyUseItemInGear(Creature *pCreature,
                                       ItemClass_t ItemClass,
                                       ItemType_t ItemType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseItemInGear", CreatureInfo,
                                  ItemClass, ItemType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseItemInInventory(Creature *pCreature,
                                            ItemClass_t ItemClass,
                                            ItemType_t ItemType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseItemInInventory",
                                  CreatureInfo, ItemClass, ItemType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseItemInQuick(Creature *pCreature,
                                        ItemClass_t ItemClass,
                                        ItemType_t ItemType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseItemInQuick", CreatureInfo,
                                  ItemClass, ItemType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseInventorySkill(Creature *pCreature,
                                           SkillType_t SkillType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseInventorySkill",
                                  CreatureInfo, SkillType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseObjectSkill(Creature *pCreature,
                                        SkillType_t SkillType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseObjectSkill", CreatureInfo,
                                  SkillType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseNamedSkill(Creature *pCreature,
                                       SkillType_t SkillType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseNamedSkill", CreatureInfo,
                                  SkillType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseSelfSkill(Creature *pCreature,
                                      SkillType_t SkillType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseSelfSkill", CreatureInfo,
                                  SkillType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyUseTileSkill(Creature *pCreature,
                                      SkillType_t SkillType) {
  try {
    if (!pCreature->isPC()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnUseTileSkill", CreatureInfo,
                                  SkillType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

bool DEScriptZone::NotifyBeforeUseMonsterSkill(Creature *pCreature,
                                               SkillType_t SkillType) {
  try {
    if (!pCreature->isMonster()) {
      return false;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    return lua_tinker::call<bool>(m_pLuaState, "OnBeforeUseMonsterSkill",
                                  CreatureInfo, SkillType);
  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    return true;
  }
}

void DEScriptZone::NotifyAfterUseMonsterSkill(Creature *pCreature,
                                              SkillType_t SkillType) {
  try {
    if (!pCreature->isMonster()) {
      return;
    }

    lua_tinker::table CreatureInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    lua_tinker::call<void>(m_pLuaState, "OnAfterUseMonsterSkill", CreatureInfo,
                           SkillType);
  } catch (DEScriptNS::CallException &e) {
    filelog("LuaError.txt", "%s", e.getMessage().c_str());
  }
}

void DEScriptZone::NotifySetStartPos(Creature *pCreature, ZoneCoord_t &StartX,
                                     ZoneCoord_t &StartY) {
  try {
    lua_tinker::table CreatureInfo(m_pLuaState);
    lua_tinker::table PositionInfo(m_pLuaState);

    this->MakeCreatureInfo(CreatureInfo, pCreature);

    PositionInfo.set<ZoneCoord_t>("X", pCreature->getX());
    PositionInfo.set<ZoneCoord_t>("Y", pCreature->getY());

    lua_tinker::call<void>(m_pLuaState, "OnSetStartPos", CreatureInfo,
                           PositionInfo);

    StartX = PositionInfo.get<ZoneCoord_t>("X");
    StartY = PositionInfo.get<ZoneCoord_t>("Y");

  } catch(DEScriptNS::CallException &e) {
    // 占쏙옙크占쏙옙트占쏙옙 占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占 占쏙옙占

    StartX = GetStartPosX();
    StartY = GetStartPosY();
  }
}

lua_tinker::table DEScriptZone::GetCreature(ObjectID_t objectID) {
  lua_tinker::table CreatureInfo(m_pLuaState);

  Creature *pCreature = m_pZone->getPCManager()->getCreature(objectID);

  this->MakeCreatureInfo(CreatureInfo, pCreature);

  return CreatureInfo;
}

lua_tinker::table DEScriptZone::GetMonster(ObjectID_t objectID) {
  lua_tinker::table MonsterInfo(m_pLuaState);

  Creature *pCreature = m_pZone->getMonsterManager()->getCreature(objectID);

  if (pCreature != NULL && pCreature->isMonster()) {
    Monster *pMonster = dynamic_cast<Monster *>(pCreature);

    this->MakeMonsterInfo(MonsterInfo, pMonster);
  } else {
    this->MakeMonsterInfo(MonsterInfo, NULL);
  }

  return MonsterInfo;
}

void DEScriptZone::SetPKZone(bool bPK) { m_pZone->setPKZone(bPK); }
void DEScriptZone::MonsterUseSkill(lua_tinker::table CreatureInfo,
                                   lua_tinker::table MonsterInfo) {
  Creature *pCreature = CreatureInfo.get<Creature *>("_pCreature");

  Assert(pCreature != NULL);

  Monster *pMonster = dynamic_cast<Monster *>(pCreature);

  Assert(pMonster != NULL);
  Zone *pZone = pMonster->getZone();
  Assert(pZone != NULL);

  string strAI;
  string strDeadAI;

  strAI = MonsterInfo.get<const char *>("AI");
  strDeadAI = MonsterInfo.get<const char *>("DeadAI");

  pMonster->setBrain(NULL);

  if (!strAI.empty()) {
    MonsterAI *pMonsterAI = new MonsterAI(pMonster, strAI, strDeadAI);

    pMonster->setBrain(pMonsterAI);
  }
  /*MonsterAI *pBrain = pMonster->getBrain();
  Creature* pEnemy = pMonster->getPrimaryEnemy();
  Assert( pEnemy != NULL );
  if ( pBrain != NULL )
  {
          pBrain->useSkill(pEnemy, skilltype, 100);
  }*/
}
void DEScriptZone::GiveDianJuan(lua_tinker::table CreatureInfo,
                                unsigned int amount) {

  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

  if (pPC == NULL || !pPC->increaseMarketCashEx(amount)) {
    filelog("MarketCurrencyError.log",
            "[DEScriptZone::GiveDianJuan] MarketCash grant failed amount=%u",
            amount);
    return;
  }

  GCModifyInformation gcMI;
  gcMI.addLongData(MODIFY_DIANJUAN, pPC->getDianJuan());
  if (pPC->getPlayer() != NULL)
    pPC->getPlayer()->sendPacket(&gcMI);
}
void DEScriptZone::GiveJiFen(lua_tinker::table CreatureInfo,
                             unsigned int amount) {

  Creature *pCreature;

  pCreature = CreatureInfo.get<Creature *>("_pCreature");

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);

  if (pPC == NULL || !pPC->increaseMarketPointEx(amount)) {
    filelog("MarketCurrencyError.log",
            "[DEScriptZone::GiveJiFen] MarketPoint grant failed amount=%u",
            amount);
    return;
  }

  GCModifyInformation gcMI;
  gcMI.addLongData(MODIFY_JIFEN, pPC->getJiFen());
  if (pPC->getPlayer() != NULL)
    pPC->getPlayer()->sendPacket(&gcMI);
}
