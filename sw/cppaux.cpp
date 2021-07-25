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
  g_env.onSetup(v_config->toString(), 4096);
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
    model->dataTo(v_output);
  }
}


void x_inputUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_input)
{
  sim::model::Wire * model = g_env.model<sim::model::Wire>(v_ref);
  if (model) {
    model->dataFrom(v_input);
  }
}


void x_pushUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_adata,
  sim::vhdl::Logic        v_ack)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->ackFrom(v_ack, v_adata);
  }
}


void x_pushState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_sdata,
  sim::vhdl::Logic      * v_strb)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->stbTo(v_strb, v_sdata);
  }
}


void x_pullUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_sdata,
  sim::vhdl::Logic        v_strb)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->stbFrom(v_strb, v_sdata);
  }
}

void x_pullState(
  std::uint32_t           v_ref,
  sim::vhdl::LogicArray * v_adata,
  sim::vhdl::Logic      * v_ack)
{
  sim::model::Event * model = g_env.model<sim::model::Event>(v_ref);
  if (model) {
    model->ackTo(v_ack, v_adata);
  }
}


void x_sourceUpdate(
  std::uint32_t           v_ref,
  sim::vhdl::Logic        v_tready)
{
  sim::model::Stream * model = g_env.model<sim::model::Stream>(v_ref);
  if (model) {
    model->tNextFrom(v_tready);
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
    model->tHeadTo(v_tdata, v_tkeep, v_tid, v_tlast, v_tvalid);
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
    model->tPushFrom(v_tdata, v_tkeep, v_tid, v_tlast, v_tvalid);
  }
}

void x_sinkState(
  std::uint32_t           v_ref,
  sim::vhdl::Logic      * v_tready)
{
  sim::model::Stream * model = g_env.model<sim::model::Stream>(v_ref);
  if (model) {
    model->tFreeTo(v_tready);
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
    model->awPushFrom(v_awaddr, v_awlen, v_awsize, v_awburst, v_awid, v_awvalid);
    model->wPushFrom(v_wdata, v_wstrb, v_wlast, v_wvalid);
    model->bNextFrom(v_bready);
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
    model->arPushFrom(v_araddr, v_arlen, v_arsize, v_arburst, v_arid, v_arvalid);
    model->rNextFrom(v_rready);
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
    model->awFreeTo(v_awready);
    model->wFreeTo(v_wready);
    model->bHeadTo(v_bresp, v_bid, v_bvalid);
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
    model->arFreeTo(v_arready);
    model->rHeadTo(v_rdata,v_rresp, v_rlast, v_rid, v_rvalid);
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
    model->awNextFrom(v_awready);
    model->wNextFrom(v_wready);
    model->bPushFrom(v_bresp, v_bid, v_bvalid);
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
    model->arNextFrom(v_arready);
    model->rPushFrom(v_rdata, v_rresp, v_rlast, v_rid, v_rvalid);
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
    model->awHeadTo(v_awaddr, v_awlen, v_awsize, v_awburst, v_awid, v_awvalid);
    model->wHeadTo(v_wdata, v_wstrb, v_wlast, v_wvalid);
    model->bFreeTo(v_bready);
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
    model->arHeadTo(v_araddr, v_arlen, v_arsize, v_arburst, v_arid, v_arvalid);
    model->rFreeTo(v_rready);
  }
}

}
