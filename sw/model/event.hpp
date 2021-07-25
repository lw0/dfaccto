#pragma once

#include <string>

#include "../types.hpp"
#include "../model.hpp"
#include "../bitvector.hpp"
#include "../vhdl/logicarray.hpp"


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

  void stbFrom(sim::vhdl::Logic stb, const sim::vhdl::LogicArray * stbData);
  void stbTo(sim::vhdl::Logic * stb, sim::vhdl::LogicArray * stbData);
  void ackFrom(sim::vhdl::Logic ack, const sim::vhdl::LogicArray * ackData);
  void ackTo(sim::vhdl::Logic * ack, sim::vhdl::LogicArray * ackData);

  inline State state() const;
  inline sim::Signal sigState(State state) const;

  inline State canStb(bool assert) const;
  inline State canAck(bool assert) const;

  inline void stb(bool assert, const sim::BitVector & stbData);
  inline bool stbSet() const;
  inline const sim::BitVector & stbData() const;
  inline size_t stbBits() const;

  inline void ack(bool assert, const sim::BitVector & stbData);
  inline bool ackSet() const;
  inline const sim::BitVector & ackData() const;
  inline size_t ackBits() const;

protected:
  bool doStb(bool assert);
  bool doAck(bool assert);

private:
  bool m_swapRelease;

  State m_state;
  sim::BitVector m_stbData;
  sim::BitVector m_ackData;
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

inline void Event::stb(bool assert, const sim::BitVector & stbData)
{
  if (doStb(assert)) {
    m_stbData.set(0, m_stbData.bits(), stbData);
  }
}

inline bool Event::stbSet() const
{
  return m_state & StbState;
}

inline const sim::BitVector & Event::stbData() const
{
  return m_stbData;
}

inline size_t Event::stbBits() const
{
  return m_stbData.bits();
}

inline void Event::ack(bool assert, const sim::BitVector & ackData)
{
  if (doAck(assert)) {
    m_ackData.set(0, m_ackData.bits(), ackData);
  }
}

inline bool Event::ackSet() const
{
  return m_state & AckState;
}

inline const sim::BitVector & Event::ackData() const
{
  return m_ackData;
}

inline size_t Event::ackBits() const
{
  return m_ackData.bits();
}

} // namespace sim::model

