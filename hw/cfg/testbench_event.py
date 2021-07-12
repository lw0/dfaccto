Inc('include/dfaccto.py')


with Pkg('user', x_templates={'generic/package.vhd': 'pkg/user.vhd'}):

  EventType('ByteEvent', stb_bits=8)

  UnsignedType('Byte', width=8)


EntSimEnv('Environment', 'Config', 'Period', 'sys',
  SimEnv.Output('valA', T('Byte')),
  SimEnv.Output('valB', T('Byte')),
  SimEnv.Input('valC',  T('Byte')),
  SimEnv.Push('evtA',   T('ByteEvent')),
  SimEnv.Push('evtB',   T('ByteEvent')),
  SimEnv.Pull('evtC',   T('ByteEvent')))

Ent('UnitUnderTest',
  PortI('sys',  T('Sys')),
  PortI('valA', T('Byte')),
  PortI('valB', T('Byte')),
  PortO('valC', T('Byte')),
  PortS('evtA', T('ByteEvent')),
  PortS('evtB', T('ByteEvent')),
  PortM('evtC', T('ByteEvent')))


with Ent('Testbench', x_templates={'generic/entity.vhd': 'Testbench.vhd'}):

  Ins('Environment', None,
    MapGeneric('Config', Lit('tb/testevent.lua')),
    MapGeneric('Period', Lit(4)), # 4ns ~ 200MHz
    MapPort('sys',  S('sys')),
    MapPort('valA', S('valA')),
    MapPort('valB', S('valB')),
    MapPort('valC', S('valC')),
    MapPort('evtA', S('evtA')),
    MapPort('evtB', S('evtB')),
    MapPort('evtC', S('evtC')))

  Ins('UnitUnderTest', None,
    MapPort('sys',  S('sys')),
    MapPort('valA', S('valA')),
    MapPort('valB', S('valB')),
    MapPort('valC', S('valC')),
    MapPort('evtA', S('evtA')),
    MapPort('evtB', S('evtB')),
    MapPort('evtC', S('evtC')))


