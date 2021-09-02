Inc('dfaccto.py', abs='lib')


with Pkg('user',
    x_templates={File('generic/package.vhd.tpl', mod='lib'): File('pkg/user.vhd')}):

  TypeEvent('IntrEvent', stb_bits=4)

  TypeAxi('CtrlAxi', data_bytes=4, addr_bits=12, has_burst=False)
  TypeRegPort('CtrlReg', data_bytes=4, addr_bits=12)

  TypeAxi('MemAxiExt', data_bytes=64, addr_bits=64, id_bits=4, has_attr=True,
      aruser_bits=1, awuser_bits=1, ruser_bits=1, wuser_bits=1, buser_bits=1)
  TypeAxi('MemAxi', data_bytes=64, addr_bits=64)
  TypeAxiStream('MemStream', data_bytes=64)

tIntr=T('IntrEvent')
tCtrlAxi=T('CtrlAxi')
tCtrlReg=T('CtrlReg')
tMemAxiE=T('MemAxiExt')
tMemAxi=T('MemAxi')
tMemStm=T('MemStream')

EntRegDemux('CtrlDemux',
    axi_type=tCtrlAxi, reg_type=tCtrlReg,
    x_file=File('CtrlDemux.vhd'))
EntAxiWiring('MemSplitter',
    master_type=tMemAxiE, slave_type=tMemAxi,
    master_mode='full', slave_mode='wr_rd',
    x_file=File('MemSplitter.vhd'))
EntAxiWrMux('MemWrMux',
    axi_type=tMemAxi,
    x_file_split=File('MemWrMuxSplitter.vhd'),
    x_file_join=File('MemWrMuxJoiner.vhd'),
    x_file=File('MemWrMux.vhd'))
EntAxiRdMux('MemRdMux',
    axi_type=tMemAxi,
    x_file_split=File('MemRdMuxSplitter.vhd'),
    x_file_join=File('MemRdMuxJoiner.vhd'),
    x_file=File('MemRdMux.vhd'))
EntAxiWriter('MemWriter',
    axi_type=tMemAxi, stm_type=tMemStm, reg_type=tCtrlReg,
    x_file_amach=File('MemWriter_AMach.vhd'),
    x_file=File('MemWriter.vhd'))
EntAxiReader('MemReader',
    axi_type=tMemAxi, stm_type=tMemStm, reg_type=tCtrlReg,
    x_file_amach=File('MemReader_AMach.vhd'),
    x_file=File('MemReader.vhd'))


with Ent('AxiTest',
    PortI('sys',  T('Sys')),
    PortM('intr', tIntr),
    PortS('ctrl', tCtrlAxi),
    PortM('mem',  tMemAxiE),
    x_templates={File('generic/entity.vhd.tpl', mod='lib'): File('axitest.vhd')}):

  regs = RegMap(
      RegEntry(0x000, 0x010, 'ctrlAct'), # Start-Ready Control
      RegEntry(0x100, 0x010, 'ctrlRd0'), # Reader 0
      RegEntry(0x110, 0x010, 'ctrlRd1'), # Reader 1
      RegEntry(0x120, 0x010, 'ctrlRd2'), # Reader 2
      RegEntry(0x130, 0x010, 'ctrlRd3'), # Reader 3
      RegEntry(0x180, 0x010, 'ctrlWr0'), # Writer 0
      RegEntry(0x190, 0x010, 'ctrlWr1'), # Writer 1
      RegEntry(0x1a0, 0x010, 'ctrlWr2'), # Writer 2
      RegEntry(0x1b0, 0x010, 'ctrlWr3'), # Writer 3
      RegEntry(0x200, 0x100, 'ctrlMon')) # Monitor
  Ins('CtrlDemux', None,
    MapGeneric('Ports', regs.LitPorts()),
    MapPort('sys', S('sys')),
    MapPort('axi', S('ctrl')),
    MapPort('ports', regs.SigPorts()))

  Ins('MemSplitter', None,
    MapPort('sys', S('sys')),
    MapPort('master', S('mem')),
    MapPort('slaveWr', S('memWr')),
    MapPort('slaveRd', S('memRd')))

  Ins('MemWrMux', None,
    MapPort('sys', S('sys')),
    MapPort('master', S('memWr')),
    MapPort('slaves', SV('memWr{:d}', expand=range(4))))

  Ins('MemRdMux', None,
    MapPort('sys', S('sys')),
    MapPort('master', S('memRd')),
    MapPort('slaves', SV('memRd{:d}', expand=range(4))))

  for i in range(4):

    Ins('MemReader', None,
      MapPort('sys', S('sys')),
      MapPort('axiRd', S('axiRd{:d}', expand=i)),
      MapPort('stm', S('stm{:d}', expand=i)))

    Ins('MemWriter', None,
      MapPort('sys', S('sys')),
      MapPort('stm', S('stm{:d}', expand=i)),
      MapPort('axiWr', S('axiWr{:d}', expand=i)))


