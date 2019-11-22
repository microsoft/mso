// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
CRT based implementation for Mso::Memory on Apple platform
*/
#include <core/memoryApi.h>
#include <malloc.h>

namespace Mso
{
	namespace Memory
	{
		MSOCPPAPI_(size_t) AllocationSize(_In_opt_ const void* pv) noexcept
		{
			if (pv == nullptr)
			return 0;

			return ::malloc_size(const_cast<void *>(pv));
		}
	}
}
