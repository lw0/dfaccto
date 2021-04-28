-- {{name}}
x_sinkState(
  v_ref     => c_{{name}}_ref,
  v_tready  => v_{{name}}_tready);
{{qualified_sm}}.tready <= v_{{name}}_tready after c_ChangeDelay;
