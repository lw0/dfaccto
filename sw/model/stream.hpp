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
};

class Stream : public Model
{
  static const SignalCode SigFree = 0x1;
  static const SignalCode SigAvailable = 0x2;
  static const SignalCode SigEmpty = 0x3;

protected:
  Stream(Environment &       env,
         const std::string & name,
         size_t              depth,
         float               sinkBandwidth,
         float               sourceBandwidth,
         float               latency,
         sim::ChannelFlags   flags,
         sim::RndSeed        seed);

public:
  Stream(Environment &       env,
         const std::string & primaryName,
         const std::string & secondaryName,
         size_t              depth,
         float               sinkBandwidth,
         float               sourceBandwidth,
         float               latency,
         bool                sourceFair,
         bool                sourcePacked,
         bool                noArbit,
         sim::RndSeed        seed);

  virtual void tick() override;
  virtual void reset(sim::RndSeed seed = 0);

  inline sim::Signal sigFree();
  inline sim::Signal sigAvailable(sim::Id id);
  inline sim::Signal sigEmpty(sim::Id id);

  void tNextFrom(sim::vhdl::Logic v_tready);
  void tHeadTo(sim::vhdl::LogicArray * v_tdata,
               sim::vhdl::LogicArray * v_tkeep,
               sim::vhdl::LogicArray * v_tid,
               sim::vhdl::Logic      * v_tlast,
               sim::vhdl::Logic      * v_tvalid);
  void tPushFrom(sim::vhdl::LogicArray * v_tdata,
                 sim::vhdl::LogicArray * v_tkeep,
                 sim::vhdl::LogicArray * v_tid,
                 sim::vhdl::Logic        v_tlast,
                 sim::vhdl::Logic        v_tvalid);
  void tFreeTo(sim::vhdl::Logic * v_tready);

  inline size_t dataBits() const;
  inline size_t dataBytes() const;

  inline size_t idBits() const;
  inline size_t maxId() const;

  inline void enableSink(bool enabled);
  inline void enableSource(bool enabled);

protected:
  void setup();
  inline MultiChannel<StreamTBeat> & tch();
  inline const MultiChannel<StreamTBeat> & tch() const;

private:
  sim::MultiChannel<StreamTBeat> m_tChannel;
  bool m_noArbit;

  size_t m_dataBits;
  size_t m_idBits;
};


class StreamSource : public Stream
{
public:
  StreamSource(Environment & env,
               const std::string & name,
               size_t depth,
               float bandwidth,
               float latency,
               bool fair,
               bool packed,
               sim::RndSeed seed);

  bool write(sim::Id id, const sim::BitVector & data, bool last, size_t & pos);
};

class StreamSink : public Stream
{
public:
  StreamSink(Environment & env,
               const std::string & name,
               size_t depth,
               float bandwidth,
               float latency,
               bool fair,
               bool packed,
               sim::RndSeed seed);

  size_t read(sim::Id id, sim::BitVector & data, bool & last, size_t maxBeats);
};

} // namespace sim::model


namespace sim::model {

inline sim::Signal Stream::sigFree()
{
  return signalFor(SigFree);
}

inline sim::Signal Stream::sigAvailable(sim::Id id)
{
  return signalFor(SigAvailable, id);
}

inline sim::Signal Stream::sigEmpty(sim::Id id)
{
  return signalFor(SigEmpty, id);
}

inline size_t Stream::dataBits() const
{
  return m_dataBits;
}

inline size_t Stream::dataBytes() const
{
  return (m_dataBits - 1) / 8 + 1;
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
  m_tChannel.pushEnable(enabled);
}

inline void Stream::enableSource(bool enabled)
{
  m_tChannel.pushEnable(enabled);
}

inline MultiChannel<StreamTBeat> & Stream::tch()
{
  return m_tChannel;
}

inline const MultiChannel<StreamTBeat> & Stream::tch() const
{
  return m_tChannel;
}

} // namespace sim:model
