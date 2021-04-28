#include "wire.hpp"


namespace sim::model {

Wire::Wire(Environment & env, const std::string & name)
: Model(env, name)
, m_data {0}
, m_dataBits {0}
, m_matchers {}
, m_matcherIds {}
{ }

Wire::~Wire()
{ }

void Wire::tick()
{ }

void Wire::data(sim::Unit data)
{
  if (m_data != data) {
    m_data = data;

    emit(SigChanged);

    auto begin = m_matchers.lower_bound(data);
    auto end = m_matchers.upper_bound(data);
    for (auto it = begin; it != end; ++it) {
      emit(SigMatch, it->second);
    }
    m_matchers.erase(begin, end);
  }
}

sim::Signal Wire::sigMatch(sim::Unit data)
{
  sim::SignalParam id = m_matcherIds.alloc();
  m_matchers.insert({data, id});
  return signalFor(SigMatch, id);
}

} // namespace sim::model

