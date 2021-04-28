-- {{name}}
{{?type.x_has_wr}}
v_{{name}}_awaddr  := {{qualified_ms}}.awaddr;
{{? type.x_tlen}}
v_{{name}}_awlen   := {{qualified_ms}}.awlen;
{{/ type.x_tlen}}
{{? type.x_tsize}}
v_{{name}}_awsize  := {{qualified_ms}}.awsize;
{{/ type.x_tsize}}
{{? type.x_tburst}}
v_{{name}}_awburst := {{qualified_ms}}.awburst;
{{/ type.x_tburst}}
{{? type.x_tid}}
v_{{name}}_awid    := {{qualified_ms}}.awid;
{{/ type.x_tid}}
v_{{name}}_awvalid := {{qualified_ms}}.awvalid;
v_{{name}}_wdata   := {{qualified_ms}}.wdata;
v_{{name}}_wstrb   := {{qualified_ms}}.wstrb;
{{? type.x_tlast}}
v_{{name}}_wlast   := {{qualified_ms}}.wlast;
{{/ type.x_tlast}}
v_{{name}}_wvalid  := {{qualified_ms}}.wvalid;
v_{{name}}_bready  := {{qualified_ms}}.bready;
x_slaveWrUpdate(
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
{{/type.x_has_wr}}
{{?type.x_has_rd}}
v_{{name}}_araddr  := {{qualified_ms}}.araddr;
{{? type.x_tlen}}
v_{{name}}_arlen   := {{qualified_ms}}.arlen;
{{/ type.x_tlen}}
{{? type.x_tsize}}
v_{{name}}_arsize  := {{qualified_ms}}.arsize;
{{/ type.x_tsize}}
{{? type.x_tburst}}
v_{{name}}_arburst := {{qualified_ms}}.arburst;
{{/ type.x_tburst}}
{{? type.x_tid}}
v_{{name}}_arid    := {{qualified_ms}}.arid;
{{/ type.x_tid}}
v_{{name}}_arvalid := {{qualified_ms}}.arvalid;
v_{{name}}_rready  := {{qualified_ms}}.rready;
x_slaveRdUpdate(
  v_ref     => c_{{name}}_ref,
  v_araddr  => v_{{name}}_araddr,
  v_arlen   => v_{{name}}_arlen,
  v_arsize  => v_{{name}}_arsize,
  v_arburst => v_{{name}}_arburst,
  v_arid    => v_{{name}}_arid,
  v_arvalid => v_{{name}}_arvalid,
  v_rready  => v_{{name}}_rready);
{{/type.x_has_rd}}
