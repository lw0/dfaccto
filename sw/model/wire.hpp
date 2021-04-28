#pragma once

#include <map>
#include <string>

#include "../model.hpp"
#include "../paramallocator.hpp"
#include "../types.hpp"


namespace sim::model {

class Wire : public sim::Model
{
  static const SignalCode SigChanged = 0x1;
  static const SignalCode SigMatch = 0x2;

public:
  Wire(Environment & env, const std::string & name);
  virtual ~Wire() override;

  virtual void tick() override;

  inline sim::Unit data();
  void data(sim::Unit data);
  inline sim::Signal sigChanged() const;
  sim::Signal sigMatch(sim::Unit data);

  inline size_t dataBits();
  inline void dataBits(size_t bits);

private:
  sim::Unit m_data;
  size_t m_dataBits;

  std::multimap<sim::Unit, sim::SignalParam> m_matchers;
  sim::ParamAllocator m_matcherIds;
};

} // namespace sim


namespace sim::model {

inline sim::Unit Wire::data()
{
  return m_data;
}

Signal Wire::sigChanged() const
{
  return signalFor(SigChanged);
}

inline size_t Wire::dataBits()
{
  return m_dataBits;
}

inline void Wire::dataBits(size_t bits)
{
  m_dataBits = bits;
}

} // namespace sim

