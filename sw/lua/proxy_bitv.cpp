#include <lua.hpp>

#include "proxy.hpp"
#include "../bitvector.hpp"

namespace sim::lua::bitv {

int f_new(lua_State * L);
int f_fromInt(lua_State * L);
int f_fromStr(lua_State * L);
int f_fromHex(lua_State * L);
int f_fromBin(lua_State * L);

const luaL_Reg f_bitv[] = {
  {"__call", f_new},
  {"new", f_new},
  {"int", f_fromInt},
  {"str", f_fromStr},
  {"hex", f_fromHex},
  {"bin", f_fromBin},
  {nullptr, nullptr}};

/*
 * new(bits : integer, value : boolean | integer | nil, valid : boolean | integer | nil) : bitv
 * fromStr(value : string) : bitv
 * fromHex(value : string) : bitv
 * fromBin(value : string) : bitv
 */

int m_gc(lua_State * L);
int m_bits(lua_State * L);
int m_get(lua_State * L);
int m_slice(lua_State * L);
int m_valid(lua_State * L);
int m_set(lua_State * L);
int m_append(lua_State * L);
int m_resize(lua_State * L);
int m_toStr(lua_State * L);
int m_toHex(lua_State * L);
int m_toBin(lua_State * L);

const char * t_bitv = "dfaccto.bitv";

const luaL_Reg m_bitv[] = {
  {"__gc",       m_gc},
  {"__call",     m_slice},
  {"__tostring", m_toHex},
  {"bits",       m_bits},
  {"get",        m_get},
  {"slice",      m_slice},
  {"valid",      m_valid},
  {"set",        m_set},
  {"append",     m_append},
  {"resize",     m_resize},
  {"toStr",      m_toStr},
  {"toHex",      m_toHex},
  {"toBin",      m_toBin},
  {nullptr,      nullptr}};

void open(lua_State * L)
{
  luaL_newmetatable(L, t_bitv);
  luaL_setfuncs(L, m_bitv, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newlib(L, f_bitv);
}

sim::BitVector * pushBitVector(lua_State * L, size_t bits, bool value, bool valid)
{
  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector(bits, value, valid);
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);
  return bitv;
}

sim::BitVector * pushBitVector(lua_State * L, const sim::BitVector & other)
{
  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector(other);
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);
  return bitv;
}

sim::BitVector * pushBitVector(lua_State * L, sim::BitVector && other)
{
  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector(std::move(other));
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);
  return bitv;
}

sim::BitVector * getBitVector(lua_State * L, int pos, size_t defaultBits)
{
  if (lua_isnoneornil(L, pos)) {
    sim::BitVector * bitv = pushBitVector(L);
    lua_replace(L, pos);
    return bitv;
  } else if (lua_isinteger(L, pos)) {
    lua_Integer value = lua_tointeger(L, pos);
    sim::BitVector * bitv = pushBitVector(L, defaultBits);
    bitv->unitSet(0, defaultBits, value, sim::UnitOnes);
    lua_replace(L, pos);
    return bitv;
  } else {
    return (sim::BitVector *)luaL_checkudata(L, pos, t_bitv);
  }
}

// .new(bits : number, value : boolean := false, valid : boolean := false) : bitv
int f_new(lua_State * L)
{
  size_t bits = luaL_checkinteger(L, 1);
  bool value = luaL_opt(L, lua_toboolean, 2, false);
  bool valid = luaL_opt(L, lua_toboolean, 3, false);

  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector(bits, value, valid);
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);

  return 1;
}

// .fromInt(num : integer, len : integer := UnitBits) : bitv
int f_fromInt(lua_State * L)
{
  lua_Integer value = luaL_checkinteger(L, 1);
  lua_Integer bits = luaL_opt(L, lua_tointeger, 2, sim::UnitBits);

  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector(bits);
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);

  bitv->unitSet(0, bits, value, sim::UnitOnes);

  return 1;

}

// .fromStr(str : string) : bitv
int f_fromStr(lua_State * L)
{
  const char * str = luaL_checkstring(L, 1);

  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector();
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);

  bitv->fromStr(str);

  return 1;
}

// .fromHex(str : string) bitv
int f_fromHex(lua_State * L)
{
  const char * str = luaL_checkstring(L, 1);

  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector();
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);

  bitv->fromHex(str);

  return 1;
}

// .fromBin(str : string)
int f_fromBin(lua_State * L)
{
  const char * str = luaL_checkstring(L, 1);

  sim::BitVector * bitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (bitv) sim::BitVector();
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);

  bitv->fromBin(str);

  return 1;
}

int m_gc(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  bitv->~BitVector();
  return 0;
}

// :bits()
int m_bits(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  lua_pushinteger(L, bitv->bits());
  return 1;
}

// :get(pos : number) -> value : boolean, valid : boolean
// :get(pos : number, len : number) -> value : number, valid : boolean
int m_get(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  lua_Integer pos = luaL_checkinteger(L, 2);
  if (lua_isnone(L, 3)) {
    sim::Unit value, valid;
    bitv->unit(pos, 1, value, valid);
    lua_pushboolean(L, value == 1);
    lua_pushboolean(L, valid == 1);
    return 2;
  } else {
    lua_Integer len = luaL_checkinteger(L, 3);
    lua_pushinteger(L, bitv->unitValue(pos, len));
    lua_pushboolean(L, bitv->allValid(pos, len));
    return 2;
  }
}

// :slice(pos : number, len : number) -> value : bitv
int m_slice(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  lua_Integer pos = luaL_checkinteger(L, 2);
  lua_Integer len = luaL_checkinteger(L, 3);

  sim::BitVector * newBitv = (sim::BitVector *)lua_newuserdatauv(L, sizeof(sim::BitVector), 0);
  new (newBitv) sim::BitVector(len);
  luaL_getmetatable(L, t_bitv);
  lua_setmetatable(L, -2);

  newBitv->set(0, len, *bitv, pos);

  return 1;
}

// :valid(pos : number) : valid : boolean
// :valid(pos : number, len : number) : valid : number
int m_valid(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  lua_Integer pos = luaL_checkinteger(L, 2);

  if (lua_isnone(L, 3)) {
    lua_pushboolean(L, bitv->unitValid(pos, 0) == 1);
    return 1;
  } else {
    lua_Integer len = luaL_checkinteger(L, 3);
    lua_pushinteger(L, bitv->unitValid(pos, len));
    return 1;
  }
}

// :set(pos : number, value : bool, valid : bool | nil)
// :set(pos : number, len : number, value : bool, valid : boolean | nil)
// :set(pos : number, len : number, value : number, valid : number | nil)
// :set(pos : number, len : number, value : bitv, offset : number := 0)
int m_set(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  lua_Integer pos = luaL_checkinteger(L, 2);

  if (lua_isboolean(L, 3)) {
    sim::Unit value = lua_toboolean(L, 3)? 1 : 0;
    sim::Unit valid = lua_isnoneornil(L, 4)? 1 : lua_toboolean(L, 4)? 1 : 0;
    bitv->unitSet(pos, 1, value, valid);
  } else {
    lua_Integer len = luaL_checkinteger(L, 3);
    if (lua_isboolean(L, 4)){
      bool value = lua_toboolean(L, 4);
      bool valid = lua_isnoneornil(L, 5)? true : lua_toboolean(L, 5);
      bitv->fill(pos, len, value, valid);
    } else if (lua_isnumber(L, 4)) {
      lua_Integer value = lua_tointeger(L, 4);
      lua_Integer valid = lua_isboolean(L, 5)? (lua_toboolean(L, 5)? sim::UnitOnes : sim::UnitZeros) : luaL_optinteger(L, 5, sim::UnitOnes);
      bitv->unitSet(pos, len, value, valid);
    } else {
      sim::BitVector * other = (sim::BitVector *)luaL_checkudata(L, 4, t_bitv);
      lua_Integer off = luaL_optinteger(L, 5, 0);
      bitv->set(pos, len, *other, off);
    }
  }
  return 0;
}

// :append(value : bitv)
int m_append(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  sim::BitVector * other = (sim::BitVector *)luaL_checkudata(L, 2, t_bitv);
  bitv->append(*other);
  return 0;
}

// :resize(bits : number, value : boolean := false, valid : boolean := false)
int m_resize(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  lua_Integer pos = luaL_checkinteger(L, 2);
  bool value = luaL_opt(L, lua_toboolean, 3, false);
  bool valid = luaL_opt(L, lua_toboolean, 4, false);

  bitv->resize(pos, value, valid);
  return 0;
}

// :toStr() : string
int m_toStr(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  std::string str = bitv->toStr();
  lua_pushlstring(L, str.c_str(), str.size());
  return 1;
}

// :toHex() : string
int m_toHex(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  std::string str = bitv->toHex();
  lua_pushlstring(L, str.c_str(), str.size());
  return 1;
}

// :toBin() : string
int m_toBin(lua_State * L)
{
  sim::BitVector * bitv = (sim::BitVector *)luaL_checkudata(L, 1, t_bitv);
  std::string str = bitv->toBin();
  lua_pushlstring(L, str.c_str(), str.size());
  return 1;
}

} // namespace sim::lua::bitv
