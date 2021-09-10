Inc('ocaccel.py', abs='lib')

Inc('axitest.py')



with OCAccel.EntWrapper('OCAccelWrapper',
    single_context=True,
    x_file=File('OCAccelWrapper.vhd')):

  Ins('AxiTest', None,
      MapPort('sys', S('sys')),
      MapPort('intr', S('intr')),
      MapPort('ctrl', S('ctrl')),
      MapPort('host', S('host')))
