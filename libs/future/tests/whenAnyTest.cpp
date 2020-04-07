// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <thread>
#include "cppExtensions/autoRestore.h"
#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (WhenAnyTest, LibletAwareMemLeakDetection)
{
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  ~WhenAnyTest() noexcept
  {
    Mso::UnitTest_UninitConcurrentQueue();
  }

  TEST_METHOD(WhenAny_Init_Three)
  {
    Mso::ManualResetEvent finished13;
    auto f1 = Mso::PostFuture([finished13]() noexcept {
      finished13.Wait();
      return 1;
    });
    auto f2 = Mso::PostFuture([finished13]() noexcept { return 3; });
    auto f3 = Mso::PostFuture([finished13]() noexcept {
      finished13.Wait();
      return 5;
    });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([](int r) noexcept -> int { return r; });

    TestCheckEqual(3, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Init_Empty)
  {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny({}));
  }

  TEST_METHOD(WhenAny_Init_Three_Error)
  {
    Mso::ManualResetEvent finished13;
    auto f1 = Mso::PostFuture([finished13]() noexcept {
      finished13.Wait();
      return 1;
    });
    auto f2 = Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f3 = Mso::PostFuture([finished13]() noexcept {
      finished13.Wait();
      return 5;
    });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([](Mso::Maybe<int>&& result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Array_Three)
  {
    Mso::ManualResetEvent finished13;
    Mso::Future<int> futures[] = {
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 1;
        }),
        Mso::PostFuture([finished13]() noexcept { return 3; }),
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 5;
        })};

    auto fr = Mso::WhenAny(futures).Then([](int r) noexcept { return r; });

    TestCheckEqual(3, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Array_Three_Error)
  {
    Mso::ManualResetEvent finished13;
    Mso::Future<int> futures[] = {
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 1;
        }),
        Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 5;
        })};

    auto fr = Mso::WhenAny(futures).Then([](Mso::Maybe<int>&& result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Vector_Three)
  {
    Mso::ManualResetEvent finished13;
    auto futures = std::vector<Mso::Future<int>>{
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 1;
        }),
        Mso::PostFuture([finished13]() noexcept { return 3; }),
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 5;
        })};

    auto fr = Mso::WhenAny(futures).Then([](int r) noexcept -> int { return r; });

    TestCheckEqual(3, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Vector_Empty)
  {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny(std::vector<Mso::Future<int>>()));
  }

  TEST_METHOD(WhenAny_Vector_Three_Error)
  {
    Mso::ManualResetEvent finished13;
    auto futures = std::vector<Mso::Future<int>>{
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 1;
        }),
        Mso::MakeFailedFuture<int>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([finished13]() noexcept {
          finished13.Wait();
          return 5;
        })};

    auto fr = Mso::WhenAny(futures).Then([](Mso::Maybe<int>&& result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Init_Void_Three)
  {
    Mso::ManualResetEvent finished13;
    int r2 = 0;
    auto f1 = Mso::PostFuture([finished13]() noexcept { finished13.Wait(); });
    auto f2 = Mso::PostFuture([finished13, &r2]() noexcept { r2 = 3; });
    auto f3 = Mso::PostFuture([finished13]() noexcept { finished13.Wait(); });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([]() noexcept { return 42; });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    TestCheckEqual(3, r2);
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Init_Void_Empty)
  {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny({}));
  }

  TEST_METHOD(WhenAny_Init_Void_Three_Error)
  {
    Mso::ManualResetEvent finished13;

    auto f1 = Mso::PostFuture([finished13]() noexcept { finished13.Wait(); });
    auto f2 = Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true));
    auto f3 = Mso::PostFuture([finished13]() noexcept { finished13.Wait(); });

    auto fr = Mso::WhenAny({f1, f2, f3}).Then([](Mso::Maybe<void>&& result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Array_Void_Three)
  {
    Mso::ManualResetEvent finished13;
    int r2 = 0;

    Mso::Future<void> futures[] = {
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); }),
        Mso::PostFuture([finished13, &r2]() noexcept { r2 = 3; }),
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); })};

    auto fr = Mso::WhenAny(futures).Then([]() noexcept { return 42; });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    TestCheckEqual(3, r2);
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Array_Void_Three_Error)
  {
    Mso::ManualResetEvent finished13;

    Mso::Future<void> futures[] = {
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); }),
        Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); })};

    auto fr = Mso::WhenAny(futures).Then([](Mso::Maybe<void>&& result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Vector_Void_Three)
  {
    Mso::ManualResetEvent finished13;

    int r2 = 0;
    auto futures = std::vector<Mso::Future<void>>{
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); }),
        Mso::PostFuture([finished13, &r2]() noexcept { r2 = 3; }),
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); })};

    auto fr = Mso::WhenAny(futures).Then([]() noexcept {});

    Mso::FutureWait(fr);
    TestCheckEqual(3, r2);
    finished13.Set();
  }

  TEST_METHOD(WhenAny_Vector_Void_Empty)
  {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestCheckCrash(Mso::WhenAny(std::vector<Mso::Future<void>>()));
  }

  TEST_METHOD(WhenAny_Vector_Void_Three_Error)
  {
    Mso::ManualResetEvent finished13;

    auto futures = std::vector<Mso::Future<void>>{
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); }),
        Mso::MakeFailedFuture<void>(Mso::CancellationErrorProvider().MakeErrorCode(true)),
        Mso::PostFuture([finished13]() noexcept { finished13.Wait(); })};

    auto fr = Mso::WhenAny(futures).Then([](Mso::Maybe<void>&& result) noexcept {
      TestCheck(result.IsError());
      TestCheck(Mso::CancellationErrorProvider().IsOwnedErrorCode(result.GetError()));
      return 42;
    });

    TestCheckEqual(42, Mso::FutureWaitAndGetValue(fr));
    finished13.Set();
  }
};

} // namespace FutureTests
