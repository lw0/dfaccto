#include "buffer.hpp"



namespace sim {

BlockBuffer::BlockBuffer()
: m_list {}
, m_offset {0}
{ }

void BlockBuffer::put(const uint8_t * data, size_t length, bool last)
{
  if (!m_list.size()) {
    m_list.emplace_back();
  }
  auto & buffer = m_list.back();

  buffer.insert(buffer.end(), data, data+length);
  if (last) {
    m_list.emplace_back();
  }

}

void BlockBuffer::take(size_t length)
{
  if (!m_list.size())
    return;

  size_t available = m_list.front().size() - m_offset;
  if (complete() && available <= length) {
    m_list.pop_front();
    m_offset = 0;
  } else {
    m_offset += (available <= length)? available : length;
  }
}

IdBuffer::IdBuffer()
: m_buffers { }
, m_nextId {0}
{ }

bool IdBuffer::select(sim::Id & result, size_t minAvailable, bool fair)
{
  bool choiceValid = false;
  sim::Id choicePrio = 0;
  sim::Id choice = 0;
  for (auto & item : m_buffers) {
    auto & entry = item.second;
    sim::Id id = item.first;
    sim::Id prio = fair? id - m_nextId : id;
    if ((!choiceValid || prio < choicePrio) && (entry.complete() || entry.available() >= minAvailable)) {
      choiceValid = true;
      choicePrio = prio;
      choice = id;
    }
  }
  if (choiceValid) {
    result = choice;
    m_nextId = choice + 1;
    return true;
  } else {
    return false;
  }
}

} // namespace sim
