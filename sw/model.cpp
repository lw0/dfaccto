#include "model.hpp"

#include "environment.hpp"


namespace sim {

Model::Model(Environment & env, const std::string & name)
: m_env {env}
, m_ref {env.registerModel(name, this)}
{ }

Model::~Model()
{
  m_env.forgetModel(m_ref);
}

void Model::emit(sim::SignalCode code, sim::SignalParam param) {
  m_env.queueSignal(signalFor(code, param));
}

} // namespace sim

