// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include "queueService.h"

#include <array>
#include <condition_variable>
#include <mutex>

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
  std::condition_variable m_wakeUpThread;
  std::mutex m_threadMutex;
  const uint32_t m_maxThreads{1};
  uint32_t m_busyThreads{0};
  uint32_t m_threadCount{0};
  bool m_isShutdown{false};
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
    if (auto queue = m_queue.GetStrongPtr())
    {
      DispatchTask task;
      while (queue->TryDequeTask(/*ref*/ task))
      {
        queue->InvokeTask(std::move(task), std::nullopt);
      }

      std::unique_lock lock{m_threadMutex};
      --m_busyThreads;
      if (m_isShutdown)
      {
        break;
      }
      else if (queue->HasTasks())
      {
        ++m_busyThreads;
        continue;
      }

      queue = nullptr; // release the queue

      m_wakeUpThread.wait(lock);
      ++m_busyThreads;
    }
    else
    {
      break;
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
  std::unique_lock lock{m_threadMutex};

  // See if we need to increase number of threads
  if (m_busyThreads == m_threadCount && m_threadCount < m_maxThreads)
  {
    ++m_busyThreads;
    m_threads[m_threadCount++] = std::thread(&ThreadPoolSchedulerLinux::RunInThread, this);
  }
  else
  {
    m_wakeUpThread.notify_one();
  }
}

void ThreadPoolSchedulerLinux::Shutdown() noexcept
{
  std::unique_lock lock{m_threadMutex};
  m_isShutdown = true;
  m_wakeUpThread.notify_all();
}

void ThreadPoolSchedulerLinux::AwaitTermination() noexcept
{
  Shutdown();
  for (auto& thread : m_threads)
  {
    if (thread.joinable())
    {
      thread.join();
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
