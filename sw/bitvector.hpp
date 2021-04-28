#pragma once

#include "types.hpp"


namespace sim {

class BitVector
{
public:
  BitVector(size_t bits, bool value = false, bool valid = false);
  ~BitVector();

  BitVector(const BitVector & other);
  BitVector(BitVector && other);

  BitVector & operator=(const BitVector & other);
  BitVector & operator=(BitVector && other);

  inline size_t bits() const;
  inline size_t units() const;

  void set(size_t pos, bool value, bool valid = true);
  void set(size_t high, size_t low, Unit value, bool setone = true, bool setzero = true);
  void fill(size_t high, size_t low, bool value, bool valid = true);
  void fillValid(size_t high, size_t low, bool valid = true);

  bool get(size_t pos, bool & valid) const;
  Unit get(size_t high, size_t low, bool & valid) const;
  bool valid(size_t high, size_t low) const;

  inline Unit valueAt(size_t unit, Unit mask = UnitOnes) const;
  inline Unit validAt(size_t unit, Unit mask = UnitOnes) const;
  inline bool validAll(size_t unit, Unit mask = UnitOnes) const;
  inline bool validAny(size_t unit, Unit mask = UnitOnes) const;
  inline void mux(size_t unit, Unit value, Unit valid, Unit mask = UnitOnes);
  inline void muxValue(size_t unit, Unit value, Unit mask = UnitOnes);
  inline void muxValid(size_t unit, Unit valid, Unit mask = UnitOnes);

private:
  size_t m_bits;
  Unit * m_value;
  Unit * m_valid;
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

inline Unit BitVector::valueAt(size_t unit, Unit mask) const
{
  return m_value[unit] & mask;
}

inline Unit BitVector::validAt(size_t unit, Unit mask) const
{
  return m_valid[unit] & mask;
}

inline bool BitVector::validAll(size_t unit, Unit mask) const
{
  return !(~m_valid[unit] & mask);
}

inline bool BitVector::validAny(size_t unit, Unit mask) const
{
  return m_valid[unit] & mask;
}

inline void BitVector::mux(size_t unit, Unit value, Unit valid, Unit mask)
{
  m_value[unit] = Mux(mask, m_value[unit], value);
  m_valid[unit] = Mux(mask, m_valid[unit], valid);
}

inline void BitVector::muxValue(size_t unit, Unit value, Unit mask)
{
  m_value[unit] = Mux(mask, m_value[unit], value);
}

inline void BitVector::muxValid(size_t unit, Unit valid, Unit mask)
{
  m_valid[unit] = Mux(mask, m_valid[unit], valid);
}

} // namespace sim
