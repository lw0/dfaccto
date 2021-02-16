#include "streammodel.hpp"

#include <iomanip>
#include <iostream>
#include <vector>


StreamModel::StreamModel(uint32_t totalSize, uint32_t burstSize, float bandwidth, float latency)
: m_state {Init}
, m_totalSize {totalSize}
, m_burstSize {burstSize}
, m_totalCount {0}
, m_burstCount {0}
, m_delay {0}
, m_rndGen {}
, m_rndDistBeat {(bandwidth <= 0 || bandwidth > 1)? 0 : (1 - bandwidth) / bandwidth}
, m_rndDistBurst {(latency < 0)? 0 : latency}
{
  // std::cout << "Setup StreamModel with " << totalSize << " / " << burstSize << " @" << bandwidth << "|" << latency << std::endl;
}

void StreamModel::reset()
{
  m_state = Init;
  m_totalCount = 0;
  m_burstCount = 0;
  m_delay = 0;
}

void StreamModel::tick(bool active_in, bool last_in)
{
  switch (m_state) {
  case Init:
    if (!m_totalSize || m_totalCount < m_totalSize) {
      m_delay = burstDelay();
      if (m_delay > 0) {
        m_state = WaitDelay;
      } else {
        m_state = WaitAck;
      }
    } else {
      m_state = Done;
    }
    break;

  case WaitDelay:
    m_delay--;
    if (m_delay == 0) {
      m_state = WaitAck;
    }
    break;

  case WaitAck:
    if (active_in) {
      m_totalCount++;
      m_burstCount++;
      if (m_totalSize && m_totalCount >= m_totalSize) {
        m_state = Done;
      } else {
        if (m_last || last_in) {
          m_burstCount = 0;
          m_delay = burstDelay();
        } else {
          m_delay = beatDelay();
        }
      }
      if (m_delay > 0) {
        m_state = WaitDelay;
      } // else remain active in WaitAck
    }
    break;

  case Done:
    // Only left through reset()
    break;
  }

  // "combinatorial" logic for m_valid, m_last from m_state
  m_active = m_state == WaitAck;
  m_last = m_state == WaitAck && (m_burstCount >= m_burstSize - 1 || m_burstSize && m_totalCount >= m_totalSize - 1);
  m_done = m_state == Done;

  switch (m_state) {
  case Init:
    std::cout << "Init      "; break;
  case WaitDelay:
    std::cout << "WaitDelay "; break;
  case WaitAck:
    std::cout << "WaitAck   "; break;
  case Done:
    std::cout << "Done      "; break;
  }
  std::cout << std::setw(3) << std::setfill('0') << m_totalCount << " / " << m_burstCount << " ";
  std::cout << (active_in? ">ACT " : "     ") << (last_in? ">LST " : "     ");
  std::cout << (m_active? "ACT> " : "     ") << (m_last? "LST> " : "     ") << std::endl;
}

