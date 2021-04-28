-- {{name}}
{{?type.x_has_wr}}
x_slaveWrState(
  v_ref     => c_{{name}}_ref,
  v_awready => v_{{name}}_awready,
  v_wready  => v_{{name}}_wready,
  v_bresp   => v_{{name}}_bresp,
  v_bid     => v_{{name}}_bid,
  v_bvalid  => v_{{name}}_bvalid);
{{qualified_sm}}.awready <= v_{{name}}_awready after c_ChangeDelay;
{{qualified_sm}}.wready  <= v_{{name}}_wready after c_ChangeDelay;
{{qualified_sm}}.bresp   <= v_{{name}}_bresp after c_ChangeDelay;
{{? type.x_tid}}
{{qualified_sm}}.bid     <= v_{{name}}_bid after c_ChangeDelay;
{{/ type.x_tid}}
{{? type.x_tbuser}}
{{qualified_sm}}.buser   <= {{type.x_tbuser.x_cnull.qualified}};
{{/ type.x_tbuser}}
{{qualified_sm}}.bvalid  <= v_{{name}}_bvalid after c_ChangeDelay;
{{/type.x_has_wr}}
{{?type.x_has_rd}}
x_slaveRdState(
  v_ref     => c_{{name}}_ref,
  v_arready => v_{{name}}_arready,
  v_rdata   => v_{{name}}_rdata,
  v_rresp   => v_{{name}}_rresp,
  v_rlast   => v_{{name}}_rlast,
  v_rid     => v_{{name}}_rid,
  v_rvalid  => v_{{name}}_rvalid);
{{qualified_sm}}.arready <= v_{{name}}_arready after c_ChangeDelay;
{{qualified_sm}}.rdata   <= v_{{name}}_rdata after c_ChangeDelay;
{{qualified_sm}}.rresp   <= v_{{name}}_rresp after c_ChangeDelay;
{{? type.x_tlast}}
{{qualified_sm}}.rlast   <= v_{{name}}_rlast after c_ChangeDelay;
{{/ type.x_tlast}}
{{? type.x_tid}}
{{qualified_sm}}.rid     <= v_{{name}}_rid after c_ChangeDelay;
{{/ type.x_tid}}
{{? type.x_truser}}
{{qualified_sm}}.ruser   <= {{type.x_truser.x_cnull.qualified}};
{{/ type.x_truser}}
{{qualified_sm}}.rvalid  <= v_{{name}}_rvalid after c_ChangeDelay;
{{/type.x_has_rd}}
