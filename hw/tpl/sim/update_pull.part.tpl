-- {{name}}
{{?type.x_tsdata}}
v_{{name}}_sdata := {{qualified_ms}}.sdata;
{{/type.x_tsdata}}
v_{{name}}_stb  := {{qualified_ms}}.stb;
x_pullUpdate(
  v_ref     => c_{{name}}_ref,
  v_sdata   => v_{{name}}_sdata,
  v_stb     => v_{{name}}_stb);
