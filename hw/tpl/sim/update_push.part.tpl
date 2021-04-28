-- {{name}}
{{?type.x_tadata}}
v_{{name}}_adata := {{qualified_sm}}.adata;
{{/type.x_tadata}}
v_{{name}}_ack   := {{qualified_sm}}.ack;
x_pushUpdate(
  v_ref     => c_{{name}}_ref,
  v_adata   => v_{{name}}_adata,
  v_ack     => v_{{name}}_ack);
