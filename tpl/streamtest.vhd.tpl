{{>lib:vhdl/dependencies.part.tpl}}


{{>lib:vhdl/defentity.part.tpl}}


architecture Behavioral of {{identifier}} is

  -- subtype at_Stream_ms is {{x_psrc.type.qualified_ms}};
  -- subtype at_Stream_sm is {{x_psrc.type.qualified_sm}};

  alias ai_sys is {{x_psys.identifier}};
  alias ai_stmSrc_ms is {{x_psrc.identifier_ms}};
  alias ao_stmSrc_sm is {{x_psrc.identifier_sm}};
  alias ao_stmToBuf_ms is {{x_ptobuf.identifier_ms}};
  alias ai_stmToBuf_sm is {{x_ptobuf.identifier_sm}};
  alias ai_stmFromBuf_ms is {{x_pfrombuf.identifier_ms}};
  alias ao_stmFromBuf_sm is {{x_pfrombuf.identifier_sm}};
  alias ao_stmSnk_ms is {{x_psnk.identifier_ms}};
  alias ai_stmSnk_sm is {{x_psnk.identifier_sm}};

  --  signal s_stmPre_ms : at_Stream_ms;
  --  signal s_stmPre_sm : at_Stream_sm;
  --  signal s_stmPost_ms : at_Stream_ms;
  --  signal s_stmPost_sm : at_Stream_sm;

begin

  ao_stmToBuf_ms <= ai_stmSrc_ms;
  ao_stmSrc_sm <= ai_stmToBuf_sm;

  ao_stmSnk_ms <= ai_stmFromBuf_ms;
  ao_stmFromBuf_sm <= ai_stmSnk_sm;

end Behavioral;
