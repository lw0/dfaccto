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
, m_dataBits {0}
, m_idBits {0}
{ }

Stream::Stream(Environment &       env,
         const std::string & primaryName,
         const std::string & secondaryName,
         size_t              depth,
         float               sinkBandwidth,
         float               latency,
         float               sourceBandwidth,
         bool                sourceFair,
         bool                sourcePacked,
         sim::RndSeed        seed)
: Model(env, primaryName, secondaryName)
, m_tChannel (depth, sinkBandwidth, sourceBandwidth, latency, (sourceFair? ChannelFairArbit : 0) | (sourcePacked? ChannelPackArbit : 0), seed)
, m_dataBits {0}
, m_idBits {0}
{ }

void Stream::tick()
{
  m_tChannel.tick();
}

void Stream::reset(sim::RndSeed seed)
{
  m_tChannel.reset(seed);
}

} // namespace sim:model
