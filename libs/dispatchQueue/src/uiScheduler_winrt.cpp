// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#if !__has_include(<base.h>)

#include "uiSchedulerStub.h"

#else

#include "eventWaitHandle/eventWaitHandle.h"
#include "object/refCountedObject.h"
#include "queueService.h"
#include "taskQueue.h"
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.Core.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;

namespace Mso {

struct UISchedulerWinRT;

//! TaskDispatchedHandler is a DispatchedHandler delegate that we pass to CoreDispatcher.
//! We use custom ref counting to avoid extra memory allocations and to handle reference to DispatchTask.
struct TaskDispatchedHandler final : impl::abi_t<DispatchedHandler>
{
  TaskDispatchedHandler(UISchedulerWinRT* scheduler) noexcept;
  int32_t __stdcall QueryInterface(guid const& id, void** result) noexcept final;
  uint32_t __stdcall AddRef() noexcept final;
  uint32_t __stdcall Release() noexcept final;
  int32_t __stdcall Invoke() noexcept final;

private:
  UISchedulerWinRT* m_scheduler;
};

struct UISchedulerWinRT : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler>
{
  UISchedulerWinRT(CoreDispatcher&& coreDispatcher) noexcept;
  ~UISchedulerWinRT() noexcept override;

  uint32_t AddHandlerRef() noexcept;
  uint32_t ReleaseHandlerRef() noexcept;

  DispatchedHandler MakeDispatchedHandler() noexcept;
  bool TryTakeTask(Mso::CntPtr<IDispatchQueueService>& queue, DispatchTask& task) noexcept;

public: // IDispatchQueueScheduler
  void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService>&& queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

private:
  struct CleanupContext
  {
    CleanupContext(UISchedulerWinRT* scheduler) noexcept;
    ~CleanupContext() noexcept;
    void CheckShutdown() noexcept;
    void CheckTermination() noexcept;

  private:
    UISchedulerWinRT* m_scheduler;
    bool m_isShutdown{false};
    bool m_isTerminated{false};
  };

private:
  CoreDispatcher m_coreDispatcher{nullptr};
  TaskDispatchedHandler m_dispatchedHandler{this};
  ManualResetEvent m_terminationEvent;
  ThreadMutex m_mutex;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  Mso::CntPtr<UISchedulerWinRT> m_self;
  uint32_t m_handlerRefCount{0};
  uint32_t m_taskCount{0};
  bool m_isShutdown{false};
};

//=============================================================================
// TaskDispatchedHandler implementation
//=============================================================================

TaskDispatchedHandler::TaskDispatchedHandler(UISchedulerWinRT* scheduler) noexcept : m_scheduler{scheduler} {}

int32_t __stdcall TaskDispatchedHandler::QueryInterface(guid const& id, void** result) noexcept
{
  if (is_guid_of<DispatchedHandler>(id) || is_guid_of<Windows::Foundation::IUnknown>(id)
      || is_guid_of<IAgileObject>(id))
  {
    *result = static_cast<impl::abi_t<DispatchedHandler>*>(this);
    AddRef();
    return impl::error_ok;
  }

  if (is_guid_of<IMarshal>(id))
  {
    return make_marshaler(this, result);
  }

  *result = nullptr;
  return impl::error_no_interface;
}

uint32_t __stdcall TaskDispatchedHandler::AddRef() noexcept
{
  return m_scheduler->AddHandlerRef();
}

uint32_t __stdcall TaskDispatchedHandler::Release() noexcept
{
  return m_scheduler->ReleaseHandlerRef();
}

int32_t __stdcall TaskDispatchedHandler::Invoke() noexcept
{
  Mso::CntPtr<IDispatchQueueService> queue;
  DispatchTask task;
  if (m_scheduler->TryTakeTask(queue, task))
  {
    queue->InvokeTask(std::move(task), std::chrono::steady_clock::now() + std::chrono::milliseconds(1000 / 60));
  }

  return impl::error_ok;
}

//=============================================================================
// UISchedulerWinRT implementation
//=============================================================================

UISchedulerWinRT::UISchedulerWinRT(CoreDispatcher&& coreDispatcher) noexcept : m_coreDispatcher{std::move(coreDispatcher)}
{
}

UISchedulerWinRT::~UISchedulerWinRT() noexcept
{
  AwaitTermination();
}

uint32_t UISchedulerWinRT::AddHandlerRef() noexcept
{
  std::lock_guard lock{m_mutex};
  return ++m_handlerRefCount;
}

uint32_t UISchedulerWinRT::ReleaseHandlerRef() noexcept
{
  Mso::CntPtr<UISchedulerWinRT> self;
  CleanupContext context{this};

  {
    std::lock_guard lock{m_mutex};
    VerifyElseCrashSz(m_handlerRefCount, "Ref count cannot be negative");

    uint32_t refCount = --m_handlerRefCount;
    if (refCount == 0)
    {
      self = std::move(m_self); // Release pointer to self outside of lock.
      context.CheckShutdown();
      context.CheckTermination();
    }

    return refCount;
  }
}

bool UISchedulerWinRT::TryTakeTask(Mso::CntPtr<IDispatchQueueService>& queue, DispatchTask& task) noexcept
{
  {
    std::lock_guard lock{m_mutex};
    VerifyElseCrashSz(m_taskCount, "Task count cannot be negative");
    --m_taskCount;
  }

  if (queue = m_queue.GetStrongPtr())
  {
    return queue->TryDequeTask(task);
  }

  return false;
}

DispatchedHandler UISchedulerWinRT::MakeDispatchedHandler() noexcept
{
  VerifyElseCrash(m_mutex.IsLockedByMe());

  if (m_handlerRefCount == 0)
  {
    m_self = this; // Keep reference to self while CoreDispatcher owns DispatchedHandler.
  }

  ++m_handlerRefCount;
  return {static_cast<void*>(&m_dispatchedHandler), take_ownership_from_abi};
}

void UISchedulerWinRT::IntializeScheduler(Mso::WeakPtr<IDispatchQueueService>&& queue) noexcept
{
  m_queue = std::move(queue);
}

bool UISchedulerWinRT::HasThreadAccess() noexcept
{
  return m_coreDispatcher.HasThreadAccess();
}

bool UISchedulerWinRT::IsSerial() noexcept
{
  return true;
}

void UISchedulerWinRT::Post() noexcept
{
  DispatchedHandler handler;
  {
    std::lock_guard lock{m_mutex};
    if (!m_isShutdown)
    {
      ++m_taskCount;
      handler = MakeDispatchedHandler();
    }
  }

  if (handler)
  {
    m_coreDispatcher.RunAsync(CoreDispatcherPriority::Normal, std::move(handler));
  }
}

void UISchedulerWinRT::Shutdown() noexcept
{
  CleanupContext context{this};
  {
    std::lock_guard lock{m_mutex};
    m_isShutdown = true;
    context.CheckTermination();
  }
}

void UISchedulerWinRT::AwaitTermination() noexcept
{
  Shutdown();
  m_terminationEvent.Wait();
}

//=============================================================================
// UISchedulerWinRT::CleanupContext implementation
//=============================================================================

UISchedulerWinRT::CleanupContext::CleanupContext(UISchedulerWinRT* scheduler) noexcept : m_scheduler{scheduler} {}

UISchedulerWinRT::CleanupContext::~CleanupContext() noexcept
{
  if (m_isTerminated)
  {
    m_scheduler->m_terminationEvent.Set();
  }

  if (m_isShutdown)
  {
    if (auto queue = m_scheduler->m_queue.GetStrongPtr())
    {
      queue->Shutdown(PendingTaskAction::Cancel);
    }
  }
}

void UISchedulerWinRT::CleanupContext::CheckShutdown() noexcept
{
  // See if core dispatcher released all handlers without invoking them.
  if (m_scheduler->m_taskCount != 0 && m_scheduler->m_handlerRefCount == 0)
  {
    m_isShutdown = true;
    m_scheduler->m_taskCount = 0;
    m_scheduler->m_isShutdown = true;
  }
}

void UISchedulerWinRT::CleanupContext::CheckTermination() noexcept
{
  m_isTerminated = m_scheduler->m_isShutdown && (m_scheduler->m_handlerRefCount == 0);
}

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeMainUIScheduler() noexcept
{
  return Mso::Make<UISchedulerWinRT, IDispatchQueueScheduler>(CoreApplication::MainView().CoreWindow().Dispatcher());
}

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeCurrentThreadUIScheduler() noexcept
{
  return Mso::Make<UISchedulerWinRT, IDispatchQueueScheduler>(CoreWindow::GetForCurrentThread().Dispatcher());
}

} // namespace Mso

#endif // __has_include(<base.h>)