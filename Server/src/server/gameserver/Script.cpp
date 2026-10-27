////////////////////////////////////////////////////////////////////////////////
// Filename    : Script.cpp
// Description : DB'den yuklenen NPC dialog/quest script entry'leri.
//
// 2026-05-12: Phoenix Faz 2.3 LUA migration ertelendi, ScriptManager stub'ti.
// ActionRandomSay/TriggerParser/QuestBoard cagrilari NULL dondurup spam
// "[Warn] ActionRandomSay: missing script id=X" yaratiyordu. Bu dosya
// DB Script tablosundan Subject/Content cifti yukleyen minimal
// (Lua-free) implementation. Server tarafi sadece getScriptID() kullanir
// (gerisi client'a yansir).
////////////////////////////////////////////////////////////////////////////////

#include "Script.h"
#include "Assert.h"
#include "Properties.h"

Script::Script() {
  m_ScriptID = 0;
  m_SubjectCount = 0;
  m_ContentCount = 0;
}

Script::Script(const string &filename) {
  m_ScriptID = 0;
  m_SubjectCount = 0;
  m_ContentCount = 0;
  load(filename);
}

Script::~Script() {}

void Script::load(const string &filename) {
  // Phoenix: file-based load kullanilmiyor. Script'ler DB'den
  // ScriptManager::load() icinde yuklenip setScriptID/setSubject/setContent
  // ile doldurulur. No-op.
}

void Script::setScriptID(ScriptID_t id) {
  m_ScriptID = id;
}

void Script::setSubject(unsigned int index, const string &text) {
  if (index >= SCRIPT_MAX_SUBJECTS) return;
  m_Subjects[index] = text;
  m_SubjectCodes[index] = 0;
  if (index + 1 > m_SubjectCount) m_SubjectCount = index + 1;
}

void Script::setContent(unsigned int index, const string &text) {
  if (index >= SCRIPT_MAX_CONTENTS) return;
  m_Contents[index] = text;
  if (index + 1 > m_ContentCount) m_ContentCount = index + 1;
}

string Script::toString() const {
  string s = "Script(id=";
  // ScriptID -> short string conversion
  char buf[32];
  snprintf(buf, sizeof(buf), "%u", (unsigned)m_ScriptID);
  s += buf;
  s += ",subjects=";
  snprintf(buf, sizeof(buf), "%u", m_SubjectCount);
  s += buf;
  s += ",contents=";
  snprintf(buf, sizeof(buf), "%u", m_ContentCount);
  s += buf;
  s += ")";
  return s;
}

const string &Script::getSubject(unsigned int index) const {
  if (index < m_SubjectCount)
    return m_Subjects[index];
  static const string emptyString = "";
  return emptyString;
}

const string &Script::getContent(unsigned int index) const {
  if (index < m_ContentCount)
    return m_Contents[index];
  static const string emptyString = "";
  return emptyString;
}

ScriptID_t Script::getScriptID() const {
    return m_ScriptID;
}
