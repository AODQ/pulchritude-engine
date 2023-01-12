#include "setup.h"

#include "pulchritude-core/core.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* static int32_t getLuaLineNumber(lua_State * const state) { */
/*   lua_Debug debug; */
/*   lua_getstack(state, 1, &debug); */
/*   lua_getinfo(state, "nSl", &debug); */
/*   return debug.currentline; */
/* } */

static void dumpLuaStack(
  lua_State * const state,
  int32_t const startIdx,
  int32_t const tabs
);

static void dumpLuaValue(
  lua_State * const state,
  int32_t const stackIdx,
  int32_t const tabs
) {
  for (int it = 0; it < tabs; ++ it) {
    puleLogLn("\t");
  }
  puleLogLn("%d [%s] ", stackIdx, luaL_typename(state, stackIdx));
  switch (lua_type(state, stackIdx)) {
    case LUA_TNUMBER:
      puleLogLn("%g\n",lua_tonumber(state, stackIdx));
    break;
    case LUA_TSTRING:
      puleLogLn("%s\n",lua_tostring(state, stackIdx));
    break;
    case LUA_TBOOLEAN:
      puleLogLn("%s\n", (lua_toboolean(state, stackIdx) ? "true" : "false"));
    break;
    case LUA_TNIL:
      puleLogLn("%s\n", "nil");
    break;
    case LUA_TTABLE:
      puleLogLn("table: {\n");
      size_t const tableLen = lua_rawlen(state, stackIdx);
      int32_t const stackTop = lua_gettop(state);
      for (size_t it = 0; it < tableLen; ++ it) {
        lua_rawgeti(state, stackIdx, it+1);
      }
      dumpLuaStack(state, stackTop, tabs+1);
      for (size_t it = 0; it < tableLen; ++ it) {
        lua_pop(state, 1);
      }
      for (int it = 0; it < tabs; ++ it) {
        puleLogLn("\t");
      }
      puleLogLn("}\n");
    break;
    default:
      puleLogLn("%p\n",lua_topointer(state, stackIdx));
    break;
  }
}

// https://stackoverflow.com/a/59097940
static void dumpLuaStack(
  lua_State * const state,
  int32_t const startIdx,
  int32_t const tabs
) {
  int32_t const top = lua_gettop(state);
  if (startIdx == 1) {
    puleLogDebug("lua stack (%d) ------:", top);
  }
  for (int32_t stackIdx = startIdx; stackIdx <= top; ++ stackIdx) {
    dumpLuaValue(state, stackIdx, tabs);
    PULE_assert(top == lua_gettop(state));
  }
  if (startIdx == 1) {
    puleLogDebug("----------------------");
  }
}

#define luaArgError(fnLabel, argType, argLabel) \
  puleLogError( \
    "Lua error: invalid call to %s@%d; expecting '%s %s'", \
    fnLabel, getLuaLineNumber(state), argType, argLabel \
  ); \
  return 0

#define luaArgCountError(fnLabel, usercount, realcount) \
  puleLogError( \
    "Lua error: invalid call to %s@%d; expecting %d parameters, not %d", \
    fnLabel, getLuaLineNumber(state), realcount, usercount \
  ); \
  return 0

#define luaUserdataNull(argType, argLabel) \
  puleLogError( \
    "Lua error: userdata of '%s %s' is null", \
    argType, argLabel \
  ); \
  return 0


//// This is an automatically generated file that imports all necessary includes
//#include "../../../bindings/lua/binding-includes.h"
//
//#include "../../../bindings/lua/binding-functions.h"
//
//void puleScriptContextSetup(PuleScriptContext const ctx) {
//  lua_State * state = (lua_State *)(ctx.id);
//
//  #include "../../../bindings/lua/binding-states.h"
//}
//
