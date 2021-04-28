-- {{name}}
x_pullState(
  v_ref     => c_{{name}}_ref,
  v_adata   => v_{{name}}_adata,
  v_ack     => v_{{name}}_ack);
{{?type.x_tadata}}
{{qualified_sm}}.adata <= v_{{name}}_adata after c_ChangeDelay;
{{/type.x_tadata}}
{{qualified_sm}}.ack   <= v_{{name}}_ack after c_ChangeDelay;
