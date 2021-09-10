Inc('dfaccto.py', abs='lib')
Inc('ocaccel.py', abs='lib')


with Pkg('user',
    x_templates={File('generic/package.vhd.tpl', mod='lib'): File('pkg/user.vhd')}):

  tHostStm = Axi.TypeStream('HostStream',
      data_bytes=OCAccel.Host_DataBytes)

OCAccel.EntCtrlDemux

OCAccel.EntControl

Axi.EntWiring('HostSplitter',
    master_type=OCAccel.tHost,
    master_mode='full',
    slave_type=OCAccel.tHost,
    slave_mode='wr_rd',
    x_file=File('HostSplitter.vhd'))

Axi.EntWrMux('HostWrMux',
    axi_type=OCAccel.tHost,
    x_file_split=File('HostWrMuxSplitter.vhd'),
    x_file_join=File('HostWrMuxJoiner.vhd'),
    x_file=File('HostWrMux.vhd'))

Axi.EntRdMux('HostRdMux',
    axi_type=OCAccel.tHost,
    x_file_split=File('HostRdMuxSplitter.vhd'),
    x_file_join=File('HostRdMuxJoiner.vhd'),
    x_file=File('HostRdMux.vhd'))

Axi.EntWriter('HostWriter',
    axi_type=OCAccel.tHost,
    stm_type=tHostStm,
    reg_type=OCAccel.tCtrlReg,
    x_file=File('HostWriter.vhd'))

Axi.EntReader('HostReader',
    axi_type=OCAccel.tHost,
    stm_type=tHostStm,
    reg_type=OCAccel.tCtrlReg,
    x_file=File('HostReader.vhd'))


with Ent('AxiTest',
    PortI('sys',  Util.tsys),
    PortM('intr', OCAccel.tIntr),
    PortS('ctrl', OCAccel.tCtrl),
    PortM('host', OCAccel.tHost),
    x_templates={File('generic/entity.vhd.tpl', mod='lib'): File('axitest.vhd')}):

  regs = Reg.Map(
      (0x000, 0x020, 'ctrlAct'), # Start-Ready Control
      (0x100, 0x010, 'ctrlRd0'), # Reader 0
      (0x110, 0x010, 'ctrlRd1'), # Reader 1
      (0x120, 0x010, 'ctrlRd2'), # Reader 2
      (0x130, 0x010, 'ctrlRd3'), # Reader 3
      (0x140, 0x010, 'ctrlWr0'), # Writer 0
      (0x150, 0x010, 'ctrlWr1'), # Writer 1
      (0x160, 0x010, 'ctrlWr2'), # Writer 2
      (0x170, 0x010, 'ctrlWr3')) # Writer 3
  Ins(OCAccel.EntCtrlDemux.name, None,
    MapGeneric('Ports', regs.LitPorts()),
    MapPort('sys', S('sys')),
    MapPort('axi', S('ctrl')),
    MapPort('ports', regs.SigPorts()))

  Ins(OCAccel.EntControl.name, None,
    MapGeneric('ActionType', Lit(0x7e57719e)),
    MapGeneric('ActionVersion', Lit(0x00000001)),
    MapPort('sys', S('sys')),
    MapPort('reg', S('ctrlAct')),
    MapPort('intr', S('intr')),
    MapPort('start', SV('startWr{:d}', 'startRd{:d}', expand=range(4))),
    MapPort('irq', SV()))

  Ins('HostSplitter', None,
    MapPort('sys', S('sys')),
    MapPort('master', S('host')),
    MapPort('slaveWr', S('hostWr')),
    MapPort('slaveRd', S('hostRd')))

  Ins('HostWrMux', None,
    MapPort('sys', S('sys')),
    MapPort('master', S('hostWr')),
    MapPort('slaves', SV('hostWr{:d}', expand=range(4))))

  Ins('HostRdMux', None,
    MapPort('sys', S('sys')),
    MapPort('master', S('hostRd')),
    MapPort('slaves', SV('hostRd{:d}', expand=range(4))))

  for i in range(4):
    Ins('HostReader', None,
      MapPort('sys', S('sys')),
      MapPort('reg', S('ctrlRd{:d}', expand=i)),
      MapPort('start', S('startRd{:d}', expand=i)),
      MapPort('axi', S('hostRd{:d}', expand=i)),
      MapPort('stm', S('stm{:d}', expand=i)))

    Ins('HostWriter', None,
      MapPort('sys', S('sys')),
      MapPort('reg', S('ctrlWr{:d}', expand=i)),
      MapPort('start', S('startWr{:d}', expand=i)),
      MapPort('axi', S('hostWr{:d}', expand=i)),
      MapPort('stm', S('stm{:d}', expand=i)))


