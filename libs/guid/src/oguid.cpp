/*************************************************************************
  Simple namespace to wrap GUID generation
  Copyright (C) 2004 Microsoft Corporation
*************************************************************************/
#include <guid/oguid.h>
#include <guid/msoguid.h>
#include <crash/verifyElseCrash.h>

const size_t c_cchGUID = 38;

std::wstring OGuid::ToString(const GUID& guid, bool noBraces) noexcept
{
  wchar_t wzGuid[c_cchGUID + 1];
  int cchGuid = ::StringFromGUID2(guid, RgC(wzGuid));
  VerifyElseCrashSzTag(cchGuid == _countof(wzGuid), "Failed to convert guid?", 0x01114612 /* tag_beuys */);

  const wchar_t* wzGuidStart = wzGuid;
  const wchar_t* wzGuidEnd = wzGuid + _countof(wzGuid) - 1;
  if (noBraces)
  {
    wzGuidStart++;
    wzGuidEnd--;
  }

  return {wzGuidStart, wzGuidEnd};
}

/// <summary>
/// Creates a GUID and converts into string
/// </summary>
/// <param name="str">string to return guid</param>
/// <param name="noBraces">if we want braces removed (dafult false)</param>
/// <returns>void</returns>
void OGuid::Create(std::wstring& str, bool noBraces) noexcept
{
  str = ToString(Create(), noBraces);
}

/// <summary>
/// Creates a GUID and converts into string
/// </summary>
/// <param name="str">string to return guid</param>
/// <param name="noBraces">if we want braces removed (dafult false)</param>
/// <returns>void</returns>
std::wstring OGuid::Create(bool noBraces) noexcept
{
  return ToString(Create(), noBraces);
}

/// <summary>
/// Creates a GUID
/// </summary>
/// <param name="guid">newly generated  GUID</param>
/// <returns></returns>
void OGuid::Create(GUID& guid) noexcept
{
  guid = Create();
}

/// <summary>
/// Creates a GUID
/// </summary>
/// <returns>newly generated  GUID</returns>
GUID OGuid::Create() noexcept
{
  GUID guid{};
  HRESULT hr = ::CoCreateGuid(&guid);
  VerifyElseCrashTag(hr == S_OK, 0x0245759c /* tag_crxw2 */);
  return guid;
}