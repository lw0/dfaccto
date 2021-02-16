Inc('simple_types.py')


Ent('Inner', g_DataWidth=None,
    ps_hsIn='simple.Handshake', pm_hsOut='simple.Handshake',
    pi_dataIn=('simple.Data', 'DataWidth'),
    po_dataOut=('simple.Data', 'DataWidth'),
    po_done='simple.Logic')

Ent('Barrier', g_PortCount=None,
    pi_doneIn=('Logic', 'PortCount'),
    po_done='Logic')


e=Ent('Toplevel', g_DataWidth=None,
      ps_hsIn='Handshake',
      pm_hsOut='Handshake',
      pi_dataIn=('Data', 'DataWidth'),
      po_dataOut=('Data', 'DataWidth'),
      po_done='Logic',
      x_templates={'generic/entity.vhd': 'Toplevel.vhd'})

e.Ins('Inner', name='mid',
      p_hsIn='hsIntFirst',
      p_hsOut='hsIntMid',
      p_dataIn='dataIntFirst',
      p_dataOut='dataIntMid',
      p_done='doneMid')

e.Ins('Inner', name='first',
      p_hsIn='hsIn',
      p_hsOut='hsIntFirst',
      p_dataIn='dataIn',
      p_dataOut='dataIntFirst',
      p_done='doneFirst')

e.Ins('Inner', name='last',
      p_hsIn='hsIntMid',
      p_hsOut='hsOut',
      p_dataIn='dataIntMid',
      p_dataOut='dataOut',
      p_done='doneLast')

e.Ins('Barrier',
      p_doneIn=('doneFirst', 'doneMid', 'doneLast'),
      p_done='done')

