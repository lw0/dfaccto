###############################################################################
# User Definitions
###############################################################################
Ent('StreamAdder',
    pi_offset='RegData',
    ps_stmIn='NativeStream',
    pm_stmOut='NativeStream')
###############################################################################


###############################################################################
# FOSI Environment and Register Map
###############################################################################
Env((0x040,          0x10, 'regsRd'),
    (0x050,          0x10, 'regsWr'),
    (0x060,          0x04, 'regsUsr'),
    g_ActionType=0x80, g_ActionRev=0x0,
    p_start='start', p_ready=['readyRd', 'readyWr'],
    p_hmem='hmem')

Ins('RegisterFile',
    p_regs='regsUsr',
    p_regRd=['regOffset'], p_regWr=['regOffset'])
###############################################################################


###############################################################################
# User Design
###############################################################################
Ins('AxiSplitter',
    p_axi='hmem',
    p_axiRd='axiRd',
    p_axiWr='axiWr')

Ins('AxiReader', g_FIFOLogDepth=3,
    p_regs='regsRd',
    p_start='start', p_ready='readyRd',
    p_axiRd='axiRd',
    p_stm='stmSrc')

Ins('StreamAdder',
    p_offset='regOffset',
    p_stmIn='stmSrc',
    p_stmOut='stmSnk')

Ins('AxiWriter', g_FIFOLogDepth=0,
    p_regs='regsWr',
    p_start='start', p_ready='readyWr',
    p_axiWr='axiWr',
    p_stm='stmSnk')
###############################################################################

