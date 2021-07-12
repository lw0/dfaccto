Inc('include/dfaccto.py')


with Pkg('user', x_templates={'generic/package.vhd': 'pkg/user.vhd'}):

  EventType('ByteEvent', stb_bits=8)

  UnsignedType('Byte', width=8)

  AxiStreamType('LongStream', data_bytes=8, id_bits=2)


EntSimEnv('Environment', 'Config', 'Period', 'sys',
  SimEnv.Source('stmSrc',    T('LongStream')),
  SimEnv.Sink('stmBufIn',    T('LongStream')),
  SimEnv.Source('stmBufOut', T('LongStream')),
  SimEnv.Sink('stmSnk',      T('LongStream')))


with Ent('Testbench', x_templates={'generic/entity.vhd': 'Testbench.vhd'}):

  Ins('Environment', None,
    MapGeneric('Config', Lit('tb/teststream.lua')),
    MapGeneric('Period', Lit(4)), # 4ns ~ 200MHz
    MapPort('sys',       S('sys')),
    MapPort('stmSrc',    S('stmA')),
    MapPort('stmBufIn',  S('stmA')),
    MapPort('stmBufOut', S('stmB')),
    MapPort('stmSnk',    S('stmB')))


