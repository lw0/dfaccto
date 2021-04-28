variable v_{{name}}_tdata  : {{type.x_tdata.qualified}};
variable v_{{name}}_tkeep  : {{type.x_tkeep.qualified}};
variable v_{{name}}_tid    : {{?type.x_tid}}{{type.x_tid.qualified}}{{|type.x_tid}}unsigned (-1 downto 0) := ""{{/type.x_tid}};
variable v_{{name}}_tlast  : {{type.x_tlogic.qualified}};
variable v_{{name}}_tvalid : {{type.x_tlogic.qualified}};
variable v_{{name}}_tready : {{type.x_tlogic.qualified}};
