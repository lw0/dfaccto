#include "bitvector.hpp"

#include <iostream>
#include <algorithm>
#include <utility>



namespace {

inline bool hexdigit(char digit, sim::Unit & value)
{
  if ('0' <= digit && digit <= '9') {
    value = digit - '0';
    return true;
  } else if ('a' <= digit && digit <= 'f') {
    value = digit - 'a';
    return true;
  } else if ('A' <= digit && digit <= 'F') {
    value = digit - 'A';
    return true;
  } else {
    value = 0;
    return false;
  }
}

inline bool bindigit(char digit, sim::Unit & value)
{
  if ('0' <= digit && digit <= '1') {
    value = digit - '0';
    return true;
  } else {
    value = 0;
    return false;
  }
}

const char * digithex_map = "0123456789ABCDEF";
inline char digithex(sim::Unit value, bool valid)
{
  if (valid) {
    return digithex_map[value & 0xf];
  } else {
    return '-';
  }
}

const char * digitbin_map = "01";
inline char digitbin(sim::Unit value, bool valid)
{
  if (valid) {
    return digithex_map[value & 0x1];
  } else {
    return '-';
  }
}

}


namespace sim {

// void bitIter(size_t dstPos, size_t srcPos, size_t len) {
//   size_t dstIdxLow = sim::UnitIdx(dstPos);
//   size_t dstOffLow = sim::UnitOff(dstPos);
//   size_t dstIdxHigh = sim::UnitIdx(dstPos + len - 1);
//   size_t dstOffHigh = sim::UnitOff(dstPos + len - 1);
//   for (size_t dstIdx = dstIdxLow; dstIdx <= dstIdxHigh; ++dstIdx) {
//     sim::Unit dstMsk = sim::UnitOnes;
//     size_t dstOff = 0;
//     size_t srcLen = sim::UnitBits;
//     if (dstIdx == dstIdxLow) {
//       dstMsk &= sim::BitsAbove(dstOffLow);
//       dstOff = dstOffLow;
//       srcLen -= dstOffLow;
//     }
//     if (dstIdx == dstIdxHigh) {
//       dstMsk &= sim::BitsBelow(dstOffHigh);
//       srcLen -= sim::UnitBits - dstOffHigh - 1;
//     }
//     size_t srcIdxLow = sim::UnitIdx(srcPos);
//     size_t srcPosLow = sim::UnitOff(srcPos);
//     size_t srcIdxHigh = sim::UnitIdx(srcPos + srcLen - 1);
//     size_t srcPosHigh = sim::UnitBits - srcPosLow;
//     // ?dstMux dstIdx dstPos dstMsk ?srcMux srcIdxLow srcOffLow srcIdxHigh srcOffHigh
//     srcPos += srcLen;
//   }
// }

inline sim::Unit & VALUE(sim::Unit * buffer, size_t unit) { return buffer[2 * unit]; }
inline sim::Unit & VALID(sim::Unit * buffer, size_t unit) { return buffer[2 * unit + 1]; }
inline const sim::Unit & VALUE(const sim::Unit * buffer, size_t unit) { return buffer[2 * unit]; }
inline const sim::Unit & VALID(const sim::Unit * buffer, size_t unit) { return buffer[2 * unit + 1]; }

class BitIter {
public:

  BitIter(size_t dstPos, size_t len, size_t dstBits) : BitIter(dstPos, 0, len, dstBits, 0) { }

  BitIter(size_t dstPos, size_t srcPos, size_t len, size_t dstBits, size_t srcBits) {
    // std::clog << "BitIter(" << dstPos << ", " << srcPos << ", " << len << ", " << dstBits << ", " << srcBits << ")" << std::endl;
    m_active = dstPos < dstBits && len > 0;
    if (!m_active) return;

    m_srcBits = srcBits;

    m_dstIdx = sim::UnitIdx(dstPos);
    m_dstPos = sim::UnitOff(dstPos);
    size_t dstPosHigh = std::min(dstPos + len - 1, dstBits - 1);
    m_dstIdxHigh = sim::UnitIdx(dstPosHigh);
    m_dstOffHigh = sim::UnitOff(dstPosHigh);

    m_srcPos = srcPos;
    m_srcLen = sim::UnitBits - m_dstPos;

    m_dstMsk = sim::BitsAbove(m_dstPos);
    if (m_dstIdx == m_dstIdxHigh) {
      m_dstMsk &= sim::BitsBelow(m_dstOffHigh);
      m_srcLen -= sim::UnitBits - m_dstOffHigh - 1;
    }
  }

  inline operator bool () {
    if (m_active) {
      // std::clog << "dst[" << m_dstIdx << "] = src << " << std::setw(3) << m_dstPos << " & " << std::hex << std::setw(16) << std::setfill('0') << m_dstMsk << std::dec << std::setw(0) << std::setfill(' ') << std::endl;
    }
    return m_active;
  }

  BitIter & operator++() {
    m_dstIdx += 1;
    m_active = m_dstIdx <= m_dstIdxHigh;
    if (!m_active) return *this;

    m_srcPos += m_srcLen;
    m_srcLen = sim::UnitBits;
    m_dstPos = 0;
    m_dstMsk = sim::UnitOnes;

    if (m_dstIdx == m_dstIdxHigh) {
      m_dstMsk = sim::BitsBelow(m_dstOffHigh);
      m_srcLen -= sim::UnitBits - m_dstOffHigh - 1;
    }

    return *this;
  }

  sim::Unit dstGetValue(const sim::Unit * dst) {
    return (VALID(dst, m_dstIdx) & m_dstMsk) >> m_dstPos;
  }
  sim::Unit dstGetValid(const sim::Unit * dst) {
    return (VALID(dst, m_dstIdx) & m_dstMsk) >> m_dstPos;
  }

  sim::Unit dstGetInvalid(const sim::Unit * dst) {
    return (~VALID(dst, m_dstIdx) & m_dstMsk) >> m_dstPos;
  }

  void dstSet(sim::Unit * dst, sim::Unit * value, sim::Unit * valid) {
    if (m_dstMsk == 0) return;
    if (~m_dstMsk == 0) {
      if (value) {
        VALUE(dst, m_dstIdx) = *value;
      }
      if (valid) {
        VALID(dst, m_dstIdx) = *valid;
      }
    } else {
      if (value) {
        VALUE(dst, m_dstIdx) = ~m_dstMsk & VALUE(dst, m_dstIdx) | m_dstMsk & (*value << m_dstPos);
      }
      if (valid) {
        VALID(dst, m_dstIdx) = ~m_dstMsk & VALID(dst, m_dstIdx) | m_dstMsk & (*valid << m_dstPos);
      }
    }
  }

  bool dstSetChanged(sim::Unit * dst, sim::Unit * value, sim::Unit * valid) {
    if (m_dstMsk == 0) return false;
    bool valueChanged = false;
    bool validChanged = false;
    if (value) {
      sim::Unit lastValue = VALUE(dst, m_dstIdx);
      sim::Unit newValue = *value << m_dstPos;
      valueChanged = (lastValue & m_dstMsk) != (newValue & m_dstMsk);
      if (valueChanged) {
        VALUE(dst, m_dstIdx) = ~m_dstMsk & lastValue | m_dstMsk & newValue;
      }
    }
    if (valid) {
      sim::Unit lastValid = VALID(dst, m_dstIdx);
      sim::Unit newValid = *valid << m_dstPos;
      validChanged = (lastValid & m_dstMsk) != (newValid & m_dstMsk);
      if (validChanged) {
        VALID(dst, m_dstIdx) = ~m_dstMsk & lastValid | m_dstMsk & newValid;
      }
    }
    return valueChanged || validChanged;
  }

  void srcGet(const sim::Unit * src, sim::Unit * value, sim::Unit * valid) {
    if (m_srcPos < m_srcBits) {
      size_t srcIdxLow = sim::UnitIdx(m_srcPos);
      size_t srcOffLow = sim::UnitOff(m_srcPos);
      size_t srcPosHigh = std::min(m_srcPos + m_srcLen - 1, m_srcBits - 1);
      size_t srcIdxHigh = sim::UnitIdx(srcPosHigh);
      size_t srcOffHigh = sim::UnitBits - srcOffLow;
      bool multiSrc = srcIdxLow != srcIdxHigh;
      sim::Unit srcMsk = sim::BitsBelow(srcPosHigh - m_srcPos);
      if (value) {
        sim::Unit v = VALUE(src, srcIdxLow) >> srcOffLow;
        if (multiSrc) {
          v |= VALUE(src, srcIdxHigh) << srcOffHigh;
        }
        *value = v & srcMsk;
      }
      if (valid) {
        sim::Unit v = VALID(src, srcIdxLow) >> srcOffLow;
        if (multiSrc) {
          v |= VALID(src, srcIdxHigh) << srcOffHigh;
        }
        *valid = v & srcMsk;

      }
    } else {
      if (value) {
        *value = 0;
      }
      if (valid) {
        *valid = 0;
      }
    }
  }

  inline size_t dstIdx() { return m_dstIdx; }
  inline size_t dstPos() { return m_dstPos; }
  inline size_t dstMsk() { return m_dstMsk; }


private:
  size_t m_dstIdxHigh;
  size_t m_dstOffHigh;

  bool m_active;

  size_t m_srcBits;
  size_t m_srcPos;
  size_t m_srcLen;

  size_t m_dstIdx;
  size_t m_dstPos;
  sim::Unit m_dstMsk;
};

BitVectorAllocator BitVector::s_alloc;

BitVector::BitVector(size_t bits, bool value, bool valid)
: m_bits {bits}
, m_buffer {s_alloc.alloc(UnitCount(m_bits))}
{
  fill(0, m_bits, value, valid);
}

BitVector::BitVector(size_t bits, sim::Unit value, size_t bitPos)
: m_bits {bits}
, m_buffer {s_alloc.alloc(UnitCount(m_bits))}
{
  unitSet(bitPos, sim::UnitBits, value, sim::UnitOnes);
}

BitVector::~BitVector()
{
  s_alloc.free(m_buffer, UnitCount(m_bits));
}

BitVector::BitVector(const BitVector & other)
: m_bits {other.m_bits}
, m_buffer {s_alloc.alloc(UnitCount(m_bits))}
{
  set(0, m_bits, other, 0);
}

BitVector::BitVector(BitVector && other)
: m_bits {other.m_bits}
, m_buffer {other.m_buffer}
{
  other.m_buffer = nullptr;
  other.m_bits = 0;
}

BitVector & BitVector::operator=(const BitVector & other)
{
  m_buffer = s_alloc.realloc(m_buffer, UnitCount(m_bits), UnitCount(other.m_bits));
  m_bits = other.m_bits;

  set(0, m_bits, other, 0);

  return *this;
}

BitVector & BitVector::operator=(BitVector && other)
{
  std::swap(m_buffer, other.m_buffer);
  std::swap(m_bits, other.m_bits);

  return *this;
}

void BitVector::resize(size_t newBits, bool value, bool valid)
{
  size_t oldBits = m_bits;
  m_bits = newBits;
  m_buffer = s_alloc.realloc(m_buffer, sim::UnitCount(oldBits), sim::UnitCount(newBits));

  if (oldBits < newBits) {
    fill(oldBits, newBits - oldBits, value, valid);
  }
}

void BitVector::unit(size_t bitPos, size_t bitCount, sim::Unit & value, sim::Unit & valid) const
{
  BitIter iter(0, bitPos, bitCount, sim::UnitBits, m_bits);
  iter.srcGet(m_buffer, &value, &valid);
}

sim::Unit BitVector::unitValue(size_t bitPos, size_t bitCount) const
{
  sim::Unit value;
  BitIter iter(0, bitPos, bitCount, sim::UnitBits, m_bits);
  iter.srcGet(m_buffer, &value, nullptr);
  return value;
}

sim::Unit BitVector::unitValid(size_t bitPos, size_t bitCount) const
{
  sim::Unit valid;
  BitIter iter(0, bitPos, bitCount, sim::UnitBits, m_bits);
  iter.srcGet(m_buffer, nullptr, &valid);
  return valid;
}

void BitVector::unitSet(size_t bitPos, size_t bitCount, sim::Unit value, sim::Unit valid)
{
  sim::Unit buffer[] = {value, valid};
  for (BitIter iter(bitPos, 0, bitCount, m_bits, sim::UnitBits); iter; ++iter) {
    sim::Unit value, valid;
    iter.srcGet(buffer, &value, &valid);
    iter.dstSet(m_buffer, &value, &valid);
  }
}

void BitVector::unitSetValue(size_t bitPos, size_t bitCount, sim::Unit value)
{
  sim::Unit buffer[] = {value, sim::UnitZeros};
  for (BitIter iter(bitPos, 0, bitCount, m_bits, sim::UnitBits); iter; ++iter) {
    sim::Unit value;
    iter.srcGet(buffer, &value, nullptr);
    iter.dstSet(m_buffer, &value, nullptr);
  }
}

void BitVector::unitSetValid(size_t bitPos, size_t bitCount, sim::Unit valid)
{
  sim::Unit buffer[] = {sim::UnitZeros, valid};
  for (BitIter iter(bitPos, 0, bitCount, m_bits, sim::UnitBits); iter; ++iter) {
    sim::Unit valid;
    iter.srcGet(buffer, nullptr, &valid);
    iter.dstSet(m_buffer, nullptr, &valid);
  }
}

bool BitVector::unitSetChanged(size_t bitPos, size_t bitCount, sim::Unit value, sim::Unit valid)
{
  bool changed = false;
  sim::Unit buffer[] = {value, valid};
  for (BitIter iter(bitPos, 0, bitCount, m_bits, sim::UnitBits); iter; ++iter) {
    sim::Unit value, valid;
    iter.srcGet(buffer, &value, &valid);
    changed = changed || iter.dstSetChanged(m_buffer, &value, &valid);
  }
  return changed;
}

bool BitVector::unitSetValueChanged(size_t bitPos, size_t bitCount, sim::Unit value)
{
  bool changed = false;
  sim::Unit buffer[] = {value, sim::UnitZeros};
  for (BitIter iter(bitPos, 0, bitCount, m_bits, sim::UnitBits); iter; ++iter) {
    sim::Unit value;
    iter.srcGet(buffer, &value, nullptr);
    changed = changed || iter.dstSetChanged(m_buffer, &value, nullptr);
  }
  return changed;
}

bool BitVector::unitSetValidChanged(size_t bitPos, size_t bitCount, sim::Unit valid)
{
  bool changed = false;
  sim::Unit buffer[] = {sim::UnitZeros, valid};
  for (BitIter iter(bitPos, 0, bitCount, m_bits, sim::UnitBits); iter; ++iter) {
    sim::Unit valid;
    iter.srcGet(buffer, nullptr, &valid);
    changed = changed || iter.dstSetChanged(m_buffer, nullptr, &valid);
  }
  return changed;
}


BitVector BitVector::slice(size_t bitPos, size_t bitCount) const
{
  BitVector res(bitCount);
  res.set(0, bitCount, *this, bitPos);
  return res;
}

bool BitVector::allValid(size_t bitPos, size_t bitCount) const
{
  if (bitPos >= m_bits) {
    return false;
  }
  for (BitIter iter(bitPos, bitCount, m_bits); iter; ++iter) {
    if (iter.dstGetInvalid(m_buffer)) {
      return false;
    }
  }
  return true;
}

bool BitVector::anyValid(size_t bitPos, size_t bitCount) const
{
  for (BitIter iter(bitPos, bitCount, m_bits); iter; ++iter) {
    if (iter.dstGetValid(m_buffer)) {
      return true;
    }
  }
  return false;
}

void BitVector::set(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit value, valid;
    iter.srcGet(other.m_buffer, &value, &valid);
    iter.dstSet(m_buffer, &value, &valid);
  }
}

void BitVector::setValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit value;
    iter.srcGet(other.m_buffer, &value, nullptr);
    iter.dstSet(m_buffer, &value, nullptr);
  }
}

void BitVector::setValid(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit valid;
    iter.srcGet(other.m_buffer, nullptr, &valid);
    iter.dstSet(m_buffer, nullptr, &valid);
  }
}

void BitVector::setValidFromValue(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit valid;
    iter.srcGet(other.m_buffer, &valid, nullptr);
    iter.dstSet(m_buffer, nullptr, &valid);
  }
}

bool BitVector::setChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  bool changed = false;
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit value, valid;
    iter.srcGet(other.m_buffer, &value, &valid);
    changed = changed || iter.dstSetChanged(m_buffer, &value, &valid);
  }
  return changed;
}

bool BitVector::setValueChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  bool changed = false;
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit value;
    iter.srcGet(other.m_buffer, &value, nullptr);
    changed = changed || iter.dstSetChanged(m_buffer, &value, nullptr);
  }
  return changed;
}

bool BitVector::setValidChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  bool changed = false;
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit valid;
    iter.srcGet(other.m_buffer, nullptr, &valid);
    changed = changed || iter.dstSetChanged(m_buffer, nullptr, &valid);
  }
  return changed;
}

bool BitVector::setValidFromValueChanged(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos)
{
  bool changed = false;
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit valid;
    iter.srcGet(other.m_buffer, &valid, nullptr);
    changed = changed || iter.dstSetChanged(m_buffer, nullptr, &valid);
  }
  return changed;
}

bool BitVector::equals(size_t bitPos, size_t bitCount, const BitVector & other, size_t otherPos) const
{
  for (BitIter iter(bitPos, otherPos, bitCount, m_bits, other.m_bits); iter; ++iter) {
    sim::Unit dstValue, dstValid, srcValue, srcValid;
    dstValue = iter.dstGetValue(m_buffer);
    dstValid = iter.dstGetValid(m_buffer);
    iter.srcGet(other.m_buffer, &srcValue, &srcValid);
    if (dstValue != srcValue || dstValid != srcValid) {
      return false;
    }
  }
  return true;
}

bool BitVector::equals(const BitVector & other) const
{
  if (m_bits != other.m_bits) {
    return false;
  }
  return equals(0, m_bits, other, 0);
}


void BitVector::fill(size_t bitPos, size_t bitCount, bool value, bool valid)
{
  sim::Unit fillValue = value? sim::UnitOnes : sim::UnitZeros;
  sim::Unit fillValid = valid? sim::UnitOnes : sim::UnitZeros;
  for (BitIter iter(bitPos, 0, bitCount, m_bits, 0); iter; ++iter) {
    iter.dstSet(m_buffer, &fillValue, &fillValid);
  }
}

void BitVector::fillValue(size_t bitPos, size_t bitCount, bool value)
{
  sim::Unit fillValue = value? sim::UnitOnes : sim::UnitZeros;
  for (BitIter iter(bitPos, 0, bitCount, m_bits, 0); iter; ++iter) {
    iter.dstSet(m_buffer, &fillValue, nullptr);
  }
}

void BitVector::fillValid(size_t bitPos, size_t bitCount, bool valid)
{
  sim::Unit fillValid = valid? sim::UnitOnes : sim::UnitZeros;
  for (BitIter iter(bitPos, 0, bitCount, m_bits, 0); iter; ++iter) {
    iter.dstSet(m_buffer, nullptr, &fillValid);
  }
}

bool BitVector::fillChanged(size_t bitPos, size_t bitCount, bool value, bool valid)
{
  bool changed = false;
  sim::Unit fillValue = value? sim::UnitOnes : sim::UnitZeros;
  sim::Unit fillValid = valid? sim::UnitOnes : sim::UnitZeros;
  for (BitIter iter(bitPos, 0, bitCount, m_bits, 0); iter; ++iter) {
    changed = changed || iter.dstSetChanged(m_buffer, &fillValue, &fillValid);
  }
  return changed;
}

bool BitVector::fillValueChanged(size_t bitPos, size_t bitCount, bool value)
{
  bool changed = false;
  sim::Unit fillValue = value? sim::UnitOnes : sim::UnitZeros;
  for (BitIter iter(bitPos, 0, bitCount, m_bits, 0); iter; ++iter) {
    changed = changed || iter.dstSetChanged(m_buffer, &fillValue, nullptr);
  }
  return changed;
}

bool BitVector::fillValidChanged(size_t bitPos, size_t bitCount, bool valid)
{
  bool changed = false;
  sim::Unit fillValid = valid? sim::UnitOnes : sim::UnitZeros;
  for (BitIter iter(bitPos, 0, bitCount, m_bits, 0); iter; ++iter) {
    changed = changed || iter.dstSetChanged(m_buffer, nullptr, &fillValid);
  }
  return changed;
}

void BitVector::append(const BitVector & other)
{
  size_t newPos = m_bits;
  resize(m_bits + other.bits());
  set(newPos, other.bits(), other, 0);
}

void BitVector::fromString(const std::string & string)
{
  size_t newBits = 8 * string.size();
  resize(newBits);

  for (size_t idx = 0; idx < string.size(); ++idx) {
    unitSet(8*idx, 8, string.at(idx), sim::UnitOnes);
  }
}

void BitVector::fromHex(const std::string & string)
{
  size_t newBits = 4 * string.size();
  resize(newBits);

  for (size_t idx = 0; idx < string.size(); ++idx) {
    sim::Unit value;
    sim::Unit valid = hexdigit(string.at(idx), value)? sim::UnitOnes : sim::UnitZeros;
    unitSet(4*idx, 4, value, valid);
  }
}

void BitVector::fromBin(const std::string & string)
{
  size_t newBits = string.size();
  resize(newBits);

  for (size_t idx = 0; idx < string.size(); ++idx) {
    sim::Unit value;
    sim::Unit valid = bindigit(string.at(idx), value)? sim::UnitOnes : sim::UnitZeros;
    unitSet(idx, 1, value, valid);
  }
}



std::string BitVector::toString()
{
  size_t count = (m_bits - 1) / 8 + 1;
  std::string str;
  str.reserve(count);
  for (size_t idx = 0; idx < count; ++idx) {
    if (allValid(8 * idx, 8)) {
      str.append(1, unitValue(8 * idx, 8));
    }
  }
  return str;
}

std::string BitVector::toHex()
{
  size_t count = (m_bits - 1) / 4 + 1;
  std::string str;
  str.reserve(count);
  for (size_t idx = 0, pos = 4 * (count - 1); idx < count; ++idx, pos -= 4) {
    str.append(1, digithex(unitValue(pos, 4), allValid(pos, 4)));
  }
  return str;
}

std::string BitVector::toBin()
{
  size_t count = m_bits;
  std::string str;
  // std::clog << "toBin " << m_bits << std::endl;
  str.reserve(count);

  for (size_t idx = 0, pos = count - 1; idx < count; ++idx, --pos) {
    str.append(1, digitbin(unitValue(pos, 1), allValid(pos, 1)));
  }
  return str;
}


BitVectorAllocator::BitVectorAllocator()
: m_allocs {}
, m_active {true}
{ }

BitVectorAllocator::~BitVectorAllocator()
{
  freeAll();
  m_active = false;
}

Unit * BitVectorAllocator::realloc(Unit * buffer, size_t nUnitsOld, size_t nUnitsNew)
{
  // std::clog << "realloc(" << buffer << ", old=" << nUnitsOld << ", new=" << nUnitsNew << ")" << std::endl;
  size_t idxOld, sizeOld;
  bool fitOld = allocSize(nUnitsOld, idxOld, sizeOld);
  size_t idxNew, sizeNew;
  bool fitNew = allocSize(nUnitsNew, idxNew, sizeNew);


  // shortcut for same-size allocation
  if (buffer && nUnitsOld && nUnitsNew && fitOld && fitNew && idxOld == idxNew) {
    // std::clog << "  shortcut" << std::endl;
    return buffer;
  }

  sim::Unit * newBuffer = nullptr;

  if (!nUnitsNew) {
    // std::clog << "  new nullptr" << std::endl;
    newBuffer = nullptr;
  } else if (fitNew && m_active && m_allocs[idxNew].size() > 0) {
    // std::clog << "  new reuse from " << idxNew << std::endl;
    auto & freelist = m_allocs[idxNew];
    newBuffer = freelist.back();
    freelist.pop_back();
  } else {
    // std::clog << "  new allocate " << sizeNew << std::endl;
    newBuffer = new Unit[sizeNew];
  }

  if (newBuffer && buffer) {
    // std::clog << "  copy" << std::endl;
    size_t count = std::min(nUnitsOld, nUnitsOld);
    for (size_t idx = 0; idx < count; ++idx) {
      newBuffer[idx] = buffer[idx];
    }
  }

  if (buffer) {
    if (!m_active || !nUnitsOld || !fitOld) {
      // std::clog << "  old free"<< std::endl;
      delete [] buffer;
    } else {
      // std::clog << "  old return to" << idxOld << std::endl;
      auto & freelist = m_allocs[idxOld];
      freelist.push_back(buffer);
    }
  }

  // std::clog << "=> " << newBuffer << std::endl;
  return newBuffer;
}

Unit * BitVectorAllocator::alloc(size_t nUnits)
{
  return realloc(nullptr, 0, nUnits);
}

void BitVectorAllocator::free(Unit * buffer, size_t nUnits)
{
  realloc(buffer, nUnits, 0);
}

void BitVectorAllocator::freeAll()
{
  for (size_t idx = 0; idx < AllocLogLimit; ++idx) {
    auto & alloc = m_allocs[idx];
    while (!alloc.empty()) {
      Unit * buffer = alloc.back();
      alloc.pop_back();
      delete[] buffer;
    }
  }
}

bool BitVectorAllocator::allocSize(size_t nUnits, size_t & idx, size_t & size)
{
  for (size_t ln = 0; ln < AllocLogLimit; ++ln) {
    if ((1 << ln) >= nUnits) {
      idx = ln;
      size = 2 << ln;
      // std::clog << "allocSize(n=" << nUnits << ", idx=" << idx << ", size=" << size << ") -> true" << std::endl;
      return true;
    }
  }
  size = 2 * nUnits;
  // std::clog << "allocSize(n=" << nUnits << ", idx=-, size=" << size << ") -> false" << std::endl;
  return false;
}

} // namespace sim

