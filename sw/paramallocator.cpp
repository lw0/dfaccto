#include "paramallocator.hpp"

#include <cassert>


namespace sim {

ParamAllocator::ParamAllocator()
: m_freeParams {}
, m_nextParam {0}
{ }

sim::SignalParam ParamAllocator::alloc()
{
  if (m_freeParams.size()) {
    sim::SignalParam param = m_freeParams.back();
    m_freeParams.pop_back();
    return param;
  } else {
    assert(m_nextParam < sim::SignalParamMax);
    return m_nextParam++;
  }
}

void ParamAllocator::free(sim::SignalParam param)
{
  m_freeParams.push_back(param);
}

} // namespace sim

