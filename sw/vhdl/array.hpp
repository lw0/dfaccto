#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>



namespace sim::vhdl {

struct Bounds
{
  std::int32_t left;
  std::int32_t right;
  std::int32_t dir;
  std::int32_t length;
};

template <typename T>
struct Array
{
  T * data;
  Bounds * bounds;

  inline std::int32_t size() const;
  inline bool downto() const;
  inline std::int32_t leftIdx() const;
  inline std::int32_t rightIdx() const;
  inline std::int32_t lowIdx() const;
  inline std::int32_t highIdx() const;

  inline std::size_t toPhy(std::int32_t logIdx) const;
  inline std::int32_t toLog(std::size_t phyIdx) const;

  inline T & operator[](std::size_t phyIdx);
  inline const T & operator[](std::size_t phyIdx) const;
};

} // namespace sim::vhdl


namespace sim::vhdl {

template <typename T>
inline std::int32_t Array<T>::size() const
{
  return bounds->length;
}

template <typename T>
inline bool Array<T>::downto() const
{
  return bounds->dir == 1;
}

template <typename T>
inline std::int32_t Array<T>::leftIdx() const
{
  return bounds->left;
}

template <typename T>
inline std::int32_t Array<T>::rightIdx() const
{
  return bounds->right;
}

template <typename T>
inline std::int32_t Array<T>::lowIdx() const
{
  return downto()? rightIdx() : leftIdx();
}

template <typename T>
inline std::int32_t Array<T>::highIdx() const
{
  return downto()? leftIdx() : rightIdx();
}

template <typename T>
inline std::size_t Array<T>::toPhy(std::int32_t logIdx) const
{
  return downto()? leftIdx() - logIdx : logIdx - leftIdx();
}

template <typename T>
inline std::int32_t Array<T>::toLog(std::size_t phyIdx) const
{
  return downto()? leftIdx() - phyIdx : phyIdx + leftIdx();
}

template <typename T>
inline T & Array<T>::operator[](std::size_t phyIdx)
{
  return data[phyIdx];
}

template <typename T>
inline const T & Array<T>::operator[](std::size_t phyIdx) const
{
  return data[phyIdx];
}

} // namespace sim::vhdl

