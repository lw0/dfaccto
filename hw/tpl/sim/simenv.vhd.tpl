library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.sim_interface.all;
{{#dependencies}}
use work.{{identifier}};
{{/dependencies}}


entity {{identifier}} is
{{?generics}}
  generic (
{{# generics}}
{{#  is_complex}}
    {{identifier_ms}} : {{#is_scalar}}{{type.qualified_ms}}{{|is_scalar}}{{type.qualified_v_ms}} (0 to {{=size}}{{?is_literal}}{{=value}}{{*type.x_format}}{{/value}}{{|is_literal}}{{qualified}}{{/is_literal}}{{/size}}-1){{/is_scalar}}{{?_last}}){{/_last}};
    {{identifier_sm}} : {{#is_scalar}}{{type.qualified_sm}}{{|is_scalar}}{{type.qualified_v_sm}} (0 to {{=size}}{{?is_literal}}{{=value}}{{*type.x_format}}{{/value}}{{|is_literal}}{{qualified}}{{/is_literal}}{{/size}}-1){{/is_scalar}}{{?_last}}){{/_last}};
{{|  is_complex}}
    {{identifier}} : {{#is_scalar}}{{type.qualified}}{{|is_scalar}}{{type.qualified_v}} (0 to {{=size}}{{?is_literal}}{{=value}}{{*type.x_format}}{{/value}}{{|is_literal}}{{qualified}}{{/is_literal}}{{/size}}-1){{/is_scalar}}{{?_last}}){{/_last}};
{{/  is_complex}}
{{/ generics}}
{{/generics}}
{{?ports}}
  port (
{{# ports}}
{{#  is_complex}}
    {{identifier_ms}} : {{mode_ms}} {{#is_scalar}}{{type.qualified_ms}}{{|is_scalar}}{{type.qualified_v_ms}} (0 to {{=size}}{{?is_literal}}{{=value}}{{*type.x_format}}{{/value}}{{|is_literal}}{{qualified}}{{/is_literal}}{{/size}}-1){{/is_scalar}};
    {{identifier_sm}} : {{mode_sm}} {{#is_scalar}}{{type.qualified_sm}}{{|is_scalar}}{{type.qualified_v_sm}} (0 to {{=size}}{{?is_literal}}{{=value}}{{*type.x_format}}{{/value}}{{|is_literal}}{{qualified}}{{/is_literal}}{{/size}}-1){{/is_scalar}}{{?_last}}){{/_last}};
{{|  is_complex}}
    {{identifier}} : {{mode}} {{#is_scalar}}{{type.qualified}}{{|is_scalar}}{{type.qualified_v}} (0 to {{=size}}{{?is_literal}}{{=value}}{{*type.x_format}}{{/value}}{{|is_literal}}{{qualified}}{{/is_literal}}{{/size}}-1){{/is_scalar}}{{?_last}}){{/_last}};
{{/  is_complex}}
{{/ ports}}
{{/ports}}
end {{identifier}};


architecture Structure of {{identifier}} is

constant c_ClockDelay : {{x_gperiod.type.qualified}} := {{x_gperiod.qualified}} / 2;
constant c_ChangeDelay : {{x_gperiod.type.qualified}} := {{x_gperiod.qualified}} / 16;

signal s_clk : std_logic := '0';
signal s_rst_n : std_logic := '0';
signal s_stop : std_logic := '0';

begin

  process
  begin
    x_setup({{x_gconfig.qualified}});
    while s_stop = '0' loop
      wait for c_ClockDelay;
      s_clk <= '0';
      wait for c_ClockDelay;
      s_clk <= '1';
    end loop;
    wait for c_ClockDelay;
    x_shutdown;
    report "End Simulation" severity failure;
  end process;

  process (s_clk)
    variable v_rst_n : std_logic;
    variable v_stop : std_logic;
{{#ports}}
{{? x_interface}}
    -- {{name}}
    constant c_{{name}}_ref : natural := x_register("{{x_interface}}", "{{name}}");
    {{*x_vars}}
{{/ x_interface}}
{{/ports}}
  begin
    if s_clk'event and s_clk = '1' then
      ------- Update -------
{{#ports}}
{{? x_interface}}
      {{*x_update}}
{{/ x_interface}}
{{/ports}}
      -------- Tick --------
      x_tick(v_rst_n, v_stop);
      s_rst_n <= v_rst_n;
      s_stop <= v_stop;
      ------- State --------
{{#ports}}
{{? x_interface}}
      {{*x_state}}
{{/ x_interface}}
{{/ports}}
      ----------------------
    end if;
  end process;

  {{x_psys.qualified}}.clk <= s_clk;
  {{x_psys.qualified}}.rst_n <= s_rst_n;

end Structure;
