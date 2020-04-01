// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#ifndef _CPPEXTENSIONS_THREADLOCAL_PTHREAD_H_
#define _CPPEXTENSIONS_THREADLOCAL_PTHREAD_H_
#include <compilerAdapters/cppMacros.h>
#include <memory>
#include <pthread.h>
#include <utility>
#include <cstdlib>

namespace Mso { namespace Details {

struct ThreadLocalImpl
{
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(ThreadLocalImpl);

  ThreadLocalImpl() noexcept
  {
    if (pthread_key_create(&m_pthread_key, nullptr) != 0)
      std::abort();
  }

  ThreadLocalImpl(void (*pfnCleanup)(void*) noexcept) noexcept
  {
    if (pthread_key_create(&m_pthread_key, pfnCleanup) != 0)
      std::abort();
  }

  ~ThreadLocalImpl() noexcept
  {
    if (m_pthread_key)
      pthread_key_delete(m_pthread_key);
  }

  void* Get() const noexcept
  {
    return pthread_getspecific(m_pthread_key);
  }

  void Set(const void* t) noexcept
  {
    if (pthread_setspecific(m_pthread_key, t) != 0)
      std::abort();
  }

private:
  pthread_key_t m_pthread_key{};
};

}} // namespace Mso::Details

#endif // _CPPEXTENSIONS_THREADLOCAL_PTHREAD_H_
