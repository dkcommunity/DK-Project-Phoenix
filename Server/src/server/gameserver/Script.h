#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "../../Core/types/QuestTypes.h"
#include "Exception.h"
#include "Types.h"
#include <string>

using namespace std;

#define SCRIPT_MAX_SUBJECTS 64
#define SCRIPT_MAX_SUBJECTS 64
#define SCRIPT_MAX_CONTENTS 64

class Script {
public:
  Script();
  Script(const string &filename);
  ~Script();

  void load(const string &filename);
  string toString() const;
  const string &getSubject(unsigned int index) const;
  const string &getContent(unsigned int index) const;
  ScriptID_t getScriptID() const;
  unsigned int getSubjectCount() const { return m_SubjectCount; }
  unsigned int getContentCount() const { return m_ContentCount; }

  // 2026-05-12: DB-driven loader (ScriptManager::load -> setScriptID/setSubject/setContent).
  void setScriptID(ScriptID_t id);
  void setSubject(unsigned int index, const string &text);
  void setContent(unsigned int index, const string &text);

private:
  ScriptID_t m_ScriptID;
  string m_Subjects[SCRIPT_MAX_SUBJECTS];
  DWORD m_SubjectCodes[SCRIPT_MAX_SUBJECTS];
  unsigned int m_SubjectCount;

  string m_Contents[SCRIPT_MAX_CONTENTS];
  unsigned int m_ContentCount;
};

#endif
