#pragma once

#include <lua.hpp>

#include "context.hpp"


namespace sim::lua {

void open_dfaccto(lua_State * L, Context * context);

void open_wire(lua_State * L);
void new_wire(lua_State * L, Context * context);

void open_event(lua_State * L);
void new_event(lua_State * L, Context * context);

// bool new_stream(lua_State * L, Context * context);
// bool new_memory(lua_State * L, Context * context);
// bool new_master(lua_State * L, Context * context);
// bool new_slave(lua_State * L, Context * context);
// bool new_raw_stream(lua_State * L, Context * context);
// bool new_raw_master(lua_State * L, Context * context);
// bool new_raw_slave(lua_State * L, Context * context);

} // namespace sim::lua
