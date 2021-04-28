#include "bitvector.hpp"
#include "environment.hpp"
#include "types.hpp"
#include "vhdl/types.hpp"
#include "vhdl/logicarray.hpp"
#include "vhdl/string.hpp"
#include "model/wire.hpp"
#include "model/event.hpp"
#include "model/stream.hpp"
#include "model/memory.hpp"


sim::Environment g_env;

extern "C" {

void x_setup(sim::vhdl::String * v_config)
{
  g_env.onSetup(v_config->toString(), 64);
}

void x_tick(
  sim::vhdl::Logic * v_rst_n,
  sim::vhdl::Logic * v_stop)
{
  g_env.onTick();
  v_rst_n->set(!g_env.system().getReset());
  v_stop->set(g_env.system().getStop());
}

void x_shutdown()
{
  g_env.onShutdown();
}

std::uint32_t x_register(
  sim::vhdl::String * v_kind,
  sim::vhdl::String * v_name)
{
  return g_env.registerModel(v_name->toString());
}


void x_outputState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_output)
{
  sim::model::Wire * model = g_env.model<sim::model::Wire>(v_ref);
  if (model) {
    model->dataBits(v_output->size());
    v_output->fromUnit(model->data());
  }
}


void x_inputUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_input)
{
  sim::model::Wire * model = g_env.model<sim::model::Wire>(v_ref);
  if (model) {
    model->dataBits(v_input->size());
    model->data(v_input->toUnit());
  }
}


void x_pushUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_adata,
  sim::vhdl::Logic        v_ack)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->ackBits(v_adata->size());
    model->ack(v_ack.isSet(), v_adata->toUnit());
  }
}


void x_pushState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_sdata,
  sim::vhdl::Logic      * v_strb)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->stbBits(v_sdata->size());
    v_strb->set(model->stbSet());
    v_sdata->fromUnit(model->stbData());
  }
}


void x_pullUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_sdata,
  sim::vhdl::Logic        v_strb)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->stbBits(v_sdata->size());
    model->stb(v_strb.isSet(), v_sdata->toUnit());
  }
}

void x_pullState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_adata,
  sim::vhdl::Logic      * v_ack)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->ackBits(v_adata->size());
    v_ack->set(model->ackSet());
    v_adata->fromUnit(model->ackData());
  }
}


void x_sourceUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::Logic        v_tready)
{
  sim::model::Stream * model = g_env.model<sim::model::Stream>(v_ref);
  if (model) {
    if (model->tch().head() && v_tready.isSet()) {
      model->tch().take();
    }
  }
}

void x_sourceState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_tdata,
  sim::vhdl::LogicArray * v_tkeep,
  sim::vhdl::LogicArray * v_tid,
  sim::vhdl::Logic      * v_tlast,
  sim::vhdl::Logic      * v_tvalid)
{
  sim::model::Stream * model = g_env.model<sim::model::Stream>(v_ref);
  if (model) {
    model->dataBits(v_tdata->size());
    model->idBits(v_tid->size());
    if (model->tch().head()) {
      const sim::model::StreamTBeat & beat = *model->tch().head();
      v_tdata->fromBitVector(beat.data);
      v_tkeep->fromBitVectorValid(beat.data, 8);
      v_tid->fromUnit(beat.id);
      v_tlast->set(beat.last);
      v_tvalid->set(true);
    } else {
      v_tdata->fill(sim::vhdl::Logic::VX);
      v_tkeep->fill(sim::vhdl::Logic::VX);
      v_tid->fill(sim::vhdl::Logic::VX);
      v_tlast->set(sim::vhdl::Logic::VX);
      v_tvalid->set(false);
    }
  }
}


void x_sinkUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_tdata,
  sim::vhdl::LogicArray * v_tkeep,
  sim::vhdl::LogicArray * v_tid,
  sim::vhdl::Logic        v_tlast,
  sim::vhdl::Logic        v_tvalid)
{
  sim::model::Stream * model = g_env.model<sim::model::Stream>(v_ref);
  if (model) {
    model->dataBits(v_tdata->size());
    model->idBits(v_tid->size());
    if (v_tvalid.isSet() && model->tch().free()) {
      sim::model::StreamTBeat beat (
        v_tdata->size(),
        (sim::Id)v_tid->toUnit(),
        v_tlast.isSet());
      v_tdata->toBitVector(beat.data);
      v_tkeep->toBitVectorValid(beat.data, 8);
      model->tch().push(std::move(beat), beat.id);
    }
  }
}

void x_sinkState(
  std::uint32_t           v_ref,
  sim::vhdl::Logic      * v_ready)
{
  sim::model::Stream * model = g_env.model<sim::model::Stream>(v_ref);
  if (model) {
    v_ready->set(model->tch().free());
  }
}


void x_slaveWrUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_awaddr,
  sim::vhdl::LogicArray * v_awlen,
  sim::vhdl::LogicArray * v_awsize,
  sim::vhdl::LogicArray * v_awburst,
  sim::vhdl::LogicArray * v_awid,
  sim::vhdl::Logic        v_awvalid,
  sim::vhdl::LogicArray * v_wdata,
  sim::vhdl::LogicArray * v_wstrb,
  sim::vhdl::Logic        v_wlast,
  sim::vhdl::Logic        v_wvalid,
  sim::vhdl::Logic        v_bready)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    model->dataBits(v_wdata->size());
    model->addrBits(v_awaddr->size());
    model->idBits(v_awid->size());
    if (model->awch().free() && v_awvalid.isSet()) {
      sim::model::MemoryABeat beat (
        (sim::model::MemoryAddr)v_awaddr->toUnit(),
        (sim::model::MemoryLen)v_awlen->toUnit(),
        (sim::model::MemorySize)v_awsize->toUnit(),
        (sim::model::MemoryBurst)v_awburst->toUnit(),
        (sim::Id)v_awid->toUnit());
      model->awch().push(std::move(beat));
    }
    if (model->wch().free() && v_wvalid.isSet()) {
      sim::model::MemoryWBeat beat (
        v_wdata->size(),
        v_wlast.isSet());
      v_wdata->toBitVector(beat.data);
      v_wstrb->toBitVectorValid(beat.data, 8);
      model->wch().push(std::move(beat));
    }
    if (model->bch().head() && v_bready.isSet()) {
      model->bch().take();
    }
  }
}

void x_slaveRdUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_araddr,
  sim::vhdl::LogicArray * v_arlen,
  sim::vhdl::LogicArray * v_arsize,
  sim::vhdl::LogicArray * v_arburst,
  sim::vhdl::LogicArray * v_arid,
  sim::vhdl::Logic        v_arvalid,
  sim::vhdl::Logic        v_rready)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    model->addrBits(v_araddr->size());
    model->idBits(v_arid->size());
    if (model->arch().free() && v_arvalid.isSet()) {
      sim::model::MemoryABeat beat (
        (sim::model::MemoryAddr)v_araddr->toUnit(),
        (sim::model::MemoryLen)v_arlen->toUnit(),
        (sim::model::MemorySize)v_arsize->toUnit(),
        (sim::model::MemoryBurst)v_arburst->toUnit(),
        (sim::Id)v_arid->toUnit());
      model->arch().push(std::move(beat));
    }
    if (model->rch().head() && v_rready.isSet()) {
      model->rch().take();
    }
  }
}

void x_slaveWrState(
  std::uint32_t           v_ref,
  sim::vhdl::Logic      * v_awready,
  sim::vhdl::Logic      * v_wready,
  sim::vhdl::LogicArray * v_bresp,
  sim::vhdl::LogicArray * v_bid,
  sim::vhdl::Logic      * v_bvalid)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    v_awready->set(model->awch().free());
    v_wready->set(model->wch().free());
    if (model->bch().head()) {
      const sim::model::MemoryBBeat & beat = *model->bch().head();
      v_bresp->fromUnit(beat.resp);
      v_bid->fromUnit(beat.id);
      v_bvalid->set(true);
    } else {
      v_bresp->fill(sim::vhdl::Logic::VX);
      v_bid->fill(sim::vhdl::Logic::VX);
      v_bvalid->set(false);
    }
  }
}

void x_slaveRdState(
  std::uint32_t           v_ref,
  sim::vhdl::Logic      * v_arready,
  sim::vhdl::LogicArray * v_rdata,
  sim::vhdl::LogicArray * v_rresp,
  sim::vhdl::Logic      * v_rlast,
  sim::vhdl::LogicArray * v_rid,
  sim::vhdl::Logic      * v_rvalid)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    model->dataBits(v_rdata->size());
    v_arready->set(model->arch().free());
    if (model->rch().head()) {
      const sim::model::MemoryRBeat & beat = *model->rch().head();
      v_rdata->fromBitVector(beat.data);
      v_rresp->fromUnit(beat.resp);
      v_rlast->set(beat.last);
      v_rid->fromUnit(beat.id);
      v_rvalid->set(true);
    } else {
      v_rdata->fill(sim::vhdl::Logic::VX);
      v_rresp->fill(sim::vhdl::Logic::VX);
      v_rlast->set(sim::vhdl::Logic::VX);
      v_rid->fill(sim::vhdl::Logic::VX);
      v_rvalid->set(false);
    }
  }
}


void x_masterWrUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::Logic        v_awready,
  sim::vhdl::Logic        v_wready,
  sim::vhdl::LogicArray * v_bresp,
  sim::vhdl::LogicArray * v_bid,
  sim::vhdl::Logic        v_bvalid)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    if (model->awch().head() && v_awready.isSet()) {
      model->awch().take();
    }
    if (model->wch().head() && v_wready.isSet()) {
      model->wch().take();
    }
    if (model->bch().free() && v_bvalid.isSet()) {
      sim::model::MemoryBBeat beat(
        (sim::Id)v_bid->toUnit(),
        (sim::model::MemoryResp)v_bresp->toUnit());
      model->bch().push(std::move(beat));
    }
  }
}

void x_masterRdUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::Logic        v_arready,
  sim::vhdl::LogicArray * v_rdata,
  sim::vhdl::LogicArray * v_rresp,
  sim::vhdl::Logic        v_rlast,
  sim::vhdl::LogicArray * v_rid,
  sim::vhdl::Logic        v_rvalid)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    model->dataBits(v_rdata->size());
    if (model->arch().head() && v_arready.isSet()) {
      model->arch().take();
    }
    if (model->rch().free() && v_rvalid.isSet()) {
      sim::model::MemoryRBeat beat (
        v_rdata->size(),
        (sim::Id)v_rid->toUnit(),
        (sim::model::MemoryResp)v_rresp->toUnit(),
        v_rlast.isSet());
      v_rdata->toBitVector(beat.data);
      model->rch().push(std::move(beat));
    }
  }
}

void x_masterWrState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_awaddr,
  sim::vhdl::LogicArray * v_awlen,
  sim::vhdl::LogicArray * v_awsize,
  sim::vhdl::LogicArray * v_awburst,
  sim::vhdl::LogicArray * v_awid,
  sim::vhdl::Logic      * v_awvalid,
  sim::vhdl::LogicArray * v_wdata,
  sim::vhdl::LogicArray * v_wstrb,
  sim::vhdl::Logic      * v_wlast,
  sim::vhdl::Logic      * v_wvalid,
  sim::vhdl::Logic      * v_bready)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    model->dataBits(v_wdata->size());
    model->addrBits(v_awaddr->size());
    model->idBits(v_awid->size());
    if (model->awch().head()) {
      const sim::model::MemoryABeat & beat = *model->awch().head();
      v_awaddr->fromUnit(beat.addr);
      v_awlen->fromUnit(beat.info.len);
      v_awsize->fromUnit(beat.info.size);
      v_awburst->fromUnit(beat.info.burst);
      v_awid->fromUnit(beat.id);
      v_awvalid->set(true);
    } else {
      v_awaddr->fill(sim::vhdl::Logic::VX);
      v_awlen->fill(sim::vhdl::Logic::VX);
      v_awsize->fill(sim::vhdl::Logic::VX);
      v_awburst->fill(sim::vhdl::Logic::VX);
      v_awid->fill(sim::vhdl::Logic::VX);
      v_awvalid->set(false);
    }
    if (model->wch().head()) {
      const sim::model::MemoryWBeat & beat = *model->wch().head();
      v_wdata->fromBitVector(beat.data);
      v_wstrb->fromBitVectorValid(beat.data, 8);
      v_wlast->set(beat.last);
      v_wvalid->set(true);
    } else {
      v_wdata->fill(sim::vhdl::Logic::VX);
      v_wstrb->fill(sim::vhdl::Logic::VX);
      v_wlast->set(sim::vhdl::Logic::VX);
      v_wvalid->set(false);
    }
    v_bready->set(model->bch().free());
  }
}

void x_masterRdState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_araddr,
  sim::vhdl::LogicArray * v_arlen,
  sim::vhdl::LogicArray * v_arsize,
  sim::vhdl::LogicArray * v_arburst,
  sim::vhdl::LogicArray * v_arid,
  sim::vhdl::Logic      * v_arvalid,
  sim::vhdl::Logic      * v_rready)
{
  sim::model::Memory * model = g_env.model<sim::model::Memory>(v_ref);
  if (model) {
    model->addrBits(v_araddr->size());
    model->idBits(v_arid->size());
    if (model->arch().head()) {
      const sim::model::MemoryABeat & beat = *model->arch().head();
      v_araddr->fromUnit(beat.addr);
      v_arlen->fromUnit(beat.info.len);
      v_arsize->fromUnit(beat.info.size);
      v_arburst->fromUnit(beat.info.burst);
      v_arid->fromUnit(beat.id);
      v_arvalid->set(true);
    } else {
      v_araddr->fromUnit(sim::vhdl::Logic::VX);
      v_arlen->fromUnit(sim::vhdl::Logic::VX);
      v_arsize->fromUnit(sim::vhdl::Logic::VX);
      v_arburst->fromUnit(sim::vhdl::Logic::VX);
      v_arid->fromUnit(sim::vhdl::Logic::VX);
      v_arvalid->set(false);
    }
    v_rready->set(model->rch().free());
  }
}

}
