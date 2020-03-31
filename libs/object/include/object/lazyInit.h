// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <atomic>
#include <type_traits>

#include <object/IUnknownShim.h>
#include <object/make.h>

namespace Mso { namespace Object {

// A class that lazily initializes its data member in a thread-safe fashion. If stored in a global object, this class
// can be used to provide Singleton functionality. However, before using this class in this way, please research
// whether a Singleton is required for your scenarios as there are often preferable alternatives.
//
// The data member type must derive from either Mso::IRefCounted or IUnknown.
//
// The object in the class should be constructed by calling Create(), specifying any constructor parameters as
// required. This method will return a pointer to the underlying object. Note that multiple simultaneous calls to
// Create() may result in multiple objects being created. However, all objects except for one will be discarded. This
// means that your constructor should not have any permanent side effects resulting from calling Create() multiple
// times.
//
// The pointer can also be retrieved by calling Get(). This will return nullptr if the underlying object has not yet
// been created.
//
// The underlying object can be released by calling Release(). Subsequent calls to Get() will return the nullptr, but
// a new instance of the object can be created by again calling Create(). Release() will be called automatically by
// the destructor if it is not manually called beforehand.
//
// Note that the pointer returned by these methods should not be deleted or copied into some mechanism for managing
// memory. All necessary memory management takes place within the LazyInit class.

template <class TObject, class TInterface = TObject>
class LazyInit final
{
  static_assert(
      std::is_base_of<Mso::IRefCounted, TInterface>::value || std::is_base_of<IUnknown, TInterface>::value,
      "The type must be derived from Mso::IRefCounted or IUnknown.");

public:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(LazyInit);

  explicit LazyInit() noexcept = default;

  ~LazyInit() noexcept
  {
    Release();
  }

  template <class... TArgs>
  TInterface& Get(TArgs&&... args) noexcept
  {
    bool unused{false};
    return Get(unused, std::forward<TArgs>(args)...);
  }

  template <class... TArgs>
  TInterface& Get(bool& created, TArgs&&... args) noexcept
  {
    created = false;
    auto instance{m_instance.load(std::memory_order_acquire)};
    if (!instance)
    {
      auto newInstance{Mso::Make<TObject, TInterface>(std::forward<TArgs>(args)...)};
      if (m_instance.compare_exchange_strong(instance, newInstance.Get()))
      {
        // The current thread won the race.
        // Ensure that a pointer with a reference count of 1 is stored.
        instance = newInstance.Detach();
        created = true;
      }
    }

    return *instance;
  }

  bool IsInitialized() const noexcept
  {
    return m_instance.load(std::memory_order_acquire) != nullptr;
  }

  void Release() noexcept
  {
    auto instance{m_instance.exchange(nullptr)};
    if (instance)
    {
      // Release the reference count added in Create().
      instance->Release();
    }
  }

private:
  std::atomic<TInterface*> m_instance{nullptr};
};

}} // namespace Mso::Object