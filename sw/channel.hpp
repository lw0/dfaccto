#pragma once

#include <functional>
#include <list>
#include <optional>
#include <random>
#include <set>
#include <map>

#include "types.hpp"


namespace sim {

using ChannelFlags = uint8_t;
constexpr ChannelFlags ChannelPushNoblock = 0x01;
constexpr ChannelFlags ChannelTakeNoblock = 0x02;
constexpr ChannelFlags ChannelUnordered = 0x10;
constexpr ChannelFlags ChannelFairArbit = 0x20;
constexpr ChannelFlags ChannelPackArbit = 0x40;

template <typename T>
class Channel
{
  struct ChannelItem
  {
    T item;
    sim::Delay timeout;
    sim::Id id;
    bool last;

    inline ChannelItem(T && item, sim::Id id, bool last, sim::Delay timeout)
    : item {std::move(item)}
    , timeout {timeout}
    , id {id}
    , last {last}
    { }

    inline ChannelItem(const T & item, sim::Id id, bool last, sim::Delay timeout)
    : item {item}
    , timeout {timeout}
    , id {id}
    , last {last}
    { }
  };

  using TQueue = std::list<ChannelItem>;
  using TQueueIter = typename TQueue::iterator;
  using THeadMap = std::map<sim::Id, std::optional<TQueueIter>>;

  using TRndEngine = std::default_random_engine;
  using TRndDist = std::poisson_distribution<sim::Delay>;

public:
  // TODO-lw reorganize names and concepts!
  Channel(size_t depth = 0,
          float pushBandwidth = 1.f,
          float takeBandwidth = 1.f,
          float itemLatency = 0.f,
          ChannelFlags flags = 0,
          sim::RndSeed seed = 0,
          bool enablePush = true,
          bool enableTake = true);

  void reset(sim::RndSeed seed = 0);
  void tick();

  inline void enablePush(bool enabled);
  inline void enableTake(bool enabled);

  inline bool free() const;
  bool push(T && item, sim::Id id, bool last);
  bool push(const T & item, sim::Id id, bool last);

  inline const T * head(sim::Id id = 0) const;
  std::optional<T> take(sim::Id id = 0, bool mayArbit = false);

  inline const T * headArbit() const;
  std::optional<T> takeArbit();
  inline sim::Id curId() const;

protected:
  // void update();
  // void updateFor(sim::Id target);
  bool arbit() const;

  void rndSeed(sim::RndSeed seed);
  inline sim::Delay rndPushDelay();
  inline sim::Delay rndTakeDelay();
  inline sim::Delay rndItemDelay();

  inline bool isPushNoblock() const;
  inline bool isTakeNoblock() const;
  inline bool isUnordered() const;
  inline bool isFairArbit() const;
  inline bool isPackArbit() const;

private:
  ChannelFlags m_flags;
  size_t m_depth;

  bool m_enablePush;
  sim::Delay m_pushTimeout;
  bool m_enableTake;
  sim::Delay m_takeTimeout;

  TQueue m_queue;
  mutable THeadMap m_heads;
  mutable sim::Id m_curId;
  mutable bool m_latchId;

  sim::RndSeed m_lastSeed;
  TRndEngine m_rndGen;
  TRndDist m_rndDistPushDelay;
  TRndDist m_rndDistTakeDelay;
  TRndDist m_rndDistItemDelay;
};

} // namespace sim


namespace sim {

template <typename T>
Channel<T>::Channel(size_t depth,
                    float pushBandwidth,
                    float takeBandwidth,
                    float itemLatency,
                    std::uint8_t flags,
                    sim::RndSeed seed,
                    bool enablePush,
                    bool enableTake)
: m_flags {flags}
, m_depth {depth}
, m_enablePush {enablePush}
, m_pushTimeout {}
, m_enableTake {enableTake}
, m_takeTimeout {}
, m_queue {}
, m_heads {}
, m_curId {0}
, m_latchId {false}
, m_lastSeed {0}
, m_rndGen {}
, m_rndDistPushDelay {(pushBandwidth <= 0 || pushBandwidth > 1)? 0 : (1 - pushBandwidth) / pushBandwidth}
, m_rndDistTakeDelay {(takeBandwidth <= 0 || takeBandwidth > 1)? 0 : (1 - takeBandwidth) / takeBandwidth}
, m_rndDistItemDelay {(itemLatency < 0)? 0 : itemLatency}
{
  rndSeed(seed);
  m_pushTimeout = rndPushDelay();
  m_takeTimeout = rndTakeDelay();
}

template <typename T>
void Channel<T>::reset(sim::RndSeed seed)
{
  rndSeed(seed);
  m_queue.clear();
  m_heads.clear();
  m_curId = 0;
  m_latchId = false;
  m_pushTimeout = rndPushDelay();
  m_takeTimeout = rndTakeDelay();
}

template <typename T>
void Channel<T>::tick()
{
  if (m_enablePush && m_pushTimeout > 0) --m_pushTimeout;
  if (m_enableTake && m_takeTimeout > 0) --m_takeTimeout;

  std::set<sim::Id> blocked;
  for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
    if (it->timeout > 0) --(it->timeout);
    bool valid = it->timeout == 0;

    if (!blocked.count(it->id)) {
      if (valid || !isUnordered()) {
        blocked.insert(it->id); // if ordered, all items after invalid item are blocked
      }

      if (valid && !m_heads[it->id]) {
        m_heads[it->id] = it; // assign new head if required and valid
      }
    }
  }
}

template <typename T>
inline void Channel<T>::enablePush(bool enabled)
{
  m_enablePush = enabled;
}

template <typename T>
inline void Channel<T>::enableTake(bool enabled)
{
  m_enableTake = enabled;
}

template <typename T>
inline bool Channel<T>::free() const
{
  return m_pushTimeout == 0 && (m_depth == 0 || m_queue.size() < m_depth);
}

template <typename T>
bool Channel<T>::push(T && item, sim::Id id, bool last)
{
  if (!free()) {
    return false;
  }
  m_queue.emplace_back(std::move(item), id, last, rndItemDelay());
  m_pushTimeout = rndPushDelay();
  return true;
}

template <typename T>
bool Channel<T>::push(const T & item, sim::Id id, bool last)
{
  if (!free()) {
    return false;
  }
  m_queue.emplace_back(item, id, last, rndItemDelay());
  m_pushTimeout = rndPushDelay();
  return true;
}

template <typename T>
inline const T * Channel<T>::head(sim::Id id) const
{
  if (m_takeTimeout > 0 || !m_heads[id])
    return nullptr;

  return &((*m_heads[id])->item);
}

template <typename T>
std::optional<T> Channel<T>::take(sim::Id id, bool mayArbit)
{
  if (m_takeTimeout > 0 || !m_heads[id])
    return std::nullopt;

  TQueueIter it = *m_heads[id];
  T tmp = std::move(it->item);
  if (mayArbit && id == m_curId && (it->last || !isPackArbit())) {
    m_latchId = false; // mark for rearbitration
  }
  m_queue.erase(it);
  m_heads[id] = std::nullopt;
  m_takeTimeout = rndTakeDelay();
  if (m_takeTimeout == 0) {
    // try to find the next head immediately
    for (it = m_queue.begin(); it != m_queue.end(); ++it) {
      if (it->id != id) {
        continue;
      }
      bool valid = it->timeout == 0;
      if (valid) {
        m_heads[id] = it;
      }
      if (!isUnordered() || valid) {
        break;
      }
    }
  }
  return tmp;
}

template <typename T>
inline const T * Channel<T>::headArbit() const
{
  if (!m_latchId) {
    // latch (new) m_curId if arbit() succeeds
    m_latchId = arbit();
  }
  return head(m_curId);
}

template <typename T>
std::optional<T> Channel<T>::takeArbit()
{
  if (!m_latchId) {
    // latch (new) m_curId if arbit() succeeds
    m_latchId = arbit();
  }
  return take(m_curId, true);
}

template <typename T>
inline sim::Id Channel<T>::curId() const
{
  return m_curId;
}

// template <typename T>
// void Channel<T>::update()
// {
//   std::set<sim::Id> blocked;
//   for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
//     if (it->timeout > 0) --(it->timeout);
//     bool valid = it->timeout == 0;

//     if (!blocked.count(it->id)) {
//       if (valid || !isUnordered()) {
//         blocked.insert(it->id); // if ordered, all items after invalid item are blocked
//       }

//       if (valid && !m_heads[it->id]) {
//         m_heads[it->id] = it; // assign new head if required and valid
//       }
//     }
//   }
// }

// template <typename T>
// void Channel<T>::updateFor(sim::Id target)
// {
//   if (m_heads[target].head)
//     return;

//   for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
//     if (it->id != target) {
//       continue;
//     }
//     bool valid = it->timeout == 0;
//     if (valid) {
//       m_heads[target].head = it;
//     }
//     if (!isUnordered() || valid) {
//       return;
//     }
//   }
// }

template <typename T>
bool Channel<T>::arbit() const
{
  return sim::arbit<std::optional<TQueueIter>>(m_curId, m_heads, isFairArbit(),
    [](const std::optional<TQueueIter> & opt){
      return (bool)opt;
    });
}

template <typename T>
void Channel<T>::rndSeed(sim::RndSeed seed)
{
  if (seed != 0) {
    m_lastSeed = seed ^ (sim::RndSeed)this;
  } else if (m_lastSeed == 0) {
    m_lastSeed = (sim::RndSeed)this;
  }
  m_rndGen.seed(m_lastSeed);
}

template <typename T>
inline uint32_t Channel<T>::rndPushDelay()
{
  return isPushNoblock()? 0 : m_rndDistPushDelay(m_rndGen) + 1;
}

template <typename T>
inline uint32_t Channel<T>::rndTakeDelay()
{
  return isTakeNoblock()? 0 : m_rndDistTakeDelay(m_rndGen) + 1;
}

template <typename T>
inline uint32_t Channel<T>::rndItemDelay()
{
  return m_rndDistItemDelay(m_rndGen) + 1;
}

template <typename T>
inline bool Channel<T>::isPushNoblock() const
{
  return m_flags & ChannelPushNoblock;
}

template <typename T>
inline bool Channel<T>::isTakeNoblock() const
{
  return m_flags & ChannelTakeNoblock;
}

template <typename T>
inline bool Channel<T>::isUnordered() const
{
  return m_flags & ChannelUnordered;
}

template <typename T>
inline bool Channel<T>::isFairArbit() const
{
  return m_flags & ChannelFairArbit;
}

template <typename T>
inline bool Channel<T>::isPackArbit() const
{
  return m_flags & ChannelPackArbit;
}

} // namespace sim
