#include "proxy.hpp"

#include <iostream>

#include "../environment.hpp"

namespace sim::lua {

int dfaccto_run(lua_State *);
int dfaccto_port(lua_State *);
int dfaccto_ticks(lua_State *);
int dfaccto_waitFor(lua_State *);
int dfaccto_waitUntil(lua_State *);
int dfaccto_reset(lua_State *);
int dfaccto_getReset(lua_State *);
int dfaccto_waitReset(lua_State *);
int dfaccto_stop(lua_State *);

using dfaccto_Constructor = void (*)(lua_State *, Context *);

const luaL_Reg f_dfaccto[] = {
 {"run",       dfaccto_run},
 {"port",      dfaccto_port},
 {"ticks",     dfaccto_ticks},
 {"waitFor",   dfaccto_waitFor},
 {"waitUntil", dfaccto_waitUntil},
 {"reset",     dfaccto_reset},
 {"getReset",  dfaccto_getReset},
 {"waitReset", dfaccto_waitReset},
 {"stop",      dfaccto_stop},
 {nullptr,     nullptr}};

const char * dfaccto_model_types[] = {
  "wire",
  "event",
  // "stream",
  // "memory",
  // "master",
  // "slave",
  // "raw_stream",
  // "raw_master",
  // "raw_slave",
  // "pass_stream, // TODO-lw
  // "pass_memory, // TODO-lw
  nullptr};

const dfaccto_Constructor dfaccto_model_constructors[] = {
  new_wire,
  new_event};
  // new_stream,
  // new_master,
  // new_slave,
  // new_memory,
  // new_raw_stream,
  // new_raw_master,
  // new_raw_slave};


/*
 * port("<system>")
 * new("wire", ...)
 *   :setData(data : integer)
 *   :waitChange()
 *   :waitMatch(data : integer)
 *   :data() : integer
 *   :dataBits() : integer
 * new("event", {autorelease : bool, autocomplete : bool})
 *   :reset()
 *   :state() : string {"complete", "asserted", "received", "released"}
 *   :waitState(state : string)
 *   :trans(state : string, data : integer) : boolean
 *   :waitTrans(state : string, data : integer)
 *   :data(from : string {*"sender", "receiver"})
 *   :dataBits(from : string {*"sender", "receiver"})
 * new("source", {bandwidth : integer, latency : integer, fair : bool})
 *   reset(seed : integer := 0)
 *   write(data : string, last : bool := false, id : integer := 0)
 *   waitWritten(id : integer := 0)
 *   dataBits() : integer
 *   idBits() : integer
 * new("sink", {bandwidth : integer, latency : integer, fair : bool})
 *   reset(seed : integer := 0)
 *   read(bytes : integer := 0, id : integer := 0) : string, bool
 *   readAny(bytes : integer) : string, bool, integer
 *   dataBits() : integer
 *   idBits() : integer
 */

void open_dfaccto(lua_State * L, Context * context)
{
  // init metatables
  open_wire(L);
  open_event(L);

  // setup dfaccto functions
  luaL_newlibtable(L, f_dfaccto);
  lua_pushlightuserdata(L, context);
  luaL_setfuncs(L, f_dfaccto, 1);

  // init port cache table in registry under context key
  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, context);
}

// dfaccto.run(func : function)
int dfaccto_run(lua_State * L)
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
int dfaccto_port(lua_State * L)
{
  static constexpr int ArgName = 1;
  static constexpr int ArgType = 2;
  static constexpr int ArgParams = 3;

  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));

  // get self._ports[name]
  luaL_checkstring(L, ArgName);
  lua_rawgetp(L, LUA_REGISTRYINDEX, context);
  lua_pushvalue(L, ArgName); // _ports, name
  lua_rawget(L, -2); // _ports, _ports[name]

  // port is not present in _ports
  if (lua_isnil(L, -1)) {
    lua_pop(L, 2);

    // validate arguments for "new" mode
    int typeIdx = luaL_checkoption(L, ArgType, nullptr, dfaccto_model_types);
    if (lua_isnoneornil(L, ArgParams)) {
      lua_newtable(L);
    }
    luaL_checktype(L, ArgParams, LUA_TTABLE);

    // construct new model according to (name, type, params)
    dfaccto_model_constructors[typeIdx](L, context); // model

    // store self._ports[name] = model
    lua_rawgetp(L, LUA_REGISTRYINDEX, context); // model, _ports
    lua_pushvalue(L, ArgName); // model, _ports, name
    lua_pushvalue(L, -3); // model, _ports, name, model
    lua_rawset(L, -3); // model, _ports{name = model}
    lua_pop(L, 1); // model
  }

  return 1;
}

// dfaccto.ticks() : integer
int dfaccto_ticks(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  lua_pushinteger(L, sys.ticks());
  return 1;
}

// dfaccto.waitFor(tick_delay : integer)
int dfaccto_waitFor(lua_State * L)
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
int dfaccto_waitUntil(lua_State * L)
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
int dfaccto_reset(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  bool asserted = luaL_opt(L, lua_toboolean, 1, true);
  sys.reset(asserted);
  return 0;
}

// dfaccto.getReset() : boolean
int dfaccto_getReset(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  lua_pushboolean(L, sys.getReset());
  return 1;
}

// dfaccto.waitReset(asserted : boolean := true)
int dfaccto_waitReset(lua_State * L)
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
int dfaccto_stop(lua_State * L)
{
  sim::model::System & sys = ((Context *)lua_touserdata(L, lua_upvalueindex(1)))->env().system();

  sys.stop();
  return 0;
}


} // namespace sim::lua
