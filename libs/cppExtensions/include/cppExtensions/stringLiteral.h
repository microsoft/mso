// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Use this type to enforce that an actual literal string is provided.

  void DoSomething(const StringLiteral<char>& input) { s_name = input; }

  DoSomething("Hi"_S);

  To use this class and _S, you need to pull in the namespace.
  using namespace Mso::StringLiterals;

*/
#ifndef _CPPEXTENSIONS_STRINGLITERAL_H_
#define _CPPEXTENSIONS_STRINGLITERAL_H_

#include <sal.h>
#include <stddef.h>
#include <stdint.h>

namespace Mso { namespace StringLiterals {

template <typename char_type>
class StringLiteral;

constexpr StringLiteral<char> operator""_S(const char* str, size_t) noexcept;

/**
  StringLiteral can only be constructed using the user-defined-literal operators below.
*/
template <typename char_type>
class StringLiteral
{
public:
  constexpr operator _Null_terminated_ const char_type *() const noexcept
  {
    return m_string;
  }

  constexpr size_t length() const noexcept
  {
    // std::char_traits<>::length() is preferred, but it's not constexpr (even though it optimizes away).
    // note: using recursion until next version of msvc compiler which supports loops
    return length_worker(m_string);
  }

  constexpr const char_type* const* operator&() const noexcept
  {
    return &m_string;
  }

  // private:
  constexpr StringLiteral(_In_z_ const char_type* str) noexcept : m_string(str) {}

  constexpr static size_t length_worker(_In_z_ const char_type* str) noexcept
  {
    return *str ? length_worker(str + 1) + 1 : 0;
  }

  const char_type* const m_string;

  // friend constexpr StringLiteral<char> operator""_S(const char*, size_t) noexcept;
  // friend constexpr StringLiteral<wchar_t> operator""_S(const wchar_t*, size_t);
  // friend constexpr StringLiteral<char16_t> operator""_S(const char16_t*, size_t);
  // friend constexpr StringLiteral<char32_t> operator""_S(const char32_t*, size_t);
};

constexpr StringLiteral<char> operator""_S(const char* str, size_t) noexcept
{
  return {str};
}

inline constexpr StringLiteral<wchar_t> operator""_S(const wchar_t* str, size_t) noexcept
{
  return {str};
}

inline constexpr StringLiteral<char16_t> operator""_S(const char16_t* str, size_t) noexcept
{
  return {str};
}

inline constexpr StringLiteral<char32_t> operator""_S(const char32_t* str, size_t) noexcept
{
  return {str};
}

/**
  Special tag used with overloads accepting a raw string pointer.

  struct HoldSomething
  {
    HoldSomething(const StringLiteral<char>& input) : m_string(input) {}
    HoldSomething(const char* input, const NonStringLiteral_lifetime_semantics_reviewed&) : m_string(input) {}

    private: const char* m_string = nullptr;
  };

  void SafeCall()
  {
    const char* sz = GetName();
    HoldSomething h(sz, NonStringLiteral_lifetime_semantics_reviewed);
  }
*/
constexpr struct NonStringLiteral_lifetime_semantics_reviewed_t
{
} NonStringLiteral_lifetime_semantics_reviewed{};

}} // namespace Mso::StringLiterals

#endif // _CPPEXTENSIONS_STRINGLITERAL_H_
