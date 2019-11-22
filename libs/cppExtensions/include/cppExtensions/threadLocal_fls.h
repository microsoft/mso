// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	FLS-based implementation of TLS
*/
#ifndef _CPPEXTENSIONS_THREADLOCAL_FLS_H_
#define _CPPEXTENSIONS_THREADLOCAL_FLS_H_
#ifdef __cplusplus
#include <CompilerAdapters/CppMacros.h>
#include <CompilerAdapters/FunctionDecorations.h>
#include <platformadapters/windowsfirst.h>
#include <cstdint>

namespace Mso { namespace Details {

struct ThreadLocalImpl
{
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(ThreadLocalImpl);
	static_assert(FLS_OUT_OF_INDEXES == TLS_OUT_OF_INDEXES, "Assumption violation");

	ThreadLocalImpl() noexcept : ThreadLocalImpl(nullptr)
	{
	}

	/**
		Callback will be invoked with the TLS data on each thread exit
	*/
	ThreadLocalImpl(void (__stdcall *pfnCleanup)(void*) noexcept) noexcept
	{
		if (pfnCleanup)
		{
			m_index = ::FlsAlloc(pfnCleanup);
			m_isFls = true;
		}
		else
		{
			m_index = ::TlsAlloc();
		}

		CheckFatal(m_index != TLS_OUT_OF_INDEXES);
	}

	~ThreadLocalImpl() noexcept
	{
		if (m_isFls)
			::FlsFree(m_index);
		else
			::TlsFree(m_index);
	}

	void* Get() const noexcept
	{
		if (m_isFls)
			return ::FlsGetValue(m_index);
		else
			return ::TlsGetValue(m_index);
	}

	void Set(void* t) noexcept
	{
		if (m_isFls)
			CheckFatal(!!::FlsSetValue(m_index, t));
		else
			CheckFatal(!!::TlsSetValue(m_index, t));
   }

private:
	static void CheckFatal(bool result) noexcept
	{
		if (!result)
			__fastfail(FAST_FAIL_FATAL_APP_EXIT);
	}

	uint32_t m_index = TLS_OUT_OF_INDEXES;
	bool m_isFls = false;
};

}} // Mso::Details

#endif // __cplusplus
#endif // _CPPEXTENSIONS_THREADLOCAL_FLS_H_
