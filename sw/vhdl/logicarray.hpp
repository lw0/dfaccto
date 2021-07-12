#pragma once

#include <ostream>

#include "../types.hpp"
#include "../bitvector.hpp"
#include "array.hpp"
#include "types.hpp"


namespace sim::vhdl {

class LogicArray : public Array<Logic>
{
public:
  void fill(Logic::Level value, size_t off = 0, size_t len = sim::UnitBits);
  void fill(bool value, size_t off = 0, size_t len = sim::UnitBits);
  void fillX(bool value, bool valid, size_t off = 0, size_t len = sim::UnitBits);

  sim::Unit toUnit(size_t off = 0, size_t len = sim::UnitBits);
  sim::Unit toUnitV(sim::Unit & valid, size_t off = 0, size_t len = sim::UnitBits);
  void fromUnit(sim::Unit value, size_t off = 0, size_t len = sim::UnitBits);
  void fromUnitV(sim::Unit value, sim::Unit valid, size_t off = 0, size_t len = sim::UnitBits);

  void toBitVector(sim::BitVector & vec);
  void toBitVectorValid(sim::BitVector & vec, size_t group = 1);
  bool toBitVectorChanged(sim::BitVector & vec);
  bool toBitVectorValidChanged(sim::BitVector & vec, size_t group = 1);
  void fromBitVector(const sim::BitVector & vec);
  void fromBitVectorValid(const sim::BitVector & vec, size_t group = 1);
};

std::ostream & operator<<(std::ostream & os, const LogicArray & array);

} // namespace sim::vhdl

