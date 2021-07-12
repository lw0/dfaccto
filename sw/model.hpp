#pragma once

#include <string>

#include "types.hpp"


namespace sim {

class Environment;

class Model
{
public:
  Model(Environment & env, const std::string & primaryName);
  Model(Environment & env, const std::string & primaryName, const std::string & secondaryName);
  virtual ~Model();

  virtual void tick() = 0;

protected:
  inline sim::Signal signalFor(sim::SignalCode code, sim::SignalParam param = 0) const;
  void emit(sim::SignalCode code, sim::SignalParam param = 0);

private:
  Environment & m_env;
  bool m_secondary;
  sim::ModelRef m_primaryRef;
  sim::ModelRef m_secondaryRef;
};

} // namespace sim


namespace sim {

inline sim::Signal Model::signalFor(sim::SignalCode code, sim::SignalParam param) const
{
  return {m_primaryRef, code, param};
}

} // namespace sim

