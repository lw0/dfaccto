#include "proxy.hpp"

#include <iostream>

#include "../environment.hpp"

namespace sim::lua::sys {

int f_run(lua_State *);
int f_port(lua_State *);
int f_pass(lua_State *);
int f_ticks(lua_State *);
int f_next(lua_State *);
int f_waitFor(lua_State *);
int f_waitUntil(lua_State *);
int f_reset(lua_State *);
int f_getReset(lua_State *);
int f_waitReset(lua_State *);
int f_stop(lua_State *);
int f_condition(lua_State *);

const luaL_Reg f_sys[] = {
 {"run",       f_run},
 {"port",      f_port},
 {"pass",      f_pass},
 {"ticks",     f_ticks},
 {"next",      f_next},
 {"waitFor",   f_waitFor},
 {"waitUntil", f_waitUntil},
 {"reset",     f_reset},
 {"getReset",  f_getReset},
 {"waitReset", f_waitReset},
 {"stop",      f_stop},
 {"condition", f_condition},
 {nullptr,     nullptr}};


int m_gc(lua_State *);
int m_state(lua_State *);
int m_wait(lua_State *);
int m_set(lua_State *);

const char * t_condition = "dfaccto.condition";

const luaL_Reg m_condition[] = {
  {"__gc", m_gc},
  {"state", m_state},
  {"wait", m_wait},
  {"set", m_set},
  {nullptr, nullptr}};

void open_condition(lua_State * L)
{
  luaL_newmetatable(L, t_condition);
  luaL_setfuncs(L, m_condition, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}


void open(lua_State * L, Context * context)
{
  open_condition(L);

  // setup sys table
  luaL_newlibtable(L, f_sys);
  lua_pushlightuserdata(L, context);
  luaL_setfuncs(L, f_sys, 1);

  // init port cache table in registry under context key
  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, context);
}


// sys.run(func : function)
int f_run(lua_State * L)
{
  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));

  // check argument func
  luaL_checktype(L, 1, LUA_TFUNCTION);

  // create and register new thread
  lua_State * T = lua_newthread(L);
  lua_Integer threadRef = luaL_ref(L, LUA_REGISTRYINDEX);
  context->thread(threadRef);

  // prime new thread with func
  lua_pushvalue(L, 1);
  lua_xmove(L, T, 1);

  return 0;
}

// dfaccto.port(name : string, {type : string, params : table}) : userdata(port)
int f_port(lua_State * L)
{
  static constexpr int ArgName = 1;
  static constexpr int ArgType = 2;
  static constexpr int ArgParams = 3;

  // _cache is REGISTRY[RegKeyCache] : table{[name] = port_instance}
  // _ports is REGISTRY[RegKeyPort] : table{[name] = port_constructor}

  luaL_checkstring(L, ArgName);
  if (lua_isnone(L, ArgType)) {
    lua_pushnil(L);
  }
  if (lua_isnone(L, ArgParams)) {
    lua_pushnil(L);
  }

  // read port cache entry
  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyCache);   // _cache
  lua_pushvalue(L, ArgName);                        // _cache, name
  lua_rawget(L, -2);                                // _cache, port = _cache[name]

  // port is not present in _cache
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);                                  // _cache

    lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPort);  // _cache, _ports
    lua_pushvalue(L, ArgType);                      // _cache, _ports, name
    lua_rawget(L, -2);                              // _cache, _ports, _ports[name]

    if (lua_isnil(L, -1)) {
      lua_pushfstring(L, "Unknown port type %s for %s", lua_tostring(L, ArgType) , lua_tostring(L, ArgName));
      return lua_error(L);
    }

    // call port constructor
    lua_pushvalue(L, ArgName);                      // _cache, _ports, _ports[name], name
    lua_pushvalue(L, ArgParams);                    // _cache, _ports, _ports[name], name, params
    lua_call(L, 2, 1);                              // _cache, _ports, port

    // store model registry entry
    lua_pushvalue(L, ArgName);                      // _cache, _ports, port, name
    lua_pushvalue(L, -2);                           // _cache, _ports, port, name, port
    lua_rawset(L, -5);                              // _cache{name = port}, _ports, port
  }

  return 1;
}

// dfaccto.pass(priName : string, secName : string, {type : string, params : table}) : userdata(port)
int f_pass(lua_State * L)
{
  static constexpr int ArgPriName = 1;
  static constexpr int ArgSecName = 2;
  static constexpr int ArgType = 3;
  static constexpr int ArgParams = 4;

  // _cache is REGISTRY[RegKeyCache] : table{[name] = port_instance}
  // _passes is REGISTRY[RegKeyPass] : table{[name] = pass_constructor}

  luaL_checkstring(L, ArgPriName);
  luaL_checkstring(L, ArgSecName);
  if (lua_isnone(L, ArgType)) {
    lua_pushnil(L);
  }
  if (lua_isnone(L, ArgParams)) {
    lua_pushnil(L);
  }

  // read model cache entries
  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyCache);   // _cache
  lua_pushvalue(L, ArgPriName);                     // _cache, priName
  lua_rawget(L, -2);                                // _cache, _cache[priName]
  lua_pushvalue(L, ArgSecName);                     // _cache, _cache[priName], secName
  lua_rawget(L, -3);                                // _cache, _cache[priName], pass =_cache[secName]

  if (lua_isnil(L, -2) && lua_isnil(L, -1)) {
    // no cache entries for both names, construct new
    lua_pop(L, 2);                                  // _cache

    lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPass);  // _cache, _passes
    lua_pushvalue(L, ArgType);                      // _cache, _passes, type
    lua_rawget(L, -2);                              // _cache, _passes[type]

    if (lua_isnil(L, -1)) {
      lua_pushfstring(L, "Unknown pass type %s for %s/%s", lua_tostring(L, ArgType), lua_tostring(L, ArgPriName), lua_tostring(L, ArgSecName));
      return lua_error(L);
    }

    // call pass constructor
    lua_pushvalue(L, ArgPriName);                   // _cache, _passes[type], priName
    lua_pushvalue(L, ArgSecName);                   // _cache, _passes[type], priName, secName
    lua_pushvalue(L, ArgParams);                    // _cache, _passes[type], priName, secName, params
    lua_call(L, 3, 1);                              // _cache, _passes[type], pass

    // store cache entries
    lua_pushvalue(L, ArgPriName);                   // _cache, _passes[type], pass, priName
    lua_pushvalue(L, -2);                           // _cache, _passes[type], pass, priName, pass
    lua_rawset(L, -5);                              // _cache{[priName] = pass}, _passes[type], pass
    lua_pushvalue(L, ArgSecName);                   // _cache{[priName] = pass}, _passes[type], pass, secName
    lua_pushvalue(L, -2);                           // _cache{[priName] = pass}, _passes[type], pass, secName, pass
    lua_rawset(L, -5);                              // _cache{[priName] = pass, [secName] = pass}, _passes[type], pass
  } else if (!lua_rawequal(L, -2, -1)) {
    lua_pushstring(L, "at least one of the named ports already exists and both are not equal");
    return lua_error(L);
  }

  return 1;
}


// dfaccto.ticks() -> timestamp : integer
int f_ticks(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  lua_pushinteger(L, sys.ticks());
  return 1;
}

// dfaccto.next()
int f_next(lua_State * L)
{
  return lua_yield(L, 0);
}

// dfaccto.waitFor(tick_delay : integer)
int f_waitFor(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  sim::Ticks now = sys.ticks();
  sim::Ticks target = now + luaL_checkinteger(L, 1);
  if (target > now) {
    lua_pushinteger(L, sys.sigTimer(target));
    return lua_yield(L, 1);
  }
  return 0;
}

// dfaccto.waitUntil(tick_stamp : integer)
int f_waitUntil(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  sim::Ticks now = sys.ticks();
  sim::Ticks target = luaL_checkinteger(L, 1);
  if (target > now) {
    lua_pushinteger(L, sys.sigTimer(target));
    return lua_yield(L, 1);
  }
  return 0;
}

// dfaccto.reset(asserted : boolean := true)
int f_reset(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  bool asserted = luaL_opt(L, lua_toboolean, 1, true);
  sys.reset(asserted);
  return 0;
}

// dfaccto.getReset() -> resetState : boolean
int f_getReset(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  lua_pushboolean(L, sys.getReset());
  return 1;
}

// dfaccto.waitReset(asserted : boolean := true)
int f_waitReset(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  bool asserted = luaL_opt(L, lua_toboolean, 1, true);
  if (sys.getReset() != asserted) {
    lua_pushinteger(L, sys.sigReset(asserted));
    return lua_yield(L, 1);
  } else {
    return 0;
  }
}

// dfaccto.stop()
int f_stop(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  sys.stop();
  return 0;
}

// dfaccto.condition(state : bool := false) : condition
int f_condition(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();
  bool state = lua_toboolean(L, 1);

  sim::model::SystemCondition * cond = (sim::model::SystemCondition *)lua_newuserdata(L, sizeof(sim::model::SystemCondition));
  new (cond) sim::model::SystemCondition(sys, state);
  luaL_getmetatable(L, t_condition);
  lua_setmetatable(L, -2);

  return 1;
}

// condition:__gc()
int m_gc(lua_State * L)
{
  sim::model::SystemCondition * cond = (sim::model::SystemCondition *)luaL_checkudata(L, 1, t_condition);
  cond->~SystemCondition();
  return 0;
}

// condition:state() : bool
int m_state(lua_State * L)
{
  sim::model::SystemCondition * cond = (sim::model::SystemCondition *)luaL_checkudata(L, 1, t_condition);
  lua_pushboolean(L, cond->state());
  return 0;
}

int dfaccto_condition_wait_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::SystemCondition * cond = (sim::model::SystemCondition *)luaL_checkudata(L, 1, t_condition);
  luaL_checkany(L, 2);
  bool state = lua_toboolean(L, 2);

  if (cond->state() == state) {
    return 0;
  } else {
    lua_pushinteger(L, cond->signal(state));
    return lua_yieldk(L, 1, 0, dfaccto_condition_wait_k);
  }
}

// condition:wait(state : bool)
int m_wait(lua_State * L)
{
  return dfaccto_condition_wait_k(L, 0, 0);
}

// condition:set(state : bool)
int m_set(lua_State * L)
{
  sim::model::SystemCondition * cond = (sim::model::SystemCondition *)luaL_checkudata(L, 1, t_condition);
  luaL_checkany(L, 2);
  bool state = lua_toboolean(L, 2);
  cond->set(state);
  return 0;
}


} // namespace sim::lua
