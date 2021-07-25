#pragma once

#include <array>
#include <list>
#include <string>

#include "types.hpp"
#include "vhdl/logicarray.hpp"



namespace sim {


class BitVectorAllocator
{
  /* Allocates data buffers for BitVector instances,
   * assuming frequent reuse of small buffers (up to 64Kbits / 256 units).
   *
   * Layout:
   *   | Value 0 | Valid 0 | Value 1 | Valid 1 | ... | Value n-1 | Valid n-1 |
   * this->alloc(n) allocates a buffer of 2*n units
   * this->free(buf, n) frees buf assuming it has 2*n units
   */

protected:
  static constexpr size_t AllocLogLimit = 6; // 64bit * 2^10 = 64Kbit

public:
  BitVectorAllocator();
  ~BitVectorAllocator();

  Unit * realloc(Unit * buffer, size_t nUnitsOld, size_t nUnitsNew);
  Unit * alloc(size_t nUnits);
  void free(Unit * buffer, size_t nUnits);

protected:
  bool allocSize(size_t nUnits, size_t & idx, size_t & size);
  void freeAll();

private:
  std::array<std::list<Unit *>, AllocLogLimit> m_allocs;
  bool m_active;
};


class BitVector
{
protected:
  static BitVectorAllocator s_alloc;

public:

  BitVector(size_t bits = 0, bool value = false, bool valid = false);
  ~BitVector();

  BitVector(const BitVector & other);
  BitVector(BitVector && other);

  BitVector & operator=(const BitVector & other);
  BitVector & operator=(BitVector && other);

  inline size_t bits() const;
  inline size_t units() const;

  void resize(size_t newBits, bool value = false, bool valid = false);

  void unit(size_t bitPos, size_t bitCount, sim::Unit & value, sim::Unit & valid) const;
  sim::Unit unitValue(size_t bitPos, size_t bitCount) const;
  sim::Unit unitValid(size_t bitPos, size_t bitCount) const;
  void unitSet(size_t bitPos, size_t bitCount, sim::Unit value, sim::Unit valid);
  void unitSetValue(size_t bitPos, size_t bitCount, sim::Unit value);
  void unitSetValid(size_t bitPos, size_t bitCount, sim::Unit valid);

  BitVector slice(size_t bitPos, size_t bitCount) const;
  bool allValid(size_t bitPos, size_t bitCount) const;
  bool anyValid(size_t bitPos, size_t bitCount) const;

  void set(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValid(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValidFromValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValueFromValid(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);

  bool equals(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0) const;
  bool equals(const BitVector & other) const;

  void fill(size_t bitPos, size_t bitCount, bool value, bool valid = true);
  void fillValue(size_t bitPos, size_t bitCount, bool value);
  void fillValid(size_t bitPos, size_t bitCount, bool valid);

  void append(const BitVector & other);

  void fromLogic(const sim::vhdl::LogicArray * array);
  void fromLogicValid(const sim::vhdl::LogicArray * array, size_t group = 1);
  void fromStr(const std::string & str);
  void fromHex(const std::string & str);
  void fromBin(const std::string & str);

  void toLogic(sim::vhdl::LogicArray * array, sim::vhdl::Logic::Level invalid = sim::vhdl::Logic::VX) const;
  void toLogicValid(sim::vhdl::LogicArray * array, size_t group = 1) const;
  std::string toStr() const;
  std::string toHex() const;
  std::string toBin() const;

protected:
  inline Unit * buffer();
  inline const Unit * buffer() const;

  friend class BitVectorFlagged;

private:
  size_t m_bits;
  Unit * m_buffer;
};

class BitVectorFlagged : public BitVector {
public:
  BitVectorFlagged(size_t bits = 0, bool value = false, bool valid = false);

  BitVectorFlagged(const BitVectorFlagged & other);
  BitVectorFlagged(const BitVector & other);
  BitVectorFlagged(BitVectorFlagged && other);
  BitVectorFlagged(BitVector && other);

  BitVectorFlagged & operator=(const BitVectorFlagged & other);
  BitVectorFlagged & operator=(const BitVector & other);
  BitVectorFlagged & operator=(BitVectorFlagged && other);
  BitVectorFlagged & operator=(BitVector && other);

  inline bool changed();
  inline bool changed(bool set);

  void resize(size_t newBits, bool value = false, bool valid = false);

  void unitSet(size_t bitPos, size_t bitCount, sim::Unit value, sim::Unit valid);
  void unitSetValue(size_t bitPos, size_t bitCount, sim::Unit value);
  void unitSetValid(size_t bitPos, size_t bitCount, sim::Unit valid);

  void set(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValid(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValidFromValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValueFromValid(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);

  void fill(size_t bitPos, size_t bitCount, bool value, bool valid = true);
  void fillValue(size_t bitPos, size_t bitCount, bool value);
  void fillValid(size_t bitPos, size_t bitCount, bool valid);

  void append(const BitVector & other);

  void fromLogic(const sim::vhdl::LogicArray * array);
  void fromLogicValid(const sim::vhdl::LogicArray * array, size_t group = 1);
  void fromStr(const std::string & str);
  void fromHex(const std::string & str);
  void fromBin(const std::string & str);

private:
  bool m_changed;
};

} // namespace sim


namespace sim {

inline size_t BitVector::bits() const
{
  return m_bits;
}

inline size_t BitVector::units() const
{
  return UnitCount(m_bits);
}

inline Unit * BitVector::buffer()
{
  return m_buffer;
}

inline const Unit * BitVector::buffer() const
{
  return m_buffer;
}

inline bool BitVectorFlagged::changed()
{
  return m_changed;
}

inline bool BitVectorFlagged::changed(bool set)
{
  bool changed = m_changed;
  m_changed = set;
  return changed;
}


} // namespace sim
