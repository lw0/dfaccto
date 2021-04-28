#include "bitvector.hpp"

#include <utility>


namespace sim {

BitVector::BitVector(size_t bits, bool value, bool valid)
: m_bits {bits}
, m_value {nullptr}
, m_valid {nullptr}
{
  const size_t units = UnitCount(m_bits);
  const Unit uValue = value? UnitOnes : UnitZeros;
  const Unit uValid = valid? UnitOnes : UnitZeros;

  m_value = new Unit[units];
  m_valid = new Unit[units];
  for (size_t idx = 0; idx < units; ++idx) {
    m_value[idx] = uValue;
    m_valid[idx] = uValid;
  }
}

BitVector::~BitVector()
{
  if (m_value)
    delete m_value;

  if (m_valid)
    delete m_valid;
}

BitVector::BitVector(const BitVector & other)
: m_bits {other.m_bits}
, m_value {nullptr}
, m_valid {nullptr}
{
  const size_t units = UnitCount(m_bits);

  m_value = new Unit[units];
  m_valid = new Unit[units];
  for (size_t idx = 0; idx < units; ++idx) {
    m_value[idx] = other.m_value[idx];
    m_valid[idx] = other.m_valid[idx];
  }
}

BitVector::BitVector(BitVector && other)
: m_bits {other.m_bits}
, m_value {other.m_value}
, m_valid {other.m_valid}
{
  other.m_value = nullptr;
  other.m_valid = nullptr;
}

BitVector & BitVector::operator=(const BitVector & other)
{
  const size_t units = UnitCount(other.m_bits);
  if (m_bits != other.m_bits) {
    if (m_value)
      delete m_value;

    if (m_valid)
      delete m_valid;

    m_bits = other.m_bits;
    m_value = new Unit[units];
    m_valid = new Unit[units];
  }

  for (size_t idx = 0; idx < units; ++idx) {
    m_value[idx] = other.m_value[idx];
    m_valid[idx] = other.m_valid[idx];
  }

  return *this;
}

BitVector & BitVector::operator=(BitVector && other)
{
  std::swap(m_bits, other.m_bits);
  std::swap(m_value, other.m_value);
  std::swap(m_valid, other.m_valid);

  return *this;
}

void BitVector::set(size_t pos, bool value, bool valid)
{
  if (pos < 0 || pos >= m_bits)
    return;

  size_t pIdx = UnitIdx(pos);
  Unit pMsk = BitAt(UnitOff(pos));
  mux(pIdx, value? UnitOnes : UnitZeros, valid? UnitOnes : UnitZeros, pMsk);
}

void BitVector::set(size_t high, size_t low, Unit value, bool ones, bool zeros)
{
  if (low < 0 || low > high || high >= m_bits)
    return;
  if (high > low + UnitBits - 1)
    high = low + UnitBits - 1;

  size_t lIdx = UnitIdx(low);
  size_t hIdx = UnitIdx(high);
  size_t lOff = UnitOff(low);
  Unit lMsk = BitsAbove(lOff);
  Unit hMsk = BitsBelow(UnitOff(high));
  Unit lVal = SplitLow(lOff, value);
  Unit hVal = SplitHigh(lOff, value);
  if (!ones) {
    lMsk &= ~lVal;
    hMsk &= ~hVal;
  }
  if (!zeros) {
    lMsk &= lVal;
    hMsk &= hVal;
  }
  if (lIdx == hIdx) {
    mux(lIdx, SplitLow(lOff, value), UnitOnes, hMsk & lMsk);
  } else {
    mux(lIdx, SplitLow(lOff, value), UnitOnes, lMsk);
    mux(hIdx, SplitHigh(lOff, value), UnitOnes, hMsk);
  }
}

void BitVector::fill(size_t high, size_t low, bool value, bool valid)
{
  if (low < 0 || low > high || high >= m_bits)
    return;

  size_t lIdx = UnitIdx(low);
  size_t hIdx = UnitIdx(high);
  Unit lMsk = UnitOff(low);
  Unit hMsk = UnitOff(high);
  for (size_t idx = lIdx; idx <= hIdx; ++idx) {
    Unit msk = UnitOnes;
    if (idx == lIdx) msk &= lMsk;
    if (idx == hIdx) msk &= hMsk;
    mux(idx, value? UnitOnes : UnitZeros, valid? UnitOnes : UnitZeros, msk);
  }
}

void BitVector::fillValid(size_t high, size_t low, bool valid)
{
  if (low < 0 || low > high || high >= m_bits)
    return;

  size_t lIdx = UnitIdx(low);
  size_t hIdx = UnitIdx(high);
  Unit lMsk = UnitOff(low);
  Unit hMsk = UnitOff(high);
  for (size_t idx = lIdx; idx <= hIdx; ++idx) {
    Unit msk = UnitOnes;
    if (idx == lIdx) msk &= lMsk;
    if (idx == hIdx) msk &= hMsk;
    muxValid(idx, valid? UnitOnes : UnitZeros, msk);
  }
}

bool BitVector::get(size_t pos, bool & valid) const
{
  if (pos < 0 || pos >= m_bits) {
    valid = false;
    return false;
  }

  size_t pIdx = UnitIdx(pos);
  Unit pMsk = BitAt(UnitOff(pos));
  valid = validAll(pIdx, pMsk);
  return valueAt(pIdx, pMsk);
}

Unit BitVector::get(size_t high, size_t low, bool & valid) const
{
  if (low < 0 || low > high || high >= m_bits) {
    valid = false;
    return 0;
  }
  if (high > low + UnitBits - 1) {
    high = low + UnitBits - 1;
  }

  size_t lIdx = UnitIdx(low);
  size_t hIdx = UnitIdx(high);
  size_t lOff = UnitOff(low);
  Unit lMsk = BitsAbove(lOff);
  Unit hMsk = BitsBelow(UnitOff(high));
  if (lIdx == hIdx) {
    valid = validAll(lIdx, lMsk & hMsk);
    return Join(lOff, valueAt(lIdx, lMsk & hMsk), UnitZeros);
  } else {
    valid = validAll(lIdx, lMsk) && validAll(hIdx, hMsk);
    return Join(lOff, valueAt(lIdx, lMsk), valueAt(hIdx, hMsk));
  }
}

bool BitVector::valid(size_t high, size_t low) const
{
  if (low < 0 || low > high || high >= m_bits)
    return false;

  size_t lIdx = UnitIdx(low);
  size_t hIdx = UnitIdx(high);
  Unit lMsk = UnitOff(low);
  Unit hMsk = UnitOff(high);
  for (size_t idx = lIdx; idx <= hIdx; ++idx) {
    Unit msk = UnitOnes;
    if (idx == lIdx) msk &= lMsk;
    if (idx == hIdx) msk &= hMsk;
    if (!validAll(idx, msk)) {
      return false;
    }
  }
  return true;
}

} // namespace sim

