-- {{name}}
v_{{name}}_tdata  := {{qualified_ms}}.tdata;
v_{{name}}_tkeep  := {{qualified_ms}}.tkeep;
{{?type.x_tid}}
v_{{name}}_tid    := {{qualified_ms}}.tid;
{{/type.x_tid}}
v_{{name}}_tlast  := {{qualified_ms}}.tlast;
v_{{name}}_tvalid := {{qualified_ms}}.tvalid;
x_sinkUpdate(
  v_ref     => c_{{name}}_ref,
  v_tdata   => v_{{name}}_tdata,
  v_tkeep   => v_{{name}}_tkeep,
  v_tid     => v_{{name}}_tid,
  v_tlast   => v_{{name}}_tlast,
  v_tvalid  => v_{{name}}_tvalid);
