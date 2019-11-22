// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	CRT based implementation for Mso::Memory
*/
#include <core/memoryApi.h>
#include <platformadapters/windowsfirst.h>
#include <malloc.h>

#ifdef DEBUG

MSOAPI_(BOOL) MsoCheckHeap(void) noexcept
{
	return ::_heapchk() == _HEAPOK;
}

#endif // DEBUG

