#include "proxy.hpp"

#include "../environment.hpp"
#include "../model/wire.hpp"

namespace sim::lua {

// :setData(data : integer)
// :waitChange()
// :waitMatch(data : integer)
// :data() : integer
// :bits() : integer
int wire__gc(lua_State *);
int wire_setData(lua_State *);
int wire_waitChange(lua_State *);
int wire_waitMatch(lua_State *);
int wire_data(lua_State *);
int wire_bits(lua_State *);

const char * t_dfaccto_wire = "dfaccto_wire";

const luaL_Reg f_dfaccto_wire[] = {
 {"__gc",       wire__gc},
 {"setData",    wire_setData},
 {"waitChange", wire_waitChange},
 {"waitMatch",  wire_waitMatch},
 {"data",       wire_data},
 {"bits",       wire_bits},
 {nullptr,      nullptr}};


void open_wire(lua_State * L)
{
  luaL_newmetatable(L, t_dfaccto_wire);
  luaL_setfuncs(L, f_dfaccto_wire, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
}

void new_wire(lua_State * L, Context * context)
{
  // expects 1: name, 2: type, 3: params
  const char * name = lua_tostring(L, 1);

  sim::model::Wire * model = (sim::model::Wire *)lua_newuserdatauv(L, sizeof(sim::model::Wire), 0);
  new (model) sim::model::Wire(context->env(), name);
  luaL_getmetatable(L, t_dfaccto_wire);
  lua_setmetatable(L, -2);
}

// :__gc()
int wire__gc(lua_State * L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_dfaccto_wire);
  model->~Wire();
  return 0;
}

// :setData(data : integer)
int wire_setData(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_dfaccto_wire);
  lua_Integer data = luaL_checkinteger(L, 2);
  model->data(data);
  return 0;
}

// :waitChange()
int wire_waitChange(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_dfaccto_wire);

  lua_pushinteger(L, model->sigChanged());
  return lua_yield(L, 1);
}

// :waitMatch(data : integer)
int wire_waitMatch(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_dfaccto_wire);
  lua_Integer data = luaL_checkinteger(L, 2);

  lua_pushinteger(L, model->sigMatch(data));
  return lua_yield(L, 1);
}

// :data() : integer
int wire_data(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_dfaccto_wire);
  lua_pushinteger(L, model->data());
  return 1;
}

// :bits() : integer
int wire_bits(lua_State *L)
{
  sim::model::Wire * model = (sim::model::Wire *)luaL_checkudata(L, 1, t_dfaccto_wire);
  lua_pushinteger(L, model->dataBits());
  return 1;
}

} // namespace sim::lua
