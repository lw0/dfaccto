#include "types.hpp"


namespace sim::vhdl {

const char Logic::CharMap[9]  = { 'U',   'X',   '0',   '1',   'Z',   'W',   'L',   'H',   '-' };
const bool Logic::ValueMap[9] = { false, false, false, true,  false, false, false, true,  false };
const bool Logic::ValidMap[9] = { false, false, true,  true,  false, false, true,  true,  false };

bool Logic::operator==(Logic::Level rhs) const
{
  return value == rhs;
}

bool Logic::operator!=(Logic::Level rhs) const
{
  return value != rhs;
}

char Logic::asChar() const
{
  if (value > 8)
    return '?';
  return CharMap[value];
}

bool Logic::isSet() const
{
  if (value > 8)
    return false;
  return ValueMap[value];
}

bool Logic::isValid() const
{
  if (value > 8)
    return false;
  return ValidMap[value];
}

void Logic::set(Logic::Level v)
{
  value = v;
}

void Logic::set(bool state)
{
  value = state? V1 : V0;
}

void Logic::setV(bool state, bool valid, Level invvalue)
{
  if (!valid) {
    value = invvalue;
  } else {
    value = state? V1 : V0;
  }
}

std::ostream & operator<<(std::ostream & os, const Logic & value)
{
  os << value.asChar();
  return os;
}

} // namespace sim::vhdl

