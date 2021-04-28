#pragma once

#include "../types.hpp"
#include "../bitvector.hpp"
#include "../model.hpp"
#include "../channel.hpp"


namespace sim::model {

class StreamConfig
{
public:
  virtual size_t tSize() const = 0;
  virtual float tPush() const = 0;
  virtual float tTake() const = 0;
  virtual float tLat() const = 0;
  virtual ChannelFlags tFlag() const = 0;
};

struct StreamTBeat
{
  sim::BitVector data; // encodes tdata and tkeep
  sim::Id id;
  bool last;

  inline StreamTBeat(size_t bits, sim::Id id = 0, bool last = false);
};

class Stream : public Model
{
public:
  Stream(Environment & env, const std::string & name, const StreamConfig & cfg);

  virtual void tick() override;

  void reset(sim::RndSeed seed = 0);

  inline sim::Channel<StreamTBeat> & tch();

  inline void dataBits(size_t bits);
  inline size_t dataBits() const;

  inline void idBits(size_t bits);
  inline size_t idBits() const;

private:
  sim::Channel<StreamTBeat> m_tChannel;

  size_t m_dataBits;
  size_t m_idBits;
};

} // namespace sim::model


namespace sim::model {

inline StreamTBeat::StreamTBeat(size_t bits, sim::Id id, bool last)
: data (bits)
, id {id}
, last {last}
{ }

inline sim::Channel<StreamTBeat> & Stream::tch()
{
  return m_tChannel;
}

inline void Stream::dataBits(size_t bits)
{
  // TODO-lw detect changes
  m_dataBits = bits;
}

inline size_t Stream::dataBits() const
{
  return m_dataBits;
}

inline void Stream::idBits(size_t bits)
{
  m_idBits = bits;
}

inline size_t Stream::idBits() const
{
  return m_idBits;
}

} // namespace sim:model
