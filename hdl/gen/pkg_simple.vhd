library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


package simple is

  subtype t_Logic is std_logic;
subtype t_Logic_v is unsigned;


  -----------------------------------------------------------------------------
-- Unsigned Type: Data
-----------------------------------------------------------------------------
subtype t_Data is unsigned (32-1 downto 0);
type t_Data_v is array (integer range <>) of t_Data;


  subtype t_Handshake_ms is std_logic;
subtype t_Handshake_sm is std_logic;
subtype t_Handshake_v_ms is unsigned;
subtype t_Handshake_v_sm is unsigned;


end simple;
