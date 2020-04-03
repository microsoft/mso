// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/*
 * Utility methods for converting tag string to integer value and vice versa
 */
#pragma once

#ifndef LIBLET_TAGUTILS_TAGUTILS_H
#define LIBLET_TAGUTILS_TAGUTILS_H

#include <sal.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <tagUtils/tagTypes.h>

#ifdef __cplusplus

/**
  MAX_TAG_CCH denotes the maximum tag string buffer size, including null termination.
*/
const uint32_t MAX_TAG_CCH = 6;

namespace TaggingUtilities {
namespace details {
template <typename T>
struct SymbolSpace
{
  const T lowAlpha;
  const T highAlpha;
  const T lowDecimal;
  const T highDecimal;
  T const* const empty = nullptr;
  T const value[1] = "*";
};

template <>
struct SymbolSpace<char>
{
  SymbolSpace<char>() noexcept {};
  SymbolSpace<char>& operator=(const SymbolSpace<char>&) = delete;

  const char lowAlpha = 'a';
  const char highAlpha = 'z';
  const char lowDecimal = '0';
  const char highDecimal = '9';
  char const* const empty = "00000";
  char const value[65] = "abcdefghijklmnopqrstuvwxyz0123456789****************************";
};

template <>
struct SymbolSpace<wchar_t>
{
  SymbolSpace<wchar_t>() noexcept {};
  SymbolSpace<wchar_t>& operator=(const SymbolSpace<wchar_t>&) = delete;

  const wchar_t lowAlpha = L'a';
  const wchar_t highAlpha = L'z';
  const wchar_t lowDecimal = L'0';
  const wchar_t highDecimal = L'9';
  wchar_t const* const empty = L"00000";
  wchar_t const value[65] = L"abcdefghijklmnopqrstuvwxyz0123456789****************************";
};

template <>
struct SymbolSpace<char16_t>
{
  SymbolSpace<char16_t>() noexcept {};
  SymbolSpace<char16_t>& operator=(const SymbolSpace<char16_t>&) = delete;

  const char16_t lowAlpha = u'a';
  const char16_t highAlpha = u'z';
  const char16_t lowDecimal = u'0';
  const char16_t highDecimal = u'9';
  char16_t const* const empty = u"00000";
  char16_t const value[65] = u"abcdefghijklmnopqrstuvwxyz0123456789****************************";
};

template <>
struct SymbolSpace<char32_t>
{
  SymbolSpace<char32_t>() noexcept {};
  SymbolSpace<char32_t>& operator=(const SymbolSpace<char32_t>&) = delete;

  const char32_t lowAlpha = U'a';
  const char32_t highAlpha = U'z';
  const char32_t lowDecimal = U'0';
  const char32_t highDecimal = U'9';
  char32_t const* const empty = U"00000";
  char32_t const value[65] = U"abcdefghijklmnopqrstuvwxyz0123456789****************************";
};

template <typename T>
static T TagLetterConvertValue(T character) noexcept
{
  static const SymbolSpace<T> ss;

  // a-z
  if (character >= ss.lowAlpha && character <= ss.highAlpha)
    return static_cast<T>(character - ss.lowAlpha);
  // 0-9: 0 is at index 26 in details::m_szSymbolSpace. (z - a) + 1 = (122 - 97) + 1 = 26
  if (character >= ss.lowDecimal && character <= ss.highDecimal)
    return static_cast<T>(character - ss.lowDecimal + (ss.highAlpha - ss.lowAlpha) + 1);

  // should not be calling with characters that are not a-z or 0-9, if they do give it back to them
  return character;
}

static const int m_iMaxNumericTag = 0x0000FFFF;
static const int m_iMinOldSchemeHighByteValue = 36;

inline bool IsUntagged(uint32_t value) noexcept
{
  return (value == 0 || value == UNTAGGED);
}
} // namespace details

template <typename T>
_Success_(return == true) bool NumericTagToString(uint32_t iTag, _Out_z_cap_c_(MAX_TAG_CCH) T* result) noexcept
{
  using ss = details::SymbolSpace<T>;
  ss symbol_space;
  const T* digit_space = symbol_space.value + 26;

  int insertDigit = 0;
  for (int maxDigit = 0; maxDigit < 5; ++maxDigit)
  {
    const uint32_t place = static_cast<uint32_t>(pow(10, (4 - maxDigit)));
    const uint32_t digit = iTag / place;
    if (iTag == 0 || digit || insertDigit)
    {
      iTag -= (place * digit);
      result[insertDigit] = digit_space[digit];
      ++insertDigit;
    }
  }
  result[insertDigit] = 0;

  return true;
}

template <typename T>
_Success_(return == true) bool FourCharTagToString(uint32_t iTag, _Out_z_cap_c_(MAX_TAG_CCH) T* result) noexcept
{
  result[0] = static_cast<T>((iTag >> 24) & 0xFF);
  result[1] = static_cast<T>((iTag >> 16) & 0xFF);
  result[2] = static_cast<T>((iTag >> 8) & 0xFF);
  result[3] = static_cast<T>((iTag)&0xFF);
  result[4] = 0;
  return true;
}

template <typename T>
_Success_(return == true) bool FiveCharTagToString(uint32_t iTag, _Out_z_cap_c_(MAX_TAG_CCH) T* result) noexcept
{
  using ss = details::SymbolSpace<T>;
  ss symbol_space;
  result[0] = symbol_space.value[(iTag >> 24) & 0x3F];
  result[1] = symbol_space.value[(iTag >> 18) & 0x3F];
  result[2] = symbol_space.value[(iTag >> 12) & 0x3F];
  result[3] = symbol_space.value[(iTag >> 6) & 0x3F];
  result[4] = symbol_space.value[(iTag)&0x3F];
  result[5] = 0;
  return true;
}

template <typename T>
_Success_(return
          == true) bool ConvertTagToStringInternal(uint32_t iTag, _Out_z_cap_(cch) T* result, size_t cch) noexcept
{
  if (cch < MAX_TAG_CCH || result == nullptr)
  {
    return false;
  }

  if (iTag <= details::m_iMaxNumericTag)
  {
    return NumericTagToString(iTag, result);
  }

  if ((iTag >> 24) >= details::m_iMinOldSchemeHighByteValue)
  {
    return FourCharTagToString(iTag, result);
  }

  return FiveCharTagToString(iTag, result);
}

/**
  Converts integer tag into equivalent string
*/
inline bool ConvertiTagToSzTag(uint32_t iTag, _Out_z_cap_(iBufSize) char* result, size_t iBufSize) noexcept
{
  return ConvertTagToStringInternal(iTag, result, iBufSize);
}

/**
  Converts integer tag into equivalent wstring
*/
inline bool ConvertiTagToWzTag(uint32_t iTag, _Out_z_cap_(iBufSize) wchar_t* result, size_t iBufSize) noexcept
{
  return ConvertTagToStringInternal(iTag, result, iBufSize);
}

/**
  ConvertTagToString can be used with fixed buffers.
  Use ConvertiTagTo[Sz|Wz]Tag for non-fixed buffers.

  wchar_t wzTag[MAX_TAG_CCH];
  ConvertTagToString(tag, wzTag);
*/
template <size_t cch>
inline void ConvertTagToString(uint32_t iTag, _Out_ _Post_z_ char (&szOut)[cch]) noexcept
{
  static_assert(cch >= MAX_TAG_CCH, "buffer is too small");
  (void)ConvertiTagToSzTag(iTag, szOut, cch);
}

template <size_t cch>
inline void ConvertTagToString(uint32_t iTag, _Out_ _Post_z_ wchar_t (&wzOut)[cch]) noexcept
{
  static_assert(cch >= MAX_TAG_CCH, "buffer is too small");
  (void)ConvertiTagToWzTag(iTag, wzOut, cch);
}

template <size_t cch>
inline void ConvertTagToString(uint32_t iTag, _Out_ _Post_z_ char16_t (&wzOut)[cch]) noexcept
{
  static_assert(cch >= MAX_TAG_CCH, "buffer is too small");
  (void)ConvertTagToStringInternal(iTag, wzOut, cch);
}

template <size_t cch>
inline void ConvertTagToString(uint32_t iTag, _Out_ _Post_z_ char32_t (&wzOut)[cch]) noexcept
{
  static_assert(cch >= MAX_TAG_CCH, "buffer is too small");
  (void)ConvertTagToStringInternal(iTag, wzOut, cch);
}

/**
  Converts character buffer to integer tag
*/
template <typename T>
inline uint32_t ConvertStringToTagInternal(_In_count_(cch) const T* wz, size_t cch) noexcept
{
  if (4 == cch)
  {
    return static_cast<uint32_t>((wz[0] << 24) | (wz[1] << 16) | (wz[2] << 8) | wz[3]);
  }

  if (5 == cch)
  {
    return static_cast<uint32_t>(
        ((((((((details::TagLetterConvertValue(wz[0])) << 6) + details::TagLetterConvertValue(wz[1])) << 6)
            + details::TagLetterConvertValue(wz[2]))
           << 6)
          + details::TagLetterConvertValue(wz[3]))
         << 6)
        + details::TagLetterConvertValue(wz[4]));
  }

  return 0;
}

inline uint32_t ConvertStringToTag(_In_count_(cch) const wchar_t* wz, size_t cch) noexcept
{
  return ConvertStringToTagInternal(wz, cch);
}

inline uint32_t ConvertStringToTag(_In_count_(cch) const char* wz, size_t cch) noexcept
{
  return ConvertStringToTagInternal(wz, cch);
}

inline uint32_t ConvertStringToTag(_In_count_(cch) const char16_t* wz, size_t cch) noexcept
{
  return ConvertStringToTagInternal(wz, cch);
}

inline uint32_t ConvertStringToTag(_In_count_(cch) const char32_t* wz, size_t cch) noexcept
{
  return ConvertStringToTagInternal(wz, cch);
}

/**
  Converts character buffer to integer tag inferring the buffer size
*/
template <typename T, size_t cch>
inline uint32_t ConvertStringToTag(T (&wzOut)[cch]) noexcept
{
  static_assert(cch > 0, "No point in converting a zero sized buffer");
  static_assert(cch <= MAX_TAG_CCH, "No parsed buffer should be greater than MAX_TAG_CCH");
  // inferring the size assumes that the string in the buffer is null terminated so we
  // ignore the last character since this is the equivalent of what strlen/wcslen do
  return ConvertStringToTag(wzOut, cch - 1);
}

/**
  A RAII holder converting a tag value into a string
*/
template <typename T>
class TagHolder
{
public:
  /**
  @param tag value
  @param bool value indicating whether or not untagged values should result in an empty string
  */
  TagHolder(uint32_t iTag, bool formatZero) noexcept
  {
    if (!formatZero && details::IsUntagged(iTag))
      return;

    ConvertTagToString(iTag, m_szTag);
  }

  /**
  @param tag value
  */
  TagHolder(uint32_t iTag) noexcept : TagHolder(iTag, true) {}

  /**
  @return a pointer to a null-terminated character buffer
  @deprecated prefer c_str() instead
  */
  _Ret_z_ T const* GetPCWSTR() const noexcept
  {
    return this->c_str();
  }

  /**
  @return a pointer to a null-terminated character buffer
  */
  _Ret_z_ T const* c_str() const noexcept
  {
    return m_szTag;
  }

  /**
  @return a pointer to a null-terminated character buffer
  */
  _Ret_z_ operator T const *() const noexcept
  {
    return this->c_str();
  }

private:
  T m_szTag[MAX_TAG_CCH] = {0};
};

using WzTag = TagHolder<wchar_t>;
using SzTag = TagHolder<char>;
} // namespace TaggingUtilities

#endif // __cplusplus

#endif // LIBLET_TAGUTILS_TAGUTILS_H