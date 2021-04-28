#include "memory.hpp"


namespace sim::model {

Memory::Memory(Environment & env, const std::string & name, const MemoryConfig & cfg)
: Model(env, name)
, m_awChannel(cfg.awSize(), cfg.awPush(), cfg.awTake(), cfg.awLat(), cfg.awFlag())
, m_wChannel(cfg.wSize(),   cfg.wPush(),  cfg.wTake(),  cfg.wLat(),  cfg.wFlag())
, m_bChannel(cfg.bSize(),   cfg.bPush(),  cfg.bTake(),  cfg.bLat(),  cfg.bFlag())
, m_arChannel(cfg.arSize(), cfg.arPush(), cfg.arTake(), cfg.arLat(), cfg.arFlag())
, m_rChannel(cfg.rSize(),   cfg.rPush(),  cfg.rTake(),  cfg.rLat(),  cfg.rFlag())
, m_dataBits {0}
, m_addrBits {0}
, m_idBits {0}
{ }

void Memory::tick()
{
  m_awChannel.tick();
  m_wChannel.tick();
  m_bChannel.tick();
  m_arChannel.tick();
  m_rChannel.tick();
}

void Memory::reset(sim::RndSeed seed)
{
  m_awChannel.reset(seed);
  m_wChannel.reset(seed);
  m_bChannel.reset(seed);
  m_arChannel.reset(seed);
  m_rChannel.reset(seed);
}

}

