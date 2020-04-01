// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
This file contains the basic set of allocation APIs that anyone can use:
- Non-throwing Mso::Memory allocation APIs
- Allocation flags
- Mso::MemoryPtr smart pointer for Mso::Memory APIs

See MsoMemory.h for information about operator new.
*/
#pragma once
#ifndef LIBLET_CORE_MEMORYAPI_H
#define LIBLET_CORE_MEMORYAPI_H
#include <compilerAdapters/cppMacrosDebug.h>
#include <memoryApi/memoryApi.h>
#include <platformAdapters/types.h>
#include <safeAlloct.h>
#include <type_traits>

#ifndef MsoSetLazyLeakDetection
#ifdef DEBUG
LIBLET_PUBLICAPI_EX("android", "win") MSOAPI_(void) MsoSetLazyLeakDetection(const void* pv) noexcept;
#else
#define MsoSetLazyLeakDetection(pv) 0
#endif // DEBUG
#endif // !MsoSetLazyLeakDetection

#ifndef MsoSetShutdownLeakDetection
#ifdef DEBUG
LIBLET_PUBLICAPI_EX("android", "win") MSOAPI_(void) MsoSetShutdownLeakDetection(const void* pv) noexcept;
#else
#define MsoSetShutdownLeakDetection(pv) 0
#endif // DEBUG
#endif // !MsoSetShutdownLeakDetection

#ifdef __cplusplus
namespace Mso { namespace LibletAPI {
struct ILibletMemoryMarking
{
  virtual void MarkLiblets(intptr_t lParam) noexcept = 0;
};
}} // namespace Mso::LibletAPI

#ifdef DEBUG
struct IMsoMemHeap;
LIBLET_PUBLICAPI_EX("android", "win")
MSOAPI_(BOOL)
FMemHeapMsoSaveBeHost(void* pinst, LPARAM lParam, const void* pvBlock, LONG_PTR cb, IMsoMemHeap* pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI_EX("android", "win") MSOAPI_(void) MsoCheckShutdownLeaks() noexcept;
LIBLET_PUBLICAPI_EX("win") MSOAPI_(void) HeapEnableLeakTracking(bool isEnabled);
LIBLET_PUBLICAPI_EX("win") MSOAPI_(void) MsoBeforeThreadTerminatesThreaded(DWORD mainThreadId) noexcept;
#endif
LIBLET_PUBLICAPI_EX("android", "win") __declspec(noreturn) void ThrowOOM();

namespace Mso { namespace Memory {
#ifdef DEBUG
LIBLET_PUBLICAPI_EX("android", "win")
MSOCPPAPI_(void) RegisterCallback(Mso::LibletAPI::ILibletMemoryMarking& libletCallback) noexcept;
LIBLET_PUBLICAPI_EX("android", "win")
MSOCPPAPI_(void) UnregisterCallback(Mso::LibletAPI::ILibletMemoryMarking& libletCallback) noexcept;

/**
Helper class to register arbitrary callback for memory marking
*/
struct RegisterMarkMemoryCallback : public Mso::LibletAPI::ILibletMemoryMarking
{
  using MarkMemHandler = std::add_pointer_t<void(intptr_t) noexcept>;

  DECLARE_COPYCONSTR_AND_ASSIGNMENT(RegisterMarkMemoryCallback);
  RegisterMarkMemoryCallback(MarkMemHandler handler) noexcept : m_handler(handler)
  {
    RegisterCallback(*this);
  }

  virtual ~RegisterMarkMemoryCallback() noexcept
  {
    UnregisterCallback(*this);
  }

  void MarkLiblets(intptr_t lParam) noexcept override
  {
    m_handler(lParam);
  }

private:
  MarkMemHandler m_handler;
};
#endif
}} // namespace Mso::Memory

#ifdef DEBUG
namespace Mso { namespace Memory {
// Legacy API
inline _Ret_notnull_ _Post_writable_byte_size_(cb) void* Allocate(size_t cb FILELINEPARAMSUNUSED) noexcept
{
  return AllocateEx(cb, 0);
}

// Legacy API
inline _Ret_maybenull_ _Post_writable_byte_size_(
    cb) void* AllocateEx(size_t cb, DWORD allocFlags FILELINEPARAMSUNUSED) noexcept
{
  return AllocateEx(cb, allocFlags);
}

// Legacy API
inline _Ret_maybenull_ void* Reallocate(_Inout_ void** ppv, size_t cb FILELINEPARAMSUNUSED) noexcept
{
  return Reallocate(ppv, cb);
}
}} // namespace Mso::Memory
#endif

#endif

#endif // LIBLET_CORE_MEMORYAPI_H
