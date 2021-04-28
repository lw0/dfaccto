#pragma once

#include <string>

#include "types.hpp"


namespace sim {

class Environment;

class Model
{
public:
  Model(Environment & env, const std::string & name);
  virtual ~Model();

  virtual void tick() = 0;

protected:
  inline sim::Signal signalFor(sim::SignalCode code, sim::SignalParam param = 0) const;
  void emit(sim::SignalCode code, sim::SignalParam param = 0);

private:
  Environment & m_env;
  sim::ModelRef m_ref;
};

} // namespace sim


namespace sim {

inline sim::Signal Model::signalFor(sim::SignalCode code, sim::SignalParam param) const
{
  return {m_ref, code, param};
}

} // namespace sim

