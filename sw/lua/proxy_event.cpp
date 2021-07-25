#include "proxy.hpp"

#include "../environment.hpp"
#include "../model/event.hpp"

#include <iostream>

/*
 * :reset()

 * :state() : string
 * :waitState(state : string {Idle, StbAssert, AckAssert, StbRelease, AckRelease}) !blocking

 * :stbSignal() : bool
 * :stbData() : bitv
 * :stbBits() : integer
 * :canStb(mode : string {"assert", "release", "cycle"}) : bool
 * :stb(assert : string {"assert", "release", "cycle"}, data : bitv := nil) : integer or nil !blocking
     stb("assert", stbData)
     stb("release") -> ackData
     stb("cycle", stbData) -> ackData

 * :ackSignal() : bool
 * :ackData() : bitv
 * :ackBits() : integer
 * :canAck(mode : string {"assert", "release", "cycle"}) : bool
 * :ack(assert : string {"assert", "release", "cycle"}, data : integer := nil) integer or nil !blocking
     ack("assert", ackData) -> stbData
     ack("release")
     ack("cycle", ackData) -> stbData
 */

namespace sim::lua::event {

int f_new(lua_State * L);

int m_gc(lua_State *);
int m_reset(lua_State *);
int m_state(lua_State *);
int m_waitState(lua_State *);
int m_stbSignal(lua_State *);
int m_stbData(lua_State *);
int m_stbBits(lua_State *);
int m_stb(lua_State *);
int m_ackSignal(lua_State *);
int m_ackData(lua_State *);
int m_ackBits(lua_State *);
int m_ack(lua_State *);

const char * n_event = "Event";

const char * t_event = "dfaccto.event";

const luaL_Reg f_dfaccto_event[] = {
  {"__gc",      m_gc},
  {"reset",     m_reset},
  {"state",     m_state},
  {"waitState", m_waitState},
  {"stbSignal", m_stbSignal},
  {"stbData",   m_stbData},
  {"stbBits",   m_stbBits},
  {"stb",       m_stb},
  {"ackSignal", m_ackSignal},
  {"ackData",   m_ackData},
  {"ackBits",   m_ackBits},
  {"ack",       m_ack},
  {nullptr,     nullptr}};

const char * event_states[] = {
  "Idle",
  "StbAssert",
  "AckAssert",
  "StbRelease",
  "AckRelease",
  nullptr};

const size_t event_states_len = sizeof(event_states) / sizeof(const char *) - 1;

const sim::model::Event::State event_states_rev[] = {
  sim::model::Event::Idle,
  sim::model::Event::StbAssert,
  sim::model::Event::AckAssert,
  sim::model::Event::StbRelease,
  sim::model::Event::AckRelease};

const char * event_modes[] = {
  "assert",
  "release",
  "cycle",
  nullptr};

void open(lua_State * L, Context * context)
{
  // setup metatable
  luaL_newmetatable(L, t_event);
  luaL_setfuncs(L, f_dfaccto_event, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // register constructor
  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPort);
  lua_pushlightuserdata(L, context);
  lua_pushcclosure(L, f_new, 1);
  lua_setfield(L, -2, n_event);
  lua_pop(L, 1);
}


// new(name : string, params : table | nil)
int f_new(lua_State * L)
{
  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));
  const char * name = lua_tostring(L, 1);

  if (lua_isnil(L, 2)) {
    lua_newtable(L);
    lua_replace(L, 2);
  }
  lua_getfield(L, 2, "swapRelease");
  bool swapRelease = luaL_opt(L, lua_toboolean, -1, false);
  lua_pop(L, 1);

  sim::model::Event * model = (sim::model::Event *)lua_newuserdata(L, sizeof(sim::model::Event));
  new (model) sim::model::Event(context->env(), name, swapRelease);
  luaL_getmetatable(L, t_event);
  lua_setmetatable(L, -2);

  return 1;
}

// :__gc()
int m_gc(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  model->~Event();
  return 0;
}

// :reset()
int m_reset(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  model->reset();
  return 0;
}


// :state() : string
int m_state(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);

  size_t stateIdx = model->state() & sim::model::Event::Index;
  if (stateIdx < event_states_len) {
    lua_pushstring(L, event_states[stateIdx]);
    return 1;
  } else {
    lua_pushstring(L, "invalid state");
    return lua_error(L);
  }
}

// :waitState(state : string)
int m_waitState(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  size_t stateIdx = luaL_checkoption(L, 2, nullptr, event_states);
  sim::model::Event::State state = event_states_rev[stateIdx];

  if (model->state() == state) {
    return 0;
  } else {
    lua_pushinteger(L, model->sigState(state));
    return lua_yield(L, 1);
  }
}

// :stbSignal() : boolean
int m_stbSignal(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  lua_pushboolean(L, model->stbSet());
  return 1;
}

// :stbData() : bitv
int m_stbData(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  sim::lua::bitv::pushBitVector(L, model->stbData());
  return 1;
}

// :stbBits() : integer
int m_stbBits(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  lua_pushinteger(L, model->stbBits());
  return 1;
}

int event_stb_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);

  switch (ctx) {
  case 0: // "assert"
  case 2: // "cycle" assert phase
    if (model->canStb(true) == model->state()) {
      sim::BitVector * data = sim::lua::bitv::getBitVector(L, 3, model->stbBits());
      model->stb(true, *data);
      if (ctx == 0) {
        return 0;
      } else {
        // continue with release phase in next cycle
        return lua_yieldk(L, 0, 3, event_stb_k);
      }
    } else {
      lua_pushinteger(L, model->sigState(model->canStb(true)));
      return lua_yieldk(L, 1, ctx, event_stb_k);
    }

  case 1: // "release"
  case 3: // "cycle" release phase
    if (model->canStb(false) == model->state()) {
      sim::BitVector * data = sim::lua::bitv::getBitVector(L, 3, model->stbBits());
      model->stb(false, *data);
      sim::lua::bitv::pushBitVector(L, model->ackData());
      return 1;
    } else {
      lua_pushinteger(L, model->sigState(model->canStb(false)));
      return lua_yieldk(L, 1, ctx, event_stb_k);
    }

  default:
    lua_pushstring(L, "inconsistent continuation context");
    return lua_error(L);
  }
}

// :stb(mode : string, data : integer := 0)
int m_stb(lua_State * L)
{
  lua_KContext mode = luaL_checkoption(L, 2, nullptr, event_modes);
  return event_stb_k(L, 0, mode);
}


// :ackSignal() : boolean
int m_ackSignal(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  lua_pushboolean(L, model->ackSet());
  return 1;
}
// :ackData() : integer
int m_ackData(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  sim::lua::bitv::pushBitVector(L, model->ackData());
  return 1;
}

// :ackBits() : integer
int m_ackBits(lua_State * L)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);
  lua_pushinteger(L, model->ackBits());
  return 1;
}

int event_ack_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::Event * model = (sim::model::Event *)luaL_checkudata(L, 1, t_event);

  switch (ctx) {
  case 0: // "assert"
  case 2: // "cycle" assert phase
    if (model->canAck(true) == model->state()) {
      sim::BitVector * data = sim::lua::bitv::getBitVector(L, 3, model->stbBits());
      model->ack(true, *data);
      if (ctx == 0) {
        sim::lua::bitv::pushBitVector(L, model->stbData());
        return 1;
      } else {
        // continue with release phase in next cycle
        return lua_yieldk(L, 0, 3, event_ack_k);
      }
    } else {
      lua_pushinteger(L, model->sigState(model->canAck(true)));
      return lua_yieldk(L, 1, ctx, event_ack_k);
    }

  case 1: // "release"
  case 3: // "cycle" release phase
    if (model->canAck(false) == model->state()) {
      sim::BitVector * data = sim::lua::bitv::getBitVector(L, 3, model->stbBits());
      model->ack(false, *data);
      if (ctx == 1) {
        return 0;
      } else {
        sim::lua::bitv::pushBitVector(L, model->stbData());
        return 1;
      }
    } else {
      lua_pushinteger(L, model->sigState(model->canAck(false)));
      return lua_yieldk(L, 1, ctx, event_ack_k);
    }

  default:
    lua_pushstring(L, "inconsistent continuation context");
    return lua_error(L);
  }
}

// :waitAck(assert : bool, data : integer := 0)
int m_ack(lua_State * L)
{
  lua_KContext mode = luaL_checkoption(L, 2, nullptr, event_modes);
  return event_ack_k(L, 0, mode);
}

} // namespace sim::lua
