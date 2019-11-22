// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	CRT based implementation for Mso::Memory
*/
#include <core/memoryApi.h>
#include <malloc.h>

namespace Mso
{
	namespace Memory
	{
		MSOCPPAPI_(size_t) AllocationSize(_In_opt_ const void* pv) noexcept
		{
			return malloc_usable_size(pv);
		}
	}
}
