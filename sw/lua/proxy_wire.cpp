#include "proxy.hpp"

#include "../environment.hpp"
#include "../model/wire.hpp"

namespace sim::lua::wire {

// :setData(data : integer)
// :waitChange()
// :waitMatch(data : integer)
// :data() : integer
// :bits() : integer

int f_new(lua_State * L);

int m_gc(lua_State *);
int m_setData(lua_State *);
int m_waitChange(lua_State *);
int m_waitMatch(lua_State *);
int m_data(lua_State *);
int m_bits(lua_State *);

const char * n_wire = "Wire";

const char * t_wire = "dfaccto.wire";

const luaL_Reg m_wire[] = {
 {"__gc",       m_gc},
 {"setData",    m_setData},
 {"waitChange", m_waitChange},
 {"waitMatch",  m_waitMatch},
 {"data",       m_data},
 {"bits",       m_bits},
 {nullptr,      nullptr}};


void open(lua_State * L, Context * context)
{
  // setup metatable
  luaL_newmetatable(L, t_wire);
  luaL_setfuncs(L, m_wire, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // register constructor
  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPort);
  lua_pushlightuserdata(L, context);
  lua_pushcclosure(L, f_new, 1);
  lua_setfield(L, -2, n_wire);
  lua_pop(L, 1);
}


// new(name : string, params : table | nil)
int f_new(lua_State * L)
{
  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));
  const char * name = lua_tostring(L, 1);

  sim::model::Wire * model = (sim::model::Wire *)lua_newuserdatauv(L, sizeof(sim::model::Wire), 0);
  new (model) sim::model::Wire(context->env(), name);
  luaL_getmetatable(L, t_wire);
  lua_setmetatable(L, -2);

  return 1;
}

// :__gc()
int m_gc(lua_State * L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_wire);
  model->~Wire();
  return 0;
}

// :setData(data : bitv)
int m_setData(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_wire);
  sim::BitVector * bitv = sim::lua::bitv::getBitVector(L, 2, model->dataBits());
  model->data(*bitv);
  return 0;
}
//TODO-lw convenience setters according to bitv?

// :waitChange()
int m_waitChange(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_wire);

  lua_pushinteger(L, model->sigChanged());
  return lua_yield(L, 1);
}

// :waitMatch(data : integer)
int m_waitMatch(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_wire);
  lua_Integer data = luaL_checkinteger(L, 2);

  lua_pushinteger(L, model->sigMatch(data));
  return lua_yield(L, 1);
}

// :data() : bitv
int m_data(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_wire);
  sim::lua::bitv::pushBitVector(L, model->data());
  return 1;
}

// :bits() : integer
int m_bits(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_wire);
  lua_pushinteger(L, model->dataBits());
  return 1;
}

} // namespace sim::lua
