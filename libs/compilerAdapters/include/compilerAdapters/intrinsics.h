// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	Adapters for various compiler intrinsics
*/
#pragma once
#ifndef COMPILERADAPTERS_INTRINSICS_H
#define COMPILERADAPTERS_INTRINSICS_H

/** Macro that calls compiler intrinsics to terminate the application and bypass all exception handlers */
#if defined(__clang__) || defined(__GNUC__)
	#define MSOFASTFAIL(arg) __builtin_trap()
	#define MSO_FUNC_RETURN_ADDRESS() __builtin_return_address(0)
#else
	#include <intrin.h>
	#define MSOFASTFAIL(arg) __fastfail(arg)
	#define MSO_FUNC_RETURN_ADDRESS() _ReturnAddress()
#endif

#endif
