// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
    Various low level macros that are globally useful
*/
#pragma once
#ifndef COMPILERADAPTERS_CPPMACROS_H
#define COMPILERADAPTERS_CPPMACROS_H

#include <compilerAdapters/compilerWarnings.h>

/**
    Broadly used macros
*/
#ifndef FImplies
#define FImplies(a, b) (!(a) || (b))
#endif

#ifndef FBiImplies
#define FBiImplies(a, b) (!(a) == !(b))
#endif

#ifndef RgC
#define RgC(ary) (ary), _countof(ary)
#endif

/**
    Statement - used for control flow macros like Check, IfFailGo
    Make the macro act like a statement.
*/
#define Statement(x)                                                                                                   \
    BEGIN_DISABLE_WARNING_CONDITIONAL_EXPRESSION_IS_CONSTANT()                                                         \
    BEGIN_DISABLE_WARNING_TRUE_CONSTANT_EXPR_IN_AND()                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        x;                                                                                                             \
    } while (0) END_DISABLE_WARNING_TRUE_CONSTANT_EXPR_IN_AND() END_DISABLE_WARNING_CONDITIONAL_EXPRESSION_IS_CONSTANT()

#ifdef __GNUC__
// error: there are no arguments to '__noop' that depend on a template parameter, so a declaration of '__noop' must be
// available [-fpermissive]
#define __noop()
#endif

/**
    Prevent the compiler from automatically providing implementations of various
    class features. Use the macro in your class public: section.
    TODO: probably need to update with new Move functions
*/
#define MSO_NO_COPYCONSTR(C)                                                                                           \
    BEGIN_DISABLE_WARNING_LOCAL_CLASS_FUNC_NOT_DEFINED()                                                               \
    C(const C &) = delete;                                                                                             \
    END_DISABLE_WARNING_LOCAL_CLASS_FUNC_NOT_DEFINED()

#define MSO_NO_ASSIGNMENT(C)                                                                                           \
    BEGIN_DISABLE_WARNING_LOCAL_CLASS_FUNC_NOT_DEFINED()                                                               \
    const C &operator=(const C &) = delete;                                                                            \
    END_DISABLE_WARNING_LOCAL_CLASS_FUNC_NOT_DEFINED()

#define MSO_NO_DEFAULTCONSTR(C)                                                                                        \
    BEGIN_DISABLE_WARNING_LOCAL_CLASS_FUNC_NOT_DEFINED()                                                               \
    explicit C() = delete;                                                                                             \
    END_DISABLE_WARNING_LOCAL_CLASS_FUNC_NOT_DEFINED()

#ifndef DECLARE_COPYCONSTR_AND_ASSIGNMENT
#define DECLARE_COPYCONSTR_AND_ASSIGNMENT(C)                                                                           \
    MSO_NO_COPYCONSTR(C);                                                                                              \
    MSO_NO_ASSIGNMENT(C)
#endif

#if defined(__cplusplus)
#include <type_traits>

/**
    When using an enum class to define a set of bitflags, normal bitflag
    enum operations, such as |, ^, and &, don't work without lots of casts.
    Use this macro to define |, ^, and & for your enum class, where |, ^ and ~
    will return an enum class type, and & will evaluate to true or false.
    The implementation causes error C3281 (global operator cannot have
    managed type in signature) for managed code.
*/
#define ENUM_CLASS_FLAGS_OPERATORS(TEnum)                                                                              \
    constexpr TEnum operator~(TEnum a) noexcept                                                                        \
    {                                                                                                                  \
        return static_cast<TEnum>(~static_cast<std::underlying_type<TEnum>::type>(a));                                 \
    }                                                                                                                  \
    constexpr TEnum operator|(TEnum a, TEnum b) noexcept                                                               \
    {                                                                                                                  \
        return static_cast<TEnum>(static_cast<std::underlying_type<TEnum>::type>(a) |                                  \
                                  static_cast<std::underlying_type<TEnum>::type>(b));                                  \
    }                                                                                                                  \
    constexpr bool operator&(TEnum a, TEnum b) noexcept                                                                \
    {                                                                                                                  \
        return !!(static_cast<std::underlying_type<TEnum>::type>(a) &                                                  \
                  static_cast<std::underlying_type<TEnum>::type>(b));                                                  \
    }                                                                                                                  \
    constexpr TEnum operator^(TEnum a, TEnum b) noexcept                                                               \
    {                                                                                                                  \
        return static_cast<TEnum>(static_cast<std::underlying_type<TEnum>::type>(a) ^                                  \
                                  static_cast<std::underlying_type<TEnum>::type>(b));                                  \
    }                                                                                                                  \
    inline TEnum &operator|=(TEnum &a, TEnum b) noexcept                                                               \
    {                                                                                                                  \
        return reinterpret_cast<TEnum &>(reinterpret_cast<std::underlying_type<TEnum>::type &>(a) |=                   \
                                         static_cast<std::underlying_type<TEnum>::type>(b));                           \
    }                                                                                                                  \
    inline TEnum &operator&=(TEnum &a, TEnum b) noexcept                                                               \
    {                                                                                                                  \
        return reinterpret_cast<TEnum &>(reinterpret_cast<std::underlying_type<TEnum>::type &>(a) &=                   \
                                         static_cast<std::underlying_type<TEnum>::type>(b));                           \
    }                                                                                                                  \
    inline TEnum &operator^=(TEnum &a, TEnum b) noexcept                                                               \
    {                                                                                                                  \
        return reinterpret_cast<TEnum &>(reinterpret_cast<std::underlying_type<TEnum>::type &>(a) ^=                   \
                                         static_cast<std::underlying_type<TEnum>::type>(b));                           \
    }

#else // !__cplusplus

#define ENUM_CLASS_FLAGS_OPERATORS(TEnum)

#endif // !__cplusplus

/**
    Macros to help share enums between C++ and CX.
    Enums must derive from int (the default)
    Flag enums must derive from unsigned int
*/
#if defined(_WINRT_COMP) && !defined(__clang__)
#define EXPOSE_WINRT_ENUM public
#define EXPOSE_WINRT_FLAGS_ENUM [::Platform::Metadata::Flags] EXPOSE_WINRT_ENUM
#else
#define EXPOSE_WINRT_ENUM
#define EXPOSE_WINRT_FLAGS_ENUM
#endif

/**
    Macros to force placement of symbols into a particular segment.
    For example on iOS ensure large globals aren't placed in the text
    segment because there is a size limit enforced by Apple.
 */
#if MS_TARGET_APPLE
#define DATA_SEGMENT_CONST __attribute__((section("__DATA,__const")))
#else
#define DATA_SEGMENT_CONST
#endif

#if __GNUC__
#define __forceinline __attribute__((always_inline, unused))
// Stub __declspec in GCC
#define __declspec(s)
#endif

#endif // COMPILERADAPTERS_CPPMACROS_H
