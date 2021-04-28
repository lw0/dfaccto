#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>

#include "types.hpp"
#include "array.hpp"



namespace sim::vhdl {

class String : public Array<char>
{
public:
  std::string toString() const;
};

std::ostream & operator<<(std::ostream & os, const String & array);

} // namespace sim::vhdl

