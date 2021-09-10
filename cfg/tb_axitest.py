Inc('sim.py', abs='sim')

Inc('axitest.py')


with EntTestbench(
  SimPort.Sys('sys',     Util.tsys),
  SimPort.Pull('intr',   OCAccel.tIntr),
  SimPort.Master('ctrl', OCAccel.tCtrl),
  SimPort.Slave('host',  OCAccel.tHost)):

  Ins('AxiTest', None,
    MapPort('sys',  S('sys')),
    MapPort('intr', S('intr')),
    MapPort('ctrl', S('ctrl')),
    MapPort('host', S('host')))


