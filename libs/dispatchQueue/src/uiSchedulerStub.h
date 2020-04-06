// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

// This a stubbed implementation of UI scheduler.
// We use to compile code in cases when we cannot resolve dependencies.
// It could be considered as a temporary solution to be removed when
// we figure out how to provide all required dependencies.

#include "queueService.h"

#include <crash/verifyElseCrash.h>
#include <dispatchQueue/dispatchQueue.h>

namespace Mso {

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeMainUIScheduler() noexcept
{
  VerifyElseCrashSz(false, "Not implemented");
}

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeCurrentThreadUIScheduler() noexcept
{
  VerifyElseCrashSz(false, "Not implemented");
}

} // namespace Mso