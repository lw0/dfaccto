#include "model.hpp"

#include "environment.hpp"


namespace sim {

Model::Model(Environment & env, const std::string & primaryName)
: m_env {env}
, m_secondary {false}
, m_primaryRef {env.registerModel(primaryName, this)}
, m_secondaryRef {0}
{ }

Model::Model(Environment & env, const std::string & primaryName, const std::string & secondaryName)
: m_env {env}
, m_secondary {true}
, m_primaryRef {env.registerModel(primaryName, this)}
, m_secondaryRef {env.registerModel(secondaryName, this)}
{ }

Model::~Model()
{
  m_env.forgetModel(m_primaryRef);
  if (m_secondary) {
    m_env.forgetModel(m_primaryRef);
  }
}

void Model::emit(sim::SignalCode code, sim::SignalParam param) {
  m_env.queueSignal(signalFor(code, param));
}

} // namespace sim

