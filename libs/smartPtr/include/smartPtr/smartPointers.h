// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Definitions for generic smart pointers
*/
#pragma once
#ifndef LIBLET_CORE_SMARTPOINTERS_H
#define LIBLET_CORE_SMARTPOINTERS_H
#ifdef __cplusplus
#include "smartPointerBase.h"

namespace Mso {

/**
  Manages a pointer allocated with new
*/
template <typename T>
struct TDeleteHelper
{
  static void Free(T pT) noexcept
  {
    delete pT;
  }
};

/**
  Smart pointer for a pointer deleted with delete
  typename T should not include the pointer
  TOwnerPtr<CFoo> pFoo( new CFoo() );

  Prefer to use std::unique_ptr.
*/
template <typename T>
class TOwnerPtr : public THolder<T*, TDeleteHelper<T*>>
{
  typedef THolder<T*, TDeleteHelper<T*>> Super;

public:
  TOwnerPtr() noexcept {}
  /*explicit*/ TOwnerPtr(T* pT) noexcept : Super(pT) {}
  IMPLEMENT_THOLDER_OPERATOR_EQUALS(TOwnerPtr<T>)
  IMPLEMENT_THOLDER_RVALUE_REFS(TOwnerPtr<T>);

  T* get() noexcept
  {
    return Super::Get();
  }
  const T* get() const noexcept
  {
    return Super::Get();
  }
  T* release() noexcept
  {
    return Super::Detach();
  }
  void reset(T* pT) noexcept
  {
    this = pT;
  }

private:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(TOwnerPtr<T>)
};

/**
  Manages a pointer allocated with new[]
*/
template <typename T>
struct TArrayHelper
{
  static void Free(T pT) noexcept
  {
    delete[] pT;
  }
};

/**
  Smart pointer for an array deleted with delete[]
  typename T should not include the array pointer
  TArrayHolder<LPWSTR> rgwz = new LPWSTR[10];

  Prefer to use Mso::MemoryPtr or std::vector.
*/
template <typename T>
class TArrayHolder : public THolder<T*, TArrayHelper<T*>>
{
  typedef THolder<T*, TArrayHelper<T*>> Super;

public:
  TArrayHolder() noexcept {}
  /*explicit*/ TArrayHolder(T* pT) noexcept : Super(pT) {}
  IMPLEMENT_THOLDER_OPERATOR_EQUALS(TArrayHolder)
  IMPLEMENT_THOLDER_RVALUE_REFS(TArrayHolder);

  T* get() noexcept
  {
    return Super::Get();
  }
  const T* get() const noexcept
  {
    return Super::Get();
  }
  T* release() noexcept
  {
    return Super::Detach();
  }
  void reset(T* pT) noexcept
  {
    this = pT;
  }

private:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(TArrayHolder)
};

/**
  Manages a pointer cleaned up with Free()
*/
template <typename T>
struct TFreeHelper
{
  static void Free(T pT) noexcept
  {
    pT->Free();
  }
};

/**
  Smart pointer for pointers deleted by calling Free() on them
  typename T should not include the pointer
  TOwnerIPtr<IFoo> pFoo( new CFoo() );

  Prefer to have a virtual destructor on your interface instead of a Free
  method and use std::unique_ptr, std::shared_ptr, etc.
*/
template <typename T>
class TOwnerIPtr : public THolder<T*, TFreeHelper<T*>>
{
  typedef THolder<T*, TFreeHelper<T*>> Super;

public:
  TOwnerIPtr() noexcept {}
  /*explicit*/ TOwnerIPtr(T* pT) noexcept : Super(pT) {}
  IMPLEMENT_THOLDER_OPERATOR_EQUALS(TOwnerIPtr)
  IMPLEMENT_THOLDER_RVALUE_REFS(TOwnerIPtr);

private:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(TOwnerIPtr)
};

/**
  Manages a pointer cleaned up with Destroy()
*/
template <typename T>
struct TDestroyHelper
{
  static void Free(T pT) noexcept
  {
    pT->Destroy();
  }
};

/**
  Smart pointer for pointers deleted by calling Destroy() on them
  typename T should not include the pointer
  Mso::TDestroyPtr<IFoo> pFoo( new CFoo() );

  Prefer to have a virtual destructor on your interface instead of a Free
  method and use std::unique_ptr, std::shared_ptr, etc.
*/
template <typename T>
class TDestroyPtr : public THolder<T*, TDestroyHelper<T*>>
{
  typedef THolder<T*, TDestroyHelper<T*>> Super;

public:
  TDestroyPtr() noexcept {}
  explicit TDestroyPtr(T* pT) noexcept : Super(pT) {}
  IMPLEMENT_THOLDER_OPERATOR_EQUALS(TDestroyPtr)
  IMPLEMENT_THOLDER_RVALUE_REFS(TDestroyPtr);

private:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(TDestroyPtr)
};

/**
  Calls Finish on the resource
*/
template <typename T>
struct FinishHelper
{
  static void Free(T* pT) noexcept
  {
    pT->Finish();
  }
};

/**
  Helper that takes a function pointer
*/
template <typename T, void (*pfnDeleter)(T)>
struct TPfnHelper
{
  inline static void Free(T pT) noexcept
  {
    pfnDeleter(pT);
  }
};

} // namespace Mso

#endif // __cplusplus

#endif // LIBLET_CORE_SMARTPOINTERS_H
