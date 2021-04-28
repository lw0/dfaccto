
def IntegerType(name, min=None, max=None):
  return TypeS(name, x_min=min, x_max=max,
               x_definition='{{>types/integer.part}}',
               x_format='{{>formats/integer.part}}',
               x_cnull=lambda t: Con('{}Null'.format(name), t, value=Lit(0)))

def UnsignedType(name, width, **directives):
  return TypeS(name, x_is_unsigned=True,
               x_width=width,
               x_definition='{{>types/unsigned.part}}',
               x_format='{{>formats/unsigned.part}}',
               x_cnull=lambda t: Con('{}Null'.format(name), t, value=Lit(0)),
               **directives)

def uwidth(x):
  assert x >= 0, 'Can not compute unsigned width on a negative value'
  return x.bit_length()

def swidth(max):
  if x < 0:
    x = ~x
  return x.bit_length()+1

