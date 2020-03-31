// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Empty implementation for memoryleakscope

  DO NOT INCLUDE THIS HEADER - LINK WITH THE LOB
  (unless you need to link in pass0 e.g. msoprep\legacy)
*/
#include <memoryleakscope/memoryLeakScope.h>

#ifdef DEBUG

namespace Mso { namespace Memory {

bool IsInShutdownLeakScope() noexcept
{
  return false;
}

void EnterShutdownLeakScope(unsigned int /*framesToSkip*/) noexcept {}

void LeaveShutdownLeakScope() noexcept {}

bool IsInIgnoreLeakScope() noexcept
{
  return false;
}

void EnterIgnoreLeakScope(unsigned int /*framesToSkip*/) noexcept {}

void LeaveIgnoreLeakScope() noexcept {}

}} // namespace Mso::Memory

#endif // DEBUG
