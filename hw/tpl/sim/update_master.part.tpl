-- {{name}}
{{?type.x_has_wr}}
v_{{name}}_awready := {{qualified_sm}}.awready;
v_{{name}}_wready  := {{qualified_sm}}.wready;
v_{{name}}_bresp   := {{qualified_sm}}.bresp;
{{?type.x_tid}}
v_{{name}}_bid     := {{qualified_sm}}.bid;
{{/type.x_tid}}
v_{{name}}_bvalid  := {{qualified_sm}}.bvalid;
x_masterWrUpdate(
  v_ref     => c_{{name}}_ref,
  v_awready => v_{{name}}_awready,
  v_wready  => v_{{name}}_wready,
  v_bresp   => v_{{name}}_bresp,
  v_bid     => v_{{name}}_bid,
  v_bvalid  => v_{{name}}_bvalid);
{{/type.x_has_wr}}
{{?type.x_has_rd}}
v_{{name}}_arready := {{qualified_sm}}.arready;
v_{{name}}_rdata   := {{qualified_sm}}.rdata;
v_{{name}}_rresp   := {{qualified_sm}}.rresp;
{{?type.x_tlast}}
v_{{name}}_rlast   := {{qualified_sm}}.rlast;
{{/type.x_tlast}}
{{?type.x_tid}}
v_{{name}}_rid     := {{qualified_sm}}.rid;
{{/type.x_tid}}
v_{{name}}_rvalid  := {{qualified_sm}}.rvalid;
x_masterRdUpdate(
  v_ref     => c_{{name}}_ref,
  v_arready => v_{{name}}_arready,
  v_rdata   => v_{{name}}_rdata,
  v_rresp   => v_{{name}}_rresp,
  v_rlast   => v_{{name}}_rlast,
  v_rid     => v_{{name}}_rid,
  v_rvalid  => v_{{name}}_rvalid);
{{/type.x_has_rd}}
