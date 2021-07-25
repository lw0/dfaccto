#pragma once

#include <list>
#include <tuple>
#include <string>

#include "../bitvector.hpp"
#include "../model.hpp"
#include "../paramallocator.hpp"
#include "../types.hpp"
#include "../vhdl/logicarray.hpp"


namespace sim::model {

class Wire : public sim::Model
{
  static const SignalCode SigChanged = 0x1;
  static const SignalCode SigMatch = 0x2;

public:
  Wire(Environment & env, const std::string & name);
  virtual ~Wire() override;

  virtual void tick() override;

  void dataFrom(const sim::vhdl::LogicArray * data);
  void dataTo(sim::vhdl::LogicArray * data);

  inline void data(const sim::BitVector & data);
  inline const sim::BitVector & data() const;

  inline sim::Signal sigChanged() const;
  sim::Signal sigMatch(const sim::BitVector & data);

  inline size_t dataBits();

protected:
  void changed();

private:
  sim::BitVectorFlagged m_data;

  std::list<std::tuple<sim::BitVector, sim::SignalParam>> m_matchers;
  sim::ParamAllocator m_matcherIds;
};

} // namespace sim


namespace sim::model {

inline void Wire::data(const sim::BitVector & data)
{
  m_data.set(0, m_data.bits(), data);
  if (m_data.changed(false)) {
    changed();
  }
}

inline const sim::BitVector & Wire::data() const
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

