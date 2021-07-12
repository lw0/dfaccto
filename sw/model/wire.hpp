#pragma once

#include <list>
#include <tuple>
#include <string>

#include "../bitvector.hpp"
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

  inline sim::BitVector & data();
  void changed();

  inline sim::Signal sigChanged() const;
  sim::Signal sigMatch(const sim::BitVector & data);

  inline size_t dataBits();
  void dataBits(size_t bits);

private:
  sim::BitVector m_data;

  std::list<std::tuple<sim::BitVector, sim::SignalParam>> m_matchers;
  sim::ParamAllocator m_matcherIds;
};

} // namespace sim


namespace sim::model {

inline sim::BitVector & Wire::data()
{
  return m_data;
}

Signal Wire::sigChanged() const
{
  return signalFor(SigChanged);
}

inline size_t Wire::dataBits()
{
  return m_data.bits();
}

} // namespace sim

