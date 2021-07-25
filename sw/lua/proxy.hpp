#pragma once

#include <iostream>

#include <lua.hpp>

#include "../bitvector.hpp"


namespace sim::lua {

class Context;

extern const void * RegKeyPort;
extern const void * RegKeyPass;
extern const void * RegKeyCache;


void open(lua_State * L, Context * context);

namespace bitv {
  void open(lua_State * L);
  sim::BitVector * pushBitVector(lua_State * L, size_t bits = 0, bool value = false, bool valid = false);
  sim::BitVector * pushBitVector(lua_State * L, const sim::BitVector & other);
  sim::BitVector * pushBitVector(lua_State * L, sim::BitVector && other);
  sim::BitVector * getBitVector(lua_State * L, int pos, size_t defaultBits);
}

namespace sys {
  void open(lua_State * L, Context * ctx);
}

namespace wire {
  void open(lua_State * L, Context * ctx);
}

namespace event {
  void open(lua_State * L, Context * ctx);
}

namespace stream {
  void open(lua_State * L, Context * ctx);
}

// void open_stream(lua_State * L);
// void new_stream_source(lua_State * L, Context * context);
// void new_stream_sink(lua_State * L, Context * context);
// void new_stream_pass(lua_State * L, Context * context);

// bool new_stream(lua_State * L, Context * context);
// bool new_memory(lua_State * L, Context * context);
// bool new_master(lua_State * L, Context * context);
// bool new_slave(lua_State * L, Context * context);
// bool new_raw_stream(lua_State * L, Context * context);
// bool new_raw_master(lua_State * L, Context * context);
// bool new_raw_slave(lua_State * L, Context * context);

inline void dumpstack(lua_State * L, const char * position)
{
  const int count = lua_gettop(L);
  std::clog << position << " #" << count << " > ";
  for (int i = 1; i <= count; ++i) {
    std::clog << luaL_typename(L, i) << " | ";
  }
  std::clog << std::endl;
}


} // namespace sim::lua
