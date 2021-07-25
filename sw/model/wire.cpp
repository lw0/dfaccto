#include "wire.hpp"


namespace sim::model {

Wire::Wire(Environment & env, const std::string & name)
: Model(env, name)
, m_data {0}
, m_matchers {}
, m_matcherIds {}
{ }

Wire::~Wire()
{ }

void Wire::tick()
{ }

void Wire::dataFrom(const sim::vhdl::LogicArray * data)
{
  m_data.fromLogic(data); // also updates m_data.bits()
  if (m_data.changed(false)) {
    changed();
  }
}

void Wire::dataTo(sim::vhdl::LogicArray * data)
{
  m_data.resize(data->size());
  m_data.toLogic(data, sim::vhdl::Logic::VX);
}

sim::Signal Wire::sigMatch(const sim::BitVector & data)
{
  sim::SignalParam id = m_matcherIds.alloc();
  m_matchers.emplace_back(data, id);
  return signalFor(SigMatch, id);
}

void Wire::changed()
{
  emit(SigChanged);

  auto it = m_matchers.begin();
  while (it != m_matchers.end()) {
    if (std::get<0>(*it).equals(m_data)) {
      emit(SigMatch, std::get<1>(*it));
      m_matcherIds.free(std::get<1>(*it));
      it = m_matchers.erase(it);
    } else {
      ++it;
    }
  }
}


} // namespace sim::model

