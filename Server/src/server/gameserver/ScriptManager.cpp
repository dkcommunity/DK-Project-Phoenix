////////////////////////////////////////////////////////////////////////////////
// Filename    : ScriptManager.cpp
// Description : NPC dialog/quest script manager.
//
// 2026-05-12: Phoenix FAZ 2.3 LUA migration ertelendiginden stub'ti.
//             DB-driven minimal implementation: DARKEDEN.Script tablosundan
//             yukler (1413 row / 151 NPC). ActionRandomSay, TriggerParser,
//             QuestBoard cagrilari icin getScript() ID->Script* lookup yapar.
//             Lua state acmaz — re-enable icin update_scriptmanager.sh
//             referans.
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"
#include "Script.h"
#include "SXml.h"
#include "DB.h"
#include "Assert.h"
#include "script/lua_tinker.h"
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <stdio.h>

ScriptManager *g_pPublicScriptManager = NULL;

void ScriptManager::init() {
  // 2026-05-12: Lua state init.
  // DEScriptZone constructor (script/DEScriptZone.cpp:47-150) m_pLuaState'i
  // her DynamicZone instance icin lua_tinker::class_add/class_def cagrisi
  // ile kullaniyor. NULL Lua state -> tinker icinde NULL deref -> segfault.
  // ScriptManager init asamasinda Lua state acilmazsa DynamicZone yaratimi
  // (InstanceDoungeon, MasterLair, dynamic teleport) crash uretir.
  if (m_pLuaState == NULL) {
    m_pLuaState = lua_open();
    if (m_pLuaState != NULL) {
      luaL_openlibs(m_pLuaState);
      lua_tinker::init(m_pLuaState);
    } else {
      fprintf(stderr, "[Error] ScriptManager::init: lua_open() failed\n");
      fflush(stderr);
    }
  }

  // ownerID "PUBLIC" history-only — Phoenix tum Script'leri filter'siz
  // yukluyor (server tarafi sadece getScriptID() kullanir; subject/content
  // client'a yansir).
  load("PUBLIC");
}

void ScriptManager::load(const string &ownerID) {
  // Mevcut entry'leri temizle (re-load durumunda leak olmasin).
  for (map<ScriptID_t, Script*>::iterator it = m_Scripts.begin();
       it != m_Scripts.end(); ++it) {
    delete it->second;
  }
  m_Scripts.clear();

  Statement *pStmt = NULL;
  int loaded = 0;
  int failed = 0;
  BEGIN_DB
  {
    pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
    Result *pResult = pStmt->executeQuery(
        "SELECT ScriptID, Subject, Content FROM Script");

    while (pResult->next()) {
      uint i = 0;
      ScriptID_t scriptID = (ScriptID_t)pResult->getInt(++i);
      string subj         = pResult->getString(++i);
      string content      = pResult->getString(++i);

      if (scriptID == 0) {
        // ScriptID 0 reserved — atla.
        failed++;
        continue;
      }

      Script *pScript = new Script();
      pScript->setScriptID(scriptID);
      pScript->setSubject(0, subj);
      pScript->setContent(0, content);

      map<ScriptID_t, Script*>::iterator it = m_Scripts.find(scriptID);
      if (it != m_Scripts.end()) {
        // Duplicate (shouldn't happen — PK constraint var DB'de) — replace.
        delete it->second;
        it->second = pScript;
      } else {
        m_Scripts[scriptID] = pScript;
      }
      loaded++;
    }

    SAFE_DELETE(pStmt);
  }
  END_DB(pStmt)

  printf("ScriptManager::load(%s) : %d script loaded (skipped=%d)\n",
         ownerID.c_str(), loaded, failed);
}

Script *ScriptManager::getScript(ScriptID_t scriptID) const {
  map<ScriptID_t, Script*>::const_iterator it = m_Scripts.find(scriptID);
  if (it == m_Scripts.end()) return NULL;
  return it->second;
}

const XMLTree *ScriptManager::getScriptXML(ScriptID_t scriptID) const {
  // XMLTree-tabanli legacy API — Phoenix bu yolu kullanmiyor; dummy node
  // doner ki caller (eski quest parser) NULL deref etmesin.
  static XMLTree dummy("Script");
  return &dummy;
}

void ScriptManager::setScript(ScriptID_t scriptID, Script *pScript) {
  if (pScript == NULL) return;
  map<ScriptID_t, Script*>::iterator it = m_Scripts.find(scriptID);
  if (it != m_Scripts.end()) {
    delete it->second;
    it->second = pScript;
  } else {
    m_Scripts[scriptID] = pScript;
  }
}

string ScriptManager::toString() const {
  char buf[64];
  snprintf(buf, sizeof(buf), "ScriptManager(%zu entries)", m_Scripts.size());
  return string(buf);
}
