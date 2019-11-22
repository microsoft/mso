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
	// passing nullptr checks all zones
	return malloc_zone_check(nullptr);
}

#endif // DEBUG
