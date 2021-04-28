#pragma once

#include <functional>
#include <list>
#include <optional>
#include <random>
#include <set>

#include "types.hpp"


namespace sim {

using ChannelFlags = uint8_t;
constexpr ChannelFlags ChannelPushNoblock = 0x01;
constexpr ChannelFlags ChannelTakeNoblock = 0x02;
constexpr ChannelFlags ChannelUnordered = 0x10;
constexpr ChannelFlags ChannelFair = 0x20;

template <typename T>
class Channel
{
  struct ChannelItem
  {
    T item;
    sim::Id id;
    sim::Delay timeout;
    inline ChannelItem(T && item, sim::Id id, sim::Delay timeout)
    : item {item}
    , id {id}
    , timeout {timeout}
    { }

    inline ChannelItem(const T & item, sim::Id id, sim::Delay timeout)
    : item {item}
    , id {id}
    , timeout {timeout}
    { }
  };

protected:

  using TQueue = std::list<ChannelItem>;
  using TRndEngine = std::default_random_engine;
  using TRndDist = std::poisson_distribution<sim::Delay>;

public:
  Channel(size_t depth = 0,
          float pushBandwidth = 1.f,
          float takeBandwidth = 1.f,
          float itemLatency = 0.f,
          ChannelFlags flags = 0,
          sim::RndSeed seed = 0);

  void reset(sim::RndSeed seed = 0);

  inline bool free() const;
  bool push(T && item, uint32_t id = 0);
  bool push(const T & item, uint32_t id = 0);

  void tick();

  inline const std::optional<T> & head() const;
  std::optional<T> take();

protected:
  std::optional<T> select();

  void rndSeed(sim::RndSeed seed);
  inline sim::Delay rndPushDelay();
  inline sim::Delay rndTakeDelay();
  inline sim::Delay rndItemDelay();

private:
  ChannelFlags m_flags;
  size_t m_depth;

  TQueue m_queue;
  sim::Id m_nextId;
  std::optional<T> m_head;
  sim::Delay m_pushTimeout;
  sim::Delay m_takeTimeout;

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
                    sim::RndSeed seed)
: m_flags {flags}
, m_depth {depth}
, m_queue {}
, m_nextId {}
, m_head {}
, m_pushTimeout {}
, m_takeTimeout {}
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
  m_nextId = 0;
  m_head = std::nullopt;
  m_pushTimeout = rndPushDelay();
  m_takeTimeout = rndTakeDelay();
}

template <typename T>
inline bool Channel<T>::free() const
{
  return !m_pushTimeout && (m_depth == 0 || m_queue.size() < m_depth);
}

template <typename T>
bool Channel<T>::push(T && item, uint32_t id)
{
  if (!free()) {
    return false;
  }
  m_queue.emplace_back(item, id, rndItemDelay());
  m_pushTimeout = rndPushDelay();
  return true;
}

template <typename T>
bool Channel<T>::push(const T & item, uint32_t id)
{
  if (!free()) {
    return false;
  }
  m_queue.emplace_back(item, id, rndItemDelay());
  m_pushTimeout = rndPushDelay();
  return true;
}

template <typename T>
void Channel<T>::tick()
{
  if (m_pushTimeout > 0) --m_pushTimeout;
  if (m_takeTimeout > 0) --m_takeTimeout;
  for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
    if (it->timeout > 0) --(it->timeout);
  }

  if (!m_head && !m_takeTimeout) {
    m_head = select();
    m_takeTimeout = rndTakeDelay();
  }
}

template <typename T>
inline const std::optional<T> & Channel<T>::head() const
{
  return m_head;
}

template <typename T>
std::optional<T> Channel<T>::take()
{
  if (m_head) {
    auto tmp = std::move(*m_head);
    if (!m_takeTimeout) {
      m_head = select();
      m_takeTimeout = rndTakeDelay();
    } else {
      m_head = std::nullopt;
    }
    return tmp;
  } else {
    return std::nullopt;
  }
}

template <typename T>
std::optional<T> Channel<T>::select()
{
  std::set<sim::Id> blocked;
  sim::Id choicePrio;
  auto noChoice = m_queue.end();
  auto choice = noChoice;
  for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
    bool valid = it->timeout == 0;
    if (!blocked.count(it->id)) {
      // Block later items for id: if ordered: always | if unordered: only if valid
      if (!(m_flags & ChannelUnordered) || valid) {
        blocked.insert(it->id);
      }
      sim::Id prio = (m_flags & ChannelFair)? it->id - m_nextId : it->id;
      // Select valid item for id, if no lower prio items are present
      if (valid && (prio < choicePrio || choice == noChoice)) {
        choicePrio = prio;
        choice = it;
      }
    }
  }
  if (choice != noChoice) {
    auto tmp = std::move(choice->item);
    m_queue.erase(choice);
    return tmp;
  } else {
    return std::nullopt;
  }
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
  return (m_flags & ChannelPushNoblock)? 0 : m_rndDistPushDelay(m_rndGen) + 1;
}

template <typename T>
inline uint32_t Channel<T>::rndTakeDelay()
{
  return (m_flags & ChannelTakeNoblock)? 0 : m_rndDistTakeDelay(m_rndGen) + 1;
}

template <typename T>
inline uint32_t Channel<T>::rndItemDelay()
{
  return m_rndDistItemDelay(m_rndGen) + 1;
}

} // namespace sim
