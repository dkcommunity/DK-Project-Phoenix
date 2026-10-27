#!/bin/bash
TARGET_H="/mnt/d/DARKEDEN FILES/670/darkeden/670DarkEdenServerSRC/src/server/gameserver/ScriptManager.h"
TARGET_CPP="/mnt/d/DARKEDEN FILES/670/darkeden/670DarkEdenServerSRC/src/server/gameserver/ScriptManager.cpp"

# Modify ScriptManager.h
sed -i '/class ScriptManager/i struct lua_State;' "$TARGET_H"
sed -i 's/void init() throw(Error) { load("PUBLIC"); }/void init() throw(Error);/' "$TARGET_H"
sed -i '/public:/a \\ \\ \\ \\ struct lua_State* getLuaState() { return m_pLuaState; }' "$TARGET_H"
sed -i '/private:/a \\ \\ \\ \\ struct lua_State* m_pLuaState;' "$TARGET_H"

# Modify ScriptManager.cpp
sed -i '/#include "ScriptManager.h"/a #include "script/lua_tinker.h"' "$TARGET_CPP"

cat >> "$TARGET_CPP" <<EOF

//////////////////////////////////////////////////////////////////////////////
// init
//////////////////////////////////////////////////////////////////////////////
void ScriptManager::init() throw(Error)
{
    m_pLuaState = lua_open();
    luaL_openlibs(m_pLuaState);
    lua_tinker::init(m_pLuaState);
    load("PUBLIC");
}
EOF
