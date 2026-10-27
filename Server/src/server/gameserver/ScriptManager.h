#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

#include "Exception.h"
#include "SXml.h"
#include "Script.h"

using namespace std;
#include "Script.h"
#include "Types.h"
#include <map>
#include <string>

using namespace std;

class ScriptManager {
public:
  ScriptManager() { m_pLuaState = NULL; }
  ~ScriptManager() {}

  void init();
  void load(const string &ownerID);

  Script *getScript(ScriptID_t scriptID) const;
  const XMLTree *getScriptXML(ScriptID_t scriptID) const;
  void setScript(ScriptID_t scriptID, Script *pScript);

  string toString() const;

  struct lua_State *getLuaState() { return m_pLuaState; }

private:
  map<ScriptID_t, Script *> m_Scripts;
  struct lua_State *m_pLuaState;
};

extern ScriptManager *g_pPublicScriptManager;

#endif
