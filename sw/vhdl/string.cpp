#include "string.hpp"


namespace sim::vhdl {

std::string String::toString() const
{
  return std::string(data, size());
}

std::ostream & operator<<(std::ostream & os, const String & array)
{
  os << array.toString();
  return os;
}

} // namespace sim::vhdl
