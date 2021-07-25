#pragma once

#include <functional>
#include <map>
#include <random>
#include <type_traits>
#include <optional>

#include "types.hpp"


namespace sim {

template <class T>
struct MemberTest
{
  typedef char Yes[1];
  typedef char No[2];

  template <typename C, C> struct Check;

  struct FakeBase {
    bool last;
    sim::Id id; };

  struct TestSubject : public FakeBase, public T {};

  template <typename C> static No & TestLast(Check<bool FakeBase::*, &C::last> *);
  template <typename> static Yes & TestLast(...);

  template <typename C> static No & TestId(Check<sim::Id FakeBase::*, &C::id> *);
  template <typename> static Yes & TestId(...);

  const static bool haslast = sizeof(TestLast<TestSubject>(0)) == sizeof(Yes);
  const static bool hasid = sizeof(TestId<TestSubject>(0)) == sizeof(Yes);
};

template <typename T, typename std::enable_if<MemberTest<T>::haslast, bool>::type = true>
inline bool _get_last(const T & item) { return item.last; }

template <typename T, typename std::enable_if<!MemberTest<T>::haslast, bool>::type = true>
inline bool _get_last(const T & item) { return true; }

template <typename T, typename std::enable_if<MemberTest<T>::hasid, bool>::type = true>
inline sim::Id _get_id(const T & item) { return item.id; }

template <typename T, typename std::enable_if<!MemberTest<T>::hasid, bool>::type = true>
inline sim::Id _get_id(const T & item) { return 0; }


template<typename T> struct ChannelItem;

template<typename T>
struct ChannelList
{
  ChannelItem<T> * front;
  ChannelItem<T> * back;

  ChannelList()
  : front {nullptr}
  , back {nullptr}
  { }
};

template<typename T>
inline void _unlink(ChannelList<T> & list, ChannelItem<T> * & next, ChannelItem<T> * & prev)
{
  if (!next) {
    list.back = prev;
  } else {
    next->prev = prev;
  }
  if (!prev) {
    list.front = next;
  } else {
    prev->next = next;
  }
  next = nullptr;
  prev = nullptr;
}

template<typename T>
inline void _linkFront(ChannelList<T> & list, ChannelItem<T> * & next, ChannelItem<T> * & prev, ChannelItem<T> * me)
{
  next = list.front;
  prev = nullptr;
  list.front = me;
  if (!next) {
    list.back = me;
  } else {
    next->prev = me;
  }
}

template<typename T>
inline void _linkBack(ChannelList<T> & list, ChannelItem<T> * & next, ChannelItem<T> * & prev, ChannelItem<T> * me)
{
  next = nullptr;
  prev = list.back;
  list.back = me;
  if (!prev) {
    list.front = me;
  } else {
    prev->next = me;
  }
}


template<typename T>
struct ChannelItem
{
  T data;
  sim::Delay blockUntil;
  ChannelItem<T> * next;
  ChannelItem<T> * prev;

  inline bool last() const { return _get_last(data); }
  inline sim::Id id() const { return _get_id(data); }
  inline bool blocked(sim::Delay now) { return !DelayAtOrAfter(now, blockUntil); }

  ChannelItem()
  : data {}
  , blockUntil {0}
  , next {nullptr}
  , prev {nullptr}
  { }

  void unlink(ChannelList<T> & list) { _unlink(list, next, prev); }
  void linkFront(ChannelList<T> & list) { _linkFront(list, next, prev, this); }
  void linkBack(ChannelList<T> & list) { _linkBack(list, next, prev, this); }
};


template<typename T>
struct ChannelContext
{
  ChannelList<T> items;
  ChannelItem<T> * head;
  size_t itemCount;
  size_t packetCount;

  ChannelContext()
  : items {}
  , head {nullptr}
  , itemCount {0}
  , packetCount {0}
  { }
};


using ChannelFlags = uint8_t;
constexpr ChannelFlags ChannelPushNoblock = 0x01;
constexpr ChannelFlags ChannelTakeNoblock = 0x02;
constexpr ChannelFlags ChannelUnordered = 0x10;
constexpr ChannelFlags ChannelFairArbit = 0x20;
constexpr ChannelFlags ChannelPackArbit = 0x40;


template<typename T>
class ChannelBase
{
  using TRndEngine = std::default_random_engine;
  using TRndDist = std::poisson_distribution<sim::Delay>;

public:
  ChannelBase(size_t depth = 0,
          float pushBandwidth = 1.f,
          float takeBandwidth = 1.f,
          float itemLatency = 0.f,
          ChannelFlags flags = 0,
          sim::RndSeed seed = 0);
  ~ChannelBase();

  void reset(sim::RndSeed seed = 0);
  void tick();

  inline size_t itemCount() const;
  inline size_t packetCount() const;

  inline void pushEnable(bool enabled);
  inline bool pushEnable() const;
  inline void takeEnable(bool enabled);
  inline bool takeEnable() const;

  inline void setFreeCallback(std::function<void(void)> callback);
  inline void setPushCallback(std::function<void(sim::Id, bool)> callback);
  inline void setHeadCallback(std::function<void(sim::Id, bool)> callback);
  inline void setTakeCallback(std::function<void(sim::Id, bool)> callback);

protected:
  inline bool mayPush() const;
  inline bool mayTake(ChannelContext<T> & context) const;

  bool selectItem(ChannelContext<T> & context);

  void pushItem(ChannelContext<T> & context, ChannelItem<T> * item);

  ChannelItem<T> * takeItem(ChannelContext<T> & context);

  void clearItems(ChannelContext<T> & context);

  template <typename... Args>
  ChannelItem<T> * allocItem(sim::Delay delay, Args&&... args);
  T freeItem(ChannelItem<T> * item);

  void rndSeed(sim::RndSeed seed);
  inline sim::Delay rndPushDelay();
  inline sim::Delay rndTakeDelay();
  inline sim::Delay rndItemDelay();

  inline bool is(ChannelFlags flag) const;

  inline void doFreeCallback();
  inline void doPushCallback(sim::Id id, bool last);
  inline void doHeadCallback(sim::Id id, bool last);
  inline void doTakeCallback(sim::Id id, bool last);

private:
  ChannelFlags m_flags;
  size_t m_depth;

  bool m_lastFree;
  size_t m_itemCount;
  size_t m_packetCount;

  sim::Delay m_timestamp;
  bool m_pushEnable;
  sim::Delay m_pushTimeout;
  bool m_takeEnable;
  sim::Delay m_takeTimeout;

  sim::RndSeed m_lastSeed;
  TRndEngine m_rndGen;
  TRndDist m_rndDistPushDelay;
  TRndDist m_rndDistTakeDelay;
  TRndDist m_rndDistItemDelay;

  ChannelList<T> m_freelist;

  std::function<void(void)> m_freeCallback;
  std::function<void(sim::Id, bool)> m_pushCallback;
  std::function<void(sim::Id, bool)> m_headCallback;
  std::function<void(sim::Id, bool)> m_takeCallback;
};


template <typename T>
class SingleChannel : public ChannelBase<T>
{
public:
  SingleChannel(size_t depth = 0,
          float pushBandwidth = 1.f,
          float takeBandwidth = 1.f,
          float itemLatency = 0.f,
          ChannelFlags flags = 0,
          sim::RndSeed seed = 0);
  ~SingleChannel();

  void reset(sim::RndSeed seed = 0);
  void tick();

  inline bool free() const;
  template <typename... Args>
  bool push(sim::Delay delay, Args... args);

  const T * head();
  std::optional<T> take();

private:
  ChannelContext<T> m_global;
};

template <typename T>
class MultiChannel : public ChannelBase<T>
{
public:
  MultiChannel(size_t depth = 0,
          float pushBandwidth = 1.f,
          float takeBandwidth = 1.f,
          float itemLatency = 0.f,
          ChannelFlags flags = 0,
          sim::RndSeed seed = 0);
  ~MultiChannel();

  void reset(sim::RndSeed seed = 0);
  void tick();

  inline size_t itemCount(sim::Id id) const;
  inline size_t packetCount(sim::Id id) const;

  inline bool free() const;
  template <typename... Args>
  bool push(sim::Delay delay, Args... args);

  const T * head(sim::Id id);
  std::optional<T> take(sim::Id id);

  const T * head();
  std::optional<T> take();

protected:
  bool arbit() const;

private:
  mutable std::map<sim::Id, ChannelContext<T>> m_perId;

  mutable sim::Id m_curId;
  mutable bool m_latchId;
};

}

namespace sim {

//-----------------------------------------------------------------------------
// ChannelBase
//-----------------------------------------------------------------------------

template <typename T>
ChannelBase<T>::ChannelBase(size_t depth,
                    float pushBandwidth,
                    float takeBandwidth,
                    float itemLatency,
                    ChannelFlags flags,
                    sim::RndSeed seed)
: m_flags {flags}
, m_depth {depth}
, m_lastFree {false}
, m_itemCount {0}
, m_packetCount {0}
, m_timestamp {0}
, m_pushEnable {true}
, m_pushTimeout {0}
, m_takeEnable {true}
, m_takeTimeout {0}
, m_lastSeed {0}
, m_rndGen {}
, m_rndDistPushDelay {(pushBandwidth <= 0.f || pushBandwidth > 1.f)? 0.f : (1.f - pushBandwidth) / pushBandwidth}
, m_rndDistTakeDelay {(takeBandwidth <= 0.f || takeBandwidth > 1.f)? 0.f : (1.f - takeBandwidth) / takeBandwidth}
, m_rndDistItemDelay {(itemLatency < 0.f)? 0.f : itemLatency}
, m_freelist {}
, m_freeCallback {nullptr}
, m_pushCallback {nullptr}
, m_headCallback {nullptr}
, m_takeCallback {nullptr}
{
  rndSeed(seed);
  m_pushTimeout = rndPushDelay();
  m_takeTimeout = rndTakeDelay();
}

template <typename T>
ChannelBase<T>::~ChannelBase()
{
  for (ChannelItem<T> * cur = m_freelist.front, * next; cur; cur = next) {
    next = cur->next;
    delete cur;
  }
}

template <typename T>
void ChannelBase<T>::reset(sim::RndSeed seed)
{
  rndSeed(seed);
  m_itemCount = 0;
  m_packetCount = 0;
  m_timestamp = 0;
  m_pushTimeout = rndPushDelay();
  m_takeTimeout = rndTakeDelay();
}

template <typename T>
void ChannelBase<T>::tick()
{
  ++m_timestamp;
  if (m_pushEnable && m_pushTimeout > 0) --m_pushTimeout;
  if (m_takeEnable && m_takeTimeout > 0) --m_takeTimeout;

  bool nowFree = mayPush();
  if (!m_lastFree && nowFree) {
    doFreeCallback();
  }
  m_lastFree = nowFree;
}


template <typename T>
inline size_t ChannelBase<T>::itemCount() const
{
  return m_itemCount;
}

template <typename T>
inline size_t ChannelBase<T>::packetCount() const
{
  return m_packetCount;
}

template <typename T>
inline void ChannelBase<T>::pushEnable(bool enabled)
{
  m_pushEnable = enabled;
}

template <typename T>
inline bool ChannelBase<T>::pushEnable() const
{
  return m_pushEnable;
}

template <typename T>
inline void ChannelBase<T>::takeEnable(bool enabled)
{
  m_takeEnable = enabled;
}

template <typename T>
inline bool ChannelBase<T>::takeEnable() const
{
  return m_takeEnable;
}

template <typename T>
inline void ChannelBase<T>::setFreeCallback(std::function<void(void)> callback)
{
  m_freeCallback = callback;
}

template <typename T>
inline void ChannelBase<T>::setPushCallback(std::function<void(sim::Id, bool)> callback)
{
  m_pushCallback = callback;
}

template <typename T>
inline void ChannelBase<T>::setHeadCallback(std::function<void(sim::Id, bool)> callback)
{
  m_headCallback = callback;
}

template <typename T>
inline void ChannelBase<T>::setTakeCallback(std::function<void(sim::Id, bool)> callback)
{
  m_takeCallback = callback;
}

template <typename T>
inline bool ChannelBase<T>::mayPush() const
{
  return m_pushTimeout == 0 && (m_depth == 0 || m_itemCount < m_depth);
}

template <typename T>
inline bool ChannelBase<T>::mayTake(ChannelContext<T> & context) const
{
  return m_takeTimeout == 0 && context.head;
}

template <typename T>
bool ChannelBase<T>::selectItem(ChannelContext<T> & context)
{
  for (ChannelItem<T> * it = context.items.front; it; it = it->next) {
    if (!it->blocked(m_timestamp)) {
      context.head = it;
      doHeadCallback(it->id(), it->last());
      return true;
    } else if (!is(ChannelUnordered)) {
      break;
    }
  }
  context.head = nullptr;
  return false;
}

template <typename T>
void ChannelBase<T>::pushItem(ChannelContext<T> & context, ChannelItem<T> * item)
{
  m_pushTimeout = rndPushDelay();

  item->linkBack(context.items);

  ++m_itemCount;
  ++context.itemCount;
  if (item->last()) {
    ++m_packetCount;
    ++context.packetCount;
  }

  doPushCallback(item->id(), item->last());
}

template <typename T>
ChannelItem<T> * ChannelBase<T>::takeItem(ChannelContext<T> & context)
{
  m_takeTimeout = rndTakeDelay();

  ChannelItem<T> * item = context.head;
  item->unlink(context.items);

  selectItem(context);

  --m_itemCount;
  --context.itemCount;
  if (item->last()) {
    --m_packetCount;
    --context.packetCount;
  }

  doTakeCallback(item->id(), item->last());

  return item;
}

template <typename T>
void ChannelBase<T>::clearItems(ChannelContext<T> & context)
{
  for (ChannelItem<T> * cur = context.items.front, * next; cur; cur = next) {
    next = cur->next;
    cur->unlink(context.items);
    freeItem(cur);
  }
  context.head = nullptr;
  context.itemCount = 0;
  context.packetCount = 0;
}

template <typename T>
template <typename... Args>
ChannelItem<T> * ChannelBase<T>::allocItem(sim::Delay delay, Args&&... args)
{
  ChannelItem<T> * item;
  if (m_freelist.front) {
    item = m_freelist.front;
    item->unlink(m_freelist);
  } else {
    item = new ChannelItem<T>;
  }
  item->data = {std::forward<Args>(args)...};
  item->blockUntil = m_timestamp + delay + rndItemDelay();
  return item;
}

template <typename T>
T ChannelBase<T>::freeItem(ChannelItem<T> * item)
{
  item->linkFront(m_freelist);
  return std::move(item->data);
}

template <typename T>
void ChannelBase<T>::rndSeed(sim::RndSeed seed)
{
  if (seed != 0) {
    m_lastSeed = seed ^ (sim::RndSeed)this;
  } else if (m_lastSeed == 0) {
    m_lastSeed = (sim::RndSeed)this;
  }
  m_rndGen.seed(m_lastSeed);
}

template <typename T>
inline uint32_t ChannelBase<T>::rndPushDelay()
{
  return is(ChannelPushNoblock)? 0 : m_rndDistPushDelay(m_rndGen) + 1;
}

template <typename T>
inline uint32_t ChannelBase<T>::rndTakeDelay()
{
  return is(ChannelTakeNoblock)? 0 : m_rndDistTakeDelay(m_rndGen) + 1;
}

template <typename T>
inline uint32_t ChannelBase<T>::rndItemDelay()
{
  return m_rndDistItemDelay(m_rndGen) + 1;
}

template <typename T>
inline bool ChannelBase<T>::is(ChannelFlags flag) const
{
  return m_flags & flag;
}

template <typename T>
inline void ChannelBase<T>::doFreeCallback()
{
  if (m_freeCallback) {
    m_freeCallback();
  }
}

template <typename T>
inline void ChannelBase<T>::doPushCallback(sim::Id id, bool last)
{
  if (m_pushCallback) {
    m_pushCallback(id, last);
  }
}

template <typename T>
inline void ChannelBase<T>::doHeadCallback(sim::Id id, bool last)
{
  if (m_headCallback) {
    m_headCallback(id, last);
  }
}

template <typename T>
inline void ChannelBase<T>::doTakeCallback(sim::Id id, bool last)
{
  if (m_takeCallback) {
    m_takeCallback(id, last);
  }
}


//-----------------------------------------------------------------------------
// SingleChannel
//-----------------------------------------------------------------------------

template <typename T>
SingleChannel<T>::SingleChannel(size_t depth,
                                float pushBandwidth,
                                float takeBandwidth,
                                float itemLatency,
                                ChannelFlags flags,
                                sim::RndSeed seed)
: ChannelBase<T> (depth, pushBandwidth, takeBandwidth, itemLatency, flags, seed)
, m_global { }
{ }

template <typename T>
SingleChannel<T>::~SingleChannel()
{
  ChannelBase<T>::clearItems(m_global);
}

template <typename T>
void SingleChannel<T>::reset(sim::RndSeed seed)
{
  ChannelBase<T>::reset(seed);
  ChannelBase<T>::clearItems(m_global);
}

template <typename T>
void SingleChannel<T>::tick()
{
  ChannelBase<T>::tick();

  if (!m_global.head) {
    ChannelBase<T>::selectItem(m_global);
  }
}

template <typename T>
inline bool SingleChannel<T>::free() const
{
  return ChannelBase<T>::mayPush();
}

template <typename T>
template <typename... Args>
bool SingleChannel<T>::push(sim::Delay delay, Args... args)
{
  if (!ChannelBase<T>::mayPush()) {
    return false;
  }

  ChannelItem<T> * item = ChannelBase<T>::allocItem(delay, std::forward<Args>(args)...);
  ChannelBase<T>::pushItem(m_global, item);
  return true;
}

template <typename T>
const T * SingleChannel<T>::head()
{
  if (!ChannelBase<T>::mayTake(m_global)) {
    return nullptr;
  }

  return &(m_global.head->data);
}

template <typename T>
std::optional<T> SingleChannel<T>::take()
{
  if (!ChannelBase<T>::mayTake(m_global)) {
    return std::nullopt;
  }

  ChannelItem<T> * item = ChannelBase<T>::takeItem(m_global);
  return ChannelBase<T>::freeItem(item);
}

//-----------------------------------------------------------------------------
// MultiChannel
//-----------------------------------------------------------------------------

template <typename T>
  MultiChannel<T>::MultiChannel(size_t depth,
                              float pushBandwidth,
                              float takeBandwidth,
                              float itemLatency,
                              ChannelFlags flags,
                              sim::RndSeed seed)
: ChannelBase<T> (depth, pushBandwidth, takeBandwidth, itemLatency, flags, seed)
, m_perId {}
, m_curId {0}
, m_latchId {false}
{ }

template <typename T>
MultiChannel<T>::~MultiChannel()
{
  for (auto it = m_perId.begin(); it != m_perId.end(); ++it) {
    ChannelContext<T> & perId = it->second;
    ChannelBase<T>::clearItems(perId);
  }
}

template <typename T>
void MultiChannel<T>::reset(sim::RndSeed seed)
{
  ChannelBase<T>::reset(seed);

  m_curId = 0;
  m_latchId = false;
  for (auto it = m_perId.begin(); it != m_perId.end(); ++it) {
    ChannelContext<T> & perId = it->second;
    ChannelBase<T>::clearItems(perId);
  }
}

template <typename T>
void MultiChannel<T>::tick()
{
  ChannelBase<T>::tick();

  for (auto it = m_perId.begin(); it != m_perId.end(); ++it) {
    ChannelContext<T> & perId = it->second;
    if (!perId.head) {
      ChannelBase<T>::selectItem(perId);
    }
  }
}

template <typename T>
inline size_t MultiChannel<T>::itemCount(sim::Id id) const
{
  return m_perId[id].itemCount;
}

template <typename T>
inline size_t MultiChannel<T>::packetCount(sim::Id id) const
{
  return m_perId[id].packetCount;
}


template <typename T>
inline bool MultiChannel<T>::free() const
{
  return ChannelBase<T>::mayPush();
}

template <typename T>
template <typename... Args>
bool MultiChannel<T>::push(sim::Delay delay, Args... args)
{
  if (!ChannelBase<T>::mayPush()) {
    return false;
  }

  ChannelItem<T> * item = ChannelBase<T>::allocItem(delay, std::forward<Args>(args)...);
  ChannelContext<T> & perId = m_perId[item->id()];
  ChannelBase<T>::pushItem(perId, item);
  return true;
}

template <typename T>
const T * MultiChannel<T>::head(sim::Id id)
{
  ChannelContext<T> & perId = m_perId[id];
  if (!ChannelBase<T>::mayTake(perId)) {
    return nullptr;
  }

  return &(perId.head->data);
}

template <typename T>
std::optional<T> MultiChannel<T>::take(sim::Id id)
{
  ChannelContext<T> & perId = m_perId[id];
  if (!ChannelBase<T>::mayTake(perId)) {
    return std::nullopt;
  }

  ChannelItem<T> * item = ChannelBase<T>::takeItem(perId);
  return ChannelBase<T>::freeItem(item);
}

template <typename T>
const T * MultiChannel<T>::head()
{
  if (!m_latchId) {
    m_latchId = arbit();
  }

  ChannelContext<T> & perId = m_perId[m_curId];
  if (!ChannelBase<T>::mayTake(perId)) {
    return nullptr;
  }

  return &(perId.head->data);
}

/* Using take() and take(id) together on the same MultiChannel
 * may cause undesired blocking:
   * m_latchId is only set when arbit detects a valid head once
   * if afterwards the head is taken via take(id),
   * m_latchId will not be cleared so the MultiChannel may block
   * until either another beat or the entire packet on the
   * latched ID become available, even if there are valid heads
   * available on other IDs!
 */

template <typename T>
std::optional<T> MultiChannel<T>::take()
{
  if (!m_latchId) {
    m_latchId = arbit();
  }

  ChannelContext<T> & perId = m_perId[m_curId];
  if (!ChannelBase<T>::mayTake(perId)) {
    return std::nullopt;
  }

  ChannelItem<T> * item = ChannelBase<T>::takeItem(perId);
  if (!ChannelBase<T>::is(ChannelPackArbit) || item->last()) {
    m_latchId = false;
  }

  return ChannelBase<T>::freeItem(item);
}

template <typename T>
bool MultiChannel<T>::arbit() const
{
  bool choiceValid = false;
  sim::Id choiceId = 0;
  sim::Id choicePrio = 0;
  for (auto it = m_perId.begin(); it != m_perId.end(); ++it) {
    sim::Id id = it->first;
    const ChannelContext<T> & ctx = it->second;
    if (ctx.head) {
      sim::Id prio = ChannelBase<T>::is(ChannelFairArbit)? it->first - m_curId - 1 : it->first;
      if (!choiceValid || prio < choicePrio) {
        choiceValid = true;
        choiceId = id;
        choicePrio = prio;
      }
    }
  }
  if (choiceValid) {
    m_curId = choiceId;
  }
  return choiceValid;
}


//-----------------------------------------------------------------------------



// template <typename T>
// void MultiChannel<T>::tick()
// {
//   ++m_timestamp;
//   if (m_pushEnable && m_pushTimeout > 0) --m_pushTimeout;
//   if (m_takeEnable && m_takeTimeout > 0) --m_takeTimeout;

//   // if (!m_head) {
//   //   m_head = select();
//   // }
//   // for (auto it = m_perId.begin(); it != m_perId.end(); ++it) {
//   //   ChannelContext<T> & ctx = it->second;
//   //   if (!ctx.head) {
//   //     ctx.head = selectId(ctx.items);
//   //     // TODO-lw m_headCallback(id, last);
//   //   }
//   // }
// }

// template <typename T>
// inline bool Channel<T>::free() const
// {
//   return m_pushTimeout == 0 && (m_depth == 0 || m_itemCount < m_depth);
// }

// template <typename T>
// bool Channel<T>::push(T && item, sim::Delay delay)
// {
//   if (!free()) {
//     return false;
//   }

//   ChannelItem<T> * it = allocItem();
//   it->item = std::move(item);
//   it->blockUntil = delay;
//   pushItem(it);
//   return true;
// }

// template <typename T>
// bool Channel<T>::push(const T & item, sim::Delay delay)
// {
//   if (!free()) {
//     return false;
//   }

//   ChannelItem<T> * it = allocItem();
//   it->item = item;
//   it->blockUntil = delay;
//   pushItem(it);
//   return true;
// }


// template <typename T>
// const T * Channel<T>::headId(sim::Id id)
// {
//   ChannelContext<T> & ctx = m_perId[id];
//   if (m_takeTimeout > 0 || !ctx.head) {
//     return nullptr;
//   }

//   return &(ctx.head->item);
// }

// template <typename T>
// std::optional<T> Channel<T>::takeId(sim::Id id)
// {
//   ChannelContext<T> & ctx = m_perId[id];
//   if (m_takeTimeout > 0 || !ctx.head) {
//     return std::nullopt;
//   }

//   ChannelItem<T> * it = ctx.head;
//   takeItem(it, ctx);
//   return std::move(it->item);
// }

// template <typename T>
// const T * Channel<T>::headArbit()
// {
//   if (!m_latchId) {
//     m_latchId = arbit(); // latch (new) m_curId if arbit() succeeds
//   }

//   ChannelContext<T> & ctx = m_perId[m_curId];
//   if (m_takeTimeout > 0 || !ctx.head) {
//     // TODO-lw maybe reset m_latchId to avoid unnecessary blocking on concurrent takeId or takeOrder
//     return nullptr;
//   }

//   return &(ctx.head->item);
// }

// template <typename T>
// std::optional<T> Channel<T>::takeArbit()
// {
//   if (!m_latchId) {
//     m_latchId = arbit(); // latch (new) m_curId if arbit() succeeds
//   }

//   ChannelContext<T> & ctx = m_perId[m_curId];
//   if (m_takeTimeout > 0 || !ctx.head) {
//     // TODO-lw maybe reset m_latchId to avoid unnecessary blocking on concurrent takeId or takeOrder
//     return std::nullopt;
//   }

//   ChannelItem<T> * it = ctx.head;
//   takeItem(it, ctx);
//   if (!isPackArbit() || it->last()) {
//     m_latchId = false; // rearbit on either any beat or only last beat in packet
//   }
//   return std::move(it->item);
// }
// 

// template <typename T>
// const T * Channel<T>::headOrder()
// {
//   if (m_takeTimeout > 0 || !m_head) {
//     return nullptr;
//   }

//   return &(m_head->item);
// }

// template <typename T>
// std::optional<T> Channel<T>::takeOrder()
// {
//   if (m_takeTimeout > 0 || !m_head) {
//     return std::nullopt;
//   }

//   ChannelItem<T> * it = m_head;
//   ChannelContext<T> & ctx = m_perId[it->id()];
//   takeItem(it, ctx);
//   return std::move(it->item);
// }

// template <typename T>
// bool Channel<T>::arbit() const
// {
//   bool choiceValid = false;
//   sim::Id choiceId = 0;
//   sim::Id choicePrio = 0;
//   for (auto it = m_perId.begin(); it != m_perId.end(); ++it) {
//     sim::Id id = it->first;
//     const ChannelContext<T> & ctx = it->second;
//     if (ctx.head) {
//       sim::Id prio = isFairArbit()? it->first - m_curId - 1 : it->first;
//       if (!choiceValid || prio < choicePrio) {
//         choiceValid = true;
//         choiceId = id;
//         choicePrio = prio;
//       }
//     }
//   }
//   if (choiceValid) {
//     m_curId = choiceId;
//   }
//   return choiceValid;
// }

// template <typename T>
// ChannelItem<T> * Channel<T>::selectId(ChannelList<T> & items)
// {
//   for (ChannelItem<T> * it = items.front; it; it = it->idNext) {
//     if (!it->blocked(m_timestamp)) {
//       return it;
//     } else if (!isUnordered()) {
//       break;
//     }
//   }
//   return nullptr;
// }


// // template <typename T>
// // inline bool Channel<T>::isTakeNoblock() const
// // {
// //   return m_flags & ChannelTakeNoblock;
// // }

// // template <typename T>
// // inline bool Channel<T>::isUnordered() const
// // {
// //   return m_flags & ChannelUnordered;
// // }

// // template <typename T>
// // inline bool Channel<T>::isFairArbit() const
// // {
// //   return m_flags & ChannelFairArbit;
// // }

// // template <typename T>
// // inline bool Channel<T>::isPackArbit() const
// // {
// //   return m_flags & ChannelPackArbit;
// // }
}; // namespace sim
