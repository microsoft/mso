// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#ifdef MS_TARGET_WIN

#include <wtypes.h>

#else // !MS_TARGET_WIN

#ifdef MS_TARGET_POSIX

#include <stdint.h>

#ifdef MS_TARGET_APPLE
#include <MacTypes.h>
#endif // MS_TARGET_APPLE

#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char BYTE;//__clang__ : conflict with libc++17 byte definition
#endif // !_BYTE_DEFINED

#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short WORD;
#endif // !_WORD_DEFINED

#ifdef _INT_DEFINED
#define _INT_DEFINED
typedef int INT;
#endif // !_INT_DEFINED

#ifdef _UINT_DEFINED
#define _UINT_DEFINED
typedef unsigned int UINT;
#endif // !_UINT_DEFINED

#ifndef _LONG_DEFINED
#define _LONG_DEFINED
typedef int32_t LONG; //MS_TARGET_APPLE LONG as a Windows type must stay 32-bit even in 64-bit code
#endif // !_LONG_DEFINED

#ifndef _ULONG_DEFINED
#define _ULONG_DEFINED
typedef uint32_t ULONG;
#endif // !_ULONG_DEFINED

#ifndef _BOOL_DEFINED
#define _BOOL_DEFINED
typedef int32_t BOOL; //MS_TARGET_APPLE
#define TRUE 1
#define FALSE 0
#endif // !_BOOL_DEFINED

#ifndef _LONG_PTR_DEFINED
#define _LONG_PTR_DEFINED
typedef intptr_t LONG_PTR;
#endif // !_LONG_PTR_DEFINED

#ifndef _UINT_PTR_DEFINED
#define _UINT_PTR_DEFINED
typedef uintptr_t UINT_PTR;
#endif // !_UINT_PTR_DEFINED

#ifndef _WPARAM_DEFINED
#define _WPARAM_DEFINED
typedef UINT_PTR WPARAM;
#endif // _WPARAM_DEFINED

#ifndef _DWORD_DEFINED
#define _DWORD_DEFINED
typedef uint32_t DWORD; //MS_TARGET_APPLE DWORD as a Windows type must stay 32-bit even in 64-bit code
#endif // !_DWORD_DEFINED

#ifndef _LPARAM_DEFINED
#define _LPARAM_DEFINED
typedef LONG_PTR LPARAM;
#endif // !_LPARAM_DEFINED

#ifndef _LRESULT_DEFINED
#define _LRESULT_DEFINED
typedef LONG_PTR LRESULT;
#endif // !_LRESULT_DEFINED

#endif // MS_TARGET_POSIX

#endif // MS_TARGET_WIN
