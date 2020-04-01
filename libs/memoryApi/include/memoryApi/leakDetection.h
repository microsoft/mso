// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Deprecated APIs related to leak detection
*/
#pragma once
#include <memoryLeakScope/memoryLeakScope.h>

/**
  When this object is active, allocations on this thread are marked as
  needing to be cleaned up by shutdown.
*/
using MsoDebugShutdownLazyScope = Mso::Memory::AutoShutdownLeakScope;

/**
  When this object is active, allocations on this thread are not tracked
  for leak detection. Typically used when the memory lifetime is the life of
  the DLL or is managed by an external dependency.
*/
using MsoDebugLazyScope = Mso::Memory::AutoIgnoreLeakScope;
