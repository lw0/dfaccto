#pragma once

#include <ostream>

#include "../types.hpp"
// #include "../bitvector.hpp"
#include "array.hpp"
#include "types.hpp"


namespace sim::vhdl {

class LogicArray : public Array<Logic>
{
public:
  void fill(Logic::Level value, size_t off = 0, size_t len = sim::UnitBits);
  void fill(bool value, size_t off = 0, size_t len = sim::UnitBits);
  void fillV(bool value, bool valid, Logic::Level invvalue = Logic::VX, size_t off = 0, size_t len = sim::UnitBits);

  sim::Unit toUnit(size_t off = 0, size_t len = sim::UnitBits) const;
  sim::Unit toUnitGroup(size_t off, size_t len, size_t group) const;
  sim::Unit toUnitV(sim::Unit & valid, size_t off = 0, size_t len = sim::UnitBits) const;

  void fromUnit(sim::Unit value, size_t off = 0, size_t len = sim::UnitBits);
  void fromUnitV(sim::Unit value, sim::Unit valid, Logic::Level invvalue = Logic::VX, size_t off = 0, size_t len = sim::UnitBits);
  void fromUnitGroupAll(sim::Unit value, size_t off, size_t len, size_t group);
  void fromUnitGroupAny(sim::Unit value, size_t off, size_t len, size_t group);
};

std::ostream & operator<<(std::ostream & os, const LogicArray & array);

} // namespace sim::vhdl

