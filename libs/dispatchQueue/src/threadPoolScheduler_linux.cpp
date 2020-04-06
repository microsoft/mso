// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include "queueService.h"

#include <array>

namespace Mso {

struct ThreadPoolSchedulerLinux : Mso::UnknownObject<IDispatchQueueScheduler>
{
  ThreadPoolSchedulerLinux(uint32_t maxThreads) noexcept;
  ~ThreadPoolSchedulerLinux() noexcept override;

  void RunInThread() noexcept;

public: // IDispatchQueueScheduler
  void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService>&& queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

private:
  struct ThreadAccessGuard
  {
    ThreadAccessGuard(ThreadPoolSchedulerLinux* scheduler) noexcept;
    ~ThreadAccessGuard() noexcept;

    static bool HasThreadAccess(ThreadPoolSchedulerLinux* scheduler) noexcept;

  private:
    ThreadPoolSchedulerLinux* m_prevScheduler{nullptr};
    static thread_local ThreadPoolSchedulerLinux* tls_scheduler;
  };

private:
  constexpr static uint32_t MaxConcurrentThreads{64};

  Mso::WeakPtr<IDispatchQueueService> m_queue;
  ManualResetEvent m_wakeUpEvent;
  const uint32_t m_maxThreads{1};
  std::atomic<uint32_t> m_busyThreads{0};
  std::atomic<uint32_t> m_threadCount{0};
  std::atomic_bool m_isShutdown{false};
  std::array<std::thread, (size_t)MaxConcurrentThreads> m_threads;
};

//=============================================================================
// ThreadPoolSchedulerLinux implementation
//=============================================================================

ThreadPoolSchedulerLinux::ThreadPoolSchedulerLinux(uint32_t maxThreads) noexcept
    : m_maxThreads{maxThreads == 0 ? MaxConcurrentThreads : maxThreads}
{
}

ThreadPoolSchedulerLinux::~ThreadPoolSchedulerLinux() noexcept
{
  AwaitTermination();
}

void ThreadPoolSchedulerLinux::RunInThread() noexcept
{
  ThreadAccessGuard guard{this};
  for (;;)
  {
    ++m_busyThreads;

    if (auto queue = m_queue.GetStrongPtr())
    {
      DispatchTask task;
      while (queue->TryDequeTask(task))
      {
        queue->InvokeTask(std::move(task), std::nullopt);
      }
    }

    --m_busyThreads;

    if (m_isShutdown)
    {
      break;
    }

    m_wakeUpEvent.Wait();

    if (!m_isShutdown)
    {
      m_wakeUpEvent.Reset();
    }
  }
}

void ThreadPoolSchedulerLinux::IntializeScheduler(Mso::WeakPtr<IDispatchQueueService>&& queue) noexcept
{
  m_queue = std::move(queue);
}

bool ThreadPoolSchedulerLinux::HasThreadAccess() noexcept
{
  return ThreadAccessGuard::HasThreadAccess(this);
}

bool ThreadPoolSchedulerLinux::IsSerial() noexcept
{
  return m_maxThreads == 1;
}

void ThreadPoolSchedulerLinux::Post() noexcept
{
  // Increase number of threads if m_busyThreads == m_threadCount
  uint32_t threadCount = m_threadCount.load(std::memory_order_relaxed);
  for (;;)
  {
    if (threadCount > m_busyThreads.load(std::memory_order_relaxed))
    {
      // Just wake up a sleeping thread
      m_wakeUpEvent.Set();
      break;
    }

    if (m_isShutdown)
    {
      // Do not add new threads when we shutdown.
      break;
    }

    if (threadCount >= m_maxThreads)
    {
      break;
    }

    uint32_t index = threadCount;
    if (m_threadCount.compare_exchange_weak(
            threadCount, threadCount + 1, std::memory_order_release, std::memory_order_relaxed))
    {
      m_threads[index] = std::thread(&ThreadPoolSchedulerLinux::RunInThread, this);
    }
  }
}

void ThreadPoolSchedulerLinux::Shutdown() noexcept
{
  m_isShutdown = true;
  m_wakeUpEvent.Set();
}

void ThreadPoolSchedulerLinux::AwaitTermination() noexcept
{
  Shutdown();
  for (auto& thread : m_threads)
  {
    if (thread.joinable())
    {
      if (thread.get_id() != std::this_thread::get_id())
      {
        thread.join();
      }
      else
      {
        // We are on the same thread. We cannot join because it would cause a deadlock and crash.
        // We also cannot allow std::thread destructor to run because it would crash on non-joined thread.
        // So, we just detach and let the underlying system thread finish on its own.
        thread.detach();
      }
    }
  }
}

//=============================================================================
// ThreadPoolSchedulerLinux::ThreadAccessGuard implementation
//=============================================================================

/*static*/ thread_local ThreadPoolSchedulerLinux* ThreadPoolSchedulerLinux::ThreadAccessGuard::tls_scheduler{nullptr};

ThreadPoolSchedulerLinux::ThreadAccessGuard::ThreadAccessGuard(ThreadPoolSchedulerLinux* scheduler) noexcept
    : m_prevScheduler{tls_scheduler}
{
  tls_scheduler = scheduler;
}

ThreadPoolSchedulerLinux::ThreadAccessGuard::~ThreadAccessGuard() noexcept
{
  tls_scheduler = m_prevScheduler;
}

/*static*/ bool ThreadPoolSchedulerLinux::ThreadAccessGuard::HasThreadAccess(
    ThreadPoolSchedulerLinux* scheduler) noexcept
{
  return tls_scheduler == scheduler;
}

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeThreadPoolScheduler(
    uint32_t maxThreads) noexcept
{
  return Mso::Make<ThreadPoolSchedulerLinux, IDispatchQueueScheduler>(maxThreads);
}

} // namespace Mso
