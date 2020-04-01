// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef LIBLET_COMUTIL_QICAST_H
#define LIBLET_COMUTIL_QICAST_H
#ifdef __cplusplus

#include "qiCastCore.h"

#include <platformAdapters/windowsFirst.h>
#include <platformAdapters/msoGuid.h>
#ifdef MS_TARGET_WINDOWS
#include <comBaseApi.h>
#include <unknwn.h>

namespace Mso { namespace ComUtil {

template <typename T>
HRESULT HrCoCreateInstance(
    Mso::TCntPtr<T>& target,
    REFCLSID rclsid,
    _In_opt_ LPUNKNOWN pUnkOuter = nullptr,
    DWORD dwClsContext = CLSCTX_ALL) noexcept
{
  return ::CoCreateInstance(
      rclsid, pUnkOuter, dwClsContext, __uuidof(T), reinterpret_cast<void**>(target.GetAddressOf()));
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
template <typename T>
HRESULT HrCoCreateInstance(
    Mso::TCntPtr<T>& target,
    _In_z_ LPCOLESTR szProgID,
    _In_opt_ LPUNKNOWN pUnkOuter = nullptr,
    DWORD dwClsContext = CLSCTX_ALL) noexcept
{
  CLSID clsid;
  HRESULT hr = ::CLSIDFromProgID(szProgID, &clsid);
  return SUCCEEDED(hr) ? HrCoCreateInstance(target, clsid, pUnkOuter, dwClsContext) : hr;
}
#endif

}} // namespace Mso::ComUtil

namespace Mso {

template <typename T1, typename T2>
__declspec(
    deprecated("Use Mso::ComUtil::AreEqualObjects")) bool AreEqualObjects(const T1* pLeft, const T2* pRight) noexcept
{
  return Mso::ComUtil::AreEqualObjects(pLeft, pRight);
}

} // namespace Mso

#endif // MS_TARGET_WINDOWS

namespace Mso { namespace Details {

// Overloaded global function to provide to IID_PPV_ARGS that support Details::TCntPtrRef
template <typename T>
void** IID_PPV_ARGS_Helper(_Inout_ TCntPtrRef<T> pp) noexcept
{
  static_assert(std::is_base_of<IUnknown, T>::value, "T has to derive from IUnknown");
  return pp;
}

}} // namespace Mso::Details

#endif // __cplusplus
#endif // LIBLET_COMUTIL_QICAST_H
