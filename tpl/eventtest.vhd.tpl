{{>lib:vhdl/dependencies.part.tpl}}


{{>lib:vhdl/defentity.part.tpl}}


architecture Behavioral of {{identifier}} is

  subtype at_Byte is {{x_pvala.type.qualified}};

  alias ai_sys is {{x_psys.identifier}};
  alias ai_valA is {{x_pvala.identifier}};
  alias ai_valB is {{x_pvalb.identifier}};
  alias ao_valC is {{x_pvalc.identifier}};
  alias ai_evtA_ms is {{x_pevta.identifier_ms}};
  alias ao_evtA_sm is {{x_pevta.identifier_sm}};
  alias ai_evtB_ms is {{x_pevtb.identifier_ms}};
  alias ao_evtB_sm is {{x_pevtb.identifier_sm}};
  alias ao_evtC_ms is {{x_pevtc.identifier_ms}};
  alias ai_evtC_sm is {{x_pevtc.identifier_sm}};

  constant c_WordZero : at_Byte := to_unsigned(0, at_Byte'length);
  constant c_WordOne : at_Byte := to_unsigned(1, at_Byte'length);

  type t_State is (Idle, Delay, Active, Done);

  signal s_stateA : t_State;
  signal s_countA : at_Byte;
  signal s_valueA : at_Byte;

  signal s_stateB : t_State;
  signal s_countB : at_Byte;
  signal s_valueB : at_Byte;

  signal s_stateC : t_State;
  signal s_countC : at_Byte;
  signal s_valueC : at_Byte;

begin

  -- Event A
  process (ai_sys.clk)
  begin
    if ai_sys.clk'event and ai_sys.clk = '1' then
      if ai_sys.rst_n = '0' then
        s_stateA <= Idle;
        s_countA <= (others => '0');
        s_valueA <= (others => '0');
      else
        case s_stateA is
          when Idle =>
            if ai_evtA_ms.stb = '1' then
              s_countA <= ai_evtA_ms.sdata;
              s_valueA <= ai_evtA_ms.sdata;
              s_stateA <= Delay;
            end if;

          when Delay =>
            if s_countA /= c_WordZero then
              s_countA <= s_countA - c_WordOne;
              s_stateA <= Delay;
            elsif ai_evtA_ms.stb = '1' then
              s_stateA <= Active;
            else
              s_stateA <= Done;
            end if;

          when Active =>
            if ai_evtA_ms.stb = '0' then
              s_stateA <= Done;
            end if;

          when Done =>
            s_stateA <= Done;
        end case;
      end if;
    end if;
  end process;
  with s_stateA select ao_evtA_sm.ack <=
    '1' when Delay,
    '1' when Active,
    '0' when others;

  -- Event B
  process (ai_sys.clk)
  begin
    if ai_sys.clk'event and ai_sys.clk = '1' then
      if ai_sys.rst_n = '0' then
        s_stateB <= Idle;
        s_countB <= (others => '0');
        s_valueB <= (others => '0');
      else
        case s_stateB is
          when Idle =>
            if ai_evtB_ms.stb = '1' then
              s_countB <= ai_evtB_ms.sdata;
              s_valueB <= ai_evtB_ms.sdata;
              s_stateB <= Delay;
            end if;

          when Delay =>
            if s_countB /= c_WordZero then
              s_countB <= s_countB - c_WordOne;
            elsif ai_evtB_ms.stb = '1' then
              s_stateB <= Active;
            else
              s_stateB <= Done;
            end if;

          when Active =>
            if ai_evtB_ms.stb = '0' then
              s_stateB <= Done;
            end if;

          when Done =>
            s_stateB <= Done;
        end case;
      end if;
    end if;
  end process;
  with s_stateB select ao_evtB_sm.ack <=
    '1' when Delay,
    '1' when Active,
    '0' when others;

  -- Event C
  process (ai_sys.clk)
  begin
    if ai_sys.clk'event and ai_sys.clk = '1' then
      if ai_sys.rst_n = '0' then
        s_stateC <= Idle;
        s_countC <= (others => '0');
        s_valueC <= (others => '0');
      else
        case s_stateC is
          when Idle =>
            if s_stateA = Done and s_stateB = Done then
              s_countC <= ai_valA + ai_valB;
              s_valueC <= ai_valA + ai_valB;
              s_stateC <= Delay;
            end if;

          when Delay =>
            if s_countC /= c_WordZero then
              s_countC <= s_countC - c_WordOne;
            else
              s_stateC <= Active;
            end if;

          when Active =>
            if ai_evtC_sm.ack = '1' then
              s_stateC <= Done;
            end if;

          when Done =>
            s_stateC <= Done;
        end case;
      end if;
    end if;
  end process;
  with s_stateC select ao_evtC_ms.stb <=
    '1' when Active,
    '0' when others;
  ao_evtC_ms.sdata <= s_valueC;

  ao_valC <= s_valueA + s_valueB;

end Behavioral;
