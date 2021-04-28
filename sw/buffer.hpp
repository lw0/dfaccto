#pragma once

#include <map>
#include <list>
#include <vector>

#include "types.hpp"


namespace sim {

class BlockBuffer
{
public:
  BlockBuffer();

  inline void reset();

  inline bool complete() const;
  inline size_t available() const;
  inline const uint8_t * data() const;

  void put(const uint8_t * data, size_t length, bool last = false);
  void take(size_t length);

private:
  std::list<std::vector<uint8_t>> m_list;
  size_t m_offset;
};

class IdBuffer
{
public:
  IdBuffer();

  inline void reset();
  bool select(sim::Id & result, size_t minAvailable = std::numeric_limits<size_t>::max(), bool fair = true);
  inline BlockBuffer & get(sim::Id id);

private:
  std::map<sim::Id, BlockBuffer> m_buffers;
  sim::Id m_nextId;
};

} // namespace sim


namespace sim {

inline void BlockBuffer::reset()
{
  m_list.clear();
  m_offset = 0;
}

inline bool BlockBuffer::complete() const
{
  return m_list.size() > 1;
}

inline size_t BlockBuffer::available() const
{
  return m_list.size()? m_list.front().size() - m_offset : 0;
}

inline const uint8_t * BlockBuffer::data() const
{
  return m_list.size()? m_list.front().data() + m_offset : nullptr;
}

inline void IdBuffer::reset()
{
  m_buffers.clear();
  m_nextId = 0;
}

inline BlockBuffer & IdBuffer::get(sim::Id id)
{
  if (!m_buffers.count(id)) {
    m_buffers.emplace(id, BlockBuffer());
  }
  return m_buffers[id];
}
} // namespace sim
