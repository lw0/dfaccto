library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.simple.t_Logic;
use work.simple.t_Logic_v;
use work.simple.t_Handshake_ms;
use work.simple.t_Handshake_sm;
use work.simple.t_Handshake_v_ms;
use work.simple.t_Handshake_v_sm;
use work.simple.c_HandshakeNull_ms;
use work.simple.c_HandshakeNull_sm;
use work.simple.t_Data;
use work.simple.t_Data_v;


entity Toplevel is
  generic (
    g_DataWidth : integer);
  port (
    pi_clk : in std_logic;
    pi_rst_n : in std_logic;

    pi_hsIn_ms : in t_Handshake_ms;
    po_hsIn_sm : out t_Handshake_sm;
    po_hsOut_ms : out t_Handshake_ms;
    pi_hsOut_sm : in t_Handshake_sm;
    pi_dataIn : in t_Data_v (0 to g_DataWidth-1);
    po_dataOut : out t_Data_v (0 to g_DataWidth-1);
    po_done : out t_Logic);
end Toplevel;


architecture Structure of Toplevel is

  signal s_hsIntFirst_ms : t_Handshake_ms;
  signal s_hsIntFirst_sm : t_Handshake_sm;
  signal s_hsIntMid_ms : t_Handshake_ms;
  signal s_hsIntMid_sm : t_Handshake_sm;
  signal s_dataIntFirst : t_Data_v (0 to g_DataWidth-1);
  signal s_dataIntMid : t_Data_v (0 to g_DataWidth-1);
  signal s_doneMid : t_Logic;
  signal s_doneFirst : t_Logic;
  signal s_doneLast : t_Logic;

begin

  i_mid : entity work.Inner
    generic map (
      g_DataWidth => g_DataWidth)
    port map (
      pi_clk => pi_clk,
      pi_rst_n => pi_rst_n,
      pi_hsIn_ms => s_hsIntFirst_ms,
      po_hsIn_sm => s_hsIntFirst_sm,
      po_hsOut_ms => s_hsIntMid_ms,
      pi_hsOut_sm => s_hsIntMid_sm,
      pi_dataIn => s_dataIntFirst,
      po_dataOut => s_dataIntMid,
      po_done => s_doneMid);

  i_first : entity work.Inner
    generic map (
      g_DataWidth => g_DataWidth)
    port map (
      pi_clk => pi_clk,
      pi_rst_n => pi_rst_n,
      pi_hsIn_ms => pi_hsIn_ms,
      po_hsIn_sm => po_hsIn_sm,
      po_hsOut_ms => s_hsIntFirst_ms,
      pi_hsOut_sm => s_hsIntFirst_sm,
      pi_dataIn => pi_dataIn,
      po_dataOut => s_dataIntFirst,
      po_done => s_doneFirst);

  i_last : entity work.Inner
    generic map (
      g_DataWidth => g_DataWidth)
    port map (
      pi_clk => pi_clk,
      pi_rst_n => pi_rst_n,
      pi_hsIn_ms => s_hsIntMid_ms,
      po_hsIn_sm => s_hsIntMid_sm,
      po_hsOut_ms => po_hsOut_ms,
      pi_hsOut_sm => pi_hsOut_sm,
      pi_dataIn => s_dataIntMid,
      po_dataOut => po_dataOut,
      po_done => s_doneLast);

  i_barrier : entity work.Barrier
    generic map (
      g_PortCount => 3)
    port map (
      pi_clk => pi_clk,
      pi_rst_n => pi_rst_n,
      pi_doneIn(0) => s_doneFirst,
      pi_doneIn(1) => s_doneMid,
      pi_doneIn(2) => s_doneLast,
      po_done => po_done);

end Structure;
