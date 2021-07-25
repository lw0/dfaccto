#include "proxy.hpp"

#include "../environment.hpp"
#include "../model/stream.hpp"

/* port("StreamSource", {
           depth : integer := 0,
           bandwidth : number := 1.0,
           latency : number := 0.0,
           fair : bool := false,
           packed : bool := false,
           seed : integer := 0 })
 *   reset(seed : integer := 0)
 *   dataBytes() : integer
 *   maxId() : integer
 *   write(id : integer, data : bitv, last : bool := false, mask : string)
 *   flush(id : integer) !blocking
 */

/* port("StreamSink", {
           depth : integer := 0,
           bandwidth : number := 1.0,
           latency : number := 0.0,
           fair : bool := false,
           packed : bool := false,
           seed : integer := 0 })
 *   reset(seed : integer := 0)
 *   dataBytes() : integer
 *   maxId() : integer
 *   tryRead(id : integer, what : number or "packet") : integer, string, bool, string
 *   read(id : integer, what : number or "packet") : integer, string, bool, string !blocking
 */

/* pass("StreamPass", {
           depth : integer := 0,
           bandwidthSink : number := 1.0,
           bandwidthSource : number := 1.0,
           latency : number := 0.0,
           fair : bool := false,
           packed : bool := false })
 *   reset(seed : integer := 0)
 *   dataBytes() : integer
 *   maxId() : integer
 */

namespace sim::lua::stream {

int f_newPass(lua_State * L);
int m__gc(lua_State *);
int m_reset(lua_State *);
int m_dataBytes(lua_State *);
int m_maxId(lua_State *);

int f_newSource(lua_State * L);
int m_write(lua_State *);
int m_flush(lua_State *);

int f_newSink(lua_State * L);
int m_read(lua_State *);


const char * n_streamSource = "StreamSource";
const char * t_streamSource = "dfaccto.stream_source";
const char * n_streamSink = "StreamSink";
const char * t_streamSink = "dfaccto.stream_sink";
const char * n_streamPass = "StreamPass";
const char * t_streamPass = "dfaccto.stream_pass";

const luaL_Reg m_streamPass[] = {
 {"__gc",      m__gc},
 {"reset",     m_reset},
 {"dataBytes", m_dataBytes},
 {"maxId",     m_maxId},
 {nullptr,     nullptr}};

const luaL_Reg m_streamSource[] = {
 {"__gc",      m__gc},
 {"reset",     m_reset},
 {"dataBytes", m_dataBytes},
 {"maxId",     m_maxId},
 {"write",     m_write},
 {"flush",     m_flush},
 {nullptr,     nullptr}};

const luaL_Reg m_streamSink[] = {
 {"__gc",      m__gc},
 {"reset",     m_reset},
 {"dataBytes", m_dataBytes},
 {"maxId",     m_maxId},
 {"read",      m_read},
 {nullptr,     nullptr}};

void open(lua_State * L, Context * context)
{
  // setup metatables
  luaL_newmetatable(L, t_streamSource);
  luaL_setfuncs(L, m_streamSource, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newmetatable(L, t_streamSink);
  luaL_setfuncs(L, m_streamSink, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newmetatable(L, t_streamPass);
  luaL_setfuncs(L, m_streamPass, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // register constructors
  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPort);
  lua_pushlightuserdata(L, context);
  lua_pushcclosure(L, f_newSource, 1);
  lua_setfield(L, -2, n_streamSource);
  lua_pop(L, 1);

  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPort);
  lua_pushlightuserdata(L, context);
  lua_pushcclosure(L, f_newSink, 1);
  lua_setfield(L, -2, n_streamSink);
  lua_pop(L, 1);

  lua_rawgetp(L, LUA_REGISTRYINDEX, RegKeyPass);
  lua_pushlightuserdata(L, context);
  lua_pushcclosure(L, f_newPass, 1);
  lua_setfield(L, -2, n_streamPass);
  lua_pop(L, 1);
}

// newPass(priName : string, secName : string, params : table | nil)
int f_newPass(lua_State * L)
{
  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));
  const char * priName = lua_tostring(L, 1);
  const char * secName = lua_tostring(L, 2);

  if (lua_isnoneornil(L, 3)) {
    lua_newtable(L);
    lua_replace(L, 3);
  }
  lua_getfield(L, 3, "depth");
  lua_Integer depth = luaL_optinteger(L, -1, 0);
  lua_getfield(L, 3, "bandwidthSink");
  lua_Number bandwidthSink = luaL_optnumber(L, -1, 1.f);
  lua_getfield(L, 3, "bandwidthSource");
  lua_Number bandwidthSource = luaL_optnumber(L, -1, 1.f);
  lua_getfield(L, 3, "latency");
  lua_Number latency = luaL_optnumber(L, -1, 0.f);
  lua_getfield(L, 3, "fair");
  bool fair = lua_toboolean(L, -1);
  lua_getfield(L, 3, "packed");
  bool packed = lua_toboolean(L, -1);
  lua_getfield(L, 3, "noId");
  bool noId = lua_toboolean(L, -1);
  lua_getfield(L, 3, "seed");
  lua_Integer seed = luaL_optinteger(L, -1, 0);
  lua_pop(L, 8);

  sim::model::Stream * model = (sim::model::Stream *)lua_newuserdata(L, sizeof(sim::model::Stream));
  new (model) sim::model::Stream(context->env(), priName, secName,
                                 depth,
                                 bandwidthSink,
                                 bandwidthSource,
                                 latency,
                                 fair,
                                 packed,
                                 noId,
                                 seed);
  luaL_getmetatable(L, t_streamPass);
  lua_setmetatable(L, -2);
  return 1;
}

// newSource(name : string, params : table | nil)
int f_newSource(lua_State * L)
{
  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));
  const char * name = lua_tostring(L, 1);

  if (lua_isnoneornil(L, 2)) {
    lua_newtable(L);
    lua_replace(L, 2);
  }
  lua_getfield(L, 2, "depth");
  lua_Integer depth = luaL_optinteger(L, -1, 0);
  lua_getfield(L, 2, "latency");
  lua_Number latency = luaL_optnumber(L, -1, 0.f);
  lua_getfield(L, 2, "bandwidth");
  lua_Number bandwidth = luaL_optnumber(L, -1, 1.f);
  lua_getfield(L, 2, "fair");
  bool fair = lua_toboolean(L, -1);
  lua_getfield(L, 2, "packed");
  bool packed = lua_toboolean(L, -1);
  lua_getfield(L, 2, "seed");
  lua_Integer seed = luaL_optinteger(L, -1, 0);
  lua_pop(L, 6);

  sim::model::StreamSource * model = (sim::model::StreamSource *)lua_newuserdata(L, sizeof(sim::model::StreamSource));
  new (model) sim::model::StreamSource(context->env(), name,
                                       depth,
                                       bandwidth,
                                       latency,
                                       fair,
                                       packed,
                                       seed);
  luaL_getmetatable(L, t_streamSource);
  lua_setmetatable(L, -2);
  return 1;
}

// newSink(name : string, params : table | nil)
int f_newSink(lua_State * L)
{
  Context * context = (Context *)lua_touserdata(L, lua_upvalueindex(1));
  const char * name = lua_tostring(L, 1);

  if (lua_isnoneornil(L, 2)) {
    lua_newtable(L);
    lua_replace(L, 2);
  }
  lua_getfield(L, 2, "depth");
  lua_Integer depth = luaL_optinteger(L, -1, 0);
  lua_getfield(L, 2, "latency");
  lua_Number latency = luaL_optnumber(L, -1, 0.f);
  lua_getfield(L, 2, "bandwidth");
  lua_Number bandwidth = luaL_optnumber(L, -1, 1.f);
  lua_getfield(L, 2, "fair");
  bool fair = lua_toboolean(L, -1);
  lua_getfield(L, 2, "packed");
  bool packed = lua_toboolean(L, -1);
  lua_getfield(L, 2, "seed");
  lua_Integer seed = luaL_optinteger(L, -1, 0);
  lua_pop(L, 6);

  sim::model::StreamSink * model = (sim::model::StreamSink *)lua_newuserdata(L, sizeof(sim::model::StreamSink));
  new (model) sim::model::StreamSink(context->env(), name,
                                     depth,
                                     bandwidth,
                                     latency,
                                     fair,
                                     packed,
                                     seed);
  luaL_getmetatable(L, t_streamSink);
  lua_setmetatable(L, -2);
  return 1;
}

sim::model::Stream * getStream(lua_State * L, int pos)
{
  lua_getmetatable(L, pos);
  luaL_getmetatable(L, t_streamPass);
  if (lua_rawequal(L, -1, -2)) {
    lua_pop(L, 2);
    return (sim::model::Stream *) lua_touserdata(L, pos);
  }
  lua_pop(L, 1);
  luaL_getmetatable(L, t_streamSink);
  if (lua_rawequal(L, -1, -2)) {
    lua_pop(L, 2);
    return (sim::model::Stream *) lua_touserdata(L, pos);
  }
  lua_pop(L, 1);
  luaL_getmetatable(L, t_streamSource);
  if (lua_rawequal(L, -1, -2)) {
    lua_pop(L, 2);
    return (sim::model::Stream *) lua_touserdata(L, pos);
  }
  lua_pop(L, 2);
  lua_pushfstring(L, "Argument #%d must be of type dfaccto.stream_*", pos);
  lua_error(L);
  return nullptr;
}

// :__gc()
int m__gc(lua_State * L)
{
  sim::model::Stream * model = getStream(L, 1);
  model->~Stream();
  return 0;
}

// :reset(seed : integer := 0)
int m_reset(lua_State * L)
{
  sim::model::Stream * model = getStream(L, 1);
  lua_Integer seed = luaL_optinteger(L, 2, 0);
  model->reset(seed);
  return 0;
}

// :dataBytes() : integer
int m_dataBytes(lua_State * L)
{
  sim::model::Stream * model = getStream(L, 1);
  lua_pushinteger(L, model->dataBytes());
  return 1;
}

// :maxId() : integer
int m_maxId(lua_State * L)
{
  sim::model::Stream * model = getStream(L, 1);
  lua_pushinteger(L, model->maxId());
  return 1;
}

int m_write_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::StreamSource * model = (sim::model::StreamSource *)luaL_checkudata(L, 1, t_streamSource);

  sim::Id id = luaL_checkinteger(L, 2);
  sim::BitVector * data = sim::lua::bitv::getBitVector(L, 3, model->dataBits());
  bool last = lua_toboolean(L, 4);

  size_t writePos = lua_tointeger(L, 5);

  if (model->write(id, *data, last, writePos)) {
    return 0;
  } else {
    lua_pushinteger(L, writePos);
    lua_replace(L, 5);
    lua_pushinteger(L, model->sigFree());
    return lua_yieldk(L, 1, 0, m_write_k);
  }
}

// :write(id : integer, data : bitv | integer, last : bool := false) !blocking
int m_write(lua_State * L)
{
  while (lua_gettop(L) < 4) {
    lua_pushnil(L);
  }
  lua_pushinteger(L, 0); // push write position marker
  return m_write_k(L, 0, 0);
}

// :flush(id : integer) !blocking
int m_flush(lua_State * L)
{
  sim::model::StreamSource * model = (sim::model::StreamSource *)luaL_checkudata(L, 1, t_streamSource);

  sim::Id id = luaL_checkinteger(L, 2);
  lua_pushinteger(L, model->sigEmpty(id));
  return lua_yield(L, 1);
}

int m_read_k(lua_State * L, int status, lua_KContext ctx)
{
  sim::model::StreamSink * model = (sim::model::StreamSink *)luaL_checkudata(L, 1, t_streamSink);
  sim::Id id = luaL_checkinteger(L, 2);
  size_t maxBeats = luaL_optinteger(L, 3, 0);
  size_t curBeats = lua_tointeger(L, 4);
  sim::BitVector * data = sim::lua::bitv::getBitVector(L, 5, 0);

  bool last = false;
  size_t remBeats = (maxBeats > 0)? maxBeats - curBeats : 0;
  curBeats += model->read(id, *data, last, remBeats);
  lua_pushinteger(L, curBeats);
  lua_replace(L, 4);

  if (last || (maxBeats > 0 && curBeats >= maxBeats)) {
    lua_pushboolean(L, last);
    lua_pushinteger(L, id);
    return 3;
  } else {
    lua_pushinteger(L, model->sigAvailable(id));
    return lua_yieldk(L, 1, 0, m_read_k);
  }
}

// :read(id : integer, maxBeats : integer | nil) : data : bitv, last : bool, id : integer
int m_read(lua_State * L)
{
  while (lua_gettop(L) < 3) {
    lua_pushnil(L);
  }
  lua_pushinteger(L, 0); // push read count
  sim::lua::bitv::pushBitVector(L); // push read data
  return m_read_k(L, 0, 0);
}


} // namespace sim::lua
