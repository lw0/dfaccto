Inc('utils.py')


class SimEnv:
  @classmethod
  def Output(cls, name, type):
    return cls._create(name, type, 'unsigned', 'output', PortO, 'x_is_unsigned', 'UnsignedType()')

  @classmethod
  def Input(cls, name, type):
    return cls._create(name, type, 'unsigned', 'input', PortI, 'x_is_unsigned', 'UnsignedType()')

  @classmethod
  def Push(cls, name, type):
    return cls._create(name, type, 'event', 'push', PortM, 'x_is_event', 'EventType()')

  @classmethod
  def Pull(cls, name, type):
    return cls._create(name, type, 'event', 'pull', PortS, 'x_is_event', 'EventType()')

  @classmethod
  def Source(cls, name, type):
    return cls._create(name, type, 'stream', 'source', PortM, 'x_is_axi_stream', 'AxiStreamType()')

  @classmethod
  def Sink(cls, name, type):
    return cls._create(name, type, 'stream', 'sink', PortS, 'x_is_axi_stream', 'AxiStreamType()')

  @classmethod
  def Master(cls, name, type):
    return cls._create(name, type, 'memory', 'master', PortM, 'x_is_axi', 'AxiType()')

  @classmethod
  def Slave(cls, name, type):
    return cls._create(name, type, 'memory', 'slave', PortS, 'x_is_axi', 'AxiType()')

  @classmethod
  def _create(cls, name, type, group, kind, port_func, type_check, type_kind):
    if not getattr(type, type_check, False):
      raise ValueError('{} interface expects {}, got "{}"'.format(kind, type_kind, type))
    return cls(name, type, group, kind, port_func)

  def __init__(self, name, type, group, kind, port_func):
    self._name = name
    self._type = type
    self._group = group
    self._kind = kind
    self._port_func = port_func

  def resolve(self):
    return self._port_func(self._name, self._type,
                           x_interface=self._kind,
                           x_vars='{{{{>sim/vars_{}.part}}}}'.format(self._group),
                           x_update='{{{{>sim/update_{}.part}}}}'.format(self._kind),
                           x_state='{{{{>sim/state_{}.part}}}}'.format(self._kind))


def EntSimEnv(name, g_config_name, g_period_name, p_sys_name, *interfaces):
  ports = []
  for intf in interfaces:
    if not isinstance(intf, SimEnv):
      raise ValueError('EntSimEnv() expects SimEnv interfaces, got "{}"'.format(intf))
    ports.append(intf.resolve())

  Ent(name,
      Generic(g_config_name, T('String', 'dfaccto')),
      Generic(g_period_name, T('Time', 'dfaccto')),
      PortO(p_sys_name, T('Sys', 'dfaccto')),
      *ports,
      x_gconfig=lambda e: e.generics[g_config_name],
      x_gperiod=lambda e: e.generics[g_period_name],
      x_psys=lambda e: e.ports[p_sys_name],
      x_templates={'sim/simenv.vhd' : 'sim/{}.vhd'.format(name)})


