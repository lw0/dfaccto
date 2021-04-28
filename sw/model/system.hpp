#pragma once

#include <string>
#include <vector>

#include "../model.hpp"
#include "../paramallocator.hpp"
#include "../types.hpp"


namespace sim::model {

class System : public sim::Model
{
  static const SignalCode SigTimer = 0x1;
  static const SignalCode SigReset = 0x2;
  static const SignalCode SigStop  = 0x3;

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

private:
  sim::Ticks m_ticks;
  std::vector<std::pair<sim::Ticks, sim::SignalParam>> m_timers;
  sim::ParamAllocator m_timerIds;

  bool m_reset;

  bool m_stop;
  sim::Ticks m_idleTimeout;
  sim::Ticks m_idleTicks;
};

} // namespace sim


namespace sim::model {

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

} // namespace sim

