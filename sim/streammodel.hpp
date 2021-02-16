#pragma once

#include <cstdint>
#include <random>


class StreamModel
{
  enum State {
    Init,
    WaitDelay,
    WaitAck,
    Done
  };

public:
  StreamModel(uint32_t totalSize, uint32_t burstSize, float bandwidth, float latency);

  void reset();

  void tick(bool active_in, bool last_in);

  inline bool active() { return m_active; }
  inline bool last()   { return m_last;   }
  inline bool done()   { return m_done;   }

  inline uint32_t state() { return m_state; }
  inline uint32_t delay() { return m_delay; }
  inline uint32_t count() { return m_totalCount; }
  inline uint32_t burst() { return m_burstCount; }

protected:
  inline uint32_t beatDelay()  { return m_rndDistBeat(m_rndGen);  }
  inline uint32_t burstDelay() { return m_rndDistBurst(m_rndGen); }

private:
  State m_state;
  uint32_t m_totalSize;
  uint32_t m_burstSize;
  uint32_t m_totalCount;
  uint32_t m_burstCount;
  uint32_t m_delay;

  bool m_active;
  bool m_last;
  bool m_done;

  std::default_random_engine m_rndGen;
  std::poisson_distribution<uint32_t> m_rndDistBeat;
  std::poisson_distribution<uint32_t> m_rndDistBurst;
};

