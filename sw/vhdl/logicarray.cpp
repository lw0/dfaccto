#include "logicarray.hpp"


namespace sim::vhdl {

void LogicArray::fill(Logic::Level value, size_t off, size_t len)
{
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    data[pos].set(value);
  }
}

void LogicArray::fill(bool value, size_t off, size_t len)
{
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    data[pos].set(value);
  }
}

void LogicArray::fillX(bool value, bool valid, size_t off, size_t len)
{
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    data[pos].setX(value, valid);
  }
}

Unit LogicArray::toUnit(size_t off, size_t len)
{
  Unit mask = 1;
  Unit value = 0;
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    if (data[pos].isSet()) {
      value |= mask;
    }
    mask <<= 1;
  }
  return value;
}

Unit LogicArray::toUnitV(Unit & rvalid, size_t off, size_t len)
{
  Unit mask = 1;
  Unit value = 0;
  Unit valid = 0;
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    if (data[pos].isSet()) {
      value |= mask;
    }
    if (data[pos].isValid()) {
      valid |= mask;
    }
    mask <<= 1;
  }
  rvalid = valid;
  return value;
}

void LogicArray::fromUnit(Unit value, size_t off, size_t len)
{
  Unit mask = 1;
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    data[pos].set(value & mask);
    mask <<= 1;
  }
}

void LogicArray::fromUnitV(Unit value, Unit valid, size_t off, size_t len)
{
  Unit mask = 1;
  size_t end = (off + len > size())? size() : off + len;
  for (size_t bit = off; bit < end; ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    data[pos].setX(value & mask, valid & mask);
    mask <<= 1;
  }
}

void LogicArray::toBitVector(BitVector & vec)
{
  for (size_t unit = 0; unit < vec.units(); ++unit) {
    Unit value, valid;
    value = toUnitV(valid, unit * UnitBits, UnitBits);
    vec.unitSet(unit * UnitBits, UnitBits, value, valid);
  }
}

void LogicArray::toBitVectorValid(BitVector & vec, size_t group)
{
  for (size_t bit = 0; bit < size(); ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    size_t low = group * bit;
    size_t high = low + group - 1;
    vec.fillValid(high, low, data[pos].isSet());
  }
}

bool LogicArray::toBitVectorChanged(BitVector & vec)
{
  bool changed = false;
  for (size_t unit = 0; unit < vec.units(); ++unit) {
    Unit value, valid;
    value = toUnitV(valid, unit * UnitBits, UnitBits);
    changed = changed || vec.unitSetChanged(unit * UnitBits, UnitBits, value, valid);
  }
  return changed;
}

bool LogicArray::toBitVectorValidChanged(BitVector & vec, size_t group)
{
  bool changed = false;
  for (size_t bit = 0; bit < size(); ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    size_t low = group * bit;
    size_t high = low + group - 1;
    changed = changed || vec.fillValidChanged(high, low, data[pos].isSet());
  }
  return changed;
}

void LogicArray::fromBitVector(const BitVector & vec)
{
  for (size_t unit = 0; unit < vec.units(); ++unit) {
    Unit value, valid;
    vec.unit(unit * UnitBits, UnitBits, value, valid);
    fromUnitV(value, valid, unit * UnitBits, UnitBits);
  }
}

void LogicArray::fromBitVectorValid(const BitVector & vec, size_t group)
{
  for (size_t bit = 0; bit < size(); ++bit) {
    size_t pos = downto()? size() - bit - 1 : bit;
    data[pos].set(vec.allValid(group * bit, group));
  }
}

std::ostream & operator<<(std::ostream & os, const LogicArray & array)
{
  os << "\"";
  for (int idx = 0; idx < array.size(); ++idx) {
    os << array[idx];
  }
  os << "\"";
  return os;
}

} // namespace sim::hdl
