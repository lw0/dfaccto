#include "proxy.hpp"

#include "context.hpp"


namespace sim::lua {

static const char RegKeyPort_Location = 'P';
const void * RegKeyPort = (void *)&RegKeyPort_Location;

static const char RegKeyPass_Location = 'L';
const void * RegKeyPass = (void *)&RegKeyPass_Location;

static const char RegKeyCache_Location = 'C';
const void * RegKeyCache = (void *)&RegKeyCache_Location;

void open(lua_State * L, Context * context)
{
  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, RegKeyPort);

  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, RegKeyPass);

  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, RegKeyCache);

  bitv::open(L);
  lua_setglobal(L, "bitv");

  sys::open(L, context);
  lua_setglobal(L, "sys");

  wire::open(L, context);

  event::open(L, context);

  stream::open(L, context);
}

} // namespace sim::lua


