// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	Things related to linking
*/
#pragma once
#ifndef COMPILERADAPTERS_LINKER_H
#define COMPILERADAPTERS_LINKER_H

/**
	If a library has no direct entry points, it may be necessary to force
	the linker to include it. There are two ways to accomplish this:

	1) Preferably - link the library as a LOB instead.

	2) Use MSO_DEFINE_SYMBOL to define a symbol in the source file that needs to
	be included. Then, add the corresponding MSO_LINK_SYMBOL macro to one of the
	source files that gets compiled directly into your binary (as an *.obj file)
*/
#define MSO_DEFINE_SYMBOL(symbol) extern "C" void __cdecl symbol() {}

// MSO_LINK_INCLUDE and MSO_LINK_SYMBOL macros are only needed for MSVC.
#if MS_TARGET_WINDOWS

/**
	Machinery for the MSO_LINK_SYMBOL macro
*/
#if defined(_M_HYBRID)
#define MSO_LINK_INCLUDE(symbol) "/INCLUDE:#" symbol
#elif defined(_M_IX86)
#define MSO_LINK_INCLUDE(symbol) "/INCLUDE:_" symbol
#elif defined(_M_X64) || defined(_M_ARM)
#define MSO_LINK_INCLUDE(symbol) "/INCLUDE:" symbol
#else
#error Unknown Platform!
#endif

/**
	See MSO_DEFINE_SYMBOL above
*/
#define MSO_LINK_SYMBOL( symbol ) __pragma( comment( linker, MSO_LINK_INCLUDE( #symbol ) ) )

/**
	Platform-agnostic macro to define dll export name ("foo" vs "_foo@4")
	The number is generally [# of params * 4].

	Note: using GetProcAddress is usually wrong. Prefer linker delay-loading.

	Example:
	void Foo(int bar);
	auto pFoo = (decltype(Foo) *) GetProcAddress(hMod, SzDllExport("Foo", 4));
*/
#if defined(_M_X64) || defined(_M_ARM)
#define SzDllExport( fn, c ) fn
#elif defined(_M_IX86)
#define SzDllExport( fn, c ) "_" fn "@" #c
#else
#error Unknown Platform!
#endif // x64 || _M_ARM

#else

#define MSO_LINK_INCLUDE(symbol)
#define MSO_LINK_SYMBOL(symbol)

#endif // MS_TARGET_WINDOWS
#endif
