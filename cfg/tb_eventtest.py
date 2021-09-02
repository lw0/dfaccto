Inc('sim.py', abs='sim')

Inc('eventtest.py')


with EntTestbench(
  SimPort.Sys('sys',     T('Sys')),
  SimPort.Output('valA', T('Byte')),
  SimPort.Output('valB', T('Byte')),
  SimPort.Input('valC',  T('Byte')),
  SimPort.Push('evtA',   T('ByteEvent')),
  SimPort.Push('evtB',   T('ByteEvent')),
  SimPort.Pull('evtC',   T('ByteEvent'))):

  Ins('EventTestIMap', None,
    MapPort('sys',  S('sys')),
    MapPort('valA', S('valA')),
    MapPort('valB', S('valB')),
    MapPort('valC', S('valC')),
    MapPort('evtA', S('evtA')),
    MapPort('evtB', S('evtB')),
    MapPort('evtC', S('evtC')))


