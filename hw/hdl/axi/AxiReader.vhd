library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.fosi_axi.all;
use work.fosi_ctrl.all;
use work.fosi_stream.all;
use work.fosi_util.all;


entity AxiReader is
  generic (
    g_FIFOLogDepth : natural);
  port (
    pi_clk     : in  std_logic;
    pi_rst_n   : in  std_logic;

    -- operation is started when both start and ready are asserted
    pi_start   : in  std_logic;
    po_ready   : out std_logic;
    -- while asserted, no new burst will be started
    pi_hold    : in  std_logic := '0';

    -- memory interface data will be read from
    po_axiRd_ms : out t_NativeAxiRd_ms;
    pi_axiRd_sm : in  t_NativeAxiRd_sm;

    -- output stream of read data
    po_stm_ms : out t_NativeStream_ms;
    pi_stm_sm : in  t_NativeStream_sm;

    -- Config port (4 registers):
    --  Reg0: Start address low word
    --  Reg1: Start address high word
    --  Reg2: Transfer count
    --  Reg3: Maximum Burst length
    pi_regs_ms : in  t_RegPort_ms;
    po_regs_sm : out t_RegPort_sm;

    po_status : out unsigned(19 downto 0));
end AxiReader;

architecture AxiReader of AxiReader is

  signal so_ready         : std_logic;
  signal s_addrStart      : std_logic;
  signal s_addrReady      : std_logic;

  -- Address State Machine
  signal s_address        : t_NativeAxiWordAddr;
  signal s_count          : t_RegData;
  signal s_maxLen         : t_NativeAxiBurstLen;

  -- Burst Count Queue
  signal s_queueBurstCount: t_NativeAxiBurstLen;
  signal s_queueBurstLast : std_logic;
  signal s_queueValid     : std_logic;
  signal s_queueReady     : std_logic;

  -- Data State Machine
  type t_State is (Idle, ThruConsume, Thru, ThruWait);
  signal s_state          : t_State;
  signal s_burstCount     : t_NativeAxiBurstLen;
  signal s_burstLast      : std_logic;
  signal so_mem_ms_rready : std_logic;

  -- Control Registers
  signal so_regs_sm_ready : std_logic;
  signal s_regAdr         : unsigned(2*c_RegDataWidth-1 downto 0);
  alias  a_regALo is s_regAdr(c_RegDataWidth-1 downto 0);
  alias  a_regAHi is s_regAdr(2*c_RegDataWidth-1 downto c_RegDataWidth);
  signal s_regCnt         : t_RegData;
  signal s_regBst         : t_RegData;

  -- Status Output
  signal s_addrStatus     : unsigned (7 downto 0);
  signal s_stateEnc       : unsigned (2 downto 0);

begin

  s_addrStart <= so_ready and pi_start;
  so_ready <= s_addrReady and f_logic(s_state = Idle);
  po_ready <= so_ready;

  -----------------------------------------------------------------------------
  -- Address State Machine
  -----------------------------------------------------------------------------
  po_axiRd_ms.arsize <= c_NativeAxiFullSize;
  po_axiRd_ms.arburst <= c_AxiBurstIncr;

  s_address <= f_resizeLeft(s_regAdr, s_address'length);
  s_count   <= s_regCnt;
  s_maxLen  <= f_resize(s_regBst, s_maxLen'length);
  i_addrMachine : entity work.AxiAddrMachine
    generic map (
      g_FIFOLogDepth => g_FIFOLogDepth)
    port map (
      pi_clk             => pi_clk,
      pi_rst_n           => pi_rst_n,
      pi_start           => s_addrStart,
      po_ready           => s_addrReady,
      pi_hold            => pi_hold,
      pi_address         => s_address,
      pi_count           => s_count,
      pi_maxLen          => s_maxLen,
      po_axiAAddr        => po_axiRd_ms.araddr,
      po_axiALen         => po_axiRd_ms.arlen,
      po_axiAValid       => po_axiRd_ms.arvalid,
      pi_axiAReady       => pi_axiRd_sm.arready,
      po_queueBurstCount => s_queueBurstCount,
      po_queueBurstLast  => s_queueBurstLast,
      po_queueValid      => s_queueValid,
      pi_queueReady      => s_queueReady,
      po_status          => s_addrStatus);

  -----------------------------------------------------------------------------
  -- Data State Machine
  -----------------------------------------------------------------------------

  po_stm_ms.tdata <= pi_axiRd_sm.rdata;
  with s_state select po_stm_ms.tkeep <=
    (others => '1')     when Thru,
    (others => '1')     when ThruConsume,
    (others => '0')     when others;
  po_stm_ms.tlast <= f_logic(s_burstCount = to_unsigned(0, s_burstCount'length) and s_burstLast = '1');
  with s_state select po_stm_ms.tvalid <=
    pi_axiRd_sm.rvalid    when Thru,
    pi_axiRd_sm.rvalid    when ThruConsume,
    '0'                 when others;
  with s_state select so_mem_ms_rready <=
    pi_stm_sm.tready when Thru,
    pi_stm_sm.tready when ThruConsume,
    '0'                 when others;
  po_axiRd_ms.rready <= so_mem_ms_rready;
  -- TODO-lw: handle rresp /= OKAY

  with s_state select s_queueReady <=
    '1' when ThruConsume,
    '0' when others;

  process (pi_clk)
    variable v_beat : boolean; -- Data Channel Handshake
    variable v_bend : boolean; -- Last Data Channel Handshake in Burst
    variable v_blst : boolean; -- Last Burst
    variable v_qval : boolean; -- Queue Valid
  begin
    if pi_clk'event and pi_clk = '1' then
      v_beat := pi_axiRd_sm.rvalid = '1' and
                so_mem_ms_rready = '1';
      v_bend := (s_burstCount = to_unsigned(0, s_burstCount'length)) and
                pi_axiRd_sm.rvalid = '1' and
                so_mem_ms_rready = '1';
      v_blst := s_burstLast = '1';
      v_qval := s_queueValid = '1';

      if pi_rst_n = '0' then
        s_burstCount <= (others => '0');
        s_burstLast <= '0';
        s_state  <= Idle;
      else
        case s_state is

          when Idle =>
            if v_qval then
              s_burstCount <= s_queueBurstCount;
              s_burstLast <= s_queueBurstLast;
              s_state <= ThruConsume;
            end if;

          when ThruConsume =>
            if v_beat then
              s_burstCount <= s_burstCount - to_unsigned(1, s_burstCount'length);
            end if;
            if v_bend then
              if v_blst then
                s_state <= Idle;
              else
                s_state <= ThruWait;
              end if;
            else
                s_state <= Thru;
            end if;

          when Thru =>
            if v_beat then
              s_burstCount <= s_burstCount - to_unsigned(1, s_burstCount'length);
            end if;
            if v_bend then
              if v_blst then
                s_state <= Idle;
              elsif v_qval then
                s_burstCount <= s_queueBurstCount;
                s_burstLast <= s_queueBurstLast;
                s_state <= ThruConsume;
              else
                s_state <= ThruWait;
              end if;
            end if;

          when ThruWait =>
            if v_qval then
              s_burstCount <= s_queueBurstCount;
              s_burstLast <= s_queueBurstLast;
              s_state <= ThruConsume;
            end if;

        end case;
      end if;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- Register Access
  -----------------------------------------------------------------------------
  po_regs_sm.ready <= so_regs_sm_ready;
  process (pi_clk)
    variable v_portAddr : integer range 0 to 2**pi_regs_ms.addr'length-1;
  begin
    if pi_clk'event and pi_clk = '1' then
      v_portAddr := to_integer(pi_regs_ms.addr);

      if pi_rst_n = '0' then
        po_regs_sm.rddata <= (others => '0');
        so_regs_sm_ready <= '0';
        s_regAdr <= (others => '0');
        s_regCnt <= (others => '0');
        s_regBst <= (others => '0');
      else
        if pi_regs_ms.valid = '1' and so_regs_sm_ready = '0' then
          so_regs_sm_ready <= '1';
          case v_portAddr is
            when 0 =>
              po_regs_sm.rddata <= a_regALo;
              if pi_regs_ms.wrnotrd = '1' then
                a_regALo <= f_byteMux(pi_regs_ms.wrstrb, a_regALo, pi_regs_ms.wrdata);
              end if;
            when 1 =>
              po_regs_sm.rddata <= a_regAHi;
              if pi_regs_ms.wrnotrd = '1' then
                a_regAHi <= f_byteMux(pi_regs_ms.wrstrb, a_regAHi, pi_regs_ms.wrdata);
              end if;
            when 2 =>
              po_regs_sm.rddata <= s_regCnt;
              if pi_regs_ms.wrnotrd = '1' then
                s_regCnt <= f_byteMux(pi_regs_ms.wrstrb, s_regCnt, pi_regs_ms.wrdata);
              end if;
            when 3 =>
              po_regs_sm.rddata <= s_regBst;
              if pi_regs_ms.wrnotrd = '1' then
                s_regBst <= f_byteMux(pi_regs_ms.wrstrb, s_regBst, pi_regs_ms.wrdata);
              end if;
            when others =>
              po_regs_sm.rddata <= (others => '0');
          end case;
        else
          so_regs_sm_ready <= '0';
        end if;
      end if;
    end if;
  end process;


  -----------------------------------------------------------------------------
  -- Status Output
  -----------------------------------------------------------------------------
  with s_state select s_stateEnc <=
    "000" when Idle,
    "001" when Thru,
    "011" when ThruConsume,
    "010" when ThruWait;
  po_status <= s_burstLast & s_stateEnc & f_resize(s_burstCount, 8) & s_addrStatus;

end AxiReader;
