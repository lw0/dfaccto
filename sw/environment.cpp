#include "environment.hpp"

#include <iostream>
#include <cassert>


namespace sim {

Environment::Environment()
: m_registry { }
, m_system (*this, "<system>")
, m_lua { *this }
{ }

sim::ModelRef Environment::registerModel(const std::string & name, sim::Model * model)
{
  for (size_t idx = 0; idx < m_registry.size(); ++idx) {
    if (m_registry[idx].first == name) {
      if (model) {
        m_registry[idx].second = model;
      }
      return idx;
    }
  }
  assert(m_registry.size() < ModelRefMax);

  sim::ModelRef ref = m_registry.size();
  m_registry.emplace_back(name, model);
  return ref;
}

void Environment::forgetModel(sim::ModelRef ref)
{
  if (ref >= m_registry.size())
    return;

  m_registry[ref].second = nullptr;
}

void Environment::onSetup(const std::string & config, sim::Ticks idleTimeout)
{
  std::cout << "setup(" << config << ", " << idleTimeout << ")" << std::endl;
  m_system.setIdleTimeout(idleTimeout);
  m_lua.load(config);
}

void Environment::onTick()
{
  for (auto & entry : m_registry) {
    if (entry.second) {
      entry.second->tick();
    }
  }

  m_lua.dispatch();
}


void Environment::onShutdown()
{
  std::cout << "shutdown()" << std::endl;
  m_lua.dispatch();
}


void Environment::queueSignal(sim::Signal sig)
{
  m_lua.signal(sig);
}

} // namespace sim
