library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.util.all;


entity PipelineTest is
end PipelineTest;

architecture PipelineTest of PipelineTest is

  signal s_clk      : std_logic;
  signal s_rst_n    : std_logic;
  signal s_stop     : std_logic;

  signal s_srcDone  : std_logic;
  signal s_snkDone  : std_logic;


  signal s_inLast   : std_logic;
  signal s_inValid  : std_logic;
  signal s_inReady  : std_logic;

  signal s_outLast  : std_logic;
  signal s_outValid : std_logic;
  signal s_outReady : std_logic;


  type t_State is (Empty, Full, Overflow);
  signal s_state : t_State;
  signal s_bufLast : std_logic;

begin

  i_sysenv : entity work.Sim_Sysenv
    port map (
      po_clk      => s_clk,
      po_rst_n    => s_rst_n,
      pi_stop     => s_stop);

  i_source : entity work.Sim_StreamSource
    generic map (
      g_TotalSize => 108*1024,
      g_BurstSize => 64,
      g_Bandwidth => 0.5,
      g_Latency   => 5.0)
    port map (
      pi_clk      => s_clk,
      pi_rst_n    => s_rst_n,
      po_last     => s_inLast,
      po_valid    => s_inValid,
      pi_ready    => s_inReady,
      po_done     => s_srcDone);

  i_sink : entity work.Sim_StreamSink
    generic map (
      g_TotalSize => 108*1024,
      g_Bandwidth => 0.8,
      g_Latency   => 15.0)
    port map (
      pi_clk      => s_clk,
      pi_rst_n    => s_rst_n,
      pi_last     => s_outLast,
      pi_valid    => s_outValid,
      po_ready    => s_outReady,
      po_done     => s_snkDone);

  s_stop <= s_srcDone and s_snkDone;

  -----------------------------------------------------------------------------
  -- DUT
  -----------------------------------------------------------------------------

  process(s_clk)
  begin
    if s_clk'event and s_clk = '1' then
      if s_rst_n = '0' then
        s_state <= Empty;
        s_bufLast <= '0';
        s_outLast <= '0';
      else
        case s_state is

          when Empty =>
            if s_inValid = '1' then
              s_outLast <= s_inLast;
              s_state <= Full;
            end if;

          when Full =>
            if s_inValid = '1' and s_outReady = '1' then
              s_outLast <= s_inLast;
              s_state <= Full;
            elsif s_inValid = '1' then
              s_bufLast <= s_inLast;
              s_state <= Overflow;
            elsif s_outReady = '1' then
              s_state <= Empty;
            end if;

          when Overflow =>
            if s_outReady = '1' then
              s_outLast <= s_bufLast;
              s_state <= Full;
            end if;

        end case;
      end if;
    end if;
  end process;

  with s_state select s_inReady <=
    '1' when Empty,
    '1' when Full,
    '0' when Overflow;

  with s_state select s_outValid <=
    '0' when Empty,
    '1' when Full,
    '1' when Overflow;

end PipelineTest;
