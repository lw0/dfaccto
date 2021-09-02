Inc('sim.py', abs='sim')

Inc('axitest.py')


with EntTestbench(
  SimPort.Sys('sys',     T('Sys')),
  SimPort.Pull('intr',   T('IntrEvent')),
  SimPort.Master('ctrl', T('CtrlAxi')),
  SimPort.Slave('mem',   T('MemAxiExt'))):

  Ins('AxiTest', None,
    MapPort('sys',  S('sys')),
    MapPort('intr', S('intr')),
    MapPort('ctrl', S('ctrl')),
    MapPort('mem',  S('mem')))


