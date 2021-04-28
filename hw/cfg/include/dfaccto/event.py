Inc('utils.py')


def EventType(name, stb_bits=None, ack_bits=None):

  tlogic = T('Logic', 'dfaccto')

  if stb_bits is not None:
    tsdata = UnsignedType('{}Strb'.format(name), width=stb_bits)
  else:
    tsdata = None

  if ack_bits is not None:
    tadata = UnsignedType('{}Ack'.format(name), width=ack_bits)
  else:
    tadata = None

  TypeC(name, x_is_event=True,
        x_definition='{{>types/event.part}}',
        x_format_ms='{{>formats/event_ms.part}}',
        x_format_sm='{{>formats/event_sm.part}}',
        x_tlogic=tlogic, x_tsdata=tsdata, x_tadata=tadata,
        x_cnull=lambda t: Con('{}Null'.format(name), t, value=Lit({'stb': False, 'ack': False})))


