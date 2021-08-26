Inc('dfaccto.py', abs='lib')


with Pkg('user',
    x_templates={File('generic/package.vhd.tpl', mod='lib'): File('pkg/user.vhd')}):
  TypeEvent('ByteEvent', stb_bits=8)
  TypeUnsigned('Byte', width=8)

Ent('EventTest',
  PortI('sys',  T('Sys'),       label='psys'),
  PortI('valA', T('Byte'),      label='pvala'),
  PortI('valB', T('Byte'),      label='pvalb'),
  PortO('valC', T('Byte'),      label='pvalc'),
  PortS('evtA', T('ByteEvent'), label='pevta'),
  PortS('evtB', T('ByteEvent'), label='pevtb'),
  PortM('evtC', T('ByteEvent'), label='pevtc'),
  x_templates={File('eventtest.vhd.tpl'): File('eventtest.vhd')})


