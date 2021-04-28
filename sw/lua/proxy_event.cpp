#include "proxy.hpp"

#include "../environment.hpp"
#include "../model/event.hpp"

namespace sim::lua {

/*
 * :reset()

 * :state() : bool, bool
 * :waitState(stb : bool, ack : bool)

 * :stbState() : bool
 * :stbData() : integer
 * :stbBits() : integer
 * :stb(assert : bool, data : integer := 0) : bool
 * :waitStb(assert : bool, data : integer := 0)

 * :ackState() : bool
 * :ackData() : integer
 * :ackBits() : integer
 * :ack(assert : bool, data : integer := 0) : bool
 * :waitAck(assert : bool, data : integer := 0)
 */

int event__gc(lua_State *);
int event_reset(lua_State *);
int event_state(lua_State *);
int event_waitState(lua_State *);
int event_stbState(lua_State *);
int event_stbData(lua_State *);
int event_stbBits(lua_State *);
int event_stb(lua_State *);
int event_waitStb(lua_State *);
int event_ackState(lua_State *);
int event_ackData(lua_State *);
int event_ackBits(lua_State *);
int event_ack(lua_State *);
int event_waitAck(lua_State *);

const char * t_dfaccto_event = "dfaccto_event";

const luaL_Reg f_dfaccto_event[] = {
 {"__gc",      event__gc},
 {"reset",     event_reset},
 {"state",     event_state},
 {"waitState", event_waitState},
 {"stbState",  event_stbState},
 {"stbData",   event_stbData},
 {"stbBits",   event_stbBits},
 {"stb",       event_stb},
 {"waitStb",   event_waitStb},
 {"ackState",  event_ackState},
 {"ackData",   event_ackData},
 {"ackBits",   event_ackBits},
 {"ack",       event_ack},
 {"waitAck",   event_waitAck},
 {nullptr,     nullptr}};


void open_event(lua_State * L)
{
  luaL_newmetatable(L, t_dfaccto_event);
  luaL_setfuncs(L, f_dfaccto_event, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
}

void new_event(lua_State * L, Context * context)
{
  // expects 1: name, 2: type, 3: params
  const char * name = lua_tostring(L, 1);
  lua_getfield(L, 3, "autostb");
  bool autostb = luaL_opt(L, lua_toboolean, -1, false);
  lua_getfield(L, 3, "autoack");
  bool autoack = luaL_opt(L, lua_toboolean, -1, false);

  sim::model::Event * model = (sim::model::Event *)lua_newuserdata(L, sizeof(sim::model::Event));
  new (model) sim::model::Event(context->env(), name, autostb, autoack);
  luaL_getmetatable(L, t_dfaccto_event);
  lua_setmetatable(L, -2);
}

// :__gc()
int event__gc(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  model->~Event();
  return 0;
}

// :reset()
int event_reset(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  model->reset();
  return 0;
}


// :state() : bool, bool
int event_state(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);

  lua_pushboolean(L, model->stbSet());
  lua_pushboolean(L, model->ackSet());
  return 2;
}

// :waitState(stb : bool, ack : bool)
int event_waitState(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  luaL_checkany(L, 2);
  bool stb = lua_toboolean(L, 2);
  luaL_checkany(L, 3);
  bool ack = lua_toboolean(L, 3);

  if (stb == model->stbSet() && ack == model->ackSet()) {
    return 0;
  } else {
    lua_pushinteger(L, model->sigState(stb, ack));
    return lua_yield(L, 1);
  }
}


// :stbState() : boolean
int event_stbState(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  lua_pushboolean(L, model->stbSet());
  return 1;
}

// :stbData() : integer
int event_stbData(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  lua_pushinteger(L, model->stbData());
  return 1;
}

// :stbBits() : integer
int event_stbBits(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  lua_pushinteger(L, model->stbBits());
  return 1;
}

// :stb(assert : bool, data : integer := 0) : bool
int event_stb(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  luaL_checkany(L, 2);
  bool assert = lua_toboolean(L, 2);
  lua_Integer data = luaL_optinteger(L, 3, 0);
  lua_pushboolean(L, model->stb(assert, data));
  return 1;
}

int event_waitStb_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  luaL_checkany(L, 2);
  bool assert = lua_toboolean(L, 2);
  lua_Integer data = luaL_optinteger(L, 3, 0);

  if (model->stb(assert, data)) {
    return 0;
  } else {
    lua_pushinteger(L, model->sigCanStb(assert));
    return lua_yieldk(L, 1, 0, event_waitStb_k);
  }
}

// :waitStb(assert : bool, data : integer := 0)
int event_waitStb(lua_State * L)
{
  return event_waitStb_k(L, 0, 0);
}


// :ackState() : boolean
int event_ackState(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  lua_pushboolean(L, model->ackSet());
  return 1;
}
// :ackData() : integer
int event_ackData(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  lua_pushinteger(L, model->ackData());
  return 1;
}

// :ackBits() : integer
int event_ackBits(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  lua_pushinteger(L, model->ackBits());
  return 1;
}

// :ack(assert : bool, data : integer := 0) : bool
int event_ack(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  luaL_checkany(L, 2);
  bool assert = lua_toboolean(L, 2);
  lua_Integer data = luaL_optinteger(L, 3, 0);
  lua_pushboolean(L, model->ack(assert, data));
  return 1;
}

int event_waitAck_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_dfaccto_event);
  luaL_checkany(L, 2);
  bool assert = lua_toboolean(L, 2);
  lua_Integer data = luaL_optinteger(L, 3, 0);

  if (model->ack(assert, data)) {
    return 0;
  } else {
    lua_pushinteger(L, model->sigCanAck(assert));
    return lua_yieldk(L, 1, 0, event_waitAck_k);
  }
}

// :waitAck(assert : bool, data : integer := 0)
int event_waitAck(lua_State * L)
{
  return event_waitAck_k(L, 0, 0);
}

} // namespace sim::lua
