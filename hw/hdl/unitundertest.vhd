library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.dfaccto.t_Sys;
use work.user.t_Byte;
use work.user.t_ByteEvent_ms;
use work.user.t_ByteEvent_sm;


entity UnitUnderTest is
  port (
    pi_sys : in t_Sys;
    pi_valA : in  t_Byte;
    pi_valB : in  t_Byte;
    po_valC : out t_Byte;
    pi_evtA_ms : in  t_ByteEvent_ms;
    po_evtA_sm : out t_ByteEvent_sm;
    pi_evtB_ms : in  t_ByteEvent_ms;
    po_evtB_sm : out t_ByteEvent_sm;
    po_evtC_ms : out t_ByteEvent_ms;
    pi_evtC_sm : in  t_ByteEvent_sm);
end UnitUnderTest;

architecture UnitUnderTest of UnitUnderTest is

  constant c_WordZero : t_Byte := to_unsigned(0, t_Byte'length);
  constant c_WordOne : t_Byte := to_unsigned(1, t_Byte'length);

  type t_State is (Idle, Delay, Active, Done);

  signal s_stateA : t_State;
  signal s_countA : t_Byte;
  signal s_valueA : t_Byte;

  signal s_stateB : t_State;
  signal s_countB : t_Byte;
  signal s_valueB : t_Byte;

  signal s_stateC : t_State;
  signal s_countC : t_Byte;
  signal s_valueC : t_Byte;

begin

  -- Event A
  process (pi_sys.clk)
  begin
    if pi_sys.clk'event and pi_sys.clk = '1' then
      if pi_sys.rst_n = '0' then
        s_stateA <= Idle;
        s_countA <= (others => '0');
        s_valueA <= (others => '0');
      else
        case s_stateA is
          when Idle =>
            if pi_evtA_ms.stb = '1' then
              s_countA <= pi_evtA_ms.sdata;
              s_valueA <= pi_evtA_ms.sdata;
              s_stateA <= Delay;
            end if;

          when Delay =>
            if s_countA /= c_WordZero then
              s_countA <= s_countA - c_WordOne;
              s_stateA <= Delay;
            elsif pi_evtA_ms.stb = '1' then
              s_stateA <= Active;
            else
              s_stateA <= Done;
            end if;

          when Active =>
            if pi_evtA_ms.stb = '0' then
              s_stateA <= Done;
            end if;

          when Done =>
            s_stateA <= Done;
        end case;
      end if;
    end if;
  end process;
  with s_stateA select po_evtA_sm.ack <=
    '1' when Delay,
    '1' when Active,
    '0' when others;

  -- Event B
  process (pi_sys.clk)
  begin
    if pi_sys.clk'event and pi_sys.clk = '1' then
      if pi_sys.rst_n = '0' then
        s_stateB <= Idle;
        s_countB <= (others => '0');
        s_valueB <= (others => '0');
      else
        case s_stateB is
          when Idle =>
            if pi_evtB_ms.stb = '1' then
              s_countB <= pi_evtB_ms.sdata;
              s_valueB <= pi_evtB_ms.sdata;
              s_stateB <= Delay;
            end if;

          when Delay =>
            if s_countB /= c_WordZero then
              s_countB <= s_countB - c_WordOne;
            elsif pi_evtB_ms.stb = '1' then
              s_stateB <= Active;
            else
              s_stateB <= Done;
            end if;

          when Active =>
            if pi_evtB_ms.stb = '0' then
              s_stateB <= Done;
            end if;

          when Done =>
            s_stateB <= Done;
        end case;
      end if;
    end if;
  end process;
  with s_stateB select po_evtB_sm.ack <=
    '1' when Delay,
    '1' when Active,
    '0' when others;

  -- Event C
  process (pi_sys.clk)
  begin
    if pi_sys.clk'event and pi_sys.clk = '1' then
      if pi_sys.rst_n = '0' then
        s_stateC <= Idle;
        s_countC <= (others => '0');
        s_valueC <= (others => '0');
      else
        case s_stateC is
          when Idle =>
            if s_stateA = Done and s_stateB = Done then
              s_countC <= pi_valA + pi_valB;
              s_valueC <= pi_valA + pi_valB;
              s_stateC <= Delay;
            end if;

          when Delay =>
            if s_countC /= c_WordZero then
              s_countC <= s_countC - c_WordOne;
            else
              s_stateC <= Active;
            end if;

          when Active =>
            if pi_evtC_sm.ack = '1' then
              s_stateC <= Done;
            end if;

          when Done =>
            s_stateC <= Done;
        end case;
      end if;
    end if;
  end process;
  with s_stateC select po_evtC_ms.stb <=
    '1' when Active,
    '0' when others;
  po_evtC_ms.sdata <= s_valueC;

  po_valC <= s_valueA + s_valueB;

end UnitUnderTest;
