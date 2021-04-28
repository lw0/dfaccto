-- {{name}}
x_outputState(
  v_ref     => c_{{name}}_ref,
  v_output  => v_{{name}}_data);
{{qualified}} <= v_{{name}}_data after c_ChangeDelay;
