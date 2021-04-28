#include "stream.hpp"


namespace sim::model {

Stream::Stream(Environment & env, const std::string & name, const StreamConfig & cfg)
: Model(env, name)
, m_tChannel (cfg.tSize(), cfg.tPush(), cfg.tTake(), cfg.tLat(), cfg.tFlag())
, m_dataBits {0}
, m_idBits {0}
{ }

inline void Stream::tick()
{
  m_tChannel.tick();
}

inline void Stream::reset(sim::RndSeed seed)
{
  m_tChannel.reset(seed);
}

} // namespace sim:model
