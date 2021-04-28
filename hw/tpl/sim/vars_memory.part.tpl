{{?type.x_has_wr}}
variable v_{{name}}_awaddr   : {{type.x_taddr.qualified}};
variable v_{{name}}_awlen    : {{?type.x_tlen}}{{type.x_tlen.qualified}}{{|type.x_tlen}}{{type.x_clen.type.qualified}} := {{type.x_clen.qualified}}{{/type.x_tlen}};
variable v_{{name}}_awsize   : {{?type.x_tsize}}{{type.x_tsize.qualified}}{{|type.x_tsize}}{{type.x_csize.type.qualified}} := {{type.x_csize.qualified}}{{/type.x_tsize}};
variable v_{{name}}_awburst  : {{?type.x_tburst}}{{type.x_tburst.qualified}}{{|type.x_tburst}}{{type.x_cburst.type.qualified}} := {{type.x_cburst.qualified}}{{/type.x_tburst}};
variable v_{{name}}_awid     : {{?type.x_tid}}{{type.x_tid.qualified}}{{|type.x_tid}}unsigned (-1 downto 0) := ""{{/type.x_tid}};
variable v_{{name}}_awvalid  : {{type.x_tlogic.qualified}};
variable v_{{name}}_wdata    : {{type.x_tdata.qualified}};
variable v_{{name}}_wstrb    : {{type.x_tstrb.qualified}};
variable v_{{name}}_wlast    : {{?type.x_tlast}}{{type.x_tlogic.qualified}}{{|type.x_tlast}}std_logic := '1'{{/type.x_tlast}};
variable v_{{name}}_wvalid   : {{type.x_tlogic.qualified}};
variable v_{{name}}_bready   : {{type.x_tlogic.qualified}};
variable v_{{name}}_awready  : {{type.x_tlogic.qualified}};
variable v_{{name}}_wready   : {{type.x_tlogic.qualified}};
variable v_{{name}}_bresp    : {{type.x_tresp.qualified}};
variable v_{{name}}_bid      : {{?type.x_tid}}{{type.x_tid.qualified}}{{|type.x_tid}}unsigned (-1 downto 0) := ""{{/type.x_tid}};
variable v_{{name}}_bvalid   : {{type.x_tlogic.qualified}};
{{/type.x_has_wr}}
{{?type.x_has_rd}}
variable v_{{name}}_araddr   : {{type.x_taddr.qualified}};
variable v_{{name}}_arlen    : {{?type.x_tlen}}{{type.x_tlen.qualified}}{{|type.x_tlen}}{{type.x_clen.type.qualified}} := {{type.x_clen.qualified}}{{/type.x_tlen}};
variable v_{{name}}_arsize   : {{?type.x_tsize}}{{type.x_tsize.qualified}}{{|type.x_tsize}}{{type.x_csize.type.qualified}} := {{type.x_csize.qualified}}{{/type.x_tsize}};
variable v_{{name}}_arburst  : {{?type.x_tburst}}{{type.x_tburst.qualified}}{{|type.x_tburst}}{{type.x_cburst.type.qualified}} := {{type.x_cburst.qualified}}{{/type.x_tburst}};
variable v_{{name}}_arid     : {{?type.x_tid}}{{type.x_tid.qualified}}{{|type.x_tid}}unsigned (-1 downto 0) := ""{{/type.x_tid}};
variable v_{{name}}_arvalid  : {{type.x_tlogic.qualified}};
variable v_{{name}}_rready   : {{type.x_tlogic.qualified}};
variable v_{{name}}_arready  : {{type.x_tlogic.qualified}};
variable v_{{name}}_rdata    : {{type.x_tdata.qualified}};
variable v_{{name}}_rresp    : {{type.x_tresp.qualified}};
variable v_{{name}}_rlast    : {{?type.x_tlast}}{{type.x_tlast.qualified}}{{|type.x_tlast}}std_logic := '1'{{/type.x_tlast}};
variable v_{{name}}_rid      : {{?type.x_tid}}{{type.x_tid.qualified}}{{|type.x_tid}}unsigned (-1 downto 0) := ""{{/type.x_tid}};
variable v_{{name}}_rvalid   : {{type.x_tlogic.qualified}};
{{/type.x_has_rd}}
