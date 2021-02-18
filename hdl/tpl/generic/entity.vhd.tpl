library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

{{#used_types}}
{{# identifiers}}
use work.{{package.identifier}}.{{.}};
{{/ identifiers}}
{{/used_types}}


entity {{identifier}} is
{{#has_generics}}
  generic (
{{# generics}}
    {{identifier}} : integer{{#_last}}){{/_last}};
{{/ generics}}
{{/has_generics}}
  port (
    pi_clk : in std_logic;
    pi_rst_n : in std_logic{{^has_ports}}){{/has_ports}};
{{#has_ports}}

{{#ports}}
{{# is_complex}}
    {{identifier_ms}} : {{mode_ms}} {{type.identifier_ms}}{{#is_vector}} (0 to {{size}}-1){{/is_vector}};
    {{identifier_sm}} : {{mode_sm}} {{type.identifier_sm}}{{#is_vector}} (0 to {{size}}-1){{/is_vector}}{{#_last}}){{/_last}};
{{/ is_complex}}
{{# is_simple}}
    {{identifier}} : {{mode}} {{type.identifier}}{{#is_vector}} (0 to {{size}}-1){{/is_vector}}{{#_last}}){{/_last}};
{{/ is_simple}}
{{/ports}}
{{/has_ports}}
end {{identifier}};


architecture Structure of {{identifier}} is

{{#signals}}
{{# is_simple}}
  signal {{identifier}} : {{type.identifier}}{{#is_vector}} (0 to {{size}}-1){{/is_vector}};
{{/ is_simple}}
{{# is_complex}}
  signal {{identifier_ms}} : {{type.identifier_ms}}{{#is_vector}} (0 to {{size}}-1){{/is_vector}};
  signal {{identifier_sm}} : {{type.identifier_sm}}{{#is_vector}} (0 to {{size}}-1){{/is_vector}};
{{/ is_complex}}
{{/signals}}

begin

{{#instances}}
  {{identifier}} : entity work.{{base.identifier}}
{{# has_generics}}
    generic map (
{{#  generics}}
{{#   has_value}}
      {{identifier}} => {{value}}{{#_last}}){{|_last}},{{/_last}}
{{/   has_value}}
{{^   has_value}}
      {{identifier}} => open{{#_last}}){{|_last}},{{/_last}}
{{/   has_value}}
{{/  generics}}
{{/ has_generics}}
    port map (
      pi_clk => pi_clk,
      pi_rst_n => pi_rst_n,
{{# ports}}
{{^  is_connected}}
      {{identifier}} => open{{#_last}});{{|_last}},{{/_last}}
{{/  is_connected}}
{{#  connections}}
{{#   is_simple}}
      {{'identifier}}({{_idx}}) => {{identifier}}{{#_last}}{{#'_last}});{{|'_last}},{{/'_last}}{{|_last}},{{/_last}}
{{/   is_simple}}
{{#   is_complex}}
      {{'identifier_ms}}({{_idx}}) => {{identifier_ms}},
      {{'identifier_sm}}({{_idx}}) => {{identifier_sm}}{{#_last}}{{#'_last}});{{|'_last}},{{/'_last}}{{|_last}},{{/_last}}
{{/   is_complex}}
{{/  connections}}
{{#  connection}}
{{#   is_simple}}
      {{'identifier}} => {{identifier}}{{#_last}});{{|_last}},{{/_last}}
{{/   is_simple}}
{{#   is_complex}}
      {{'identifier_ms}} => {{identifier_ms}},
      {{'identifier_sm}} => {{identifier_sm}}{{#_last}});{{|_last}},{{/_last}}
{{/   is_complex}}
{{/  connection}}
{{/ ports}}

{{/instances}}
end Structure;
