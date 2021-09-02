Inc('dfaccto.py', abs='lib')


with Pkg('user',
    x_templates={File('generic/package.vhd.tpl', mod='lib'): File('pkg/user.vhd')}):
  TypeEvent('ByteEvent', stb_bits=8)
  TypeUnsigned('Byte', width=8)

Ent('EventTestInner',
    PortI('sys',  T('Sys'),       label='psys'),
    PortI('valA', T('Byte'),      label='pvala'),
    PortI('valB', T('Byte'),      label='pvalb'),
    PortO('valC', T('Byte'),      label='pvalc'),
    PortS('evtA', T('ByteEvent'), label='pevta'),
    PortS('evtB', T('ByteEvent'), label='pevtb'),
    PortM('evtC', T('ByteEvent'), label='pevtc'),
    x_templates={File('eventtest.vhd.tpl'): File('eventtestinner.vhd')})

with Ent('EventTestEMap',
    PortI('sys',  T('Sys'),       x_wrapname='ext_sys'),
    PortI('valA', T('Byte'),      x_wrapname='ext_vala'),
    PortI('valB', T('Byte'),      x_wrapname='ext_valb'),
    PortO('valC', T('Byte'),      x_wrapname='ext_valc'),
    PortS('evtA', T('ByteEvent'), x_wrapname='ext_evta'),
    PortS('evtB', T('ByteEvent'), x_wrapname='ext_evtb'),
    PortM('evtC', T('ByteEvent'), x_wrapname='ext_evtc'),
    x_templates={File('generic/ext_wrapper.vhd.tpl', mod='lib'): File('eventtestext.vhd')}):

  Ins('EventTestInner', None,
    MapPort('sys',  S('sys')),
    MapPort('valA', S('valA')),
    MapPort('valB', S('valB')),
    MapPort('valC', S('valC')),
    MapPort('evtA', S('evtA')),
    MapPort('evtB', S('evtB')),
    MapPort('evtC', S('evtC')))

Ent('EventTestIMap',
    PortI('sys',  T('Sys'),       x_wrapname='ext_sys'),
    PortI('valA', T('Byte'),      x_wrapname='ext_vala'),
    PortI('valB', T('Byte'),      x_wrapname='ext_valb'),
    PortO('valC', T('Byte'),      x_wrapname='ext_valc'),
    PortS('evtA', T('ByteEvent'), x_wrapname='ext_evta'),
    PortS('evtB', T('ByteEvent'), x_wrapname='ext_evtb'),
    PortM('evtC', T('ByteEvent'), x_wrapname='ext_evtc'),
    x_wrapname='EventTestEMap',
    x_templates={File('generic/int_wrapper.vhd.tpl', mod='lib'): File('eventtestint.vhd')})


