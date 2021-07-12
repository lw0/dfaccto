#include "context.hpp"

#include <assert.h>
#include <iostream>
#include <iomanip>

#include "../environment.hpp"
#include "proxy.hpp"


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
  std::cout << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
  std::cout << " SIG " << signal << " > ";
  auto begin = m_waiting.lower_bound(signal);
  auto end = m_waiting.upper_bound(signal);
  for (auto it = begin; it != end; ++it) {
    std::cout << it->second << " ";
    ready().push_back(it->second);
  }
  std::cout << std::endl;
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
    std::cout << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
    std::cout << " RUN " << threadRef << std::endl;
    int code = lua_resume(T, m_state, 0, &nres);

    if (code == LUA_YIELD) {
      if (nres == 1 && lua_isinteger(T, -1)) {
        // Thread yields the Signal it is waiting for
        sim::Signal waitFor = lua_tointeger(T, -1);
        std::cout << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
        std::cout << " STP " << threadRef << " < " << waitFor << std::endl;
        m_waiting.insert({waitFor, threadRef});
      } else {
        std::cout << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
        std::cout << " STP " << threadRef << std::endl;
        // Thread yields nothing, immediately ready for next dispatch()
        ready().push_back(threadRef);
      }
      lua_pop(T, nres);
    } else if (code == LUA_OK) {
      std::cout << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
      std::cout << " END " << threadRef << std::endl;
      luaL_unref(T, LUA_REGISTRYINDEX, threadRef);
    } else {
      std::cout << "T" << std::setw(4) << std::setfill('0') << m_env.system().ticks() << std::setfill(' ') << std::setw(0);
      std::cout << "ERR " << threadRef << " " << lua_tostring(T, -1) << std::endl;
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
  std::cout << "Lua Error: " << lua_tostring(m_state, -1) << std::endl;
  std::cout << "Stopping simulation!" << std::endl;
  m_env.system().stop();
}

} // namespace sim::lua
