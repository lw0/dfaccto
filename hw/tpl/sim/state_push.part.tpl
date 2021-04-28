-- {{name}}
x_pushState(
  v_ref     => c_{{name}}_ref,
  v_sdata   => v_{{name}}_sdata,
  v_stb     => v_{{name}}_stb);
{{?type.x_tsdata}}
{{qualified_ms}}.sdata <= v_{{name}}_sdata after c_ChangeDelay;
{{/type.x_tsdata}}
{{qualified_ms}}.stb  <= v_{{name}}_stb after c_ChangeDelay;
