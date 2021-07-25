#pragma once

#include <functional>
#include <queue>
#include <string>
#include <vector>

#include "../model.hpp"
#include "../paramallocator.hpp"
#include "../types.hpp"


namespace sim::model {


class System;


class SystemCondition
{
public:
  SystemCondition(System & system, bool state);
  ~SystemCondition();

  inline bool state() const;
  inline sim::Signal signal(bool state);
  void set(bool state);

private:
  System & m_system;
  sim::SignalParam m_id;
  bool m_state;
};


class System : public sim::Model
{
  static const SignalCode SigTimer = 0x1;
  static const SignalCode SigReset = 0x2;
  static const SignalCode SigStop  = 0x3;
  static const SignalCode SigEventAssert = 0x4;
  static const SignalCode SigEventRelease = 0x5;

  using TimerElement = std::pair<sim::Ticks, sim::SignalParam>;
  using TimerQueue = std::priority_queue<TimerElement, std::vector<TimerElement>, std::greater<TimerElement>>;
public:
  System(Environment & env, const std::string & name);
  virtual ~System() override;

  virtual void tick() override;

  inline sim::Ticks ticks() const;
  sim::Signal sigTimer(sim::Ticks at);


  void reset(bool assert);
  inline bool getReset() const;
  inline sim::Signal sigReset(bool asserted) const;

  void stop();
  inline void setIdleTimeout(sim::Ticks timeout);
  void clearIdle();
  inline bool getStop() const;
  inline sim::Signal sigStop() const;

protected:
  inline sim::SignalParam allocCondition();
  inline void freeCondition(sim::SignalParam id);
  inline sim::Signal sigCondition(sim::SignalParam id, bool state) const;
  inline void emitCondition(sim::SignalParam id, bool state);

  friend class SystemCondition;

private:
  sim::Ticks m_ticks;
  TimerQueue m_timers;
  sim::ParamAllocator m_timerIds;

  sim::ParamAllocator m_conditionIds;

  bool m_reset;

  bool m_stop;
  sim::Ticks m_idleTimeout;
  sim::Ticks m_idleTicks;
};

} // namespace sim


namespace sim::model {

inline bool SystemCondition::state() const
{
  return m_state;
}

inline sim::Signal SystemCondition::signal(bool state)
{
  return m_system.sigCondition(m_id, state);
}

inline sim::Ticks System::ticks() const
{
  return m_ticks;
}

inline bool System::getReset() const
{
  return m_reset;
}

inline Signal System::sigReset(bool asserted) const
{
  return signalFor(SigReset, (sim::SignalParam) asserted);
}

inline void System::setIdleTimeout(sim::Ticks timeout)
{
  m_idleTimeout = timeout;
  m_idleTicks = timeout;
}

inline bool System::getStop() const
{
  return m_stop;
}

inline sim::Signal System::sigStop() const
{
  return signalFor(SigStop);
}

inline sim::SignalParam System::allocCondition()
{
  return m_conditionIds.alloc();
}

inline void System::freeCondition(sim::SignalParam id)
{
  m_conditionIds.free(id);
}

inline sim::Signal System::sigCondition(sim::SignalParam id, bool state) const
{
  return signalFor(state? SigEventAssert : SigEventRelease, id);
}

inline void System::emitCondition(sim::SignalParam id, bool state)
{
  emit(state? SigEventAssert : SigEventRelease, id);
}


} // namespace sim

