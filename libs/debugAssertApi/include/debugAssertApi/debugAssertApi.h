// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  API for debug asserts. Must support C callers
*/
#pragma once
#ifndef LIBLET_DEBUGASSERTAPI_DEBUGASSERTAPI_H
#define LIBLET_DEBUGASSERTAPI_DEBUGASSERTAPI_H
#ifndef RC_INVOKED
#pragma pack(push, 8)

#include <compilerAdapters/compilerWarnings.h>
#include <compilerAdapters/cppMacros.h>
#include <compilerAdapters/functionDecorations.h>
#include <stdint.h>
#include <stdio.h>
#include <tagUtils/tagTypes.h>

/**
Basic debug assert macros to use.

Assert:        evaluate in debug, call the debug handler on failure
AssertDo:		always evaluate, call the debug handler on failure
*/
#define Assert(f) AssertTag(f, UNTAGGED)
#define AssertSz(f, _sz) AssertSzTag(f, _sz, UNTAGGED)
#define AssertSz0(f, _sz) AssertSzTag(f, _sz, UNTAGGED)
#define AssertSz1(f, _sz, a) AssertSz1Tag(f, _sz, a, UNTAGGED)
#define AssertSz2(f, _sz, a, b) AssertSz2Tag(f, _sz, a, b, UNTAGGED)
#define AssertSz3(f, _sz, a, b, c) AssertSz3Tag(f, _sz, a, b, c, UNTAGGED)
#define AssertSz4(f, _sz, a, b, c, d) AssertSz4Tag(f, _sz, a, b, c, d, UNTAGGED)
#define AssertSz5(f, _sz, a, b, c, d, e) AssertSz5Tag(f, _sz, a, b, c, d, e, UNTAGGED)

#define AssertDo(f) AssertDoTag(f, UNTAGGED)
#define AssertDoSz(f, _sz) AssertDoSzTag(f, _sz, UNTAGGED)
#define AssertDoSz1(f, _sz, a) AssertDoSz1Tag(f, _sz, a, UNTAGGED)
#define AssertDoSz2(f, _sz, a, b) AssertDoSz2Tag(f, _sz, a, b, UNTAGGED)
#define AssertDoSz3(f, _sz, a, b, c) AssertDoSz3Tag(f, _sz, a, b, c, UNTAGGED)
#define AssertDoSz4(f, _sz, a, b, c, d) AssertDoSz4Tag(f, _sz, a, b, c, d, UNTAGGED)
#define AssertDoSz5(f, _sz, a, b, c, d, e) AssertDoSz5Tag(f, _sz, a, b, c, d, e, UNTAGGED)

/**
The following macros can be used to assert inside an expression.
Note: Always returns (f), the assert is purely a side-effect.
LATER: add more if needed or ShipAssert versions.

if (FAssertDo(f)) { ... } // same as "if (f) { ... } else Assert(false);"
*/
#define FAssertDo(f) FAssertDoTag(f, UNTAGGED)
#define FAssertDoSz(f, _sz) FAssertDoSzTag(f, _sz, UNTAGGED)
#define FAssertDoSz1(f, _sz, a) FAssertDoSz1Tag(f, _sz, a, UNTAGGED)
#define FAssertDoSz2(f, _sz, a, b) FAssertDoSz2Tag(f, _sz, a, b, UNTAGGED)

#ifdef __cplusplus
#define AssertDetails_SzCast(sz) static_cast<const char*>(sz)
#else
#define AssertDetails_SzCast(sz) (const char*)(sz)
#endif

#define AssertDetails_Statement_Begin                          \
  BEGIN_DISABLE_WARNING_CONDITIONAL_EXPRESSION_IS_CONSTANT()   \
  BEGIN_DISABLE_WARNING_SIGNED_TO_UNSIGNED_CONVERSION()        \
  BEGIN_DISABLE_WARNING_MIXING_SIGNED_AND_UNSIGNED_VARIABLES() \
  BEGIN_DISABLE_WARNING_NONZERO_LOGICAL_AND()                  \
  BEGIN_DISABLE_WARNING_MISSING_BREAK_OR_FALLTHROUGH()         \
  BEGIN_DISABLE_WARNING_TRUE_CONSTANT_EXPR_IN_AND()            \
  BEGIN_DISABLE_WARNING_FALSE_CONSTANT_EXPR_IN_AND()           \
  BEGIN_DISABLE_WARNING_TRUE_CONSTANT_EXPR_IN_OR()             \
  BEGIN_DISABLE_WARNING_IF_CONDITION_IS_ALWAYS_TRUE()          \
  BEGIN_DISABLE_WARNING_IF_CONDITION_IS_ALWAYS_FALSE()         \
  BEGIN_DISABLE_WARNING_FUNCTION_CALLED_TWICE_IN_MACRO()       \
  BEGIN_DISABLE_WARNING_NOTHROW_FUNC_THROWS()                  \
  {
#define AssertDetails_Statement_End                          \
  }                                                          \
  while (0)                                                  \
    END_DISABLE_WARNING_NOTHROW_FUNC_THROWS()                \
  END_DISABLE_WARNING_FUNCTION_CALLED_TWICE_IN_MACRO()       \
  END_DISABLE_WARNING_IF_CONDITION_IS_ALWAYS_FALSE()         \
  END_DISABLE_WARNING_IF_CONDITION_IS_ALWAYS_TRUE()          \
  END_DISABLE_WARNING_TRUE_CONSTANT_EXPR_IN_OR()             \
  END_DISABLE_WARNING_FALSE_CONSTANT_EXPR_IN_AND()           \
  END_DISABLE_WARNING_TRUE_CONSTANT_EXPR_IN_AND()            \
  END_DISABLE_WARNING_MISSING_BREAK_OR_FALLTHROUGH()         \
  END_DISABLE_WARNING_NONZERO_LOGICAL_AND()                  \
  END_DISABLE_WARNING_MIXING_SIGNED_AND_UNSIGNED_VARIABLES() \
  END_DISABLE_WARNING_SIGNED_TO_UNSIGNED_CONVERSION()        \
  END_DISABLE_WARNING_CONDITIONAL_EXPRESSION_IS_CONSTANT()

// NOTE: OACR_ASSUME uses the Assert macro, so oacr.h must be included after Assert is defined
#include <oacr.h>

/**
Return Address
FUTURE: get rid of this and just use void*
*/
typedef struct _MSORADDR
{
  void* pfnCaller;
} MSORADDR;

/**
Ugly workarounds to support C callers, essentially:
C++: pass by reference, use constructor
C  : pass by pointer, use initializer list
*/
#if defined(__cplusplus)
using MsoAssertParamsType = const struct _MsoAssertParams&;
#ifdef DEBUG
#define DeclareMsoAssertParams(...) MsoAssertParams params(__VA_ARGS__)
#else
#define DeclareMsoAssertParams(...)
#endif // DEBUG
#define AccessMsoAssertParams(_p) (&(_p))
#define InlineMsoAssertParams(...) MsoAssertParams(__VA_ARGS__)
#define DeclareInlineMsoAssertParams(_p)
#define PassMsoAssertParams(_p) _p
#else // __cplusplus
typedef struct _MsoAssertParams* MsoAssertParamsType;
#ifdef DEBUG
#define DeclareMsoAssertParams(...)                       \
  (BEGIN_DISABLE_WARNING_NONCONST_AGGREGATE_INITIALIZER() \
       MsoAssertParams params = {__VA_ARGS__} END_DISABLE_WARNING_NONCONST_AGGREGATE_INITIALIZER())
#else
#define DeclareMsoAssertParams(...)
#endif // DEBUG
#define AccessMsoAssertParams(_p) _p
#define InlineMsoAssertParams(szCondition, ...) DeclareMsoAssertParams(__VA_ARGS__)
#define DeclareInlineMsoAssertParamsUnwrap(...) __VA_ARGS__
#define DeclareInlineMsoAssertParams(_p) DeclareInlineMsoAssertParamsUnwrap _p
#define PassMsoAssertParams(_p) &params
#endif // __cplusplus

#ifdef DEBUG
/**
Various parameters that can be passed to the assert functions.
This structure is duplicated in MotifTest project.
*/
typedef struct _MsoAssertParams
{
  uint32_t dwTag;
  const char* szFile;
  uint32_t iLine;
  const char* szTitle;
  const MSORADDR* rgCallstack;
  uint32_t cCallstack;
  const char* szCondition;

  // Skip this many frames if a callstack is generated.
#ifdef __cplusplus
  mutable
#endif // __cplusplus
      uint32_t framesToSkip;

#ifdef __cplusplus
  explicit _MsoAssertParams(
      uint32_t dwTagLocal = UNTAGGED,
      _In_z_ const char* szFileLocal = __FILE__,
      uint32_t iLineLocal = __LINE__,
      _In_opt_z_ const char* szTitleLocal = nullptr,
      _In_opt_count_(cCallstackLocal) const MSORADDR* rgCallstackLocal = nullptr,
      uint32_t cCallstackLocal = 0,
      _In_opt_z_ const char* szConditionLocal = "false") MSONOEXCEPT
      : dwTag(dwTagLocal)
      , szFile(szFileLocal)
      , iLine(iLineLocal)
      , szTitle(szTitleLocal)
      , rgCallstack(rgCallstackLocal)
      , cCallstack(cCallstackLocal)
      , szCondition(szConditionLocal)
      , framesToSkip(0)
  {
  }

  explicit _MsoAssertParams(
      _In_z_ const char* szConditionLocal,
      uint32_t dwTagLocal,
      _In_z_ const char* szFileLocal,
      int32_t iLineLocal) MSONOEXCEPT
      : dwTag(dwTagLocal)
      , szFile(szFileLocal)
      , iLine(static_cast<uint32_t>(iLineLocal))
      , szTitle(nullptr)
      , rgCallstack(nullptr)
      , cCallstack(0)
      , szCondition(szConditionLocal)
      , framesToSkip(0)
  {
  }
#endif // __cplusplus
} MsoAssertParams;

/**
  Function for handling asserts with strings. Return value is AssertResult.
  Note: this function has to be callable from C code
*/
#if defined(__cplusplus)
extern "C"
{
#endif // C++
  LIBLET_PUBLICAPI int32_t
  MsoAssertSzTagProc(MsoAssertParamsType params, _Printf_format_string_ const char* szFmt, va_list argList) MSONOEXCEPT;
#if defined(__cplusplus)
}
#endif // C++

#endif // DEBUG

#include "debugAssertDetails.h"

#ifdef DEBUG
#ifdef __cplusplus
namespace Mso { namespace DebugAsserts {

using AssertIgnorer = bool (*)(const MsoAssertParams& params, const char* szMsg);

/**
  Add an assert ignorer for this process.
  Note: this API is not thread-safe.
*/
LIBLET_PUBLICAPI void AddAssertIgnorer(AssertIgnorer ignorer) noexcept;

/**
  Remove an assert ignorer for this process.
  Note: this API is not thread-safe.
*/
LIBLET_PUBLICAPI void RemoveAssertIgnorer(AssertIgnorer ignorer) noexcept;

struct AutoRegisterAssertIgnorer
{
  AutoRegisterAssertIgnorer(AssertIgnorer ignorer) noexcept : m_ignorer(ignorer)
  {
    AddAssertIgnorer(m_ignorer);
  }

  ~AutoRegisterAssertIgnorer() noexcept
  {
    RemoveAssertIgnorer(m_ignorer);
  }

private:
  AssertIgnorer m_ignorer;
};

using AssertListener = void (*)(const MsoAssertParams& params, const char* szMsg);

/**
  Add an assert listener for this process.
  Note: this API is not thread-safe.
*/
LIBLET_PUBLICAPI void AddAssertListener(AssertListener listener) noexcept;

/**
  Remove a previously registered assert listener for this process.
  Note: this API is not thread-safe.
*/
LIBLET_PUBLICAPI void RemoveAssertListener(AssertListener listener) noexcept;

struct AutoRegisterAssertListener
{
  AutoRegisterAssertListener(AssertListener listener) noexcept : m_listener(listener)
  {
    AddAssertListener(m_listener);
  }

  ~AutoRegisterAssertListener() noexcept
  {
    RemoveAssertListener(m_listener);
  }

private:
  AssertListener m_listener;
};

using AssertHandler = int32_t (*)(const MsoAssertParams& params, const char* szMsg);

/**
  Set the assert handler for this process. The previous handler is returned.
  Note: this API is not thread-safe.
*/
LIBLET_PUBLICAPI AssertHandler SetAssertHandler(AssertHandler handler) noexcept;

/**
  Get the current assert handler for this process.
*/
LIBLET_PUBLICAPI AssertHandler GetAssertHandler() noexcept;

struct AutoRegisterAssertHandler
{
  AutoRegisterAssertHandler(AssertHandler handler) noexcept : m_previous(SetAssertHandler(handler)) {}

  ~AutoRegisterAssertHandler() noexcept
  {
    SetAssertHandler(m_previous);
  }

private:
  AssertHandler m_previous;
};

}} // namespace Mso::DebugAsserts
#endif // __cplusplus
#endif // DEBUG

#pragma pack(pop)
#endif // RC_INVOKED

#endif // LIBLET_DEBUGASSERTAPI_DEBUGASSERTAPI_H
