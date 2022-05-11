// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// #include "precomp.h"
#include "eventWaitHandleImpl.h"
#include <pthread.h>

namespace Mso {

namespace {

// Mutex class based on pthread_mutex_t that meets BasicLockable standard requirements.
struct PThreadMutex
{
  ~PThreadMutex() noexcept
  {
    pthread_mutex_destroy(&Handle);
  }

  void lock() noexcept
  {
    pthread_mutex_lock(&Handle);
  }

  void unlock() noexcept
  {
    pthread_mutex_unlock(&Handle);
  }

  pthread_mutex_t Handle = PTHREAD_MUTEX_INITIALIZER;
};

// pthread_cond_t wrapper
struct ConditionVariable
{
  ~ConditionVariable() noexcept
  {
    pthread_cond_destroy(&m_cond);
  }

  void NotifyOne() noexcept
  {
    pthread_cond_signal(&m_cond);
  }

  void NotifyAll() noexcept
  {
    pthread_cond_broadcast(&m_cond);
  }

  bool WaitUntil(PThreadMutex& mutex, const WaitTimePoint& waitTimePoint) noexcept
  {
    if (waitTimePoint.IsInfinite)
    {
      VerifyElseCrashSzTag(
          !pthread_cond_wait(&m_cond, &mutex.Handle), "pthread_cond_wait failed", 0x026e348e /* tag_c19so */);
      return true;
    }

    auto tsWaitUntil = DurationToTimeSpec(waitTimePoint.WaitUntil.time_since_epoch());
    if (int error = pthread_cond_timedwait(&m_cond, &mutex.Handle, &tsWaitUntil))
    {
      VerifyElseCrashSzTag(error == ETIMEDOUT, "pthread_cond_wait failed", 0x026e348f /* tag_c19sp */);
      return false;
    }

    return true;
  }

private:
  static timespec DurationToTimeSpec(const TimePoint::duration& duration) noexcept
  {
    using namespace std::chrono;
    timespec ts;
    ts.tv_sec = static_cast<time_t>(duration_cast<seconds>(duration).count());
    ts.tv_nsec = static_cast<long>(duration_cast<nanoseconds>(duration % seconds{1}).count());
    return ts;
  }

private:
  pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
};

} // namespace

LIBLET_PUBLICAPI ManualResetEvent::ManualResetEvent(EventWaitHandleState state) noexcept
    : m_handle{Mso::Make<EventWaitHandle<PThreadMutex, ConditionVariable>>(/*isAutoReset:*/ false, state)}
{
}

LIBLET_PUBLICAPI AutoResetEvent::AutoResetEvent(EventWaitHandleState state) noexcept
    : m_handle{Mso::Make<EventWaitHandle<PThreadMutex, ConditionVariable>>(/*isAutoReset:*/ true, state)}
{
}

} // namespace Mso
