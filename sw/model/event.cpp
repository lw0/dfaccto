#include "event.hpp"


namespace sim::model {

Event::Event(Environment & env, const std::string & name, bool autostb, bool autoack)
: Model(env, name)
, m_autostb {autostb}
, m_autoack {autoack}
, m_state {Idle}
, m_stbData {0}
, m_stbBits {0}
, m_ackData {0}
, m_ackBits {0}
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
  if (assert) {
    if (m_state != Idle)
      return false;

    m_stbData = data;
    m_state = Stb;
    emit(SigEnter, Stb);
    return true;

  } else {
    if (m_state != StbAck)
      return false;

    m_state = Ack;
    emit(SigEnter, Ack);
    if (m_autoack) {
      m_state = Idle;
      emit(SigEnter, Idle);
    }
    return true;
  }
}

bool Event::ack(bool assert, sim::Unit data)
{
  if (assert) {
    if (m_state != Stb)
      return false;

    m_ackData = data;
    m_state = StbAck;
    emit(SigEnter, StbAck);
    if (m_autostb) {
      m_state = Ack;
      emit(SigEnter, Ack);
    }
    return true;

  } else {
    if (m_state != Ack)
      return false;

    m_state = Idle;
    emit(SigEnter, Idle);
    return true;
  }
}

} // namespace sim

