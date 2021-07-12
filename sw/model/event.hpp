#pragma once

#include <string>

#include "../types.hpp"
#include "../model.hpp"


namespace sim::model {

class Event : public sim::Model
{
public:
  enum State : std::uint8_t {
    // Bit Masks:
    StbState   = 0x10,
    StbChange  = 0x20,
    AckState   = 0x40,
    AckChange  = 0x80,
    Index      = 0x0f,
    // Actual States:
    Idle       =                                               0,
    StbAssert  =                        StbChange | StbState | 1,
    AckAssert  = AckChange | AckState |             StbState | 2,
    StbRelease =             AckState | StbChange            | 3,
    AckRelease = AckChange |                        StbState | 4,
    /* State graph:
     * +----------->Idle<----------+
     * |             | stb+        |
     * |             v             |
     * |         StbAssert         |
     * |             | ack+        |
     * |             v             |
     * |         AckAssert         |
     * |  !swap & |     | swap &   |
     * |     stb- |     | ack-     |
     * |          v     v          |
     * |  StbRelease   AckRelease  |
     * |  !swap & |     | swap &   |
     * |     ack- |     | stb-     |
     * +----------+     +----------+
     */
  };

protected:
  static const sim::SignalCode SigEnter = 0x1;

public:
  Event(Environment & env, const std::string & name, bool swapRelease);
  virtual ~Event() override;

  void reset();
  virtual void tick() override;

  inline State state() const;
  inline sim::Signal sigState(State state) const;

  inline State canStb(bool assert) const;
  inline State canAck(bool assert) const;

  bool stb(bool assert, sim::Unit data = 0);
  inline bool stbSet();
  inline sim::Unit stbData();
  inline size_t stbBits();
  inline void stbBits(size_t bits);

  bool ack(bool assert, sim::Unit data = 0);
  inline bool ackSet();
  inline sim::Unit ackData();
  inline size_t ackBits();
  inline void ackBits(size_t bits);

private:
  bool m_swapRelease;
  size_t m_stbBits;
  size_t m_ackBits;

  State m_state;
  sim::Unit m_stbData;
  sim::Unit m_ackData;
};

} // namespace sim::model


namespace sim::model {

inline Event::State Event::state() const
{
  return m_state;
}

inline sim::Signal Event::sigState(State state) const
{
  return signalFor(SigEnter, state);
}

inline Event::State Event::canStb(bool assert) const
{
  if (assert) {
    return Idle;
  } else {
    return m_swapRelease? AckRelease : AckAssert;
  }
}

inline Event::State Event::canAck(bool assert) const
{
  if (assert) {
    return StbAssert;
  } else {
    return m_swapRelease? AckAssert : StbRelease;
  }
}

inline bool Event::stbSet()
{
  return m_state & StbState;
}

inline sim::Unit Event::stbData()
{
  return m_stbData;
}

inline size_t Event::stbBits()
{
  return m_stbBits;
}

inline void Event::stbBits(size_t bits)
{
  m_stbBits = bits;
}

inline bool Event::ackSet()
{
  return m_state & AckState;
}

inline sim::Unit Event::ackData()
{
  return m_ackData;
}

inline size_t Event::ackBits()
{
  return m_ackBits;
}

inline void Event::ackBits(size_t bits)
{
  m_ackBits = bits;
}

} // namespace sim::model

