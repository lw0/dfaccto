#pragma once

#include <vector>

#include "types.hpp"


namespace sim {

class ParamAllocator
{
public:
  ParamAllocator();

  sim::SignalParam alloc();
  void free(sim::SignalParam param);

private:
  std::vector<sim::SignalParam> m_freeParams;
  sim::SignalParam m_nextParam;
};

} // namespace sim

