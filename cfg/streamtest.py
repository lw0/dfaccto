Inc('dfaccto.py', abs='lib')


with Pkg('user',
    x_templates={File('generic/package.vhd.tpl', mod='lib'): File('pkg/user.vhd')}):
  TypeAxiStream('LongStream', data_bytes=8, id_bits=2)


Ent('Streamtest',
    PortI('sys',       T('Sys'),        label='psys'),
    PortS('stmSrc',    T('LongStream'), label='psrc'),
    PortM('stmBufIn',  T('LongStream'), label='ptobuf'),
    PortS('stmBufOut', T('LongStream'), label='pfrombuf'),
    PortM('stmSnk',    T('LongStream'), label='psnk'),
    x_templates={File('streamtest.vhd.tpl') : File('streamtest.vhd')})

