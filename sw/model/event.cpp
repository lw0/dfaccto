#include "event.hpp"


namespace sim::model {

Event::Event(Environment & env, const std::string & name, bool swapRelease)
: Model(env, name)
, m_swapRelease {swapRelease}
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

void Event::stbFrom(sim::vhdl::Logic stb, const sim::vhdl::LogicArray * stbData)
{
  m_stbData.resize(stbData->size());
  if (doStb(stb.isSet())) {
    m_stbData.fromLogic(stbData);
  }
}

void Event::stbTo(sim::vhdl::Logic * stb, sim::vhdl::LogicArray * stbData)
{
  m_stbData.resize(stbData->size());
  stb->set(stbSet());
  m_stbData.toLogic(stbData, sim::vhdl::Logic::VX);
}

void Event::ackFrom(sim::vhdl::Logic ack, const sim::vhdl::LogicArray * ackData)
{
  m_ackData.resize(ackData->size());
  if (doAck(ack.isSet())) {
    m_ackData.fromLogic(ackData);
  }
}

void Event::ackTo(sim::vhdl::Logic * ack, sim::vhdl::LogicArray * ackData)
{
  m_ackData.resize(ackData->size());
  ack->set(ackSet());
  m_ackData.toLogic(ackData, sim::vhdl::Logic::VX);
}

bool Event::doStb(bool assert)
{
  bool setData = false;
  if (m_state != canStb(assert)) {
    return false;
  }

  if (assert) {
    setData = true;
    m_state = StbAssert;
    emit(SigEnter, m_state);
  } else if (m_swapRelease) {
    m_state = Idle;
  } else {
    m_state = StbRelease;
  }
  emit(SigEnter, m_state);
  return setData;
}

bool Event::doAck(bool assert)
{
  bool setData = false;
  if (m_state != canAck(assert)) {
    return false;
  }

  if (assert) {
    setData = false;
    m_state = AckAssert;
  } else if (m_swapRelease) {
    m_state = AckRelease;
  } else {
    m_state = Idle;
  }
  emit(SigEnter, m_state);
  return setData;
}

} // namespace sim

