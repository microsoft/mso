// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
Implementation of a Swarm - a reference counting scheme where multiple objects
share the same reference counter.
****************************************************************************/

#pragma once

#include <object/objectWithWeakRef.h>
#include <object/queryCast.h>
#include <object/weakPtr.h>
#include <core/memoryApi.h>

//
// Swarm is a collection of a ref counted objects that share the same reference counting.
// When the ref counter goes to zero all objects are destroyed.
// Once object becomes part of a swarm it cannot be removed.
// Swarm may have objects of the same or different types.
//
// Any class that implements Mso::RefCountStrategy::WeakRef can be part of a swarm.
// It is possible because ObjectWeakRef leaves outside of object and can be shared across multiple objects.
//
// Mso::FixedSwarmBase does only one memory allocation for all objects in the swarm.
// It is very compact: on 64 bit platform the size is 16 bytes for ObjectWeakRef
// (8 bytes v-table + 4 bytes ref count + 4 bytes weak ref count) and one pointer for each swarm member to point to the
// ObjectWeakRef.
//
// To create an Mso::FixedSwarmBase use Mso::Make method..
// To create a swarm member call MakeMember() instance methods.
//

#pragma pack(push, _CRT_PACKING)

#pragma push_macro("new")
#undef new

namespace Mso {

namespace Details {

/**
  Storage for FixedSwarmBase class. In some sense this is a specialized std::tuple.
*/
template <typename... Ts>
struct FixedSwarmStorage;

template <>
struct FixedSwarmStorage<>
{
  void DestroyObject() const noexcept
  {
    // Do nothing because there is no storage in this class
    UNREFERENCED_OACR(this);
  }
};

template <typename T0, typename... Ts>
struct FixedSwarmStorage<T0, Ts...> : public FixedSwarmStorage<Ts...>
{
  using Super = FixedSwarmStorage<Ts...>;

  FixedSwarmStorage() noexcept
  {
    Mso::Details::SetWeakRef(&m_memberStorage, nullptr);
  }

  void DestroyObject() const noexcept
  {
    if (Mso::Details::GetWeakRef(&m_memberStorage))
    {
      T0::RefCountPolicy::Deleter::template Delete(static_cast<typename T0::TypeToDelete*>((void*)&m_memberStorage));
    }

    Super::DestroyObject();
  }

  ObjectWeakRef* m_weakRefPlaceholder; // reserves negative space before m_memberStorage.
  typename std::aligned_storage<sizeof(T0), std::alignment_of<T0>::value>::type m_memberStorage;
};

/**
  A helper type to access FixedSwarmStorage member by index similar to std::tuple.
*/
template <size_t Index, typename Storage>
struct FixedSwarmStorageMember;

template <typename T, typename... Ts>
struct FixedSwarmStorageMember<0, FixedSwarmStorage<T, Ts...>>
{
  using Type = T;
  using StorageType = FixedSwarmStorage<T, Ts...>;
};

template <size_t Index, typename T, typename... Ts>
struct FixedSwarmStorageMember<Index, FixedSwarmStorage<T, Ts...>>
    : public FixedSwarmStorageMember<Index - 1, FixedSwarmStorage<Ts...>>
{
};

} // namespace Details

template <typename T, typename TContainer>
struct FixedSwarmMemberMemoryGuard
{
  using Type = T;

  ~FixedSwarmMemberMemoryGuard() noexcept
  {
    if (ObjMemory)
    {
      Container->ReleaseWeakRef();
      Mso::Details::SetWeakRef(ObjMemory, nullptr);
    }
    else if (Obj)
    {
      // Initialize method failed.
      T::RefCountPolicy::Deleter::template Delete(static_cast<typename T::TypeToDelete*>(Obj));
      Mso::Details::SetWeakRef(Obj, nullptr);
    }
  }

public: // We use a public fields to reduce number of generated methods.
  // VC++ bug: Make sure that the order of the fields is the same for all memory guards. Otherwise, VC++ generates
  // incorrect code for ship builds.
  void* ObjMemory;
  T* Obj;
  TContainer* Container;
};

/**
  FixedSwarmBase is a fixed size swarm which knows all its member type at the creation.
  It is like an std::tuple.
  By knowing all types upfront we can make the most compact representation of the swarm, and be very flexible about
  swarm member types because we know their type in swarm.
  Members can be IUnknown, IRefCounted, or object without v-table with AddRef() and Release() methods.
  The only requirement is that the members must support the weak ref count where pointer to ObjectWeakRef is in
  the nearest negative offset from the object.
  When FixedSwarmBase ref count goes to zero it destroys all objects in the Swarm.
*/
template <typename TBase, typename... Ts>
class FixedSwarmBase : public TBase
{
  static_assert(std::is_base_of<ObjectWeakRef, TBase>::value, "TBase must be inherited from ObjectWeakRef.");

  using Super = TBase;
  using StorageType = Details::FixedSwarmStorage<Ts...>;
  template <size_t Index>
  using MemberType = typename Details::FixedSwarmStorageMember<Index, StorageType>::Type;
  template <size_t Index>
  using MemberStorageType = typename Details::FixedSwarmStorageMember<Index, StorageType>::StorageType;

public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;

  FixedSwarmBase() noexcept : Super(), m_storage() {}

  DECLARE_COPYCONSTR_AND_ASSIGNMENT(FixedSwarmBase)

  template <size_t Index>
  MemberType<Index>* Get() const noexcept
  {
    void* item = &static_cast<MemberStorageType<Index>*>(const_cast<StorageType*>(&m_storage))->m_memberStorage;
    if (Mso::Details::GetWeakRef(item))
    {
      return static_cast<MemberType<Index>*>(item);
    }

    return nullptr;
  }

  template <size_t Index, typename TResult = MemberType<Index>>
  Mso::WeakPtr<TResult> GetWeakPtr() const noexcept
  {
    Mso::WeakPtr<TResult> result;

    void* member = &static_cast<MemberStorageType<Index>*>(const_cast<StorageType*>(&m_storage))->m_memberStorage;
    if (Mso::Details::GetWeakRef(member))
    {
      result = Mso::WeakPtr<TResult>(
          reinterpret_cast<MemberType<Index>*>(member),
          const_cast<ObjectWeakRef*>(static_cast<const ObjectWeakRef*>(this)));
    }

    return result;
  }

  template <size_t Index, typename... TArgs>
  void MakeMember(TArgs&&... args) noexcept(MemberType<Index>::MakePolicy::IsNoExcept)
  {
    using TMember = MemberType<Index>;

    void* memberMemory = &static_cast<MemberStorageType<Index>*>(const_cast<StorageType*>(&m_storage))->m_memberStorage;

    // Initialize weak reference for the member
    Mso::Details::SetWeakRef(memberMemory, this);
    Super::AddWeakRef(); // To match ReleaseWeakRef() call in member destructor.

    FixedSwarmMemberMemoryGuard<TMember, FixedSwarmBase> memoryGuard = {memberMemory, nullptr, this};
    TMember::MakePolicy::template Make<TMember>(memoryGuard, std::forward<TArgs>(args)...);

    Debug(TMember::RefCountPolicy::ValidateObject(memoryGuard));
    memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  }

protected:
  using FixedSwarmType = FixedSwarmBase;

  virtual void DestroyObject() noexcept override
  {
    m_storage.DestroyObject();
    Super::ReleaseWeakRef();
  }

  virtual void DestroyContainer() const noexcept override
  {
    this->~FixedSwarmBase();
    Super::RefCountPolicy::Allocator::Deallocate(const_cast<FixedSwarmBase*>(this));
  }

private:
  StorageType m_storage;
};

template <typename... Ts>
using FixedSwarm = FixedSwarmBase<ObjectWeakRef, Ts...>;

} // namespace Mso

#pragma pop_macro("new")
#pragma pack(pop)
