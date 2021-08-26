Inc('sim.py', abs='sim')

Inc('streamtest.py')


with EntTestbench(
    SimPort.Sys('sys', T('Sys', 'dfaccto')),
    SimPort.Source('stmSrc', T('LongStream')),
    SimPort.Sink('stmBufIn', T('LongStream')),
    SimPort.Source('stmBufOut', T('LongStream')),
    SimPort.Sink('stmSnk', T('LongStream'))):

  Ins('Streamtest', None,
    MapPort('sys', S('sys')),
    MapPort('stmSrc', S('stmSrc')),
    MapPort('stmBufIn', S('stmBufIn')),
    MapPort('stmBufOut', S('stmBufOut')),
    MapPort('stmSnk', S('stmSnk')))


