#include "system.hpp"

#include <algorithm>
#include <cassert>

namespace sim::model {

System::System(Environment & env, const std::string & name)
: Model(env, name)
, m_ticks {0}
, m_timers {}
, m_timerIds {}
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

  while (m_timers.size() && m_timers.front().first <= m_ticks) {
    sim::SignalParam id = m_timers.front().second;
    std::pop_heap(m_timers.begin(), m_timers.end());
    m_timers.pop_back();
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
  m_timers.emplace_back(at, id);
  std::push_heap(m_timers.begin(), m_timers.end());
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

