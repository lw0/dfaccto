#include "context.hpp"

#include <assert.h>
#include <iostream>
#include <iomanip>

#include "../environment.hpp"
#include "proxy.hpp"

#define NOMSG

namespace sim::lua {

Context::Context(Environment & env)
: m_env {env}
, m_state {nullptr}
, m_waiting { }
, m_readyA { }
, m_readyB { }
, m_readySelect {false}
{
  m_state = luaL_newstate();
  assert(m_state);

  luaL_openlibs(m_state);

  open(m_state, this);
}

Context::~Context()
{
  if (m_state) {
    lua_close(m_state);
    m_state = nullptr;
  }
}

void Context::load(const std::string & script)
{
  if (luaL_dofile(m_state, script.c_str())) {
    error();
  }
}

void Context::signal(sim::Signal signal)
{
#ifndef NOMSG
  std::clog << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
  std::clog << " SIG " << signal << " > ";
#endif
  auto begin = m_waiting.lower_bound(signal);
  auto end = m_waiting.upper_bound(signal);
  for (auto it = begin; it != end; ++it) {
#ifndef NOMSG
    std::clog << it->second << " ";
#endif
    ready().push_back(it->second);
  }
#ifndef NOMSG
  std::clog << std::endl;
#endif
  m_waiting.erase(begin, end);
}

void Context::dispatch()
{
  if (ready().size()){
    m_env.system().clearIdle();
  }

  swapReady();
  for (lua_Integer threadRef : readyLast()) {
    lua_rawgeti(m_state, LUA_REGISTRYINDEX, threadRef);
    lua_State * T = lua_tothread(m_state, -1);

    int nres;
#ifndef NOMSG
    std::clog << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
    std::clog << " RUN " << threadRef << std::endl;
#endif
    int code = lua_resume(T, m_state, 0, &nres);

    if (code == LUA_YIELD) {
      if (nres == 1 && lua_isinteger(T, -1)) {
        // Thread yields the Signal it is waiting for
        sim::Signal waitFor = lua_tointeger(T, -1);
#ifndef NOMSG
        std::clog << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
        std::clog << " STP " << threadRef << " < " << waitFor << std::endl;
#endif
        m_waiting.insert({waitFor, threadRef});
      } else {
#ifndef NOMSG
        std::clog << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
        std::clog << " STP " << threadRef << std::endl;
#endif
        // Thread yields nothing, immediately ready for next dispatch()
        ready().push_back(threadRef);
      }
      lua_pop(T, nres);
    } else if (code == LUA_OK) {
#ifndef NOMSG
      std::clog << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
      std::clog << " END " << threadRef << std::endl;
#endif
      luaL_unref(T, LUA_REGISTRYINDEX, threadRef);
    } else {
      std::clog << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
      std::clog << " ERR " << threadRef << " " << lua_tostring(T, -1) << std::endl;
      luaL_unref(T, LUA_REGISTRYINDEX, threadRef);
    }

    lua_pop(m_state, 1);
  }
  readyLast().clear();
}

void Context::thread(lua_Integer threadRef)
{
  ready().push_back(threadRef);
}

void Context::error()
{
  std::clog << "Lua Error: " << lua_tostring(m_state, -1) << std::endl;
  std::clog << "Stopping simulation!" << std::endl;
  m_env.system().stop();
}

} // namespace sim::lua
