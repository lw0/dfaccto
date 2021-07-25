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

  // inline MemoryABeat(MemoryAddr addr, MemoryLen len, MemorySize size, MemoryBurst burst, sim::Id id)
  // : addr {addr}
  // , info {len, size, burst}
  // , id {id}
  // { }
};

struct MemoryRBeat
{
  sim::BitVector data; // rdata and rstrb
  sim::Id id;
  MemoryResp resp;
  bool last;

  // inline MemoryRBeat(std::size_t bits, sim::Id id = 0, MemoryResp resp = RespOk, bool last = false)
  // : data (bits)
  // , id {id}
  // , resp {resp}
  // , last {last}
  // { }
};

struct MemoryWBeat
{
  sim::BitVector data; // wdata and wstrb
  bool last;

  // inline MemoryWBeat(std::size_t bits, bool last = false)
  // : data (bits)
  // , last {last}
  // { }
};

struct MemoryBBeat
{
  sim::Id id;
  MemoryResp resp;

  // inline MemoryBBeat(sim::Id id = 0, MemoryResp resp = RespOk)
  // : id {id}
  // , resp {resp}
  // { }
};

class Memory : public Model
{
public:
  Memory(Environment & env, const std::string & name, const MemoryConfig & cfg);

  virtual void tick() override;

  void reset(sim::RndSeed seed = 0);

  void awNextFrom(sim::vhdl::Logic v_awready);
  void awHeadTo(sim::vhdl::LogicArray * v_awaddr,
                sim::vhdl::LogicArray * v_awlen,
                sim::vhdl::LogicArray * v_awsize,
                sim::vhdl::LogicArray * v_awburst,
                sim::vhdl::LogicArray * v_awid,
                sim::vhdl::Logic      * v_awvalid);
  void awPushFrom(sim::vhdl::LogicArray * v_awaddr,
                  sim::vhdl::LogicArray * v_awlen,
                  sim::vhdl::LogicArray * v_awsize,
                  sim::vhdl::LogicArray * v_awburst,
                  sim::vhdl::LogicArray * v_awid,
                  sim::vhdl::Logic        v_awvalid);
  void awFreeTo(sim::vhdl::Logic * v_awready);

  void arNextFrom(sim::vhdl::Logic v_arready);
  void arHeadTo(sim::vhdl::LogicArray * v_araddr,
                sim::vhdl::LogicArray * v_arlen,
                sim::vhdl::LogicArray * v_arsize,
                sim::vhdl::LogicArray * v_arburst,
                sim::vhdl::LogicArray * v_arid,
                sim::vhdl::Logic      * v_arvalid);
  void arPushFrom(sim::vhdl::LogicArray * v_araddr,
                  sim::vhdl::LogicArray * v_arlen,
                  sim::vhdl::LogicArray * v_arsize,
                  sim::vhdl::LogicArray * v_arburst,
                  sim::vhdl::LogicArray * v_arid,
                  sim::vhdl::Logic        v_arvalid);
  void arFreeTo(sim::vhdl::Logic * v_arready);

  void wNextFrom(sim::vhdl::Logic v_wready);
  void wHeadTo(sim::vhdl::LogicArray * v_wdata,
               sim::vhdl::LogicArray * v_wstrb,
               sim::vhdl::Logic      * v_wlast,
               sim::vhdl::Logic      * v_wvalid);
  void wPushFrom(sim::vhdl::LogicArray * v_wdata,
                 sim::vhdl::LogicArray * v_wstrb,
                 sim::vhdl::Logic        v_wlast,
                 sim::vhdl::Logic        v_wvalid);
  void wFreeTo(sim::vhdl::Logic * v_wready);

  void rNextFrom(sim::vhdl::Logic v_rready);
  void rHeadTo(sim::vhdl::LogicArray * v_rdata,
               sim::vhdl::LogicArray * v_rresp,
               sim::vhdl::Logic      * v_rlast,
               sim::vhdl::LogicArray * v_rid,
               sim::vhdl::Logic      * v_rvalid);
  void rPushFrom(sim::vhdl::LogicArray * v_rdata,
                 sim::vhdl::LogicArray * v_rresp,
                 sim::vhdl::Logic        v_rlast,
                 sim::vhdl::LogicArray * v_rid,
                 sim::vhdl::Logic        v_rvalid);
  void rFreeTo(sim::vhdl::Logic * v_rready);

  void bNextFrom(sim::vhdl::Logic v_bready);
  void bHeadTo(sim::vhdl::LogicArray * v_bresp,
               sim::vhdl::LogicArray * v_bid,
               sim::vhdl::Logic      * v_bvalid);
  void bPushFrom(sim::vhdl::LogicArray * v_bresp,
                 sim::vhdl::LogicArray * v_bid,
                 sim::vhdl::Logic        v_bvalid);
  void bFreeTo(sim::vhdl::Logic * v_bready);

  inline size_t dataBits() const;

  inline size_t addrBits() const;

  inline size_t idBits() const;

private:
  sim::SingleChannel<MemoryABeat> m_awChannel;
  sim::SingleChannel<MemoryWBeat> m_wChannel;
  sim::SingleChannel<MemoryBBeat> m_bChannel;
  sim::SingleChannel<MemoryABeat> m_arChannel;
  sim::SingleChannel<MemoryRBeat> m_rChannel;

  size_t m_dataBits;
  size_t m_addrBits;
  size_t m_idBits;
};

} // namespace sim::model


namespace sim::model {

inline size_t Memory::dataBits() const
{
  return m_dataBits;
}

inline size_t Memory::addrBits() const
{
  return m_addrBits;
}

inline size_t Memory::idBits() const
{
  return m_idBits;
}

}

