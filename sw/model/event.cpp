#include "event.hpp"


namespace sim::model {

Event::Event(Environment & env, const std::string & name, bool swapRelease)
: Model(env, name)
, m_swapRelease {swapRelease}
, m_stbBits {0}
, m_ackBits {0}
, m_state {Idle}
, m_stbData {0}
, m_ackData {0}
{ }

Event::~Event()
{ }

void Event::reset()
{
  m_state = Idle;
  m_stbData = 0;
  m_ackData = 0;
}

void Event::tick()
{ }

bool Event::stb(bool assert, sim::Unit data)
{
  if (m_state != canStb(assert)) {
    return false;
  }

  if (assert) {
    m_stbData = data;
    m_state = StbAssert;
    emit(SigEnter, m_state);
  } else if (m_swapRelease) {
    m_state = Idle;
  } else {
    m_state = StbRelease;
  }
  emit(SigEnter, m_state);
  return true;
}

bool Event::ack(bool assert, sim::Unit data)
{
  if (m_state != canAck(assert)) {
    return false;
  }

  if (assert) {
    m_ackData = data;
    m_state = AckAssert;
  } else if (m_swapRelease) {
    m_state = AckRelease;
  } else {
    m_state = Idle;
  }
  emit(SigEnter, m_state);
  return true;
}

} // namespace sim

