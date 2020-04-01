// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef LIBLET_COMUTIL_QICASTCORE_H
#define LIBLET_COMUTIL_QICASTCORE_H
#include <guiddef.h>
#include <object/IUnknownShim.h>
#include <core/TCntPtr.h>

//============================================================================
//
//	qi_cast<...>(...)
//
//	Using the qi_cast<> idiom, the construct:
//
//		SPointer _target_;
//		if (SUCCEEDED(_source_->QueryInterface(IID_Type, (void**)_target_.Address())) && _target_) ...
//
//	gets reduced to:
//
//		SPointer _target_ = qi_cast<Type>(_source_);
//		if (_target_) ...
//
//	Note that _source_ is allowed to be null.
//
//	The templates use type inference for the SourceType parameter to avoid
//	having to cast to IUnknown. Casts to IUnknown easily get ambiguous because
//	IUnknown is not usually inherited 'virtual' (as it really should be)
//============================================================================

/**
  qi_cast<Type>(source, optional riid)

  Primary version of the template. Generally speaking, it is preferable to
  add a MSO_CLASS_GUID(T, "") / MSO_STRUCT_GUID(T, "") declaration to the class/interface, or use
  MSO_ASSIGN_CLASS_GUID(T, "") / MSO_ASSIGN_STRUCT_GUID(T, "") if type is already defined. Otherwise it can be
  explicitly passed as a 2nd parameter.
*/
template <typename TTarget, typename TSource>
Mso::TCntPtr<TTarget> qi_cast(TSource& piSource, const IID& riid = __uuidof(TTarget)) noexcept
{
  Mso::TCntPtr<TTarget> spTarget;
  if (piSource != nullptr)
  {
    if (FAILED(piSource->QueryInterface(riid, &spTarget)))
      spTarget.Empty();
  }
  return spTarget;
}

/**
  qi_cast<Type>(const source, optional riid)

  Shorthand for classes and interfaces with an MSO_CLASS_GUID(T, "") / MSO_STRUCT_GUID(T, "") attribute
  or a MSO_ASSIGN_CLASS_GUID(T, "") / MSO_ASSIGN_STRUCT_GUID(T, "") assignment.
  TSource does not include the pointer type, which allows the compiler
  to disambiguate against the primary function.
*/
template <typename TTarget, typename TSource>
Mso::TCntPtr<TTarget> qi_cast(const TSource* piSource, const IID& riid = __uuidof(TTarget)) noexcept
{
  TSource* piSourceNonConst = const_cast<TSource*>(piSource);
  return qi_cast<TTarget, TSource*>(piSourceNonConst, riid);
}

#pragma warning(push)
#pragma warning(disable : 4995) // VerifyElseCrashSz gives "warning C4995: 'IsDebuggerPresent': name was marked as
                                // #pragma deprecated"

/**
  qi_cast_or_crash<Type>(source, optional riid)

  Calls qi_cast and crashes if it fails.
  This is useful when the qi_cast must succeed or otherwise it is a bug.
*/
template <typename TTarget, typename TSource>
Mso::TCntPtr<TTarget> qi_cast_or_crash(TSource& piSource, const IID& riid = __uuidof(TTarget)) noexcept
{
  Mso::TCntPtr<TTarget> target = qi_cast<TTarget>(piSource, riid);
  VerifyElseCrashSzTag(target, "Query interface failed.", 0x022054c3 /* tag_ciftd */);
  return target;
}

/**
  qi_cast_or_crash<Type>(const source, optional riid)

  Calls qi_cast and crashes if it fails.
  This is useful when the qi_cast must succeed or otherwise it is a bug.
*/
template <typename TTarget, typename TSource>
Mso::TCntPtr<TTarget> qi_cast_or_crash(const TSource* piSource, const IID& riid = __uuidof(TTarget)) noexcept
{
  Mso::TCntPtr<TTarget> target = qi_cast<TTarget>(piSource, riid);
  VerifyElseCrashSzTag(target, "Query interface failed.", 0x022054c4 /* tag_cifte */);
  return target;
}

#pragma warning(pop)

/**
  simpleqi_cast<Type>(source, optional riid)

  Primary version of the template. Generally speaking, it is preferable to
  add a MSO_CLASS_GUID(T, "") / MSO_STRUCT_GUID(T, "") declaration to the class/interface, or use
  MSO_ASSIGN_CLASS_GUID(T, "") / MSO_ASSIGN_STRUCT_GUID(T, "") if type is already defined. Otherwise it can be
  explicitly passed as a 2nd parameter.

  Used for Simple interfaces that do not AddRef
*/
template <typename TTarget, typename TSource>
TTarget* simpleqi_cast(TSource& piSource, const IID& riid = __uuidof(TTarget)) noexcept
{
  TTarget* pTarget = nullptr;
  if (piSource != nullptr)
  {
    if (FAILED(piSource->QueryInterface(riid, reinterpret_cast<void**>(&pTarget))))
      pTarget = nullptr;
  }
  return pTarget;
}

/**
  simpleqi_cast<Type>(const source, optional riid)

  Shorthand for classes and interfaces with an MSO_CLASS_GUID(T, "") / MSO_STRUCT_GUID(T, "") attribute
  or a MSO_ASSIGN_CLASS_GUID(T, "") / MSO_ASSIGN_STRUCT_GUID(T, "") assignment.
  TSource does not include the pointer type, which allows the compiler
  to disambiguate against the primary function.
*/
template <typename TTarget, typename TSource>
TTarget* simpleqi_cast(const TSource* piSource, const IID& riid = __uuidof(TTarget)) noexcept
{
  TSource* piSourceNonConst = const_cast<TSource*>(piSource);
  return simpleqi_cast<TTarget, TSource*>(piSourceNonConst, riid);
}

namespace Mso { namespace ComUtil {

template <typename T, typename TOther>
HRESULT HrQueryFrom(Mso::TCntPtr<T>& target, const TOther* pOther, const IID& riid = __uuidof(T)) noexcept
{
  if (pOther == nullptr)
    return E_POINTER;
  return const_cast<TOther*>(pOther)->QueryInterface(riid, reinterpret_cast<void**>(target.ClearAndGetAddressOf()));
}

template <typename T, typename TOther>
HRESULT HrQueryFrom(Mso::TCntPtr<T>& target, const TOther& other, const IID& riid = __uuidof(T)) noexcept
{
  if (other == nullptr)
    return E_POINTER;
  return const_cast<TOther&>(other)->QueryInterface(riid, reinterpret_cast<void**>(target.ClearAndGetAddressOf()));
}

template <typename T, typename TOther>
HRESULT HrQueryFrom(Mso::TCntPtr<T>& target, const Mso::TCntPtr<TOther>& other, const IID& riid = __uuidof(T)) noexcept
{
  if (other == nullptr)
    return E_POINTER;
  return const_cast<Mso::TCntPtr<TOther>&>(other)->QueryInterface(
      riid, reinterpret_cast<void**>(target.ClearAndGetAddressOf()));
}

template <typename T, typename TOther>
bool FQueryFrom(Mso::TCntPtr<T>& target, const TOther* pOther, const IID& riid = __uuidof(T)) noexcept
{
  return SUCCEEDED(HrQueryFrom(target, pOther, riid));
}

template <typename T, typename TOther>
bool FQueryFrom(Mso::TCntPtr<T>& target, const TOther& other, const IID& riid = __uuidof(T)) noexcept
{
  return SUCCEEDED(HrQueryFrom(target, other, riid));
}

template <typename T, typename TOther>
bool FQueryFrom(Mso::TCntPtr<T>& target, const Mso::TCntPtr<TOther>& other, const IID& riid = __uuidof(T)) noexcept
{
  return SUCCEEDED(HrQueryFrom(target, other, riid));
}

/// Checks if two IUnknown objects are equal.
template <typename T1, typename T2>
bool AreEqualObjects(const T1* pLeft, const T2* pRight) noexcept
{
  if (reinterpret_cast<const void*>(pLeft) == reinterpret_cast<const void*>(pRight))
    return true; // Both pointers are the same
  if (pLeft == nullptr || pRight == nullptr)
    return false; // One is null the other is not

  // Compare IUnknown pointers.
  auto punk1 = qi_cast<IUnknown>(pLeft);
  if (!punk1)
    return false;

  auto punk2 = qi_cast<IUnknown>(pRight);
  if (!punk2)
    return false;

  IUnknown* pComp1 = Details::TCntPtrAddRefStrategyImpl<
      Details::AddRefStrategyForType<T1>::TAddRefStrategy::Strategy>::GetIUnknownForObjectCompare(punk1.Get());
  IUnknown* pComp2 = Details::TCntPtrAddRefStrategyImpl<
      Details::AddRefStrategyForType<T2>::TAddRefStrategy::Strategy>::GetIUnknownForObjectCompare(punk2.Get());
  return (pComp1 == pComp2);
}

template <typename T1, typename T2>
bool AreEqualObjects(const T1* pLeft, const Mso::TCntPtr<T2>& right) noexcept
{
  return Mso::ComUtil::AreEqualObjects(pLeft, right.Get());
}

template <typename T1, typename T2>
bool AreEqualObjects(const Mso::TCntPtr<T1>& left, const T2* pRight) noexcept
{
  return Mso::ComUtil::AreEqualObjects(left.Get(), pRight);
}

template <typename T1, typename T2>
bool AreEqualObjects(const Mso::TCntPtr<T1>& left, const Mso::TCntPtr<T2>& right) noexcept
{
  return Mso::ComUtil::AreEqualObjects(left.Get(), right.Get());
}

}} // namespace Mso::ComUtil

#endif // LIBLET_COMUTIL_QICASTCORE_H
