-- {{name}}
v_{{name}}_tready := {{qualified_sm}}.tready;
x_sourceUpdate(
  v_ref     => c_{{name}}_ref,
  v_tready  => v_{{name}}_tready);
