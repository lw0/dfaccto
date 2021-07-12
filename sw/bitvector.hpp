#pragma once

#include <array>
#include <list>
#include <string>

#include "types.hpp"



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
  BitVector(size_t bits, sim::Unit value, size_t bitPos = 0);
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

  bool unitSetChanged(size_t bitPos, size_t bitCount, sim::Unit value, sim::Unit valid);
  bool unitSetValueChanged(size_t bitPos, size_t bitCount, sim::Unit value);
  bool unitSetValidChanged(size_t bitPos, size_t bitCount, sim::Unit valid);

  BitVector slice(size_t bitPos, size_t bitCount) const;
  bool allValid(size_t bitPos, size_t bitCount) const;
  bool anyValid(size_t bitPos, size_t bitCount) const;

  void set(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValid(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  void setValidFromValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);

  bool setChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  bool setValueChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  bool setValidChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);
  bool setValidFromValueChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0);

  bool equals(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos = 0) const;
  bool equals(const BitVector & other) const;

  void fill(size_t bitPos, size_t bitCount, bool value, bool valid = true);
  void fillValue(size_t bitPos, size_t bitCount, bool value);
  void fillValid(size_t bitPos, size_t bitCount, bool valid);

  bool fillChanged(size_t bitPos, size_t bitCount, bool value, bool valid = true);
  bool fillValueChanged(size_t bitPos, size_t bitCount, bool value);
  bool fillValidChanged(size_t bitPos, size_t bitCount, bool valid);

  void append(const BitVector & other);

  void fromString(const std::string & str);
  void fromHex(const std::string & str);
  void fromBin(const std::string & str);

  std::string toString();
  std::string toHex();
  std::string toBin();

private:
  size_t m_bits;
  Unit * m_buffer;
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

} // namespace sim
