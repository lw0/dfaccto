#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>


namespace sim::vhdl {

using std::size_t;
using std::uint8_t;

using Integer = std::int32_t;

struct Logic
{
private:
  static const char CharMap[9];
  static const bool ValueMap[9];
  static const bool ValidMap[9];

public:
  enum Level : std::uint8_t
  {
    VU = 0,
    VX = 1,
    V0 = 2,
    V1 = 3,
    VZ = 4,
    VW = 5,
    VL = 6,
    VH = 7,
    VD = 8,
  };

  Level value;

  bool operator==(Level rhs) const;
  bool operator!=(Level rhs) const;

  char asChar() const;
  bool isSet() const;
  bool isValid() const;
  void set(Level v);
  void set(bool state);
  void setV(bool state, bool valid, Level invvalue);
};

std::ostream & operator<<(std::ostream & os, const Logic & logic);

} // namespace sim::vhdl

