// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
Implementation of a Swarm - a reference counting scheme where multiple objects
share the same reference counter.
****************************************************************************/

#pragma once

#include <memory>

#include <compilerAdapters/compilerWarnings.h>
#include <object/objectWithWeakRef.h>
#include <object/queryCast.h>
#include <object/weakPtr.h>

//
// Swarm is a collection of a ref counted objects that share the same reference counting.
// When the ref counter goes to zero all objects are destroyed.
// Once object becomes part of a swarm it cannot be removed.
// Swarm may have objects of the same or different types.
//
// Any class that is inherited from UnknownObjectWithWeakRef can be part of a swarm.
//
// Use SwarmMemberPtr or WeakPtr to reference swarm members between each other. Using TCntPtr between
// swarm members may cause a memory leak because of an extra ref count to itself.
// SwarmMemberPtr may be used for strong pointers and WeakPtr for weak pointers.
// Do not use the ObjectWeakRef for weak pointers because there is only one ObjectWeakRef for all swarm members.
//
// To create a swarm with a first swarm member use Swarm::Make() static method.
// To add a member to a swarm use MakeMember() instance method.
// To retrieve a swarm from a swarm member use static method Swarm::FromObject().
// To check if object is a part of a swarm use Contains() instance method.
//

#pragma pack(push, _CRT_PACKING)

BEGIN_DISABLE_WARNING_COPY_CTOR_IMPLICITLY_DELETED()
BEGIN_DISABLE_WARNING_ASSIGNMENT_OPERATOR_IMPLICITLY_DELETED()

#pragma push_macro("new")
#undef new

namespace Mso {

namespace Details {

template <typename T>
struct ObjectWeakRefContainerDeleter
{
  void operator()(T* ptr) noexcept
  {
    if (ptr)
    {
      ptr->DestroyContainer();
    }
  }
};

} // namespace Details

/**
  A base class to hold a swarm members.
  A swarm member is allocated as a part of the ObjectWeakRefContainer<T, SwarmMemberHolder> class instance.
  SwarmMemberHolder instances form a single linked list.
*/
class SwarmMemberHolder
{
public:
  using UniquePtrType = std::unique_ptr<SwarmMemberHolder, Details::ObjectWeakRefContainerDeleter<SwarmMemberHolder>>;
  static_assert(
      sizeof(UniquePtrType) == sizeof(void*),
      "SwarmMemberHolder::UniquePtrType must have a pointer size. It is needed for using InterlockedCompareExchangePointer below.");

  virtual ~SwarmMemberHolder() noexcept {}

protected:
  virtual void DestroyObject() noexcept {}

  virtual void DestroyContainer() const noexcept = 0;

private:
  UniquePtrType m_nextMember; // Pointer to the next member in a list.

  friend class Swarm;
  template <typename T>
  friend struct Details::ObjectWeakRefContainerDeleter;
};

// Forward declaration
class Swarm;

template <typename T, typename TContainer>
struct SwarmMemberMemoryGuard
{
  using Type = T;

  ~SwarmMemberMemoryGuard() noexcept;

public: // We use a public fields to reduce number of generated methods.
  // VC++ bug: Make sure that the order of the fields is the same for all memory guards. Otherwise, VC++ generates
  // incorrect code for ship builds.
  void* ObjMemory;
  T* Obj;
  Swarm* Container;
  TContainer* MemberContainer;
};

/**
  Swarm is a special ObjectWeakRef that owns 1 or more objects.
  When Swarm ref count goes to zero it destroys all objects in the Swarm.
  Swarm may have any object inherited from a WeakSourceRefCount.
*/
MSO_CLASS_GUID(Swarm, "A0252DA6-7817-4536-B265-0A0152781652")
class Swarm : public ObjectWeakRef
{
  using Super = ObjectWeakRef;
  using UniquePtrType = SwarmMemberHolder::UniquePtrType;

public:
  Swarm() noexcept : ObjectWeakRef(), m_headMember() {}

  DECLARE_COPYCONSTR_AND_ASSIGNMENT(Swarm)

  template <typename T, typename TResult = T, typename... TArgs>
  static Mso::TCntPtr<TResult> Make(TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
  {
    typename T::RefCountPolicy::template MemoryGuard<T, ObjectWeakRefContainer<T, Swarm>> memoryGuard = {};
    T::RefCountPolicy::AllocateMemory(memoryGuard);
    VerifyAllocElseCrashTag(memoryGuard.ObjMemory, 0x0111774c /* tag_bex3m */);

    T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
    Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

    TResult* result = memoryGuard.Obj;
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
    return Mso::TCntPtr<TResult>(result, /*fDoAddRef*/ false);
  }

  template <typename T, typename TResult = T, typename TAllocArg, typename... TArgs>
  static Mso::TCntPtr<TResult> MakeAlloc(TAllocArg&& allocArg, TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
  {
    typename T::RefCountPolicy::template MemoryGuard<T, ObjectWeakRefContainer<T, Swarm>> memoryGuard = {};
    T::RefCountPolicy::AllocateMemory(memoryGuard, std::forward<TAllocArg>(allocArg));
    VerifyAllocElseCrashTag(memoryGuard.ObjMemory, 0x0111774d /* tag_bex3n */);

    T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
    Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

    TResult* result = memoryGuard.Obj;
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
    return Mso::TCntPtr<TResult>(result, /*fDoAddRef*/ false);
  }

  // We return swarm member as a raw pointer because the new object shares ref count with the swarm and in many cases
  // we want to avoid the extra AddRef/Release because object's lifetime is already tracked.
  BEGIN_DISABLE_WARNING_UNREACHABLE_CODE()
  template <typename T, typename TResult = T, typename... TArgs>
  TResult* MakeMember(TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
  {
    using Container = ObjectWeakRefContainer<T, SwarmMemberHolder>;

    void* containerMemory = T::RefCountPolicy::Allocator::Allocate(sizeof(Container));
    VerifyAllocElseCrashTag(containerMemory, 0x0111774e /* tag_bex3o */);
    Container* container = ::new (containerMemory) Container();
    void* objMemory = container->Get();
    SwarmMemberMemoryGuard<T, Container> memoryGuard = {objMemory, nullptr, this, container};
    Mso::Details::SetWeakRef(memoryGuard.ObjMemory, this);
    AddWeakRef(); // To match ReleaseWeakRef() call in object destructor.

    T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
    Debug(T::RefCountPolicy::ValidateObject(memoryGuard));
    AddSwarmMember(container);

    memoryGuard.MemberContainer = nullptr; // To prevent memoryGuard from destroying the member container.

    TResult* result = memoryGuard.Obj;
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
    return result;
  }
  END_DISABLE_WARNING_UNREACHABLE_CODE()

  template <typename T, typename TResult = T, typename TAllocArg, typename... TArgs>
  TResult* MakeMemberAlloc(TAllocArg&& allocArg, TArgs&&... args) noexcept(T::MakePolicy::IsNoExcept)
  {
    using Container = ObjectWeakRefContainer<T, SwarmMemberHolder>;

    void* containerMemory =
        T::RefCountPolicy::Allocator::Allocate(sizeof(Container), std::forward<TAllocArg>(allocArg));
    VerifyAllocElseCrashTag(containerMemory, 0x0111774f /* tag_bex3p */);
    Container* container = ::new (containerMemory) Container();
    void* objMemory = container->Get();
    SwarmMemberMemoryGuard<T, Container> memoryGuard = {objMemory, nullptr, this, container};
    Mso::Details::SetWeakRef(memoryGuard.ObjMemory, this);
    AddWeakRef(); // To match ReleaseWeakRef() call in object destructor.

    T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
    Debug(T::RefCountPolicy::ValidateObject(memoryGuard));
    AddSwarmMember(container);

    memoryGuard.MemberContainer = nullptr; // To prevent memoryGuard from destroying the member container.

    TResult* result = memoryGuard.Obj;
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
    return result;
  }

  virtual void* QueryCastWeakRef(const GUID& riid) noexcept override
  {
    if (riid == __uuidof(Swarm))
    {
      return this;
    }

    return Super::QueryCastWeakRef(riid);
  }

  static TCntPtr<Swarm> FromWeakRef(_In_opt_ const ObjectWeakRef* weakRef) noexcept
  {
    TCntPtr<Swarm> swarm;
    if (weakRef)
    {
      swarm = static_cast<Swarm*>(const_cast<ObjectWeakRef*>(weakRef)->QueryCastWeakRef(__uuidof(Swarm)));
    }

    return swarm;
  }

  template <typename T>
  static TCntPtr<Swarm> FromObject(_In_opt_ T* obj) noexcept
  {
    return FromObjectInternal(obj, OverloadTag());
  }

  template <typename T>
  bool Contains(_In_opt_ T* obj) const noexcept
  {
    TCntPtr<Swarm> swarm = Swarm::FromObjectInternal(obj, OverloadTag());
    return this == swarm.Get();
  }

protected:
  using SwarmType = Swarm;

  virtual void DestroyObject() noexcept override
  {
    for (SwarmMemberHolder* member = m_headMember.get(); member != nullptr; member = member->m_nextMember.get())
    {
      member->DestroyObject();
    }

    Super::DestroyObject();
  }

private:
  template <typename T>
  static auto FromObjectInternal(_In_opt_ T* obj, OverloadTagP1) noexcept
      -> decltype(obj->GetWeakRef(), TCntPtr<Swarm>())
  {
    TCntPtr<Swarm> swarm;
    if (obj)
    {
      swarm = Swarm::FromWeakRef(&obj->GetWeakRef());
    }

    return swarm;
  }

  // This method assumes that obj implements IUnknown.
  template <typename T>
  static TCntPtr<Swarm> FromObjectInternal(_In_opt_ T* obj, OverloadTagP2) noexcept
  {
    TCntPtr<Swarm> swarm;
    ObjectWeakRef* weakRef = query_cast<ObjectWeakRef*>(obj);
    if (weakRef)
    {
      swarm = FromWeakRef(weakRef);
    }

    return swarm;
  }

  void AddSwarmMember(SwarmMemberHolder* holder) noexcept
  {
    // Atomically set the new member as a head member in the single linked list
    for (;;)
    {
      SwarmMemberHolder* headMember = m_headMember.get();
      UniquePtrType& nextMember = holder->m_nextMember;
      nextMember.release(); // To make sure that reset will not delete value when we do second iteration.
      nextMember.reset(headMember);
      if (std::atomic_compare_exchange_weak(
              (std::atomic<SwarmMemberHolder*>*)(void*)&m_headMember, &headMember, holder))
      {
        // Success: We were able to put pointer to our new member to the m_headMember without any conflicts.
        // The object is now owned by the m_headMember. Make sure that memberContainer does not delete it.
        break;
      }
    }
  }

private:
  UniquePtrType m_headMember;
};

// Define SwarmMemberMemoryGuard destructor here to avoid using Swarm class before it is defined.
template <typename T, typename TContainer>
inline SwarmMemberMemoryGuard<T, TContainer>::~SwarmMemberMemoryGuard() noexcept
{
  if (ObjMemory)
  {
    // Object construction failed. Make sure that we release the weak reference added before construction.
    Container->ReleaseWeakRef();
  }
  else if (Obj)
  {
    // Initialize method failed.
    T::RefCountPolicy::Deleter::template Delete(static_cast<typename T::TypeToDelete*>(Obj));
  }

  if (MemberContainer)
  {
    MemberContainer->~TContainer();
    T::RefCountPolicy::Allocator::Deallocate(MemberContainer);
  }
}

/**
  SwarmMemberPtr can be used by a swarm member to point to another swarm member.
  The pointer represents a strong reference. It does not contribute to the reference count if the pointed object
  is from the same swarm. This is done to avoid extra add ref count which may cause a memory leak.
  SwarmMemberPtr assumes that swarm members implement IUnknown.
*/
template <typename T, bool KnownSameSwarm = false>
class SwarmMemberPtr
{
public:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(SwarmMemberPtr)

  SwarmMemberPtr() noexcept {}

#if __clang__
// save the clang warning state and silence warnings about [-Wtautological-undefined-compare].
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif

  template <typename TOther, typename = std::enable_if_t<std::is_base_of<T, TOther>::value>>
  SwarmMemberPtr(
      _In_opt_ TOther* ptr,
      _In_ const Swarm& ownerSwarm,
      _In_opt_ const Swarm* otherSwarm = nullptr) noexcept
      : m_ptr(ptr), m_isDifferentSwarm(&ownerSwarm != (otherSwarm ? otherSwarm : Swarm::FromObject(ptr).Get()))
  {
    if (m_ptr && m_isDifferentSwarm)
    {
      m_ptr->AddRef();
    }
  }

  template <typename TOther, typename = std::enable_if_t<std::is_base_of<T, TOther>::value>>
  SwarmMemberPtr(
      _In_opt_ TOther* ptr,
      _In_ const Mso::ObjectWeakRef& ownerWeak,
      _In_opt_ const Mso::ObjectWeakRef* otherWeak = nullptr) noexcept
      : m_ptr(ptr), m_isDifferentSwarm(&ownerWeak != (otherWeak ? otherWeak : Details::WeakPtrBase::GetWeakRef(ptr)))
  {
    if (m_ptr && m_isDifferentSwarm)
    {
      m_ptr->AddRef();
    }
  }

// resume clang warning state
#if __clang__
#pragma clang diagnostic pop
#endif
  SwarmMemberPtr(SwarmMemberPtr&& from) noexcept : m_ptr(from.m_ptr), m_isDifferentSwarm(from.m_isDifferentSwarm)
  {
    from.m_ptr = nullptr;
    from.m_isDifferentSwarm = false;
  }

  ~SwarmMemberPtr() noexcept
  {
    Reset();
  }

  SwarmMemberPtr& operator=(SwarmMemberPtr&& from) noexcept
  {
    if (this != &from)
    {
      SwarmMemberPtr<T>(std::move(from)).Swap(*this);
    }

    return *this;
  }

  void Swap(SwarmMemberPtr& other) noexcept
  {
    using std::swap;
    OACR_ASSUME_NOTHROW_BEGIN
    swap(m_ptr, other.m_ptr);
    swap(m_isDifferentSwarm, other.m_isDifferentSwarm);
    OACR_ASSUME_NOTHROW_END
  }

  void Reset() noexcept
  {
    if (m_ptr && m_isDifferentSwarm)
    {
      m_ptr->Release();
    }

    m_ptr = nullptr;
  }

  T* Get() const noexcept
  {
    return m_ptr;
  }

  T** GetRaw() const noexcept
  {
    return &m_ptr;
  }

  bool IsEmpty() const noexcept
  {
    return m_ptr == nullptr;
  }

  bool IsDifferentSwarm() const noexcept
  {
    return m_isDifferentSwarm;
  }

  T* operator->() const noexcept
  {
    AssertTag(!IsEmpty(), 0x01003713 /* tag_bad2t */);
    return Get();
  }

  T& operator*() const noexcept
  {
    AssertTag(!IsEmpty(), 0x01003714 /* tag_bad2u */);
    return *m_ptr;
  }

private:
  T* m_ptr = nullptr;
  bool m_isDifferentSwarm = false;
};

/**
  SwarmMemberPtr specialization for a case when we know for sure that we have a pointer inside of the same Swarm.
  In that case we can have just a single pointer field and have less code.
  It is preferable to use this smart pointer instead of raw pointer because we preserve semantic of the pointer.
  I.e. this is not just a random raw pointer, but a raw pointer which is made according to the specific design decision.
*/
template <typename T>
class SwarmMemberPtr<T, /*KnownSameSwarm*/ true>
{
public:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(SwarmMemberPtr)

  SwarmMemberPtr() noexcept {}

  template <typename TOther, typename = typename std::enable_if<std::is_base_of<T, TOther>::value>::type>
  SwarmMemberPtr(_In_opt_ TOther* ptr) noexcept : m_ptr(ptr)
  {
  }

  SwarmMemberPtr(SwarmMemberPtr&& from) noexcept : m_ptr(from.m_ptr)
  {
    from.m_ptr = nullptr;
  }

  SwarmMemberPtr& operator=(SwarmMemberPtr&& from) noexcept
  {
    if (this != &from)
    {
      SwarmMemberPtr<T, /*KnownSameSwarm*/ true>(std::move(from)).Swap(*this);
    }

    return *this;
  }

  void Swap(SwarmMemberPtr& other) noexcept
  {
    using std::swap;
    OACR_ASSUME_NOTHROW_BEGIN
    swap(m_ptr, other.m_ptr);
    OACR_ASSUME_NOTHROW_END
  }

  void Reset() noexcept
  {
    m_ptr = nullptr;
  }

  T* Get() const noexcept
  {
    return m_ptr;
  }

  T** GetRaw() noexcept
  {
    return &m_ptr;
  }

  bool IsEmpty() const noexcept
  {
    return m_ptr == nullptr;
  }

  bool IsDifferentSwarm() const noexcept
  {
    return false;
  }

  T* operator->() const noexcept
  {
    AssertTag(!IsEmpty(), 0x01003715 /* tag_bad2v */);
    return m_ptr;
  }

  T& operator*() const noexcept
  {
    AssertTag(!IsEmpty(), 0x01003716 /* tag_bad2w */);
    return *m_ptr;
  }

private:
  T* m_ptr = nullptr;
};

template <typename T, bool KnownSameSwarm>
void swap(SwarmMemberPtr<T, KnownSameSwarm>& left, SwarmMemberPtr<T, KnownSameSwarm>& right) noexcept
{
  left.Swap(right);
}

template <typename T, bool KnownSameSwarm>
bool operator==(const SwarmMemberPtr<T, KnownSameSwarm>& left, std::nullptr_t) noexcept
{
  return left.IsEmpty();
}

template <typename T, bool KnownSameSwarm>
bool operator==(std::nullptr_t, const SwarmMemberPtr<T, KnownSameSwarm>& right) noexcept
{
  return right.IsEmpty();
}

template <typename T, bool KnownSameSwarm>
bool operator!=(const SwarmMemberPtr<T, KnownSameSwarm>& left, std::nullptr_t) noexcept
{
  return !left.IsEmpty();
}

template <typename T, bool KnownSameSwarm>
bool operator!=(std::nullptr_t, const SwarmMemberPtr<T, KnownSameSwarm>& right) noexcept
{
  return !right.IsEmpty();
}

} // namespace Mso

#pragma pop_macro("new")

END_DISABLE_WARNING_ASSIGNMENT_OPERATOR_IMPLICITLY_DELETED()
END_DISABLE_WARNING_COPY_CTOR_IMPLICITLY_DELETED()

#pragma pack(pop)
