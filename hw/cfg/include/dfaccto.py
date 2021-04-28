Inc('dfaccto/utils.py')


with Pkg('dfaccto',
         x_templates={'generic/package.vhd': 'pkg/dfaccto.vhd'}):

  TypeS('Bool',
        x_definition='{{>types/bool.part}}',
        x_format='{{>formats/bool.part}}',
        x_cnull=lambda t: Con('BoolNull', t, value=Lit(False)))

  TypeS('String',
        x_definition='{{>types/string.part}}',
        x_format='{{>formats/string.part}}',
        x_cnull=lambda t: Con('StringNull', t, value=Lit('')))

  TypeS('Time', # base unit nanoseconds
        x_definition='{{>types/time.part}}',
        x_format='{{>formats/time.part}}',
        x_cnull=lambda t: Con('TimeNull', t, value=Lit(0)))

  TypeS('Logic',
        x_definition='{{>types/logic.part}}',
        x_format='{{>formats/logic.part}}',
        x_cnull=lambda t: Con('LogicNull', t, value=Lit(False)))

  IntegerType('Integer')

  IntegerType('Size', min=0)

  TypeS('Sys', x_is_sys=True,
        x_definition='{{>types/sys.part}}',
        x_format='{{>formats/sys.part}}',
        x_tlogic=T('Logic', 'dfaccto'),
        x_cnull=lambda t: Con('SysNull', t, value=Lit({'clk': False, 'rst_n': False})))


Inc('dfaccto/axi.py')
Inc('dfaccto/event.py')

Inc('dfaccto/sim.py')


# p=Pkg('builtin')
# p.Typ('Logic',        Simple)
# p.Typ('Handshake',    Complex)
# p.Typ('Ctrl',         Complex)
# p.Typ('NativeAxi',    Complex)
# p.Typ('NativeAxiRd',  Complex)
# p.Typ('NativeAxiWr',  Complex)
# p.Typ('NativeStream', Complex)
# p.Typ('RegPort',      Complex)
# p.Typ('BlkMap',       Complex)
# p.Typ('RegData',      Simple, unsigned=True, width=32)

# Ent('AxiSplitter',
#       pm_axi='NativeAxi', ps_axiRd='NativeAxiRd', ps_axiWr='NativeAxiWr')

# Ent('AxiRdMultiplexer', g_PortCount=None, g_FIFOLogDepth=None,
#       pm_axiRd='NativeAxiRd', ps_axiRds=('NativeAxiRd', 'PortCount'))
# Ent('AxiWrMultiplexer', g_PortCount=None, g_FIFOLogDepth=None,
#       pm_axiWr='NativeAxiWr', ps_axiWrs=('NativeAxiWr', 'PortCount'))

# Ent('AxiMonitor', g_RdPortCount=None, g_WrPortCount=None, g_StmPortCount=None,
#       ps_regs='RegPort', pi_start='Logic',
#       pv_axiRd=('NativeAxiRd', 'RdPortCount'),
#       pv_axiWr=('NativeAxiWr', 'WrPortCount'),
#       pv_stream=('NativeStream', 'StmPortCount'))

# Ent('AxiReader', g_FIFOLogDepth=None,
#       pi_start='Logic', po_ready='Logic', pi_hold='Logic',
#       pm_axiRd='NativeAxiRd', pm_stm='NativeStream',
#       ps_regs='RegPort')
# Ent('AxiWriter', g_FIFOLogDepth=None,
#       pi_start='Logic', po_ready='Logic', pi_hold='Logic',
#       ps_stm='NativeStream', pm_axiWr='NativeAxiWr',
#       ps_regs='RegPort')

# Ent('AxiRdBlockMapper',
#       pm_map='BlkMap',
#       ps_axiLog='NativeAxiRd', pm_axiPhy='NativeAxiRd')
# Ent('AxiWrBlockMapper',
#       pm_map='BlkMap',
#       ps_axiLog='NativeAxiWr', pm_axiPhy='NativeAxiWr')

# Ent('ExtentStore', g_PortCount=None,
#       ps_ports=('BlkMap', 'PortCount'),
#       ps_regs='RegPort', pm_int='Handshake')

# Ent('RegisterFile', g_RegCount=None,
#       pi_regRd=('RegData', 'RegCount'), po_regWr=('RegData', 'RegCount'),
#       po_eventRd=('Logic', 'RegCount'), po_eventWr=('Logic', 'RegCount'),
#       po_eventRdAny='Logic', po_eventWrAny='Logic',
#       ps_regs='RegPort')

# Ent('ActionControl', g_PortCount=None, g_ReadyCount=None, g_ActionType=None, g_ActionRev=None,
#     ps_ctrl='Ctrl',
#     pm_regs=('RegPort', 'PortCount'),
#     po_start='Logic',
#     pi_ready=('Logic', 'ReadyCount'))


