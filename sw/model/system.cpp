#include "system.hpp"

#include <algorithm>
#include <cassert>

namespace sim::model {

SystemCondition::SystemCondition(System & system, bool state)
: m_system {system}
, m_id {m_system.allocCondition()}
, m_state {state}
{ }

SystemCondition::~SystemCondition()
{
  m_system.freeCondition(m_id);
}

void SystemCondition::set(bool state)
{
  if (m_state != state) {
    m_state = state;
    m_system.emitCondition(m_id, state);
  }
}

System::System(Environment & env, const std::string & name)
: Model(env, name)
, m_ticks {0}
, m_timers {}
, m_timerIds {}
, m_conditionIds {}
, m_reset {true}
, m_stop {false}
, m_idleTimeout {0}
, m_idleTicks {0}
{ }

System::~System()
{ }

void System::tick()
{
  ++m_ticks;

  while (!m_timers.empty() && m_timers.top().first <= m_ticks) {
    sim::Ticks at = m_timers.top().first;
    sim::SignalParam id = m_timers.top().second;
    m_timers.pop();
    m_timerIds.free(id);
    emit(SigTimer, id);
  }

  if (m_idleTicks > 0) {
    --m_idleTicks;
  } else if (m_idleTimeout != 0) {
    stop();
  }
}

Signal System::sigTimer(sim::Ticks at)
{
  sim::SignalParam id = m_timerIds.alloc();
  m_timers.emplace(at, id);
  return signalFor(SigTimer, id);
}

void System::reset(bool asserted)
{
  if (m_reset != asserted) {
    m_reset = asserted;
    emit(SigReset, (sim::SignalParam) asserted);
  }
}

void System::stop()
{
  m_stop = true;
  emit(SigStop);
}

void System::clearIdle()
{
  m_idleTicks = m_idleTimeout;
}


} // namespace sim::model

