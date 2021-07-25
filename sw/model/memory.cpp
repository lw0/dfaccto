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

void Memory::awNextFrom(sim::vhdl::Logic v_awready)
{
  if (m_awChannel.head() && v_awready.isSet()) {
    m_awChannel.take();
  }
}

void Memory::awHeadTo(sim::vhdl::LogicArray * v_awaddr,
              sim::vhdl::LogicArray * v_awlen,
              sim::vhdl::LogicArray * v_awsize,
              sim::vhdl::LogicArray * v_awburst,
              sim::vhdl::LogicArray * v_awid,
              sim::vhdl::Logic      * v_awvalid)
{
  m_addrBits = v_awaddr->size();
  m_idBits = v_awid->size();
  if (m_awChannel.head()) {
    const MemoryABeat & beat = *m_awChannel.head();
    v_awaddr->fromUnit(beat.addr);
    v_awlen->fromUnit(beat.info.len);
    v_awsize->fromUnit(beat.info.size);
    v_awburst->fromUnit(beat.info.burst);
    v_awid->fromUnit(beat.id);
    v_awvalid->set(true);
  } else {
    v_awaddr->fill(sim::vhdl::Logic::VD);
    v_awlen->fill(sim::vhdl::Logic::VD);
    v_awsize->fill(sim::vhdl::Logic::VD);
    v_awburst->fill(sim::vhdl::Logic::VD);
    v_awid->fill(sim::vhdl::Logic::VD);
    v_awvalid->set(false);
  }
}

void Memory::awPushFrom(sim::vhdl::LogicArray * v_awaddr,
                sim::vhdl::LogicArray * v_awlen,
                sim::vhdl::LogicArray * v_awsize,
                sim::vhdl::LogicArray * v_awburst,
                sim::vhdl::LogicArray * v_awid,
                sim::vhdl::Logic        v_awvalid)
{
  m_addrBits = v_awaddr->size();
  m_idBits = v_awid->size();
  if (m_awChannel.free() && v_awvalid.isSet()) {
    sim::model::MemoryABeat beat;
    beat.addr = v_awaddr->toUnit();
    beat.info.len = v_awlen->toUnit();
    beat.info.size = (sim::model::MemorySize)v_awsize->toUnit();
    beat.info.burst = (sim::model::MemoryBurst)v_awburst->toUnit();
    beat.id = v_awid->toUnit();
    m_awChannel.push(0, std::move(beat));
  }
}

void Memory::awFreeTo(sim::vhdl::Logic * v_awready)
{
  v_awready->set(m_awChannel.free());
}

void Memory::arNextFrom(sim::vhdl::Logic v_arready)
{
  if (m_arChannel.head() && v_arready.isSet()) {
    m_arChannel.take();
  }
}

void Memory::arHeadTo(sim::vhdl::LogicArray * v_araddr,
              sim::vhdl::LogicArray * v_arlen,
              sim::vhdl::LogicArray * v_arsize,
              sim::vhdl::LogicArray * v_arburst,
              sim::vhdl::LogicArray * v_arid,
              sim::vhdl::Logic      * v_arvalid)
{
  m_addrBits = v_araddr->size();
  m_idBits = v_arid->size();
  if (m_arChannel.head()) {
    const MemoryABeat & beat = *m_arChannel.head();
    v_araddr->fromUnit(beat.addr);
    v_arlen->fromUnit(beat.info.len);
    v_arsize->fromUnit(beat.info.size);
    v_arburst->fromUnit(beat.info.burst);
    v_arid->fromUnit(beat.id);
    v_arvalid->set(true);
  } else {
    v_araddr->fill(sim::vhdl::Logic::VD);
    v_arlen->fill(sim::vhdl::Logic::VD);
    v_arsize->fill(sim::vhdl::Logic::VD);
    v_arburst->fill(sim::vhdl::Logic::VD);
    v_arid->fill(sim::vhdl::Logic::VD);
    v_arvalid->set(false);
  }
}

void Memory::arPushFrom(sim::vhdl::LogicArray * v_araddr,
                sim::vhdl::LogicArray * v_arlen,
                sim::vhdl::LogicArray * v_arsize,
                sim::vhdl::LogicArray * v_arburst,
                sim::vhdl::LogicArray * v_arid,
                sim::vhdl::Logic        v_arvalid)
{
  m_addrBits = v_araddr->size();
  m_idBits = v_arid->size();
  if (m_arChannel.free() && v_arvalid.isSet()) {
    sim::model::MemoryABeat beat;
    beat.addr = v_araddr->toUnit();
    beat.info.len = v_arlen->toUnit();
    beat.info.size = (sim::model::MemorySize)v_arsize->toUnit();
    beat.info.burst = (sim::model::MemoryBurst)v_arburst->toUnit();
    beat.id = v_arid->toUnit();
    m_arChannel.push(0, std::move(beat));
  }
}

void Memory::arFreeTo(sim::vhdl::Logic * v_arready)
{
  v_arready->set(m_arChannel.free());
}

void Memory::wNextFrom(sim::vhdl::Logic v_wready)
{
  if (m_wChannel.head() && v_wready.isSet()) {
    m_wChannel.take();
  }
}

void Memory::wHeadTo(sim::vhdl::LogicArray * v_wdata,
             sim::vhdl::LogicArray * v_wstrb,
             sim::vhdl::Logic      * v_wlast,
             sim::vhdl::Logic      * v_wvalid)
{
  m_dataBits = v_wdata->size();
  if (m_wChannel.head()) {
    const MemoryWBeat & beat = *m_wChannel.head();
    beat.data.toLogic(v_wdata);
    beat.data.toLogicValid(v_wstrb, 8);
    v_wlast->set(beat.last);
    v_wvalid->set(true);
  } else {
    v_wdata->fill(sim::vhdl::Logic::VD);
    v_wstrb->fill(sim::vhdl::Logic::VD);
    v_wlast->set(sim::vhdl::Logic::VD);
    v_wvalid->set(false);
  }

}

void Memory::wPushFrom(sim::vhdl::LogicArray * v_wdata,
               sim::vhdl::LogicArray * v_wstrb,
               sim::vhdl::Logic        v_wlast,
               sim::vhdl::Logic        v_wvalid)
{
  m_dataBits = v_wdata->size();
  if (m_wChannel.free() && v_wvalid.isSet()) {
    sim::model::MemoryWBeat beat;
    beat.data.fromLogic(v_wdata);
    beat.data.fromLogicValid(v_wstrb, 8);
    beat.last = v_wlast.isSet();
    m_wChannel.push(0, std::move(beat));
  }
}

void Memory::wFreeTo(sim::vhdl::Logic * v_wready)
{
  v_wready->set(m_wChannel.free());
}

void Memory::rNextFrom(sim::vhdl::Logic v_rready)
{
  if (m_rChannel.head() && v_rready.isSet()) {
    m_rChannel.take();
  }
}

void Memory::rHeadTo(sim::vhdl::LogicArray * v_rdata,
             sim::vhdl::LogicArray * v_rresp,
             sim::vhdl::Logic      * v_rlast,
             sim::vhdl::LogicArray * v_rid,
             sim::vhdl::Logic      * v_rvalid)
{
  m_idBits = v_rid->size();
  m_dataBits = v_rdata->size();
  if (m_rChannel.head()) {
    const MemoryRBeat & beat = *m_rChannel.head();
    beat.data.toLogic(v_rdata);
    v_rresp->fromUnit(beat.resp);
    v_rlast->set(beat.last);
    v_rid->fromUnit(beat.id);
    v_rvalid->set(true);
  } else {
    v_rdata->fill(sim::vhdl::Logic::VD);
    v_rresp->fill(sim::vhdl::Logic::VD);
    v_rlast->set(sim::vhdl::Logic::VD);
    v_rid->fill(sim::vhdl::Logic::VD);
    v_rvalid->set(false);
  }
}

void Memory::rPushFrom(sim::vhdl::LogicArray * v_rdata,
               sim::vhdl::LogicArray * v_rresp,
               sim::vhdl::Logic        v_rlast,
               sim::vhdl::LogicArray * v_rid,
               sim::vhdl::Logic        v_rvalid)
{
  m_idBits = v_rid->size();
  m_dataBits = v_rdata->size();
  if (m_rChannel.free() && v_rvalid.isSet()) {
    sim::model::MemoryRBeat beat;
    beat.data.fromLogic(v_rdata);
    beat.id = v_rid->toUnit();
    beat.resp = (sim::model::MemoryResp)v_rresp->toUnit();
    beat.last = v_rlast.isSet();
    m_rChannel.push(0, std::move(beat));
  }
}

void Memory::rFreeTo(sim::vhdl::Logic * v_rready)
{
  v_rready->set(m_rChannel.free());
}

void Memory::bNextFrom(sim::vhdl::Logic v_bready)
{
  if (m_bChannel.head() && v_bready.isSet()) {
    m_bChannel.take();
  }
}

void Memory::bHeadTo(sim::vhdl::LogicArray * v_bresp,
             sim::vhdl::LogicArray * v_bid,
             sim::vhdl::Logic      * v_bvalid)
{
  m_idBits = v_bid->size();
  if (m_bChannel.head()) {
    const sim::model::MemoryBBeat & beat = *m_bChannel.head();
    v_bresp->fromUnit(beat.resp);
    v_bid->fromUnit(beat.id);
    v_bvalid->set(true);
  } else {
    v_bresp->fill(sim::vhdl::Logic::VD);
    v_bid->fill(sim::vhdl::Logic::VD);
    v_bvalid->set(false);
  }
}

void Memory::bPushFrom(sim::vhdl::LogicArray * v_bresp,
               sim::vhdl::LogicArray * v_bid,
               sim::vhdl::Logic        v_bvalid)
{
  m_idBits = v_bid->size();
  if (m_bChannel.free() && v_bvalid.isSet()) {
    sim::model::MemoryBBeat beat;
    beat.id = v_bid->toUnit();
    beat.resp = (sim::model::MemoryResp)v_bresp->toUnit();
    m_bChannel.push(0, std::move(beat));
  }
}

void Memory::bFreeTo(sim::vhdl::Logic * v_bready)
{
  v_bready->set(m_bChannel.free());
}

}

