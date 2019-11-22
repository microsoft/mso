// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	A collection of functions to help with restricting the behavior of
    C++ templates based on properties of template parameters. Search the
    Interweb for "SFINAE" if you want to understand how this works.
*/
#pragma once
#ifndef LIBLET_CORE_RESTRICTHELPER_H
#define LIBLET_CORE_RESTRICTHELPER_H
#ifdef __cplusplus

#include <type_traits>

/**
 * The `MSO_ENABLE_CLASS_IF` macro allows a partial specialization of a class template to apply only when a certain
 * compile-time constraint is satisfied. Make the generic form of your class have an extra template parameter,
 * defaulting to `void` * (conventionally named "Enable"), and then use this macro in the parameter list of the partial
 * specialization, like so:
 *    template< typename T, typename Enable = void >
 *    class MyClass { ... };
 *
 *    template< typename T >
 *    class MyClass< T, MSO_ENABLE_CLASS_IF( std::is_integral< int >::value ) > { ... };
 *
 * This works via the "SFINAE" technique (go Bing that right now, if you're not familiar with this).
 *
 * @see MSO_ENABLE_FUNC_IF for enabling a function overload based on a compile-time constraint
 */
#define MSO_ENABLE_CLASS_IF( ... ) typename std::enable_if< __VA_ARGS__ >::type


// The Mso::Enabled enumeration is part of the `MSO_ENABLE_FUNC_IF` SFINAE magic.
// @see MSO_ENABLE_FUNC_IF
namespace Mso { enum class Enabled { _ }; }

/**
 * The `MSO_ENABLE_FUNC_IF` macro allows a function template to apply only when a certain compile-time constraint is
 * satisfied. Put this at the end of the template parameter list for the function you wish to constrain, like so:
 *    template< typename T, MSO_ENABLE_FUNC_IF( std::is_integral< T >::value ) >
 *    T Factorial( T t ) { ... }
 *
 * This works via the "SFINAE" technique (go Bing that right now, if you're not familiar with this). It appends a
 * parameter to the template parameter list. If the constraint is satisfied, this extra parameter is of type
 * `Math::Enabled` (an enum), with a default value of `Math::Enabled::_`. If the constraint is unsatisfied,
 * substitution of the `Math::Enabled` type fails, and the function is ignored during overload resolution.
 *
 * The advantage of doing it this way, with an enum rather than a generic `typename` parameter, is that by having the
 * failing substitution happen on the left-hand side of the "=", the compiler will not get confused by two functions
 * with identical signatures, but mutually exclusive constraints. Thus, the following is possible:
 *    template< typename T, MSO_ENABLE_FUNC_IF( std::is_integral< T >::value ) >
 *    void Ahoy( T t ) { std::cout << "Yargh! This here be an integer!\n"; }
 *
 *    template< typename T, MSO_ENABLE_FUNC_IF( std::is_floating_point< T >::value ) >
 *    void Ahoy( T t ) { std::cout << "Avast! It's the cursed SS-754 afloat on the horizon\n"; }
 *
 * If `MSO_ENABLE_FUNC_IF` were instead implemented as `typename Enabled = std::enable_if< constraint >::type`, the
 * compiler would reject the second version of `Ahoy`, thinking that it's a redefinition of the first.
 *
 * Also, note the use of the variadic ... and __VA_ARGS__. This allows commas to appear in the constraint expression
 * without having to be specially escaped by the caller.
 *
 * @see MSO_ENABLE_CLASS_IF for enabling a partial specialization of a class template based on a compile-time constraint
 */
#define MSO_ENABLE_FUNC_IF( ... ) typename std::enable_if< __VA_ARGS__, ::Mso::Enabled >::type = ::Mso::Enabled::_


// Helper macro for constraining function templates to only match certain types via the SFINAE pattern.
// Append this macro as the last item in a function template's template parameter list in order to disable
// that function template when FROM is not implicitly convertible to TO (note that if what you want is to mimic the
// implicit conversion rules for raw pointers, you'll need to specify FROM and TO as pointer types in the macro
// invocation, like: MSO_ENABLE_IF_IMPLICITLY_CONVERTIBLE( X*, Y* )
#define MSO_ENABLE_IF_IMPLICITLY_CONVERTIBLE( FROM, TO ) \
    typename Enable = typename std::enable_if< std::is_convertible< FROM, TO >::value >::type


// Macro for stamping out embedded typedefs for TYPE.
//
// This is primarily intended to be invoked within the body of a "smart pointer" class definition, and used in
// conjunction with the MSO_IMPLICIT_CONVERSION_[FROM|TO] macros, below.
#define MSO_SMART_POINTER_TYPEDEFS( TYPE )  \
    typedef TYPE       ValueType;           \
    typedef ValueType* PointerType;


// Helper macros for constraining smart pointer conversions to mimic builtin ptr implicit conversion rules. Use these
// with constructor, assignment operator, or cast operators that are implemented as function templates, in order to
// cause the compiler to ignore those functions in cases where the corresponding conversion on the underlying pointer
// type is not legal.
//
// In order to use these, first invoke the MSO_SMART_POINTER_TYPEDEFS function within the smart pointer's class body
// in order to create the necessary typedefs. Then, use the appropriate MSO_IMPLICIT_CONVERSION_[FROM|TO] macro within
// your function's template parameter list, like so:
//
//    template< typename MSO_IMPLICIT_CONVERSION_TO( U ) >
//    operator U&() noexcept { return ...; }
//
// Use MSO_IMPLICIT_CONVERSION_TO when implementing a cast operator, and MSO_IMPLICIT_CONVERSION_FROM when implementing
// a converting constructor or assignment operator.
#define MSO_IMPLICIT_CONVERSION_FROM( TYPE ) TYPE, MSO_ENABLE_IF_IMPLICITLY_CONVERTIBLE( TYPE*, PointerType )
#define MSO_IMPLICIT_CONVERSION_TO( TYPE )   TYPE, MSO_ENABLE_IF_IMPLICITLY_CONVERTIBLE( PointerType, TYPE* )


#endif // __cplusplus

#endif // LIBLET_CORE_RESTRICTHELPER_H
