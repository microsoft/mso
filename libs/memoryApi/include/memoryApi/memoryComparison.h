/**
  Include this file to get templetized memory based comparisons, including
  templetized Functors which are handy with STL containers.
*/

#pragma once
#ifndef MSO_PLATFORMADAPTERS_MEMORY_COMPARISON_H
#define MSO_PLATFORMADAPTERS_MEMORY_COMPARISON_H

#include <memory>

namespace Mso::Memory {
template <typename T>
int Compare(const T& lhs, const T& rhs) noexcept
{
  return memcmp(
      static_cast<const void*>(std::addressof(lhs)), static_cast<const void*>(std::addressof(rhs)), sizeof(T));
}

template <typename T>
bool Exact(const T& lhs, const T& rhs) noexcept
{
  return (Mso::Memory::Compare<T>(lhs, rhs) == 0);
}

template <typename T>
bool Less(const T& lhs, const T& rhs) noexcept
{
  return (Mso::Memory::Compare<T>(lhs, rhs) < 0);
}

template <typename T>
bool LessOrEqual(const T& lhs, const T& rhs) noexcept
{
  return (Mso::Memory::Compare<T>(lhs, rhs) <= 0);
}

template <typename T>
bool More(const T& lhs, const T& rhs) noexcept
{
  return (Mso::Memory::Compare<T>(lhs, rhs) > 0);
}

template <typename T>
bool MoreOrEqual(const T& lhs, const T& rhs) noexcept
{
  return (Mso::Memory::Compare<T>(lhs, rhs) >= 0);
}

template <typename T>
class LessFunctor
{
public:
  bool operator()(const T& lhs, const T& rhs) const noexcept
  {
    return Mso::Memory::Less<T>(lhs, rhs);
  }
};

} // namespace Mso::Memory

#endif // MSO_PLATFORMADAPTERS_MEMORY_COMPARISON_H
