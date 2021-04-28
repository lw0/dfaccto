-- {{name}}
{{?type.x_has_wr}}
x_masterWrState(
  v_ref     => c_{{name}}_ref,
  v_awaddr  => v_{{name}}_awaddr,
  v_awlen   => v_{{name}}_awlen,
  v_awsize  => v_{{name}}_awsize,
  v_awburst => v_{{name}}_awburst,
  v_awid    => v_{{name}}_awid,
  v_awvalid => v_{{name}}_awvalid,
  v_wdata   => v_{{name}}_wdata,
  v_wstrb   => v_{{name}}_wstrb,
  v_wlast   => v_{{name}}_wlast,
  v_wvalid  => v_{{name}}_wvalid,
  v_bready  => v_{{name}}_bready);
{{qualified_ms}}.awaddr   <= v_{{name}}_awaddr after c_ChangeDelay;
{{? type.x_tlen}}
{{qualified_ms}}.awlen    <= v_{{name}}_awlen after c_ChangeDelay;
{{/ type.x_tlen}}
{{? type.x_tsize}}
{{qualified_ms}}.awsize   <= v_{{name}}_awsize after c_ChangeDelay;
{{/ type.x_tsize}}
{{? type.x_tburst}}
{{qualified_ms}}.awburst  <= v_{{name}}_awburst after c_ChangeDelay;
{{/ type.x_tburst}}
{{? type.x_tlock}}
{{qualified_ms}}.awlock   <= {{type.x_tlock.x_cnull.qualified}};
{{/ type.x_tlock}}
{{? type.x_tcache}}
{{qualified_ms}}.awcache  <= {{type.x_tcache.x_cnull.qualified}};
{{/ type.x_tcache}}
{{? type.x_tprot}}
{{qualified_ms}}.awprot   <= {{type.x_tprot.x_cnull.qualified}};
{{/ type.x_tprot}}
{{? type.x_tqos}}
{{qualified_ms}}.awqos    <= {{type.x_tqos.x_cnull.qualified}};
{{/ type.x_tqos}}
{{? type.x_tregion}}
{{qualified_ms}}.awregion <= {{type.x_tregion.x_cnull.qualified}};
{{/ type.x_tregion}}
{{? type.x_tid}}
{{qualified_ms}}.awid     <= v_{{name}}_awid after c_ChangeDelay;
{{/ type.x_tid}}
{{? type.x_tawuser}}
{{qualified_ms}}.awuser   <= {{type.x_tawuser.x_cnull.qualified}};
{{/ type.x_tawuser}}
{{qualified_ms}}.awvalid  <= v_{{name}}_awvalid after c_ChangeDelay;
{{qualified_ms}}.wdata    <= v_{{name}}_wdata after c_ChangeDelay;
{{qualified_ms}}.wstrb    <= v_{{name}}_wstrb after c_ChangeDelay;
{{? type.x_tlast}}
{{qualified_ms}}.wlast    <= v_{{name}}_wlast after c_ChangeDelay;
{{/ type.x_tlast}}
{{? type.x_twuser}}
{{qualified_ms}}.wuser    <= {{type.x_twuser.x_cnull.qualified}};
{{/ type.x_twuser}}
{{qualified_ms}}.wvalid   <= v_{{name}}_wvalid after c_ChangeDelay;
{{qualified_ms}}.bready   <= v_{{name}}_bready after c_ChangeDelay;
{{/type.x_has_wr}}
{{?type.x_has_rd}}
x_masterRdState(
  v_ref     => c_{{name}}_ref,
  v_araddr  => v_{{name}}_araddr,
  v_arlen   => v_{{name}}_arlen,
  v_arsize  => v_{{name}}_arsize,
  v_arburst => v_{{name}}_arburst,
  v_arid    => v_{{name}}_arid,
  v_arvalid => v_{{name}}_arvalid,
  v_rready  => v_{{name}}_rready);
{{qualified_ms}}.araddr   <= v_{{name}}_araddr after c_ChangeDelay;
{{? type.x_tlen}}
{{qualified_ms}}.arlen    <= v_{{name}}_arlen after c_ChangeDelay;
{{/ type.x_tlen}}
{{? type.x_tsize}}
{{qualified_ms}}.arsize   <= v_{{name}}_arsize after c_ChangeDelay;
{{/ type.x_tsize}}
{{? type.x_tburst}}
{{qualified_ms}}.arburst  <= v_{{name}}_arburst after c_ChangeDelay;
{{/ type.x_tburst}}
{{? type.x_tlock}}
{{qualified_ms}}.arlock   <= {{type.x_tlock.x_cnull.qualified}};
{{/ type.x_tlock}}
{{? type.x_tcache}}
{{qualified_ms}}.arcache  <= {{type.x_tcache.x_cnull.qualified}};
{{/ type.x_tcache}}
{{? type.x_tprot}}
{{qualified_ms}}.arprot   <= {{type.x_tprot.x_cnull.qualified}};
{{/ type.x_tprot}}
{{? type.x_tqos}}
{{qualified_ms}}.arqos    <= {{type.x_tqos.x_cnull.qualified}};
{{/ type.x_tqos}}
{{? type.x_tregion}}
{{qualified_ms}}.arregion <= {{type.x_tregion.x_cnull.qualified}};
{{/ type.x_tregion}}
{{? type.x_tid}}
{{qualified_ms}}.arid     <= v_{{name}}_arid after c_ChangeDelay;
{{/ type.x_tid}}
{{? type.x_taruser}}
{{qualified_ms}}.aruser   <= {{type.x_taruser.x_cnull.qualified}};
{{/ type.x_taruser}}
{{qualified_ms}}.arvalid  <= v_{{name}}_arvalid after c_ChangeDelay;
{{qualified_ms}}.rready   <= v_{{name}}_rready after c_ChangeDelay;
{{/type.x_has_rd}}
