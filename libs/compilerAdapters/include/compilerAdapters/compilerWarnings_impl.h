// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMPILERADAPTERS_COMPILERWARNINGS_IMPL_H
#define MSO_COMPILERADAPTERS_COMPILERWARNINGS_IMPL_H
/**
  Implementation details for suppressing compiler warnings in a cross-plat manner.
  */

#if defined(__clang__)

#define COMPILER_WARNING_PUSH() __pragma(clang diagnostic push)
#define COMPILER_WARNING_DISABLE_ALL(msvcNum, clangWarn, gccWarn) COMPILER_WARNING_DISABLE_CLANG(clangWarn)
#define COMPILER_WARNING_DISABLE_MSVC(msvcNum)
#define COMPILER_WARNING_DISABLE_CLANG(clangWarn) __pragma(clang diagnostic ignored clangWarn)
#define COMPILER_WARNING_DISABLE_GCC(gccWarn)
#define COMPILER_WARNING_POP() __pragma(clang diagnostic pop)

#elif defined(__GNUC__)

#define COMPILER_GCC_PRAGMA(x) _Pragma(#x)
#define COMPILER_WARNING_PUSH() COMPILER_GCC_PRAGMA(GCC diagnostic push)
#define COMPILER_WARNING_DISABLE_ALL(msvcNum, clangWarn, gccWarn) COMPILER_WARNING_DISABLE_GCC(gccWarn)
#define COMPILER_WARNING_DISABLE_MSVC(msvcNum)
#define COMPILER_WARNING_DISABLE_CLANG(clangWarn)
#define COMPILER_WARNING_DISABLE_GCC(gccWarn) COMPILER_GCC_PRAGMA(GCC diagnostic ignored gccWarn)
#define COMPILER_WARNING_POP() COMPILER_GCC_PRAGMA(GCC diagnostic push)

#else

#define COMPILER_WARNING_PUSH() __pragma(warning(push))
#define COMPILER_WARNING_DISABLE_ALL(msvcNum, clangWarn, gccWarn) COMPILER_WARNING_DISABLE_MSVC(msvcNum)
#define COMPILER_WARNING_DISABLE_MSVC(msvcNum) __pragma(warning(disable :##msvcNum))
#define COMPILER_WARNING_DISABLE_CLANG(clangWarn)
#define COMPILER_WARNING_DISABLE_GCC(gccWarn)
#define COMPILER_WARNING_POP() __pragma(warning(pop))

#endif

#define BEGIN_DISABLE_COMPILER_WARNING_ALL(msvcNum, clangWarn, gccWarn) \
  COMPILER_WARNING_PUSH()                                               \
  COMPILER_WARNING_DISABLE_ALL(msvcNum, clangWarn, gccWarn)

#define BEGIN_DISABLE_COMPILER_WARNING_MSVC(msvcNum) \
  COMPILER_WARNING_PUSH()                            \
  COMPILER_WARNING_DISABLE_MSVC(msvcNum)

#define BEGIN_DISABLE_COMPILER_WARNING_CLANG(clangWarn) \
  COMPILER_WARNING_PUSH()                               \
  COMPILER_WARNING_DISABLE_CLANG(clangWarn)

#define BEGIN_DISABLE_COMPILER_WARNING_GCC(gccWarn) \
  COMPILER_WARNING_PUSH()                           \
  COMPILER_WARNING_DISABLE_GCC(gccWarn)

#define END_DISABLE_COMPILER_WARNING() COMPILER_WARNING_POP()

#endif // MSO_COMPILERADAPTERS_COMPILERWARNINGS_IMPL_H
