#pragma once

#include <string>

#include "../types.hpp"
#include "../bitvector.hpp"
#include "../channel.hpp"
#include "../model.hpp"


namespace sim::model {

using std::size_t;
using std::uint8_t;

class MemoryConfig
{
public:
  virtual size_t awSize() const = 0;
  virtual float awPush() const = 0;
  virtual float awTake() const = 0;
  virtual float awLat() const = 0;
  virtual ChannelFlags awFlag() const = 0;

  virtual size_t wSize() const = 0;
  virtual float wPush() const = 0;
  virtual float wTake() const = 0;
  virtual float wLat() const = 0;
  virtual ChannelFlags wFlag() const = 0;

  virtual size_t bSize() const = 0;
  virtual float bPush() const = 0;
  virtual float bTake() const = 0;
  virtual float bLat() const = 0;
  virtual ChannelFlags bFlag() const = 0;

  virtual size_t arSize() const = 0;
  virtual float arPush() const = 0;
  virtual float arTake() const = 0;
  virtual float arLat() const = 0;
  virtual ChannelFlags arFlag() const = 0;

  virtual size_t rSize() const = 0;
  virtual float rPush() const = 0;
  virtual float rTake() const = 0;
  virtual float rLat() const = 0;
  virtual ChannelFlags rFlag() const = 0;

  // static MemoryConfig Master(size_t addrDepth, float addrBandwidth, float addrLatency,
  //                            size_t dataDepth, float dataBandwidth, float dataLatency,
  //                            bool fair, sim::ChannelSeed seed)
  // {
  //   uint8_t flags = fair? ChannelFair : 0;
  //   return {addrDepth, 1.f, addrBandwidth, addrLatency, flags,
  //           dataDepth, 1.f, dataBandwidth, dataLatency, flags,
  //           addrDepth, addrBandwidth, 1.f, addrLatency, flags,
  //           addrDepth, 1.f, addrBandwidth, addrLatency, flags,
  //           dataDepth, dataBandwidth, 1.f, dataLatency, flags, seed};
  // }

  // static MemoryConfig Slave(size_t addrDepth, float addrBandwidth, float addrLatency,
  //                           size_t dataDepth, float dataBandwidth, float dataLatency,
  //                           bool fair, sim::ChannelSeed seed)
  // {
  //   uint8_t flags = fair? ChannelFair : 0;
  //   return {addrDepth, addrBandwidth, 1.f, addrLatency, flags,
  //           dataDepth, dataBandwidth, 1.f, dataLatency, flags,
  //           addrDepth, 1.f, addrBandwidth, addrLatency, flags,
  //           addrDepth, addrBandwidth, 1.f, addrLatency, flags,
  //           dataDepth, 1.f, dataBandwidth, dataLatency, flags, seed};
  // }

  // static MemoryConfig Pass(size_t addrDepth, float addrBandwidth, float addrLatency,
  //                          size_t dataDepth, float dataBandwidth, float dataLatency,
  //                          bool fair, sim::ChannelSeed seed)
  // {
  //   uint8_t flags = fair? ChannelFair : 0;
  //   return {addrDepth, addrBandwidth, addrBandwidth, addrLatency, flags,
  //           dataDepth, dataBandwidth, dataBandwidth, dataLatency, flags,
  //           addrDepth, addrBandwidth, addrBandwidth, addrLatency, flags,
  //           addrDepth, addrBandwidth, addrBandwidth, addrLatency, flags,
  //           dataDepth, dataBandwidth, dataBandwidth, dataLatency, flags, seed};
  // }
};

enum MemorySize
{
  Size1 = 0,
  Size2 = 1,
  Size4 = 2,
  Size8 = 3,
  Size16 = 4,
  Size32 = 5,
  Size64 = 6,
  Size128 = 7
};

enum MemoryBurst
{
  BurstFixed = 0,
  BurstIncr = 1,
  BurstWrap = 2
};

enum MemoryResp
{
  RespOk = 0,
  RespExOk = 1,
  RespSlvErr = 2,
  RespErr = 3
};

using MemoryAddr = std::uint64_t;
using MemoryLen = std::uint8_t;

struct MemoryBurstInfo
{
  MemoryLen len : 8;
  MemorySize size : 3;
  MemoryBurst burst : 2;
};

struct MemoryTransaction
{
  std::vector<sim::BitVector> data;
  sim::Unit addr;
  MemoryBurstInfo info;
  sim::Id id;
  MemoryResp resp;
};

struct MemoryABeat
{
  MemoryAddr addr;
  MemoryBurstInfo info;
  sim::Id id;

  inline MemoryABeat(MemoryAddr addr, MemoryLen len, MemorySize size, MemoryBurst burst, sim::Id id)
  : addr {addr}
  , info {len, size, burst}
  , id {id}
  { }
};

struct MemoryRBeat
{
  sim::BitVector data; // rdata and rstrb
  sim::Id id;
  MemoryResp resp;
  bool last;

  inline MemoryRBeat(std::size_t bits, sim::Id id = 0, MemoryResp resp = RespOk, bool last = false)
  : data (bits)
  , id {id}
  , resp {resp}
  , last {last}
  { }
};

struct MemoryWBeat
{
  sim::BitVector data; // wdata and wstrb
  bool last;

  inline MemoryWBeat(std::size_t bits, bool last = false)
  : data (bits)
  , last {last}
  { }
};

struct MemoryBBeat
{
  sim::Id id;
  MemoryResp resp;

  inline MemoryBBeat(sim::Id id = 0, MemoryResp resp = RespOk)
  : id {id}
  , resp {resp}
  { }
};

class Memory : public Model
{
public:
  Memory(Environment & env, const std::string & name, const MemoryConfig & cfg);

  virtual void tick() override;

  void reset(sim::RndSeed seed = 0);

  inline sim::Channel<MemoryABeat> & awch();
  inline sim::Channel<MemoryWBeat> & wch();
  inline sim::Channel<MemoryBBeat> & bch();
  inline sim::Channel<MemoryABeat> & arch();
  inline sim::Channel<MemoryRBeat> & rch();

  inline void dataBits(size_t bits);
  inline size_t dataBits() const;
  inline void addrBits(size_t bits);
  inline size_t addrBits() const;
  inline void idBits(size_t bits);
  inline size_t idBits() const;

private:
  sim::Channel<MemoryABeat> m_awChannel;
  sim::Channel<MemoryWBeat> m_wChannel;
  sim::Channel<MemoryBBeat> m_bChannel;
  sim::Channel<MemoryABeat> m_arChannel;
  sim::Channel<MemoryRBeat> m_rChannel;

  size_t m_dataBits;
  size_t m_addrBits;
  size_t m_idBits;
};

} // namespace sim::model


namespace sim::model {

inline sim::Channel<MemoryABeat> & Memory::awch()
{
  return m_awChannel;
}

inline sim::Channel<MemoryWBeat> & Memory::wch()
{
  return m_wChannel;
}

inline sim::Channel<MemoryBBeat> & Memory::bch()
{
  return m_bChannel;
}

inline sim::Channel<MemoryABeat> & Memory::arch()
{
  return m_arChannel;
}

inline sim::Channel<MemoryRBeat> & Memory::rch()
{
  return m_rChannel;
}

inline void Memory::dataBits(size_t bits)
{
  m_dataBits = bits;
}

inline size_t Memory::dataBits() const
{
  return m_dataBits;
}

inline void Memory::addrBits(size_t bits)
{
  m_addrBits = bits;
}

inline size_t Memory::addrBits() const
{
  return m_addrBits;
}

inline void Memory::idBits(size_t bits)
{
  m_idBits = bits;
}

inline size_t Memory::idBits() const
{
  return m_idBits;
}


}

