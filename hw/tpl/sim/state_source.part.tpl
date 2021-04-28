-- {{name}}
x_sourceState(
  v_ref     => c_{{name}}_ref,
  v_tdata   => v_{{name}}_tdata,
  v_tkeep   => v_{{name}}_tkeep,
  v_tid     => v_{{name}}_tid,
  v_tlast   => v_{{name}}_tlast,
  v_tvalid  => v_{{name}}_tvalid);
{{qualified_ms}}.tdata  <= v_{{name}}_tdata after c_ChangeDelay;
{{qualified_ms}}.tkeep  <= v_{{name}}_tkeep after c_ChangeDelay;
{{? type.x_tid}}
{{qualified_ms}}.tid    <= v_{{name}}_tid after c_ChangeDelay;
{{/ type.x_tid}}
{{? type.x_tuser}}
{{qualified_ms}}.tuser  <= {{type.x_tuser.x_cnull.qualified}};
{{/ type.x_tuser}}
{{qualified_ms}}.tlast  <= v_{{name}}_tlast after c_ChangeDelay;
{{qualified_ms}}.tvalid <= v_{{name}}_tvalid after c_ChangeDelay;
