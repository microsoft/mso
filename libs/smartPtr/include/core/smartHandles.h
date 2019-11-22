// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	Helper classes for managing handles
*/
#pragma once
#ifndef LIBLET_CORE_SMARTHANDLES_H
#define LIBLET_CORE_SMARTHANDLES_H
#ifdef __cplusplus
#ifdef MS_TARGET_WINDOWS
#include <platformadapters/windowsfirst.h>
#endif
#include <core/smartPointerBase.h>

namespace Mso {

/**
	Manages a HANDLE
*/
struct HandleHelper
	{
	static void Free(HANDLE h) noexcept 
		{ 
		::CloseHandle(h);
		}
	};

/**
	HANDLE holder
*/
typedef THolder<HANDLE, HandleHelper> HandleHolder;

} // Mso

#endif // __cplusplus

#endif // LIBLET_CORE_SMARTHANDLES_H
