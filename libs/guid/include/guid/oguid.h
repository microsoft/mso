/*************************************************************************
Simple namespace to wrap GUID generation
Copyright (C) 2004 Microsoft Corporation
*************************************************************************/
#pragma once
#ifndef MSO_PLATFORMADAPTERS_OGUID_H
#define MSO_PLATFORMADAPTERS_OGUID_H

#include <codecvt>
#include <compileradapters/cppmacros.h>
#include <compileradapters/functiondecorations.h>
#include <locale>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <string>

#ifdef MS_TARGET_WINDOWS
#include <comBaseApi.h>
#else
#include <guiddef.h>
#endif
/// <summary>
/// Helpers for GUIDs
/// </summary>
namespace OGuid {
LIBLET_PUBLICAPI void Create(std::wstring& str, bool noBraces = false) noexcept;
LIBLET_PUBLICAPI std::wstring Create(bool noBraces) noexcept;
LIBLET_PUBLICAPI void Create(GUID& pguid) noexcept;
LIBLET_PUBLICAPI GUID Create() noexcept;

/// <summary>
/// Convert GUID to guid string format
/// </summary>
/// <param name="guid">input guid</param>
/// <param name="noBraces">if we want no braces (default false)</param>
/// <returns>guid in string form</returns>
LIBLET_PUBLICAPI std::wstring ToString(const GUID& guid, bool noBraces = false) noexcept;

/// <summary>
/// Convert GUID structure to guid string format
/// </summary>
/// <param name="guid">input guid</param>
/// <param name="noBraces">if we want no braces (default false)</param>
/// <returns>guid in string form</returns>
inline std::string ToAsciiString(const GUID& guid, bool noBraces = false) noexcept
{
  auto str = ToString(guid, noBraces);
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;
  return converter.to_bytes(str);
}

/// <summary>
/// Convert GUID structure to guid string format
/// </summary>
/// <param name="guid">input guid</param>
/// <param name="str">output guid string</param>
/// <param name="noBraces">if we want no braces (default false)</param>
/// <returns>void</returns>
/// <remarks></remarks>
inline void GUIDToString(const GUID& guid, std::wstring& strOut, bool noBraces = false) noexcept
{
  auto str = ToString(guid, noBraces);
  std::swap(str, strOut);
}

/// <summary>
/// convert guid string into GUID structure
/// </summary>
/// <param name="str">input guid string</param>
/// <param name="guid">output GUID</param>
/// <returns>void</returns>
/// <remarks></remarks>
/// <exception cref="et::OutOfMemory">memory failure in IIDFromString API</exception>
/// <exception cref="et::InvalidArgument">bad guid string</exception>
/// <exception cref="HRESULT">unexpected failure in IIDFromString API</exception>
inline void StringToGUID(const std::wstring& str, GUID& guid)
{
  std::wstring localStr = str;
  // ensure that the localStr is well formed i.e. GUID is enclosed in {}
  if (localStr[0] != L'{' && localStr[localStr.length() - 1] != L'}')
  {
    std::wstringstream formattedGuidString;
    formattedGuidString << L"{" << localStr << L"}";
    localStr = formattedGuidString.str();
  }
  HRESULT hr = ::IIDFromString(&localStr[0], &guid);
  if (hr == E_INVALIDARG)
  {
    throw std::invalid_argument("Invalid GUID string");
  }
  else if (hr == E_OUTOFMEMORY)
  {
    throw std::runtime_error("memory failure converting string to guid");
  }
  else if (hr != S_OK)
  {
    throw std::exception("Unexpected failure converting string to guid");
  }
}

inline GUID StringToGUID(const std::wstring& str)
{
  GUID guid{};
  StringToGUID(str, guid);
  return guid;
}

inline GUID StringToGUID(const std::string_view& guidStr)
{
  std::wstring wStr(guidStr.data(), guidStr.data() + guidStr.size());
  return StringToGUID(wStr);
}

// a const char* member of the overload set is required as long as we provide both basic_string<> and
// basic_string_view<> due to ambiguous implict construction.  In the future, we should consider if
// its worth it to collapse this entire space down to basic_string_view<> as the only input parameter.
// basic_string<> knows how to implicitly convert to basic_string_view<> and basic_string_view<> has
// a ctor for the CharT* types so the basic_string_view<> type should, in theory, be All We Ever Need (tm)
inline GUID StringToGUID(const char* guidStr)
{
  return StringToGUID(std::string_view(guidStr));
}

// The wc16* workarounds for wstring_view => wstring don't exist yet
#if !MS_TARGET_ANDROID && !MS_TARGET_APPLE

inline GUID StringToGUID(const std::wstring_view& guidStr)
{
  std::wstring wStr(guidStr);
  return StringToGUID(wStr);
}

inline GUID StringToGUID(const wchar_t* guidStr)
{
  return StringToGUID(std::wstring_view(guidStr));
}

#endif

inline std::optional<GUID> TryStringToGUID(const std::wstring& str) noexcept
{
  try
  {
    return StringToGUID(str);
  }
  catch (const std::exception&)
  {
    return std::nullopt;
  }
}

inline std::optional<GUID> TryStringToGUID(const std::string_view& str) noexcept
{
  try
  {
    return StringToGUID(str);
  }
  catch (const std::exception&)
  {
    return std::nullopt;
  }
}

#if !MS_TARGET_ANDROID && !MS_TARGET_APPLE
inline std::optional<GUID> TryStringToGUID(const std::wstring_view& str) noexcept
{
  try
  {
    return StringToGUID(str);
  }
  catch (const std::exception&)
  {
    return std::nullopt;
  }
}
#endif

/// <summary>
/// convert short unformatted guid string (i.e. length 32) into GUID structure
/// </summary>
/// <param name="str">input guid string</param>
/// <param name="guid">output GUID</param>
/// <returns>void</returns>
/// <remarks></remarks>
/// <exception cref="et::OutOfMemory">memory failure in IIDFromString API</exception>
/// <exception cref="et::InvalidArgument">bad guid string</exception>
/// <exception cref="HRESULT">unexpected failure in IIDFromString API</exception>
inline void UnformattedStringToGUID(const std::wstring& str, GUID& guid)
{
  std::wstring localStr = str;
  if (localStr.length() != 32)
  {
    throw std::invalid_argument("Invalid unformatted GUID string");
  }
  // expand the localStr to be well formed i.e. GUID contains "-"
  localStr.insert(0, 1, '{');
  localStr.insert(9, 1, '-');
  localStr.insert(14, 1, '-');
  localStr.insert(19, 1, '-');
  localStr.insert(24, 1, '-');
  localStr.insert(37, 1, '}');

  StringToGUID(localStr, guid);
}

inline bool IsEmptyGuid(const GUID& guid) noexcept
{
  static const GUID emptyGuid = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  return static_cast<bool>(IsEqualGUID(emptyGuid, guid));
}

} // namespace OGuid

#endif // MSO_PLATFORMADAPTERS_OGUID_H