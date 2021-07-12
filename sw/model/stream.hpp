#pragma once

#include "../types.hpp"
#include "../bitvector.hpp"
#include "../model.hpp"
#include "../channel.hpp"


namespace sim::model {

struct StreamTBeat
{
  sim::BitVector data; // encodes tdata and tkeep
  sim::Id id;
  bool last;

  inline StreamTBeat(size_t bits, sim::Id id = 0, bool last = false);
};

class Stream : public Model
{

protected:
  Stream(Environment &       env,
         const std::string & name,
         size_t              depth,
         float               sinkBandwidth,
         float               sourceBandwidth,
         float               latency,
         ChannelFlags        flags,
         sim::RndSeed        seed);

public:
  Stream(Environment &       env,
         const std::string & primaryName,
         const std::string & secondaryName,
         size_t              depth,
         float               sinkBandwidth,
         float               latency,
         float               sourceBandwidth,
         bool                sourceFair,
         bool                sourcePacked,
         sim::RndSeed        seed);

  virtual void tick() override;
  virtual void reset(sim::RndSeed seed = 0);

  inline bool tFree() const;
  inline bool tPush(StreamTBeat && beat);
  inline const StreamTBeat * tHead();
  inline std::optional<StreamTBeat> tTake();

  inline void dataBits(size_t bits);
  inline size_t dataBits() const;
  inline size_t dataBytes() const;

  inline void idBits(size_t bits);
  inline size_t idBits() const;
  inline size_t maxId() const;

  inline void enableSink(bool enabled);
  inline void enableSource(bool enabled);

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

inline bool Stream::tFree() const
{
  return m_tChannel.free();
}

inline bool Stream::tPush(StreamTBeat && beat)
{
  return m_tChannel.push(std::move(beat), beat.last, beat.id);
}

inline const StreamTBeat * Stream::tHead()
{
  return m_tChannel.headArbit();
}

inline std::optional<StreamTBeat> Stream::tTake()
{
  return m_tChannel.takeArbit();
}

inline void Stream::dataBits(size_t bits)
{
  m_dataBits = bits;
}

inline size_t Stream::dataBits() const
{
  return m_dataBits;
}

inline size_t Stream::dataBytes() const
{
  return (m_dataBits - 1) / 8 + 1;
}

inline void Stream::idBits(size_t bits)
{
  m_idBits = bits;
}

inline size_t Stream::idBits() const
{
  return m_idBits;
}

inline size_t Stream::maxId() const
{
  return (1u << m_idBits) - 1;
}

inline void Stream::enableSink(bool enabled)
{
  m_tChannel.enablePush(enabled);
}

inline void Stream::enableSource(bool enabled)
{
  m_tChannel.enableTake(enabled);
}

} // namespace sim:model
