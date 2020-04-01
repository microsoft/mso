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
  AssertSzTag(false, "MsoCheckHeap is not implemented for this platform", 0x027c404d /* tag_c5ebn */);
  return true;
}

#endif // DEBUG
