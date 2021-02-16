p=Pkg('builtin')
p.Typ('Logic',       Simple)
p.Typ('Handshake',   Complex)
p.Typ('Ctrl',        Complex)
p.Typ('NativeAxi',   Complex)
p.Typ('NativeAxiRd', Complex)
p.Typ('NativeAxiWr', Complex)
p.Typ('RegPort',     Complex)
p.Typ('BlkMap',      Complex)
p.Typ('RegData',     Simple, unsigned=True, width=32)

Ent('AxiSplitter',
      pm_axi='NativeAxi', ps_axiRd='NativeAxiRd', ps_axiWr='NativeAxiWr')

Ent('AxiRdMultiplexer', g_PortCount=None, g_FIFOLogDepth=None,
      pm_axiRd='NativeAxiRd', ps_axiRds=('NativeAxiRd', 'PortCount'))
Ent('AxiWrMultiplexer', g_PortCount=None, g_FIFOLogDepth=None,
      pm_axiWr='NativeAxiWr', ps_axiWrs=('NativeAxiWr', 'PortCount'))

Ent('AxiMonitor', g_RdPortCount=None, g_WrPortCount=None, g_StmPortCount=None,
      ps_regs='RegPort', pi_start='Logic',
      pv_axiRd=('NativeAxiRd', 'RdPortCount'),
      pv_axiWr=('NativeAxiWr', 'WrPortCount'),
      pv_stream=('NativeStream', 'StmPortCount'))

Ent('AxiReader', g_FIFOLogDepth=None,
      pi_start='Logic', po_ready='Logic', pi_hold='Logic',
      pm_axiRd='NativeAxiRd', pm_stm='NativeStream',
      ps_regs='RegPort')
Ent('AxiWriter', g_FIFOLogDepth=None,
      pi_start='Logic', po_ready='Logic', pi_hold='Logic',
      ps_stm='NativeStream', pm_axiWr='NativeAxiWr',
      ps_regs='RegPort')

Ent('AxiRdBlockMapper',
      pm_map='BlkMap',
      ps_axiLog='NativeAxiRd', pm_axiPhy='NativeAxiRd')
Ent('AxiWrBlockMapper',
      pm_map='BlkMap',
      ps_axiLog='NativeAxiWr', pm_axiPhy='NativeAxiWr')

Ent('ExtentStore', g_PortCount=None,
      ps_ports=('BlkMap', 'PortCount'),
      ps_regs='RegPort', pm_int='Handshake')

Ent('RegisterFile', g_RegCount=None,
      pi_regRd=('RegData', 'RegCount'), po_regWr=('RegData', 'RegCount'),
      po_eventRd=('Logic', 'RegCount'), po_eventWr=('Logic', 'RegCount'),
      po_eventRdAny='Logic', po_eventWrAny='Logic',
      ps_regs='RegPort')

Ent('NativeStreamSource',
      pi_start='Logic', po_ready='Logic',
      pm_stm='NativeStream',
      ps_regs='RegPort',
      x_template='StreamSource.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamSource.vhd')
Ent('NativeStreamSink',
      pi_start='Logic', po_ready='Logic',
      ps_stm='NativeStream',
      ps_regs='RegPort',
      x_template='StreamSink.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamSink.vhd')
Ent('NativeStreamInfiniteSource',
      pm_stm='NativeStream',
      x_template='StreamInfiniteSource.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamInfiniteSource.vhd')
Ent('NativeStreamInfiniteSink',
      ps_stm='NativeStream',
      x_template='StreamInfiniteSink.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamInfiniteSink.vhd')

Ent('NativeStreamMultiplier', g_PortCount=None,
      ps_stm='NativeStream', pm_stms=('NativeStream', 'PortCount'),
      x_template='StreamMultiplier.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamMultiplier.vhd')
Ent('NativeStreamBuffer',
        g_LogDepth=None, g_OmitKeep=None,
        g_InThreshold=None, g_OutThreshold=None,
      ps_stmIn='NativeStream', pm_stmOut='NativeStream',
      po_inEnable='Logic', po_inHold='Logic',
      po_outEnable='Logic', po_outHold='Logic',
      x_template='StreamBuffer.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamBuffer.vhd')
Ent('NativeStreamSwitch', g_InPortCount=None, g_OutPortCount=None,
      ps_regs='RegPort',
      ps_stmIn=('NativeStream', 'InPortCount'),
      pm_stmOut=('NativeStream', 'OutPortCount'),
      x_template='StreamSwitch.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamSwitch.vhd')
Ent('NativeStreamRouter', g_InPortCount=None, g_OutPortCount=None,
      ps_regs='RegPort',
      ps_stmIn=('NativeStream', 'InPortCount'),
      pm_stmOut=('NativeStream', 'OutPortCount'),
      x_template='StreamRouter.vhd', xt_type='NativeStream',
      x_outfile='NativeStreamRouter.vhd')

Ent('ActionControl', g_PortCount=None, g_ReadyCount=None, g_ActionType=None, g_ActionRev=None,
    ps_ctrl='Ctrl',
    pm_regs=('RegPort', 'PortCount'),
    po_start='Logic',
    pi_ready=('Logic', 'ReadyCount'))

Typ('MaskStream', Role.Complex, x_user=True, x_stream=True, x_datawidth=16)

Ent('MaskStreamRouter', g_InPortCount=None, g_OutPortCount=None,
    ps_regs='RegPort',
    ps_stmIn=('MaskStream', 'InPortCount'),
    pm_stmOut=('MaskStream', 'OutPortCount'),
    x_template='StreamRouter.vhd', x_outfile='MaskStreamRouter.vhd', xt_type='MaskStream')

Ent('HLSFilter',
    ps_stmIn='NativeStream', ps_stmRef='NativeStream', pm_stmOut= 'MaskStream',
    pi_regLConst='RegData', pi_regRConst='RegData', pi_regMode='RegData',
    x_template='HLSWrapper.vhd', x_outfile='HLSFilter.vhd', x_hls_name='hls_filter')


### actual design

act = Ent('Toplevel',
          ps_ctrl='Ctrl',
          pm_hmem='NativeAxi',
          pm_cmem='NativeAxi',
          x_template='Action.vhd',
          x_outfile='Toplevel.vhd')

act.Inst('ActionControl',
        p_ctrl='ctrl',
        p_regs=('regExtStore', 'regSwitch', 'regHRd', 'regHWr', 'regCRd', 'regCWr'),
        p_start='start',
        p_ready=('readyHRd', 'readyHWr', 'readyCRd', 'readyCWr'))

act.Inst('AxiSplitter',
        p_axi='hmem', p_axiRd='hmemRd', p_axiWr='hmemWr')
act.Inst('AxiSplitter',
        p_axi='cmem', p_axiRd='cmemRd', p_axiWr='cmemWr')

act.Inst('AxiRdBlockMapper',
        p_map='extmapHRd',
        p_axiLog='hmemRdLog', p_axiPhy='hmemRd')
act.Inst('AxiWrBlockMapper',
        p_map='extmapHWr',
        p_axiLog='hmemWrLog', p_axiPhy='hmemWr')
act.Inst('AxiRdBlockMapper',
        p_map='extmapCRd',
        p_axiLog='cmemRdLog', p_axiPhy='cmemRd')
act.Inst('AxiWrBlockMapper',
        p_map='extmapCWr',
        p_axiLog='cmemWrLog', p_axiPhy='cmemWr')

act.Inst('ExtentStore',
        p_regs='regExtStore',
        p_ports=['extmapHRd', 'extmapHWr', 'extmapCRd', 'extmapCWr'])

act.Inst('AxiReader', g_FIFOLogDepth=3,
        p_regs='regHRd',
        p_start='start', p_ready='readyHRd',
        p_axiRd='hmemRdLog', p_stm='stmHRd')
act.Inst('AxiWriter', g_FIFOLogDepth=1,
        p_regs='regHWr',
        p_start='start', p_ready='readyHWr',
        p_axiWr='hmemWrLog', p_stm='stmHWr')
act.Inst('AxiReader', g_FIFOLogDepth=8,
        p_regs='regCRd',
        p_start='start', p_ready='readyCRd',
        p_axiRd='cmemRdLog', p_stm='stmCRd')
act.Inst('AxiWriter', g_FIFOLogDepth=1,
        p_regs='regCWr',
        p_start='start', p_ready='readyCWr',
        p_axiWr='cmemWrLog', p_stm='stmCWr')

act.Inst('NativeStreamSwitch', p_regs='regSwitch',
        p_stmIn=['stmHRd', 'stmCRd'], p_stmOut=['stmHWr', 'stmCWr'])


