variable v_{{name}}_sdata : {{?type.x_tsdata}}{{type.x_tsdata.qualified}}{{|type.x_tsdata}}unsigned (-1 downto 0) := ""{{/type.x_tsdata}};
variable v_{{name}}_stb   : {{type.x_tlogic.qualified}};
variable v_{{name}}_adata : {{?type.x_tadata}}{{type.x_tadata.qualified}}{{|type.x_tadata}}unsigned (-1 downto 0) := ""{{/type.x_tadata}};
variable v_{{name}}_ack   : {{type.x_tlogic.qualified}};
