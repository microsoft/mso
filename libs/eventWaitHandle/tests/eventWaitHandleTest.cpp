// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include <eventWaitHandle/eventWaitHandle.h>
#include <motifCpp/libletAwareMemLeakDetection.h>
#include <motifCpp/testCheck.h>
#include <test/skipSEHUT.h>
#include <atomic>
#include <thread>

using namespace std::chrono_literals;

namespace Mso { namespace Async { namespace Test {

TEST_CLASS (EventWaitHandleTest)
{
  // TODO: enable MemoryLeakDetection
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(ManualResetEvent_ctor_default)
  {
    ManualResetEvent ev;
    TestCheck(&ev.GetHandle());
  }

  TEST_METHOD(ManualResetEvent_ctor_Signaling)
  {
    ManualResetEvent ev{EventWaitHandleState::IsSet};
    TestCheck(&ev.GetHandle());
    ev.Wait();
  }

  TEST_METHOD(ManualResetEvent_ctor_NotSignaling)
  {
    ManualResetEvent ev{EventWaitHandleState::NotSet};
    TestCheck(&ev.GetHandle());
    TestCheck(!ev.WaitFor(1ms)); // Must timeout
  }

  TEST_METHOD(ManualResetEvent_ctor_handle)
  {
    ManualResetEvent ev1;
    ManualResetEvent ev2{ev1.GetHandle()};
    TestCheck(&ev1.GetHandle() == &ev2.GetHandle());
  }

  TEST_METHOD(ManualResetEvent_ctor_copy)
  {
    ManualResetEvent ev1;
    ManualResetEvent ev2{ev1}; // Copy
    TestCheck(&ev1.GetHandle() == &ev2.GetHandle());
  }

  TEST_METHOD(ManualResetEvent_assignment_copy)
  {
    ManualResetEvent ev1;
    ManualResetEvent ev2;
    ev2 = ev1; // Copy
    TestCheck(&ev1.GetHandle() == &ev2.GetHandle());
  }

  TEST_METHOD(ManualResetEvent_Set)
  {
    std::atomic<int32_t> value{0};
    ManualResetEvent ev;
    // Note that we capture by value to make a copy of the pointer.
    std::thread th([ev, &value]() noexcept {
      ev.Set();
      ++value;
    });
    ev.Wait();
    th.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(ManualResetEvent_NoAutoReset)
  {
    std::atomic<int32_t> value{0};
    ManualResetEvent ev;
    std::thread th([ev, &value]() noexcept {
      ev.Set();
      ++value;
    });
    ev.Wait();
    ev.Wait(); // Second Wait succeeds because it is not auto-reset.
    th.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(ManualResetEvent_TwoWaitThreadsWakeup)
  {
    ManualResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      ev.Wait();
      ++value;
    });
    std::thread th2([ev, &value]() noexcept {
      ev.Wait();
      ++value;
    });
    ev.Set();
    th1.join();
    th2.join();
    TestCheckEqual(2, value.load());
  }

  TEST_METHOD(ManualResetEvent_Reset)
  {
    ManualResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      ev.Wait();
      ++value;
      ev.Reset();
    });
    ev.Set();
    th1.join();
    TestCheckEqual(1, value.load());
    std::thread th2([ev, &value]() noexcept {
      ev.Wait();
      ++value;
    });
    TestCheckEqual(1, value.load()); // Make sure that th2 waits
    ev.Set();
    th2.join();
    TestCheckEqual(2, value.load());
  }

  TEST_METHOD(ManualResetEvent_WaitFor0)
  {
    ManualResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      TestCheck(!ev.WaitFor(0ms)); // Must always timeout in our case.
      ++value;
    });
    th1.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(ManualResetEvent_WaitFor1)
  {
    ManualResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      TestCheck(!ev.WaitFor(1ms)); // Must always timeout in our case.
      ++value;
    });
    th1.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(ManualResetEvent_WaitFor_Succeed)
  {
    ManualResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      TestCheck(ev.WaitFor(1000s)); // Must succeed because we call Set.
      ++value;
    });
    ev.Set();
    th1.join();
    TestCheckEqual(1, value.load());
  }

  TESTMETHOD_REQUIRES_SEH(ManualResetEvent_WaitFor_CrashForOverflow)
  {
    // TODO: enable MemoryLeakDetection
    // TEST_DISABLE_MEMORY_LEAK_DETECTION();
    ManualResetEvent ev;
    TestCheckCrash(ev.WaitFor(std::chrono::seconds::max()));
  }

  TEST_METHOD(ManualResetEvent_WaitFor_CrashForInfinite)
  {
    // TODO: enable MemoryLeakDetection
    // TEST_DISABLE_MEMORY_LEAK_DETECTION();
    ManualResetEvent ev;
    TestCheckCrash(ev.WaitFor(std::chrono::milliseconds(std::numeric_limits<uint32_t>::max())));
  }

  TEST_METHOD(AutoResetEvent_ctor_default)
  {
    AutoResetEvent ev;
    TestCheck(&ev.GetHandle());
  }

  TEST_METHOD(AutoResetEvent_ctor_Signaling)
  {
    AutoResetEvent ev{EventWaitHandleState::IsSet};
    TestCheck(&ev.GetHandle());
    ev.Wait();
  }

  TEST_METHOD(AutoResetEvent_ctor_NotSignaling)
  {
    AutoResetEvent ev{EventWaitHandleState::NotSet};
    TestCheck(&ev.GetHandle());
    TestCheck(!ev.WaitFor(1ms)); // Must timeout
  }

  TEST_METHOD(AutoResetEvent_ctor_handle)
  {
    AutoResetEvent ev1;
    AutoResetEvent ev2{ev1.GetHandle()};
    TestCheck(&ev1.GetHandle() == &ev2.GetHandle());
  }

  TEST_METHOD(AutoResetEvent_Set)
  {
    std::atomic<int32_t> value{0};
    AutoResetEvent ev;
    // Note that we capture by value to make a copy of the pointer.
    std::thread th([ev, &value]() noexcept {
      ev.Set();
      ++value;
    });
    ev.Wait();
    th.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(AutoResetEvent_AutoReset)
  {
    std::atomic<int32_t> value{0};
    AutoResetEvent ev;
    std::thread th([ev, &value]() noexcept {
      ev.Set();
      ++value;
    });
    ev.Wait();
    TestCheck(!ev.WaitFor(1ms)); // Second Wait fails because it is auto-reset.
    th.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(AutoResetEvent_TwoWaitThreadsWakeup)
  {
    AutoResetEvent ev;
    ManualResetEvent valueChanged;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value, valueChanged]() noexcept {
      ev.Wait();
      ++value;
      valueChanged.Set();
    });
    std::thread th2([ev, &value, valueChanged]() noexcept {
      ev.Wait();
      ++value;
      valueChanged.Set();
    });
    ev.Set();
    valueChanged.Wait();
    TestCheckEqual(1, value.load());
    ev.Set();
    th1.join();
    th2.join();
    TestCheckEqual(2, value.load());
  }

  TEST_METHOD(AutoResetEvent_Reset)
  {
    AutoResetEvent ev;
    std::atomic<int32_t> value{0};
    ev.Set();
    ev.Reset();
    TestCheck(!ev.WaitFor(1ms)); // Make sure that ev is not signaling.
    std::thread th([ev, &value]() noexcept {
      ev.Wait();
      ++value;
    });
    ev.Set();
    th.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(AutoResetEvent_WaitFor0)
  {
    AutoResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      TestCheck(!ev.WaitFor(0ms)); // Must always timeout in our case.
      ++value;
    });
    th1.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(AutoResetEvent_WaitFor1)
  {
    AutoResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      TestCheck(!ev.WaitFor(1ms)); // Must always timeout in our case.
      ++value;
    });
    th1.join();
    TestCheckEqual(1, value.load());
  }

  TEST_METHOD(AutoResetEvent_WaitFor_Succeed)
  {
    AutoResetEvent ev;
    std::atomic<int32_t> value{0};
    std::thread th1([ev, &value]() noexcept {
      TestCheck(ev.WaitFor(1000s)); // Must succeed because we call Set.
      ++value;
    });
    ev.Set();
    th1.join();
    TestCheckEqual(1, value.load());
  }

  TESTMETHOD_REQUIRES_SEH(AutoResetEvent_WaitFor_CrashForOverflow)
  {
    // TODO: enable MemoryLeakDetection
    // TEST_DISABLE_MEMORY_LEAK_DETECTION();
    AutoResetEvent ev;
    TestCheckCrash(ev.WaitFor(std::chrono::seconds::max()));
  }

  TESTMETHOD_REQUIRES_SEH(AutoResetEvent_WaitFor_CrashForInfinite)
  {
    // TODO: enable MemoryLeakDetection
    // TEST_DISABLE_MEMORY_LEAK_DETECTION();
    AutoResetEvent ev;
    TestCheckCrash(ev.WaitFor(std::chrono::milliseconds(std::numeric_limits<uint32_t>::max())));
  }
};

}}} // namespace Mso::Async::Test
