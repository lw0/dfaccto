-------------------------------------------------------------------------------
-- Axi Stream Type: {{name}}
-------------------------------------------------------------------------------
type {{identifier_ms}} is record
  tdata   : unsigned ({{x_datawidth}}-1 downto 0);
  tkeep   : unsigned ({{x_datawidth}}/8-1 downto 0);
{{#x_idwidth}}
  tid     : unsigned ({{x_idwidth}}-1 downto 0);
{{/x_idwidth}}
{{#x_userwidth}}
  tuser   : unsigned ({{x_userwidth}}-1 downto 0);
{{/x_userwidth}}
  tlast   : std_logic;
  tvalid  : std_logic;
end record;
type {{identifier_sm}} is record
  tready  : std_logic;
end record;
constant {{const_null_ms}} : {{identifier_ms}} := (
  tdata  => (others => '0'),
  tkeep  => (others => '0'),
{{#x_idwidth}}
  tid    => (others => '0'),
{{/x_idwidth}}
{{#x_userwidth}}
  tuser  => (others => '0'),
{{/x_userwidth}}
  tlast  => '0',
  tvalid => '0');
constant {{const_null_sm}} : {{identifier_sm}} := (
  tready => '0');
type {{identifier_v_ms}} is array (integer range <>) of {{identifier_ms}};
type {{identifier_v_sm}} is array (integer range <>) of {{identifier_sm}};
