#pragma once

#include <string>
#include <utility>
#include <vector>

#include "types.hpp"
#include "model.hpp"
#include "model/system.hpp"
#include "lua/context.hpp"

namespace sim {

class Environment
{
public:
  Environment();

  sim::ModelRef registerModel(const std::string & name, sim::Model * model = nullptr);
  void forgetModel(sim::ModelRef id);

  template <typename T>
  T * model(sim::ModelRef ref);

  inline model::System & system();

  void queueSignal(sim::Signal sig);

  void reset();

  void onSetup(const std::string & config, sim::Ticks idleTimeout = 0);
  void onTick();
  void onShutdown();

private:
  std::vector<std::pair<std::string, Model *>> m_registry;
  model::System m_system;

  lua::Context m_lua;
};

} // namespace sim


namespace sim {

template <typename T>
inline T * Environment::model(sim::ModelRef ref)
{
  if (ref >= m_registry.size())
    return nullptr;

  return dynamic_cast<T*>(m_registry[ref].second);
}

inline model::System & Environment::system()
{
  return m_system;
}

} // namespace sim
