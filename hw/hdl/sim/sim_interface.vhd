library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


package sim_interface is

  procedure x_setup(
    v_config  : in  string);
  attribute foreign of x_setup : procedure is "VHPIDIRECT x_setup";

  procedure x_tick(
    v_rst_n   : out std_logic;
    v_stop    : out std_logic);
  attribute foreign of x_tick : procedure is "VHPIDIRECT x_tick";

  procedure x_shutdown;
  attribute foreign of x_shutdown : procedure is "VHPIDIRECT x_shutdown";

  function x_register(
    v_kind    : in  string;
    v_name    : in  string) return natural;
  attribute foreign of x_register : function is "VHPIDIRECT x_register";

  procedure x_outputState(
    v_ref     : in  natural;
    v_output  : out unsigned);
  attribute foreign of x_outputState : procedure is "VHPIDIRECT x_outputState";

  procedure x_inputUpdate(
    v_ref     : in  natural;
    v_input   : in  unsigned);
  attribute foreign of x_inputUpdate : procedure is "VHPIDIRECT x_inputUpdate";

  procedure x_pushUpdate(
    v_ref     : in  natural;
    v_adata   : in  unsigned;
    v_ack     : in  std_logic);
  attribute foreign of x_pushUpdate : procedure is "VHPIDIRECT x_pushUpdate";

  procedure x_pushState(
    v_ref     : in  natural;
    v_sdata   : out unsigned;
    v_stb     : out std_logic);
  attribute foreign of x_pushState : procedure is "VHPIDIRECT x_pushState";

  procedure x_pullUpdate(
    v_ref     : in  natural;
    v_sdata   : in  unsigned;
    v_stb     : in  std_logic);
  attribute foreign of x_pullUpdate : procedure is "VHPIDIRECT x_pullUpdate";

  procedure x_pullState(
    v_ref     : in  natural;
    v_adata   : out unsigned;
    v_ack     : out std_logic);
  attribute foreign of x_pullState : procedure is "VHPIDIRECT x_pullState";

  procedure x_sourceUpdate(
    v_ref     : in  natural;
    v_tready  : in  std_logic);
  attribute foreign of x_sourceUpdate : procedure is "VHPIDIRECT x_sourceUpdate";

  procedure x_sourceState(
    v_ref     : in  natural;
    v_tdata   : out unsigned;
    v_tkeep   : out unsigned;
    v_tid     : out unsigned;
    v_tlast   : out std_logic;
    v_tvalid  : out std_logic);
  attribute foreign of x_sourceState : procedure is "VHPIDIRECT x_sourceState";

  procedure x_sinkUpdate(
    v_ref     : in  natural;
    v_tdata   : in  unsigned;
    v_tkeep   : in  unsigned;
    v_tid     : in  unsigned;
    v_tlast   : in  std_logic;
    v_tvalid  : in  std_logic);
  attribute foreign of x_sinkUpdate : procedure is "VHPIDIRECT x_sinkUpdate";

  procedure x_sinkState(
    v_ref     : in  natural;
    v_tready  : out std_logic);
  attribute foreign of x_sinkState : procedure is "VHPIDIRECT x_sinkState";

  procedure x_slaveWrUpdate(
    v_ref     : in  natural;
    v_awaddr  : in  unsigned;
    v_awlen   : in  unsigned;
    v_awsize  : in  unsigned;
    v_awburst : in  unsigned;
    v_awid    : in  unsigned;
    v_awvalid : in  std_logic;
    v_wdata   : in  unsigned;
    v_wstrb   : in  unsigned;
    v_wlast   : in  std_logic;
    v_wvalid  : in  std_logic;
    v_bready  : in  std_logic);
  attribute foreign of x_slaveWrUpdate : procedure is "VHPIDIRECT x_slaveWrUpdate";

  procedure x_slaveRdUpdate(
    v_ref     : in  natural;
    v_araddr  : in  unsigned;
    v_arlen   : in  unsigned;
    v_arsize  : in  unsigned;
    v_arburst : in  unsigned;
    v_arid    : in  unsigned;
    v_arvalid : in  std_logic;
    v_rready  : in  std_logic);
  attribute foreign of x_slaveRdUpdate : procedure is "VHPIDIRECT x_slaveRdUpdate";

  procedure x_slaveWrState(
    v_ref     : in  natural;
    v_awready : out std_logic;
    v_wready  : out std_logic;
    v_bresp   : out unsigned;
    v_bid     : out unsigned;
    v_bvalid  : out std_logic);
  attribute foreign of x_slaveWrState : procedure is "VHPIDIRECT x_slaveWrState";

  procedure x_slaveRdState(
    v_ref     : in  natural;
    v_arready : out std_logic;
    v_rdata   : out unsigned;
    v_rresp   : out unsigned;
    v_rlast   : out std_logic;
    v_rid     : out unsigned;
    v_rvalid  : out std_logic);
  attribute foreign of x_slaveRdState : procedure is "VHPIDIRECT x_slaveRdState";

  procedure x_masterWrUpdate(
    v_ref     : in  natural;
    v_awready : in  std_logic;
    v_wready  : in  std_logic;
    v_bresp   : in  unsigned;
    v_bid     : in  unsigned;
    v_bvalid  : in  std_logic);
  attribute foreign of x_masterWrUpdate : procedure is "VHPIDIRECT x_masterWrUpdate";

  procedure x_masterRdUpdate(
    v_ref     : in  natural;
    v_arready : in  std_logic;
    v_rdata   : in  unsigned;
    v_rresp   : in  unsigned;
    v_rlast   : in  std_logic;
    v_rid     : in  unsigned;
    v_rvalid  : in  std_logic);
  attribute foreign of x_masterRdUpdate : procedure is "VHPIDIRECT x_masterRdUpdate";

  procedure x_masterWrState(
    v_ref     : in  natural;
    v_awaddr  : out unsigned;
    v_awlen   : out unsigned;
    v_awsize  : out unsigned;
    v_awburst : out unsigned;
    v_awid    : out unsigned;
    v_awvalid : out std_logic;
    v_wdata   : out unsigned;
    v_wstrb   : out unsigned;
    v_wlast   : out std_logic;
    v_wvalid  : out std_logic;
    v_bready  : out std_logic);
  attribute foreign of x_masterWrState : procedure is "VHPIDIRECT x_masterWrState";

  procedure x_masterRdState(
    v_ref     : in  natural;
    v_araddr  : out unsigned;
    v_arlen   : out unsigned;
    v_arsize  : out unsigned;
    v_arburst : out unsigned;
    v_arid    : out unsigned;
    v_arvalid : out std_logic;
    v_rready  : out std_logic);
  attribute foreign of x_masterRdState : procedure is "VHPIDIRECT x_masterRdState";

end sim_interface;


package body sim_interface is

  procedure x_setup(
    v_config  : in  string) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_setup'" severity error;
  end x_setup;

  procedure x_tick(
    v_rst_n   : out std_logic;
    v_stop    : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_tick'" severity error;
  end x_tick;

  procedure x_shutdown is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_shutdown'" severity error;
  end x_shutdown;

  function x_register(
    v_kind    : in  string;
    v_name    : in  string) return natural is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_register'" severity error;
  end x_register;

  procedure x_outputState(
    v_ref     : in  natural;
    v_output  : out unsigned) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_outputState'" severity error;
  end x_outputState;

  procedure x_inputUpdate(
    v_ref     : in  natural;
    v_input   : in  unsigned) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_inputUpdate'" severity error;
  end x_inputUpdate;

  procedure x_pushUpdate(
    v_ref     : in  natural;
    v_adata   : in  unsigned;
    v_ack     : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_pushUpdate'" severity error;
  end x_pushUpdate;

  procedure x_pushState(
    v_ref     : in  natural;
    v_sdata   : out unsigned;
    v_stb     : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_pushState'" severity error;
  end x_pushState;

  procedure x_pullUpdate(
    v_ref     : in  natural;
    v_sdata   : in  unsigned;
    v_stb     : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_pullUpdate'" severity error;
  end x_pullUpdate;

  procedure x_pullState(
    v_ref     : in  natural;
    v_adata   : out unsigned;
    v_ack     : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_pullState'" severity error;
  end x_pullState;

  procedure x_sourceUpdate(
    v_ref     : in  natural;
    v_tready  : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_sourceUpdate'" severity error;
  end x_sourceUpdate;

  procedure x_sourceState(
    v_ref     : in  natural;
    v_tdata   : out unsigned;
    v_tkeep   : out unsigned;
    v_tid     : out unsigned;
    v_tlast   : out std_logic;
    v_tvalid  : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_sourceState'" severity error;
  end x_sourceState;

  procedure x_sinkUpdate(
    v_ref     : in  natural;
    v_tdata   : in  unsigned;
    v_tkeep   : in  unsigned;
    v_tid     : in  unsigned;
    v_tlast   : in  std_logic;
    v_tvalid  : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_sinkUpdate'" severity error;
  end x_sinkUpdate;

  procedure x_sinkState(
    v_ref     : in  natural;
    v_tready  : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_sinkState'" severity error;
  end x_sinkState;

  procedure x_slaveWrUpdate(
    v_ref     : in  natural;
    v_awaddr  : in  unsigned;
    v_awlen   : in  unsigned;
    v_awsize  : in  unsigned;
    v_awburst : in  unsigned;
    v_awid    : in  unsigned;
    v_awvalid : in  std_logic;
    v_wdata   : in  unsigned;
    v_wstrb   : in  unsigned;
    v_wlast   : in  std_logic;
    v_wvalid  : in  std_logic;
    v_bready  : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_slaveWrUpdate'" severity error;
  end x_slaveWrUpdate;

  procedure x_slaveRdUpdate(
    v_ref     : in  natural;
    v_araddr  : in  unsigned;
    v_arlen   : in  unsigned;
    v_arsize  : in  unsigned;
    v_arburst : in  unsigned;
    v_arid    : in  unsigned;
    v_arvalid : in  std_logic;
    v_rready  : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_slaveRdUpdate'" severity error;
  end x_slaveRdUpdate;

  procedure x_slaveWrState(
    v_ref     : in  natural;
    v_awready : out std_logic;
    v_wready  : out std_logic;
    v_bresp   : out unsigned;
    v_bid     : out unsigned;
    v_bvalid  : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_slaveWrState'" severity error;
  end x_slaveWrState;

  procedure x_slaveRdState(
    v_ref     : in  natural;
    v_arready : out std_logic;
    v_rdata   : out unsigned;
    v_rresp   : out unsigned;
    v_rlast   : out std_logic;
    v_rid     : out unsigned;
    v_rvalid  : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_slaveRdState'" severity error;
  end x_slaveRdState;

  procedure x_masterWrUpdate(
    v_ref     : in  natural;
    v_awready : in  std_logic;
    v_wready  : in  std_logic;
    v_bresp   : in  unsigned;
    v_bid     : in  unsigned;
    v_bvalid  : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_masterWrUpdate'" severity error;
  end x_masterWrUpdate;

  procedure x_masterRdUpdate(
    v_ref     : in  natural;
    v_arready : in  std_logic;
    v_rdata   : in  unsigned;
    v_rresp   : in  unsigned;
    v_rlast   : in  std_logic;
    v_rid     : in  unsigned;
    v_rvalid  : in  std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_masterRdUpdate'" severity error;
  end x_masterRdUpdate;

  procedure x_masterWrState(
    v_ref     : in  natural;
    v_awaddr  : out unsigned;
    v_awlen   : out unsigned;
    v_awsize  : out unsigned;
    v_awburst : out unsigned;
    v_awid    : out unsigned;
    v_awvalid : out std_logic;
    v_wdata   : out unsigned;
    v_wstrb   : out unsigned;
    v_wlast   : out std_logic;
    v_wvalid  : out std_logic;
    v_bready  : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_masterWrState'" severity error;
  end x_masterWrState;

  procedure x_masterRdState(
    v_ref     : in  natural;
    v_araddr  : out unsigned;
    v_arlen   : out unsigned;
    v_arsize  : out unsigned;
    v_arburst : out unsigned;
    v_arid    : out unsigned;
    v_arvalid : out std_logic;
    v_rready  : out std_logic) is
  begin
    report "Unresolved VHPIDIRECT symbol 'x_masterRdState'" severity error;
  end x_masterRdState;

end sim_interface;
