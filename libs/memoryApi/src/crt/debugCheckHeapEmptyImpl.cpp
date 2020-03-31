// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  CRT based implementation for Mso::Memory
*/
#include <core/memoryApi.h>
#include <malloc.h>

#ifdef DEBUG

MSOAPI_(BOOL) MsoCheckHeap(void) noexcept
{
  AssertSzTag(false, "MsoCheckHeap is not implemented for this platform", 0x0125310c /* tag_bjtem */);
  return true;
}

#endif // DEBUG
