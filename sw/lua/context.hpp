#pragma once

#include <lua.hpp>
#include <map>
#include <vector>
#include <string>

#include "../types.hpp"


namespace sim {

class Environment;

}

namespace sim::lua {

class Context
{
public:
  Context(Environment & env);
  ~Context();

  void load(const std::string & script);

  void signal(sim::Signal signal);
  void dispatch();
  void thread(lua_Integer threadRef);

  inline Environment & env();

protected:
  void error();

  inline std::vector<lua_Integer> & ready();
  inline std::vector<lua_Integer> & readyLast();
  inline void swapReady();

private:
  Environment & m_env;

  lua_State * m_state;

  std::multimap<sim::Signal, lua_Integer> m_waiting;
  std::vector<lua_Integer> m_readyA;
  std::vector<lua_Integer> m_readyB;
  bool m_readySelect;
};

} // namespace sim::lua


namespace sim::lua {

inline Environment & Context::env()
{
  return m_env;
}

inline std::vector<lua_Integer> & Context::ready()
{
  return m_readySelect? m_readyB : m_readyA;
}

inline std::vector<lua_Integer> & Context::readyLast()
{
  return m_readySelect? m_readyA : m_readyB;
}

inline void Context::swapReady()
{
  m_readySelect = !m_readySelect;
}

}
