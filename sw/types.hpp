#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <iostream>
#include <iomanip>
#include <map>
#include <functional>

namespace sim {

using std::size_t;
using std::uint8_t;

using Id = std::uint16_t;
constexpr Id MaxId = std::numeric_limits<Id>::max();

template <typename T>
bool arbit(sim::Id & curId, std::map<sim::Id, T> choices, bool fair, std::function<bool(const T &)> valid)
{
  bool choiceValid = false;
  sim::Id choiceId = 0;
  sim::Id choicePrio = 0;
  for (auto it = choices.begin(); it != choices.end(); ++it) {
    if (valid(it->second)) {
      sim::Id prio = fair? it->first - curId - 1 : it->first;
      if (!choiceValid || prio < choicePrio) {
        choiceValid = true;
        choiceId = it->first;
        choicePrio = prio;
      }
    }
  }
  if (choiceValid) {
    curId = choiceId;
  }
  return choiceValid;
}

using Delay = std::uint32_t;
constexpr Delay DelayMax = std::numeric_limits<Delay>::max();
inline bool DelayAtOrAfter(Delay time, Delay ref)
{
  return (time - ref) <= (DelayMax >> 1);
}

using Ticks = std::uint64_t;
constexpr Ticks TicksMax = std::numeric_limits<Ticks>::max();

using RndSeed = std::uint64_t;
constexpr RndSeed RndSeedMax = std::numeric_limits<RndSeed>::max();

using ModelRef = std::uint32_t;
constexpr ModelRef ModelRefMax = std::numeric_limits<ModelRef>::max();

using SignalCode = std::uint16_t;
constexpr SignalCode SignalCodeMax = std::numeric_limits<SignalCode>::max();

using SignalParam = std::uint16_t;
constexpr SignalParam SignalParamMax = std::numeric_limits<SignalParam>::max();

struct Signal
{
  union {
    struct {
      ModelRef source : 32;
      SignalCode code : 16;
      SignalParam param : 16;
    };
    std::uint64_t flat;
  };

  inline Signal(ModelRef source, SignalCode code, SignalParam param)
  : source {source}
  , code {code}
  , param {param}
  { }

  inline Signal(std::uint64_t flat)
  : flat {flat}
  { }

  inline operator uint64_t () const
  {
    return flat;
  }
};

inline std::ostream & operator<<(std::ostream & os, const Signal & sig) {
  os << std::hex << std::setfill('0') << std::setw(2) << sig.source << ":" << std::setw(2) << sig.code << ":" << std::setw(4) << sig.param << std::setw(0) << std::setfill(' ') << std::dec;
  return os;
}


using Unit = std::uint64_t;

constexpr size_t UnitBits = std::numeric_limits<Unit>::digits;
constexpr Unit UnitZeros = 0;
constexpr Unit UnitOnes = ~UnitZeros;

inline size_t UnitCount(size_t bits) { return (bits == 0)? 0 : (bits - 1) / UnitBits + 1; }
inline size_t UnitIdx(size_t bit) { return bit / UnitBits; }
inline size_t UnitOff(size_t bit) { return bit % UnitBits; }
inline Unit BitAt(size_t off) { return (off < UnitBits)? ((Unit)1) << off : 0; }
inline Unit BitsBelow(size_t off) { return BitAt(off+1) - 1; }
inline Unit BitsAbove(size_t off) { return ~(BitAt(off) - 1); }
inline Unit BitsBetween(size_t high, size_t low) { return BitsBelow(high) & BitsAbove(low); }

inline Unit Mux(Unit mask, Unit val0, Unit val1) { return (~mask & val0) | (mask & val1); }
inline Unit SplitLow(size_t off, Unit value) { return value << off; }
inline Unit SplitHigh(size_t off, Unit value) { return value >> (UnitBits - off); }
inline Unit Join(size_t off, Unit low, Unit high) { return (high << (UnitBits - off)) | (low >> off); }


} // namespace sim
