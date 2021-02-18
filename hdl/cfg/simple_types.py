p=Pkg('simple',
      x_templates={'generic/package.vhd': 'pkg_simple.vhd'})

p.Typ('Logic', Simple,
      x_definition='{{>types/logic.part}}')

p.Typ('Data', Simple,
      x_width=32,
      x_definition='{{>types/unsigned.part}}')

p.Typ('Handshake', Complex,
      x_definition='{{>types/handshake.part}}')
