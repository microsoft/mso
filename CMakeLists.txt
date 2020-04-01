# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

# set minimum cmake version
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# project name and language
project(Mso
  HOMEPAGE_URL "https://github.com/microsoft/Mso"
  LANGUAGES CXX)

# When our project is the root project, we are building stand-alone and can
# can safely manipulate the warning level. Turn on all warnings, and treat
# them as errors. This ensures that when we are imported, we will build
# correctly in even the most restrictive projects.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(REGEX REPLACE "[/-][wW]3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- /W4 /WX")
  elseif(CMAKE_CXX_COMPILER_ID IN_LIST "AppleClang;Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Werror")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Allow specific GCC warnings to remain as warnings so they are visible,
    # but don't fail the build:
    #
    #   noexcept-type : name mangling for noexcept types is changing in C++17
    #     >> Symbolic signature changes don't matter because we don't export binaries
    #
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Werror -Wno-noexcept-type")
  endif()
  message(STATUS "Set CXX flags for standalone build: ${CMAKE_CXX_FLAGS}")
endif()

# include liblet.cmake with liblet-specific functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")
include(liblet)

if(MSO_ENABLE_UNIT_TESTS)
  enable_testing()
endif()

add_subdirectory(external)
add_subdirectory(libs)

liblet_detect_dependency_cycles()