#pragma once

#include <string>

#include "../types.hpp"
#include "../model.hpp"


namespace sim::model {

class Event : public sim::Model
{
public:
  enum State : std::uint8_t {
    Idle =   0x00,
    Stb =    0x01,
    StbAck = 0x11,
    Ack =    0x10
  };

protected:
  static const sim::SignalCode SigEnter = 0x1;

public:
  Event(Environment & env, const std::string & name, bool autostb, bool autoack);
  virtual ~Event() override;

  void reset();
  virtual void tick() override;

  inline sim::Signal sigState(bool stb, bool ack);
  inline sim::Signal sigCanStb(bool assert);
  inline sim::Signal sigCanAck(bool assert);

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
  bool m_autostb;
  bool m_autoack;
  State m_state;
  sim::Unit m_stbData;
  size_t m_stbBits;
  sim::Unit m_ackData;
  size_t m_ackBits;
};

} // namespace sim::model


namespace sim::model {

inline sim::Signal Event::sigState(bool stb, bool ack)
{
  return signalFor(SigEnter, stb? (ack? StbAck : Stb) : (ack? Ack : Idle));
}

inline sim::Signal Event::sigCanStb(bool assert)
{
  return signalFor(SigEnter, assert? Idle : StbAck);
}

inline sim::Signal Event::sigCanAck(bool assert)
{
  return signalFor(SigEnter, assert? Stb : Ack);
}

inline bool Event::stbSet()
{
  return m_state & Stb;
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
  return m_state & Ack;
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

