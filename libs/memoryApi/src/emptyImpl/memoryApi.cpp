// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	CRT based implementation for Mso::Memory
*/
#include <core/memoryApi.h>
#include <malloc.h>
#include <memory>

#pragma detect_mismatch("Allocator", "EmptyImpl")

__declspec(noreturn) void ThrowOOM() { throw std::bad_alloc(); }

namespace Mso { namespace Memory { 

MSOCPPAPI_(size_t) AllocationSize(_In_opt_ const void*) noexcept
{
	VerifyElseCrashTag(false, 0x0115e605 /* tag_bf4yf */);
}

MSOCPPAPI_(void*) AllocateEx(size_t /*cb*/, DWORD /*allocFlags*/) noexcept
{
	VerifyElseCrashTag(false, 0x006cc64b /* tag_a1mzl */);
}

MSOCPPAPI_(void*) Reallocate(_Inout_ void** /*ppv*/, size_t /*cb*/) noexcept
{
	VerifyElseCrashTag(false, 0x006cc64c /* tag_a1mzm */);
}

MSOCPPAPI_(void) Free(_In_opt_ void* /*pv*/) noexcept
{
	VerifyElseCrashTag(false, 0x006cc64d /* tag_a1mzn */);
}

#ifdef DEBUG
MSOCPPAPI_(void) RegisterCallback(Mso::LibletAPI::ILibletMemoryMarking&) noexcept { VerifyElseCrashTag(false, 0x006cc64e /* tag_a1mzo */); }
MSOCPPAPI_(void) UnregisterCallback(Mso::LibletAPI::ILibletMemoryMarking&) noexcept { VerifyElseCrashTag(false, 0x006cc64f /* tag_a1mzp */); }
#endif 

}} // Mso::Memory

#ifdef DEBUG

MSOAPI_(BOOL) MsoCheckHeap(void) noexcept
{
	VerifyElseCrashTag(false, 0x0125310d /* tag_bjten */);
	return true;
}

MSOAPI_(void) MsoSetLazyLeakDetection(const void *) noexcept
{
	VerifyElseCrashTag(false, 0x006cc650 /* tag_a1mzq */);
}

MSOAPI_(void) MsoSetShutdownLeakDetection(const void *) noexcept
{
	VerifyElseCrashTag(false, 0x006cc651 /* tag_a1mzr */);
}

MSOAPI_(BOOL) FMemHeapMsoSaveBeHost(void* /*pinst*/, LPARAM /*lParam*/, const void* /*pvBlock*/, LONG_PTR /*cb*/, struct IMsoMemHeap* /*pmmh*/) noexcept
{
	VerifyElseCrashTag(false, 0x006cc652 /* tag_a1mzs */);
}

MSOAPI_(VOID) MsoDebugRegisterLazyObject(IMsoDebugLazyObject* /*pidlo*/) noexcept {}
MSOAPI_(VOID) MsoDebugUnregisterLazyObjectThreaded(IMsoDebugLazyObject* /*pidlo*/, bool /*fMainThread*/) noexcept {}

#endif // DEBUG
