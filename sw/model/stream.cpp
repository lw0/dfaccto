#include "stream.hpp"


namespace sim::model {

Stream::Stream(Environment &       env,
               const std::string & name,
               size_t              depth,
               float               sinkBandwidth,
               float               sourceBandwidth,
               float               latency,
               ChannelFlags        flags,
               sim::RndSeed        seed)
: Model(env, name)
, m_tChannel (depth, sinkBandwidth, sourceBandwidth, latency, flags, seed)
, m_noArbit {false}
, m_dataBits {0}
, m_idBits {0}
{
  setup();
}

Stream::Stream(Environment &       env,
         const std::string & primaryName,
         const std::string & secondaryName,
         size_t              depth,
         float               sinkBandwidth,
         float               sourceBandwidth,
         float               latency,
         bool                sourceFair,
         bool                sourcePacked,
         bool                noArbit,
         sim::RndSeed        seed)
: Model(env, primaryName, secondaryName)
, m_tChannel (depth, sinkBandwidth, sourceBandwidth, latency, (sourceFair? ChannelFairArbit : 0) | (sourcePacked? ChannelPackArbit : 0), seed)
, m_noArbit {noArbit}
, m_dataBits {0}
, m_idBits {0}
{
  setup();
}

void Stream::setup()
{
  m_tChannel.setFreeCallback([this]()
    {
      emit(SigFree);
    });
  m_tChannel.setHeadCallback([this](sim::Id id, bool last)
    {
      emit(SigAvailable, id);
    });
  m_tChannel.setTakeCallback([this](sim::Id id, bool last)
    {
      if (m_tChannel.itemCount(id) == 0) {
        emit(SigEmpty, id);
      }
    });
}

void Stream::tick()
{
  m_tChannel.tick();
}

void Stream::reset(sim::RndSeed seed)
{
  m_tChannel.reset(seed);
}

void Stream::tNextFrom(sim::vhdl::Logic v_tready)
{
  if (m_tChannel.head() && v_tready.isSet()) {
    m_tChannel.take();
  }
}

void Stream::tHeadTo(sim::vhdl::LogicArray * v_tdata,
                     sim::vhdl::LogicArray * v_tkeep,
                     sim::vhdl::LogicArray * v_tid,
                     sim::vhdl::Logic      * v_tlast,
                     sim::vhdl::Logic      * v_tvalid)
{
  m_dataBits = v_tdata->size();
  m_idBits = v_tid->size();
  const StreamTBeat * head = nullptr;
  head = m_tChannel.head();
  if (head) {
    const StreamTBeat & beat = *head;
    beat.data.toLogic(v_tdata, sim::vhdl::Logic::VD);
    beat.data.toLogicValid(v_tkeep, 8);
    v_tid->fromUnit(beat.id);
    v_tlast->set(beat.last);
    v_tvalid->set(true);
  } else {
    v_tdata->fill(sim::vhdl::Logic::VD);
    v_tkeep->fill(sim::vhdl::Logic::VD);
    v_tid->fill(sim::vhdl::Logic::VD);
    v_tlast->set(sim::vhdl::Logic::VD);
    v_tvalid->set(false);
  }
}

void Stream::tPushFrom(sim::vhdl::LogicArray * v_tdata,
                       sim::vhdl::LogicArray * v_tkeep,
                       sim::vhdl::LogicArray * v_tid,
                       sim::vhdl::Logic        v_tlast,
                       sim::vhdl::Logic        v_tvalid)
{
  m_dataBits = v_tdata->size();
  m_idBits = v_tid->size();
  if (v_tvalid.isSet() && m_tChannel.free()) {
    sim::Id id = v_tid->toUnit();
    bool last = v_tlast.isSet();
    sim::model::StreamTBeat beat;
    beat.data.fromLogic(v_tdata);
    beat.data.fromLogicValid(v_tkeep, 8);
    beat.id = id;
    beat.last = last;
    m_tChannel.push(0, std::move(beat));
  }
}

void Stream::tFreeTo(sim::vhdl::Logic * v_tready)
{
  v_tready->set(m_tChannel.free());
}

StreamSource::StreamSource(Environment & env,
             const std::string & name,
             size_t depth,
             float bandwidth,
             float latency,
             bool fair,
             bool packed,
             sim::RndSeed seed)
: Stream(env, name,
         depth,
         1.f,
         bandwidth,
         latency,
         ChannelPushNoblock | (fair? ChannelFairArbit : 0) | (packed? ChannelPackArbit : 0),
         seed)
{ }

bool StreamSource::write(sim::Id id, const sim::BitVector & data, bool last, size_t & pos)
{
  for (size_t idx = 0; pos < data.bits(); pos += dataBits(), ++idx) {
    bool last = (pos + dataBits()) >= data.bits();
    if (!tch().push(idx, data.slice(pos, dataBits()), id, last)) {
      return false;
    }
  }
  return true;
}

StreamSink::StreamSink(Environment & env,
             const std::string & name,
             size_t depth,
             float bandwidth,
             float latency,
             bool fair,
             bool packed,
             sim::RndSeed seed)
: Stream(env, name,
         depth,
         bandwidth,
         1.f,
         latency,
         ChannelTakeNoblock | (fair? ChannelFairArbit : 0) | (packed? ChannelPackArbit : 0),
         seed)
{ }

size_t StreamSink::read(sim::Id id, sim::BitVector & data, bool & last, size_t maxBeats)
{
  size_t beats = 0;
  std::optional<StreamTBeat> maybeat;
  while ((maybeat = tch().take(id))) {
    StreamTBeat beat = *maybeat;
    data.append(beat.data);
    last = beat.last;
    ++beats;
    if (last || (maxBeats > 0 && beats >= maxBeats)) break;
  }
  return beats;
}

// size_t StreamSink::readArbit(sim::Id & id, sim::BitVector & data, bool & last, size_t maxBeats)
// {
//   size_t beats = 0;
//   bool hasId = false;
//   const StreamTBeat * pbeat;
//   while ((pbeat = tch().head()) && (!hasId || id == pbeat->id)) {
//     StreamTBeat beat = *tch().take();
//     data.append(beat.data);
//     last = beat.last;
//     id = beat.id;
//     ++beats;
//     if (last || (maxBeats > 0 && beats >= maxBeats)) break;
//   }
//   return beats;
// }
// TODO-lw think through whether waiting for id-specific SigAvailable is consistent with reading beats as chosen by the arbiter... might need to switch from arbited to id-specific head()/take() but this collides with the intended usage and the arbiter latch flag!

} // namespace sim:model
