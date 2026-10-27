#ifndef _DESCRIPT_H_
#define _DESCRIPT_H_

#include "lua.hpp"
#include "lua_tinker.h"

#include "Creature.h"
#include "Effect.h"
#include "Monster.h"
#include "Zone.h"

#include "VSDateTime.h"
#include <map>
#include <string>
// using namespace DEScript; // Collision with class DEScript

namespace lua_tinker {
template <typename RVal>
RVal membercall(lua_State *L, const char *tn, const char *fn) {
  lua_pushcclosure(L, on_error, 0);
  int errfunc = lua_gettop(L);

  lua_pushstring(L, tn);
  lua_gettable(L, LUA_GLOBALSINDEX);
  lua_getfield(L, -1, fn);
  if (lua_isfunction(L, -1)) {
    if (lua_pcall(L, 0, 1, errfunc) != 0) {
      lua_pop(L, 1);
    }

    lua_remove(L, -2);
  } else {
    lua_remove(L, -2);

    ostringstream message;
    message << "lua_tinker::membercall() attempt to call global '" << tn
            << "' '" << fn << "' (not a function)";

    DEScriptNS::CallException e;
    e.setMessage(message.str());

    throw e;
  }

  return pop<RVal>(L);
}

template <typename RVal, typename T1>
RVal membercall(lua_State *L, const char *tn, const char *fn, T1 arg) {
  lua_pushcclosure(L, on_error, 0);
  int errfunc = lua_gettop(L);

  lua_pushstring(L, tn);
  lua_gettable(L, LUA_GLOBALSINDEX);
  lua_getfield(L, -1, fn);
  if (lua_isfunction(L, -1)) {
    push(L, arg);
    if (lua_pcall(L, 1, 1, errfunc) != 0) {
      lua_pop(L, 1);
    }

    lua_remove(L, -2);
  } else {
    lua_remove(L, -2);

    ostringstream message;
    message << "lua_tinker::membercall() attempt to call global '" << tn
            << "' '" << fn << "' (not a function)";

    DEScriptNS::CallException e;
    e.setMessage(message.str());

    throw e;
  }

  return pop<RVal>(L);
}
}; // namespace lua_tinker

typedef map<string, int> TWhiteBoardOfInteger;

class DEScript {
protected:
  lua_State *m_pLuaState;
  static Mutex m_Mutex;

  static TWhiteBoardOfInteger m_WhiteBoardOfInteger;

public:
  DEScript();
  virtual ~DEScript();

  void Load(char *strScript);
  void MakeCreatureInfo(lua_tinker::table &CreatureInfo, Creature *pCreature);
  void MakeMonsterInfo(lua_tinker::table &MonsterInfo, Monster *pMonster);
  void MakeZoneInfo(lua_tinker::table &ZoneInfo, Zone *pZone);
  void MakeEffectInfo(lua_tinker::table &EffectInfo, Effect *pEffect);
  void MakeWarOpenTime(lua_tinker::table &WarOpenTimeInfo);

protected:
  bool IsGod(lua_tinker::table CreatureInfo);
  bool IsDM(lua_tinker::table CreatureInfo);
  bool IsHELPER(lua_tinker::table CreatureInfo);
  bool IsPLAYER(lua_tinker::table CreatureInfo);

  bool IsDead(lua_tinker::table CreatureInfo);
  bool IsAlive(lua_tinker::table CreatureInfo);

  bool IsPC(lua_tinker::table CreatureInfo);
  bool IsMonster(lua_tinker::table CreatureInfo);
  bool IsSlayer(lua_tinker::table CreatureInfo);
  bool IsVampire(lua_tinker::table CreatureInfo);
  bool IsOusters(lua_tinker::table CreatureInfo);

  int playerX(lua_tinker::table CreatureInfo);
  int playerY(lua_tinker::table CreatureInfo);
  int TeamID(lua_tinker::table CreatureInfo);

  Level_t GetLevel(lua_tinker::table CreatureInfo);
  Level_t GetAdvancementClassLevel(lua_tinker::table CreatureInfo);

  void RemoveMonsterBrain(lua_tinker::table MonsterInfo);

  lua_tinker::table SetHP(lua_tinker::table CreatureInfo, HP_t hp);
  lua_tinker::table WarOpenTime();

  long GetCurrentTime();
  long AddTime(long lTimeVal, const char *strAddType, long lAddValue);

  void WriteIntegerOnWhiteBoard(const char *strKey, int iVal);
  int ReadIntegerOnWhiteBoard(const char *strKey, int iDefaultVal);

  void WriteFileLog(const char *pLogFileName, const char *pLogMsg);
};

#endif /* _DESCRIPT_H_ */
