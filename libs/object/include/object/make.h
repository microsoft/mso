// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Support for reference counting.
*/

#pragma once

#ifdef __cplusplus

#include <compilerAdapters/compilerWarnings.h>
#include <compilerAdapters/cppMacrosDebug.h>
#include <core/TCntPtr.h>
#include <memoryApi/memoryApi.h>

#pragma pack(push, _CRT_PACKING)
#pragma push_macro("new")
#undef new

namespace Mso {

/**
  Mso::Make creates a new instance of class T and returns a TCntPtr to the instance of type TResult.
  TResult is either the original type T (default), or one of its interfaces.
  Returning an interface type can help to avoid creation of unnecessary TCntPtr template instances.

  Method Make is noexcept depending on the Make policy IsNoExcept value.
*/
template <typename T, typename TResult = T, typename... TArgs>
inline Mso::TCntPtr<TResult> Make(TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
{
  typename T::RefCountPolicy::template MemoryGuard<T> memoryGuard = {};
  T::RefCountPolicy::AllocateMemory(memoryGuard);
  VerifyAllocElseCrashTag(memoryGuard.ObjMemory, 0x01117748 /* tag_bex3i */);

  T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
  Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

  TResult* result = memoryGuard.Obj;
  memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  return Mso::TCntPtr<TResult>(result, /*fDoAddRef:*/ false);
}

/**
  Mso::MakeAlloc is an Mso::Make method for types T that use allocators accepting an argument.
  The allocator argument allows to implement stateful allocators.

  MakeAlloc creates a new instance of class T and returns a TCntPtr to the instance of type TResult.
  TResult is either the original type T (default), or one of its interfaces.
  Returning an interface type can help to avoid creation of unnecessary TCntPtr template instantiations.

  Method MakeAlloc is noexcept depending on the Make policy IsNoExcept value.
*/
template <typename T, typename TResult = T, typename TAllocArg, typename... TArgs>
inline Mso::TCntPtr<TResult> MakeAlloc(TAllocArg&& allocArg, TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
{
  typename T::RefCountPolicy::template MemoryGuard<T> memoryGuard = {};
  T::RefCountPolicy::AllocateMemory(memoryGuard, std::forward<TAllocArg>(allocArg));
  VerifyAllocElseCrashTag(memoryGuard.ObjMemory, 0x01117749 /* tag_bex3j */);

  T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
  Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

  TResult* result = memoryGuard.Obj;
  memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  return Mso::TCntPtr<TResult>(result, /*fDoAddRef:*/ false);
}

/**
  Mso::MakeElseNull creates a new instance of class T and returns a TCntPtr to the instance of type TResult.
  If memory cannot be allocated then it returns nullptr.

  TResult is either the original type T (default), or one of its interfaces.
  Returning an interface type can help to avoid creation of unnecessary TCntPtr template instances.

  Method MakeElseNull is noexcept depending on the Make policy IsNoExcept value.
*/
template <typename T, typename TResult = T, typename... TArgs>
inline Mso::TCntPtr<TResult> MakeElseNull(TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
{
  Mso::TCntPtr<TResult> result; // Hopefully we can benefit from NRVO

  typename T::RefCountPolicy::template MemoryGuard<T> memoryGuard = {};
  T::RefCountPolicy::AllocateMemory(memoryGuard);
  if (memoryGuard.ObjMemory)
  {
    T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
    Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

    result = Mso::TCntPtr<TResult>(memoryGuard.Obj, /*fDoAddRef:*/ false);
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  }

  return result;
}
/**
  Mso::MakeAllocElseNull is an Mso::MakeElseNull method for types T that use allocators accepting an argument.
  The allocator argument allows to implement stateful allocators.
  If memory cannot be allocated then it returns nullptr.

  MakeAllocElseNull creates a new instance of class T and returns a TCntPtr to the instance of type TResult.
  TResult is either the original type T (default), or one of its interfaces.
  Returning an interface type can help to avoid creation of unnecessary TCntPtr template instantiations.

  Method MakeAllocElseNull is noexcept depending on the Make policy IsNoExcept value.
*/
template <typename T, typename TResult = T, typename TAllocArg, typename... TArgs>
inline Mso::TCntPtr<TResult> MakeAllocElseNull(TAllocArg&& allocArg, TArgs&&... args) noexcept(
    T::MakePolicy::IsNoExcept)
{
  Mso::TCntPtr<TResult> result; // Hopefully we can benefit from NRVO

  typename T::RefCountPolicy::template MemoryGuard<T> memoryGuard = {};
  T::RefCountPolicy::AllocateMemory(memoryGuard, std::forward<TAllocArg>(allocArg));
  if (memoryGuard.ObjMemory)
  {
    T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
    Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

    result = Mso::TCntPtr<TResult>(memoryGuard.Obj, /*fDoAddRef:*/ false);
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  }

  return result;
}

// Make policy defines how an instance is created in the pre-allocated memory.
namespace MakePolicy {

/// ThrowCtor MakePolicy passes all parameters to the constructor.
/// ThrowCtor::Make may throw an exception if constructor throws.
/// To allow this class to access private constructor add "friend MakePolicy;" to your class.
BEGIN_DISABLE_WARNING_UNREACHABLE_CODE()
struct ThrowCtor
{
  static const bool IsNoExcept = false;

  template <typename T, typename TMemoryGuard, typename... TArgs>
  static void Make(TMemoryGuard& memoryGuard, TArgs&&... args)
  {
    OACR_POSSIBLE_THROW;
    memoryGuard.Obj = ::new (memoryGuard.ObjMemory) T(std::forward<TArgs>(args)...);
    memoryGuard.ObjMemory = nullptr; // Memory is now controlled by the object. Set to null to avoid memory destruction.
  }
};
END_DISABLE_WARNING_UNREACHABLE_CODE()

MSO_PRAGMA_WARNING(pop)

/// NoThrowCtor MakePolicy passes all parameters to the constructor.
/// NoThrowCtor::Make does not throw exception and crashes the app if constructor throws.
/// To allow this class to access private constructor add "friend MakePolicy;" to your class.
struct NoThrowCtor
{
  static const bool IsNoExcept = true;

  template <typename T, typename TMemoryGuard, typename... TArgs>
  static void Make(TMemoryGuard& memoryGuard, TArgs&&... args) noexcept
  {
    memoryGuard.Obj = ::new (memoryGuard.ObjMemory) T(std::forward<TArgs>(args)...);
    memoryGuard.ObjMemory = nullptr; // Memory is now controlled by the object. Set to null to avoid memory destruction.
  }
};

/// ThrowCtorAndInitializeThis MakePolicy calls default constructor and then passes all parameters to InitializeThis
/// method. ThrowCtor::Make may throw an exception if constructor or InitializeThis throw. To allow this class to access
/// private constructor add "friend MakePolicy;" to your class.
struct ThrowCtorAndInitializeThis
{
  static const bool IsNoExcept = false;

  template <typename T, typename TMemoryGuard, typename... TArgs>
  static void Make(TMemoryGuard& memoryGuard, TArgs&&... args)
  {
    OACR_POSSIBLE_THROW;
    memoryGuard.Obj = ::new (memoryGuard.ObjMemory) T();
    memoryGuard.ObjMemory = nullptr; // Memory is now controlled by the object. Set to null to avoid memory destruction.
    memoryGuard.Obj->InitializeThis(std::forward<TArgs>(args)...);
  }
};

/// NoThrowCtorAndInitializeThis MakePolicy calls default constructor and then passes all parameters to InitializeThis
/// method. NoThrowCtor::Make does not throw exception and crashes the app if constructor or InitializeThis throw. To
/// allow this class to access private constructor add "friend MakePolicy;" to your class.
struct NoThrowCtorAndInitializeThis
{
  static const bool IsNoExcept = true;

  template <typename T, typename TMemoryGuard, typename... TArgs>
  static void Make(TMemoryGuard& memoryGuard, TArgs&&... args) noexcept
  {
    memoryGuard.Obj = ::new (memoryGuard.ObjMemory) T();
    memoryGuard.ObjMemory = nullptr; // Memory is now controlled by the object. Set to null to avoid memory destruction.
    memoryGuard.Obj->InitializeThis(std::forward<TArgs>(args)...);
  }
};

} // namespace MakePolicy

/**
  Default memory allocator for ref counted objects.
*/
struct MakeAllocator
{
  static void* Allocate(size_t size) noexcept
  {
    Debug(Mso::Memory::AutoIgnoreLeakScope lazy);
    return Mso::Memory::AllocateEx(size, Mso::Memory::AllocFlags::ShutdownLeak);
  }

  static void Deallocate(void* ptr) noexcept
  {
    Mso::Memory::Free(ptr);
  }
};

} // namespace Mso

#pragma pop_macro("new")
#pragma pack(pop)

#endif // __cplusplus
