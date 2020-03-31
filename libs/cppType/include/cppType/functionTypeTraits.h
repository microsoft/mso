// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Various C++ helpers for working with function types
*/
#pragma once
#ifndef LIBLET_CPPTYPE_FUNCTIONTYPETRAITS_H
#define LIBLET_CPPTYPE_FUNCTIONTYPETRAITS_H

#include <tuple>
#include <type_traits>

namespace Mso {

/**
  Turns a function pointer or method pointer type into a normal function type.
  Error otherwise.

  Note: does not strip volatile/const qualifiers on the pointer.

  Note: std::remove_reference<T> is used instead of std::common_type<T>
    because of a bug in the STL that causes common_type to not return the
    identity with function types.
*/
template <typename FunctionPointerType>
struct fnptr_to_fn;

// Handle a member function pointer that is non-const
template <typename Return, typename Class, typename... Arguments>
struct fnptr_to_fn<Return (Class::*)(Arguments...)>
{
  typedef typename std::remove_reference<Return(Arguments...)>::type type;
};

// Handle a pointer to a const member function
template <typename Return, typename Class, typename... Arguments>
struct fnptr_to_fn<Return (Class::*)(Arguments...) const>
{
  typedef typename std::remove_reference<Return(Arguments...)>::type type;
};

// Handle a pointer to a function
template <typename Return, typename... Arguments>
struct fnptr_to_fn<Return (*)(Arguments...)>
{
  typedef typename std::remove_reference<Return(Arguments...)>::type type;
};

#if defined(__cpp_noexcept_function_type) || (_HAS_NOEXCEPT_FUNCTION_TYPES == 1)

// Handle a noexcept member function pointer that is non-const
template <typename Return, typename Class, typename... Arguments>
struct fnptr_to_fn<Return (Class::*)(Arguments...) noexcept>
{
  typedef typename std::remove_reference<Return(Arguments...)>::type type;
};

// Handle a pointer to a const noexcept member function
template <typename Return, typename Class, typename... Arguments>
struct fnptr_to_fn<Return (Class::*)(Arguments...) const noexcept>
{
  typedef typename std::remove_reference<Return(Arguments...)>::type type;
};

// Handle a pointer to a noexcept function
template <typename Return, typename... Arguments>
struct fnptr_to_fn<Return (*)(Arguments...) noexcept>
{
  typedef typename std::remove_reference<Return(Arguments...)>::type type;
};

#endif

/**

  Checks types of two function types to ensure that it is safe to replace the
  first [From] function's invocation with the second [To].

  Guaranteed to work if both From and To are function types.
  Error otherwise.

  i.e.: (struct A {}; struct B : A {};)
    void() -> void() // good
    void(int) -> void(void*) // bad
    void(B*) -> void(A*) // good because a B* can always be converted to an A*
    A*() -> B*() // good because the same reason as the previous
    void(int, double) -> void(int) // bad because parameter count mismatch

  The helper class uses a std::tuple to pack lists of arguments to recurse on.
  This is because recursing using function templates causes and ICE on this version
  of the compiler.  Dev

*/
template <typename From, typename To>
struct are_function_types_convertible;

namespace details {

template <typename FromTuple, typename ToTuple>
struct are_function_types_convertible_arguments_helper;

// base case - true - tuples are empty, no arguments left.
template <>
struct are_function_types_convertible_arguments_helper<std::tuple<>, std::tuple<>>
{
  static bool const value = true;
};

// base case - false - too many arguments in From.
template <typename... FromArguments>
struct are_function_types_convertible_arguments_helper<std::tuple<FromArguments...>, std::tuple<>>
{
  static const bool value = false;
};

// base case - false - too many arguments in To.
template <typename... ToArguments>
struct are_function_types_convertible_arguments_helper<std::tuple<>, std::tuple<ToArguments...>>
{
  static const bool value = false;
};

// recursive step - valid if the head arguments are compatible and the recursive check on the tails are compatible.
template <
    typename FromArgumentsHead,
    typename... FromArgumentsTail,
    typename ToArgumentsHead,
    typename... ToArgumentsTail>
struct are_function_types_convertible_arguments_helper<
    std::tuple<FromArgumentsHead, FromArgumentsTail...>,
    std::tuple<ToArgumentsHead, ToArgumentsTail...>>
{
  static const bool value = std::is_convertible<FromArgumentsHead, ToArgumentsHead>::value
      && are_function_types_convertible_arguments_helper<
                                std::tuple<FromArgumentsTail...>,
                                std::tuple<ToArgumentsTail...>>::value;
};

} // namespace details

// entry point - matches two function types passed in.
template <typename FromReturnType, typename... FromArguments, typename ToReturnType, typename... ToArguments>
struct are_function_types_convertible<FromReturnType(FromArguments...), ToReturnType(ToArguments...)>
{
  static const bool value = std::is_convertible<ToReturnType, FromReturnType>::value
      && details::are_function_types_convertible_arguments_helper<
                                std::tuple<FromArguments...>,
                                std::tuple<ToArguments...>>::value;
};

} // namespace Mso

#endif // LIBLET_CPPTYPE_FUNCTIONTYPETRAITS_H
